#define FASTLED_ESP8266_RAW_PIN_ORDER
//#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
//#include <Bounce2.h>
//#include <Encoder.h>
#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>
#include <ESP8266WiFi.h>
#include <IRCClient.h>
#include <Math.h> 


//------------------------------------------------------------------------------------------------------//
//wifi login setup

#define ssid1         ""
#define password1     ""

#define ssid2         ""
#define password2     ""

#define ssid3          ""
#define password3      ""

char pasnum='B';

//Setting up the IRC Twitch Server information

#define IRC_SERVER   "irc.chat.twitch.tv"
#define IRC_PORT     6667
#define IRC_NICK     ""
#define IRC_CHAN     ""
#define IRC_PASS     ""

//naming wifi and server client

WiFiClient wiFiClient;
IRCClient client(IRC_SERVER, IRC_PORT, wiFiClient);

//----------------------------------------------------------------------------------------------------//

int posin = 0;
int posinold = 0;
//----------------------------------------------------------------------------------------------------//
#define CHIPSET WS2812B
#define PIN D3
#define COLOR_ORDER GRB

int bright=100;

// Params for width and height
const uint8_t kMatrixWidth = 18;
const uint8_t kMatrixHeight = 9;

#define NUM_LEDS ((kMatrixWidth * kMatrixHeight)-4)
CRGB leds[NUM_LEDS];


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
#define MATRIX_WIDTH   18
#define MATRIX_HEIGHT  7
#define MATRIX_TYPE    HORIZONTAL_ZIGZAG_MATRIX

cLEDMatrix<MATRIX_WIDTH, -MATRIX_HEIGHT, MATRIX_TYPE> LEDs;

cLEDText ScrollingMsg;



int CurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t chanel = 1;
uint32_t ms = 0;
uint32_t yHueDelta32 = 0;
uint32_t xHueDelta32 = 0;
uint32_t hue = 0;
uint8_t angle = 0;

//-------------------------

const unsigned char txt[] = {EFFECT_SCROLL_LEFT "          W H A T E V E R     M A K E S     Y O U ,   Y O U !                   "};
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

boolean fullrainbow = false;
unsigned int flick =0;
boolean flickoverRide = false;
unsigned int raincount = 0;
uint8_t sprand=0;
//--------------------------------------------

bool tv[9][18]={
  {0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
  {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 };


 

//---------------------------------------------------------------------------------------------------//

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


boolean channelSwitch = false;
boolean MLF = false;



void setup() {
// From the FASTLED CODE
  //---------------------------------------------
  FastLED.addLeds<CHIPSET, PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050); 
  FastLED.setBrightness( bright );
  //FastLED.clear(true);
  heart();  
  displayScreen();

  delay(100);

  //Serial.begin(115200);
  //---------------------
  
  ScrollingMsg.SetFont(MatriseFontData);
  ScrollingMsg.Init(&LEDs, LEDs.Width(), ScrollingMsg.FontHeight()+1, 0, 0);
  ScrollingMsg.SetText((unsigned char *)txt, sizeof(txt) - 1);
  ScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0x00, 0xff);

  //  FROM THE IRC TWITCHBOT CODE
  //-----------------------------------------------------------------------

  
  pinMode(LED_BUILTIN, OUTPUT);

  delay(100);

// add way to autochange wifi networks
  
  WiFi.begin(ssid1, password1);

  while (WiFi.status() != WL_CONNECTED) {
    
    digitalWrite(LED_BUILTIN, LOW);
    demonDelay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    demonDelay(250);

    client.setCallback(callback); 
    client.setSentCallback(debugSentCallback);
 
    if(wificount>15){
        WiFi.disconnect();
        digitalWrite(LED_BUILTIN,LOW);
        break; 
    }
    ++wificount; 
  }
}






