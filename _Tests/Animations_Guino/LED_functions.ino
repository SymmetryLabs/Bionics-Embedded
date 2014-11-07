

void LEDsetup() {

    // For safety (to prevent too high of a power draw), the test case defaults to
    // setting brightness to 25% brightness
    LEDS.setBrightness(255);

    // Put ws2801 strip on the hardware SPI pins with a BGR ordering of rgb and limited to a 1Mhz data rate
    // LEDS.addLeds<WS2801, 11, 13, BGR, DATA_RATE_MHZ(1)>(leds, NUM_LEDS);
    LEDS.addLeds<WS2801, 11, 13, RGB, DATA_RATE_MHZ(1)>(leds, NUM_LEDS);

    lastAnimate = millis();

}


void LEDrun( unsigned long _deltaMs ) {

    static unsigned long lastTrigger;
    if ( millis() - lastTrigger > 100 ) animations[currentAnimation]->level_Parameter.setPercent(0);
    if ( millis() - lastTrigger > 1000 ) {
        animations[currentAnimation]->level_Parameter.setPercent(float(random(0, 255))/255.);
        sparkle.trigger();
        lastTrigger = millis();
    }


    // Run animation-specific code
    switch ( currentAnimation ) {

        case SPARKLE:
        {
            // Trigger if percent high enough
            // sparkle.trigger();
            break;
        }

        case POWER:
        {
            break;
        }
        default:
            SERIAL_PRINTLN("ERROR!!! Running an unsupported animation from LEDrun()");
    }

    animations[currentAnimation]->draw( _deltaMs );
    LEDS.show();
}
