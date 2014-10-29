// ================================================================
// ===                    WRITER FUNCTIONS                      ===
// ================================================================


// MESSAGE TYPES

// Call after initial buffer opening
// Called sequentially to insert all the messages
void write_Report_Unit( const char *_parameterName, float _val) {
  writer.openMap();
  
    writer.putString("type");
    writer.putString("Report");
  
    writer.putString("msg");
    writer.openMap();
  
      writer.putString("pName");
      writer.putString(_parameterName);
  
      writer.putString("val");
      writer.putReal(_val);
  
    writer.close();
  writer.close();
}

// Call after initial buffer opening
// Called sequentially to insert all the messages
void write_Report_UnitParameter( const char *_parameterName, float _min, float _max, float _val ) {

  writer.openMap();
    writer.putString("type");
    writer.putString("paramReport");
  
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
  writer.close();
}



/*
// Doesn't work late Tuesday night.  Difficult loading BasicParameter as a pointer.
void write_Report_UnitParameter( BasicParameter *_basicParameter ) {

  writer.openMap();
    writer.putString("type");
    writer.putString("paramReport");
  
    writer.putString("msg");
    writer.openMap();
  
      writer.putString("pName");
      char _parameterName[5];
      _basicParameter->getName( _parameterName );
      writer.putString( _parameterName );
  
      writer.putString("min");
      writer.putReal( _basicParameter->getMin() );
  
      writer.putString("max");
      writer.putReal( _basicParameter->getMax() );
  
      writer.putString("val");
      writer.putReal( _basicParameter->getValue() );
  
    writer.close();
  writer.close();
}
*/


// ================================================================
// ===                    PACK FUNCTIONS                      ===
// ================================================================


// Use this for the 10/28 test...
// NEED TO ABSTRACT THIS TO INCLUDE THE FULL MAPPING OF SENT DATA FOR FLEXIBILITY
// IE SHOULD BE ABLE TO ACCEPT VARIABLE NUMBERS OF PARAMETERS, WITH THEIR NAMES

// Call every time you want to report data to central
void packTx_Report( /*float _aaRealPercent, float _rollPercent*/ ) {  // Ideally want an array of functions, combine with below function
  // Pack
  writer.setBuffer(packed_data, MAX_PACKED_DATA);

  write_Report_Unit("aaRealP", power.level_Parameter.getValue() );
  write_Report_Unit("rollP", power.hue_Parameter.getValue() );

  writer.close();

  packed_data_length = writer.getOffset();
  Serial.print("packed_data_length "); Serial.println(packed_data_length);
}

// Call when you need to send the parameter report initially
// Not sure if I'll be able to send this...
//void packTx_ParameterReport( BasicParameter *_basicParameter ) {  // Ideally want an array of functions, combine with above function
void packTx_ParameterReport( char *_name, float _min, float _max, float _val ) {
  // Pack
  writer.setBuffer(packed_data, MAX_PACKED_DATA);

//  write_Report_UnitParameter( _basicParameter );
  write_Report_UnitParameter( _name, _min, _max, _val );

  writer.close();

  packed_data_length = writer.getOffset();
  Serial.print("packed_data_length "); Serial.println(packed_data_length);
}

// ================================================================
// ===                    SEND FUNCTIONS                     ===
// ================================================================


// Had to move here in order to compile...prefer it in xBee_functions
 void sendCommunications_Report()
//void sendCommunications()
{
    // Pack data into transmission
    // These are custom mapped from Power animation
    // NEED TO UNDO THIS...
    packTx_Report();

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

void sendCommunications_ParamReport1()
//void sendCommunications()
{
    // Pack data into transmission
    // These are custom mapped from Power animation
    char _name[5];
    power.level_Parameter.getName(_name);
    packTx_ParameterReport( _name, power.level_Parameter.getMin(), power.level_Parameter.getMax(), power.level_Parameter.getValue() );

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

void sendCommunications_ParamReport2()
//void sendCommunications()
{
    // Pack data into transmission
    // These are custom mapped from Power animation
    char _name[5];
    power.hue_Parameter.getName(_name);
    packTx_ParameterReport( _name, power.hue_Parameter.getMin(), power.hue_Parameter.getMax(), power.hue_Parameter.getValue() );

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

void sendCommunications_ParamReport3()
//void sendCommunications()
{
    // Pack data into transmission
    // These are custom mapped from Power animation
    char _name[5];
    power.decay_Parameter.getName(_name);
    packTx_ParameterReport( _name, power.decay_Parameter.getMin(), power.decay_Parameter.getMax(), power.decay_Parameter.getValue() );

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
    sendCommunications_ParamReport1(); // Level
    sendCommunications_ParamReport2(); // Hue
    sendCommunications_ParamReport3(); // Decay

//    sendCommunications();

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






/*
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
*/
