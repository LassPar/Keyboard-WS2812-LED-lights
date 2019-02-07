#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

Adafruit_NeoPixel strip = Adafruit_NeoPixel(19, 0, NEO_GRB + NEO_KHZ800);

int button_pin = 4;
int numlock = 3;


boolean buttonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
boolean lastState = HIGH;
int button_timer_counter = 0;

unsigned long last_update = 0;
unsigned long updatetime = 0;

int valomodi = 3;
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

void setup() {
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(numlock, INPUT_PULLUP);
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code


  strip.begin();
  /*
  for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, 255,255,255);
    }
    */
  strip.show(); // Initialize all pixels to 'off'
  delay(10);
}

void loop() 
{
  int nappi = button();
  switch(nappi)
  {
    case 0:
      break;
    case 1:
      valomodi += 1;
      if (valomodi >= 9) {valomodi = 1;}
      break;
    case 2:
      kirkkaus +=85;
      if (kirkkaus >= 256)
      {
        kirkkaus = 0;
      }
      strip.setBrightness(kirkkaus);
      break;
  }
  if (millis() - last_update >= updatetime || nappi != 0)
   
  switch(valomodi)
  {
    case 1: 
      for(int i=0; i<strip.numPixels(); i++) {strip.setPixelColor(i, 0,0,0);} // OFF
      strip.show();
      break;
    case 2: 
      for(int i=0; i<strip.numPixels(); i++) {strip.setPixelColor(i, 255,0,0);} // RED
      strip.show();
      break;
    case 3: 
      for(int i=0; i<strip.numPixels(); i++) {strip.setPixelColor(i, 255,100,0);} // YELLOW
      strip.show();
      break;
    case 4: 
      for(int i=0; i<strip.numPixels(); i++) {strip.setPixelColor(i, 0,255,0);} // GREEN
      strip.show();
      break;
    case 5: 
      for(int i=0; i<strip.numPixels(); i++) {strip.setPixelColor(i, 0,255,255);} // SYAN
      strip.show();
      break;
    case 6: 
      for(int i=0; i<strip.numPixels(); i++) {strip.setPixelColor(i, 0,0,255);} // BLUE
      strip.show();
      break;
    case 7: 
      for(int i=0; i<strip.numPixels(); i++) {strip.setPixelColor(i, 255,0,255);} // VIOLET
      strip.show();
      break;
    case 8: 
      for(int i=0; i<strip.numPixels(); i++) {strip.setPixelColor(i, 255,255,255);} // WHiTE
      strip.show();
      break;
  }
}


