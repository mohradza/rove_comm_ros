#include "RoveCommEthernetTCPServer.h"
#include "RoveCommPacket.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
#include          <SPI.h>         // Energia/master/hardware/lm4f/libraries/SPI
#include          <Energia.h>
#include          <Ethernet.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RoveCommEthernetTCPServer::RoveCommEthernetTCPServer()
{
  return;
}

void RoveCommEthernetTCPServer::begin(uint8_t server_ip_octet, const int port)
{
  byte server_ip[4] = {192, 168, 1, server_ip_octet};
  this->begin(server_ip, port);
}

void RoveCommEthernetTCPServer::begin(byte server_ip[4], const int port)
{ 
  //Set IP
  Ethernet.enableActivityLed();
  Ethernet.enableLinkLed(); 

  //Set up Ethernet
  Ethernet.begin(   0, server_ip);

  //Set up server with correct port, and start listening for clients
  Server = EthernetServer(port);
  Server.begin();

  return;
}

void RoveCommEthernetTCPServer::begin(const int port)
{
  //Set up server with correct port, and start listening for clients
  Server = EthernetServer(port);
  Server.begin();


  delay(10);
  Serial.println("Started server");
  return;
}
/////////////////////////////////////////////////////////////////////////////////
bool RoveCommEthernetTCPServer::available()
{
  //check if there is a message from client
  EthernetClient Client = Server.available();
  //if there is a message from the client, return true
  if(Client && Client.peek() != -1)
  {
    return true;
  }
  return false; 
}

/////////////////////////////////////////////////////////////////////////////////
struct rovecomm_packet RoveCommEthernetTCPServer::read() 
{ 
  //Create new RoveCommPacket
  rovecomm_packet packet = { 0 };
  //check if there is a message from client
  Serial.println("Checked for clients");
  EthernetClient client = Server.available();
  delay(10);
  Serial.println("Checked for clients");
  //if there is a message from the client and there is something to read
  if(client && client.peek() != -1)
    {
      Serial.println("There is a message");
      packet = roveware::unpackPacket(client); 
    }
  //if there is no message, just return that there is no data to read
  else
    {
    packet.data_id = ROVECOMM_NO_DATA_DATA_ID;
    packet.data_count = 1;
    packet.data[1] = {0};
    }
  
	
  //return the packet
  return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RoveCommEthernetTCPServer::_writeReliable(const uint8_t data_type_length, const roveware::data_type_t data_type, const uint16_t data_id, const uint8_t data_count, const void* data)
{ 
  //Creat packed udp packet
  struct roveware::_packet _packet = roveware::packPacket(data_id, data_count, data_type, data);
  
  //write to all available clients
  Server.write( _packet.bytes, (ROVECOMM_PACKET_HEADER_SIZE + (data_type_length * data_count))); 
}

//Overloaded writeReliable////////////////////////////////////////////////////////////////////////////////////////////////////
//Single-value write
//handles the data->pointer conversion for user
void RoveCommEthernetTCPServer::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const  int32_t data )
{                  int32_t data_p[1];
                   data_p[0] = data;
                   this->_writeReliable( 4,  roveware::INT32_T, data_id,               data_count,        (void*) data_p ); }

void RoveCommEthernetTCPServer::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const uint32_t data )
{                  uint32_t data_p[1];
                   data_p[0] = data;
                   this->_writeReliable( 4, roveware::UINT32_T, data_id,               data_count,        (void*) data_p ); }

void RoveCommEthernetTCPServer::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const  int16_t data )
{                  int16_t data_p[1];
                   data_p[0] = data;
                   this->_writeReliable( 2,  roveware::INT16_T, data_id,               data_count,        (void*) data_p ); }

void RoveCommEthernetTCPServer::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const uint16_t data )
{                  uint16_t data_p[1];
                   data_p[0] = data;
                   this->_writeReliable( 2, roveware::UINT16_T, data_id,               data_count,        (void*) data_p ); }

void RoveCommEthernetTCPServer::writeReliable(         const uint16_t data_id, const uint8_t data_count, const   int8_t data )
{                  int8_t data_p[1];
                   data_p[0] = data;
                   this->_writeReliable( 1,   roveware::INT8_T, data_id,               data_count,        (void*) data_p ); }

void RoveCommEthernetTCPServer::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const  uint8_t data )
{                  uint8_t data_p[1];
                   data_p[0] = data;
                   this->_writeReliable( 1,  roveware::UINT8_T, data_id,               data_count,        (void*) data_p ); }

void RoveCommEthernetTCPServer::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const  float data )
{                  float data_p[1];
                   data_p[0] = data;
                   this->_writeReliable( 4,  roveware::FLOAT, data_id,               data_count,        (void*) data_p ); }
//Array-Entry write///////////////////////////////////
void RoveCommEthernetTCPServer::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const  int32_t *data )
{                  this->_writeReliable( 4,  roveware::INT32_T, data_id,               data_count,        (void*) data ); }

void RoveCommEthernetTCPServer::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const uint32_t *data )
{                  this->_writeReliable( 4, roveware::UINT32_T, data_id,               data_count,        (void*) data ); }

void RoveCommEthernetTCPServer::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const  int16_t *data )
{                  this->_writeReliable( 2,  roveware::INT16_T, data_id,               data_count,        (void*) data ); }

void RoveCommEthernetTCPServer::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const uint16_t *data )
{                  this->_writeReliable( 2, roveware::UINT16_T, data_id,               data_count,        (void*) data ); }

void RoveCommEthernetTCPServer::writeReliable(         const uint16_t data_id, const uint8_t data_count, const   int8_t *data )
{                  this->_writeReliable( 1,   roveware::INT8_T, data_id,               data_count,        (void*) data ); }

void RoveCommEthernetTCPServer::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const  uint8_t *data )
{                  this->_writeReliable( 1,  roveware::UINT8_T, data_id,               data_count,        (void*) data ); }

void RoveCommEthernetTCPServer::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const  float *data )
{                  this->_writeReliable( 4,  roveware::FLOAT, data_id,               data_count,        (void*) data ); }