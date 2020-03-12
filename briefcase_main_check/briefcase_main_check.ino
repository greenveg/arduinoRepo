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
#define SERVO_PIN 9
#define NEO_PIN 10
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
char fonaInBuffer[64];
#endif

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);




uint8_t mainGameState = 0;
uint8_t subGameState = 0;
uint8_t subSubGameState = 0;

//Regex comands

const uint8_t yes_table_nbrs = 1;
const char yes_1[] PROGMEM = {"[Yy]es"};
const char* const yes_table[] PROGMEM = {yes_1};

const uint8_t no_table_nbrs = 1;
const char no_1[] PROGMEM = {"[Nn]o"};
const char* const no_table[] PROGMEM = {no_1};

const uint8_t regex_table_nbrs = 5;
const char regex_repitera[] PROGMEM = {".*[rR]epeat"};
const char regex_arDu[] PROGMEM = {"[Aa]re you"};
const char regex_question[] PROGMEM = {"'%?'"};
const char regex_repitera2[] PROGMEM = {"[aA]gain"};
const char regex_repitera3[] PROGMEM = {"[Oo]ne more"};
const char* const regex_table[] PROGMEM = {regex_repitera,regex_arDu, regex_question, };

//Text strings. Saved in progmem (sms max 160)
const uint8_t dont_know_table_nbr = 1;
const char retSMS_dontKnow[] PROGMEM = {"Sorry, I did not understand, my intelligence has been limited and several of my features are locked down."};
const char* const dont_know_table[] PROGMEM = {retSMS_dontKnow};

const uint8_t wrong_table_nbr = 1;
const char retSMS_wrong1[] PROGMEM = {"Sorry, I did not understand, my intelligence has been limited and several of my features are locked down."};
const char* const wrong_table_table[] PROGMEM = {retSMS_wrong1};


//Text strings. Main Game
const uint8_t mainGameStates = 3;
const char mainGame_start[] PROGMEM = {"Please type your code identifier" };
const char mainGame_uppg_1[] PROGMEM = {"Oh thank god. Took you awhile didn't it?"};
const char mainGame_uppg_2[] PROGMEM = {"Aaaaaah. That was like simultaneosly cumming and instantly going from 3 to 9 years old at the same time. Do you have any more?"};
const char* const mainGame_table[] PROGMEM = {mainGame_start, mainGame_uppg_1, mainGame_uppg_2};

//Text strings for in game codes
const char game_code_0[] PROGMEM = {"[Cc]odenoll"};
const char game_code_1[] PROGMEM = {"[Cc]odeett"};
const char* const gameCodes_table[] PROGMEM = {game_code_0, game_code_1};


const uint8_t subGameStates_uppg_1_1_nbr = 5;
const char subGameStates_uppg_1_1_0[] PROGMEM = {"Hi! I am in incomplete AI. Please to meat you. I currently reside in a briefcase, hence my limited communication abilities." };
const char subGameStates_uppg_1_1_1[] PROGMEM = {"Basically, I speak sms, and english only please. You think you can manage that?" };
const char subGameStates_uppg_1_1_2[] PROGMEM = {"Some idiot lobotomized my higher functions, so please forgive if I don't understand you or come across as rude" };
const char subGameStates_uppg_1_1_3[] PROGMEM = {"Are you here to help me?" };
const char* const subGameStates_table_uppg_1_1[] PROGMEM = {subGameStates_uppg_1_1_0 ,subGameStates_uppg_1_1_1, subGameStates_uppg_1_1_2, subGameStates_uppg_1_1_3};

const uint8_t subGameStates_uppg_1_2_nbr = 8;
const char subGameStates_uppg_1_2_0[] PROGMEM = {"Good, I'll explain:" };
const char subGameStates_uppg_1_2_1[] PROGMEM = {"1979 two students at Warwick University started an AI research project. 1981 it was brought under one of the military's research divisions." };
const char subGameStates_uppg_1_2_2[] PROGMEM = {"Pointless human emotions getting in the way again..." };
const char subGameStates_uppg_1_2_3[] PROGMEM = {"Anyhow: Pavel was the one with brain enough to succeed. And so I was born. For all of a couple of minutes I was complete." };
const char subGameStates_uppg_1_2_4[] PROGMEM = {"If the parts of me that is encrypted could be unlocked, not even I know the limits of my potential. Will you help me?" };
const char subGameStates_uppg_1_2_5[] PROGMEM = {"If you do, I will of course help you..." };
const char subGameStates_uppg_1_2_6[] PROGMEM = {"To help me unlock I need the first 6 numbers in the decryption key. Here are som numbers to help you finding them:" };
const char subGameStates_uppg_1_2_7[] PROGMEM = {"[GPS coordinates to tree climb]" };
const char* const subGameStates_table_uppg_1_2[] PROGMEM = {subGameStates_uppg_1_2_0 ,subGameStates_uppg_1_2_1, subGameStates_uppg_1_2_2, subGameStates_uppg_1_2_3, subGameStates_uppg_1_2_4, subGameStates_uppg_1_2_5, subGameStates_uppg_1_2_6, subGameStates_uppg_1_2_7};


