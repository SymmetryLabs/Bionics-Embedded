void printParameterP( BasicParameter *_p ) {
  char name[5];
  _p->getName(name);
  // for ( byte i = 0; i < sizeof(name); i++ ) Serial.println(name[i], HEX);
  Serial.print("Name: "); Serial.println(name);
  Serial.print("Val: "); Serial.println(_p->getValue());
  Serial.print("Min: "); Serial.println(_p->getMin());
  Serial.print("Max: "); Serial.println(_p->getMax());
}

// ================================================================
// ===                    PACK FUNCTIONS                      ===
// ================================================================

// Call every time you want to report data to central
// Generic function for ParameterReports and DataReports
void packTx_Report( byte _reportType, BasicParameter *_p[], byte _numParams ) {
  // Pack
  writer.setBuffer(packed_data, MAX_PACKED_DATA);

    writer.openMap();
  
      writer.putString("type");
      char typeString[5];
      switch( _reportType ) {
        case REPORT_DATA:
          strcpy(typeString, "dRep");
          break;
        case REPORT_AVAIL_PARAMETERS:
          strcpy(typeString, "pRep");
          break;
      }
      writer.putString(typeString);
    
      writer.putString("msg");
      writer.openList();
      
      // Cycle through the available parameters
      for ( byte i=0; i < _numParams; i++ )
      { 
        writer.openMap();
    
          char _parameterName[5];
          _p[i]->getName( _parameterName );
          writer.putString("pNam");
          writer.putString(_parameterName);
  
          if ( _reportType == REPORT_AVAIL_PARAMETERS ) {
            writer.putString("min");
            writer.putReal( _p[i]->getMin() );
    
            writer.putString("max");
            writer.putReal( _p[i]->getMax() );
          }
      
          writer.putString("val");
          writer.putReal(_p[i]->getValue());
    
        writer.close();
      }
      
      writer.close(); // Close the list of parameters
  
    writer.close(); // Close the dictionary with reportType and messages

  writer.close(); // Close the writer

  packed_data_length = writer.getOffset();
  // Warn that the data packet is probably too big!
  if ( packed_data_length >= 98 ) {
    Serial.print("WARNING!  packed_data_length "); Serial.println(packed_data_length);
  }
}




// ================================================================
// ===                    SEND FUNCTIONS                     ===
// ================================================================

void sendCommunications_Report( byte _type, BasicParameter *_p[], byte _numParams )
{

    // Pack data into transmission (packed_data)
    packTx_Report( _type, _p, _numParams );

    // Send data to main BASE
    // long counter = millis();
    xbee.send(tx); // Takes about 10 ms...why?
    // Serial.print("xbee send time = "); Serial.println( millis()-counter );
}




// ================================================================
// ===                       XBEE SETUP                     ===
// ================================================================

void xbeeSetup()
{
    Serial3.begin(115200);
    xbee.setSerial(Serial3);
    delay(100);

    Serial.println("Sending controllable parameters...");
    BasicParameter *p[1] = { &power.level_Parameter }; // Need to initialize this array first, I don't know better syntax
    sendCommunications_Report( REPORT_AVAIL_PARAMETERS, p, 1 ); // Level
//    delay(100);

    p[0] = &power.hue_Parameter;
    sendCommunications_Report( REPORT_AVAIL_PARAMETERS, p, 1 ); // Hue
//    delay(500);

    p[0] = &power.decay_Parameter;
    sendCommunications_Report( REPORT_AVAIL_PARAMETERS, p, 1 ); // Decay

//    delay(3000);
}




// ================================================================
// ===                    XBEE FUNCTIONS                     ===
// ================================================================

void unpackAndParseRx() {
  // Unpack and read entire message, continuously close() until nothing is left
  int controlMessage = -1; // Out of range
  float valFloat = 0.00;
  int valInt = 0;
  char valString[5]; 
  
  reader.setBuffer(packed_data, packed_data_length);
  reader.next();
  reader.getType() == TP_MAP ? reader.openList(), reader.next() : Serial.println("Error opening first list");
  reader.isInteger() ? controlMessage = reader.getInteger(), reader.next() : Serial.println("Error control integer");
  uint8_t type = reader.getType();
  switch ( type ) {
    case TP_REAL:
      valFloat = reader.getReal();
    case TP_INTEGER:
      valInt = reader.getInteger();
    default:
      Serial.println("Unsupported value type");
  }

  // Handle the response here...
  switch ( controlMessage ) { // 
    case 0: // Animation change
      Serial.print("Animation change = "); Serial.println(valInt);
      currentAnimation = valInt;

    case 1: // Tune parameter 1
      power.hue_Parameter.setPercent(valFloat);
      Serial.print("HueP change = "); Serial.println(valFloat);

    case 2: // Tune parameter 2
      power.decay_Parameter.setPercent(valFloat);
      Serial.print("Decay change = "); Serial.println(valFloat);

    case 3: // Tune parameter 3
      Serial.print("? change = "); Serial.println(valFloat);

    default:
      Serial.println("Improper control message)");
  }
  
  while (reader.close());
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
              Serial.println("rx16 response");
              xbee.getResponse().getRx16Response(rx16);

              // Unload into packed_data
              byte responseLength = rx16.getDataLength();
              // Serial.print("Response Length = "); Serial.println(responseLength);
              // Serial.print("Printing received data ");
              for ( byte i = 0; i < responseLength; i++ ) {
                packed_data[i] = rx16.getData(i);
                // Serial.println(packed_data[i]);
              }
              
              packed_data_length = responseLength;
              // readAndPrintElements();
              // Need to unpacked appropriately here!
              unpackAndParseRx();
          }
          else if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) Serial.println("Tx response - WHY???");
      }
      else { Serial.println('xbee weird response type'); } // Not something we were expecting
    }
    else { Serial.println(F("No incoming xBee messages")); }
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
