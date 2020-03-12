
Arduino Basics
An Arduino tutorial blog. Free Arduino tutorials for everyone !

HomeArduino Basics Projects PageForumContact AuthorMoney Jar
2 July 2015
NeoPixel Playground



The NeoPixel Digital RGB LED Strip (144 LED/m) is a really impressive product that will have you lighting up your room in next to no time. The 144 individually addressable LEDs packed onto a 1 metre flexible water resistant strip, enables a world of luminescent creativity that will blow your blinking Arduino friends away. The following tutorial will show you how to create an immersive and interactive LED display using an Arduino UNO, a potentiometer and an accelerometer. There will be a total of FIVE LED sequences to keep you entertained or you can create your own ! 
  
This tutorial was specifically designed to work with the 144 Neopixel Digital RGB LED strip with the ws2812B chipset. 

 

Parts Required:
Arduino UNO (or compatible board)
Adafruit 5V 10A switching power supply (PRODUCT ID: 658)
NeoPixel Digital RGB LED Strip (144 LED/m)
Potentiometer
3-Axis Accelerometer Module
Breadboard
Breadboard jumper wire
300 ohm resitor
4700uF 16V Electrolytic Capacitor (Jaycar Cat No. RE6243)
6 Pole PC Mount Pluggable Header (Jaycar Cat No. HM3106)
6 Pole PC Mount Pluggable Terminal Block Socket (Jaycar Cat No. HM3126)
Double Sided mounting tape - 10m (Jaycar Cat No. NM2821) - optional

Power Requirements
Before you start any LED strip project, the first thing you will need to think about is POWER. According to the Adafruit website, each individual NeoPixel LED can draw up to 60 milliamps at maximum brightness - white. Therefore the amount of current required for the entire strip will be way more than your Arduino can handle. If you try to power this LED strip directly from your Arduino, you run the risk of damaging not only your Arduino, but your USB port as well. The Arduino will be used to control the LED strip, but the LED strip will need to be powered by a separate power supply. The power supply you choose to use is important. It must provide the correct voltage, and must able to supply sufficient current.
  
Operating Voltage(5V)
The operating voltage of the NeoPixel strip is 5 volts DC. Excessive voltage will damage/destroy your NeoPixels. 

Current requirements (8.6 Amps)
OpenLab recommend the use of a 5V 10A power supply. Having more Amps is OK, providing the output voltage is 5V DC. The LEDs will only draw as much current as they need. To calculate the amount of current this 1m strip can draw with all LEDs turned on at full brightness - white: 

144 NeoPixel LEDs x 60 mA x 1 m = 8640 mA = 8.64 Amps for a 1 metre strip.

Therefore a 5V 10A power supply would be able to handle the maximum current (8.6 Amps) demanded by a single 1m NeoPixel strip of 144 LEDs. 
  
  
Arduino Libraries and IDE

Before you start to hook up any components, upload the following sketch to the Arduino microcontroller. I am assuming that you already have the Arduino IDE installed on your computer. If not, the IDE can be downloaded from here. 
  
The FastLED library is useful for simplifying the code for programming the NeoPixels. The latest "FastLED library" can be downloaded from here. I used FastLED library version 3.0.3 in this project.
  
If you have a different LED strip or your NeoPixels have a different chipset, make sure to change the relevant lines of code to accomodate your hardware. I would suggest you try out a few of the FastLED library examples before using the code below, so that you become more familiar with the library, and will be better equipped to make the necessary changes. If you have a single 144 NeoPixel LED/m strip with the ws2812B chipset, then you will not have to make any modifications below (unless you want to).
  
ARDUINO CODE:

 1
 2
 3
 4
 5
 6
 7
 8
 9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84