void loop() {
  
  if(WiFi.status() != WL_CONNECTED){
    
    digitalWrite(LED_BUILTIN, HIGH);
    switch (pasnum){
      case 'A':
        WiFi.begin(ssid1, password1);
        break;
      case 'B':
        WiFi.begin(ssid2, password2);
        break;
    }
   
    while (WiFi.status() != WL_CONNECTED) {
    
      digitalWrite(LED_BUILTIN, LOW);
      demonDelay(250);
      digitalWrite(LED_BUILTIN, HIGH);
      demonDelay(250);

      client.setCallback(callback); 
      client.setSentCallback(debugSentCallback);
 
      if(wificount>15){
          WiFi.disconnect();
          digitalWrite(LED_BUILTIN,LOW);
          wificount = 0;
          heart();
          displayScreen();
          if(pasnum =='A'){
            pasnum='B';
          }
          else{
            pasnum='A';
          }
          
          break; 
      }
      ++wificount; 
    }
  }

  
  // getting the Chat going and reading 
  //---------------------------------------------------------
  
  if (!client.connected()) {
    Serial.println("Attempting IRC connection...");
    // Attempt to connect
    if (client.connect(IRC_NICK, IRC_CHAN, IRC_PASS)) {
    //Serial.println("connected");
      client.sendMessage(IRC_CHAN, "Hello everyone! I'm TvheadBot, a construct, assistant within the head of Atltvhead. If you have any questions type !help , and I'll post a link to all the channel commands. Let's Tune into Good Vibes! <3");
    } else {
      //Serial.println("failed... try again in 5 seconds");
      // Wait 5 seconds before retrying
      demonDelay(5000);
    }
    return;
  }
  client.loop();
  
  ms = millis();
  yHueDelta32 = ((int32_t)cos16( ms * (27/1) ) * (350 / kMatrixWidth));
  xHueDelta32 = ((int32_t)cos16( ms * (39/1) ) * (310 / kMatrixHeight));



  if(flicker == true && flickoverRide == false){
    FastLED.clear(true);
    channelSwitch = true;
    demonDelay(250);
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
  }

//65536
switch(chanel){
    case 2:
      DrawOneFrame( ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
      if(!fullrainbow){
        superRainbowHeart();
      }
      else if(fullrainbow){
        if(raincount <250){
          raincount++;
        }
        else{
          raincount = 0;
          fullrainbow = false;
        }
      }
      if(MLF==true){
        mirrorLeftToRight();
      }
      
      displayScreen();
      break;
      
    case 4:
      sparkles();
      demonDelay(50);
      if(MLF==true){
        mirrorLeftToRight();
      }
      displayScreen();
      break;
      
    case 0:
        FastLED.clear(true);
      break;
      
    case 5:
        
        if(MLF==true){
          mirrorLeftToRight();
        }
        displayScreen();  
      break;
      
    case 1:
      if(channelSwitch == true){
          //Serial.println("I am inside the heart command");
          channelSwitch = false;
          heart();
          demonDelay(250);
          displayScreen();
        }
      break;
  }
  
  //Serial.println(channelSwitch);
  posinold = posin;
}










//------------------------------------------------------------------------------------------------------
// this is where the commands for chat are located

void callback(IRCMessage ircMessage) {
//Serial.println("In CallBack");
  // PRIVMSG ignoring CTCP messages
  if (ircMessage.command == "PRIVMSG" && ircMessage.text[0] != '\001') {
    //Serial.println("Passed private message.");
    String message("<" + ircMessage.nick + "> " + ircMessage.text);
    Serial.println(message);
    ///////////////////////////////////////////////////////////////////////////////////////////
    if(ircMessage.text == "!atltvhead"){
      client.sendMessage(IRC_CHAN, "Atltvhead is a force of positivity in Atlanta. Together we can build a community of love, peace, and encouragement.");
    }
    else if(ircMessage.text == "!ch0" && ircMessage.nick == "atltvhead"){
      chanel = 0;
      client.sendMessage(IRC_CHAN, ircMessage.nick + " set atltvhead to the secret 0!");
      //Serial.println(chanel);
    }
    else if(ircMessage.text == "<3"){
      chanel = 1;
      channelSwitch = true;
      
      if(heartcount >= 2){
      client.sendMessage(IRC_CHAN, ircMessage.nick + " shows their heart! Thank you! Chat, you've shown your heart! YOU ARE AWESOME! THANK YOU!!!!!!!!!!");
       for(int ppgLooper =0; ppgLooper <= 3; ppgLooper++){
        for(int indPPG =0; indPPG<=5;indPPG++){
          ppg(indPPG);
        }
       }
        heartcount = 0;
        channelSwitch = true;
      }
      else{
        hcind = invheartc - heartcount;
        client.sendMessage(IRC_CHAN, ircMessage.nick + " shows their heart! Thank you! Chat, we need " + hcind + " more people to show their '!heart'. LET'S DO THIS!");
        heartcount++;
      }
    }
    else if(ircMessage.text == "1" && ircMessage.nick == "atltvhead"){
      //client.sendMessage(IRC_CHAN, ircMessage.nick + " It's PowerPuff Girl's Heart Time!");     
       for(int ppgLooper =0; ppgLooper <= 3; ppgLooper++){
        for(int indPPG =0; indPPG<=5;indPPG++){
          ppg(indPPG);
        }
       }
       channelSwitch = true;
    }
    else if(ircMessage.text == "!flicker"){
      flicker = true;
      client.sendMessage(IRC_CHAN, ircMessage.nick + " is messing with my signals! Someone help!");
    }
    else if(ircMessage.text == "!flickerOff"){
      flickoverRide = true;
      client.sendMessage(IRC_CHAN, ircMessage.nick + " has saved me!!! Virtual hug, just for you!");
    }
    else if(ircMessage.text == "!brighter"){
      bright = bright +50;
      FastLED.setBrightness(bright);
      channelSwitch = true;
    }
     else if(ircMessage.text == "!dimmer"){
      bright = bright -50;
      FastLED.setBrightness(bright);
      channelSwitch = true;
    }
    else if(ircMessage.text =="!heartColor"){
         changeHeartHue();
         channelSwitch = true;
    }
    else if(ircMessage.text =="!heartSat"){
         changeHeartSat();
         channelSwitch = true;
    }
    else if(ircMessage.text =="!heartBright"){
         changeHeartVal();
         channelSwitch = true;
    }
    else if(ircMessage.text =="!backgroundColor"){
         changeBackHue();
         channelSwitch = true;
    }
    else if(ircMessage.text =="!backgroundSat"){
         changeBackSat();
         channelSwitch = true;
    }
    else if(ircMessage.text =="!backgroundBright"){
         changeBackVal();
         channelSwitch = true;
    }
    else if(ircMessage.text =="!reset"){
      channelSwitch = true;
      resetHeart();
    }
    else if(ircMessage.text == "!rainbowHeart"){
      chanel = 2;
      client.sendMessage(IRC_CHAN, ircMessage.nick + " set atltvhead to rainbow heart!");
    }
    else if(ircMessage.text =="!fullRainbow"){
      client.sendMessage(IRC_CHAN, ircMessage.nick + " has gone FULL RAINBOW!!");
      
      fullrainbow = true;
    }
    else if(ircMessage.text =="!Merica" && ircMessage.nick == "atltvhead"){
      channelSwitch = true;
      //america();
      resetHeart();
    }
    else if(ircMessage.text =="!mirrorMirror"){
      MLF = true;
    }
    else if(ircMessage.text =="!mirrorOff"){
      MLF = false;
    }
    else if(ircMessage.text =="!sparkles"){
      chanel = 4;
      client.sendMessage(IRC_CHAN,ircMessage.nick + " has made me into a Twilight Vampire? Team Bella!");
    }
    else if(ircMessage.text =="High Five Mode Initiated"){
      color = cHue;
       for(int ppgLooper =0; ppgLooper <= 3; ppgLooper++){
        for(int indPPG =0; indPPG<=5;indPPG++){
          ppg(indPPG);
        }
       }
       color = 211;
       chanel = 4;
       channelSwitch = true;
    }
    
    
    

    return;
  }
 }




 

void debugSentCallback(String data) {
  //Serial.println("I am in debug");
  Serial.println(data);
}

void ban(String username){
  String whotoban = "";
  whotoban = ".ban " + username;
  client.sendMessage(IRC_CHAN, whotoban);
}

void timeout(String uname){
  String dontbedumb = "";
  dontbedumb = ".timeout " + uname;
  client.sendMessage(IRC_CHAN, dontbedumb);
}

//--------------------------------------------------------------------------------------------------
// these are the tvhead helper functions. 





//-----------------------------------------------------------------------------------------------------------------------------------
//_________________________________________________________________________________________
// heart effect
void heart(){
  for( byte y = 0; y < kMatrixHeight; y++) {    
    for( byte x = 0; x < kMatrixWidth; x++) {
     if( XY(x,y)<=126){
        if(tv[y][x]){
          leds[ XY(x, y)]  = CHSV( cHue, cSat, cVal);
        }
        else{
          leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
        }
      }
      
      else if(XY(x,y)>126 && XY(x,y)<142){
        if(tv[y][x]){
        leds[XY(x,y)-1] = CHSV( cHue, cSat,cVal);
        }
        else{
          leds[XY(x,y)-1]=CHSV(cbHue,cbSat,cbVal);
        }
      }
      
      else{
        if(tv[y][x]){
        leds[XY(x,y)-3]=CHSV(cHue,cSat,cVal);
        }
        else{
          leds[XY(x,y)-3]=CHSV(cbHue,cbSat,cbVal);
        }
      }
    }
  }
}

void changeHeartHue(){
  cHue = cHue + 25;
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
  cbHue = cbHue + 25;
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
      demonDelay(75);
      leds[XY( 8, 3)] = CHSV(color,sat,255);
      leds[XY( 8, 4)] = CHSV(color,sat,255);
      leds[XY( 9, 3)] = CHSV(color,sat,255);
      leds[XY( 9, 4)] = CHSV(color,sat,255);
      }
    else if(frame==1){

      leds[XY( 6, 3)] = CHSV(color,sat,255);
      leds[XY( 7, 3)] = CHSV(color,sat,255);

      leds[XY( 10, 3)] = CHSV(color,sat,255);
      leds[XY( 11, 3)] = CHSV(color,sat,255);

      leds[XY( 7, 4)] = CHSV(color,sat,255);
      leds[XY( 10, 4)] = CHSV(color,sat,255);

      leds[XY( 8, 5)] = CHSV(color,sat,255);
      leds[XY( 9, 5)] = CHSV(color,sat,255);
      }
      
    else if(frame==2){

      leds[XY( 5, 1)] = CHSV(color,sat,255);
      leds[XY( 6, 1)] = CHSV(color,sat,255);

      leds[XY( 11, 1)] = CHSV(color,sat,255);
      leds[XY( 12, 1)] = CHSV(color,sat,255);
      
      for(int xj = 4; xj<14; xj++){
        leds[XY( xj, 2)] = CHSV(color,sat,255);
        yield();
        }
        leds[XY( 4, 3)] = CHSV(color,sat,255);
        leds[XY( 5, 3)] = CHSV(color,sat,255);

        leds[XY( 12, 3)] = CHSV(color,sat,255);
        leds[XY( 13, 3)] = CHSV(color,sat,255);

        leds[XY( 5, 4)] = CHSV(color,sat,255);
        leds[XY( 6, 4)] = CHSV(color,sat,255);

        leds[XY( 11, 4)] = CHSV(color,sat,255);
        leds[XY( 12, 4)] = CHSV(color,sat,255);

        leds[XY( 6, 5)] = CHSV(color,sat,255);
        leds[XY( 7, 5)] = CHSV(color,sat,255);

        leds[XY( 10, 5)] = CHSV(color,sat,255);
        leds[XY( 11, 5)] = CHSV(color,sat,255);
        
        for(int xi = 7; xi<12; xi++){
          leds[XY( xi, 6)] = CHSV(color,sat,255);
          yield();
        } 
      }
      
    else if(frame==3){
      leds[XY( 5, 0)] = CHSV(color,sat,255);
      leds[XY( 6, 0)] = CHSV(color,sat,255);
      leds[XY( 11, 0)] = CHSV(color,sat,255);
      leds[XY( 12, 0)] = CHSV(color,sat,255);

      leds[XY( 4, 1)] = CHSV(color,sat,255);
      leds[XY( 7, 1)] = CHSV(color,sat,255);
      leds[XY( 10, 1)] = CHSV(color,sat,255);
      leds[XY( 13, 1)] = CHSV(color,sat,255);

      leds[XY( 3, 2)] = CHSV(color,sat,255);
      leds[XY( 14, 2)] = CHSV(color,sat,255);

      leds[XY( 3, 3)] = CHSV(color,sat,255);
      leds[XY( 14, 3)] = CHSV(color,sat,255);

      leds[XY( 4, 4)] = CHSV(color,sat,255);
      leds[XY( 13, 4)] = CHSV(color,sat,255);

      leds[XY( 5, 5)] = CHSV(color,sat,255);
      leds[XY( 12, 5)] = CHSV(color,sat,255);

      leds[XY( 6, 6)] = CHSV(color,sat,255);
      leds[XY( 11, 6)] = CHSV(color,sat,255);

      leds[XY( 8, 7)] = CHSV(color,sat,255);
      leds[XY( 9, 7)] = CHSV(color,sat,255);
      }
      
    else if(frame==4){
      for(int xk = 3; xk<16; xk++){
          leds[XY( xk, 0)] = CHSV(color,sat,255);
          yield();
        } 

      for(int xl = 2; xl<17; xl++){
          leds[XY( xl, 1)] = CHSV(color,sat,255);
          yield();
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
    demonDelay(70);
}



void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
  byte lineStartHue = startHue8;
  for( byte y = 0; y < kMatrixHeight; y++) {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;      
    for( byte x = 0; x < kMatrixWidth; x++) {
      pixelHue += xHueDelta8;
      if( XY(x,y)<126){
          leds[ XY(x, y)]  = CHSV( pixelHue+hue, 255, 255);
      }
      
      else if(XY(x,y) == 126){
        leds[ XY(x, y)]  = CHSV( pixelHue+hue, 255, 255);
      }
      
      else if(XY(x,y)>126 && XY(x,y)<142){
        leds[XY(x,y)-1] = CHSV( pixelHue+hue, 255,255);
      }
      
      else{
        leds[XY(x,y)-3]=CHSV(pixelHue+hue,255,255);
      }
    }
  }
}


void displayScreen(){
  if(chanel != 3){
      FastLED.show();
  }
  else if(chanel ==3 && ScrollingMsg.UpdateText() !=-1){
    FastLED.show();
    demonDelay(75);
  }
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
      
     if( XY(x,y)<=126){
        if(tv[y][x]){
          yield();
        }
        else{
          leds[XY(x,y)]=CHSV(cbHue,cbSat,cbVal);
        }
      }
      
      else if(XY(x,y)>126 && XY(x,y)<142){
        if(tv[y][x]){
          yield();
        }
        else{
          leds[XY(x,y)-1]=CHSV(cbHue,cbSat,cbVal);
        }
      }
      
      else{
        if(tv[y][x]){
          yield();
        }
        else{
          leds[XY(x,y)-3]=CHSV(cbHue,cbSat,cbVal);
        }
      }
    }
  }
}



