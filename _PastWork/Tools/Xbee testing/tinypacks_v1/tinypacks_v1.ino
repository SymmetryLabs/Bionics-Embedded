#include <SoftwareSerial.h>
#include <TinyPacks.h>


#define sender


// Define the pins on Arduino for XBee comminication
uint8_t pinRx = 7 , pinTx = 6; // the pin on Arduino
long BaudRate = 9600 , sysTick = 0;
char GotChar;

// Initialize NewSoftSerial
SoftwareSerial mySerial( pinRx , pinTx );


PackWriter writer;
PackReader reader;

#define MAX_PACKED_DATA 100
unsigned char packed_data[MAX_PACKED_DATA];
int packed_data_length;


void setup()  
{
  Serial.begin(BaudRate);
  Serial.println("XBee Communication Test Start !");

  #ifdef sender
    Serial.println("Sender!");
  #endif
  #ifndef sender
    Serial.println("Receiver!");
  #endif

  // This part is the NewSoftSerial for talking to XBee
  mySerial.begin(BaudRate);
  mySerial.println("Powered by NewSoftSerial!");
}


void loop()                    
{
  #define MAX_TEXT_LENGTH 32
  char text[MAX_TEXT_LENGTH] = "";
  bool status = false;
  int  count = 0;  


  #ifdef sender

    String PCbuffer = "";

    // Read from computer
    while ( Serial.available() ) {
        GotChar = Serial.read();
        PCbuffer += GotChar;
    }

    // If something received
    if ( PCbuffer.length() > 0 )
    {
      // Pack it away
      writer.setBuffer(packed_data, MAX_PACKED_DATA);
      writer.openMap();
      writer.putString("text");
      writer.putString("Hello world!");
      writer.putString("status");
      writer.putBoolean(true);
      writer.putString("count");
      writer.putInteger(123);
      writer.close();
      packed_data_length = writer.getOffset();

      // Send it
      for ( int entry = 0; entry < packed_data_length; entry++ )
      {
        mySerial.write(packed_data[entry]);
        Serial.print(packed_data[entry]);
      }
    }


  #endif


  #ifndef sender

    char strValue[MAX_PACKED_DATA];
    int index = 0;
    bool receive_flag = false;

    // // Read from Xbee
    // while ( mySerial.available() ) {
    //     GotChar = mySerial.read();
    //     XBEEbuffer += GotChar;
    // }

    while ( mySerial.available() ) {
      char ch = mySerial.read();
      Serial.print(ch);
      if ( index <  MAX_PACKED_DATA ) strValue[index] = ch;
      index++;
      receive_flag = true;
    }

    // If something received
    if ( receive_flag )
    {
      Serial.println("Packet received!");
      /*
      Serial.print("Packet: ");
      for ( int entry = 0; entry < MAX_PACKED_DATA; entry++ ) Serial.print(strValue[entry]);
      Serial.println();*/

      // Unpack it
      reader.setBuffer(packed_data, packed_data_length);
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
    }

    if ( count != 0 ) {
      // Print unpacked data
      Serial.println("Map content:");
      Serial.print("  text: ");
      Serial.println(text);
      Serial.print("  status: ");
      Serial.println(status);
      Serial.print("  count: ");
      Serial.println(count);
      Serial.println();
    }

  #endif


}
