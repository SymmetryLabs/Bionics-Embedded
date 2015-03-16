// ================================================================
// ===                       LED SETUP                          ===
// ================================================================

void LEDsetup() {

    // For safety (to prevent too high of a power draw), the test case defaults to
    // setting brightness to 25% brightness
    LEDS.setBrightness(255);

    // Put ws2801 strip on the hardware SPI pins with a BGR ordering of rgb and limited to a 1Mhz data rate
    LEDS.addLeds<WS2801, 11, 13, RGB, DATA_RATE_MHZ(1)>(leds, NUM_LEDS);
    // LEDS.addLeds<WS2801, 11, 13, BGR, DATA_RATE_MHZ(1)>(leds, NUM_LEDS);

    lastAnimate = millis();

}




void modelRun( unsigned long _deltaMs ) {

    magnitude = aaReal.getMagnitude();
//  float magnitudePercent = 2.50*( magnitude-1000.) / (56599.-1000.); // Map from 0-56599.9 -> MIN-MAX SparkleNumber
    magnitudePercent = ( magnitude-1000.00 ) / (30000.00); // Map from 0-56599.9 -> MIN-MAX SparkleNumber
    if ( magnitudePercent < 0 ) magnitudePercent = 0;
    else if ( magnitudePercent > 1 ) magnitudePercent = 1;
    SERIAL_PRINT("magPercent "); SERIAL_PRINTLN(magnitudePercent);

    rollLPF = rollLPF * rollLPFalpha + ypr[2]*(1-rollLPFalpha);
    SERIAL_PRINT("roll LPF "); SERIAL_PRINTLN(rollLPF);
    rollPercentP = rollLPF/(M_PI/4) + 0.5;

    pitchLPF = rollLPF * pitchLPFalpha + ypr[1]*(1-pitchLPFalpha);
    SERIAL_PRINT("pitch LPF "); SERIAL_PRINTLN(pitchLPF);
    pitchPercentP = pitchLPF/(M_PI/4) + 0.5;
}




// ================================================================
// ===                         LED RUN                          ===
// ================================================================

void LEDrun() {

    // Calculate deltaTime since last animate (animations are time dependent)
    // This allows for non-fixed framerate running - the Teensy does its best
    unsigned long deltaMs = millis() - lastAnimate;
    SERIAL_PRINT("dT Animation "); SERIAL_PRINTLN(deltaMs);
  
    modelRun(deltaMs);

//  animations[currentAnimation]->hue_Parameter.setPercent(rollPercentP);

    // Run animation-specific code
    switch ( currentAnimation ) {

        case SPARKLE:
        {
            // animations[currentAnimation]->level_Parameter.setPercent(magnitudePercent);
            animations[currentAnimation]->hue_Parameter.setPercent(huePercent);

            // Trigger if percent high enough
            if ( magnitudePercent > 0.2) {
                animations[currentAnimation]->level_Parameter.setPercent(magnitudePercent);
                sparkle.trigger(); // Sooooo ghetto
            }
            break;
        }

        case POWER:
        {
            animations[currentAnimation]->hue_Parameter.setPercent(huePercent);
            animations[currentAnimation]->level_Parameter.setPercent(magnitudePercent);
            break;
        }

        /*case DUALPOWER:
        {
            SERIAL_PRINT2("EQ[0] = "); SERIAL_PRINTLN2(eq[0]);
            SERIAL_PRINT2("EQ[1] = "); SERIAL_PRINTLN2(eq[1]);
            dualpower.level1_Parameter.setPercent(eq[0]);
            dualpower.level2_Parameter.setPercent(eq[1]);
            // Test code for pattern - seems to work fine
            // dualpower.level1_Parameter.setPercent( float(random(0,10)) / 10. );
            // dualpower.level2_Parameter.setPercent( float(random(0,10)) / 10. );
            break;
        }

        case FIRE:
        {
            random16_add_entropy( random());
            break;
        }

        default:
        {
            SERIAL_PRINTLN("ERROR!!! Running an unsupported animation from LEDrun()");
        }*/
    }


    animations[currentAnimation]->draw( deltaMs );

    LEDS.show();

    lastAnimate = millis();
}