85
86
87
88
89
90
91
92
93
94
95
96
97
98
99
100
101
102
103
104
105
106
107
108
109
110
111
112
113
114
115
116
117
118
119
120
121
122
123
124
125
126
127
128
129
130
131
132
133
134
135
136
137
138
139
140
141
142
143
144
145
146
147
148
149
150
151
152
153
154
155
156
157
158
159
160
161
162
163
164
165
166
167
168
169
170
171
172
173
174
175
176
177
178
179
180
181
182
183
184
185
186
187
188
189
190
191
192
193
194
195
196
197
198
199
200
201
202
203
204
205
206
207
208
209
210
211
212
213
214
215
216
217
218
219
220
221
222
223
224
225
226
227
228
229
230
231
232
233
234
235
236
237
238
239
240
241
242
243
244
245
246
247
248
249
250
251
252
253
254
255
256
257
258
259
260
261
262
263
264
265
266
267
268
269
270
271
272
273
274
275
276
277
278
279
280
281
282
283
284
285
286
287
288
289
290
291
292
293
294
295
296
297
298
299
300
301
302
303
304
305
306
307
308
309
310
311
312
313
314
315
316
317
318   
    
/* ==================================================================================================================================================
         Project: NeoPixel Playground
Neopixel chipset: ws2812B  (144 LED/m strip)
          Author: Scott C
         Created: 12th June 2015
     Arduino IDE: 1.6.4
         Website: http://arduinobasics.blogspot.com/p/arduino-basics-projects-page.html
     Description: This project will allow you to cycle through and control five LED
                  animation sequences using a potentiometer and an accelerometer
                     Sequence 1:   Cylon with Hue Control                                       Control: Potentiometer only
                     Sequence 2:   Cylon with Brightness Control                                Control: Potentiometer only
                     Sequence 3:   Comet effect with Hue and direction control                  Control: Potentiometer and Accelerometer (Y axis only)
                     Sequence 4:   FireStarter / Rainbow effect with Hue and Direction control  Control: Potentiometer and Accelerometer (Y axis only)
                     Sequence 5:   Digital Spirit Level                                         Control: Accelerometer only (Y axis)
            
                  This project makes use of the FastLED library. Some of the code below was adapted from the FastLED library examples (eg. Cylon routine).
                  The Comet, FireStarter and Digital Spirit Level sequence was designed by ScottC.
                  The FastLED library can be found here: http://fastled.io/
                  You may need to modify the code below to accomodate your specific LED strip. See the FastLED library site for more details.
===================================================================================================================================================== */

//This project needs the FastLED library - link in the description.
#include "FastLED.h"

//The total number of LEDs being used is 144
#define NUM_LEDS 144

// The data pin for the NeoPixel strip is connected to digital Pin 6 on the Arduino
#define DATA_PIN 6

//Initialise the LED array, the LED Hue (ledh) array, and the LED Brightness (ledb) array.
CRGB leds[NUM_LEDS];
byte ledh[NUM_LEDS];
byte ledb[NUM_LEDS];

//Pin connections
const int potPin = A0;      // The potentiometer signal pin is connected to Arduino's Analog Pin 0
const int yPin = A4;        // Y pin on accelerometer is connected to Arduino's Analog Pin 4
                            // The accelerometer's X Pin and the Z Pin were not used in this sketch

//Global Variables ---------------------------------------------------------------------------------
byte potVal;                // potVal:      stores the potentiometer signal value
byte prevPotVal=0;          // prevPotVal:  stores the previous potentiometer value
int LEDSpeed=1;             // LEDSpeed:    stores the "speed" of the LED animation sequence
int maxLEDSpeed = 50;       // maxLEDSpeed: identifies the maximum speed of the LED animation sequence
int LEDAccel=0;             // LEDAccel:    stores the acceleration value of the LED animation sequence (to speed it up or slow it down)
int LEDPosition=72;         // LEDPosition: identifies the LED within the strip to modify (leading LED). The number will be between 0-143.  (Zero to NUM_LEDS-1)
int oldPos=0;               // oldPos:      holds the previous position of the leading LED
byte hue = 0;               // hue:         stores the leading LED's hue value
byte intensity = 150;       // intensity:   the default brightness of the leading LED
byte bright = 80;           // bright:      this variable is used to modify the brightness of the trailing LEDs
int animationDelay = 0;     // animationDelay: is used in the animation Speed calculation. The greater the animationDelay, the slower the LED sequence.
int effect = 0;             // effect:      is used to differentiate and select one out of the four effects
int sparkTest = 0;          // sparkTest:   variable used in the "sparkle" LED animation sequence 
boolean constSpeed = false; // constSpeed:  toggle between constant and variable speed.


