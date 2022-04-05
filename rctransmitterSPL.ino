
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>

RF24 radio(7, 8);   // nRF24L01 (CE, CSN)
const byte address[6] = "00001"; // Address

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  byte j1X;
  byte j1Y;
  byte j2X;
  byte j2Y;
};

Data_Package data; //Create a variable with the above structure

void setup() {
  Serial.begin(9600);
  // Define the radio communication
  radio.begin();
  radio.openWritingPipe(address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  // Set initial default values
  data.j1X = 127; // Values from 0 to 255. When Joystick is in resting position, the value is in the middle, or 127. We actually map the pot value from 0 to 1023 to 0 to 255 because that's one BYTE value
  data.j1Y = 127;
  data.j2X = 127;
  data.j2Y = 127;
}
void loop() {
  // Read all analog inputs and map them to one Byte value
  data.j1X = map(analogRead(A0),1023,0,0,255); // Convert the analog read value from 0 to 1023 into a BYTE value from 0 to 255
  data.j1Y = map(analogRead(A1),0,1023,0,255);
  data.j2X = map(analogRead(A2),0,1023,0,255);
  data.j2Y = map(analogRead(A3),0,1023,0,255);
  
  // Send the whole data from the structure to the receiver
  radio.write(&data, sizeof(Data_Package));
}
