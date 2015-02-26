// ================================================================
// ===                    PACK FUNCTIONS                      ===
// ================================================================


// Call every time you want to report data to central
// Generic function for ParameterReports and DataReports
void packTx_Report_OSC( byte _commMessage ) {

  // Cycle through the available parameters
  // for ( byte i=0; i < _numParams; i++ ) {

    switch ( _commMessage ) {
      case ACC_R: {
        SERIAL_PRINTLN("Send ACC_R");
        osc_tx.setAddress("/m/a/r");
        osc_tx.add( *model_acc_raw[0] ).add( *model_acc_raw[1] ).add( *model_acc_raw[2] );
        break;
      }
      case ACC_P: {
        SERIAL_PRINTLN("Send ACC_P");
        osc_tx.setAddress("/m/a/p");
        osc_tx.add( *model_acc_processed[0] );
        break;
      }
      case GYR_R: {
        SERIAL_PRINTLN("Send GYR_R");
        osc_tx.setAddress("/m/g/r");
        osc_tx.add( *model_gyr_raw[0] ).add( *model_gyr_raw[1] ).add( *model_gyr_raw[2] );
        break;
      }
      case GYR_P: {
        SERIAL_PRINTLN("Send GYR_P");
        osc_tx.setAddress("/m/g/p");
        osc_tx.add( *model_gyr_processed[0] ).add( *model_gyr_processed[1] );
        break;
      }
      default: {
        SERIAL_PRINTLN("OSC packed nothing");
        break;
      }
    }


    // Pack the OSC message into the packed_data byte array
    osc_tx.send(oscbuffer);
  // }
}


// ================================================================
// ===                    SEND FUNCTIONS                     ===
// ================================================================

void sendCommunications_Report( byte _commMessage )
{

    osc_tx.empty();

    // Pack data into transmission (packed_data)
    packTx_Report_OSC ( _commMessage );

    // Send data to main BASE
    long counter = millis();

    // Resize TxRequest based on actual data length.  Eliminates sending blank data over xBee.
    uint8_t actualPayloadLength = osc_tx.bytes();
    // tx.setPayloadLength(packed_data_length);
    tx.setPayloadLength(actualPayloadLength);

    xbee.send(tx); // Takes about 10 ms...why?
    oscbuffer.reset();
    SERIAL_PRINT("xbee send time = "); SERIAL_PRINTLN( millis()-counter );
}




// ================================================================
// ===                       XBEE SETUP                     ===
// ================================================================

void xbeeSetup()
{
    Serial3.begin(115200);
    xbee.setSerial(Serial3);
    delay(100);

    #ifdef SEND_INITIAL_TRANSMISSION
      SERIAL_PRINTLN("Sending controllable parameters...");
      BasicParameter *p[3] = {
              &animations[currentAnimation]->level_Parameter,
              &animations[currentAnimation]->hue_Parameter,
              &animations[currentAnimation]->decay_Parameter
            };

  //     Is this equivalent to below?
  //    BasicParameter *p[3];
  //    p[0] = &(animations[currentAnimation]->level_Parameter);
  //    p[1] = &(animations[currentAnimation]->hue_Parameter);
  //    p[2] = &(animations[currentAnimation]->decay_Parameter);

      sendCommunications_Report( REPORT_AVAIL_PARAMETERS, &p[0], 1 ); // Level
      delay(500);

      sendCommunications_Report( REPORT_AVAIL_PARAMETERS, &p[1], 1 ); // Hue
      delay(500);

      sendCommunications_Report( REPORT_AVAIL_PARAMETERS, &p[2], 1 ); // Decay
      delay(500);
    #endif

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
  
  /*
  reader.setBuffer(packed_data, packed_data_length);

  if ( reader.getType() == TP_LIST ) {
    reader.openList();
    reader.next();
    SERIAL_PRINTLN("List opening");
  }
  else { SERIAL_PRINTLN("Error opening first list"); }
  if ( reader.isInteger() ) {
    controlMessage = reader.getInteger();
    reader.next();
    SERIAL_PRINT("control message = ");
    SERIAL_PRINTLN(controlMessage);
  }
  else { SERIAL_PRINTLN("Error control integer"); }
  uint8_t type = reader.getType();
  SERIAL_PRINT("type "); SERIAL_PRINTLN(type);
  switch ( type ) {
    case TP_REAL:
      valFloat = reader.getReal();
      break;
    case TP_INTEGER:
      valInt = reader.getInteger();
      break;
    default:
      SERIAL_PRINTLN("Unsupported value type");
      break;
  }
  while (reader.close());

  // Handle the response here...
  switch ( controlMessage ) { // 
    case 0: // Animation change
      SERIAL_PRINT("Animation change = "); SERIAL_PRINTLN(valInt);
      currentAnimation = byte( constrain(valInt, 0, NUM_ANIMATIONS-1) );
      break;

    case 1: // Tune parameter 1
//      power.hue_Parameter.setPercent(valFloat);
      animations[currentAnimation]->hue_Parameter.setPercent(valFloat);
      SERIAL_PRINT("HueP change = "); SERIAL_PRINTLN(valFloat);
      break;

    case 2: // Tune parameter 2
      
//      power.decay_Parameter.setPercent(valFloat);
      animations[currentAnimation]->decay_Parameter.setPercent(valFloat);
      SERIAL_PRINT("Decay change = "); SERIAL_PRINTLN(valFloat);
      break;

    case 3: // Tune parameter 3
      SERIAL_PRINT("? change = "); SERIAL_PRINTLN(valFloat);
      break;

    default:
      SERIAL_PRINTLN("Improper control message)");
      break;
  }
  */
  
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
              SERIAL_PRINTLN("rx16 response");
              xbee.getResponse().getRx16Response(rx16);

              // Unload into packed_data
              byte responseLength = rx16.getDataLength();
              // SERIAL_PRINT("Response Length = "); SERIAL_PRINTLN(responseLength);
              // SERIAL_PRINT("Printing received data ");
              for ( byte i = 0; i < responseLength; i++ ) {
                packed_data[i] = rx16.getData(i);
                // SERIAL_PRINTLN(packed_data[i]);
              }
              
              packed_data_length = responseLength;

              // readAndPrintElements();
              // Need to unpacked appropriately here!
              unpackAndParseRx();
          }
          else if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) SERIAL_PRINTLN("Tx response - WHY???");
      }
      else { SERIAL_PRINTLN('xbee weird response type'); } // Not something we were expecting
    }
    else { SERIAL_PRINTLN(F("No incoming xBee messages")); }
}




// ================================================================
// ===                    READER FUNCTIONS                      ===
// ================================================================
