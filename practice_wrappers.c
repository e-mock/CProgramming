#include <avr/io.h>
#include <util/delay.h>

#define MS_DELAY 50

// const int ledPin = PORTB5;//;13;
// const int redPin = PORTB3;//11;
// const int bluePin = PORTB2;//10;
// const int greenPin = PORTB1;//9;

// const int redInputPin = PORTC0;//A0;
// const int greenInputPin = PORTC1;//A1;
// const int blueInputPin = PORTC2;//A2;

const int buttonPin = PORTD7; //7;



//by experimentation, HIGH is 128 and LOW is 0
const int HIGHin = 128;
const int LOWin = 0;

//Setup for debounce delay, first time implementing it in Arduino, so I copied it from the sample here: http://www.arduino.cc/en/Tutorial/Debounce
int buttonState;
int ledState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50;   // the debounce time; increase if the output flickers

enum portID {B, C, D};

bool isRaining = false;



int main(void)
{
  //enable interrupt SREG(I) = 1
  SREG |= _BV(7);
  
  //Set EICRA register to generate interrupt on any change
  // ISC01 = 0, ISC00 = 1 for any logical change on INT0 to generate an interrupt request
  
  EICRA &= ~_BV(ISC11);
  EICRA |= _BV(ISC10);
  
  // Enable interrupts on INT0 by setting EIMSK(INT0) = 1
  EIMSK |= _BV(INT0);
  
  //EIFR(INTF0) has the flag
  // I think EXT_INT0 does too since its ithe IRQ0 handler
  //EIFR;
  
  //interrupt(_BV(INTF0), toggle_rain_interrupt);
  
  
  
    /*Set to one the fifth bit of DDRB to one
    **Set digital pin 13 to output mode */
    DDRB |= _BV(DDB5);
  
  	DDRD |= _BV(DDD5);



    
    //buttonPin setup - see https://arduino.stackexchange.com/questions/75927/i-am-trying-to-read-input-from-5th-pin-of-port-b 
    //Set digital pin 7 to input mode
    DDRD |= ~_BV(DDD7);
    // disable internal pull up
    PORTD |= ~_BV(DDD7);


    //ADC setup 
    //datasheet 24.9.1 
    //REFS1, REFS0 = 0, 1 means AVcc with external capacitor at AREF pin
    //REFS1 = 0
    ADMUX |= ~_BV(REFS1);
    //REFS0 = 1
    ADMUX |= _BV(REFS0);

    //enable ADC by making ADEN bit = 1
    ADCSRA |= _BV(ADEN);

    //enable ADIE bit in ADCSRA and I-bit in SREG so we can get ADC interrupts
    ADCSRA |= _BV(ADIE);
    //SREG |= _BV(I);

    uint16_t analog_reading;
    int buttonState;

    while (1)
    {
      
      //int r1 = EXT_INT0;
      //EIFR;
      //int r2 = _BV(INTF0);
      
      //clear the flag?
      //EIFR &= ~_BV(INTF0);
      
      analog_reading = readAnalog(PORTC0);
      
      
      if(analog_reading > 512){
		setDigital(0, PORTB5, true);
      	setDigital(0, PORTB5, false);
      }
      else{
      	setDigital(2, PORTD5, true);
      	setDigital(2, PORTD5, false);
      }
    }
}

void toggle_rain_interrupt(void) //interrupt EXT_INT0
  {
    isRaining = !isRaining;
  }
  


void setDigital(int port, int pin, bool value){
  //TODO implement in bare metal c
  switch(port){
    case 0:
      if(value == true){
        // digitalWrite(pin, HIGH);
        PORTB |= _BV(pin);
      }
      else{
        //digitalWrite(pin, LOW);
        PORTB &= ~_BV(pin);
      }
      break;
    case 2:
      if(value == true){
        // digitalWrite(pin, HIGH);
        PORTD |= _BV(pin);
      }
      else{
        //digitalWrite(pin, LOW);
        PORTD &= ~_BV(pin);
      }
      break;
      //TODO fill in
  }

  _delay_ms(500);
}

void body(){
 // analog_reading = readAnalog(PORTC0);



        // buttonState = PIND & _BV(PD7);

//        if (analog_reading >= 512)
//        {
            
            /*Set to one the fifth bit of PORTB to one
        **Set to HIGH the pin 13 */
//            PORTB |= _BV(PORTB5);
 //       }
 //       else
 //       {
            /*Set to zero the fifth bit of PORTB
        **Set to LOW the pin 13 */
 //           PORTB &= ~_BV(PORTB5);            
 //       }

        /*Wait 3000 ms */
 //     	int ports = PORTB;
 //           _delay_ms(MS_DELAY);
}


uint16_t readAnalog(int pin){
    uint8_t channel_num = 0;
    switch (pin)
    {
    case PORTC0:
        channel_num = 0;
        break;
    case PORTC1:
        channel_num = 1;
        break;
    case PORTC2:
        channel_num = 2;
        break;
    default:
        channel_num = 0;
        break;
    }

    //Select channel to read
    ADMUX |= channel_num;
    //write logical zero to Power Reduction ADC bit, PRADC
    PRR |= ~_BV(PRADC);
    //write logical one to the ADC Start Conversion bit ADSC
    ADCSRA |= _BV(ADSC);
      

    //wait for conversion to complete
    // ADIF = 1 when complete
    uint8_t complete_mask = 0 | _BV(ADIF);

    // ADCSRA & complete_mask is 0 while we are waiting, 1 in ADIF place when complete
    while(ADCSRA & complete_mask == 0){
        //do nothing, wait
    }

    //read upper and lower bytes, must read ADCL first for 10 bit result
    //both are 8 bits, hoping that first parts get filled with zero
  	uint8_t adc_high = ADCH;
  	uint8_t adc_low = ADCL;
    
    uint16_t reading = (adc_high << 8) | adc_low;

    return reading;
}