//===================================================================================================================================================
// setup() : Is used to initialise the LED strip
//===================================================================================================================================================
void setup() {
    delay(2000);          //Delay for two seconds to power the LEDS before starting the data signal on the Arduino
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);                            //initialise the LED strip       
}


//===================================================================================================================================================
// loop() : The Arduino will take readings from the potentiometer and accelerometer to control the LED strip
//===================================================================================================================================================
void loop(){
  readPotentiometer();           
  adjustSpeed();
  constrainLEDs();
 
  switch(effect){
    case 0:                                               // 1st effect : Cylon with Hue control - using Potentiometer 
      cylonWithHueControl();
      break;
      
    case 1:                                               // 2nd effect : Cylon with Brightness control - using Potentiometer
      cylonWithBrightnessControl();
      break;
      
    case 2:                                               // 3rd effect : Comet effect. Hue controlled by potentiometer, direction by accelerometer
      cometEffect();
      break;
      
    case 3:                                               // 4th effect : FireStarter / Rainbow Sparkle effect. Direction controlled by accelerometer, sparkle by potentiometer.
      fireStarter(); 
      break;
    
    case 4:
      levelSense();                                        // 5th effect : LevelSense - uses the accelerometer to create a digital "spirit" level.
      break;
  }
}


//===================================================================================================================================================
// readPotentiometer() : Take a potentiometer reading. This value will be used to control various LED animations, and to choose the animation sequence to display.
//===================================================================================================================================================
void readPotentiometer(){
  //Take a reading from the potentiometer and convert the value into a number between 0 and 255
  potVal = map(analogRead(potPin), 0, 1023 , 0, 255);
  
  // If the potentiometer reading is equal to zero, then move to the next effect in the list.
  if(potVal==0){
    if(prevPotVal>0){   // This allows us to switch effects only when the potentiometer reading has changed to zero (from a positive number). Multiple zero readings will be ignored.
      prevPotVal = 0;   // Set the prev pot value to zero in order to ignore replicate zero readings.
      effect++;         // Go to the next effect.
      if(effect>4){
        effect=0;       // Go back to the first effect after the fifth effect.
      }
    }
  }
  prevPotVal=potVal;    // Keep track of the previous potentiometer reading
}


//===================================================================================================================================================
// adjustSpeed() : use the Y axis value of the accelerometer to adjust the speed and the direction of the LED animation sequence
//===================================================================================================================================================
void adjustSpeed(){
  // Take a reading from the Y Pin of the accelerometer and adjust the value so that 
  // positive numbers move in one direction, and negative numbers move in the opposite diraction. 
  // We use the map function to convert the accelerometer readings, and the constrain function to ensure that it stays within the desired limits
  // The values of 230 and 640 were determined by trial and error and are specific to my accelerometer. You will need to adjust these numbers to suit your module.
  
  LEDAccel = constrain(map(analogRead(yPin), 230, 640 , maxLEDSpeed, -maxLEDSpeed),-maxLEDSpeed, maxLEDSpeed);
  
  
  // If the constSpeed variable is "true", then make sure that the speed of the animation is constant by modifying the LEDSpeed and LEDAccel variables.
  if(constSpeed){
    LEDAccel=0; 
    if(LEDSpeed>0){
      LEDSpeed = maxLEDSpeed/1.1;     // Adjust the LEDSpeed to half the maximum speed in the positive direction
    } 
    if (LEDSpeed<0){
      LEDSpeed = -maxLEDSpeed/1.1;    // Adjust the LEDSpeed to half the maximum speed in the negative direction
    }
  } 
 
  // The Speed of the LED animation sequence can increase (accelerate), decrease (decelerate) or stay the same (constant speed)
  LEDSpeed = LEDSpeed + LEDAccel;                        
  
  //The following lines of code are used to control the direction of the LED animation sequence, and limit the speed of that animation. 
  if (LEDSpeed>0){
    LEDPosition++;                                       // Illuminate the LED in the Next position
    if (LEDSpeed>maxLEDSpeed){
      LEDSpeed=maxLEDSpeed;                              // Ensure that the speed does not go beyond the maximum speed in the positive direction
    }
  }
  
  if (LEDSpeed<0){
    LEDPosition--;                                       // Illuminate the LED in the Prior position
    if (LEDSpeed<-maxLEDSpeed){
      LEDSpeed = -maxLEDSpeed;                           // Ensure that the speed does not go beyond the maximum speed in the negative direction
    }
  }
}


