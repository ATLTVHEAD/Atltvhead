#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include <Bounce2.h>
#include <Encoder.h>
#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>
#include <ESP8266WiFi.h>
#include <IRCClient.h>


//------------------------------------------------------------------------------------------------------//
//wifi login setup

#define ssid1         "IgotthePoops"
#define password1     "AcerbiNimbus"

#define ssid2         "DestructionDynamics"
#define password2     "CealiNimbus"

#define ssid3          "ATT8Jmy7qw"
#define password3      "7pbw6k?shf9p"

//Setting up the IRC Twitch Server information

#define IRC_SERVER   "irc.chat.twitch.tv"
#define IRC_PORT     6667
#define IRC_NICK     "tvheadbot"
#define IRC_CHAN     "#atltvhead"
#define IRC_PASS     "oauth:0hr40w69fcqdgl8frc4ti09i5g3s3w"

//naming wifi and server client

WiFiClient wiFiClient;
IRCClient client(IRC_SERVER, IRC_PORT, wiFiClient);

//----------------------------------------------------------------------------------------------------//

//positive message index
int posin = 0;
int posinold = 0;

//----------------------------------------------------------------------------------------------------//
#define CHIPSET WS2812B
#define PIN D3
#define COLOR_ORDER GRB

int BRIGHTNESS=50;

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
//------------------------

#define BUTTON D7               // Defines the Encoder Button to a pin
bool bState = true;             // Define the State of the button toggle
int b = LOW;                    // State value for the button being pressed 
long oldPosition  = -999;       // setting old position for encoder
long newPosition;               // setting new position for encoder


//#define ENCODER_DO_NOT_USE_INTERRUPTS
Encoder myEnc(D5, D6);  // Encoder set up on interupt pins. 
bool dir =true;
bool forward = true;
bool back = false;

Bounce bouncer = Bounce();  // Setting up the debounce function to the button and calling the output bouncer



int CurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t chanel = 1;
uint32_t ms = 0;
uint32_t yHueDelta32 = 0;
uint32_t xHueDelta32 = 0;
uint32_t hue = 0;
uint8_t angle = 0;

//-------------------------

const unsigned char txt[] = {EFFECT_SCROLL_LEFT "          W H A T E V E R     M A K E S     Y O U ,   Y O U     E M B R A C E     I T !                   "};
//const unsigned char txt[] = {"what up"};
char inChar;
int in = 0;
bool TW = false;
bool mh = false;
bool mv = false;
bool mq = false;
bool mqr = false;
int Hue = 0;


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








void setup() {
  Serial.begin(115200);

  delay(100);

// From the FASTLED CODE
  //---------------------------------------------
  FastLED.addLeds<CHIPSET, PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050); 
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.clear(true);
  delay(100);
    // setting up the Button mode and the led disp to begin. 
  pinMode(BUTTON,INPUT_PULLUP);
  bouncer.attach(BUTTON);
  bouncer.interval(5); // interval in ms

  delay(100);

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
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);

  client.setCallback(callback); 
  client.setSentCallback(debugSentCallback);
  ++wificount; 
  }

  //-----------------------------------------------------------------------//

}




















void loop() {
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
      delay(5000);
    }
    return;
  }
  client.loop();
  
  ms = millis();
  yHueDelta32 = ((int32_t)cos16( ms * (27/1) ) * (350 / kMatrixWidth));
  xHueDelta32 = ((int32_t)cos16( ms * (39/1) ) * (310 / kMatrixHeight));

  newPosition = myEnc.read();
  dir = setDir();
  //chanel = setchanel();
  BRIGHTNESS = brightVal(BRIGHTNESS);

if(flicker == true){
  FastLED.clear(true);
}

