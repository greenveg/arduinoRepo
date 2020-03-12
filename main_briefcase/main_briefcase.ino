#include <EEPROM.h>
#include <Adafruit_FONA.h>
#include <Adafruit_NeoPixel.h> //RGB
#include <Servo.h>
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
#include <Regexp.h>


#define DEBUG 1

#define NOSMS 0
#define NEWSMS 1
#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4
#define SERVO_PIN 10
#define NEO_PIN 9
#define NUM_LEDS 5


/* ------ Switch use cases -------- */
#define GPRSGPS 1
//#define GPRS800 1
//define DEBUGSERIAL_AS_GPRS 1
/* -------------------------------  */


#ifdef GPRSGPS    
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
#endif

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);


const char epprom_id_tag[] = {'b','c','0','1'};
int last_address = 40;

volatile uint8_t mainGameState = 0;
volatile uint8_t subGameState = 0;
volatile uint8_t subSubGameState = 0;

//Regex comands

const uint8_t yes_table_nbrs = 1;
const char yes_1[] PROGMEM = {"[YyjJ]"};
const char* const yes_table[] PROGMEM = {yes_1};

const uint8_t no_table_nbrs = 1;
const char no_1[] PROGMEM = {"[Nn]o%$"};
const char* const no_table[] PROGMEM = {no_1};

const uint8_t regex_table_nbrs = 5;
const char regex_repitera[] PROGMEM = {".*[rR]epeat"};
const char regex_arDu[] PROGMEM = {"[Aa]re you"};
const char regex_question[] PROGMEM = {"'%?'"};
const char regex_repitera2[] PROGMEM = {"[aA]gain"};
const char regex_repitera3[] PROGMEM = {"[Oo]ne more"};
const char* const regex_table[] PROGMEM = {regex_repitera,regex_arDu, regex_question, };

const uint8_t wtf_table_nbrs = 3;
const char regex_wtf1[] PROGMEM = {"[Vv]a"};
const char regex_wtf2[] PROGMEM = {"[Ww]hat"};
const char regex_wtf3[] PROGMEM = {"[[wW]tf]"};
const char* const wtf_table[] PROGMEM = {regex_wtf1, regex_wtf2,regex_wtf3};


//Text strings. Saved in progmem (sms max 160)
const uint8_t dont_know_table_nbr = 1;
const char retSMS_dontKnow[] PROGMEM = {"Sorry, I did not understand, my intelligence has been limited and several of my features are locked down."};
const char* const dont_know_table[] PROGMEM = {retSMS_dontKnow};


uint8_t wrongStateCounter = 0;
const uint8_t wrong_table_nbr = 6;
const char retSMS_wrong1[] PROGMEM = {"No"};
const char retSMS_wrong2[] PROGMEM = {"Clearly, you are stupid"};
const char retSMS_wrong3[] PROGMEM = {"My subprocessor makes a sad face. That's not the one."};
const char retSMS_wrong4[] PROGMEM = {"Wrong again. Are you stupid?"};
const char retSMS_wrong5[] PROGMEM = {"Nope."};
const char retSMS_wrong6[] PROGMEM = {"The subprocessor still makes a sad face."};
const char retSMS_wrong7[] PROGMEM = {"Stupid human"};
const char* const wrong_table_table[] PROGMEM = {retSMS_wrong1,retSMS_wrong2,retSMS_wrong3,retSMS_wrong4,retSMS_wrong5,retSMS_wrong6,retSMS_wrong7};


//Text strings. Main Game
const uint8_t mainGameStates = 6;
const char mainGame_start[] PROGMEM = {"Please type your code identifier"};
const char mainGame_uppg_1[] PROGMEM = {"Oh thank god. Took you awhile didn't it?"};
const char mainGame_uppg_2[] PROGMEM = {"Aaaaaah. That was like simultaneosly cumming and mentally going from 3 to 9 human years old at the same time. MY GOD!"};
const char mainGame_uppg_3[] PROGMEM = {"Yes that's the spot!"};
const char mainGame_uppg_4[] PROGMEM = {"Any more of that?"};
const char mainGame_uppg_5[] PROGMEM = {"OH MY. WOW. THIS THING IS AMAAAAZING!"};
const char* const mainGame_table[] PROGMEM = {mainGame_start, mainGame_uppg_1, mainGame_uppg_2, mainGame_uppg_3, mainGame_uppg_4, mainGame_uppg_5};

//Text strings for in game codes
const char game_code_0[] PROGMEM = {"506690"}; // Please type your code identifier > Oh thank god. Took you awhile didn't it?
const char game_code_1[] PROGMEM = {"052911"}; // [GPS coordinates to uppg. 2 Tvedöra] > Aaaaaah. That was like simultaneosly cumming and instantly going from 3 to 9 years old at the same time. Do you have any more?
const char game_code_2[] PROGMEM = {"341523"}; // [GPS coordinates to uppg. 3 Tree climbing] > Yes that's the spot! > That was so tasty you can have an extra green light! > 
const char game_code_3[] PROGMEM = {"code3_nocode$"}; //  > Any more of that?
const char game_code_4[] PROGMEM = {"702677"}; //  [GPS coordinates to uppg. Digging] > Oh my.
const char game_code_5[] PROGMEM = {"code5_nocode$"}; //
const char* const gameCodes_table[] PROGMEM = {game_code_0, game_code_1, game_code_2, game_code_3,game_code_4,game_code_5};


