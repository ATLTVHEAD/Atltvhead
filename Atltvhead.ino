/* Atltvhead.ino - Arduino code of how atltvhead helmet runs
 * Created by Nate Damen, 2015 
 * Apache License Version 2.0
 * Updated to use MQTT 
 * Updated 10-6-2021
 */
#include <FastLED.h>
//#include <LEDMatrix.h>
//#include <LEDText.h>
//#include <FontMatrise.h>
#include "EspMQTTClient.h"
#include "cred.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Math.h> 
#include <driver/i2s.h>

const int capacity = JSON_OBJECT_SIZE(15); 
StaticJsonDocument<capacity> doc;

const char* ssid1 = SSID1;
const char* password1 = PASSWORD1;

const char* ssid2 = SSID2;
const char* password2 = PASSWORD2;

const char* ssid3 = SSID3;
const char* password3 = PASSWORD3;
const char* mqttServerIp = MQTTBROKER;
const short mqttServerPort = PORT;
const char* mqttUser= MQTTUSER;
const char* mqttPassword= MQTTPASSWORD;

// Pubsub 
EspMQTTClient client(
  ssid1,
  password1,
  mqttServerIp,  // MQTT Broker server ip
  MQTTUSER,   // Can be omitted if not needed
  MQTTPASSWORD,
  mqttClientName,     // Client name that uniquely identify your device
  mqttServerPort
);
//-----------------------------------------------------------------------
#define I2S_WS 15
#define I2S_SD 13
#define I2S_SCK 2
#define I2S_PORT I2S_NUM_0 


//----------------------------------------------------------------------------------------------------//

int posin = 0;
int posinold = 0;
//----------------------------------------------------------------------------------------------------//
#define CHIPSET WS2812B
#define PIN 27
#define COLOR_ORDER GRB

int bright=255;

// Params for width and height
const uint8_t kMatrixWidth = 30;
const uint8_t kMatrixHeight = 18;

static uint16_t U;
static uint16_t O;
static uint16_t P;

uint16_t speed = 20;
uint16_t scale = 30; // scale is set dynamically once we've started up

#define NUM_LEDS ((kMatrixWidth * kMatrixHeight))
#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)
CRGB leds[NUM_LEDS];
CRGB ledsbuff[NUM_LEDS];

uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];


// these are for storing the max min and random users
String maxuser;
String minuser;
String selCheck;
String randuser;
// These are for storing the number of times the max and min people have chatted 
String maxUseNum;
String minUseNum;
int minValue=0;
int maxValue=0;

// Param for serpentine led layout
const bool    kMatrixSerpentineLayout = true;

// Use the "XY" function to generate the led number
uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;

  if( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }

  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }

  return i;
}

//------------------------
#define MATRIX_WIDTH   30
#define MATRIX_HEIGHT  18
#define MATRIX_TYPE    HORIZONTAL_ZIGZAG_MATRIX

//cLEDMatrix<MATRIX_WIDTH, -MATRIX_HEIGHT, MATRIX_TYPE> LEDs;

//cLEDText ScrollingMsg;



int CurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t chanel = 1;
uint8_t oldChanel = 1;
uint32_t ms = 0;
uint32_t yHueDelta32 = 0;
uint32_t xHueDelta32 = 0;
uint32_t hue = 0;
uint8_t angle = 0;
uint8_t mode = 1;

bool HFM = false;
double STime=0;
double FTime = 0;
double BTime=0;
bool BLMB = false;


char pasnum = 'B';

//-------------------------

//const unsigned char txt[] = {EFFECT_SCROLL_LEFT "          B L A C K   L I V E S   M A T T E R                   "};
//const unsigned char txt[] = {"what up"};
char inChar;
int in = 0;
bool TW = false;
bool mh = false;
bool mv = false;
bool mq = false;
bool mqr = false;
int Hue = 0;


int hHue = 211;
int hSat = 255;
int hVal = 255;
int bHue = 135;
int bSat = 255;
int bVal = 255;

int cHue = 211;
int cSat = 255;
int cVal = 255;
int cbHue = 135;
int cbSat = 255;
int cbVal = 255;

int hue5=254;
int hue6=hue5+160;


byte eyeCount=0;

boolean fullrainbow = false;
unsigned int flick =0;
boolean flickoverRide = false;
unsigned int raincount = 0;
uint8_t sprand=0;
//--------------------------------------------