switch(chanel){
    case 2:
      //chanel =1;
      //DrawOneFrame( ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
      break;
    case 0:
        FastLED.clear(true);
      break;
    case 1:
      heart();
      break;
    case 3:
        //I need to move the message across all 18 columns. meaning I need to get the length of the txt and make sure it moves all 18 across
          if (ScrollingMsg.UpdateText() == -1){
               ScrollingMsg.SetText((unsigned char *)txt, sizeof(txt) - 1);
          }
      break;
  }
  
    
  if( ms < 5000 ) {
    FastLED.setBrightness( scale8( BRIGHTNESS, (ms * 256) / 5000));
  } else {
    FastLED.setBrightness(BRIGHTNESS);
  }

  displayScreen();

  if ( bouncer.update()) {
     if ( bouncer.read() == HIGH) {
      //state change here
      bState=!bState;      
     }
   } 
  oldPosition = newPosition;
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
	  if (ircMessage.text == "!love"){
            client.sendMessage(IRC_CHAN,"TvheadBot loves " + ircMessage.nick);
	  }
    else if(ircMessage.text == "!hug"){
          client.sendMessage(IRC_CHAN,"TvheadBot hugs " + ircMessage.nick);
    }
    else if(ircMessage.text == "!atltvhead"){
      client.sendMessage(IRC_CHAN, "Atltvhead is a force of positivity in Atlanta. Together we can build a community of love, peace, and encouragement.");
    }
    else if(ircMessage.text == "!help"){
      client.sendMessage(IRC_CHAN, "Looking for the chat commands? Scroll to the bottom of the page on https://www.twitch.com/atltvhead");
    }
    else if(ircMessage.text =="!tvbot"){
      client.sendMessage(IRC_CHAN, "Hello " + ircMessage.nick + "! I'm TvheadBot, atltvhead's assistant and muscle power. I am the brains inside the tvhead, and am the controller behind the screen. If you'd like to manipulate the tvhead, just let me know what command you'd like. If you'd like a hug, I can do that. If you'd like a word of encouragment, I am here for you.");
    }
    else if(ircMessage.text == "!howtoshare"){
      client.sendMessage(IRC_CHAN, "I see you'd like to submit some words of encouragement, a story of struggle and overcoming, or even some goals you'd like to see this community work on! Plz whipser atltvhead with the story right in chat (/w atltvhead message).  You can also email atltvhead at atltvhead.com");
    }
    else if(ircMessage.text == "!goodvibes"){
      while (posin == posinold){
        posin = random(0,6);  // CHANGE ME FOR MORE MESSAGES
       //Serial.println("Im in the while loop");                                  // ----------------------------------------------------!!!!!!!!!!!!!    FIX ME CHANGE THE MESSAGE ---------------------------------------
      }
      client.sendMessage(IRC_CHAN, woe());
    }
    else if(ircMessage.text == "!goals"){
      client.sendMessage(IRC_CHAN, goals());
    }
    else if(ircMessage.text == "!ch1"){
      chanel = 1;
      client.sendMessage(IRC_CHAN, ircMessage.nick + " set tvhead to the heart!");
      //Serial.println(chanel);
    }
    else if(ircMessage.text == "!ch0" && ircMessage.nick == "atltvhead"){
      chanel = 0;
      client.sendMessage(IRC_CHAN, ircMessage.nick + " set tvhead to the secret 0!");
      //Serial.println(chanel);
    }
    else if(ircMessage.text == "!heart"){
      if(heartcount >= 2){
      
       for(int ppgLooper =0; ppgLooper <= 3; ppgLooper++){
        for(int indPPG =0; indPPG<=5;indPPG++){
          ppg(indPPG);
        }
       }
        client.sendMessage(IRC_CHAN, ircMessage.nick + " shows their heart! Thank you!");
        client.sendMessage(IRC_CHAN, "Chat, you've shown your heart! YOU ARE AWESOME! THANK YOU!!!!!!!!!!");
        heartcount = 0;
      }
      else{
        hcind = invheartc - heartcount;
        client.sendMessage(IRC_CHAN, ircMessage.nick + " shows their heart! Thank you! Chat, we need " + hcind + " more people to show their '!heart'. LET'S DO THIS!");
        heartcount++;
      }
    }
    else if(ircMessage.text == "!flicker"){
      flicker = true;
      client.sendMessage(IRC_CHAN, ircMessage.nick + " is messing with my signals! Someone help!");
    }
    else if(ircMessage.text == "!flickerOff"){
      flicker = false;
      client.sendMessage(IRC_CHAN, ircMessage.nick + " has saved me!!! Virtual hug, just for you!");
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


String woe(){
  String message = "";
  // This will be for sending out words of encouragement as in woe. 
      if(posin == 0){
        message = "Where you go, go with all your heart";
      }
      else if(posin == 1){
        message = "Be kind to yourself";
      }
      else if(posin == 2){
        message = "Let go of your fear";
      }
      else if(posin == 3){
        message = "Whatever makes you weird, embrace it";
      }
      else if(posin == 4){
        message = "Our greatest glory is not in never falling, but in rising every time we fall";
      }
      else if(posin == 5){
        message = "It does not matter how slowly you go so long as you do not stop";
      }
      
      
      return message;
    }


String goals(){
  String goallist = " ";
  if(goallist == " ");{
    goallist = "The main goals are to have a good time, be kind, and interact with people using the tv! Type !howtoshare' for instructions on submitting more goals!";
  }
  return goallist;
}



//_____________________________________________________________________________________________________
bool setDir(){
    if(newPosition > oldPosition){
    return forward;
  }
  else if (newPosition < oldPosition){
    return back;
  }
}


//_____________________________________________________________________________________________________________
int setchanel(){
 
  // -> Trying to figure out how to make code activate only on encoder clicks/divisible by 4.    
  if (abs(newPosition)%4 ==0 && newPosition!=oldPosition && bState == true && dir ==true && CurrentPatternNumber < 3){
         CurrentPatternNumber = CurrentPatternNumber +1;
    }
  else if(abs(newPosition)%4 ==0 && newPosition!=oldPosition && bState == true && dir ==false && CurrentPatternNumber > 0){
    CurrentPatternNumber = CurrentPatternNumber -1;
    }

  if(CurrentPatternNumber == 3){
    FastLED.addLeds<CHIPSET, PIN, COLOR_ORDER>(LEDs[0], LEDs.Size());
  }
  else{
    FastLED.addLeds<CHIPSET, PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  }
    
      
  return CurrentPatternNumber;
}

//_____________________________________________________________________________________________________________
int brightVal(int curBVal){
  
  if (abs(newPosition)%4 ==0 && newPosition!=oldPosition && bState == false && dir==true){
         curBVal = curBVal +10;
    }
  else if (abs(newPosition)%4 ==0 && newPosition!=oldPosition && bState == false && dir==false){
         curBVal = curBVal -10;
    }
    
  return curBVal;
}

//-----------------------------------------------------------------------------------------------------------------------------------
//_________________________________________________________________________________________
// heart effect
void heart(){
  for( byte y = 0; y < kMatrixHeight; y++) {    
    for( byte x = 0; x < kMatrixWidth; x++) {
     if( XY(x,y)<=126){
        if(tv[y][x]){
          leds[ XY(x, y)]  = CHSV( 211, 255, 255);
        }
        else{
          leds[XY(x,y)]=CHSV(135,255,255);
        }
      }
      
//      else if(XY(x,y) == 126){
//        if(tv[y][x]){
//        leds[ XY(x, y)]  = CHSV( 211, 255, 255);
//        }
//        else{
//          leds[XY(x,y)]=CHSV(135,255,255);
//        }
//      }
      
      else if(XY(x,y)>126 && XY(x,y)<142){
        if(tv[y][x]){
        leds[XY(x,y)-1] = CHSV( 211, 255,255);
        }
        else{
          leds[XY(x,y)-1]=CHSV(135,255,255);
        }
      }
      
      else{
        if(tv[y][x]){
        leds[XY(x,y)-3]=CHSV(211,255,255);
        }
        else{
          leds[XY(x,y)-3]=CHSV(135,255,255);
        }
      }
    }
  }
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
      
//      leds[XY( 7, 7)-1] = CHSV(color,sat,255);
//      leds[XY( 10, 7)-1] = CHSV(color,sat,255);
//      leds[XY( 11, 7)-1] = CHSV(color,sat,255);
      
//      leds[XY( 7, 8)-3] = CHSV(color,sat,255);
//      leds[XY( 10, 8)-3] = CHSV(color,sat,255);
      }
      
//    else if(frame==5){
//      for(int ya =0; ya<4;ya++) {
//        for(int xa = 0; xa<18; xa++){
//            leds[XY( xa, ya)] = CHSV(color,sat,255);
//            yield();
//          }
//        }
//
//        //  errors out on this section of code
//      for(int ys =0; ys<6;ys++) {    
//        for(int xs = 2+ys; xs<16-ys; xs++){
//            leds[XY( xs, ys+5)] = CHSV(color,sat,255);
//            yield();
//          }
//          yield();
//        }      
//      }
            
    else{
      fill_solid(leds,NUM_LEDS, CHSV(color,sat,255)); // change this to a solid fill
      satHolder = oldSat;
      oldSat = sat;
      sat = satHolder;       
      }
   
    FastLED.show();
    demonDelay(70);
  }






void mirrorUp(){
  // Here we are setting the bottom led's to their top led counter part -> take into consideration the 3 missing pixels on the bottom
  //row and the one missing pixel on the second to bottom row just like in the Heart settings! 
//  
//  if(XY(,) == 0 || XY(,) == 17 || XY(,) == 18 || XY(,) == 35){  // This is to pass the the top two rows, side leds which do not get set to match the bottom half
//    yield();
//  }
//  else{
//    // need to take into account that the top, 2nd, 3rd, 4th, from the top add less to get the mirrored effect -18 per each row in to NUM*******************************
//    if(XY(x,y)+NUM<=126){
//      leds[XY(,)] = leds[XY(,)+NUM];
//      yield();
//    }
//    else if(XY(x,y)+NUM>126 && XY(x,y)+NUM<142){
//      leds[XY(,)] = leds[XY(,)+NUM-1];
//      yield();
//    }
//    else{
//      leds[XY(,)] = leds[XY(,)+NUM-3];
//      yield();
//    }
//
//    
//  }


  
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
 

