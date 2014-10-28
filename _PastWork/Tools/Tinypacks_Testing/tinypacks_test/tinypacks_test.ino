#include <TinyPacks.h>

PackWriter writer;
PackReader reader;

#define MAX_PACKED_DATA 40
unsigned char packed_data[MAX_PACKED_DATA];
int packed_data_length;

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  #define MAX_TEXT_LENGTH 32
  // char text[MAX_TEXT_LENGTH] = "";
  bool status = false;
  int  count = 0;  

  /*
  // offset of 26
  // Pack
  writer.setBuffer(packed_data, MAX_PACKED_DATA);
  writer.openMap();
  writer.putString("id");
  writer.putInteger(1);
  writer.putString("m1");
  writer.putInteger(1000);
  writer.putString("m2");
  writer.putInteger(888);
  writer.putString("s1");
  writer.putInteger(250);
  */

/*  // offset of 32
  writer.setBuffer(packed_data, MAX_PACKED_DATA);
  writer.openMap();
  writer.putString("m1");
  writer.putInteger(1000);
  writer.putString("m2");
  writer.putInteger(888);
  writer.putString("s1");
  writer.putInteger(5000);
  writer.putString("s2");
  writer.putInteger(1023);
  writer.putString("e1");
  writer.putInteger(2);
  writer.putString("e2");
  writer.putInteger(0);
  */

  // offset of 32
  writer.setBuffer(packed_data, MAX_PACKED_DATA);
  writer.openMap();
  writer.putString("m1");
  writer.putInteger(1000);
  writer.putString("m2");
  writer.putInteger(888);
  writer.putString("s1");
  writer.putInteger(5000);
  // writer.putString("s2");
  // writer.putInteger(1023);
  // writer.putString("e1");
  // writer.putInteger(2);
  // writer.putString("e2");
  // writer.putInteger(0);


  writer.close();
  packed_data_length = writer.getOffset();
  Serial.print("offset: "); Serial.println(packed_data_length);
  
  for ( int data = 0; data < packed_data_length; data++ )
  {
    Serial.print(packed_data[data], HEX); Serial.print(" ");
  }
  Serial.println();
  Serial.println();

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

  delay(5000);  
}
