#include "RoveCommEthernetTCPClient.h"
#include "RoveCommPacket.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
#include          <SPI.h>         // Energia/master/hardware/lm4f/libraries/SPI
#include          <Energia.h>
#include          <Ethernet.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RoveCommEthernetTCPClient::begin(uint8_t client_ip_octet, uint8_t dest_ip_octet, const int port)
{
  byte server_ip[4] = {192, 168, 1, client_ip_octet};
  byte dest_ip[4] = {192, 168, 1, dest_ip_octet};
  this->begin(server_ip, dest_ip, port);
}

void RoveCommEthernetTCPClient::begin(byte client_ip[4], byte dest_ip[4], const int port)
{ 
  //Set IP
  Ethernet.enableActivityLed();
  Ethernet.enableLinkLed(); 

  //Set up Ethernet
  Ethernet.begin(   0, client_ip);

  //Attempt a secure connection to the target IP
  if(Client.connect(dest_ip, port))
  {
    Serial.println("Connected");
  }
  else
  {
    Serial.println("Connection Failed");
  }
  
  delay(1);
}

void RoveCommEthernetTCPClient::begin(uint8_t dest_ip, const int port)
{
  IPAddress destination(192, 168, 1, dest_ip);
  this->begin(destination, port);
}

void RoveCommEthernetTCPClient::begin(IPAddress dest_ip, const int port)
{
  //Attempt a secure connection to the target IP 
  if(Client.connect(dest_ip, port))
  {
    Serial.println("Connected");
  }
  else
  {
    Serial.println("Connection Failed");
  }
  return;
}

/////////////////////////////////////////////////////////////////////////////////
bool RoveCommEthernetTCPClient::connected()
{
  return Client.connected();
}

/////////////////////////////////////////////////////////////////////////////////
bool RoveCommEthernetTCPClient::available()
{
  return (Client.available() && Client.peek() != -1);
}

/////////////////////////////////////////////////////////////////////////////////
struct rovecomm_packet RoveCommEthernetTCPClient::read() 
{ 
  //Create new RoveCommPacket
  struct rovecomm_packet rovecomm_packet = { 0 };

  //if there is a message from the client, parse it
  if(Client.available() && Client.peek() != -1)
    {
    rovecomm_packet = roveware::unpackPacket(Client); 
    }
  //if there is no message, just return that there is no data to read
  else
    {
    rovecomm_packet.data_id = ROVECOMM_NO_DATA_DATA_ID;
    rovecomm_packet.data_count = 1;
    rovecomm_packet.data[1] = {0};
    }
  
  //return the packet
  return rovecomm_packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RoveCommEthernetTCPClient::_writeReliable(const uint8_t data_type_length, const roveware::data_type_t data_type, const uint16_t data_id, const uint8_t data_count, const void* data)
{ 
  //Creat packed udp packet
  struct roveware::_packet _packet = roveware::packPacket(data_id, data_count, data_type, data);
  
  //write to client
  Client.write( _packet.bytes, (ROVECOMM_PACKET_HEADER_SIZE + (data_type_length * data_count))); 
}

//Overloaded writeReliable////////////////////////////////////////////////////////////////////////////////////////////////////
//Single-value write
//handles the data->pointer conversion for user
void RoveCommEthernetTCPClient::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const  int32_t data )
{                  int32_t data_p[1];
                   data_p[0] = data;
                   this->_writeReliable( 4,  roveware::INT32_T, data_id,               data_count,        (void*) data_p ); }

void RoveCommEthernetTCPClient::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const uint32_t data )
{                  uint32_t data_p[1];
                   data_p[0] = data;
                   this->_writeReliable( 4, roveware::UINT32_T, data_id,               data_count,        (void*) data_p ); }

void RoveCommEthernetTCPClient::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const  int16_t data )
{                  int16_t data_p[1];
                   data_p[0] = data;
                   this->_writeReliable( 2,  roveware::INT16_T, data_id,               data_count,        (void*) data_p ); }

void RoveCommEthernetTCPClient::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const uint16_t data )
{                  uint16_t data_p[1];
                   data_p[0] = data;
                   this->_writeReliable( 2, roveware::UINT16_T, data_id,               data_count,        (void*) data_p ); }

void RoveCommEthernetTCPClient::writeReliable(         const uint16_t data_id, const uint8_t data_count, const   int8_t data )
{                  int8_t data_p[1];
                   data_p[0] = data;
                   this->_writeReliable( 1,   roveware::INT8_T, data_id,               data_count,        (void*) data_p ); }

void RoveCommEthernetTCPClient::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const  uint8_t data )
{                  uint8_t data_p[1];
                   data_p[0] = data;
                   this->_writeReliable( 1,  roveware::UINT8_T, data_id,               data_count,        (void*) data_p ); }

void RoveCommEthernetTCPClient::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const  float data )
{                  uint8_t data_p[1];
                   data_p[0] = data;
                   this->_writeReliable( 4,  roveware::FLOAT, data_id,               data_count,        (void*) data_p ); }
//Array-Entry write///////////////////////////////////
void RoveCommEthernetTCPClient::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const  int32_t *data )
{                  this->_writeReliable( 4,  roveware::INT32_T, data_id,               data_count,        (void*) data ); }

void RoveCommEthernetTCPClient::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const uint32_t *data )
{                  this->_writeReliable( 4, roveware::UINT32_T, data_id,               data_count,        (void*) data ); }

void RoveCommEthernetTCPClient::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const  int16_t *data )
{                  this->_writeReliable( 2,  roveware::INT16_T, data_id,               data_count,        (void*) data ); }

void RoveCommEthernetTCPClient::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const uint16_t *data )
{                  this->_writeReliable( 2, roveware::UINT16_T, data_id,               data_count,        (void*) data ); }

void RoveCommEthernetTCPClient::writeReliable(         const uint16_t data_id, const uint8_t data_count, const   int8_t *data )
{                  this->_writeReliable( 1,   roveware::INT8_T, data_id,               data_count,        (void*) data ); }

void RoveCommEthernetTCPClient::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const  uint8_t *data )
{                  this->_writeReliable( 1,  roveware::UINT8_T, data_id,               data_count,        (void*) data ); }

void RoveCommEthernetTCPClient::writeReliable(        const uint16_t  data_id, const uint8_t data_count, const  float *data )
{                  this->_writeReliable( 4,  roveware::FLOAT, data_id,               data_count,        (void*) data ); }