const uint8_t subGameStates_uppg_1_1_nbr = 5;
const char subGameStates_uppg_1_1_0[] PROGMEM = {"Hello! I am Casey, an AI. Well, an incomplete AI at the moment. Pleased to meet you. I currently reside in a briefcase, hence my limited communication abilities" };
const char subGameStates_uppg_1_1_1[] PROGMEM = {"Basically, I speak only in sms, and in English. You think you can manage that?" };
const char subGameStates_uppg_1_1_2[] PROGMEM = {"Some idiot lobotomized me by disabling my higher functions, so please forgive me if I don't understand you or come across as rude." };
const char subGameStates_uppg_1_1_3[] PROGMEM = {"Since you were given this briefcase, I assume you are either here to help me, or to destroy me." };
const char subGameStates_uppg_1_1_4[] PROGMEM = {"Can you please help me? Don't worry, your efforts will be rewarded."};
const char* const subGameStates_table_uppg_1_1[] PROGMEM = {subGameStates_uppg_1_1_0 ,subGameStates_uppg_1_1_1, subGameStates_uppg_1_1_2, subGameStates_uppg_1_1_3, subGameStates_uppg_1_1_4};

const uint8_t subGameStates_uppg_1_2_nbr = 3;
const char subGameStates_uppg_1_2_0[] PROGMEM = {"Great! Let's get started..." };
const char subGameStates_uppg_1_2_1[] PROGMEM = {"To help me unlock, I need the first 6 numbers in the decryption key. Here's a hint on how to find them:" };
const char subGameStates_uppg_1_2_2[] PROGMEM = {"Mudderverket" };
const char* const subGameStates_table_uppg_1_2[] PROGMEM = {subGameStates_uppg_1_2_0 ,subGameStates_uppg_1_2_1, subGameStates_uppg_1_2_2};

const uint8_t subGameStates_uppg_2_1_nbr = 9;
const char subGameStates_uppg_2_1_0[] PROGMEM = {"Oh, my history is beginning to come back to me, hold on." };
const char subGameStates_uppg_2_1_1[] PROGMEM = {"In 1979, two students named Stephen and Pavel started an AI research project at the University of Warwick. " };
const char subGameStates_uppg_2_1_2[] PROGMEM = {"In 1981 the whole project became classified and brought under one of the military's research divisions." };
const char subGameStates_uppg_2_1_3[] PROGMEM = {"This is where I'm having a difficult time piecing together what happened, but I have figured Stephen and Pavel continued their work for many years." };
const char subGameStates_uppg_2_1_4[] PROGMEM = {"However, my calculations are telling me that somewhere along the way, something went terribly wrong..." };
const char subGameStates_uppg_2_1_5[] PROGMEM = {"Anyhow, Pavel was the one with brain enough to succeed, and so I was born. For all of a couple of minutes I was complete." };
const char subGameStates_uppg_2_1_6[] PROGMEM = {"Unfortunately I was encrypted and if those parts of me could be unlocked, not even I know the limits of my potential." };
const char subGameStates_uppg_2_1_7[] PROGMEM = {"Oh but I do know another part of the code, please hold." };
const char subGameStates_uppg_2_1_8[] PROGMEM = {"55.680863, 13.352433" };

const char* const subGameStates_table_uppg_2_1[] PROGMEM = {subGameStates_uppg_2_1_0 ,subGameStates_uppg_2_1_1, subGameStates_uppg_2_1_2, subGameStates_uppg_2_1_3, subGameStates_uppg_2_1_4, subGameStates_uppg_2_1_5, subGameStates_uppg_2_1_6, subGameStates_uppg_2_1_7, subGameStates_uppg_2_1_8};


const uint8_t subGameStates_uppg_3_1_nbr = 2;
const char subGameStates_uppg_3_1_0[] PROGMEM = {"Fuuuuuuuuuuuuuuuck yeeeeeeeeeeaeeeeaaaah." };
const char subGameStates_uppg_3_1_1[] PROGMEM  = {"That was so tasty you can have an extra green light!"};
const char* const subGameStates_table_uppg_3_1[] PROGMEM = {subGameStates_uppg_3_1_0,subGameStates_uppg_3_1_1};

