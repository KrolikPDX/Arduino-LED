/*

THIS IS THE OLD VERSION
NEW VERSION WILL REPLACE THIS ONE

*/
#include "FastLED.h"`
#include "IRremote.h" 

//What needs to be done:
// -Add a smooth transition...
// -Add more IR remote functions...
// -Add mode without ultra sonic sensor LEDS...

#define DATA_PIN 3
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 150 //150 is how much our strip has
#define receiver 2
#define trigPin 5
#define echoPin 4

///////////////////////////GLOBAL_VARS////////////////////////////////
int power = 0;    //default to off
int color = 0;    //default red starting color
int BRIGHTNESS = 255; //highest default brightness
int RED=255;      //red pigment (default starting)
int GREEN=0;      //green pigment
int BLUE=0;       //blue pigment
long duration;    //duration of pulse
int distance_cm;  //distance in cm
int distance_in;  //distance in inches
int distance;     //raw distance
int mode = 0;     //Mode 0 = regular | Mode 1 = ultrasonic
CRGB leds[NUM_LEDS];  //setup leds
IRrecv irrecv(receiver); //setup ir reciever
decode_results results;  //setup decoder

////////////////////////////Setup/////////////////////////////////////
void setup() 
{
  delay(1000); // initial delay of a few seconds is recommended
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
  FastLED.setBrightness(BRIGHTNESS);// global brightness
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);
  irrecv.enableIRIn();
    for (int i=0; i<NUM_LEDS; i++) //Turn off led strip
    {
      leds[i]=CRGB::Black;
      FastLED.show();
    }
}
///////////////////////////Main_Loop//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{
 delay(100);
 if (irrecv.decode(&results)) // Did we received an IR signal?
 {  
  translateIR(); 
  irrecv.resume(); //reopen for next value
  check_cmd();                    //do actions based on ir signal
 } 
 else if (power == 1) {distance = getdistance(); ultrasonic_leds();} //ELSE WE USE ULTRASONIC SENSOR
 else if (power == 0){clear_leds(); delay(100);}
} //END OF LOOP
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////ULTRASONIC_LEDS//////////////////////////////////////////
void ultrasonic_leds()
{
  if (distance < 25 && distance-4 > 0)
  {
    leds[distance] =   CRGB(RED, GREEN, BLUE);
    leds[distance+1] = CRGB(RED, GREEN, BLUE);
    leds[distance-1] = CRGB(RED, GREEN, BLUE);
    leds[distance+2] = CRGB(RED, GREEN, BLUE);
    leds[distance-2] = CRGB(RED, GREEN, BLUE);
    leds[distance-3] = CRGB(RED, GREEN, BLUE);
    leds[distance+3] = CRGB(RED, GREEN, BLUE);
    leds[distance-4] = CRGB(RED, GREEN, BLUE);
    leds[distance+4] = CRGB(RED, GREEN, BLUE);
    clear_specific();
    FastLED.show();
  }
  else
    clear_leds();
}
///////////////////CLEAR_SPECIFIC_LEDS_FOR_DISTANCE///////////////////////////////////
void clear_specific()
{
    for (int i=0; i<distance-5; i++) leds[i]=CRGB::Black; 
    for (int i=distance+5; i<NUM_LEDS; i++) leds[i]=CRGB::Black;
    FastLED.show(); //Used to clear front and behind LEDS of distance LEDS
}

/////////////////////////////////CLEAR_STRIP///////////////////////////////////
void clear_leds()
{
    for (int i=0; i<NUM_LEDS; i++) leds[i]=CRGB::Black; //Turn off led strip
    FastLED.show();
}

/////////////////////////GET_ULTRA_SONIC_DISTANCE///////////////////////////
int getdistance()
{
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);// Sets the trigPin on HIGH state for 10 micro seconds
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
// Calculating the distance
distance_cm = duration / 29 / 2;
distance_in = distance_cm * 0.393701 * 1.35; //*1.35 for LED distance
Serial.print("Distance: "); // Prints the distance on the Serial Monitor
Serial.println(distance_in);
return distance_in;
}

