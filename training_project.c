
//Basic setup from https://create.arduino.cc/projecthub/milanistef/introduction-to-bare-metal-programming-in-arduino-uno-f3e2b4

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint-gcc.h>
#include <avr/interrupt.h>


//TODO change pins to port numbers

//interrupts: arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
// comparator interrupt https://forum.arduino.cc/index.php?topic=149840.0, https://www.youtube.com/watch?v=QdJrJXQaiy8, http://www.gammon.com.au/forum/?id=11916,


const int tempInPin =  PORTC2; //A2;
const int tempOutPin = PORTC0; //A0;
// highest temp corresponds to 358, lowest temp corresponds to 20

const int greenLedPin = PORTD7; //7
const int redLedPin = PORTB0; //8
const int builtInLedPin = PORTB5; //13

//https://vetco.net/products/rain-sensor-for-arduino?gclid=Cj0KCQjww_f2BRC-ARIsAP3zarGmMg3D0nxuw0fdA1LHU2nxvRJa2JZ81LpUyxbbfHLFhP5XPMcqfJ0aAt3hEALw_wcB
// rain sensor can either return an analog value or digital signal

//the only interrupt pins on the uno are UNO pins 2 and 3 - https://arduino.stackexchange.com/questions/1784/how-many-interrupt-pins-can-an-uno-handle#:~:text=There%20are%20only%20two%20external,edges%2C%20or%20on%20low%20level.
// followed this tutorial to set up slide switch https://www.instructables.com/id/Slide-Switch-With-Arduino-Uno-R3/
const int rainSwitchPin = PORTD2; //2;
const int nightSwitchPin = PORTD4; //4, nothing connected to 4

const int rainPin = PORTC1; //A1;

const int windowPosPinBare = PORTB2; //10;
const int windowBlindsPinBare = PORTB1; //9;

const int windowPosPin = 10;
const int windowBlindsPin = 9;


const int lightPin = PORTC3; //A3;

const int lockPin = PORTB4; //12;

const int tempDiffLedPin = PORTD5; //5;


// END PIN DECLARATIONS

const uint16_t tolerance = 10; //TODO Oliver figure out the int equilavent of 1deg Fahrenheit

const uint8_t WINDOW_CLOSED = 0;
const uint8_t WINDOW_OPEN = 180;
const uint8_t WINDOW_NIGHT = 0;

const uint16_t NIGHT_LIGHT_LEVEL = 300;
const uint16_t BRIGHT_LIGHT_LEVEL = 600;

const uint8_t BLINDS_CLOSED = 0;
const uint8_t BLINDS_OPEN = 180;

const uint8_t ANALOG_MIN = 0;
const uint8_t ANALOG_MAX = 255;

uint8_t lastSetWindowAngle;
uint8_t lastSetBlindsAngle;

//suggested to declare as volatile for use with interrupt
volatile bool isRaining = false;
volatile uint8_t analogValueA = 50;

#define MS_DELAY 3000

#define B 0
#define C 1
#define D 2

void analogReadSetup();
uint16_t readAnalog(int pin);
bool readDigital(int port, int pin);
void setDigital(int port, int pin, bool value);
void setUpAnalogWrite();
void setWindowPosition(uint16_t insideReading, uint16_t outsideReading, uint16_t lightReading, bool nightMode);
void setWindowAngle(uint8_t angle);
void setBlindsAngle(uint8_t angle);
void indicateUnexpectedError();
void indicateExpectedError();
void indicateSuccess();
void lockWindow();
void unlockWindow();

int main(void)
{
	bool nightMode = false;
	uint16_t insideReading = 0;
	uint16_t outsideReading = 0;
	uint16_t lightReading = 0;

	//configure rainSwitchPin (on port D, PD2) as digital input pin without internal pull up
	DDRD &= (0 << DDD2);
	PORTD &= (0 << DDD2);

	//configure nightSwitchPin (on port D, PD4) as digital input pin without internal pull up
	DDRD &= (0 << DDD4);
	PORTD &= (0 << DDD4);

	//configure green led (on port D, PD7) as digital output pin
	DDRD |= (1 << DDD7);
	//configure red led (on port B, PB0) as digital output pin
	DDRB |= (1 << DDB0);
	//configure built-in led (on port B, PB5) as digital output pin
	DDRB |= (1 << DDB5);
	
	//configure window servo (on port B, PB1) as digital output pin
	DDRB |= (1 << DDB1);
	//configure blinds servo (on port B, PB2) as digital output pin
	DDRB |= (1 << DDB2);
	
	//configure window lock light (on port B, PB4) as digital output pin
	DDRB |= (1 << DDB4);

	analogReadSetup();
	setUpAnalogWrite();
	
	while (1)
	{
		nightMode = readDigital(D, PORTD4);
		
		insideReading = readAnalog(tempInPin);
		outsideReading = readAnalog(tempOutPin);
		lightReading = readAnalog(lightPin);

		setWindowPosition(insideReading, outsideReading, lightReading, nightMode);
	}
}

