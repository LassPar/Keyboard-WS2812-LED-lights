#include <EEPROM.h>
#include <FastLED.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// LED nauha
#define NUM_LEDS 19
#define DATA_PIN 0
int LED_NUMBERS[19] = { 0, 18, 1, 17, 2, 16, 3, 15, 4, 14, 5, 13, 6, 12, 7, 11, 8, 10, 9 };
CRGB leds[NUM_LEDS];
int valomodi = 0;
int kirkkaus_list[4] = { 0, 85, 170, 255 };
byte kirkkaus_indeksi = 3;

// button
int button_pin = 4;
boolean buttonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
boolean lastState = HIGH;
int button_timer_counter = 0;
int long_press_time = 10; // *debouncedelay = aika millisekunneissa
bool last_long = false;



// värit
unsigned long last_update = 0;
unsigned long updatetime = 10000;
bool eka_kierros = true;

// sateenkaari eriväri
int kiertolisays = 0;
unsigned long kiertoaika_eri = 10;
unsigned long last_kierto;

// sateenkaari samaväri
unsigned long kiertoaika_sama = 120;
byte kiertovari = 0;

// tallennus
unsigned long last_storetime;
unsigned long storetime = 10000;

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
      button_timer_counter += 1;
      lastState = LOW;
    }
    else if (buttonState == HIGH && lastState == LOW && button_timer_counter < long_press_time && last_long != true)
    {
      lastState = HIGH;
      //TODO
      return 1; // LYHYT PAINALLUS
    }
    if (button_timer_counter == long_press_time)
    {
      button_timer_counter = 0;
      last_long = true;
      //TODO
      return 2; // PITKÄ PAINALLUS
    }
    if (buttonState == HIGH)
    {
      button_timer_counter = 0;
      lastState = HIGH;
      last_long = false;
    }
  }
  return 0;
}
//******************************************* BUT ***********************************

void store_E()
{
  // jos asetukset muuttuneet, tallenetaan asetukset ja näytetään animaatio

  if (valomodi != (int)(EEPROM.read(0) + EEPROM.read(1)) || kirkkaus_indeksi != (int)EEPROM.read(2))
  {
    byte val_1 = 0;
    byte val_2 = 0;

    if (valomodi < 256) // jos alle tavun mittainen tallennettava (<256)
    {
      val_1 = valomodi;
      val_2 = 0;
    }
    else // Jos yli tavun mittainen tallennettava (>255)
    {
      val_1 = 255;
      val_2 = valomodi - 255;
    }
    // Tallenetaan asetukset
    EEPROM.update(0, val_1);
    EEPROM.update(1, val_2);
    EEPROM.update(2, kirkkaus_indeksi);

    // animaatio
    for (int j = 0; j<NUM_LEDS; j++) { leds[j].setHSV(0, 0, 0); } //tyhjennä
    delay(50);
    for (int i = 0; i<NUM_LEDS; i++)
    {
      for (int j = 0; j<NUM_LEDS; j++) { leds[j].setHSV(0, 0, 0); } //tyhjennä
      leds[LED_NUMBERS[i]].setHSV(0, 0, 255); // i:nnes ledi valkoisiksi
      leds[LED_NUMBERS[constrain(i - 1, 0, NUM_LEDS - 1)]].setHSV(0, 0, 255); // i-1:nnes ledi valkoisiksi
      FastLED.show();
      delay(20);
    }
    for (int j = 0; j<NUM_LEDS; j++) { leds[j].setHSV(0, 0, 0); } //tyhjennä
    delay(50);
  }
}

int read_E()
{
  // Luetaan asetukset
  valomodi = (int)(EEPROM.read(0) + EEPROM.read(1));
  kirkkaus_indeksi = (int)EEPROM.read(2);
}

void setup()
{
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  pinMode(button_pin, INPUT_PULLUP);

  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  delay(100);
  read_E();
  eka_kierros = true;
}



void loop()
{
  if (millis() - last_storetime >= storetime) // yritetään tallentaa asetukset joka storetime:s sekunti
  {
    last_storetime = millis();
    store_E();
    eka_kierros = true;
  }

  int nappi = button();
  switch (nappi)
  {
  case 0: // ei painettu
    break;
  case 1: // lyhyt painallus muuttaa väriä
    valomodi += 16;
    if (valomodi >= 304) { valomodi = 0; }
    last_storetime = millis();
    break;
  case 2: // pitkä painallus muuttaa kirkkautta
    kirkkaus_indeksi += 1;
    if (kirkkaus_indeksi >= 4 || kirkkaus_indeksi < 0)
    {
      kirkkaus_indeksi = 0;
    }
    last_storetime = millis();
    break;
  }

  if (valomodi < 256) // Kiinteät värit
  {
    if (millis() - last_update >= updatetime || nappi != 0 || eka_kierros == true)
    {
      eka_kierros = false;
      last_update = millis();
      for (int i = 0; i<NUM_LEDS; i++) { leds[i].setHSV(valomodi, 255, kirkkaus_list[kirkkaus_indeksi]); }
      FastLED.show();
    }
  }

  else if (valomodi == 256) // valkonen
  {
    if (millis() - last_update >= updatetime || nappi != 0 || eka_kierros == true)
    {
      eka_kierros = false;
      last_update = millis();

      for (int i = 0; i<NUM_LEDS; i++) { leds[i].setHSV(0, 0, kirkkaus_list[kirkkaus_indeksi]); }
      FastLED.show();
    }
  }

  else if (valomodi == 272) // sateenkaari eriväri
  {
    if (millis() - last_kierto >= kiertoaika_eri)
    {
      last_kierto = millis();
      int vari = 0;
      int lisays = 128;
      for (int i = 0; i < NUM_LEDS; i++)
      {
        leds[LED_NUMBERS[i]].setHSV((vari / 10 + kiertolisays) % 256, 255, kirkkaus_list[kirkkaus_indeksi]);
        vari += lisays;
      }
      kiertolisays += 1;
      if (kiertolisays >= 256)
      {
        kiertolisays = 0;
      }
    }
    FastLED.show();
  }

  else if (valomodi == 288) // sateenkaari samaväri
  {
    if (millis() - last_kierto >= kiertoaika_sama)
    {
      last_kierto = millis();
      kiertovari += 1;
      for (int i = 0; i<NUM_LEDS; i++) { leds[i].setHSV(kiertovari, 255, kirkkaus_list[kirkkaus_indeksi]); }
    }
    FastLED.show();
  }
}