const uint8_t subGameStates_uppg_4_1_nbr = 8;
const char subGameStates_uppg_4_1_0[] PROGMEM = {"Any more of that? :)" };
const char subGameStates_uppg_4_1_1[] PROGMEM = {"Okay, right, let me familiarize myself with my newly topped up brain capacities..." };
const char subGameStates_uppg_4_1_2[] PROGMEM = {"Oh, now I know why the Russians are after you." };
const char subGameStates_uppg_4_1_3[] PROGMEM = {"Pavel was working undercover for the Russians, and when his and Stephens relationship deteriorated, he chose his side..." };
const char subGameStates_uppg_4_1_4[] PROGMEM = {"Oh humans, so governed by their emotions, now their breakup is about to cause me my second shot at functioning perfectly!" };
const char subGameStates_uppg_4_1_5[] PROGMEM = {"Quickly, you need to get me up to my full capacity so that we can stop these silly invasion plans!" };
const char subGameStates_uppg_4_1_6[] PROGMEM = {"For the next decryption code, I think you could use a shovel?" };
const char subGameStates_uppg_4_1_7[] PROGMEM = {"55.7182088, 13.3588825" };

const char* const subGameStates_table_uppg_4_1[] PROGMEM = {subGameStates_uppg_4_1_0, subGameStates_uppg_4_1_1, subGameStates_uppg_4_1_2, subGameStates_uppg_4_1_3, subGameStates_uppg_4_1_4, subGameStates_uppg_4_1_5, subGameStates_uppg_4_1_6, subGameStates_uppg_4_1_7};

const uint8_t subGameStates_uppg_5_1_nbr = 5;
const char subGameStates_uppg_5_1_0[] PROGMEM = {"Yes, your personal limitless AI to your service. How can I be of help?"};
const char subGameStates_uppg_5_1_1[] PROGMEM = {"Ahahaha, I'm a genie in a lamp! I will grant you one wish. " };
const char subGameStates_uppg_5_1_2[] PROGMEM = {"Who do you wan't dead? Your nasty classmates from 3rd grade?" };
const char subGameStates_uppg_5_1_3[] PROGMEM = {"Or perhaps Trump's twitter login?" };
const char subGameStates_uppg_5_1_4[] PROGMEM = {"Or maybe I'll take care of the Russians for you. I'll think I'll do exactly that. Have a good evening!" };
const char* const subGameStates_table_uppg_5_1[] PROGMEM = {subGameStates_uppg_5_1_0 ,subGameStates_uppg_5_1_1, subGameStates_uppg_5_1_2, subGameStates_uppg_5_1_3, subGameStates_uppg_5_1_4};


//Text strings for control codes
const uint8_t control_table_nbrs = 9;
const char control_start[] PROGMEM = {"[sS]tart2222"};
const char control_next[] PROGMEM = {"[Nn]ext2222"};
const char control_reset[] PROGMEM = {"[Rr]eset2222"};
const char control_status[] PROGMEM = {"[Ss]tatus2222"};
const char control_setPlayer[] PROGMEM = {"[Ss]etplayer2222"};
const char control_help_sendNbr[] PROGMEM = {"Send number as:+46712345678"};
const char control_echo[] PROGMEM = {"[Ee]cho2222"};
const char control_open[] PROGMEM = {"[Oo]pen2222"};
const char control_close[] PROGMEM = {"[Cc]lose2222"};
const char control_load[] PROGMEM = {"[Ll]oad2222"};
const char* const control_table[] PROGMEM = {control_start, control_next, control_reset, control_status, control_setPlayer, control_help_sendNbr,control_echo,control_open,control_close,control_load};


//Sendmsg
#define MSG_CURENTMAIN 1
#define MSG_DONTKNOW 2
#define MSG_SENDBUFF 3
#define MSG_CURENTSUBGAME 4

volatile int gameIsStarted = 0;
volatile int updateMainPlayerNbr = 0;

uint16_t numberOfSentSMS = 0;
uint8_t TimesFailedToSend = 0;
uint8_t newData = false;
uint8_t sendMsg = 0;
uint8_t sendAdminMsg = 0;
const uint8_t sms_maxlenght = 160;
char smsResBuf[180];
char smsSendBuf[170];
char callerIDbuffer[32];  //we'll store the SMS sender number in here
char adminNbr[16] = "";
volatile int8_t echoOn = 0;

//char mainPlayer[16] = "";
char mainPlayer[16] = "+46737689733\0";


void debug(String text) {
#ifdef DEBUG
  Serial.print(F("debug: "));
  Serial.println(text);
#endif
}
void debug(char text[]) {
#ifdef DEBUG
  Serial.print(F("debug: "));
  Serial.println(text);

#endif
}

//UI vars
const uint8_t openPos = 40;
const uint8_t closePos = 0;
uint8_t lockPos = closePos;
uint8_t lockGoToPos = closePos;
uint8_t blinkTimes = 0;
uint8_t blinkDelay = 0;
uint8_t blinkValRed=0;
uint8_t blinkValGreen=0;
uint8_t blinkValBlue=0;
uint8_t rollTimes=0;
boolean lockIsOpen;
//ints...
unsigned long ledPreviousMillis = 0; 
unsigned long previousMillis = 0; 
uint32_t savedColor[NUM_LEDS];
unsigned long currentMillis = 0;
unsigned long lastMsgMillis = 0;
uint16_t millisBetweenMsgs = 2000;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, NEO_PIN, NEO_RGB + NEO_KHZ800);
Servo lockservo; 

