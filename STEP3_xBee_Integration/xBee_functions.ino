#include "XBee.h"

PackWriter writer;
PackReader reader;

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
    if (xbee.readPacket(1))
    {
        if (xbee.getResponse().isAvailable()) {
            if (xbee.getResponse().getApiId() == RX_16_RESPONSE)
            {
                xbee.getResponse().getRx16Response(rx16);
                Serial.println("rx response");
            }
            else if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) Serial.println("tx response");
        }
        else
        {
            // not something we were expecting
            // Serial.println('xbee weird response type');    
        }
    }
    else
    {
        Serial.println(F("No incoming xBee messages"));
    }
}



void sendCommunications()
{
    // Pack data into transmission

    // Send data to main BASE
    xbee.send(tx);

    // Query xBee for incoming messages
    if (xbee.readPacket(1))
    {
        if (xbee.getResponse().isAvailable()) {
            if (xbee.getResponse().getApiId() == RX_16_RESPONSE)
            {
                xbee.getResponse().getRx16Response(rx16);
                Serial.println("rx response");

                // Unpack and parse with tinypacks
                unpackAndParseXbeeData();
            }
            else if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) Serial.println("tx response");
        }
        else
        {
            // not something we were expecting
            // Serial.println('xbee weird response type');    
        }
    }
    else
    {
        Serial.println(F("No tx ack from xBee"));
    }
}


// ================================================================
// ===                 TINYPACKS FUNCTIONS                      ===
// ================================================================


// Call tinypacks to unpack data response
void unpackAndParseXbeeData()
{
    // Extract packet length from xbee data stream
    byte packed_data_length = rx16.getData(0);
    reader.setBuffer(rx16.getData()+1, packed_data_length); // need to remove first element
    reader.next();
    if(reader.openMap()) {
        processMap();
        reader.close();
    }
}

// Iterate through map and send response variables into pointers
void processMap()
{
    while(reader.next()) {

        switch ( )


        if (reader.match("h"))
        {
            comm_hue = reader.getInteger();
//            Serial.print("hue "); Serial.println(comm_hue);
        }
        else if (reader.match("e1"))
        {
            comm_event1 = reader.getInteger();
//            Serial.print("e1 "); Serial.println(comm_event1);
        }
        else if (reader.match("e2"))
        {
            comm_event2 = reader.getInteger();
        }
        else reader.next();
    }
}