void analogReadSetup()
{
	//ADC setup
	//datasheet 24.9.1
	//REFS1 = 0 and REFS0 = 1 means AVcc with external capacitor at AREF bit
	//REFS1 = 0
	ADMUX &= (0 << REFS1);
	//REFS0 = 1
	ADMUX |= (1 << REFS0);
	
	//right adjust ADMUX(ADLAR) = 0
	ADMUX &= (0 << ADLAR);

	//enable ADC by making ADEN bit = 1
	ADCSRA |= (1 << ADEN);

	//enable ADIE bit in ADCSRA and I-bit in SREG so we can get ADC interrupts
	ADCSRA |= (1 << ADIE);
	//SREG |= (1 << I);
}

uint16_t readAnalog(int pin)
{
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
		case PORTC3:
		channel_num = 3;
		break;
		case PORTC4:
		channel_num = 4;
		break;
		case PORTC5:
		channel_num = 5;
		break;
		case PORTC6:
		channel_num = 6;
		break;
		default:
		indicateUnexpectedError();
		return (uint16_t) 0;
	}

	//Select channel to read
	ADMUX |= channel_num;
	//write logical zero to Power Reduction ADC bit, PRADC
	PRR |= (0 << PRADC);
	//write logical one to the ADC Start Conversion bit ADSC
	ADCSRA |= (1 << ADSC);

	//wait for conversion to complete
	// ADIF = 1 when complete
	uint8_t complete_mask = 0 | (1 << ADIF);

	// ADCSRA & complete_mask is 0 while we are waiting, 1 in ADIF place when complete
	while ((ADCSRA & complete_mask) == 0)
	{
		//do nothing, wait
	}

	//read upper and lower bytes, must read ADCL first for 10 bit result
	uint8_t adc_low = ADCL;
	uint8_t adc_high = ADCH;
	
	_delay_ms(10); //delay to space out readings
	//write logical zero to the ADC Start Conversion bit ADSC
	ADCSRA &= (0 << ADSC);
	uint16_t reading = (adc_high << 8) | adc_low;
	_delay_ms(10); //delay to space out readings
	return reading;
}

bool readDigital(int port, int pin)
{
	uint8_t buttonState = 0;
	switch (port)
	{
		case B: //PORTB
		buttonState = PINB & (1 << pin);
		break;
		case D: //PORTD
		buttonState = PIND & (1 << pin);
		break;
		default:
		//Error in readDigital port
		indicateUnexpectedError();
	}

	// masked buttonState = 0, means value of pin bit is 0
	if (buttonState == 0)
	{
		return false;
	}

	return true;
}

void setDigital(int port, int pin, bool value)
{
	switch (port)
	{
		case B:
		if (value == true)
		{
			// digitalWrite(pin, HIGH);
			PORTB |= (1 << pin);
		}
		else
		{
			//digitalWrite(pin, LOW);
			PORTB &= (0 << pin);
		}
		break;
		case D:
		if (value == true)
		{
			// digitalWrite(pin, HIGH);
			PORTD |= (1 << pin);
		}
		else
		{
			//digitalWrite(pin, LOW);
			PORTD &= (0 << pin);
		}
		break;
		default:
		//Error in setDigital port
		indicateUnexpectedError();
		return;
	}

	_delay_ms(10);
}

