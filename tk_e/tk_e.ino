bool runShow = false;
uint32_t showStartMillis;
uint32_t showTimeMillis;
uint32_t currentMillis;

void setup() {

}

void loop() {
  currentMillis = millis();

  if( !digitalRead(A2) ) {
    runShow = true;
    showStartMillis = currentMillis;
  }
  
  showTimeMillis = currentMillis - showStartMillis;
  
  
  if (runShow) {
  
    switch(showTimeMillis); {
      case 0 ... 99

        break;

     case 100 ... 199:



      break;
      
    }
  
  }//end runShow
}//end loop
