#include <FastLED.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NUM_LEDS 19
#define DATA_PIN 0

CRGB leds[NUM_LEDS];

int button_pin = 4;
int numlock = 3;

boolean buttonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
boolean lastState = HIGH;
int button_timer_counter = 0;

unsigned long last_update = 0;
unsigned long updatetime = 100;

int valomodi = 0;
int kirkkaus = 255;

//******************************************* BUT ***********************************
byte button()
{
  buttonState = digitalRead(button_pin);
  if (millis() - lastDebounceTime > debounceDelay) 
  {
    lastDebounceTime = millis();
    if (buttonState == LOW && lastState == HIGH) 
    {

      button_timer_counter = 0;
      lastState = LOW;
      lastDebounceTime = millis();
    }
    else if (buttonState == LOW && lastState == LOW) 
    {
      button_timer_counter +=1 ;
      lastState = LOW;
    }
    else if (buttonState == HIGH && lastState == LOW && button_timer_counter < 20) 
    {
      lastState = HIGH;
      //TODO
      return 1;
    }
    if (button_timer_counter == 20)
    {
      //button_timer_counter = 0;
      //TODO
      return 2;
    }
    if (buttonState == HIGH) 
    {
      button_timer_counter = 0;
      lastState = HIGH;
    }
  }
  return 0;
}
//******************************************* BUT ***********************************


bool power_switch(int ero)
{
  int numlockstate = analogRead(numlock);
  if (numlockstate <= ero)
  {
    return false;
  }
  else
  {
    return true;
  }
}
void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(numlock, INPUT);
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
}

int ero = 200;

void loop() 
{
  //if (power_switch(ero) == true)
  {
    int nappi = button();
    switch(nappi)
    {
      case 0:
        break;
      case 1:
        valomodi += 16;
        if (valomodi >= 256) {valomodi = 0;}
        break;
      case 2:
        kirkkaus +=85;
        if (kirkkaus >= 256)
        {
          kirkkaus = 0;
        }
        break;
    }
    if (millis() - last_update >= updatetime || nappi != 0)
    {
      ero +=50;
      last_update = millis();
      
      for(int i=0; i<5; i++) {leds[i].setHSV(analogRead(numlock), 255, kirkkaus);}
      for(int i=5; i<10; i++) {leds[i].setHSV(150,255, kirkkaus);}
      FastLED.show(); 
    }
  }
}


