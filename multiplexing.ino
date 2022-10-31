const int selectPins[2] = {12, 13};
const int analogInput = A0;

void setup() 
{
  Serial.begin(9600);
  // Set up the select pins as outputs:
  for (int i=0; i < 2; i++)
  {
    pinMode(selectPins[i], OUTPUT);
    digitalWrite(selectPins[i], HIGH);
  }
  
  // Create header table
  Serial.println("Y0\tY1\tY2\tY3\tY4\tY5\tY6\tY7");
  Serial.println("---\t---\t---\t---\t---\t---\t---\t---");
}

void loop() 
{
  // Loop through all eight pins.
  for (byte pin=0; pin <= 1; pin++)
  {
      for (int i=0; i < 3; i++) {
          digitalWrite(selectPins[i], pin & (1 << i)?HIGH:LOW);
      }
      int inputValue = analogRead(analogInput);
      
      Serial.print("Device:" + String(inputValue) + "\t");
  }
  Serial.println();
  delay(1000);
}
