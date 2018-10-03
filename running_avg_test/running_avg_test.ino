const int numberOfSensors = 4;
const int numberOfSavedValues = 4;
uint16_t sensorReadings[numberOfSavedValues][numberOfSensors] = {   //[row] [column]  
  {1,   2,    3,    4},
  {5,   6,    7,    8},
  {9,   10,   11,   12},
  {13,  14,   15,   16}
};

uint16_t currentReading[numberOfSensors] = {20,21,22,23};
uint16_t runningSum[numberOfSensors] = {0,0,0,0};
double runningSumDouble[numberOfSensors] = {0,0,0,0};
double runningAvg[numberOfSensors] = {0,0,0,0};

void printMatrix(uint16_t matrix[numberOfSavedValues][numberOfSensors]) {
  Serial.println(' ');
  for (int row = 0 ; row<4 ; row++) {
    for (int col = 0 ; col<4 ; col++) {
      Serial.print(matrix[row][col]);       
      if (col==3) {Serial. println(' ');}
      else {Serial.print("\t");}
    }
  }
  Serial.println(' ');
}

void printIntArray(uint16_t array[numberOfSensors]) {
  for (int i=0 ; i<numberOfSensors ; i++) {
    Serial.print(array[i]);
    if (i==3) {Serial. println(' ');}
    else {Serial.print("\t");}
  }

}

void insertToMatrix(uint16_t current[numberOfSensors]) {
  for (int i=2 ; i>=0 ; i--) {
    for (int k=0 ; k<4 ; k++) {
      sensorReadings[i+1][k] = sensorReadings[i][k];
    } 
  }

  for (int i=0 ; i<4 ; i++) {
    sensorReadings[0][i] = current[i];
  } 
}

void calculateAvgs() {
  for (int k = 0 ; k<4 ; k++) {
    for (int i = 0 ; i<4 ; i++) {
      runningSum[k] += sensorReadings[i][k];
    }
  }
  
  for (int i = 0 ; i<4 ; i++) {
      runningSumDouble[i] = runningSum[i];
  } 
  
  for (int i = 0 ; i<4 ; i++) {
      runningAvg[i] = runningSumDouble[i]/4;
  }
}



void setup() {
  Serial.begin(9600);
  
  Serial.println("...booting");

  printMatrix(sensorReadings);
  insertToMatrix(currentReading);
  printMatrix(sensorReadings);
  calculateAvgs();
  
  Serial.print(runningAvg[0]);Serial.print("\t");
  Serial.print(runningAvg[1]);Serial.print("\t");
  Serial.print(runningAvg[2]);Serial.print("\t");
  Serial.print(runningAvg[3]);Serial.println(' ');

  
  
}//end setup

void loop() {
  // put your main code here, to run repeatedly:

}
