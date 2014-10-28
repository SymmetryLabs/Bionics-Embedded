#include "XBee.h"

// xBee variables
XBee xbee = XBee();

#define MAX_PACKED_DATA 40
uint8_t packed_data[MAX_PACKED_DATA];
int packed_data_length;

// Transmission variables
Tx16Request tx = Tx16Request(0x0001, packed_data, sizeof(packed_data)); // 16-bit addressing: Enter address of remote XBee, typically the coordinator
TxStatusResponse txStatus = TxStatusResponse();

// Receiving variables
XBeeResponse response = XBeeResponse(); 
Rx16Response rx16 = Rx16Response(); // create reusable response objects for responses we expect to handle
uint8_t option = 0;



void xbeeSetup()
{
    Serial3.begin(115200);
    xbee.setSerial(Serial3);
    delay(3000);
}



// Needs to return array of responses to the model
void getCommunications()
{

    // Query xBee for incoming messages
    if (xbee.readPacket(5))
    {
        if (xbee.getResponse().isAvailable()) {
            if (xbee.getResponse().getApiId() == RX_16_RESPONSE)
            {
                xbee.getResponse().getRx16Response(rx16);

            }
            else if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) Serial.println("tx response");
        }
        else
        {
            // not something we were expecting
            // Serial.println('xbee weird response type');    
        }
    }
}



void sendCommunications()
{
    // Pack data into transmission

    // Send data to main BASE
    xbee.send(tx);

    // Query xBee for incoming messages
    if (xbee.readPacket(5))
    {
        if (xbee.getResponse().isAvailable()) {
            if (xbee.getResponse().getApiId() == RX_16_RESPONSE)
            {
                xbee.getResponse().getRx16Response(rx16);

            }
            else if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) Serial.println("tx response");
        }
        else
        {
            // not something we were expecting
            // Serial.println('xbee weird response type');    
        }
    }
}
