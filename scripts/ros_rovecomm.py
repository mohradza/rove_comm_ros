#!/usr/bin/python2

import rospy

import sys, logging, os, socket, time
from optparse import OptionParser
import select
import threading
from threading import Condition, RLock
import numpy as np

import fcntl
import struct
import logging
from errno import EINTR

import StringIO
import io
import pdb
import time

from RoveCommManifest import *
from geometry_msgs.msg import *

from enum import Enum
from Queue import Queue



class RoveComm(object):
    ROVECOMM_VERSION       = 2
    ROVECOMM_HEADER_FORMAT = ">Bhh"
    ROVECOMM_FLAGS  = 0x00
    ROVECOMM_SUBSCRIBE_REQUEST   = 3
    ROVECOMM_UNSUBSCRIBE_REQUEST = 4

    def __init__(self, sock):
        self.sock = sock
        self.seq_num = 1 #?

    def subscribe(self, remoteIP, remotePort):
        data_id = RoveComm.ROVECOMM_SUBSCRIBE_REQUEST
        data = ''
        self.publishRaw(remoteIP, remotePort, data_id, data)

    def publishRaw(self, remoteIP, remotePort, data_id, data):
        #Python3 requires bytes, Python2 uses str as a socket datatype
        print(len(data))
	rovecomm_packet = struct.pack(RoveComm.ROVECOMM_HEADER_FORMAT, RoveComm.ROVECOMM_VERSION, data_id, 514) + data

	self.sock.sendto(rovecomm_packet, (remoteIP, remotePort))
	# self.sock.sendto(rovecomm_packet, ('192.168.1.134', remotePort))
        self.seq_num += 1

    def publishCmdVel(self, remoteIP, remotePort, enable_joy_control, msg):
        #Data ID for a CMD_VEL:
        data_id = RC_DRIVEBOARD_DRIVELEFTRIGHT_DATAID
        print(data_id)
        #Needs to be a 2-byte bytebuffer:

        #Todo: Translate lin/ang vels to left / right, mapped between [-1000,1000]
        #Todo: Translate lin/ang vels to left / right, mapped between [-1000,1000)
        if(enable_joy_control):
            fwd_speed = msg.twist.linear.x
            angular_speed = msg.twist.angular.z

            self.left_vel_out = 0
            self.left_vel_out = 0

            left_vel = fwd_speed*1000
            right_vel = fwd_speed*1000
        else:
            left_vel = 100
            right_vel = 100 #Are they switched in the DriveBoard so that fwd means fwd?
        left_vel = 50
        right_vel = 50 #Are they switched in the DriveBoard so that fwd means fwd?

        buf = struct.pack('>hh', left_vel, right_vel)
        self.publishRaw(remoteIP, remotePort, data_id, buf)

    def sat_cmds(self, left_vel, right_vel):
        if(left > RC_DRIVEBOARD_DRIVEMOTORS_DRIVEMAXFORWARD):
            self.left_vel_out = RC_DRIVEBOARD_DRIVEMOTORS_DRIVEMAXFORWARD
        elif(left_vel < RC_DRIVEBOARD_DRIVEMOTORS_DRIVEMAXREVERSE):
            self.left_vel_out = RC_DRIVEBOARD_DRIVEMOTORS_DRIVEMAXREVERSE
        else:
            self.left_vel_out = int(left_vel)

        if(right_vel > RC_DRIVEBOARD_DRIVEMOTORS_DRIVEMAXFORWARD):
            self.right_vel_out = RC_DRIVEBOARD_DRIVEMOTORS_DRIVEMAXFORWARD
        elif(right_vel < RC_DRIVEBOARD_DRIVEMOTORS_DRIVEMAXREVERSE):
            self.right_vel_out = RC_DRIVEBOARD_DRIVEMOTORS_DRIVEMAXREVERSE
        else:
            self.right_vel_out = int(right_vel)

class BridgeCmds(Enum):
    SET_CMDVEL = 1

#BridgeCmd is more complicated than just a command and an arg to support Java sync primitives for service calls

class BridgeCmd(object):
    def __init__(self, cmdType, arg=None):
        self.cmdType = cmdType
        self.arg = arg
        self.readyBell = Condition()
        self.response = None
    def wait(self):
        self.readyBell.acquire()
        self.readyBell.wait()
        self.readyBell.release()
    def notify(self):
        self.readyBell.acquire()
        self.readyBell.notify()
        self.readyBell.release()

