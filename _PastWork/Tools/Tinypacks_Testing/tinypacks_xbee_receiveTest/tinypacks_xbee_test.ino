#include <XBee.h>
#include <AltSoftSerial.h>

#include <TinyPacks.h>

PackWriter writer;
PackReader reader;

#define MAX_PACKED_DATA 100
unsigned char packed_data[MAX_PACKED_DATA];
int packed_data_length;

void setup()
{
  Serial.begin(115200);
  xbeeSetup();
  Serial.println("xbee delay");
  delay(1000);
}

void loop()
{
  getCommunications();

  delay(1000);  
}
