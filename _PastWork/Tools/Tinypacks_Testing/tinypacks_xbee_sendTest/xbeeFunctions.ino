// xBee variables
XBee xbee = XBee();
AltSoftSerial xbeeSerial; // Use D8 = Dout and D9 = Din by default

// Transmission variables
// uint8_t payload[] = { 0, 0 }; // allocate two bytes for to hold a 10-bit analog reading
// Tx16Request tx = Tx16Request(0xFFFF, payload, sizeof(payload)); // 16-bit addressing: Enter address of remote XBee, typically the coordinator
Tx16Request tx = Tx16Request(0xFFFF, packed_data, sizeof(packed_data)); // 16-bit addressing: Enter address of remote XBee, typically the coordinator
TxStatusResponse txStatus = TxStatusResponse();

// Receiving variables
XBeeResponse response = XBeeResponse(); 
Rx16Response rx16 = Rx16Response(); // create reusable response objects for responses we expect to handle
uint8_t option = 0;
uint8_t data1 = 0;
uint8_t data2 = 0;
int data = 0;



void xbeeSetup()
{
    xbeeSerial.begin(57600);
    xbee.setSerial(xbeeSerial);
}


void getCommunications()
{
    // Serial.println("xB get");

	data = 0;
	data1 = 0;
	data2 = 0;

    // Query xBee for incoming messages
    // xbeeSerial.listen();
    if (xbee.readPacket(1))
    {
        if (xbee.getResponse().isAvailable()) {
        	// Serial.printl n("xb avail!");
            // got something
            
            if (xbee.getResponse().getApiId() == RX_16_RESPONSE)
            {
                xbee.getResponse().getRx16Response(rx16);
                option = rx16.getOption();
                data1 = rx16.getData(0);
                data2 = rx16.getData(1);
            }
            else if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE)
            {
                Serial.println("tx response");
            }

            // Serial.print("data1: "); Serial.println(data1);
            // Serial.print("data2: "); Serial.println(data2);
            
            data |= data2;
            data |= data1 << 8;

            
            Serial.print("Data: "); Serial.println(data);

            // Serial.print("RSSI: "); Serial.println(rx16.getRssi());
        }
        else
        {
            // not something we were expecting
            Serial.println('xbee weird response type');    
        }
    }
    // Serial.println("xB get end");
}


void sendCommunications()
{
    writer.setBuffer(packed_data, MAX_PACKED_DATA);
    writer.openMap();
    writer.putString("id");
    writer.putInteger(1);
    writer.putString("m1");
    writer.putInteger(1000);
    writer.putString("m2");
    writer.putInteger(888);
    writer.putString("m3");
    writer.putInteger(888);
    writer.putString("m4");
    writer.putInteger(888);
    writer.putString("s1");
    writer.putInteger(19);
    writer.putString("s2");
    writer.putInteger(1023);
    writer.putString("e1");
    writer.putInteger(2);
    writer.putString("e2");
    writer.putInteger(0);


    writer.close();
    packed_data_length = writer.getOffset();

    Serial.print("packed_data ");
    for ( int data = 0; data < packed_data_length; data++ )
    {
        Serial.print(packed_data[data], HEX);
        Serial.print(" ");
    }
    Serial.println();
      
    // Serial.println("xB send");
    // xbeeSerial.listen();
    // Send data to main BASE
    // int package = getModelOutput_Mag1();
    // Serial.print("Payload ");Serial.println(package);
    // payload[0] = package >> 8 & 0xff;
    // payload[1] = package & 0xff;
    xbee.send(tx);

    Serial.println("transmission sent");

    // xbee.readPacket(5);
    // do I need to receive the response?
    // Serial.println("xB send end");
}