//===================================================================================================================================================
// constrainLEDs() : This ensures that the LED animation sequence remains within the boundaries of the various arrays (and the LED strip)
//                   and it also creates a "bouncing" effect at both ends of the LED strip.
//===================================================================================================================================================
void constrainLEDs(){
  LEDPosition = constrain(LEDPosition, 0, NUM_LEDS-1);    // Make sure that the LEDs stay within the boundaries of the LED strip
  if(LEDPosition == 0 || LEDPosition == NUM_LEDS-1) {
    LEDSpeed = (LEDSpeed * -0.9);                         // Reverse the direction of movement when LED gets to end of strip. This creates a bouncing ball effect.
  }
}



//===================================================================================================================================================
// cylonWithHueControl() :  This is the 1st LED effect. The cylon colour is controlled by the potentiometer. The speed is constant.
//===================================================================================================================================================
void cylonWithHueControl(){
      constSpeed = true;                                  // Make the LED animation speed constant
      showLED(LEDPosition, potVal, 255, intensity);       // Illuminate the LED
      fadeLEDs(8);                                        // Fade LEDs by a value of 8. Higher numbers will create a shorter tail.
      setDelay(LEDSpeed);                                 // The LEDSpeed is constant, so the delay is constant
}


//===================================================================================================================================================
// cylonWithBrightnessControl() : This is the 2nd LED effect. The cylon colour is red (hue=0), and the brightness is controlled by the potentiometer
//===================================================================================================================================================
void cylonWithBrightnessControl(){
      constSpeed = true;                                  // Make speed constant
      showLED(LEDPosition, 0, 255, potVal);               // Brightness is controlled by potentiometer.
      fadeLEDs(16);                                       // Fade LEDs by a value of 16
      setDelay(LEDSpeed);                                 // The LEDSpeed is constant, so the delay is constant
}


//===================================================================================================================================================
// cometEffect() :  This is the 3rd LED effect. The random brightness of the trailing LEDs produces an interesting comet-like effect.
//===================================================================================================================================================
void cometEffect(){
      constSpeed = false;                                  // The speed will be controlled by the slope of the accelerometer (y-Axis)
      showLED(LEDPosition, potVal, 255, intensity);        // Hue will change with potentiometer.
      
      //The following lines create the comet effect 
      bright = random(50, 100);                            // Randomly select a brightness between 50 and 100
      leds[LEDPosition] = CHSV((potVal+40),255, bright);   // The trailing LEDs will have a different hue to the leading LED, and will have a random brightness
      fadeLEDs(8);                                         // This will affect the length of the Trailing LEDs
      setDelay(LEDSpeed);                                  // The LEDSpeed will be affected by the slope of the Accelerometer's y-Axis
}


//===================================================================================================================================================
// fireStarter() : This is the 4th LED effect. It starts off looking like a ball of fire, leaving a trail of little fires. But as you
//                 turn the potentiometer, it becomes more like a shooting star with a rainbow-sparkle trail.
//===================================================================================================================================================
void fireStarter(){
      constSpeed = false;                                  // The speed will be controlled by the slope of the accelerometer (y-Axis)
      ledh[LEDPosition] = potVal;                          // Hue is controlled by potentiometer
      showLED(LEDPosition, ledh[LEDPosition], 255, intensity); 
      
      //The following lines create the fire starter effect
      bright = random(50, 100);                            // Randomly select a brightness between 50 and 100
      ledb[LEDPosition] = bright;                          // Assign this random brightness value to the trailing LEDs
      sparkle(potVal/5);                                   // Call the sparkle routine to create that sparkling effect. The potentiometer controls the difference in hue from LED to LED.
      fadeLEDs(1);                                         // A low number creates a longer tail
      setDelay(LEDSpeed);                                  // The LEDSpeed will be affected by the slope of the Accelerometer's y-Axis
}