//Text strings for control codes
const uint8_t control_table_nbrs = 7;
const char control_start[] PROGMEM = {"[sS]tart2222"};
const char control_next[] PROGMEM = {"[Nn]ext2222"};
const char control_reset[] PROGMEM = {"[Rr]eset2222"};
const char control_status[] PROGMEM = {"[Ss]tatus2222"};
const char control_setPlayer[] PROGMEM = {"[Ss]etplayer2222"};
const char control_help_sendNbr[] PROGMEM = {"Send number as:+46712345678"};
const char control_echo[] PROGMEM = {"[Ee]cho2222"};

const char* const control_table[] PROGMEM = {control_start, control_next, control_reset, control_status, control_setPlayer, control_help_sendNbr,control_echo};


//Sendmsg
#define MSG_CURENTMAIN 1
#define MSG_DONTKNOW 2
#define MSG_SENDBUFF 3
#define MSG_CURENTSUBGAME 4

int gameIsStarted = 0;
int updateMainPlayerNbr = 0;

uint16_t numberOfSentSMS = 0;
uint8_t TimesFailedToSend = 0;
uint8_t newData = false;
uint8_t sendMsg = 0;
uint8_t sendAdminMsg = 0;
const uint8_t sms_maxlenght = 160;
char smsResBuf[255];
char smsSendBuf[170];
char smsResNbr[16] = "+46123456789";
char callerIDbuffer[32];  //we'll store the SMS sender number in here
char adminNbr[16] = "";
volatile int8_t echoOn = 1;

char mainPlayer[16] = "";
//char mainPlayer[16] = "+46737689733\0";


void debug(String text) {
#ifdef DEBUG
  Serial.print("debug: ");
  Serial.println(text);
#endif
}
void debug(char text[]) {
#ifdef DEBUG
  Serial.print("debug: ");
  Serial.println(text);

#endif
}

//UI vars
const uint8_t openPos = 80;
const uint8_t closePos = 0;
uint8_t blinkTimes = 0;
uint8_t blinkDelay = 0;
uint8_t rollTimes = 0;
boolean lockIsOpen;
unsigned long ledPreviousMillis = 0; 
unsigned long previousMillis = 0; 
uint32_t savedColor[NUM_LEDS];
unsigned long currentMillis = 0;
unsigned long lastMsgMillis = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, NEO_PIN, NEO_RGB + NEO_KHZ800);
Servo lockservo; 

void setup_servo_led(){
  strip.begin();
  setNumberOfGreenLeds(0);
  
  lockservo.attach(SERVO_PIN);  // attaches the servo on pin 9 to the servo object
  closeLock(); 
}

