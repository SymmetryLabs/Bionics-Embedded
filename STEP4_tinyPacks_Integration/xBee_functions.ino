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
          writer.putString("pName");
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
  Serial.print("packed_data_length "); Serial.println(packed_data_length);
}




// ================================================================
// ===                    SEND FUNCTIONS                     ===
// ================================================================

void sendCommunications_Report( byte _type, BasicParameter *_p[], byte _numParams )
//void sendCommunications()
{
    // Pack data into transmission
    packTx_Report( _type, _p, _numParams );

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
// ===                       XBEE SETUP                     ===
// ================================================================

void xbeeSetup()
{
    Serial3.begin(115200);
    xbee.setSerial(Serial3);
    delay(1000);

    Serial.println("Sending controllable parameters...");
    BasicParameter *p[1] = { &power.level_Parameter }; // Need to initialize this array first, I don't know better syntax
    sendCommunications_Report( REPORT_AVAIL_PARAMETERS, p, 1 ); // Level
    delay(500);

    p[0] = &power.hue_Parameter;
    sendCommunications_Report( REPORT_AVAIL_PARAMETERS, p, 1 ); // Hue
    delay(500);

    p[0] = &power.decay_Parameter;
    sendCommunications_Report( REPORT_AVAIL_PARAMETERS, p, 1 ); // Decay

    delay(3000);
}




// ================================================================
// ===                    XBEE FUNCTIONS                     ===
// ================================================================

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