//===================================================================================================================================================
// levelSense() : This is the 5th and final LED effect. The accelerometer is used in conjunction with the LED strip to create a digital "Spirit" Level.
//                You can use the illuminated LEDs to identify the angle of the LED strip
//===================================================================================================================================================
void levelSense(){
      constSpeed = true;
      LEDPosition = constrain(map(analogRead(yPin), 230, 640, 1, NUM_LEDS-1), 0 , NUM_LEDS-1);
      
      //Jitter correction: this will reduce the amount of jitter caused by the accelerometer reading variability
      if(abs(LEDPosition-oldPos) < 2){
        LEDPosition = oldPos;
      }
      
      //The following lines of code will ensure the colours remain within the red to green range, with green in the middle and red at the ends.
      hue = map(LEDPosition, 0, NUM_LEDS-1, 0, 200);
      if (hue>100){
         hue = 200 - hue;
      }
      
      //Illuminate 2 LEDs next to each other
      showLED(LEDPosition, hue, 255, intensity); 
      showLED(LEDPosition-1, hue, 255, intensity);              
      
      //If the position moves, then fade the old LED positions by a factor of 25 (high numbers mean shorter tail)
      fadeLEDs(25);                               
      oldPos = LEDPosition; 
}


//===================================================================================================================================================
// fadeLEDs(): This function is used to fade the LEDs back to black (OFF) 
//===================================================================================================================================================
void fadeLEDs(int fadeVal){
  for (int i = 0; i<NUM_LEDS; i++){
    leds[i].fadeToBlackBy( fadeVal );
  }
}



//===================================================================================================================================================
// showLED() : is used to illuminate the LEDs 
//===================================================================================================================================================
void showLED(int pos, byte LEDhue, byte LEDsat, byte LEDbright){
  leds[pos] = CHSV(LEDhue,LEDsat,LEDbright);
  FastLED.show();
}


//===================================================================================================================================================
// setDelay() : is where the speed of the LED animation sequence is controlled. The speed of the animation is controlled by the LEDSpeed variable.
//              and cannot go faster than the maxLEDSpeed variable.
//===================================================================================================================================================
void setDelay(int LSpeed){
  animationDelay = maxLEDSpeed - abs(LSpeed);
  delay(animationDelay);
}


//===================================================================================================================================================
// sparkle() : is used by the fireStarter routine to create a sparkling/fire-like effect
//             Each LED hue and brightness is monitored and modified using arrays  (ledh[]  and ledb[])
//===================================================================================================================================================
void sparkle(byte hDiff){
  for(int i = 0; i < NUM_LEDS; i++) {
    ledh[i] = ledh[i] + hDiff;                // hDiff controls the extent to which the hue changes along the trailing LEDs
    
    // This will prevent "negative" brightness.
    if(ledb[i]<3){
      ledb[i]=0;
    }
    
    // The probability of "re-igniting" an LED will decrease as you move along the tail
    // Once the brightness reaches zero, it cannot be re-ignited unless the leading LED passes over it again.
    if(ledb[i]>0){
      ledb[i]=ledb[i]-2;
      sparkTest = random(0,bright);
      if(sparkTest>(bright-(ledb[i]/1.1))){
        ledb[i] = bright;
      } else {
        ledb[i] = ledb[i] / 2;                  
      }
    }
    leds[i] = CHSV(ledh[i],255,ledb[i]);
  }
}


    
    

  
NeoPixel Strip connection
The NeoPixel strip is rolled up when you first get it. You will notice that there are wires on both sides of the strip. This allows you to chain LED strips together to make longer strips. The more LEDs you have, the more current you will need. Connect your Arduino and power supply to the left side of the strip, with the arrows pointing to the right side of the strip. 
  
 
Follow the Arrows
The arrows are quite hard to see on this particular LED strip because they are so small, plus they are located right under the thicker part of the NeoPixel weatherproof sheath. I have circled the arrows in RED so that you know where to look:

  
 


NeoPixel Strip Wires
There are 4 wires coming from either side of the NeoPixel LED strip: 
  
  One red wire, one white wire, and two black wires.
  