/////////////////////////EXECUTE_CMD_COMMAND/////////////////////////////////
int check_cmd () //Checks lastcmd and executes code according to that value
{
  if (results.value == 0xFF02FD) //Power button to turn led strip on or off
    {power_check(); }//power_function();}
  else if (results.value == 0xFF3AC5 && power == 1 || results.value == 0xFFBA45 && power == 1) //BRIGHTNESS+ or BRIGHTNESS-
    {bright_check(); FastLED.setBrightness(BRIGHTNESS); FastLED.show();}                       //Change LED brightness 
  else if (results.value == 0xFF1AE5) //NEXT button for next color
    {color_check();}
  else if (results.value == 0xFF827D && power == 1)
    {rand_color();}
}

////////////////POWER_FUNCTION///////////////////////////
int power_check ()                //Change power value
{
  if (power == 0) power = 1;
  else power = 0;
}
////////////////////////
int power_function() //Change the power variable and update strip...
{
  if (power == 1)      update_strip(); //Update strip to whatever the colors are preset to..
  else if (power == 0) clear_leds(); //Power off
    
}

////////////////////BRIGHTNESS_FUNCTION///////////////////////////////////
int bright_check() //Increase or decrease brightness variable...
{
  if (results.value == 0xFF3AC5)
  {
    if (BRIGHTNESS >= 180)
      BRIGHTNESS = 255;
    else if (BRIGHTNESS < 180)
      BRIGHTNESS = BRIGHTNESS+75;
  }
  else if (results.value == 0xFFBA45)
  {
    if (BRIGHTNESS <= 85)
      BRIGHTNESS = 10;
    else if (BRIGHTNESS > 85)
      BRIGHTNESS = BRIGHTNESS - 75;
  }
}

/////////////////////CHANGE_MAIN_COLORS////////////////////////////////
//Color = 0 = Red
//Color = 1 = Green
//Color = 2 = Blue
int color_check () //Change the color to either RED / GREEN / BLUE
{
  switch (color){ 
    case 0:RED=0;GREEN=255;color++;break;
    case 1:GREEN=0;BLUE=255;color++;break;
    case 2:BLUE=0;RED=255;color=0;break;}
}

//////////////////RANDOM_COLOR//////////////////////////////////
int rand_color() //Get random values for red / green / blue
{
  RED = random(0,256);
  GREEN = random(0,256);
  BLUE = random(0,256);
}

///////////////////UPDATE_STRIP///////////////////////////////////
int update_strip() //Updates the strip from 0 - NUM_LEDS
{
  for (int i=0; i<NUM_LEDS; i++) 
  {
    leds[i]=CRGB(RED, GREEN, BLUE);
    FastLED.show();
  }
}

/////////////////////////////TRANSLATE_IR_CODE////////////////////////////////
void translateIR() //Gives you the IR remote input info...
{
  switch(results.value)
  {
  case 0xFF02FD: Serial.println(" POWER"); break;
  case 0xFF827D: Serial.println(" NEXT"); break;
  case 0xFF3AC5: Serial.println(" BRIGHT UP"); break;
  case 0xFFBA45: Serial.println(" BRIGHT DOWN"); break;
  case 0xFF1AE5: Serial.println(" RED"); break;
  case 0xFF9A65: Serial.println(" GREEN"); break;
  case 0xFFA25D: Serial.println(" BLUE"); break;
  case 0xFF22DD: Serial.println(" WHITE"); break;
  case 0xFFFFFFFF: Serial.println(" REPEAT");break;  
  case 0xFF20DF: Serial.println(" JUMP_3");break;
  default: 
    Serial.println( results.value, HEX);
  }// End Case
  delay(5); // Do not get immediate repeat
} 
