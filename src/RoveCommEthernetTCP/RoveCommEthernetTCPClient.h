#ifndef RoveCommEthernetTCPClient_h
#define RoveCommEthernetTCPClient_h

#include <stdint.h>
#include <stddef.h>
#include <Energia.h>
#include <Ethernet.h>

#include "RoveCommManifest.h"
#include "RoveCommPacket.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class RoveCommEthernetTCPClient
{
  public:
    EthernetClient Client;
    
    struct rovecomm_packet read();

    /////begin/////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Overloaded begin
	  //Default ip address = 192.168.1.XXX
    //This will create a secure connection between this board and the target ip
    void begin(uint8_t client_ip_octet, uint8_t dest_ip_octet, const int port);
    void begin(byte client_ip[4], byte dest_ip[4], const int port);
    //Used for when the board has already been assigned an IP, and a new connection needs to be established
    void begin(IPAddress dest_ip, const int port);
    void begin(uint8_t dest_ip_fourth_octet, const int port);


    /////available/////////////////////////////////////////////////////////////////////////////////////////////////////////
    //returns whether or not there is any data available for reading from the client
    bool available();

    /////connected/////////////////////////////////////////////////////////////////////////////////////////////////////////
    //returns whether or not the client is still connected
    bool connected();
    
	  /////writeReliable////////////////////////////////////////////////////////////////////////
	  //Single-value writeReliable
	  //Overloaded for each data type
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const uint8_t  data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const uint16_t data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const uint32_t data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const int8_t   data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const int16_t  data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const int32_t  data);
    void writeReliable(const uint16_t data_id, const uint8_t data_count, const float  data);


    //Array entry writeReliable
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

#endif // RoveCommEthernetTCPClient_h