void mirrorLeftToRight(){
  // Here we are setting the bottom led's to their top led counter part -> take into consideration the 3 missing pixels on the bottom
  //row and the one missing pixel on the second to bottom row just like in the Heart settings! 

for(byte y = 0; y < kMatrixHeight; y++){
  for(byte x = 0; x < 9; x++){
      if( XY(x,y)<126){
          leds[ XY(x, y)] = leds[ XY(17-x,y) ];
      }  
      else if(XY(x,y) == 126){
        leds[ XY(x, y)]  = leds[XY(17-x,y)];
      }
      else if(XY(x,y)>126 && XY(x,y)<142){
        leds[XY(x,y)-1] = leds[XY(17-x+1,y)];
      }
      else{
        if(x<3){
          leds[XY(x,y)-3]=leds[XY(17-x,y)];
        }
        else{
          leds[XY(x,y)]=leds[XY(17-x,y)];
        }
      }
    }
  }
}


void sparkles(){
  for(byte y=0; y < kMatrixHeight;y++){
    for(byte x=0; x<kMatrixWidth;x++){
      sprand = random(100);
      if( XY(x,y)<=126){
        if(tv[y][x]){
          if(sprand < 15){
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
      
      else if(XY(x,y)>126 && XY(x,y)<142){
        if(tv[y][x]){
          if(sprand < 15){
           leds[XY(x,y)-1]=CHSV(cHue,0,cVal);
          }
          else{
            leds[XY(x,y)-1]=CHSV(cHue,cSat,cVal);
          }
        }
        else{
          leds[XY(x,y)-1]=CHSV(cbHue,cbSat,cbVal);
        }
      }
      
      else{
        if(tv[y][x]){
          if(sprand < 15){
           leds[XY(x,y)-3]=CHSV(cHue,0,cVal);
          }
          else{
            leds[XY(x,y)-3]=CHSV(cHue,cSat,cVal);
          }
        }
        else{
          leds[XY(x,y)-3]=CHSV(cbHue,cbSat,cbVal);
        }
      } 
    }
  } 
}



