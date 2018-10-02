/*
High Five Glove Twitch Augmentation
Yay and Atltvhead
Written Nate Damen
09/26/2018
*/
#include<FastLED.h>
#include <WiFi.h>
#include <IRCClient.h>
#include <Math.h>
/*-----------------------------------------------------------------------------------*/
#define ssid1         ""
#define password1     ""

#define ssid2         ""
#define password2     ""

#define ssid3          ""
#define password3      ""

#define IRC_SERVER   "irc.chat.twitch.tv"
#define IRC_PORT     6667
#define IRC_NICK     ""
#define IRC_CHAN     ""
#define IRC_PASS     ""

//naming wifi and server client

WiFiClient wiFiClient;
IRCClient client(IRC_SERVER, IRC_PORT, wiFiClient);
int wificount = 0;
/*------------------------------------------------------------------------------*/


int threshold = 62;
bool touch1detected = false;


void gotTouch1(){
  
 touch1detected = true;

}


/*-------------------------------------------------------------------------------*/

//positive message index
int posin = 0;
int posinold = 0;



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



/*----------------------------------------------------------------------------------*/
#define CHIPSET WS2812B
#define PIN 4
#define COLOR_ORDER GRB

int bright=100;
char pasnum='B';

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
/*----------------------------------------------------------------------------------*/



void setup() {
  Serial.begin(115200);
  //FastLED.addLeds<CHIPSET, PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  //FastLED.setBrightness( bright );

  delay(1000); // give me time to bring up serial monitor
  Serial.println("ESP32 Touch Interrupt Test");
  touchAttachInterrupt(T6, gotTouch1, threshold);

  delay(1000);
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

    if(wificount>15){
        WiFi.disconnect();
        digitalWrite(LED_BUILTIN,LOW);
        break;
    }
    ++wificount;
  }
}




void loop(){
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
      delay(250);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);

      client.setCallback(callback);
      client.setSentCallback(debugSentCallback);

      if(wificount>15){
          WiFi.disconnect();
          digitalWrite(LED_BUILTIN,LOW);
          wificount = 0;
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
    //Serial.println("Attempting IRC connection...");
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


  if(touch1detected){
    touch1detected = false;
    client.sendMessage(IRC_CHAN,"High Five Mode Initiated");
    delay(500);
    //Serial.println("Touch 1 detected");
  }
}


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
    else if (ircMessage.text == "!love"){
      client.sendMessage(IRC_CHAN,"TvheadBot loves " + ircMessage.nick);
    }
    else if(ircMessage.text == "!hug"){
      client.sendMessage(IRC_CHAN,"TvheadBot hugs " + ircMessage.nick);
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
     else if(ircMessage.text == "!goals"){
      client.sendMessage(IRC_CHAN, goals());
    }
    else if(ircMessage.text == "!goodvibes"){
      while (posin == posinold){
        posin = random(0,6);  // CHANGE ME FOR MORE MESSAGES
       //Serial.println("Im in the while loop");                                  // ----------------------------------------------------!!!!!!!!!!!!!    FIX ME CHANGE THE MESSAGE ---------------------------------------
      }
      client.sendMessage(IRC_CHAN, woe());
    }
    else if(ircMessage.text == "!flicker"){
      client.sendMessage(IRC_CHAN, ircMessage.nick + " is messing with my signals! Someone help!");
    }
    else if(ircMessage.text == "!flickerOff"){
      client.sendMessage(IRC_CHAN, ircMessage.nick + " has saved me!!! Virtual hug, just for you!");
    }
    else if(ircMessage.text == "!brighter"){
      client.sendMessage(IRC_CHAN, ircMessage.nick + " has brightend the screen. You can use this if the tv seems dim.");
    }
     else if(ircMessage.text == "!dimmer"){
       client.sendMessage(IRC_CHAN, ircMessage.nick + " has dimmed the screen. You can use this to help if the camera seems dark.");
    }
    else if(ircMessage.text =="!heartColor"){
      client.sendMessage(IRC_CHAN, ircMessage.nick + " changed the color of my heart! Keep that color cycling");
    }
    else if(ircMessage.text =="!heartSat"){
      client.sendMessage(IRC_CHAN, ircMessage.nick + " has changed the color saturation");
    }
    else if(ircMessage.text =="!heartBright"){
      client.sendMessage(IRC_CHAN, ircMessage.nick + " has changed the color brightness");
    }
    else if(ircMessage.text =="!backgroundColor"){
      client.sendMessage(IRC_CHAN, ircMessage.nick + " has indexed the background color! Keep that color changing!");
    }
    else if(ircMessage.text =="!backgroundSat"){
      client.sendMessage(IRC_CHAN, ircMessage.nick + " has changed the background color saturation");
    }
    else if(ircMessage.text =="!backgroundBright"){
      client.sendMessage(IRC_CHAN, ircMessage.nick + " has changed the background Color brightness");
    }
    else if(ircMessage.text =="!reset"){
      client.sendMessage(IRC_CHAN, ircMessage.nick + " has reset the screen");
    }
    else if(ircMessage.text == "!rainbowHeart"){
      client.sendMessage(IRC_CHAN, ircMessage.nick + " set atltvhead to rainbow heart!");
    }
    else if(ircMessage.text =="!fullRainbow"){
      client.sendMessage(IRC_CHAN, ircMessage.nick + " has gone FULL RAINBOW!!");
    }
    else if(ircMessage.text =="!mirrorMirror"){

    }
    else if(ircMessage.text =="!mirrorOff"){

    }
    else if(ircMessage.text =="!sparkles"){
      client.sendMessage(IRC_CHAN,ircMessage.nick + " has made me into a Twilight Vampire? Team Bella!");
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