void setUpAnalogWrite(){
	//DDRB |= (1 << DDB2);
	
	// Fast PWM, clear OC1B on compare match, set at BOTTOM (non-inverting)
	//TCCR1B |= (1 << COM1B1); //COM1B1 = 1
	//TCCR1B &= (0 << COM1B0); //COM1B0 = 0
	
	//TCCR1B |= 1 << COM0B0;
	
	//TCCR1B |= (1 << WGM00);
	// TCCR1B |= (1 << WGM01);
	
	// TCCR1A |= 1 << CS00;
	
	
	DDRD |= 1 << PORTD5;
	DDRD |= 1 << PORTD6;
	
	TCCR0A |= 1 << COM0B0;
	TCCR0A |= 1 << WGM00;
	TCCR0A |= 1 << WGM01;
	TCCR0A |= 1 << WGM02;
	
	TIMSK0 |= (1<<TOIE0);
	
	OCR0A = 50;
	
	sei();
	
	TCCR0B |= (1 << CS00) | (1 <<CS02);
	
	
	// Parts inspired by/copied from  Servo.cpp Arduino library
	
	//    TCCR1A = 0;             // normal counting mode
	//    TCCR1B = (1 << CS11);     // set prescaler of 8
	//    TCNT1 = 0;              // clear the timer count
	
	//TIFR1 |= (1 << OCF1A);     // clear any pending interrupts;
	//  TIMSK1 |=  (1 << OCIE1A) ; // enable the output compare interrupt
	
	
	
}

ISR(TIMER0_OVF_vect){
	OCR0A = analogValueA;
	PORTD ^= (1 << PORTD6);
}

//Interrupt to toggle isRaining value when the rain detector state changes
ISR(PCINT0_vect){
	isRaining = ~isRaining;
}

void setWindowPosition(uint16_t insideReading, uint16_t outsideReading, uint16_t lightReading, bool nightMode)
{
	//isRaining is determined by interrupt, close the window all the way if it is raining
	if (isRaining)
	{
		setWindowAngle(WINDOW_CLOSED);
	}
	else if (nightMode && lightReading < NIGHT_LIGHT_LEVEL)
	{
		// light level indicated night time, so set window to night time position and lock
		setWindowAngle(WINDOW_NIGHT);
		if (WINDOW_NIGHT == WINDOW_CLOSED)
		{
			lockWindow();
		}
	}
	else if (lightReading > BRIGHT_LIGHT_LEVEL)
	{
		uint8_t angle = 165; //TODO figure out math
		setBlindsAngle(angle);
	}
	else if (insideReading > (outsideReading - tolerance))
	{
		//open window
		//(358-120)/180 = 1.82
		// 358/2 = 179
		// 20/2 = 10

		//TODO mess with this scaling
		uint8_t angle = (insideReading - outsideReading) / 2;

		// open the window
		setWindowAngle(angle);

		//close the blinds
		//TODO adjust to make sure that it works even in blinds open and angle are different scales
		//setBlindsAngle(BLINDS_OPEN - angle);
	}
	else if (outsideReading > (insideReading - tolerance))
	{
		//close window because it is more than tolerance hotter outside than inside

		uint8_t angle = WINDOW_CLOSED;

		setWindowAngle(angle);

	}
}

void setWindowAngle(uint8_t angle)
{

	/**if (angle == lastSetWindowAngle)
	{
	indicateSuccess;
	}
	else*/
	if (angle >= WINDOW_CLOSED && angle <= WINDOW_OPEN)
	{
		unlockWindow();
		indicateExpectedError();
		//windowPosServo.write(angle);
		
		analogValueA = angle;
		
		
		_delay_ms(10);
		lastSetWindowAngle = angle;

		while (lastSetWindowAngle != angle)
		{
			_delay_ms(10);
		}

		indicateSuccess();
	}
	else
	{
		//Serial.println("Invalid Window Position");
		indicateUnexpectedError();
	}
}

void setBlindsAngle(uint8_t angle)
{
	if (angle >= BLINDS_CLOSED && angle <= BLINDS_OPEN && angle != lastSetBlindsAngle)
	{
		//windowBlindsServo.write(angle);

		_delay_ms(10);
		lastSetBlindsAngle = angle;

		while (lastSetBlindsAngle != angle)
		{
			_delay_ms(10);
		}
	}
	else
	{
		//Serial.println("Invalid Blinds Position");
		indicateUnexpectedError();
	}
}

void indicateUnexpectedError()
{
	setDigital(B, builtInLedPin, false);
	_delay_ms(10);
}

void indicateExpectedError()
{
	setDigital(D, greenLedPin, false);
	_delay_ms(10);
	setDigital(B, redLedPin, true);
	_delay_ms(10);
}

void indicateSuccess()
{
	setDigital(B, redLedPin, false);
	_delay_ms(10);
	setDigital(D, greenLedPin, true);
	_delay_ms(10);
}

void lockWindow()
{
	setDigital(B, lockPin, true);
	_delay_ms(10);
}

void unlockWindow()
{
	setDigital(B, lockPin, false);
	_delay_ms(10);
}

