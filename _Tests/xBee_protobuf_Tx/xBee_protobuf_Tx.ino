#include "bionic.pb.h"

#include <XBee.h>

XBee xbee = XBee();

unsigned long start = millis();

// allocate two bytes for to hold a 10-bit analog reading
#define MAX_PACKED_DATA 40
uint8_t packed_data[MAX_PACKED_DATA];
int packed_data_length;

// with Series 1 you can use either 16-bit or 64-bit addressing

// 16-bit addressing: Enter address of remote XBee, typically the coordinator
Tx16Request tx = Tx16Request(0x0001, packed_data, sizeof(packed_data));

// 64-bit addressing: This is the SH + SL address of remote XBee
//XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x4008b490);
// unless you have MY on the receiving radio set to FFFF, this will be received as a RX16 packet
//Tx64Request tx = Tx64Request(addr64, payload, sizeof(payload));

TxStatusResponse txStatus = TxStatusResponse();


void setup() {
  Serial.begin(115200);
  Serial3.begin(115200);
  xbee.setSerial(Serial3);

  Serial.print("Series1_Tx_Teensy - setup complete");
  delay(3000);
}

void loop() {
   
    Serial.println("Start transmission");

    
    xbee.send(tx);

    
    // after sending a tx request, we expect a status response
    // wait up to 5 seconds for the status response
    if (xbee.readPacket(1000)) {
        // got a response!

        // should be a znet tx status            	
    	if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
    	   xbee.getResponse().getZBTxStatusResponse(txStatus);
    		
    	   // get the delivery status, the fifth byte
           if (txStatus.getStatus() == SUCCESS) {
            	// success.  time to celebrate
              Serial.println("Success!");
           } else {
            	// the remote XBee did not receive our packet. is it powered on?
              Serial.println("No dice");
           }
        }      
    } else if (xbee.getResponse().isError()) {
        Serial.print("Error reading packet.  Error code: ");  
        Serial.println(xbee.getResponse().getErrorCode());
      // or flash error led
    } else {
      // local XBee did not provide a timely TX Status Response.  Radio is not configured properly or connected
        Serial.println("Nothing at all");
    }
    
    
    delay(1000);
}