class UDPRoveComm(object):
    DEFAULT_PORT = RC_ROVECOMM_ETHERNET_UDP_PORT
    SOCK_TIMEOUT = 0.01 #floating point number in seconds
    MAX_BLKSIZE = 1400

    def __init__(self):
        rospy.init_node('udprovecomm', anonymous=True)
        self.port = rospy.get_param('~port', self.DEFAULT_PORT)

        self.hostname =  rospy.get_param('~hostname', socket.gethostname())
        if not rospy.has_param('~dest_host'):
            rospy.loginfo('Must provide a comm destination in parameter dest_host')
            sys.exit(-1)

        self.dest_host =  socket.gethostbyname(rospy.get_param('~dest_host'))
        # print(self.dest_host)
        self.cmdQ = Queue() #for dealing with async ROS callbacks

        self.openPort()
        self.rover = RoveComm(self.sock)

        self.cmd_sub = rospy.Subscriber('cmd_vel', Twist, self.onCmdVelPresent)

        #Subscribe to telemetry from the DriveBoard:
        self.rover.subscribe(self.dest_host, self.port)

        self.enable_joy_control = rospy.get_param('~enable_joy_control', False)

    def openPort(self):
        listenip = '0.0.0.0'
        rospy.loginfo("Server requested on ip %s, port %s" % (listenip, self.port))
        try:
            # Written for nonblocking sockets
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.sock.bind((listenip, self.port))
            _, self.listenport = self.sock.getsockname()
        except socket.error as err:
            # Reraise it for now.
            raise err
        return

    def checkSockets(self):
        # Build the inputlist array of sockets to select() on - just the main one for now
        inputlist = [] #our FD_SET equivalent
        inputlist.append(self.sock)

        #rospy.loginfo("Performing select on this inputlist: %s", inputlist)
        try:
            readyinput, readyoutput, readyspecial = select.select(inputlist, [], [], self.SOCK_TIMEOUT)
        except select.error as err:
            if err[0] == EINTR:
                # Interrupted system call
                rospy.loginfo("Interrupted syscall, retrying")
                return
            else:
                raise
        for readysock in readyinput:
            # Is the traffic on the main server socket?
            if readysock == self.sock:
                #rospy.loginfo("Data ready on our  socket")
                buffer, (raddress, rport) = self.sock.recvfrom(self.MAX_BLKSIZE)
                #rospy.loginfo("Read %d bytes", len(buffer))

                #Ignore the loopbacks
                if raddress != self.socket_ip:
                    self.handlePacket(raddress, rport, buffer)

    def handlePacket(self, src, srcPort, pkt):
        if len(pkt) > 0:
            #Treat this as a rovecomm packet
            rospy.loginfo('Got inbound packet from %s' % src)
            handleRoveComm(src, srcPort, pkt)

    def handleRoveComm(self, remote_ip, srcPort, packet):
	header_size = struct.calcsize(RoveComm.ROVECOMM_HEADER_FORMAT)
	rovecomm_version, seq_num, flags, data_id, data_size = struct.unpack(RoveComm.ROVECOMM_HEADER_FORMAT, packet[0:header_size])
	data = packet[header_size:]
        #Do something with the telemetry that is received - publish some status message?

    def onCmdVelPresent(self, msg):
        #Inbound cmd_vel, post to the message q for handling
        self.cmdQ.put(BridgeCmd(BridgeCmds.SET_CMDVEL, msg))

    def handleCmdVel(self, msg):
        #Given a Twist msg, post to the UDP destination following rovecomm semantics
        self.rover.publishCmdVel(self.dest_host, self.port, self.enable_joy_control, msg)


    def runCommand(self, cmd):
        if cmd.cmdType == BridgeCmds.SET_CMDVEL:
            rospy.loginfo('Got vel cmd, posting to UDP socket')
            self.handleCmdVel(cmd.arg)
        else:
            rospy.loginfo('Unknown cmd: %s', cmd.cmdType)

    def run(self):
        #Main loop of the node
        while not rospy.is_shutdown():
            #Command q is still necessary so that a socket is only ever touched by one thread, the main thread
            #ROS callbacks are on separate threads and will corrupt state eventually
            if not self.cmdQ.empty():
                self.runCommand(self.cmdQ.get())

            #Check for data ready on a socket to us
            self.checkSockets()

        #rospy shutting down, avoid FIN_WAIT errors
        if self.sock:
            self.sock.close()


if __name__ == '__main__':
    try:
        client = UDPRoveComm()
        #cProfile.run('client.run()')
        client.run()
    except rospy.ROSInterruptException:
        pass
