#define ANALOG_MIN 0
#define ANALOG_MAX 255

const int ledPin = 13;
const int redPin = 11;
const int bluePin = 10;
const int greenPin = 9;

const int redInputPin = A0;
const int greenInputPin = A1;
const int blueInputPin = A2;

const int buttonPin = 7;

//Setup for debounce delay, first time implementing it in Arduino, so I copied it from the sample here: http://www.arduino.cc/en/Tutorial/Debounce
int buttonState;
int ledState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers


void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  pinMode(redInputPin, INPUT);
  pinMode(greenInputPin, INPUT);
  pinMode(blueInputPin, INPUT);
  
  pinMode(buttonPin, INPUT);
  
  Serial.begin(9600);
}

void loop()
{
  
  /*for(int r = 0; r < ANALOG_MAX; r+=10){
  	for(int g = 0; g < ANALOG_MAX; g+=10){
      for(int b = 0; b < ANALOG_MAX; b+=10){
		set_rgb_led(r, g, b);
      }
  	}
  }*/
 
  
  
  /*
  set_rgb_led(255, 0, 0);
  set_rgb_led(0, 255, 0);
  set_rgb_led(0, 0, 255);
  
  set_rgb_led(52, 235, 168); // a pretty blue-green 
  */
  
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }

  // set the LED:
  digitalWrite(ledPin, ledState);

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
  
  /*
  if(digitalRead(buttonPin) == LOW){
    digitalWrite(ledPin, HIGH);
  }
  else{
    digitalWrite(ledPin, LOW);
  }
  delay(1000);
  */
  
  
  //analog read returns a value between 0 and 1023, so we need to scale it before passing to rgb function
  int r = analogRead(redInputPin);
  int g = analogRead(greenInputPin);
  int b = analogRead(blueInputPin);
 	
  //could use constrain and map functions but dividing by 4 is close enough
  r/=4;
  g/=4;
  b/=4;
  
  set_rgb_led(r, g, b);
}

void set_rgb_led(int red, int green, int blue){
  if(in_analog_range(red) && in_analog_range(green) && in_analog_range(blue)){
  	//ints are valid analog values, so set the RGB LED
    analogWrite(redPin, red);
    analogWrite(greenPin, green);
    analogWrite(bluePin, blue);
    
    delay(1000);
  }
  else{
    //ints are out of analog range, so toggle alert LED
    if(digitalRead(ledPin) == HIGH){
      digitalWrite(ledPin, LOW);
    }
    else{
      digitalWrite(ledPin, HIGH);
    }
  }  
}

bool in_analog_range(int num){
  if((num < ANALOG_MIN) || (num > ANALOG_MAX)){
    Serial.write("Error, input out of range");
  	return false;
  }
  
  return true;
}