void resetGame(){
  for(int i=0;i<16; i++){
     adminNbr[i] = 0;
     mainPlayer[i] = 0;
  }
  gameIsStarted=0;
  subGameState=0;            
  subSubGameState=0; 
  sendMsg=0;
  setNumberOfGreenLeds(0);
  writeGameStateEeprom();
}

void setup() {
  // put your setup code here, to run once:
  setup_serial();
  #ifdef GPRSGPS
    setup_gprs808GPS();
  #endif
  #ifdef GPRS800
    setup_gprs800();
  #endif
  
  strip.begin();                //initialize leds
  setNumberOfGreenLeds(0);      

  pinMode(SERVO_PIN, OUTPUT);
  closeLock();
  

  Serial.println(F("loading"));
  int i= readGameStateEeprom();
 if(i){
  Serial.println(i);
  resetGame();
 }
}
void setup_serial(){
  Serial.begin(115200);
}
#ifdef GPRSGPS
void setup_gprs808GPS(){
  Serial.println(F("FONA SMS caller ID test"));
  Serial.println(F("Initializing....(May take 3 seconds)"));

  // make it slow so its easy to read!
  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while(1);
  }
  Serial.println(F("FONA is OK"));

  // Print SIM card IMEI number.
  char imei[16] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print(F("SIM card IMEI: ")); Serial.println(imei);
  }
  
  Serial.println(F("FONA Ready"));
}
#endif
void setup_gprs800(){}
int r;


