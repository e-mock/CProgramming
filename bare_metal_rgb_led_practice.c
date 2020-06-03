#include <avr/io.h>
#include <util/delay.h>

#define MS_DELAY 3000

// const int ledPin = PORTB5;//;13;
// const int redPin = PORTB3;//11;
// const int bluePin = PORTB2;//10;
// const int greenPin = PORTB1;//9;

// const int redInputPin = PORTC0;//A0;
// const int greenInputPin = PORTC1;//A1;
// const int blueInputPin = PORTC2;//A2;

const int buttonPin = PORTD7; //7;


//by experimentation, HIGH is 128 and LOW is 0
const int HIGH = 128;
const int LOW = 0;

//Setup for debounce delay, first time implementing it in Arduino, so I copied it from the sample here: http://www.arduino.cc/en/Tutorial/Debounce
int buttonState;
int ledState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50;   // the debounce time; increase if the output flickers

int main(void)
{
    /*Set to one the fifth bit of DDRB to one
    **Set digital pin 13 to output mode */
    DDRB |= _BV(DDB5);

    
    //buttonPin setup - see https://arduino.stackexchange.com/questions/75927/i-am-trying-to-read-input-from-5th-pin-of-port-b 
    //Set digital pin 7 to input mode
    DDRD |= ~_BV(DDD7);
    // disable internal pull up
    PORTD |= ~_BV(DDD7);

    while (1)
    {

        int buttonState = PIND & _BV(PD7);

        if (buttonState == HIGH)
        {

            /*Set to one the fifth bit of PORTB to one
        **Set to HIGH the pin 13 */
            PORTB |= _BV(PORTB5);
        }
        else
        {
            /*Set to zero the fifth bit of PORTB
        **Set to LOW the pin 13 */
            PORTB &= ~_BV(PORTB5);

            /*Wait 3000 ms */
            _delay_ms(MS_DELAY);
        }
    }
}