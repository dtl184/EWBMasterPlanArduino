//#include <ESP8266WiFi.h> // this may help with server stuff idk tho lol

#define VREF 3.3              // analog reference voltage(Volt) of the ADC
#define SCOUNT  30            // sum of sample point

int analogBuffer[SCOUNT];     // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
float averageVoltage = 0;
float tdsValue = 0;
float temperature = 23;       // current temperature for compensation
int copyIndex = 0;
int readD1;
int readD2;
int readD3;
int readD4;
int readD5;
int Pin_D1 = 12;  // TDS
int Pin_D2 = 13;  // soil moisture
int Pin_D3 = 16;  // pH
int Pin_D4 = 4;   // turbidity
int Pin_D5 = 0;   // potentiometer/flow
 
void setup() {
    Serial.begin(9600);
    pinMode(Pin_D1, OUTPUT);
    pinMode(Pin_D2, OUTPUT);
  
}
 
int analogRead1() {
    digitalWrite(Pin_D1, HIGH); // Turn D1 On
    digitalWrite(Pin_D2, LOW); // Turn D2 Off
    digitalWrite(Pin_D3, LOW);
    digitalWrite(Pin_D4, LOW);
    digitalWrite(Pin_D5, LOW);
    return analogRead(0);
}
 
int analogRead2() {
    digitalWrite(Pin_D1, LOW); 
    digitalWrite(Pin_D2, HIGH); 
    digitalWrite(Pin_D3, LOW);
    digitalWrite(Pin_D4, LOW);
    digitalWrite(Pin_D5, LOW);
    return analogRead(0);
}

int analogRead3() {
    digitalWrite(Pin_D1, LOW); 
    digitalWrite(Pin_D2, LOW); 
    digitalWrite(Pin_D3, HIGH);
    digitalWrite(Pin_D4, LOW);
    digitalWrite(Pin_D5, LOW);
    return analogRead(0);
}

int analogRead4() {
    digitalWrite(Pin_D1, LOW); 
    digitalWrite(Pin_D2, LOW); 
    digitalWrite(Pin_D3, LOW);
    digitalWrite(Pin_D4, HIGH);
    digitalWrite(Pin_D5, LOW);
    return analogRead(0);
}

int analogRead5() {
    digitalWrite(Pin_D1, LOW); 
    digitalWrite(Pin_D2, LOW); 
    digitalWrite(Pin_D3, LOW);
    digitalWrite(Pin_D4, LOW);
    digitalWrite(Pin_D5, HIGH);
    return analogRead(0);
}
 
void loop() {
    readD1 = analogRead1(); // Read Analog value of TDS
    delay(1000);
    readD2 = analogRead2(); // Read Analog value of soil moisture
    delay(200);
    readD3 = analogRead3(); // Read Analog value of pH
    delay(200);
    readD4 = analogRead4(); // Read Analog value of turbidity
    delay(200);
    readD5 = analogRead5(); // Read Analog value of flow
    delay(200);
    printTDSValue();
    Serial.print(" / Soil Moisture = ");
    Serial.println(readD2);
    Serial.print(" / pH = ");
    Serial.println(readD3);
    Serial.print(" / Turbidity = ");
    Serial.println(readD4);
    Serial.print(" / Water Flow = ");
    Serial.println(readD5);
}

/**
 * Calculates and prints the TDS measurement
 * 
 * @details Read https://RandomNerdTutorials.com/esp8266-nodemcu-tds-water-quality-sensor/ for a detailed description of
 *          how this functions works
 * @brief takes several TDS values and finds the mean to calculate the actual TDS
 * @output "TDS Value: X ppm"
 */
void printTDSValue(){
  static unsigned long analogSampleTimepoint = millis();
  if(millis()-analogSampleTimepoint > 40U){     //every 40 milliseconds,read the analog value from the ADC
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(Pin_D1);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if(analogBufferIndex == SCOUNT){ 
      analogBufferIndex = 0;
    }
  }   
  
  static unsigned long printTimepoint = millis();
  if(millis()-printTimepoint > 800U){
    printTimepoint = millis();
    for(copyIndex=0; copyIndex<SCOUNT; copyIndex++){
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
      
      // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0;
      
      //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0)); 
      float compensationCoefficient = 1.0+0.02*(temperature-25.0);
      //temperature compensation
      float compensationVoltage=averageVoltage/compensationCoefficient;
      
      //convert voltage value to tds value
      tdsValue=(133.42*compensationVoltage*compensationVoltage*compensationVoltage - 255.86*compensationVoltage*compensationVoltage + 857.39*compensationVoltage)*0.5;
      
      //Serial.print("voltage:");
      //Serial.print(averageVoltage,2);
      //Serial.print("V   ");
      Serial.print("TDS Value:");
      Serial.print(tdsValue,0);
      Serial.println("ppm");
    }
  }
}

/**
 * Applies a filtering algorithm to get a stable TDS measurements from
 * an array of values
 */
int getMedianNum(int bArray[], int iFilterLen){
  int bTab[iFilterLen];
  for (byte i = 0; i<iFilterLen; i++)
  bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0){
    bTemp = bTab[(iFilterLen - 1) / 2];
  }
  else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}
    



/*
TO DO:
  - print units with each parameter
  - instead of serial print lone values, put into array + print
  - calibration for each analogRead function
  - steal diodes
 */
