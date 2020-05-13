#ifndef RoveCommEthernetTCPServer_h
#define RoveCommEthernetTCPServer_h

#include <stdint.h>
#include <stddef.h>
#include <Energia.h>
#include <Ethernet.h>

#include "RoveCommManifest.h"
#include "RoveCommPacket.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class RoveCommEthernetTCPServer
{
  public:
    EthernetServer Server = EthernetServer(99);
    struct rovecomm_packet read();

    RoveCommEthernetTCPServer();

    /////begin/////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Overloaded begin
	  //Default ip address = 192.168.1.XXX
    //This TCP server will be configured with an IP and port from the RoveComm manifest and allow other boards and base-station
    //to securely communicate with it
	  void begin(uint8_t server_ip_octet, const int port);
    void begin(byte server_ip[4], const int port);
    //used when we have already set up the boards IP, and a new 
    void begin(const int port);

    /////available/////////////////////////////////////////////////////////////////////////////////////////////////////////
    //returns whether or not there is any data available for reading from the server
    bool available();

	  /////writeReliable////////////////////////////////////////////////////////////////////////
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

  private:
    //Called by overloaded writeReliable functions
    void _writeReliable(  const uint8_t  data_type_length, const roveware::data_type_t data_type, 
                          const uint16_t data_id,    const uint8_t data_count, const void* data);
};

#endif // RoveCommEthernetTCP_h