It doesn't matter which Black wire you use to connect to the power supply (or Arduino) GND. Both black wires appear to be going to the same pin on the LED strip anyway. Use the table below to make the necessary NeoPixel Strip connections to the Arduino and power supply.

 

Large Capacitor
Adafruit also recommend the use of a large capacitor across the + and - terminals of the LED strip to "prevent the initial onrush of current from damaging the pixels". Adafruit recommends a capacitor that is 1000uF, 6.3V or higher. I used a 4700uF 16V Electrolytic Capacitor. 

Resistor on Data Pin
Another recommendation from Adafruit is to place a "300 to 500 Ohm resistor" between the Arduino's data pin and the data input on the first NeoPixel to prevent voltage spikes that can damage the first pixel. I used a 330 Ohm resistor. 
  
Powering your Arduino (USB vs Power supply)
You can power your Arduino board via USB cable or via the LED strip power supply.
*** Please note: different power supplies will yield different accelerometer readings. I noticed this when changing the Arduino's power source from USB to LED power supply. My final sketch was designed to eliminate the USB/computer connection, hence I have chosen to power the Arduino via the power supply. The fritzing sketch below shows the Arduino being powered by a power supply only. 

**WARNING: If you decide to power your Arduino UNO via a USB cable, please make sure to remove (or disconnect) the wire that goes to the the Arduino VIN pin. The GND connections remain unchanged.

Fritzing Sketch - NeoPixel strip connection
 

  
Potentiometer connection
The potentiometer will be used to switch between the different LED sequences. When it reads zero, it will switch to the next sequence in the list. It will jump right back to the beginning after the last sequence. The potentiometer is also used to interact with the LEDs (e.g. controlling hue, brightness etc etc). 
See the fritzing sketch below to add the potentiometer to this project.


 

  
Accelerometer connection (Y-axis)
The accelerometer makes the LEDs much more fun and interactive. We will only be using the Y-axis of the accelerometer in this sketch. By tilting the accelerometer from one side to the other, the LEDs react and respond accordingly. The accelerometer is an essential component of the digital spirit level sequence. That's right ! You can use this sketch to create your own spirit level. This digital version can also be used to measure angles ! 
  
Have a look below to see how to hook up the accelerometer to the Arduino. The Y-axis is connected to the Arduino analog pin 4. If you wanted to use the X and Z axis, connect them to one of the other available analog pins (eg. A3 and A5).


 


 
Let the fun begin !!
Now that you have the Arduino code uploaded to the Arduino, and have made all of the necessary wire/component connections, it is time to turn on the power supply. 
  
Sequence 1: Cylon with Hue control
The LEDs will move from one end of the strip to the other. It should start off as a RED cylon effect. As you turn the potentiometer clockwise, the colour of the LEDs will change and move through the various colours of the rainbow. If the potentiometer reading gets back to zero (fully anti-clockwise), it will move to sequence 2.
  
Sequence 2: Cylon with brightness control
You will see that the LEDs have turned off. The potentiometer readings correlate with the LED brightness. At the start of this sequence, the potentiometer readings will be zero, therefore the brightness will be zero (LEDs turned off). As you turn the potentiometer clockwise, the readings increase, and so will the brightness of the LEDs. 
  
Sequence 3: Comet effect with Hue and direction control
This is where the real fun begins. You control the hue of the leading LED with the potentiometer, however the LED will move along the LED strip as though it were affected by gravity. As it hits the end of the LED strip, it will bounce for a while and eventually come to a stop. The more you tilt the accelerometer, the greater the acceleration of the leading LED. The trailing LEDs have an interesting randomised glow, which creates the "comet" effect.
  
Sequence 4: FireStarter / Rainbow effect : Hue and direction control
The initial colours of LEDs in this sequence creates a fire-like animation. As the leading LED moves along the LED strip, it appears to ignite the LEDs in its path, leaving a fire trail behind it. The fire effect is best when you turn the potentiometer clockwise slightly to introduce a small amount of yellow into the mix of colours. As you turn the potentiometer further clockwise, the fire trail turns into a pretty rainbow trail. The accelerometer affects the leading LED in the same way as the previous sequence. 
  
