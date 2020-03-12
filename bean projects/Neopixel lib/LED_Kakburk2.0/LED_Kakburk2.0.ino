//https://learn.sparkfun.com/tutorials/ws2812-breakout-hookup-guide


#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            6
#define buttonPin      3

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      35

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 10; // delay for half a second

int redRef[NUMPIXELS];
int greenRef[NUMPIXELS];
int blueRef[NUMPIXELS];
int red[NUMPIXELS];
int green[NUMPIXELS];
int blue[NUMPIXELS];
int ledToDim[NUMPIXELS];
volatile int modeNow;
volatile int chModeDetect;
int nbrOfModes=15;
int state =0;

int johannesLed[35];


unsigned long intervall = 100;
unsigned long lastTime = 0;
unsigned long dimIntervall = 50;
unsigned long lastDimT = 0;

void setup() {
  randomSeed(analogRead(0));
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pixels.begin(); // This initializes the NeoPixel library.
  pinMode(buttonPin, INPUT);
  //digitalWrite(buttonPin,LOW);
  attachInterrupt(digitalPinToInterrupt(buttonPin), chmode, CHANGE);
  modeNow=13;
  chModeDetect=0;
  Serial.begin(9600);
  sei();
  setAll(0,0,0);
}

void loop() {
  ledDim();
  

  if(chModeDetect){
    Serial.println("Detected");
     modeNow++;
     delay(200);
    if(modeNow<nbrOfModes){
      
    }else{
      modeNow=9;
    }
    chModeDetect = 0;
    delay(400);
  }
  mode(modeNow);
  //mode(5);
  
  
  
  
  
}
void chmode(){
  chModeDetect = 1;
}
void mode(int val){
  switch(val){

    case 0:
    if(millis() - lastTime > intervall){
      setAll(0,0,0);
      lastTime=millis();
    }
    break;
    case 1:
      strobe(220,220,220,5,intervall);
    break;
    case 2:

    if(state==0){
     setRandom(255);
     state=1;     
    }
    if(millis() - lastTime > intervall){
      setAll(0,0,0);
      lastTime=millis();
      state=0;
    }
    break;
    case 3:
    if(millis() - lastTime > intervall){
      if(state>5){
        for(int i=0;i<7;i++){
        setRandom(0);
        }
        lastTime=millis();
        state=0;
      }else{
        setRandom(255);
        state++;
      }
    }    
    break;
    case 4:
    //flow(random(40),random(2)<<7,random(2)<<7,random(2)<<7,random(2)<<7,random(2)<<7,random(2)<<7,-20);
    break;
    case 5:
    // flow(random(2)<<7,random(2)<<7,random(2)<<7,(-1+random(3))<<4);
    break;
    case 6:
    chase(400);  
    break;  
    case 7:
    //flow((-1+random(3))<<5,random(2)<<7,random(2)<<7,random(2)<<7,1);
    break;
    case 8:
    //flow(-20+random(40),random(2)<<7,random(2)<<7,random(2)<<7,1);
    break;
    case 9:
    setAll(200,0,0);
    break;
    case 10:
    setAll(220,150,40);
    blinkRandom(220,150,40,10);
    break;
    case 11:
    rinnljus(120,255,200,0);
    break;
    case 12:
    if(millis() - lastTime > intervall<<7){
      setDimAll(0,0,0,1);
      setRandomDim(255,1);
      setRandomDim(255,1);
      setRandomDim(255,1);
      setRandomDim(255,1);
      lastTime=millis();
    }
    break;
    case 13:
     if(millis() - lastTime > intervall<<5){
      if(state==0){
        for(int i=0; i<20; i++){
          johannesLed[i]=random(35);
          setDim(johannesLed[i],random(0,3)<<6,random(0,3)<<6,random(0,3)<<6,random(5,7));
        }
        state++;
      }else if(state==1){
        for(int i=0; i<20; i++){
         setDim(johannesLed[i],0,0,0,random(5,7));
         state=0; 
        }        
      }
      lastTime=millis();
     }
    break;
    case 14:
    setAll(0,0,0);
    break;
    case 15:
    int rmode= random(5)+1;
    for(int i=0; i< 1<<random(3); i++){
      mode(rmode);
    }
    break;
  }
}
void ledDim(){
  if(millis() - lastDimT > dimIntervall){
    for(int i =0; i < NUMPIXELS; i++){
      if(ledToDim[i]){
        if(red[i]==redRef[i] && blue[i]==blueRef[i] && green[i]==greenRef[i]){
          ledToDim[i]=0;
        }
        if(redRef[i]>red[i]){
          red[i]+=ledToDim[i];
          if(red[i]>redRef[i]){
            red[i]=redRef[i];
          }
        }else if(red[i]>redRef[i]){
          red[i]-=ledToDim[i];
          if(red[i]<redRef[i]){
            red[i]=redRef[i];
          }
        }if(greenRef[i]>green[i]){
          green[i]+=ledToDim[i];
          if(green[i]>greenRef[i]){
            green[i]=greenRef[i];
          }
        }else if(green[i]>greenRef[i]){
          green[i]-=ledToDim[i];
          if(green[i]<greenRef[i]){
            green[i]=greenRef[i];
          }
        }
        if(blueRef[i]>blue[i]){
          blue[i]+=ledToDim[i];
          if(blue[i]>blueRef[i]){
            blue[i]=blueRef[i];
          }
        }else if(blue[i]>blueRef[i]){
          blue[i]-=ledToDim[i];
          if(blue[i]<blueRef[i]){
            blue[i]=blueRef[i];
          }
        }
        pixels.setPixelColor(i, pixels.Color(red[i],green[i],blue[i])); 
      }
     
    }
    pixels.show();
    lastDimT=millis();
  }
}
  