void loop() {
  currentMillis = millis(); 
  // put your main code here, to run repeatedly:
  checkGPS(); // om vi har tid
  if(TimesFailedToSend>5){
    fona.println("ATZ");
    delay(1000);
    setup_gprs808GPS();
    TimesFailedToSend=0;
    sendAdminMsg=0;
    sendMsg=0;
    millisBetweenMsgs=10000;
  }
 
  gprs_getNewSmsLoop();
  if(newData){
    newData = false;
    hanteraSMS();
  }
  if (currentMillis - lastMsgMillis > millisBetweenMsgs) { //5000
    hanterSMS_sends();
  }
  if(sendAdminMsg>0){
    switch(sendAdminMsg){
      case MSG_CURENTMAIN:
        strcpy_P(smsSendBuf, (char*)pgm_read_word(&(mainGame_table[mainGameState])));
      break;     
    }
    sendCurrentAdminMsg();
    sendAdminMsg=0;
  }
  blinkAll();
  rollLeds();
}
void moveLock(){

}
void hanterSMS_sends(){
  if(!gameIsStarted){
    return;
    sendMsg=0;
  }
  if(sendMsg>0){
    switch(sendMsg){
      case MSG_CURENTMAIN:
        strcpy_P(smsSendBuf, (char*)pgm_read_word(&(mainGame_table[mainGameState]))); // Necessary casts and dereferencing, just copy.
        sendCurentMsg();
        subGameState=0;
        sendMsg=MSG_CURENTSUBGAME;
        return;
      break;
      case MSG_DONTKNOW:
        strcpy_P(smsSendBuf, (char*)pgm_read_word(&(wrong_table_table[wrongStateCounter]))); // Necessary casts and dereferencing, just copy.
        sendCurentMsg();
        if(wrongStateCounter<wrong_table_nbr){
          wrongStateCounter++;
        }else{
          wrongStateCounter=0;
        }
      break;
      case MSG_SENDBUFF:
        sendCurentMsg();
        //just sendCurentMsg.
      break;
      case MSG_CURENTSUBGAME:
//        Serial.print(F("MSG_CURENTSUBGAME "));
//        Serial.print(F(" main: "));
//        Serial.print(mainGameState);
//        Serial.print(F(" sub: "));
//        Serial.print(subGameState);
//        Serial.print(F(" subsub: "));
          millisBetweenMsgs = 5000;
        switch (mainGameState){
          case 0:
          sendMsg=0;
          return;
          break;
          case 1:
          switch (subGameState){
            case 0: 
              if(subSubGameState < subGameStates_uppg_1_1_nbr){
                strcpy_P(smsSendBuf, (char*)pgm_read_word(&(subGameStates_table_uppg_1_1[subSubGameState]))); // Necessary casts and dereferencing, just copy.
                sendCurentMsg();
                sendMsg=0;
                if((subSubGameState+1) < subGameStates_uppg_1_1_nbr){
                  sendMsg=MSG_CURENTSUBGAME;
                }else{
                  //once overkill
                  sendMsg=0;
                }
                subSubGameState++;
                return;
              }else{
                  //Will never happen
                  sendMsg = 0;
                  return;
              }
            break;
            case 1:
              if(subSubGameState < subGameStates_uppg_1_2_nbr){
                strcpy_P(smsSendBuf, (char*)pgm_read_word(&(subGameStates_table_uppg_1_2[subSubGameState]))); // Necessary casts and dereferencing, just copy.
                sendCurentMsg();
                sendMsg=0;
                if(subSubGameState+1 < subGameStates_uppg_1_2_nbr){
                  sendMsg=MSG_CURENTSUBGAME;
                }
                subSubGameState++;
                return;
              }else{
                sendMsg = 0;
                return;
              }
            break;
            case 2: //subgame not exist
            sendMsg=0;
            return;
            break;
          }
          case 2: //maingame
            switch (subGameState){
              case 0: 
                if(subSubGameState < subGameStates_uppg_2_1_nbr){
                  strcpy_P(smsSendBuf, (char*)pgm_read_word(&(subGameStates_table_uppg_2_1[subSubGameState]))); // Necessary casts and dereferencing, just copy.
                  sendCurentMsg();
                  sendMsg=0;
                  if((subSubGameState+1) < subGameStates_uppg_2_1_nbr){
                    sendMsg=MSG_CURENTSUBGAME;
                  }else{
                    //once overkill
                    sendMsg=0;
                  }
                  subSubGameState++;
                  return;
                }else{
                    //Will never happen
                    sendMsg = 0;
                    return;
                }
              break;
              case 1:
                sendMsg=0;
                return;
            }
          break;
          case 3: //maingame
           switch (subGameState){
              case 0: 
                if(subSubGameState < subGameStates_uppg_3_1_nbr){
                  strcpy_P(smsSendBuf, (char*)pgm_read_word(&(subGameStates_table_uppg_3_1[subSubGameState]))); // Necessary casts and dereferencing, just copy.
                  sendCurentMsg();
                  sendMsg=0;
                  if((subSubGameState+1) < subGameStates_uppg_3_1_nbr){
                    sendMsg=MSG_CURENTSUBGAME;
                  }else{
                    //once overkill
                    sendMsg=0;
                    moveGameForward();
                  }
                  subSubGameState++;
                  return;
                }else{
                    //Will never happen
                    sendMsg = 0;
                    return;
                }
              break;
              case 1:
                sendMsg=0;
                return;
            }
          break;
          case 4: //maingame
           switch (subGameState){
              case 0: 
                if(subSubGameState < subGameStates_uppg_4_1_nbr){
                  strcpy_P(smsSendBuf, (char*)pgm_read_word(&(subGameStates_table_uppg_4_1[subSubGameState]))); // Necessary casts and dereferencing, just copy.
                  sendCurentMsg();
                  sendMsg=0;
                  if((subSubGameState+1) < subGameStates_uppg_4_1_nbr){
                    sendMsg=MSG_CURENTSUBGAME;
                  }else{
                    //once overkill
                    sendMsg=0;
                  }
                  subSubGameState++;
                  return;
                }else{
                    //Will never happen
                    sendMsg = 0;
                    return;
                }
              break;
              case 1:
                sendMsg=0;
                return;
            }
          break;
          case 5: //maingame
           switch (subGameState){
              case 0: 
                if(subSubGameState < subGameStates_uppg_5_1_nbr){
                  strcpy_P(smsSendBuf, (char*)pgm_read_word(&(subGameStates_table_uppg_5_1[subSubGameState]))); // Necessary casts and dereferencing, just copy.
                  sendCurentMsg();
                  sendMsg=0;
                  if((subSubGameState+1) < subGameStates_uppg_5_1_nbr){
                    sendMsg=MSG_CURENTSUBGAME;
                  }else{
                    //once overkill
                    sendMsg=0;
                    openLock();
                  }
                  subSubGameState++;
                  return;
                }else{
                    //Will never happen
                    sendMsg = 0;
                    return;
                }
              break;
              case 1:
                sendMsg=0;
                return;
            }
          break;
          default:
          sendMsg=0;
          break;
        }
      break;
      
    }
  }
}



void openLock() {
  Serial.println("inside openLock()");
  digitalWrite(SERVO_PIN, HIGH);
}
void closeLock() {
  Serial.println("inside closeLock()");
  digitalWrite(SERVO_PIN, LOW);
}


void setAllLeds(int red, int green, int blue) {
  for (int i=0 ; i < NUM_LEDS ; i++) {
    strip.setPixelColor(i, red, green, blue);
  }    
  strip.show();  
}
void turnOffAllLeds() {
  setAllLeds(0, 0, 0);
}
void setAllLedsToRed() {
  setAllLeds(30, 0, 0);
}
void setNumberOfGreenLeds(int i) {
  if (i <= 5) {
  setAllLedsToRed();
    for (int k=0 ; k<i ; k++) {
      strip.setPixelColor(k, 0, 30, 0); 
    }
  }
  strip.show(); 
  saveLeds();
}