Sequence 5: Digital spirit level
This sequence was my original idea for this project, however I thought it would be nice to share some of the other cool effects I created on my journey of discovery. The idea was to make a digital version of a spirit level. I originally wanted the LEDs to represent a spirit level bubble that would "float" according to the vertical/horizontal position of the LED strip. However, as I played around with this sketch, I discovered that it could potentially be used to measure the angle of the strip relative to the horizon. The angle can be determined by the illuminated LED. If the strip is horizontal, the illuminated LEDs will be close to the middle of the strip, and their colour will be green. If the strip is vertical, the illuminated LEDs will be close to end of the strip, and their colour will be red. The colour is just an additional visual indicator. 
 

Concluding Comments
The NeoPixel Digital RGB LED strip is a lot of fun. The FastLED library makes for easy programming, and allows you to get up and running really quickly. 144 LEDs on a single strip means you have plenty of room for creative algorithms and lighting effects. Add a few sensors, and "pretty" quickly turns into "awesome" !!
  
This tutorial shows you how to control a "144 NeoPixel per metre Digital RGB LED strip" with an Arduino UNO. Feel free to share your own LED creations in the comments below.


If you like this page, please do me a favour and show your appreciation : 

  
Visit my ArduinoBasics Google + page.
Follow me on Twitter by looking for ScottC @ArduinoBasics.
I can also be found on Pinterest and Instagram. 
Have a look at my videos on my YouTube channel.

  
 
             

This project would not have been possible without the collaborative effort from OpenLab.
Please visit their site for more cool projects.



However, if you do not have a google profile... 
Feel free to share this page with your friends in any way you see fit.

Posted by Scott C at 10:00 
Email This
BlogThis!
Share to Twitter
Share to Facebook
Share to Pinterest

Labels: 144 LED, accelerometer, adafruit, Arduino, ArduinoBasics, Comet, Cylon, Digital, Fire Starter, How to, Level, NeoPixels, OpenLab, Playground, Potentiometer, power, project, Sequence, Spirit, tutorial
6 comments:

coGhen19 August 2015 at 22:28
Great job!

Reply

Marco Caldara15 November 2015 at 19:34
Very Good

Reply
Replies

Scott C15 November 2015 at 21:04
Thanks

Reply

Scott C13 December 2015 at 11:16
Mazlan Ismail - I have not included your comment - due to the large amount of data posted. I would suggest that you try this project with an earlier version of Arduino IDE compared to the one you are using.

Other than that - I am not sure why it is not working for you ?
Check that you have installed the FastLED properly, by trying one of their examples. Make sure you can get that to work.

Reply

Anonymous1 February 2016 at 19:54
Sorry for my bad English.I'm a newbie of arduino and i would like to ask you if the code works even without the accelerometer...
Thanks in advance for your reply.

Reply
Replies

Scott C2 February 2016 at 08:05
Yes it would work, but not very well. The animations rely on the Accelerometer values. You would have to modify the code to accommodate the change.

Reply

Feel free to leave a comment about this tutorial below. 
Any questions about your particular project should be asked in the ArduinoBasics forum.

Comments are moderated due to large amount of spam.

Links to this post
Create a Link

Newer Post Older Post Home
Subscribe to: Post Comments (Atom)
Search This Blog

Search
Translate

Drivs av Översätt
Pages
Arduino Basics Projects Page
Forum
Arduino Basics YouTube Videos
Arduino Webserver Data Viewer
Money Jar
Subscribe
   Posts
   Comments
Total Pageviews
Sparkline 2,441,977

This Week's Most Popular Posts
433 MHz RF module with Arduino Tutorial 1
HC-SR04 Ultrasonic Sensor
Simple Arduino Serial Communication
Relay Module
433 MHz RF module with Arduino Tutorial 2
Most Recent Posts
Circuit Diagram
Get Arduino Data over the internet using jQuery and AJAX
Two Million Views
Generosity Campaign Update - Day 3
Generosity Campaign Update - Day 2
Recent Posts Widget by Helplogger
Awesome Inc. template. Powered by Blogger.
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
