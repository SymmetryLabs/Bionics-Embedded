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

  sendCommunications();

  // Unpack
  /*reader.setBuffer(packed_data, packed_data_length);
  reader.next();
  if(reader.openMap()) {
    while(reader.next()) {
      if     (reader.match("status"))  status = reader.getBoolean();
      else if(reader.match("count"))   count = reader.getInteger();
      else if(reader.match("text"))    reader.getString(text, MAX_TEXT_LENGTH);
      else reader.next();
    }
    reader.close();
  }
 
  // Print unpacked data
  Serial.println("Map content:");
  Serial.print("  text: ");
  Serial.println(text);
  Serial.print("  status: ");
  Serial.println(status);
  Serial.print("  count: ");
  Serial.println(count);
  Serial.println();
  */

  delay(1000);  
}