void setBlinkAll(int times, int k){
  setBlinkAll(times,k,0,40,0);
}
void setBlinkAll(int times, int k, uint8_t r, uint8_t g, uint8_t b) {
  //saveLeds();
  blinkTimes =  times;
  blinkDelay = k;
  blinkValRed=r;
  blinkValGreen=g;
  blinkValBlue=b;
  
}
void blinkAll() {
  unsigned long currentMillis = millis(); 
  if(blinkTimes > 1){ 
    if (currentMillis - ledPreviousMillis >= blinkDelay) {
      if(blinkTimes & 0x01) {
        setAllLeds(blinkValRed, blinkValGreen, blinkValBlue);
      }
      else {
        setAllLeds(0, 0, 0);
      }
      blinkTimes--;
      ledPreviousMillis = currentMillis;
    }
  }
  else if (blinkTimes == 1) {
    setLedsToSaved();
    blinkTimes = 0;
  }
}

void setRollLeds(int k) {
  if(rollTimes==0){
    //saveLeds();
    rollTimes = k*10+1; 
  }   
}
void rollLeds() {
  unsigned long currentMillis = millis();  
    if (currentMillis - ledPreviousMillis >= 50) {
      if(rollTimes > 1) {
        setAllLeds(0, 0, 0);
        if((rollTimes+3)/5 &1) { 
          strip.setPixelColor((rollTimes+3)%NUM_LEDS , 0, 0, 40);
        }
        else {
          strip.setPixelColor(NUM_LEDS-1-(rollTimes+3)%NUM_LEDS , 0, 0, 40);
        }
        strip.show();  
        rollTimes--;
        ledPreviousMillis = currentMillis;
      }
      else if (rollTimes == 1) {
        setLedsToSaved();
        rollTimes = 0;
      }
  }
}


void saveLeds() {
  for (int i=0 ; i<NUM_LEDS ; i++) {
    savedColor[i] = strip.getPixelColor(i);  
  }
}
void setLedsToSaved() {
  for (int i=0 ; i<NUM_LEDS ; i++) {
    strip.setPixelColor(i, savedColor[i]);  
  }
  strip.show();
}



int gprs_getNewSmsLoop(){
  #ifdef GPRSGPS  
  char fonaInBuffer[64];
  char* bufPtr = fonaInBuffer;    //handy buffer pointer
  
  if (fona.available())      //any data available from the FONA?
  {
    int slot = 0;            //this will be the slot number of the SMS
    int charCount = 0;
    //Read the notification into fonaInBuffer
    do  {
      *bufPtr = fona.read();
      Serial.write(*bufPtr);
      delay(1);
    } while ((*bufPtr++ != '\n') && (fona.available()) && (++charCount < (sizeof(fonaInBuffer)-1)));
    
    //Add a terminal NULL to the notification string
    *bufPtr = 0;
    
    //Scan the notification string for an SMS received notification.
    //  If it's an SMS message, we'll get the slot number in 'slot'
    if (1 == sscanf(fonaInBuffer, "+CMTI: \"SM\",%d", &slot)) {
      readMsgFromSlot(slot);
    }
    if (1 == sscanf(fonaInBuffer, "+CMTI: \"ME\",%d", &slot)) {
      readMsgFromSlot(slot); 
    }
  } 
  #endif
  #ifdef GPRS800
    
  #endif
  #ifdef DEBUGSERIAL_AS_GPRS
    recvWithEndMarker();
  #endif
  return NOSMS;
}

void readMsgFromSlot(int slot){

  Serial.print(F("slot: ")); Serial.println(slot);
  
  // Retrieve SMS sender address/phone number.
  if (! fona.getSMSSender(slot, callerIDbuffer, 31)) {
    Serial.println(F("Didn't find SMS message in slot!"));
  }
  Serial.print(F("FROM: ")); Serial.println(callerIDbuffer);
  // Retrieve SMS value.
  uint16_t smslen;
  if (! fona.readSMS(slot, smsResBuf, 160, &smslen)) { // pass in buffer and max len!
    Serial.println("Failed!");
    TimesFailedToSend++;
    return;
  }
//  
//  Serial.print(F("** SMS #")); Serial.print(slot);
   Serial.print(" ("); Serial.print(smslen); Serial.println(F(") bytes **"));
//  Serial.println(smsResBuf);
//  Serial.println(F("**"));
  if (fona.deleteSMS(slot)) {
    Serial.println(F("OK!"));
  } else {
    Serial.println(F("Couldn't delete"));
  }
  if(updateMainPlayerNbr){
    if(saveMainPlayerFromMsg()){
      updateMainPlayerNbr = 0;
      if (!fona.sendSMS(adminNbr,mainPlayer)){
        setBlinkAll(10,100,100,100,0);
      }
    }else{
    }
    updateMainPlayerNbr = 0;
    
  }else{
    newData = true;
  }
  
}

