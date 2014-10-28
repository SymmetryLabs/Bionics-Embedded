/*
    Contains the packing / unpacking functions for the Bionics.
    This version includes nested messages (maps inside maps).
*/

#include <TinyPacks.h>

PackWriter writer;
PackReader reader;

#define MAX_PACKED_DATA 512
unsigned char packed_data[MAX_PACKED_DATA];
int packed_data_length;

#define MAX_TEXT_LENGTH 32
char text[MAX_TEXT_LENGTH] = "";

// enum MSG_TYPE {
//     UNIT_PARAM_CONTROL = 0,
//     UNIT_ANIM_CONTROL = 1
// };

struct ParamControlMessage {
  char key[16];
  float val;
};


void setup()
{
  Serial.begin(115200);
  while(!Serial);
  Serial.println("TinyPacks Bionics Test BEGIN");
}

void loop()
{
  Serial.println("Loop");


  writeFunction();

  Serial.print("Packed data length "); Serial.println(packed_data_length); Serial.println(); Serial.println();



  Serial.println("Start Unpacking Message");
  Serial.println("----------");

  readFunctionCustom();

  Serial.println("----------"); Serial.println(); Serial.println();


  delay(5000);
}




// ================================================================
// ===                    WRITER FUNCTIONS                      ===
// ================================================================

void writeFunction() {
  // Pack
  writer.setBuffer(packed_data, MAX_PACKED_DATA);

  writer.openMap();

    // write_Report_UnitParameter("decay", 0.0, 0.5, 1.0);
    // write_Report_UnitParameter("hue", 1.5, 2.0, 2.5);

  write_Control_UnitParameter("decay", 1.0);
  write_Control_UnitParameter("hue", 2.5);

  writer.close();

  packed_data_length = writer.getOffset();
}


void write_Report_UnitParameter( const char *_parameterName, float _min, float _max, float _val ) {

  writer.putString("msgType");
  writer.putString("UnitParamReport");

  writer.putString("msg");
  writer.openMap();

    writer.putString("pName");
    writer.putString(_parameterName);

    writer.putString("min");
    writer.putReal(_min);

    writer.putString("max");
    writer.putReal(_max);

    writer.putString("val");
    writer.putReal(_val);

  writer.close();
}

// THIS SHOULD NEVER BE ON THE UNITS
void write_Control_UnitParameter( const char *_parameterName, float _val ) {

  writer.putString("msgType");
  writer.putString("UnitParamControl");

  writer.putString("msg");
  writer.openMap();

    writer.putString("pName");
    writer.putString(_parameterName);

    writer.putString("val");
    writer.putReal(_val);

  writer.close();
}


void write_Report_Unit() {
  writer.putString("msgType");
  writer.putString("UnitReport");

  writer.putString("msg");
  writer.openMap();

    writer.putString("pName");
    writer.putString(_parameterName);

    writer.putString("val");
    writer.putReal(_val);

  writer.close();
}





// ================================================================
// ===                    READER FUNCTIONS                      ===
// ================================================================


void readAndPrintElements() {
  // Unpack and read entire message, continuously close() until nothing is left
  reader.setBuffer(packed_data, packed_data_length);
  do {
    while(reader.next()) {
      printElement();
    }
  }
  while (reader.close());
}


void readFunctionCustom() {

  struct ParamControlMessage message1;

  // Unpack and read entire message, continuously close() until nothing is left
  reader.setBuffer(packed_data, packed_data_length);

  // NEED TO ADD ONE FROM THE XBEE DATA STRUCTURE
  // reader.setBuffer(rx16.getData()+1, packed_data_length); // need to remove first element

  // STEPS
  // GO TO NEXT TO GET AWAY FROM THE AMOUNT
  // OPEN THE MAP
  // START ITERATING
  byte currentMessage = 0;


  if ( reader.next() && reader.openMap() ) {
    Serial.println("Begin parsing message");
    while ( reader.next() && reader.match("msgType") ) {

      // Read the message type to text
      reader.getString(text, MAX_TEXT_LENGTH);

      // TECHNICALLY this goes to "msg"
      reader.next();
      // TECHNICALY this goes to the map afterwards
      reader.next();

      // Open the map to read it
      reader.openMap();


      // Different unloaders based on the message type
      if ( strcmp(text,"UnitParamControl") == 0 ) {

        memset(&text[0], 0, sizeof(text));
        Serial.println("Unpacking UnitParamControl");

        while ( reader.next() ) {
          // printElement();
          if ( reader.match("pName") ) reader.getString(message1.key, sizeof(message1.key));
          else if ( reader.match("val") ) message1.val = reader.getReal();
          else Serial.println("Not a valid UnitParamControl key");
        }
        reader.close();
      }

      else if ( strcmp(text,"UnitAnimControl") == 0 ) {

        memset(&text[0], 0, sizeof(text));
        Serial.println("Unpacking UnitAnimControl");

        while ( reader.next() ) printElement();
        reader.close();
      }

      /*
      // These technically won't be necessary on the units...
      else if ( strcmp(text,"UnitParamReport") == 0 ) {
        memset(&text[0], 0, sizeof(text));
        Serial.println("Unpacking UnitParamReport");
        while ( reader.next() ) printElement();
        reader.close();
      }

      else if ( strcmp(text,"UnitReport") == 0 ) {
        memset(&text[0], 0, sizeof(text));
        Serial.println("Unpacking UnitReport");
        while ( reader.next() ) printElement();
        reader.close();
      }
      */

      else Serial.println("Unsupported message type");
      Serial.println();
    }
  }
  else Serial.println("Error parsing incoming message");


  Serial.print("Message1 "); Serial.print(message1.key); Serial.print(" : "); Serial.println(message1.val );
}




void read_Control_Unit () {

}




void printElement() {

  uint8_t type = reader.getType();

  switch ( type ) {
    case TP_NONE:
    {
      Serial.println("Cannot print none");
      break;
    }

    case TP_BOOLEAN:
    {
      Serial.print("Boolean "); Serial.println(reader.getBoolean());
      break;
    }

    case TP_INTEGER:
    {
      Serial.print("Integer "); Serial.println(reader.getInteger());
      break;
    }

    case TP_REAL:
    {
      Serial.print("Real "); Serial.println(reader.getReal());
      break;
    }
    
    case TP_STRING:
    {
      reader.getString(text, MAX_TEXT_LENGTH);
      Serial.print("String "); Serial.println( text );
      break;
    }
    
    case TP_BYTES:
    {
      // Serial.print("Bytes "); Serial.println(reader.getBytes());
      Serial.println("Cannot print bytes");
      break;
    }
    
    case TP_LIST:
    {
      Serial.println("Cannot print list");
      reader.openList();
      break;
    }
    
    case TP_MAP:
    {
      Serial.println("Opening map");
      reader.openMap();
      break;
    }
    default:
      Serial.println("ERROR! NO TYPE");
  }
}