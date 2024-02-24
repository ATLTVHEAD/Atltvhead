/* Atltvhead.ino - Arduino code of how atltvhead helmet runs
 * Created by Nate Damen, 2015 
 * Apache License Version 2.0
 * Updated to use MQTT 
 * Updated 2_24_2024
 */

 #import <FastLED.h>

int cbHue = 135;
int cbSat = 255;
int cbVal = 255;
int bright=250;
uint8_t sprand=0;

#define CHIPSET WS2812B
#define PIN 25
#define COLOR_ORDER GRB

// Params for width and height
const uint8_t kMatrixWidth = 34;
const uint8_t kMatrixHeight = 8;


#define NUM_LEDS ((kMatrixWidth * kMatrixHeight))
#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)
CRGB leds[NUM_LEDS];

// use these to create 2 different buffers for gradient fills
CHSV heartbuff[NUM_LEDS];
CHSV bgbuff[NUM_LEDS];

int sIndex = 0;
int eIndex = NUM_LEDS - 1;

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

bool tv[8][34]={
  {0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0}};



void setup() {
  // From the FASTLED CODE
  //---------------------------------------------
  FastLED.addLeds<CHIPSET, PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(bright);
  FastLED.clear(true);

  fill_gradient(heartbuff,sIndex,CHSV(200,254,254), eIndex ,CHSV(0,254,254),SHORTEST_HUES);
  fill_gradient(bgbuff,sIndex,CHSV(135,255,255), eIndex ,CHSV(145,254,254),SHORTEST_HUES);



  gradHeart();
  displayScreen();
}

void loop(){
  //do nothing for now
  gradHeart();
  delay(60);
  displayScreen();

 
  // More work to be done here to make the fill rotation function properly
  //rotateHeartFills();
}

void gradHeart(){
    for(byte y=0; y < kMatrixHeight;y++){
     for(byte x=0; x<kMatrixWidth;x++){
       sprand = random(100);
       if(tv[y][x]){
         //do nothing
         if (sprand < 3){
          leds[XY(x,y)]=CHSV(cbHue,0,cbVal);
         }
         else{
          leds[XY(x,y)]=heartbuff[XY(x,y)];
         }
        }
       else{
         leds[XY(x,y)]=bgbuff[XY(x,y)];
       }
     }
   }
}

void rotateFills(int start, int ending){
  fill_gradient(heartbuff,start,CHSV(200,254,254), ending,CHSV(0,254,254),SHORTEST_HUES);
  fill_gradient(bgbuff,start,CHSV(135,255,255), ending,CHSV(150,254,254),SHORTEST_HUES);
}

void rotateHeartFills(){
  CHSV oldStartColor = heartbuff[sIndex];
  CHSV oldEndColor = heartbuff[eIndex];
  computeSIndex();
  computeEIndex();
  fill_gradient(heartbuff,sIndex,CHSV(200,254,254), NUM_LEDS - 1, oldEndColor,SHORTEST_HUES);
  fill_gradient(heartbuff,0,oldStartColor, eIndex,CHSV(0,254,254),SHORTEST_HUES);
}

void computeSIndex(){
  sIndex++;
  if(sIndex == NUM_LEDS - 1){
    sIndex = 0;
  }
}

void computeEIndex(){
  eIndex = sIndex - 1;
  if(sIndex == 0){
    eIndex = NUM_LEDS - 1;
  }
}


void displayScreen(){
  FastLED.show();
}