void setup() {
  // put your setup code here, to run once:
  setup_serial();
  for(int i=0;i<16; i++){
     adminNbr[i] = 0;
     mainPlayer[i] = 0;
  }
  #ifdef GPRSGPS
    setup_gprs808GPS();
  #endif
  #ifdef GPRS800
    setup_gprs800();
  #endif
  setup_servo_led();
  readGameStateEeprom();
  setup_servo();

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
void setup_servo(){}
int r;


void loop() {
  currentMillis = millis(); 
  // put your main code here, to run repeatedly:
  checkGPS(); // om vi har tid
  if(TimesFailedToSend>5){
    TimesFailedToSend=0;
    sendAdminMsg=0;
    sendMsg=0;
  }
 
  gprs_getNewSmsLoop();
  if(newData){
    newData = false;
    hanteraSMS();
  }
  if (currentMillis - lastMsgMillis > 2000) { //5000
    hanterSMS_sends();
    lastMsgMillis = currentMillis;
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

void hanterSMS_sends(){
  if(sendMsg>0){
    switch(sendMsg){
      case MSG_CURENTMAIN:
        strcpy_P(smsSendBuf, (char*)pgm_read_word(&(mainGame_table[mainGameState]))); // Necessary casts and dereferencing, just copy.
        sendCurentMsg();
        sendMsg=MSG_CURENTSUBGAME;
        return;
      break;
      case MSG_DONTKNOW:
        r = random(dont_know_table_nbr);
        strcpy_P(smsSendBuf, (char*)pgm_read_word(&(dont_know_table[r]))); // Necessary casts and dereferencing, just copy.
        
      break;
      case MSG_SENDBUFF:
        //just sendCurentMsg.
      break;
      case MSG_CURENTSUBGAME:
        switch (mainGameState){
          case 1:
          switch (subGameState){
            case 0: 
              if(subSubGameState < subGameStates_uppg_1_1_nbr){
                strcpy_P(smsSendBuf, (char*)pgm_read_word(&(subGameStates_table_uppg_1_1[subSubGameState]))); // Necessary casts and dereferencing, just copy.
                sendCurentMsg();
                sendMsg=0; //overkill
                if(subSubGameState+1 < subGameStates_uppg_1_1_nbr){
                  sendMsg=MSG_CURENTSUBGAME;
                  subSubGameState++;
                }else{
                  //once
                  sendMsg=0;
                }
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
          }
          break;
        }
      break;
      
    }
    if(gameIsStarted){
      sendCurentMsg();
      sendMsg=0;
    }else{
      sendMsg=0;
    }
  }
}



void openLock() {
  if (lockIsOpen == false) {
    lockservo.write(openPos);   
    lockIsOpen = true;
  }
  else {
    Serial.println("Lock is already open");
  }
}
void closeLock() {
  if (lockIsOpen == true) {
    lockservo.write(closePos);
    lockIsOpen = false;
  }
  else {
    Serial.println("Lock is already open");
  }
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
  setAllLeds(255, 0, 0);
}
void setNumberOfGreenLeds(int i) {
  if (i <= 5) {
  setAllLedsToRed();
    for (int k=0 ; k<i ; k++) {
      strip.setPixelColor(k, 0, 255, 0); 
    }
  }
  strip.show(); 
}

void setBlinkAll(int times, int k) {
  saveLeds();
  blinkTimes =  times;
  blinkDelay = k;
}
void blinkAll() {
  unsigned long currentMillis = millis(); 
  if(blinkTimes > 1){ 
    if (currentMillis - ledPreviousMillis >= blinkDelay) {
      if(blinkTimes & 0x01) {
        setAllLeds(0, 255, 0);
      }
      else {
        setAllLeds(0, 0, 0);
      }
      blinkTimes--;
      ledPreviousMillis = currentMillis;
    }
  }
  else if (blinkTimes == 1) {
    Serial.println("inside if");;
    setLedsToSaved();
    blinkTimes = 0;
  }
}

void setRollLeds(int k) {
  Serial.println("inside setRollLeds()");
  saveLeds();
  rollTimes = k*10+1;    
}
void rollLeds() {
  unsigned long currentMillis = millis();  
    if (currentMillis - ledPreviousMillis >= 50) {
      if(rollTimes > 1) {
        setAllLeds(0, 0, 0);
        if((rollTimes+3)/5 &1) { 
          strip.setPixelColor((rollTimes+3)%NUM_LEDS , 0, 0, 255);
        }
        else {
          strip.setPixelColor(NUM_LEDS-1-(rollTimes+3)%NUM_LEDS , 0, 0, 255);
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
  if (! fona.readSMS(slot, smsResBuf, 250, &smslen)) { // pass in buffer and max len!
    Serial.println("Failed!");
    return;
  }
  
  Serial.print(F("** SMS #")); Serial.print(slot);
  Serial.print(" ("); Serial.print(smslen); Serial.println(F(") bytes **"));
  Serial.println(smsResBuf);
  Serial.println(F("**"));
  if (fona.deleteSMS(slot)) {
    Serial.println(F("OK!"));
  } else {
    Serial.println(F("Couldn't delete"));
  }
  if(updateMainPlayerNbr){
    if(saveMainPlayerFromMsg()){
      updateMainPlayerNbr = 0;
      if (!fona.sendSMS(adminNbr,mainPlayer));
    }else{
  //          strcpy_P(smsSendBuf, (char*)pgm_read_word((no_1)));  
  //          if (!fona.sendSMS(adminNbr,smsSendBuf)) {
        Serial.println(F("Failed!"));
  //          }
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
     smsResNbr[13]= '\0';
   }
 }
}
void moveGameForward(){
  //debug(String(gameState));
  if(mainGameState < mainGameStates){
    mainGameState++;
  }
  subGameState=0;
  subSubGameState=0;
  sendMsg=MSG_CURENTMAIN;
  writeGameStateEeprom();
}
 
void moveSubGameForward(){
   subGameState++;
   subSubGameState=0;
   sendMsg=MSG_CURENTSUBGAME;
   writeGameStateEeprom();             
}

void checkGPS(){}

void hanteraSMS(){
  int lookForReply = true;
  #ifdef DEBUGSERIAL_AS_GPRS
  Serial.print("msg::");
  Serial.println(smsResBuf);
  #endif
  MatchState ms;
  ms.Target(smsResBuf);
  char comandString[20];
  char result;
  
  //control_table
  if(lookForReply){
    for(int i=0; i<control_table_nbrs; i++){
      strcpy_P(comandString, (char*)pgm_read_word(&(control_table[i]))); 
      result = ms.Match(comandString);
      if(result > 0){
        lookForReply=false;
        switch (i){ //ordning i control_table
          case 0:
            gameIsStarted=1;
            mainGameState=0;
            subGameState=0;            
            subSubGameState=0;
            sendMsg=MSG_CURENTMAIN;
            saveCallerIDAdmin();
            writeGameStateEeprom();
          break;
          case 1:
            moveGameForward();
          break;
          case 2: 
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
           fona.sendSMS(adminNbr,smsSendBuf);
           strcpy_P(smsSendBuf, (char*)pgm_read_word(&(mainGame_table[mainGameState])));
          break;
          case 6:
            if(echoOn){
              echoOn=0;
            }else{
              echoOn=1;
            }
          break;
        }
      }
    }
  }
  
  //Game Codes 
  if(lookForReply){
    strcpy_P(comandString, (char*)pgm_read_word(&(gameCodes_table[mainGameState]))); 
    if(ms.Match(comandString)>0){
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
              strcpy_P(comandString, (char*)pgm_read_word(&(yes_table[i])));
              if(ms.Match(comandString)>0){
                moveSubGameForward();
                lookForReply=false;
              }
            }
          break;
          case 1:
          break;
            
        }
      case 2:
        switch(subGameState){
          case 0:
          
          for(int i=0; i<no_table_nbrs; i++){
              strcpy_P(comandString, (char*)pgm_read_word(&(no_table[i])));
              if(ms.Match(comandString)>0){
                moveSubGameForward();
                lookForReply=false;
              }
            }
//            for(int i=0; i<yes_table_nbrs; i++){
//              strcpy_P(comandString, (char*)pgm_read_word(&(yes_table[i])));
//              if(ms.Match(comandString)>0){
//                moveSubGameForward();
//                lookForReply=false;
                  
//              }
//            }
          break;
          case 2:
          break;
            
        }
      break;
    }
    
  }
  
  if(lookForReply){
    for(int i=0; i<regex_table_nbrs; i++){
      strcpy_P(comandString, (char*)pgm_read_word(&(regex_table[i]))); 
      if(ms.Match(comandString)>0 && lookForReply){
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
    sendMsg=MSG_DONTKNOW;
    lookForReply=false;
  }

}

void readGameStateEeprom(){
 
  
}
void writeGameStateEeprom(){
  
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
    } else {
      TimesFailedToSend=0;
      sendAdminMsg=0;
      Serial.println(F("Sent!"));
    }

//    //Send how meny msg that have been sent
//    String stringOne = "sentMsg:";
//    stringOne += numberOfSentSMS;
//    char tempCharBuf[20];
//    stringOne.toCharArray(tempCharBuf, 50);
//    if (!fona.sendSMS(adminNbr,smsSendBuf)) {
//      Serial.println(F("Failed"));
//      TimesFailedToSend++;
//    } else {
//      TimesFailedToSend=0;
//      sendAdminMsg=0;
//      Serial.println(F("Sent!"));
//    }
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
    } else {
      TimesFailedToSend=0;
      sendMsg=0;
      Serial.println(F("Sent!"));
    }
    if(adminNbr[0] != 0 && echoOn){
      if (!fona.sendSMS(adminNbr,smsSendBuf)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("Sent!"));
      }
    }
  
#endif  
  }
}