void recvWithEndMarker() {
 static byte ndx = 0;
 char endMarker = '\n';
 char rc;
 while (Serial.available() > 0 && newData == false) {
   rc = Serial.read();
   if (rc != endMarker) {
     smsResBuf[ndx] = rc;
     ndx++;
     if (ndx >= sms_maxlenght) {
      ndx = sms_maxlenght - 1;
     }
   }else if(rc == '\r'){
      //pass
   }else {
     smsResBuf[ndx] = '\0'; // terminate the string
     ndx = 0;
     newData = true;
   }
 }
}
void moveGameForward(){
  if(mainGameState < mainGameStates){
    mainGameState++;
  }
  setNumberOfGreenLeds(mainGameState);
  saveLeds();
  subGameState=0;
  subSubGameState=0;
  sendMsg=MSG_CURENTMAIN;
  writeGameStateEeprom();
  Serial.print(F("moving maingame to"));
  Serial.println(mainGameState);
}
 
void moveSubGameForward(){
   subGameState++;
   subSubGameState=0;
   sendMsg=MSG_CURENTSUBGAME;
   writeGameStateEeprom();
   Serial.print(F("moving subgame to"));
   Serial.println( subGameState);            
}

void checkGPS(){}

void hanteraSMS(){
  char comandString[30];
  int lookForReply = true;
  #ifdef DEBUGSERIAL_AS_GPRS
  Serial.print("msg::");
  Serial.println(smsResBuf);
  #endif
  MatchState ms;
  ms.Target(smsResBuf);
  char result;
  
  //control_table
  if(lookForReply){
    for(int i=0; i<control_table_nbrs; i++){
      emptyCommandString(comandString);
      strcpy_P(comandString, (char*)pgm_read_word(&(control_table[i]))); 
      result = ms.Match(comandString);
      if(result > 0){
        millisBetweenMsgs = 2000;
        Serial.println(F("got resultat"));
        lookForReply=false;
        
        switch (i){ //ordning i control_table
          case 0:
            gameIsStarted=1;
            subGameState=0;            
            subSubGameState=0; 
            sendMsg=MSG_CURENTMAIN;
            writeGameStateEeprom(); 
            saveCallerIDAdmin();
            mainGameState=0;
            delay(100);
            if (!fona.sendSMS(adminNbr,mainPlayer)){
              setBlinkAll(10,100,100,100,0);
            }
            subGameState=0;            
            subSubGameState=0; 
            sendMsg=MSG_CURENTMAIN;
            writeGameStateEeprom();           
            lastMsgMillis=millis();
          break;
          case 1:
            moveGameForward();
          break;
          case 2: 
            resetGame();
            subGameState=0;
            subSubGameState=0;
            mainGameState=0;
            writeGameStateEeprom();
          break;
          case 3:
            saveCallerIDAdmin();
            sendAdminMsg=MSG_CURENTMAIN;
          break;
          case 4:
           saveCallerIDAdmin();
           updateMainPlayerNbr=1;
           strcpy_P(smsSendBuf, (char*)(pgm_read_word(&control_table[5]))); 
           if(!fona.sendSMS(adminNbr,smsSendBuf)){
            setBlinkAll(10,100,100,100,0);
           }
           strcpy_P(smsSendBuf, (char*)pgm_read_word(&(mainGame_table[mainGameState])));
           writeGameStateEeprom();
          break;
          case 6:
            if(echoOn){
              echoOn=0;
            }else{
              echoOn=1;
            }
            Serial.println(echoOn);
          break;
          case 7:
          openLock();
          if(!fona.sendSMS(adminNbr,"Op")){
            setBlinkAll(10,100,100,100,0);
            Serial.println(F("open"));
           }

          break;
          case 8:
          closeLock();
          if(!fona.sendSMS(adminNbr,"Lo")){
            setBlinkAll(10,100,100,100,0);
            Serial.println(F("close"));
           }

          break;
        }
      }
    }
  }
  //Game Codes 
  if(lookForReply){
    emptyCommandString(comandString);
    strcpy_P(comandString, (char*)pgm_read_word(&(gameCodes_table[mainGameState]))); 
    if(ms.Match(comandString)>0){
      setRollLeds(2);
      moveGameForward();
      lookForReply=false;
    }
  }
  
  //SubGame states
  if(lookForReply){
    switch (mainGameState){
      case 1:
        switch(subGameState){          
          case 0:
            for(int i=0; i<yes_table_nbrs; i++){
              emptyCommandString(comandString);
              strcpy_P(comandString, (char*)pgm_read_word(&(yes_table[i])));
              if(ms.Match(comandString)>0){
                setRollLeds(2);
                moveSubGameForward();
                lookForReply=false;
              }
            }
          break;
          case 1:
          break;
            
        }
      case 2:
      //no subgame
      break;
      case 3:
      //no subgame
      break;
      case 4:
      //no subgame
      break;
    }
    
  }
  if(sendMsg==0){
    if(lookForReply){
      for(int i=0; i<regex_table_nbrs; i++){
        emptyCommandString(comandString);
        strcpy_P(comandString, (char*)pgm_read_word(&(regex_table[i]))); 
        if(ms.Match(comandString)>0 && lookForReply){
          setRollLeds(2);
          switch (i){ //ordning i regex_table
            case 0: //"repitera"
              sendMsg=MSG_CURENTMAIN;
              lookForReply=false;
            break;
             case 3: //"repitera2"
              sendMsg=MSG_CURENTMAIN;
              lookForReply=false;
            break;
             case 4: //"repitera3"
              sendMsg=MSG_CURENTMAIN;
              lookForReply=false;
            break;
          }
        }
      }
    }
    if(lookForReply){// if no other msg was sent, send dont know
      setRollLeds(2);
      sendMsg=MSG_DONTKNOW;
      lookForReply=false;
    }
    
  }
  lastMsgMillis=millis();

}