bool tv[18][30]={
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
  {0,0,0,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };

bool tvMiddle[18][30]={
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };


bool unibg[9][39]={
  {0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0}};



//---------------------------------------------------------------------------------

bool eyeOpen[9][39]={
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,0,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

bool eyeMid[9][39]={
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

bool eyeClosed[9][39]={
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

bool u[9][39]={
  {0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};


uint8_t marta[18][30]={
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,2,2,0,0,0,0,2,2,3,3,3,3,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,1,1,1,2,2,2,0,0,2,2,2,3,3,3,3,3,0,0,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,0,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,1,2,2,2,2,2,2,2,2,3,3,3,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,1,1,2,2,2,2,2,2,2,2,3,3,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,1,2,2,2,2,2,2,2,2,3,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };

uint8_t atl[18][30]={
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };



//---------------------------------------------------------------------------------------------------//

bool blm[9][39]={
  {0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0},
  {0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0},
  {0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0},
  {0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0},
  {0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0},
  {0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0}};

//----------------------------------------------------------------------------------------------------//

int wificount = 0;

int heartcount = 0; //for the count on getting the animation to play
const int invheartc =2;
int hcind = 0;

bool flicker = false;

//hsv color values for hear animation
uint8_t color = 211;     //CHSV(211,255,255)  Pink  //CHSV(211,100,255) Light Pink //CHSV(135,255,255)  Blue
uint8_t oldColor = 135;
uint8_t colorHolder = 0;

//saturation values for heart animation
uint8_t sat = 255;     //CHSV(211,255,255)  Pink  //CHSV(211,100,255) Light Pink //CHSV(135,255,255)  Blue
uint8_t oldSat = 150;
uint8_t satHolder = 0;

// for demonDelay function
long totalTime = 0;

boolean MLF = false;
boolean MUD = false;
boolean MUP = false;
boolean MRF = false;

int cur = 0;
int last = 0;
int diff = 50;




CRGBPalette16 currentPalette( PartyColors_p);
TBlendType    currentBlending;

uint8_t       colorLoop = 1;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;


#define HOLD_PALETTES_X_TIMES_AS_LONG 1




void setup() {
// From the FASTLED CODE
  //---------------------------------------------
  FastLED.addLeds<CHIPSET, PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( bright );
  //FastLED.clear(true);

  gradHeart();
  displayScreen();

  delay(100);
  U = random16();
  O = random16();
  P = random16();

  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);
  delay(100);

  Serial.begin(115200);
  //---------------------

  //ScrollingMsg.SetFont(MatriseFontData);
  //ScrollingMsg.Init(&LEDs, LEDs.Width(), ScrollingMsg.FontHeight()+1, 0, 0);
  //ScrollingMsg.SetText((unsigned char *)txt, sizeof(txt) - 1);
  //ScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0x00, 0xff);

  //  FROM THE IRC TWITCHBOT CODE
  //-----------------------------------------------------------------------


 // pinMode(LED_BUILTIN, OUTPUT);

  delay(100);
  currentBlending = LINEARBLEND;

// add way to autochange wifi networks

 // Optionnal functionnalities of EspMQTTClient : 
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
  client.setMaxPacketSize(256);
}



void loop() {

  // getting the MQTT going and reading
  //---------------------------------------------------------
  client.loop();

  int32_t sample = 0;
  int bytes = i2s_pop_sample(I2S_PORT, (char*)&sample, portMAX_DELAY);
  if(bytes >0){
    // This indicates if audio has occured. can be used as a switch. 
    yield();
  }

  ms = millis();
  yHueDelta32 = ((int32_t)sin16( ms * (27/1) ) * (350 / kMatrixWidth));
  xHueDelta32 = ((int32_t)cos16( ms * (39/1) ) * (310 / kMatrixHeight));

  // Periodically choose a new palette, speed, and scale
  ChangePaletteAndSettingsPeriodically();

  // generate noise data
  fillnoise8();

  switch(mode){
    case 0:
        switch(chanel)
        {
        case 0:
            tvOutlineDisp();
            glitch_side_stutter();
            mirrorHandler();
            displayScreen();
            break;
        case 1:
            gradHeart();
            glitch_side_stutter();
            mirrorHandler();
            displayScreen();
            break;
        case 2:
            DrawOneFrame( ms / 131072, xHueDelta32 / 65536, yHueDelta32 / 65536);
            if(!fullrainbow){
                superRainbowHeart();
            }
            else if(fullrainbow){
                if(raincount <3000){
                    raincount++;
                }
                else{
                    raincount = 0;
                    fullrainbow = false;
                }
            }
            glitch_side_stutter();
            mirrorHandler();
            displayScreen();
            break;
        case 3:
            atlunited();
            glitch_side_stutter();
            mirrorHandler();
            displayScreen();
            break;
        }
        break;
    case 1:
        switch(chanel)
        {
        case 0:
            tvColorCycle();
            glitch_side_stutter();
            mirrorHandler();
            displayScreen();
            break;
        case 1:
            seawave2();
            glitch_side_stutter();
            //gradHeartsp();
            //FastLED.delay(50);
            mirrorHandler();
            displayScreen();
            break;
        case 2:
            SectionGlitchesHeart();
            glitch_side_stutter();
            mirrorHandler();
            delay(random(250,1000));
            displayScreen();
            break;
        case 3:
            mahearta();
            glitch_side_stutter();
            mirrorHandler();
            displayScreen();
            break;
        }
        break;
    case 2:
        switch(chanel)
        {
        case 0:
            gradHeartspcycle(cHue, cbHue);
            glitch_side_stutter();
            mirrorHandler();
            displayScreen();
            break;
        case 1:
            randomNoiseHeart();
            glitch_side_stutter();
            mirrorHandler();
            delay(random(12,125));
            displayScreen();
            break;
        case 2:
            currentPalette = RainbowStripeColors_p;
            SetupBlackAndWhiteStripedPalette();
            static uint8_t startIndex = 0;
            startIndex = startIndex + 3; /* motion speed */
            FillLEDsFromPaletteColors1( startIndex);
            mirrorHandler();
            glitch_side_stutter();
            displayScreen();
            break;
        case 3:
            BlackLivesMatterHeart();
            glitch_side_stutter();
            mirrorHandler();
            displayScreen();
            break;
        }
        break;
    case 3:
        switch(chanel)
        {
        case 0:
            mapNoiseToHeartWithOutline();
            glitch_side_stutter();
            mirrorHandler();
            displayScreen();
            break;
        case 1:
            mapNoiseToHeart();
            glitch_side_stutter();
            mirrorHandler();
            displayScreen();
            break;
        case 2:
            NoiseToScreen();
            glitch_side_stutter();
            mirrorHandler();
            displayScreen();
            break;
        case 3:
            color = 211;
            for(int ppgLooper =0; ppgLooper <= 3; ppgLooper++){
                for(int indPPG =0; indPPG<=5;indPPG++){
                    ppg(indPPG);
                }  
            }
            break;
        }
        break;
    case 4:
        eyeTvU(eyeCount);
        glitch_side_stutter();
        delay(500);
        displayScreen();
  }
  posinold = posin;
}


//------------------------------------------------------------------------------------------------------
// this is where the commands for chat are located

void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe("stream_live", [](const String & payload) {
    Serial.println(payload);
  });
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe("tvhead_bright", [](const String & payload) {
    Serial.println(payload);
  });
  // gESTURE HANDLER
  client.subscribe("glove/gesture", [](const String & payload) {
    if(payload=="wave_mode"){
      mode=1;      
    }
    else if(payload=="speed_mode"){
      mode=3;      
    }
    else if(payload=="random_motion_mode"){
      mode=0;      
    }
    else if(payload=="fist_pump_mode"){
      mode=2;      
    }   
    
    Serial.println(payload);
  });

  
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe("viewer_click", [](const String & payload) {
    chanel++;
    if(chanel>=4){
        chanel=0;
    }
    Serial.println(payload);
  });



  // Subscribe to "mytopic/wildcardtest/#" and display received message to Serial
  //client.subscribe("mytopic/wildcardtest/#", [](const String & topic, const String & payload) {
  //  Serial.println("(From wildcard) topic: " + topic + ", payload: " + payload);
  //});

  // Publish a message to "mytopic/test"
  client.publish("tvhead", "alive"); // You can activate the retain flag by setting the third parameter to true
  

  // Execute delayed instructions
  //client.executeDelayed(5 * 1000, []() {
  //  client.publish("mytopic/wildcardtest/test123", "This is a message sent 5 seconds later");
  //});
}








/*
void callback(IRCMessage ircMessage) {
//Serial.println("In CallBack");
  // PRIVMSG ignoring CTCP messages
  if (ircMessage.command == "PRIVMSG" && ircMessage.text[0] != '\001') {
    //Serial.println("Passed private message.");
    String message("<" + ircMessage.nick + "> " + ircMessage.text);
    Serial.println(message);
    selCheck = ircMessage.text[0];
    //Serial.println(selCheck);
    ///////////////////////////////////////////////////////////////////////////////////////////
    if((ircMessage.text == "!ch0" && ircMessage.nick == "atltvhead") || (ircMessage.text =="0" && ircMessage.nick == "atltvhead") || ircMessage.text=="LUL"){
      chanel = 0;
      client.sendMessage(IRC_CHAN, ircMessage.nick + " set atltvhead to the secret 0!");
      //Serial.println(chanel);
    }
    else if(ircMessage.text == "<3" || ircMessage.text =="atltvhSph" || ircMessage.text =="1" || ircMessage.text =="!ch1" || ircMessage.text=="TwitchLit" || ircMessage.text =="TwitchUnity"){
      chanel = 1;
      if(heartcount >= 2){
      client.sendMessage(IRC_CHAN, ircMessage.nick + " shows their heart! Thank you! Chat, you've shown your heart! YOU ARE AWESOME! THANK YOU!!!!!!!!!!");
       for(int ppgLooper =0; ppgLooper <= 3; ppgLooper++){
        for(int indPPG =0; indPPG<=5;indPPG++){
          ppg(indPPG);
        }
       }
        heartcount = 0;
      }
      else{
        hcind = invheartc - heartcount;
        client.sendMessage(IRC_CHAN, ircMessage.nick + " shows their heart! Thank you! Chat, we need " + hcind + " more people to show their '!heart'. LET'S DO THIS!");
        heartcount++;
      }
    }
    else if(ircMessage.text=="TheIlluminati"){
      mode = 4;
    }
    else if(ircMessage.text == "1" && ircMessage.nick == "atltvhead"){
      //client.sendMessage(IRC_CHAN, ircMessage.nick + " It's PowerPuff Girl's Heart Time!");
       for(int ppgLooper =0; ppgLooper <= 3; ppgLooper++){
        for(int indPPG =0; indPPG<=5;indPPG++){
          ppg(indPPG);
        }
       }
    }
    else if(ircMessage.text == "!flicker"){
      flicker = true;
    }
    else if(ircMessage.text == "!flickerOff"){
      flickoverRide = true;
    }
    else if(ircMessage.text == "!brighter"){
      bright = bright +10;
      FastLED.setBrightness(bright);
    }
     else if(ircMessage.text == "!dimmer"){
      bright = bright -10;
      FastLED.setBrightness(bright);
    }
    else if(ircMessage.text =="!heartColor"){
         changeHeartHue();
    }
    else if(ircMessage.text =="!heartSat"){
         changeHeartSat();
    }
    else if(ircMessage.text =="!heartBright"){
         changeHeartVal();
    }
    else if(ircMessage.text =="!backgroundColor"){
         changeBackHue();
    }
    else if(ircMessage.text =="!backgroundSat"){
        changeBackSat();

    }
    else if(ircMessage.text =="!backgroundBright"){
        changeBackVal();

    }
    else if(ircMessage.text =="!reset"){
      resetHeart();
    }
    else if(ircMessage.text =="!fullRainbow"){
      fullrainbow = true;
    }
    else if(ircMessage.text =="!mirrorRight"){
      MLF = true;
    }
    else if(ircMessage.text =="!mirrorOff"){
      MLF = false;
      MUD = false;
      MUP = false;
    }
    else if(ircMessage.text =="!mirrorUp"){
      MUP = true;
      MUD = false;
    }
    else if(ircMessage.text =="!mirrorDown"){
      MUD = true;
      MUP = false;
    }
    else if(ircMessage.text =="atltvhRb" || ircMessage.text == "!ch2" || ircMessage.text =="2" || ircMessage.text =="!rainbowHeart" || ircMessage.text=="KappaPride" || ircMessage.text=="VirtualHug" || ircMessage.text =="PridePaint"){
      chanel = 2;
      fullrainbow = true;
    }
    else if(ircMessage.text =="!ch3" || ircMessage.text =="3" || ircMessage.text =="!United" || ircMessage.text =="!Mahearta" || ircMessage.text =="!unite" || ircMessage.text=="bleedPurple"){
      chanel=3;
    }
    else if(selCheck == "~" && ircMessage.nick == "tvheadbot"){
      //do nothing for compile check
      minuser = ircMessage.text;
      minuser.remove(0,1);
    }
    else if(selCheck == "+" && ircMessage.nick == "tvheadbot"){
      //do nothing for compile check
      maxuser = ircMessage.text;
      maxuser.remove(0,1);
    }
    else if(selCheck == "~" && ircMessage.nick == "atltvhead"){
      //do nothing for compile check
      minuser = ircMessage.text;
      minuser.remove(0,1);
    }
    else if(selCheck == "+" && ircMessage.nick == "atltvhead"){
      //do nothing for compile check
      maxuser = ircMessage.text;
      maxuser.remove(0,1);
    }
    else if(selCheck == ">" && ircMessage.nick == "atltvhead"){
      maxUseNum = ircMessage.text;
      maxUseNum.remove(0,1);
      maxValue = atoi(maxUseNum.c_str());
    }
    else if(selCheck == "<" && ircMessage.nick == "atltvhead"){
      minUseNum == ircMessage.text;
      minUseNum.remove(0,1);
      minValue = atoi(minUseNum.c_str());
    }
    else if(selCheck == ">" && ircMessage.nick == "tvheadbot"){
      maxUseNum = ircMessage.text;
      maxUseNum.remove(0,1);
      maxValue = atoi(maxUseNum.c_str());
    }
    else if(selCheck == "<" && ircMessage.nick == "tvheadbot"){
      minUseNum == ircMessage.text;
      minUseNum.remove(0,1);
      minValue = atoi(minUseNum.c_str());
    }
    else if(ircMessage.text == "!maxtvhead" && ircMessage.nick == maxuser){
      chanel = 0;
      // do nothing for right now
    }
    else if(ircMessage.text == "!mintvhead" && ircMessage.nick == minuser){
      chanel = 0;
      //do nothing for right now
    }
    else if(selCheck =="-" && ircMessage.nick == "tvheadbot"){
      randuser = ircMessage.text;
      randuser.remove(0,1);
    }
    else if(selCheck =="-" && ircMessage.nick == "atltvhead"){
      randuser = ircMessage.text;
      randuser.remove(0,1);
    }
    else if(ircMessage.text == "!randtvhead" && ircMessage.nick == randuser){
      chanel = 0;
    }
    else if(ircMessage.text == "!random_motion_mode"){
      mode = 0;
    }
    else if(ircMessage.text == "!fist_pump_mode"){
      mode = 2;
    }
    else if(ircMessage.text == "!wave_mode"){
      mode = 1;
    }
    else if(ircMessage.text == "!speed_mode"){
      mode = 3;
    }
    
    return;
  }
 }
*/


void mirrorHandler(){
    if(MLF==true){
        mirrorLeftToRight();
    }
    if(MUD == true){
        mirrorUptoDown();
    }
    if(MUP == true){
        mirrorDowntoUp();
    }
}
//--------------------------------------------------------------------------------------------------
// these are the tvhead helper functions.


//-----------------------------------------------------------------------------------------------------------------------------------

void i2s_install(){
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 44100,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false
  };
  
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin(){
    const i2s_pin_config_t pin_config = {
      .bck_io_num = I2S_SCK,
      .ws_io_num = I2S_WS,
      .data_out_num = -1,
      .data_in_num = I2S_SD
    };
    i2s_set_pin(I2S_PORT, &pin_config);
}




//_________________________________________________________________________________________
// heart effect
void heart(){
  for( byte y = 0; y < kMatrixHeight; y++) {
    for( byte x = 0; x < kMatrixWidth; x++) {
      if(tv[y][x]){
        leds[ XY(x, y)]  = CHSV( cHue, cSat, cVal);
      }
      else{
        leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
      }
    }  
  }
}


void colorFillScreen(int hue12, int hue34){
  for( byte y = 0; y < kMatrixHeight; y++) {
    for( byte x = 0; x < kMatrixWidth; x++) {
      if(tv[y][x]){
        leds[ XY(x, y)]  = CHSV( hue12, cSat, cVal);
      }
      else{
        leds[XY(x,y)]=CHSV(hue34,cbSat,cbVal);
      }
    }  
  }
}

void changeHeartHue(){
  cHue = cHue + 30;
  if(cHue >= 255){
    cHue = 10;
  }
}

void changeHeartSat(){
  cSat = cSat + 25;
  if(cSat>= 255){
    cSat = 25;
  }
}

void changeHeartVal(){
  cVal = cVal + 25;
  if(cVal>= 255){
    cVal = 25;
  }
}

void changeBackHue(){
  cbHue = cbHue + 35;
  if(cbHue >= 255){
    cbHue = 25;
  }
}

void changeBackSat(){
  cbSat = cbSat + 25;
  if(cbSat>= 255){
    cbSat = 25;
  }
}

void changeBackVal(){
  cbVal = cbVal + 25;
  if(cbVal>= 255){
    cbVal = 25;
  }
}

void resetHeart(){
  cHue = hHue;
  cVal = hVal;
  cSat = hSat;
  cbHue = bHue;
  cbVal = bVal;
  cbSat = bSat;
}



void ppg(byte frame){
  if(frame ==0){
    fill_solid(leds,NUM_LEDS, CHSV(oldColor,oldSat,255)); // change this to a solid fill
      FastLED.show();
      delay(75);
      leds[XY( 8, 3)] = CHSV(color,sat,255);
      leds[XY( 8, 4)] = CHSV(color,sat,255);
      leds[XY( 9, 3)] = CHSV(color,sat,255);
      leds[XY( 9, 4)] = CHSV(color,sat,255);
      leds[XY( 38-8, 3)] = CHSV(color,sat,255);
      leds[XY( 38-8, 4)] = CHSV(color,sat,255);
      leds[XY( 38-9, 3)] = CHSV(color,sat,255);
      leds[XY( 38-9, 4)] = CHSV(color,sat,255);
      }
    else if(frame==1){

      leds[XY( 6, 3)] = CHSV(color,sat,255);
      leds[XY( 7, 3)] = CHSV(color,sat,255);
      leds[XY( 38-6, 3)] = CHSV(color,sat,255);
      leds[XY( 38-7, 3)] = CHSV(color,sat,255);

      leds[XY( 10, 3)] = CHSV(color,sat,255);
      leds[XY( 11, 3)] = CHSV(color,sat,255);
      leds[XY( 38-10, 3)] = CHSV(color,sat,255);
      leds[XY( 38-11, 3)] = CHSV(color,sat,255);

      leds[XY( 7, 4)] = CHSV(color,sat,255);
      leds[XY( 10, 4)] = CHSV(color,sat,255);
      leds[XY( 38-7, 4)] = CHSV(color,sat,255);
      leds[XY( 38-10, 4)] = CHSV(color,sat,255);

      leds[XY( 8, 5)] = CHSV(color,sat,255);
      leds[XY( 9, 5)] = CHSV(color,sat,255);
      leds[XY( 38-8, 5)] = CHSV(color,sat,255);
      leds[XY( 38-9, 5)] = CHSV(color,sat,255);    
      }

    else if(frame==2){

      leds[XY( 5, 1)] = CHSV(color,sat,255);
      leds[XY( 6, 1)] = CHSV(color,sat,255);
      leds[XY( 38-5, 1)] = CHSV(color,sat,255);
      leds[XY( 38-6, 1)] = CHSV(color,sat,255);

      leds[XY( 11, 1)] = CHSV(color,sat,255);
      leds[XY( 12, 1)] = CHSV(color,sat,255);
      leds[XY( 38-11, 1)] = CHSV(color,sat,255);
      leds[XY( 38-12, 1)] = CHSV(color,sat,255);
      
      for(int xj = 4; xj<14; xj++){
        leds[XY( xj, 2)] = CHSV(color,sat,255);
        }

      for(int xj = 4; xj<14; xj++){
        leds[XY( 38-xj, 2)] = CHSV(color,sat,255);
        }
        
        leds[XY( 4, 3)] = CHSV(color,sat,255);
        leds[XY( 5, 3)] = CHSV(color,sat,255);
        leds[XY( 38-4, 3)] = CHSV(color,sat,255);
        leds[XY( 38-5, 3)] = CHSV(color,sat,255);

        leds[XY( 12, 3)] = CHSV(color,sat,255);
        leds[XY( 13, 3)] = CHSV(color,sat,255);
                leds[XY( 38-12, 3)] = CHSV(color,sat,255);
        leds[XY( 38-13, 3)] = CHSV(color,sat,255);

        leds[XY( 5, 4)] = CHSV(color,sat,255);
        leds[XY( 6, 4)] = CHSV(color,sat,255);
                leds[XY( 38-5, 4)] = CHSV(color,sat,255);
        leds[XY( 38-6, 4)] = CHSV(color,sat,255);

        leds[XY( 11, 4)] = CHSV(color,sat,255);
        leds[XY( 12, 4)] = CHSV(color,sat,255);
                leds[XY( 38-11, 4)] = CHSV(color,sat,255);
        leds[XY( 38-12, 4)] = CHSV(color,sat,255);

        leds[XY( 6, 5)] = CHSV(color,sat,255);
        leds[XY( 7, 5)] = CHSV(color,sat,255);
                leds[XY( 38-6, 5)] = CHSV(color,sat,255);
        leds[XY( 38-7, 5)] = CHSV(color,sat,255);

        leds[XY( 10, 5)] = CHSV(color,sat,255);
        leds[XY( 11, 5)] = CHSV(color,sat,255);
                leds[XY( 38-10, 5)] = CHSV(color,sat,255);
        leds[XY( 38-11, 5)] = CHSV(color,sat,255);

        for(int xi = 7; xi<12; xi++){
          leds[XY( xi, 6)] = CHSV(color,sat,255);
        }
        for(int xi = 7; xi<12; xi++){
          leds[XY( 38-xi, 6)] = CHSV(color,sat,255);
        }
      }

    else if(frame==3){
      leds[XY( 5, 0)] = CHSV(color,sat,255);
      leds[XY( 6, 0)] = CHSV(color,sat,255);
      leds[XY( 11, 0)] = CHSV(color,sat,255);
      leds[XY( 12, 0)] = CHSV(color,sat,255);
            leds[XY( 38-5, 0)] = CHSV(color,sat,255);
      leds[XY( 38-6, 0)] = CHSV(color,sat,255);
      leds[XY( 38-11, 0)] = CHSV(color,sat,255);
      leds[XY( 38-12, 0)] = CHSV(color,sat,255);

      leds[XY( 4, 1)] = CHSV(color,sat,255);
      leds[XY( 7, 1)] = CHSV(color,sat,255);
      leds[XY( 10, 1)] = CHSV(color,sat,255);
      leds[XY( 13, 1)] = CHSV(color,sat,255);
            leds[XY( 38-4, 1)] = CHSV(color,sat,255);
      leds[XY( 38-7, 1)] = CHSV(color,sat,255);
      leds[XY( 38-10, 1)] = CHSV(color,sat,255);
      leds[XY( 38-13, 1)] = CHSV(color,sat,255);

      leds[XY( 3, 2)] = CHSV(color,sat,255);
      leds[XY( 14, 2)] = CHSV(color,sat,255);
            leds[XY( 38-3, 2)] = CHSV(color,sat,255);
      leds[XY( 38-14, 2)] = CHSV(color,sat,255);

      leds[XY( 3, 3)] = CHSV(color,sat,255);
      leds[XY( 14, 3)] = CHSV(color,sat,255);
            leds[XY( 38-3, 3)] = CHSV(color,sat,255);
      leds[XY( 38-14, 3)] = CHSV(color,sat,255);

      leds[XY( 4, 4)] = CHSV(color,sat,255);
      leds[XY( 13, 4)] = CHSV(color,sat,255);
            leds[XY( 38-4, 4)] = CHSV(color,sat,255);
      leds[XY( 38-13, 4)] = CHSV(color,sat,255);

      leds[XY( 5, 5)] = CHSV(color,sat,255);
      leds[XY( 12, 5)] = CHSV(color,sat,255);
            leds[XY( 38-5, 5)] = CHSV(color,sat,255);
      leds[XY( 38-12, 5)] = CHSV(color,sat,255);

      leds[XY( 6, 6)] = CHSV(color,sat,255);
      leds[XY( 11, 6)] = CHSV(color,sat,255);
            leds[XY( 38-6, 6)] = CHSV(color,sat,255);
      leds[XY( 38-11, 6)] = CHSV(color,sat,255);


      leds[XY( 8, 7)] = CHSV(color,sat,255);
      leds[XY( 9, 7)] = CHSV(color,sat,255);
            leds[XY( 38-8, 7)] = CHSV(color,sat,255);
      leds[XY( 38-9, 7)] = CHSV(color,sat,255);
      }

    else if(frame==4){
      for(int xk = 3; xk<16; xk++){
          leds[XY( xk, 0)] = CHSV(color,sat,255);

        }

      for(int xl = 2; xl<17; xl++){
          leds[XY( xl, 1)] = CHSV(color,sat,255);

        }
      leds[XY( 2, 2)] = CHSV(color,sat,255);
      leds[XY( 15, 2)] = CHSV(color,sat,255);

      leds[XY( 2, 3)] = CHSV(color,sat,255);
      leds[XY( 15, 3)] = CHSV(color,sat,255);

      leds[XY( 3, 4)] = CHSV(color,sat,255);
      leds[XY( 14, 4)] = CHSV(color,sat,255);

      leds[XY( 4, 5)] = CHSV(color,sat,255);
      leds[XY( 13, 5)] = CHSV(color,sat,255);

      leds[XY(5, 6)-1] = CHSV(color,sat,255);
      leds[XY(12, 6)-1] = CHSV(color,sat,255);
      }
    else{
      fill_solid(leds,NUM_LEDS, CHSV(color,sat,255)); // change this to a solid fill
      satHolder = oldSat;
      oldSat = sat;
      sat = satHolder;
      }
    FastLED.show();
    delay(70);
}



void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
  byte lineStartHue = startHue8;
  for( byte y = 0; y < kMatrixHeight; y++) {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;
    for( byte x = 0; x < kMatrixWidth; x++) {
      pixelHue += xHueDelta8;
      leds[ XY(x, y)]  = CHSV( pixelHue+hue, 255, 255);
    }
  }
}


void displayScreen(){
  FastLED.show();
}

void demonDelay(long msec){
  totalTime = millis() + msec;
  while (millis() < totalTime) {
     yield();
  }
}


void superRainbowHeart(){
  for( byte y = 0; y < kMatrixHeight; y++) {
    for( byte x = 0; x < kMatrixWidth; x++) {
      if(tv[y][x]){
        //nothing
      }
      else{
        leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
      }
    }
  }
}



void mirrorLeftToRight(){
  // Here we are setting the bottom led's to their top led counter part -> take into consideration the 3 missing pixels on the bottom
  //row and the one missing pixel on the second to bottom row just like in the Heart settings!

for(byte y = 0; y < kMatrixHeight; y++){
  for(byte x = 0; x < 20; x++){
      leds[ XY(x, y)] = leds[ XY(38-x,y) ];
    }
  }
}
void mirrorRightToLeft(){
  // Here we are setting the bottom led's to their top led counter part -> take into consideration the 3 missing pixels on the bottom
  //row and the one missing pixel on the second to bottom row just like in the Heart settings!

for(byte y = 0; y < kMatrixHeight; y++){
  for(byte x = 0; x < 20; x++){
      leds[ XY(38-x, y)] = leds[ XY(x,y) ];
    }
  }
}

void mirrorUptoDown(){
  for(byte y=0; y < 4 ; y++){
    for(byte x=0; x<kMatrixWidth;x++){
      leds[ XY(x,8-y)] = leds[ XY(x,y) ];
    }
  }
}

void mirrorDowntoUp(){
  for(byte y=0; y < 4 ; y++){
    for(byte x=0; x<kMatrixWidth;x++){
      leds[ XY(x, y)] = leds[ XY(x,8-y) ];
    }
  }
}


void sparkles(){
  for(byte y=0; y < kMatrixHeight;y++){
    for(byte x=0; x<kMatrixWidth;x++){
      sprand = random(100);
      if(tv[y][x]){
        if(sprand < 4){
         leds[XY(x,y)]=CHSV(cHue,0,cVal);
        }
        else{
          leds[XY(x,y)]=CHSV(cHue,cSat,cVal);
        }
      }
      else{
        leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
      }
    }
  }
}


//-----------------------------------------------------------------------

void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

void SetupWhiteAndBlackStripedPalette()
{
    // 'White out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::White);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::Black;
    currentPalette[4] = CRGB::Black;
    currentPalette[8] = CRGB::Black;
    currentPalette[12] = CRGB::Black;
    
}

void mahearta(){
  for(byte y=0; y < kMatrixHeight;y++){
    for(byte x=0; x<kMatrixWidth;x++){
      sprand = random(100);
      if(marta[y][x]==1){
        if(sprand < 4){
         leds[XY(x,y)]=CHSV(160,0,204);
        }
        else{
          leds[XY(x,y)]=CHSV(160,254,204);
        }
      }
      else if(marta[y][x]==2){
        if(sprand < 4){
         leds[XY(x,y)]=CHSV(48,0,254);
        }
        else{
          leds[XY(x,y)]=CHSV(48,254,254);
        }
      }
      else if(marta[y][x]==3){
        if(sprand < 4){
         leds[XY(x,y)]=CHSV(22,0,254);
        }
        else{
          leds[XY(x,y)]=CHSV(22,254,254);
        }
      }
      else{
        leds[XY(x,y)]=CHSV(261,25, 206);
      }
    }
  }
}



void FillLEDsFromPaletteColors1( uint8_t colorIndex)
{
   uint8_t brightness = 255;

   for(byte y=0; y < kMatrixHeight;y++){
    for(byte x=0; x<kMatrixWidth;x++){  
      leds[ XY(x, y)]  = ColorFromPalette( currentPalette, colorIndex, bright, currentBlending);
      colorIndex += 3;
    }
   }

    for( byte y = 0; y < kMatrixHeight; y++) {
    for( byte x = 0; x < kMatrixWidth; x++) {
      if(tv[y][x]){
        if(leds[XY(x,y)]){
          leds[XY(x,y)]=CRGB::Black;
        }
        else{
          leds[XY(x,y)]=CRGB::White;
          }
        }
      }
  }   
}




void FillLEDsFromPaletteColors2( uint8_t colorIndex)
{
   uint8_t brightness = 255;

  for(int i=0;i<NUM_LEDS;i++){
          leds[i]=ColorFromPalette( currentPalette, colorIndex, bright, currentBlending);
        colorIndex += 3;
    }
    for( byte y = 0; y < kMatrixHeight; y++) {
    for( byte x = 0; x < kMatrixWidth; x++) {
      if(tv[y][x]){
        if(leds[XY(x,y)]){
          leds[XY(x,y)]=CRGB::Black;
        }
        else{
          leds[XY(x,y)]=CRGB::White;
        }
      }
    }
  } 
}



//-------------------------------------------------------------------------

void atlunited(){
   for( byte y = 0; y < kMatrixHeight; y++) {
    for( byte x = 0; x < kMatrixWidth; x++) {
      if(unibg[y][x]){
        leds[ XY(x, y)] = CHSV(0,0,50);
      }
      else{
        leds[XY(x,y)]=CHSV(0,180,180);
      }
      }
    }
  
  for( byte y = 0; y < kMatrixHeight; y++) {
    for( byte x = 0; x < kMatrixWidth; x++) {
      if(tv[y][x]){
        leds[ XY(x, y)]=CHSV(51,180,220);// gold
      }
    }
  }
}

void gradHeartsp(){
  fill_gradient(leds,0,CHSV(192,254,254),350,CHSV(0,254,254),SHORTEST_HUES);
  for(byte y=0; y < kMatrixHeight;y++){
    for(byte x=0; x<kMatrixWidth;x++){
      sprand = random(100);
      if(tv[y][x]){
        if(sprand < 4 + minValue){
         leds[XY(x,y)]=CHSV(cHue,0,cVal);
        }
        else{
          //leds[XY(x,y)]=CHSV(cHue,cSat,cVal);
        }
      }
      else{
        leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
      }
    }
  }
}

void gradHeartspcycle(int hue12, int hue34){
  fill_gradient(leds,0,CHSV(hue12,254,254),350,CHSV(hue34,254,254),SHORTEST_HUES);
  for(byte y=0; y < kMatrixHeight;y++){
    for(byte x=0; x<kMatrixWidth;x++){
      sprand = random(100);
      if(tv[y][x]){
        if(sprand < 4 + minValue){
         leds[XY(x,y)]=CHSV(cHue,0,cVal);
        }
        else{
          //leds[XY(x,y)]=CHSV(cHue,cSat,cVal);
        }
      }
      else{
        leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
      }
    }
  }
  hue12++;
  hue34++;
}

void gradHeart(){
  fill_gradient(leds,0,CHSV(192,254,254),350,CHSV(0,254,254),SHORTEST_HUES);
    for(byte y=0; y < kMatrixHeight;y++){
     for(byte x=0; x<kMatrixWidth;x++){
       sprand = random(100);
       if(tv[y][x]){
         //do nothing
        }
       else{
         leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
       }
     }
   }
}

void gradHeartShift(){
  fill_gradient(leds,0,CHSV(192,254,254),350,CHSV(0,254,254),SHORTEST_HUES);
    for(byte y=0; y < kMatrixHeight;y++){
     for(byte x=0; x<kMatrixWidth;x++){
       sprand = random(100);
       if(tv[y][x]){
         //do nothing
        }
       else{
         leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
       }
     }
   }
}


void gradBackground(){
  fill_gradient(leds,0,CHSV(192,254,254),350,CHSV(0,254,254),SHORTEST_HUES);
  for(byte y=0; y < kMatrixHeight;y++){
     for(byte x=0; x<kMatrixWidth;x++){
       sprand = random(100);
       if(tv[y][x]){
         leds[XY(x,y)]=CHSV(cHue,cSat,cVal);
        }
       else{
         // do nothing 
       }
     }
   }
}

void eyeOpenDisp(){
  for( byte y = 0; y < kMatrixHeight; y++) {
    for( byte x = 0; x < kMatrixWidth; x++) {
      if(eyeOpen[y][x]){
        leds[ XY(x, y)]  = CHSV( cHue, cSat, cVal);
      }
      else{
        leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
      }
    }  
  }
}

void uDisp(){
  for( byte y = 0; y < kMatrixHeight; y++) {
    for( byte x = 0; x < kMatrixWidth; x++) {
      if(u[y][x]){
        leds[XY(x,y)]  = CHSV( cHue, cSat, cVal);
      }
      else{
        leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
      }
    }  
  }
}

void eyeMidDisp(){
  for( byte y = 0; y < kMatrixHeight; y++) {
    for( byte x = 0; x < kMatrixWidth; x++) {
      if(eyeMid[y][x]){
        leds[ XY(x, y)]  = CHSV( cHue, cSat, cVal);
      }
      else{
        leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
      }
    }  
  }
}

void eyeClosedDisp(){
  for( byte y = 0; y < kMatrixHeight; y++) {
    for( byte x = 0; x < kMatrixWidth; x++) {
      if(eyeClosed[y][x]){
        leds[ XY(x, y)]  = CHSV( cHue, cSat, cVal);
      }
      else{
        leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
      }
    }  
  }
}

void eyeTvU(byte ci){
  if(ci==0){
    eyeOpenDisp();
  }
  else if(ci==1){
    heart();
  }
  else if(ci==2){
    uDisp();
  }
  ci++;
  if(ci>2){
    ci=0;
  }
}

void tvColorCycle(){
  cur=millis();
    if(cur-last>diff){
      hue5++;
      hue6=hue5+160;
      last=millis();
    }
    if(last>cur){
      last=millis();
    }
  colorFillScreen(hue5,hue6);
}

void columnScanner(){
    for( byte x = 0; x < kMatrixWidth; x++) {
      heart();
      for(byte y = 0;y<kMatrixHeight;y++){
        //for the first set
        leds[ XY(x, y)]  %= 30;
        if(x<kMatrixWidth-2){
          leds[ XY(x+1, y)]  %= 30;
        }
        if(x<kMatrixWidth-3){
          leds[ XY(x+2, y)]  %= 30;
        }
        if(x<kMatrixWidth-4){
          leds[ XY(x+3, y)]  %= 30;
        }
      }
      if(MLF==true){
        mirrorLeftToRight();
      }
      if(MUD == true){
        mirrorUptoDown();
      }
      if(MUP == true){
        mirrorDowntoUp();
       }
      displayScreen();
      delay(10);
    } 
}

void colorScanner(int cAdd){
    for( byte x = 0; x < kMatrixWidth; x++) {
      heart();
      for(byte y = 0;y<kMatrixHeight;y++){
        if(tv[y][x]){
          leds[ XY(x, y)]  = CHSV( cHue+cAdd, cSat, cVal);
        }
        else{
          leds[XY(x,y)]=CHSV(cbHue+cAdd,cbSat,cbVal);
        }
        if(x<kMatrixWidth-2){
          if(tv[y][x+1]){
          leds[ XY(x+1, y)]  = CHSV( cHue+cAdd, cSat, cVal);
          }
           else{
            leds[XY(x+1,y)]=CHSV(cbHue+cAdd,cbSat,cbVal);
          }
        }
      }
      if(MLF==true){
        mirrorLeftToRight();
      }
      if(MUD == true){
        mirrorUptoDown();
      }
      if(MUP == true){
        mirrorDowntoUp();
       }
      displayScreen();
      delay(15);
    } 
}


// this is where the wavey animation goes (user data input) with heart or background fill
void wavey(){
    for(byte y=0; y < kMatrixHeight;y++){
     for(byte x=0; x<kMatrixWidth;x++){
       if(tv[y][x]){
         leds[XY(x,y)]=CHSV(cHue,cSat,cVal);
        }
       else{
         leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
       }
     }
   }
  
}

//// Put inplace these Model A Math modes
////this is drawing one single pixel 
//tft.drawPixel(yv(v)+ytrans,xv(v)+xtrans,HX8357_WHITE);\
////these should offset the image to the center of the screen 
//uint16_t xtrans=240;
//uint16_t ytrans=160;
//
////the equations for x
//uint16_t xv(uint16_t t){
//  if(eq==0){
//    // change that 250 variable
//    return sin(t/10)*100 + sin(t/15)*(80*setEquation()); // + sin(t*10)*100;
//  }
//  else if(eq==1){
//    //change the 13 in the last sin variable
//    return sin(t/10)*100 + sin(t/setEquation()+4)*40;
//  }
//  else if(eq==2){
//    return sin(t*5/2)*100*setEquation();
//  }
//  //sin(t/15)*100
//  //return sin(t/10)*100 * sin(t/3)*100;
//}
//
////the equations for y 
//uint16_t yv(uint16_t t){
//  if(eq==0){
//    return cos(t/10)*100;
//  }
//  else if(eq==1){
//    return cos(t/10)*100 + sin(t)*40;
//  }
//  else if(eq==2){
//    return log(t)*5-50;
//  }
//   //sin(t*2)*100;
//  //return cos(t/10)*100;
//}


// Fill the x/y array of 8-bit noise values using the inoise8 function.

void fillnoise8() {
  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
  // The amount of data smoothing we're doing depends on "speed".
  uint8_t dataSmoothing = 0;
  if( speed < 50) {
    dataSmoothing = 200 - (speed * 4);
  }
  
  for(int i = 0; i < MAX_DIMENSION; i++) {
    int ioffset = scale * i;
    for(int j = 0; j < MAX_DIMENSION; j++) {
      int joffset = scale * j;
      
      uint8_t data = inoise8(U + ioffset,O + joffset,P);

      // The range of the inoise8 function is roughly 16-238.
      // These two operations expand those values out to roughly 0..255
      // You can comment them out if you want the raw noise data.
      data = qsub8(data,16);
      data = qadd8(data,scale8(data,39));

      if( dataSmoothing ) {
        uint8_t olddata = noise[i][j];
        uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
        data = newdata;
      }
      
      noise[i][j] = data;
    }
  }
  
  P += speed;
  
  // apply slow drift to X and Y, just for visual variation.
  U += speed / 8;
  O -= speed / 16;
}

void mapNoiseToLEDsUsingPalette()
{
  static uint8_t ihue=0;
  
  for(int i = 0; i < kMatrixWidth; i++) {
    for(int j = 0; j < kMatrixHeight; j++) {
      // We use the value at the (i,j) coordinate in the noise
      // array for our brightness, and the flipped value from (j,i)
      // for our pixel's index into the color palette.

      uint8_t index = noise[j][i];
      uint8_t bri =   noise[i][j];

      // if this palette is a 'loop', add a slowly-changing base value
      if( colorLoop) { 
        index += ihue;
      }

      // brighten up, as the color palette itself often contains the 
      // light/dark dynamic range desired
      if( bri > 127 ) {
        bri = 255;
      } else {
        bri = dim8_raw( bri * 2);
      }

      CRGB color = ColorFromPalette( currentPalette, index, bri);
      leds[XY(i,j)] = color;
    }
  }
  
  ihue+=1;
}

void mapNoiseToHeart()
{
  static uint8_t ihue=0;
  
  for(int i = 0; i < kMatrixWidth; i++) {
    for(int j = 0; j < kMatrixHeight; j++) {
      // We use the value at the (i,j) coordinate in the noise
      // array for our brightness, and the flipped value from (j,i)
      // for our pixel's index into the color palette.

      uint8_t index = noise[j][i];
      uint8_t bri =   noise[i][j];

      // if this palette is a 'loop', add a slowly-changing base value
      if( colorLoop) { 
        index += ihue;
      }

      // brighten up, as the color palette itself often contains the 
      // light/dark dynamic range desired
      if( bri > 127 ) {
        bri = 255;
      } else {
        bri = dim8_raw( bri * 2);
      }

      CRGB color = ColorFromPalette( currentPalette, index, bri);
      if(tv[j][i]){
        leds[XY(i,j)] = color;
      }
    }
  }
  ihue+=1;
}


void NoiseToScreen()
{
  static uint8_t ihue=0;
  
  for(int i = 0; i < kMatrixWidth; i++) {
    for(int j = 0; j < kMatrixHeight; j++) {
      // We use the value at the (i,j) coordinate in the noise
      // array for our brightness, and the flipped value from (j,i)
      // for our pixel's index into the color palette.

      uint8_t index = noise[j][i];
      uint8_t bri =   noise[i][j];

      // if this palette is a 'loop', add a slowly-changing base value
      if( colorLoop) { 
        index += ihue;
      }

      // brighten up, as the color palette itself often contains the 
      // light/dark dynamic range desired
      if( bri > 127 ) {
        bri = 255;
      } else {
        bri = dim8_raw( bri * 2);
      }

      CRGB color = ColorFromPalette( currentPalette, index, bri);
      if(tv[j][i]){
      leds[XY(i,j)] = color;
      }
      // not working need to fix
      else{
        leds[XY(i,j)] = color - CHSV(211,200,200)+ CHSV(135,210,210);
      }
    }
  }
  
  ihue+=1;
}

void ChangePaletteAndSettingsPeriodically()
{
  uint8_t secondHand = ((millis() / 1000) / HOLD_PALETTES_X_TIMES_AS_LONG) % 60;
  static uint8_t lastSecond = 99;
  
  if( lastSecond != secondHand) {
    lastSecond = secondHand;
    if( secondHand ==  0)  { currentPalette = RainbowColors_p;         speed = 10; scale = 30; colorLoop = 1; }
    if( secondHand ==  5)  { SetupPurpleAndGreenPalette();             speed = 10; scale = 50; colorLoop = 1; }
    if( secondHand == 10)  { SetupBlackAndWhiteStripedPalette();       speed = 10; scale = 30; colorLoop = 1; }
    if( secondHand == 15)  { currentPalette = ForestColors_p;          speed =  8; scale = 50; colorLoop = 1; }
    if( secondHand == 20)  { currentPalette = CloudColors_p;           speed =  4; scale = 30; colorLoop = 0; }
    if( secondHand == 25)  { currentPalette = LavaColors_p;            speed =  8; scale = 50; colorLoop = 0; }
    if( secondHand == 30)  { currentPalette = OceanColors_p;           speed = 10; scale = 50; colorLoop = 0; }
    if( secondHand == 35)  { currentPalette = PartyColors_p;           speed = 10; scale = 30; colorLoop = 1; }
    if( secondHand == 40)  { SetupRandomPalette();                     speed = 10; scale = 20; colorLoop = 1; }
    if( secondHand == 45)  { SetupRandomPalette();                     speed = 20; scale = 50; colorLoop = 1; }
    if( secondHand == 50)  { SetupRandomPalette();                     speed = 10; scale = 90; colorLoop = 1; }
    if( secondHand == 55)  { currentPalette = RainbowStripeColors_p;   speed = 10; scale = 20; colorLoop = 1; }
  }
}

void SetupPurpleAndGreenPalette()
{
  CRGB purple = CHSV( HUE_PURPLE, 255, 255);
  CRGB green  = CHSV( HUE_GREEN, 255, 255);
  CRGB black  = CRGB::Black;
  
  currentPalette = CRGBPalette16( 
    green,  green,  black,  black,
    purple, purple, black,  black,
    green,  green,  black,  black,
    purple, purple, black,  black );
}


void SetupRandomPalette()
{
  currentPalette = CRGBPalette16( 
                      CHSV( random8(), 255, 32), 
                      CHSV( random8(), 255, 255), 
                      CHSV( random8(), 128, 255), 
                      CHSV( random8(), 255, 255)); 
}

int Sect1 = 0;
int Sect2 = 0;
int Sect3 = 0;
int Sect4 = 0;

// heart effect
void SectionGlitchesHeart(){

  Sect1= random(18,80);
  Sect2= random(NUM_LEDS-Sect1);
  Sect3= random(NUM_LEDS - Sect1 - Sect2);
  Sect4= NUM_LEDS-Sect1-Sect2-Sect3;

  for( byte y = 0; y < kMatrixHeight; y++) {
    for( byte x = 0; x < kMatrixWidth; x++) {
      if(XY(x,y)<=Sect1){
        if(tv[y][x]){
          leds[ XY(x, y)]  = CHSV( cHue, cSat, cVal);
        }
        else{
          leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
        }
      }
      else if(XY(x,y)<=Sect1+Sect2){
        cHue= Sect1+Sect2;
        cbHue=cHue+120;
        
        if(tv[y][x]){
          leds[ XY(x, y)]  = CHSV( cHue, cSat, cVal);
        }
        else{
          leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
        }
      }
      else if(XY(x,y)<=Sect1+Sect2+Sect3){
        cHue= Sect1+Sect2+Sect3;
        cbHue=cHue+120;
        
        if(tv[y][x]){
          leds[ XY(x, y)]  = CHSV( cHue, cSat, cVal);
        }
        else{
          leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
        }
      }
      else{
        cHue= Sect4*5;
        cbHue=cHue+120;
        
        if(tv[y][x]){
          leds[ XY(x, y)]  = CHSV( cHue, cSat, cVal);
        }
        else{
          leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
        }
      }
      
    }  
  }
}

void randomNoiseHeart(){
 // rework to fade between colors
for( byte y = 0; y < kMatrixHeight; y++) {
    for( byte x = 0; x < kMatrixWidth; x++) {
        if(tv[y][x]){
          leds[ XY(x, y)]  = CHSV( random(0,254), cSat, cVal);
        }
        else{
          leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
        }
    }  
  }
}

void tvOutlineDisp(){
  heart();
  for( byte y = 0; y < kMatrixHeight; y++) {
    for( byte x = 0; x < kMatrixWidth; x++) {
      if(tvMiddle[y][x]){
        leds[ XY(x, y)]  = CHSV( cbHue, cSat, cVal);
        //leds[ XY(x, y)]  = CHSV( cHue+160, cSat, cVal);
      }
    } 
  }
}

void mapNoiseToHeartWithOutline()
{
  static uint8_t ihue=0;
  
  for(int i = 0; i < kMatrixWidth; i++) {
    for(int j = 0; j < kMatrixHeight; j++) {
      // We use the value at the (i,j) coordinate in the noise
      // array for our brightness, and the flipped value from (j,i)
      // for our pixel's index into the color palette.

      uint8_t index = noise[j][i];
      uint8_t bri =   noise[i][j];

      // if this palette is a 'loop', add a slowly-changing base value
      if( colorLoop) { 
        index += ihue;
      }

      // brighten up, as the color palette itself often contains the 
      // light/dark dynamic range desired
      if( bri > 127 ) {
        bri = 255;
      } else {
        bri = dim8_raw( bri * 2);
      }

      CRGB color = ColorFromPalette( currentPalette, index, bri);
      if(tvMiddle[j][i]){
        leds[XY(i,j)] = color;
      }
    }
  }
  
  ihue+=1;
}

//Heart burst color animation

//tornado data animation

//
void BlackLivesMatterHeart(){
  for( byte y = 0; y < kMatrixHeight; y++) {
    for( byte x = 0; x < kMatrixWidth; x++) {
      if(tv[y][x]){
        leds[ XY(x, y)]  = CHSV(200, 150, 200);
      }
      else{
        leds[XY(x,y)]=CHSV(200,255,255);
      }
      if(blm[y][x]){
        leds[ XY(x, y)]  = CHSV( 200, 0, 255);
      }
    }  
  }
}

#define shift 30

void seawave2(){
  
  CRGB oceancolor[4] = {0x003136, 0x2c8fa3, 0x9ec6d0, 0x4c738f}; //rgb color from bright to dark. 
  fill_solid (ledsbuff, kMatrixWidth*kMatrixHeight,   oceancolor[0]);  // first color ,  ledbuff is buffer. define CRGB ledsbuff[NUM_LEDS];
  for (byte i = 0; i < kMatrixWidth; i++) {     
    for (byte z = 0; z < 3; z++) {                                               
      byte   sinus = beatsin8 (30, 1, 4, 0, i * 256 / 45 + z * shift ) + z * 3; // 3 waves with shift
      for (byte k = sinus; k < kMatrixHeight; k++) {
        ledsbuff[k * 30 + i] =  oceancolor[z+1]; // draw sin wave with his color in buffer 
      }
    }
  }
 
  fadeToBlackBy( ledsbuff, kMatrixWidth*kMatrixHeight, 200);   
  blur2d( ledsbuff, kMatrixWidth, kMatrixHeight, 16);  
 
  for (byte i = 0; i < kMatrixHeight; i++) {                              //  cycle for move ledbuffer to leds with your layout
    for (byte k = 0; k < kMatrixWidth; k++) {
      leds[XY(k, i)] = ledsbuff[(kMatrixHeight*kMatrixWidth) - (i * 30) + k];     // change findNumByCoord (k, i) to your xy routine       
      if(tv[i][k]){
        leds[XY(k, i)]+=CHSV( 200, 225, 150);
      }                              
    }
  }
}

void seawave3(){
  
  CRGB oceancolor[4] = {0x003136, 0x2c8fa3, 0x9ec6d0, 0x4c738f}; //rgb color from bright to dark. 
  fill_solid (ledsbuff, kMatrixWidth*kMatrixHeight,   oceancolor[0]);  // first color ,  ledbuff is buffer. define CRGB ledsbuff[NUM_LEDS];
  for (byte i = 0; i < kMatrixWidth; i++) {     
    for (byte z = 0; z < 3; z++) {                                               
      byte   sinus = beatsin8 (30, 1, 4 , 0, i * 256 / 45 + z * shift ) + z * 3; // 3 waves with shift
      for (byte k = sinus; k < kMatrixHeight; k++) {
        ledsbuff[k * 39 + i] =  oceancolor[z+1]; // draw sin wave with his color in buffer 
      }
    }
  }
 
  fadeToBlackBy( ledsbuff, kMatrixWidth*kMatrixHeight, 200);   
  blur2d( ledsbuff, kMatrixWidth, kMatrixHeight, 16);  
 
  for (byte i = 0; i < kMatrixHeight; i++) {                              //  cycle for move ledbuffer to leds with your layout
    for (byte k = 0; k < kMatrixWidth; k++) {
      leds[XY(k, i)] = ledsbuff[312 - (i * 39) + k];     // change findNumByCoord (k, i) to your xy routine                                    
    }
  }
}


void glitch_side_stutter(){
  if(flicker == true && flickoverRide == false){
    FastLED.delay(50);
  }
 /* if(flicker == true && flickoverRide == false){
    FastLED.clear(true);
    delay(250);
    if(flick<20){
      flick++;
    }
    else{
      flick=0;
      flicker = false;
    }
  }
  else if(flickoverRide == true){
    if(flick<2500000){
      flick++;
    }
    else{
      flick=0;
      flickoverRide = false;
    }
  }*/
}