void setRandomDim(int color,int stepspeed){
  int led=random(NUMPIXELS);
    switch(random(6)){
    case 0:
    setDim(led,color,0,0,stepspeed);
    break;
    case 1:
    setDim(led,0,color,0,stepspeed);
    break;
    case 2:
    setDim(led,0,0,color,stepspeed);
    break;
    case 3:
    setDim(led,color,color,0,stepspeed);
    break;
    case 4:
    setDim(led,0,color,color,stepspeed);
    break;
    case 5:
    setDim(led,color,0,color,stepspeed);
    break;
  }
}
void setRandomDim(int r, int g, int b){
  int led=random(NUMPIXELS);
  setDim(led,r,g,b,10);
}

void setDimAll(int r, int g, int b, int stepspeed){
  for(int i=0;i<NUMPIXELS;i++){
    setDim(i,r,g,b,stepspeed);
  }
}
void ledOverflowFix(int led, int r, int g, int b){
  while(led<0){
    led+=NUMPIXELS;
  }
  while(led>NUMPIXELS){
    led-=NUMPIXELS;
  }
  pixels.setPixelColor(led, pixels.Color(r,g,b));
}
  
void setDim(int led, int r, int g, int b, int stepspeed){
  ledToDim[led]=stepspeed;
  redRef[led]=r;
  blueRef[led]=b;
  greenRef[led]=g;
}
void flow(int r, int g, int b,int flowSpeed){
  if(flowSpeed>0){
    if(millis()-lastTime<flowSpeed){
     if(state<NUMPIXELS){
      setAll(0,0,0);
      pixels.setPixelColor(state, pixels.Color(r,g,b));
      state++;
     }else{
      state=0;
     }
     lastTime=millis();
    }
  }else if(flowSpeed<0){
     if(millis()-lastTime<flowSpeed){
     if(0<state){
      setAll(0,0,0);
      pixels.setPixelColor(state, pixels.Color(r,g,b));
      state--;
     }else{
      state=NUMPIXELS;
     }
     lastTime=millis();
    }
  }
}
void rinnljus(int stepspeed,int red, int green, int blue){
  int redF=red>>4;
  int greenF=green>>4;
  int blueF=blue>>4;
  for(int i=5;i<NUMPIXELS+5;i++){
    pixels.setPixelColor((i+4)%NUMPIXELS, pixels.Color(redF,greenF,blueF));
    pixels.setPixelColor((i+3)%NUMPIXELS, pixels.Color(redF*4,greenF*4,blueF*4));
    pixels.setPixelColor((i+2)%NUMPIXELS, pixels.Color(redF*8,greenF*8,blueF*8));
    pixels.setPixelColor((i+1)%NUMPIXELS, pixels.Color(redF*12,greenF*12,blueF*12));
    
    pixels.setPixelColor(i%NUMPIXELS, pixels.Color(red,green,blue));
    
    pixels.setPixelColor((i-1)%NUMPIXELS, pixels.Color(redF*12,greenF*12,blueF*12));
    pixels.setPixelColor((i-2)%NUMPIXELS, pixels.Color(redF*8,greenF*8,blueF*8));
    pixels.setPixelColor((i-3)%NUMPIXELS, pixels.Color(redF*4,greenF*4,blueF*4));
    pixels.setPixelColor((i-4)%NUMPIXELS, pixels.Color(redF,greenF,blueF));
    pixels.setPixelColor((i-5)%NUMPIXELS, pixels.Color(0,0,0));
    pixels.show();
    delay(stepspeed);
  }
   
}
void flow(int flowSpeed, int red, int green, int blue, int numberDotsBin){
  if(flowSpeed>0){
    for(int i=0;i<NUMPIXELS;i++){

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      setAll(0,0,0);
      pixels.setPixelColor(i, pixels.Color(red,green,blue));
      if((i+(NUMPIXELS>>numberDotsBin)) < NUMPIXELS){
        pixels.setPixelColor((i+(NUMPIXELS>>numberDotsBin)), pixels.Color(red,green,blue));
      }else if((i-(NUMPIXELS>>numberDotsBin)) > 0){
        pixels.setPixelColor((i-(NUMPIXELS>>numberDotsBin)), pixels.Color(red,green,blue));
      }
      pixels.show();
      delay(flowSpeed);
    }
  }else if(flowSpeed<0){
        for(int i=NUMPIXELS-1;i>=0;i--){

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      setAll(0,0,0);
      pixels.setPixelColor(i, pixels.Color(red,green,blue));
      if((i+(NUMPIXELS>>numberDotsBin)) < NUMPIXELS){
        pixels.setPixelColor((i+(NUMPIXELS>>numberDotsBin)), pixels.Color(red,green,blue));
      }else if((i-(NUMPIXELS>>numberDotsBin)) > 0){
        pixels.setPixelColor((i-(NUMPIXELS>>numberDotsBin)), pixels.Color(red,green,blue));
      }
      pixels.show();
      delay(-flowSpeed);
    }
  }
}
void setRandom(int color){
  int led=random(NUMPIXELS);
  switch(random(6)){
    case 0:
    pixels.setPixelColor(led, pixels.Color(color,0,0));
    break;
    case 1:
    pixels.setPixelColor(led, pixels.Color(0,color,0));
    break;
    case 2:
    pixels.setPixelColor(led, pixels.Color(0,0,color));
    break;
    case 3:
    pixels.setPixelColor(led, pixels.Color(color,color,0));
    break;
    case 4:
    pixels.setPixelColor(led, pixels.Color(0,color,color));
    break;
    case 5:
    pixels.setPixelColor(led, pixels.Color(color,0,color));
    break;
    
  }
  pixels.show();
}
void setRandom(int red,int green, int blue){
  int led=random(NUMPIXELS);
  
  pixels.setPixelColor(led, pixels.Color(red,green,blue));

  pixels.show();
}
void blinkRandom(int red,int green, int blue, int stepspeed){
  int led=random(NUMPIXELS);
  if(random(100)<35){
    delay(stepspeed<<7);
  }else if(random(100)<70){
    delay(stepspeed<<4);
  }
  int ir =red;
  int ig =green;
  int ib =blue;
  for(int i=0;i<250;i+=5){
    if(ir<i){
      ir=i;
    }
    if(ig<i){
      ig=i;
    }
    if(ib<i){
      ib=i;
    }
    if(ir!=red || ig!=green || ib!=blue){
      pixels.setPixelColor(led, pixels.Color(ir,ig,ib));
      pixels.show();
      delay(stepspeed);
    }
  }
  for(int i=255;i>5;i-=4){
    if(ir>red){
      ir=i;
    }
    if(ig>green){
      ig=i;
    }
    if(ib>blue){
      ib=i;
    }
    if(ir!=red || ig!=green || ib!=blue){
      pixels.setPixelColor(led, pixels.Color(ir,ig,ib));
      pixels.show();
      delay(stepspeed);
    }
  }
}
void strobe(int red,int blue, int green, int ton, int toff){
      if(millis() - lastTime > toff){
        setAll(red,blue,green);
        delay(ton);    
        setAll(0,0,0);
       lastTime=millis();
      }     
}
void setAll(int r, int g, int b){
  for(int i=0;i<NUMPIXELS;i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    red[i]=r;
    green[i]=g;
    blue[i]=b;
    redRef[i]=r;
    greenRef[i]=g;
    blueRef[i]=b;
    ledToDim[i]=0;
    pixels.setPixelColor(i, pixels.Color(r,g,b)); // Moderately bright green color.
  }
  pixels.show();
}
void chase(int steptime){
  setAll(255,0,0);
  delay(steptime);
  setAll(0,255,0);
  delay(steptime);
  setAll(0,0,255);
   delay(steptime);
  

}

