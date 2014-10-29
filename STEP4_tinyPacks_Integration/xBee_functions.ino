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
// ===                    WRITER FUNCTIONS                      ===
// ================================================================


// Prototype master function for write...
void write_ReportOfBasicParameters( BasicParameter *_p[], byte _numParams ) {
  byte _type = 0;
  
  writer.openMap();
  
    writer.putString("type");
//    string typeString;
    char typeString[5];
    switch( _type ) {
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
      // Serial.print("write_ReportOfBasicParameters i = "); Serial.println(i);
      
      writer.openMap();
  
        char _parameterName[5];
        _p[i]->getName( _parameterName );
        writer.putString("pName");
        writer.putString(_parameterName);

        if ( _type == REPORT_AVAIL_PARAMETERS ) {
          writer.putString("min");
          writer.putReal( _p[i]->getMin() );
  
          writer.putString("max");
          writer.putReal( _p[i]->getMax() );
        }
    
        writer.putString("val");
        writer.putReal(_p[i]->getValue());
  
      writer.close();
    }
    writer.close();

  writer.close();

}

// Call after initial buffer opening
// Called sequentially to insert all the messages
void write_Report_Unit( BasicParameter *_p ) {
  writer.openMap();
  
    writer.putString("type");
    writer.putString("Report");
  
    writer.putString("msg");
    writer.openMap();
  
      char _parameterName[5];
      _p->getName( _parameterName );
      writer.putString("pName");
      writer.putString(_parameterName);
  
      writer.putString("val");
      writer.putReal(_p->getValue());
  
    writer.close();
  writer.close();
}

// Call after initial buffer opening
// Called sequentially to insert all the messages
void write_Report_UnitParameter( BasicParameter *_p ) {

  writer.openMap();
    writer.putString("type");
    writer.putString("paramReport");
  
    writer.putString("msg");
    writer.openMap();
  
      writer.putString("pName");
      char _parameterName[5];
      _p->getName( _parameterName );
      writer.putString( _parameterName );
  
      writer.putString("min");
      writer.putReal( _p->getMin() );
  
      writer.putString("max");
      writer.putReal( _p->getMax() );
  
      writer.putString("val");
      writer.putReal( _p->getValue() );
  
    writer.close();
  writer.close();
}



// ================================================================
// ===                    PACK FUNCTIONS                      ===
// ================================================================


// Call every time you want to report data to central
// void packTx_Report( BasicParameter *_p, byte _numParams ) {  // Ideally want an array of functions, combine with below function
void packTx_Report() {  // Ideally want an array of functions, combine with below function
  // Pack
  writer.setBuffer(packed_data, MAX_PACKED_DATA);

  // write_Report_Unit( &power.level_Parameter ); // Happens to be delivering aaReal
  // write_Report_Unit( &power.hue_Parameter ); // Happens to be delivering roll

  BasicParameter *paramsToSend[2] = { &power.level_Parameter, &power.hue_Parameter };
//  enum ReportType type = REPORT_DATA;
//  write_ReportOfBasicParameters( type, paramsToSend, 2 );
  write_ReportOfBasicParameters( paramsToSend, 2 );

  writer.close();

  packed_data_length = writer.getOffset();
  if ( packed_data_length >= 98 ) { Serial.print("packed_data_length "); Serial.println(packed_data_length); }
}

// Call when you need to send the parameter report initially
// Not sure if I'll be able to send this...
// Ideally want an array of functions, combine with above function
void packTx_ParameterReport( BasicParameter *_p ) {
  // Pack
  writer.setBuffer(packed_data, MAX_PACKED_DATA);

  write_Report_UnitParameter( _p );

  writer.close();

  packed_data_length = writer.getOffset();
  Serial.print("packed_data_length "); Serial.println(packed_data_length);
}




// ================================================================
// ===                    SEND FUNCTIONS                     ===
// ================================================================


// Had to move here in order to compile...prefer it in xBee_functions
 void sendCommunications_Report()
{
    // Pack data into transmission
    // These are custom mapped from Power animation
    // NEED TO UNDO THIS...
    packTx_Report();
    // readAndPrintElements();

    // Send data to main BASE
    xbee.send(tx);

    // Query xBee for incoming messages
    if (xbee.readPacket(1))
    {
        if (xbee.getResponse().isAvailable()) {
            if (xbee.getResponse().getApiId() == RX_16_RESPONSE)
            {
                xbee.getResponse().getRx16Response(rx16);
                Serial.println("rx response"); // Leave this in to see if there's so much data that incoming arrives before the tx ACK
            }
            else if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
              // Serial.println("tx response");
            }
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

void sendCommunications_ParamReport( BasicParameter *_p )
//void sendCommunications()
{
    // Pack data into transmission
    packTx_ParameterReport( _p );

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
    sendCommunications_ParamReport( &power.level_Parameter ); // Level
    // printParameterP( &power.level_Parameter );
    delay(500);
    sendCommunications_ParamReport( &power.hue_Parameter ); // Hue
    delay(500);
    sendCommunications_ParamReport( &power.decay_Parameter ); // Decay

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

/*
// sendCommunications argument = a function that returns void and has no arguments
 void sendCommunications( void (*_loadTx)() )
//void sendCommunications()
{
    // Pack data into transmission
    // These are custom mapped from Power animation
    // NEED TO UNDO THIS...
    (*_loadTx)();
//    loadUnitReport();

    // Send data to main BASE
    xbee.send(tx);
}
*/


// ================================================================
// ===                 TINYPACKS FUNCTIONS                      ===
// ================================================================








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
