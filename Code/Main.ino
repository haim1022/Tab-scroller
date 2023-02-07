#include <Keyboard.h>

#define switchPin 2
#define rotaryDataPin 3
#define rotaryCLKPin 6
#define ctrlTabMode 0
#define altTabMode 1
#define Clockwise 1
#define counterClockwise 2
#define BUTTON_DEBOUNCE_TIME 50 //ms
#define SCROLL_PRESS_TIMEOUT 1500 //ms

byte rotaryAction(); //Returns rotary action
bool buttonPress(); //Returns true if pressed

bool scrollerMode = altTabMode;
bool lastCLKPinState;
byte lastRotDir = 0;
unsigned long lastButtonPress = 0, lastKeyboardPress = 0;

void setup()
{
  Serial.begin(2000000);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(rotaryDataPin, INPUT);
  pinMode(rotaryCLKPin, INPUT);
  lastCLKPinState = digitalRead(rotaryCLKPin);
}

void loop()
{
  bool buttonState = buttonPress();
  if(buttonState && (millis() - lastKeyboardPress > SCROLL_PRESS_TIMEOUT))
    scrollerMode = !scrollerMode;
  else if(buttonState && (millis() - lastKeyboardPress < SCROLL_PRESS_TIMEOUT))
    Keyboard.releaseAll();
  byte rotDir = rotaryAction();
  if(rotDir)
  {
    if(millis() - lastKeyboardPress > SCROLL_PRESS_TIMEOUT)
    {
      lastKeyboardPress = millis();
      if(scrollerMode == altTabMode)
        Keyboard.press(KEY_LEFT_ALT);
      else
        Keyboard.press(KEY_LEFT_CTRL);
    }
  }
  if(rotDir == Clockwise)
  {
    lastKeyboardPress = millis();
    Keyboard.press(KEY_TAB);
    Keyboard.release(KEY_TAB);
  }
  else if(rotDir == counterClockwise)
  {
    lastKeyboardPress = millis();
    Keyboard.press(KEY_LEFT_SHIFT);
    Keyboard.press(KEY_TAB);
    Keyboard.release(KEY_LEFT_SHIFT);
    Keyboard.release(KEY_TAB);
  }
  if(millis() - lastKeyboardPress > SCROLL_PRESS_TIMEOUT)
    Keyboard.releaseAll();
}

byte rotaryAction()
{
  byte result = NULL;
  bool clkPinState = digitalRead(rotaryCLKPin);
  bool dataPinState = digitalRead(rotaryDataPin);
  if(clkPinState != lastCLKPinState && clkPinState == HIGH)
  {
    if(dataPinState != clkPinState)
      result = counterClockwise;
    else
      result = Clockwise;
  }
  lastCLKPinState = clkPinState;
  //Debounce the rotary encoder
  delay(1);
  return result;
}

bool buttonPress()
{
  bool result = NULL;
  bool btnState = digitalRead(switchPin);
  if(btnState == LOW)
  {
    if(millis() - lastButtonPress > BUTTON_DEBOUNCE_TIME)
      result = true;
    lastButtonPress = millis();
  }
  return result;
}
