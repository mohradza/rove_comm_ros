#ifndef RoveCommEthernet_h
#define RoveCommEthernet_h

#include <stdint.h>
#include <stddef.h>
#include <Energia.h>
#include <Ethernet.h>

#include "RoveCommManifest.h"
#include "RoveCommPacket.h"
#include "../RoveCommEthernetTCP/RoveCommEthernetTCPServer.h"
#include "../RoveCommEthernetTCP/RoveCommEthernetTCPClient.h"
#include "../RoveCommEthernetUDP/RoveCommEthernetUdp.h"

#define MAX_NUM_TCP_CLIENTS 4

struct RoveCommClient 
{
    RoveCommEthernetTCPClient client;
    IPAddress dest_ip;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class RoveCommEthernet
{
  private:
    uint8_t num_clients = 0;
    
  public:
    RoveCommEthernetTCPServer TCPServer;
    RoveCommClient Clients[MAX_NUM_TCP_CLIENTS];
    RoveCommEthernetUdp UDP;

    struct rovecomm_packet rovecomm_packet;

    /////begin/////////////////////////////////////////////////////////////////////////////////////////////////////////
    //checks all ongoing connections for incoming packets and returns the first one as a parsed rovecomm packet
    struct rovecomm_packet read();

    /////begin/////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Overloaded begin
	//Default ip address = 192.168.1.XXX
    //This TCP server will be configured with an IP and port from the RoveComm manifest and allow other boards and base-station
    //to securely communicate with it
    void begin(const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const int port);
    void begin(const uint8_t ip_octet_4, const int port);

    /////connect///////////////////////////////////////////////////////////////////////////////////////////////////////
    //Allows for creating a new connection to a target IP
    //checks for whether we have already connected to the dest_ip and port, or otherwise grabs an available ethernet
    //client and connects it to the dest_ip and port
    void connect(const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const int port);

    /////connected/////////////////////////////////////////////////////////////////////////////////////////////////////
    //Checks if there is an established and ongoing connection to the target IP
    bool connected(const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4);

    /////write////////////////////////////////////////////////////////////////////////
	//Single-value write
	//Overloaded for each data type
    //Causes bug when doing:RoveComm.write(SINGLE_VALUE_EXAMPLE_ID, 1, analogRead(A0));
	//void write(const uint16_t data_id, const int     data_count, const int      data);
    void write(const uint16_t data_id, const uint8_t data_count, const uint8_t  data);
    void write(const uint16_t data_id, const uint8_t data_count, const uint16_t data);
    void write(const uint16_t data_id, const uint8_t data_count, const uint32_t data);
    void write(const uint16_t data_id, const uint8_t data_count, const int8_t   data);
    void write(const uint16_t data_id, const uint8_t data_count, const int16_t  data);
    void write(const uint16_t data_id, const uint8_t data_count, const int32_t  data);
    void write(const uint16_t data_id, const uint8_t data_count, const float    data);


    //Array entry write
	//Overloaded for each data type
    void write(const uint16_t data_id, const int     data_count, const int      *data);
    void write(const uint16_t data_id, const uint8_t data_count, const uint8_t  *data);
    void write(const uint16_t data_id, const uint8_t data_count, const uint16_t *data);
    void write(const uint16_t data_id, const uint8_t data_count, const uint32_t *data);
    void write(const uint16_t data_id, const uint8_t data_count, const int8_t   *data);
    void write(const uint16_t data_id, const uint8_t data_count, const int16_t  *data);
    void write(const uint16_t data_id, const uint8_t data_count, const int32_t  *data);
    void write(const uint16_t data_id, const uint8_t data_count, const float    *data);


	/////writeTo///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Single-value writeTo
	//Overloaded for each data type
    void writeTo(const uint16_t data_id,    const uint8_t data_count, const int  data,
                 const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const uint16_t port);

    void writeTo(const uint16_t data_id,    const uint8_t data_count, const uint8_t  data,
                 const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const uint16_t port);

    void writeTo(const uint16_t data_id,    const uint8_t data_count, const uint16_t data, 
                 const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const uint16_t port);

    void writeTo(const uint16_t data_id,    const uint8_t data_count, const uint32_t data, 
                 const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const uint16_t port);

    void writeTo(const uint16_t data_id,    const uint8_t data_count, const int8_t   data, 
                 const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const uint16_t port);

    void writeTo(const uint16_t data_id,    const uint8_t data_count, const int16_t  data, 
                 const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const uint16_t port);

    void writeTo(const uint16_t data_id,    const uint8_t data_count, const int32_t  data, 
                 const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const uint16_t port);

    void writeTo(const uint16_t data_id,    const uint8_t data_count, const float  data, 
                 const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const uint16_t port);

    //Array entry write
	//Overloaded for each data type
    void writeTo(const uint16_t data_id,    const uint8_t data_count, const int  *data,
                 const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const uint16_t port);

    void writeTo(const uint16_t data_id,    const uint8_t data_count, const uint8_t  *data,
                 const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const uint16_t port);

    void writeTo(const uint16_t data_id,    const uint8_t data_count, const uint16_t *data, 
                 const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const uint16_t port);

    void writeTo(const uint16_t data_id,    const uint8_t data_count, const uint32_t *data, 
                 const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const uint16_t port);

    void writeTo(const uint16_t data_id,    const uint8_t data_count, const int8_t   *data, 
                 const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const uint16_t port);

    void writeTo(const uint16_t data_id,    const uint8_t data_count, const int16_t  *data, 
                 const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const uint16_t port);

    void writeTo(const uint16_t data_id,    const uint8_t data_count, const int32_t  *data, 
                 const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const uint16_t port);
    
    void writeTo(const uint16_t data_id,    const uint8_t data_count, const float  *data, 
                 const uint8_t  ip_octet_1, const uint8_t ip_octet_2, const uint8_t ip_octet_3, const uint8_t ip_octet_4, const uint16_t port);
	
    /////writeReliable/////////////////////////////////////////////////////////////////////////////////////////////////
	//Single-value writeReliable which ensures delivery
	//Overloaded for each data type
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const uint8_t  data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const uint16_t data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const uint32_t data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const int8_t   data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const int16_t  data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const int32_t  data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const float    data);

    //Array entry writeReliable which ensures delivery
	//Overloaded for each data type
    void writeReliable(const uint16_t data_id, const int     data_count, const int      *data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const uint8_t  *data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const uint16_t *data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const uint32_t *data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const int8_t   *data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const int16_t  *data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const int32_t  *data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const float    *data);
};

#endif // RoveCommEthernet_h