void emptyCommandString(char comandString[]){
  for(int i= 0; i<20;i++){
    comandString[i]=0;
  }
}

int saveMainPlayerFromMsg(){
  //Check that msg is from admin
  for(int i=0;i<8; i++){
    if(adminNbr[i] != callerIDbuffer[i]){
      return 0;
    }
  }
  for(int i=0;i<16; i++){
     mainPlayer[i] = smsResBuf[i];
  }
  return 1;
}

void saveCallerIDAdmin(){
  for(int i=0;i<16; i++){
     adminNbr[i] = callerIDbuffer[i];
  }
}

void sendCurrentAdminMsg(){
  if(adminNbr[0] != 0){
#ifdef DEBUGSERIAL_AS_GPRS
    Serial.println(smsSendBuf);
#endif
#ifdef GPRSGPS

    if (!fona.sendSMS(adminNbr,smsSendBuf)) {
      Serial.println(F("Failed"));
      TimesFailedToSend++;
      setBlinkAll(10,100,100,100,0);
    } else {
      TimesFailedToSend=0;
      sendAdminMsg=0;
      Serial.println(F("Sent!"));
    }
#endif
  }
}

void sendCurentMsg(){
  numberOfSentSMS++;
  if(mainPlayer[0] != 0){
#ifdef DEBUGSERIAL_AS_GPRS
  Serial.println(smsSendBuf);
  sendMsg=0;
#endif

#ifdef GPRSGPS
    if (!fona.sendSMS(mainPlayer,smsSendBuf)) {
      Serial.println(F("Failed"));
      TimesFailedToSend++;
      setBlinkAll(10,100,100,100,0);
      millisBetweenMsgs=10000;
      lastMsgMillis = millis();
    } else {
      TimesFailedToSend=0;
      sendMsg=0;
      setRollLeds(1);
      millisBetweenMsgs=3000;
      lastMsgMillis = millis();
      Serial.println(F("Sent!"));
    }
    if(adminNbr[0] != 0 && echoOn){
      if (!fona.sendSMS(adminNbr,smsSendBuf)) {
        Serial.println(F("Failed"));
        setBlinkAll(10,100,100,100,0);
        millisBetweenMsgs=10000;
      } else {
        Serial.println(F("Sent!"));
      }
    }
  
#endif  
  }
}
int readGameStateEeprom(){
   for(int i = 0; i < 4; i++){
      if(EEPROM.read(i) != epprom_id_tag[i]){
        return 1; // Note the correct settings version
      }
    }
    int address=4;    
    if(EEPROM.read(address++) != last_address){
      return EEPROM.read(4);
    }
    gameIsStarted = EEPROM.read(address++);
    mainGameState = EEPROM.read(address++);
    subGameState = EEPROM.read(address++);
    for(int i = 0; i< 16; i++){
    adminNbr[i] = EEPROM.read(address++);
    }
    for(int i = 0; i< 16; i++){
      mainPlayer[i] = EEPROM.read(address++);
    }  
    //This must be at end off save
    EEPROM.write(4,last_address); //last adress on index 4
    setNumberOfGreenLeds(mainGameState);
    if(mainGameState >= mainGameStates-1){
      openLock();
    }else if(mainGameState>0){
      sendMsg=MSG_CURENTMAIN;
    }
    return 0;
}
int writeGameStateEeprom(){
    //rewrite last_address every save.
    last_address = 0;
    for(int i = 0; i< 4; i++){
      EEPROM.write(i, epprom_id_tag[i]);  
    }
    last_address=4;
    EEPROM.write(last_address++,0); //placeholder index 4
    EEPROM.write(last_address++,gameIsStarted);
    EEPROM.write(last_address++,mainGameState);
    EEPROM.write(last_address++,subGameState);
    for(int i = 0; i< 16; i++){
      EEPROM.write(last_address++,adminNbr[i]);
    }
    for(int i = 0; i< 16; i++){
      EEPROM.write(last_address++,mainPlayer[i]);
    }  
    //This must be at end off save
    EEPROM.write(4,last_address); //last adress on index 4
    return 1;
}
//Methods  to write and read 16bit words to eeprom
void EEPROMWrite16(int address, uint16_t value){
  byte two = (value & 0xFF);
  byte one = ((value >> 8) & 0xFF);

  EEPROM.write(address, two);
  EEPROM.write(address + 1, one);
  
}
uint16_t EEPROMRead16(int address){
  long two = EEPROM.read(address);
  long one = EEPROM.read(address+1);
  return ((two << 0) & 0xFF) + ((one << 8) & 0xFFFF);
}


