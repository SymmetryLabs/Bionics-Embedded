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
    osc_tx.empty();
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


// Needs to return array of responses to the model
void getCommunications()
{

    // Query xBee for incoming messages
    if (xbee.readPacket(1))
    {
      if (xbee.getResponse().isAvailable()) {
          if (xbee.getResponse().getApiId() == RX_16_RESPONSE)
          {
              SERIAL_PRINTLN2("RX16 response");
              xbee.getResponse().getRx16Response(rx16);

              // Get lenght of reponse in bytes, for unloading into OSC
              byte responseLength = rx16.getDataLength();


              // OPTIONALLY print all the reponse bytes...
              #ifdef RX_PRINT_DATA_BYTES
                SERIAL_PRINT2("Response Length = "); SERIAL_PRINTLN2(responseLength);
                SERIAL_PRINT2("Response DATA = ");
                // Could use getData() below instead to return a pointer to the data array...
                for ( byte i = 0; i < responseLength; i++ ) { SERIAL_PRINT2( rx16.getData(i) ); SERIAL_PRINT2(" "); }
                SERIAL_PRINTLN2();
              #endif


              // Unpack bytes in OSC message
              osc_rx.fill( rx16.getData(), responseLength );


              // OPTIONALLY print all the OSC contents...
              #ifdef RX_PRINT_OSC_BYTES
                printOSCMessage(osc_rx, responseLength);
              #endif

              // Dispatch the message
              osc_rx.dispatch("/eq", eqControl);
              osc_rx.dispatch("/midi/cc", midiControl);

              // Dispatch for selecting data reporting types
              osc_rx.dispatch("report/acc_r", setReport_ACC_R);
              osc_rx.dispatch("report/acc_p", setReport_ACC_P);
              osc_rx.dispatch("report/gyr_r", setReport_GYR_R);
              osc_rx.dispatch("report/gyr_p", setReport_GYR_P);

              // Dispatch for selecting animations
              osc_rx.dispatch("anim/power", setAnimation_Power);
              osc_rx.dispatch("anim/sparkle", setAnimation_Sparkle);


          }
          else if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) SERIAL_PRINTLN2("Tx response - WHY???");
      }
      else { SERIAL_PRINTLN2('xbee weird response type'); } // Not something we were expecting
    }
    else { SERIAL_PRINTLN(F("No incoming xBee messages")); }

    osc_rx.empty();
//    SERIAL_PRINTLN2();
}


void printOSCMessage( OSCMessage &_msg , byte _msgLength ) {
  // Print the OSC Address
  char addr[32];
  SERIAL_PRINT2("OSC RX Address = ");
  unsigned int addr_length = _msg.getAddress(addr, 0, _msgLength);
  // Is this loop necesary for printing? How can I print a char array without looping through it?
  for ( int i = 0; i < addr_length; i++ ) SERIAL_PRINT2(addr[i]);
  SERIAL_PRINTLN2();

  // Print the OSC contents
  // Get the number of data entries
  uint8_t numData = _msg.size();
  SERIAL_PRINT2("Num OSC data = "); SERIAL_PRINTLN2(numData);
  for ( uint8_t dataIndex = 0; dataIndex < numData; dataIndex++ ){
    // Print the data at this address...
    char dataType = _msg.getType( dataIndex );
    switch (dataType) {

      case 'i': {
        SERIAL_PRINT2("Integer found: ");
        SERIAL_PRINTLN2( _msg.getInt(dataIndex) );
        break;
      }

      case 'f': {
        SERIAL_PRINT2("Float found: ");
        SERIAL_PRINTLN2( _msg.getFloat(dataIndex) );
        break;
      }

      case 's': {
        SERIAL_PRINT2("String found: ");
        char dataString[32];
        uint8_t stringLength = _msg.getString(dataIndex, dataString, 32);
        SERIAL_PRINTLN2(dataString); // Can print like this because the string is null terminated
        break;
      }

      default: {
        SERIAL_PRINTLN2("Unhandled OSC data type");
        break;
      }
    }
  }
  SERIAL_PRINTLN2();
}