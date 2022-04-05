
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

RF24 radio(8, 9);   // nRF24L01 (CE, CSN)
const byte address[6] = "00001";
unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

Servo throttle;  // create servo object to control the ESC
Servo rudder;
Servo elevator1Servo;
Servo elevator2Servo;
int travel=25;
int x;
int y;
int minRange = 85;
int maxRange = 170;
int throttleValue,  elevatorValue, elevator1Value, elevator2Value ;

//  size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  byte j1X;
  byte j1Y;
  byte j2X;
  byte j2Y;
};
Data_Package data; //Create a variable with the above structure

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening(); //  Set the module as receiver
  resetData();
 
  throttle.attach(5);
  elevator1Servo.attach(3); // CH3
  elevator2Servo.attach(4); // CH4

}
void loop() 
{
  // Check whether we keep receving data, or we have a connection between the two modules
  currentTime = millis();
  if ( currentTime - lastReceiveTime > 1000 ) { // If current time is more then 1 second since we have recived the last data, that means we have lost connection
    resetData(); // If connection is lost, reset the data. It prevents unwanted behavior, for eample if a drone jas a throttle up, if we lose connection it can keep flying away if we dont reset the function
  }
  // Check whether there is data to be received
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package)); // Read the whole data and store it into the 'data' structure
    lastReceiveTime = millis(); // At this moment we have received the data
  }
  int x= data.j2X;
  int y= data.j2Y;

  // Controlling throttle - BLDC
  throttleValue = constrain(data.j1X, 130, 255); // Joysticks stays in middle. So we only need values the upper values from 130 to 255
  throttleValue = map(throttleValue, 130, 255, 1000, 2000);
  throttle.writeMicroseconds(throttleValue);


  //take off||landing
    if( x<= minRange || x >= maxRange &&  y>= minRange &&y<= maxRange)//y=x
    {
      elevator1Value = map(data.j2X, 255,0, (10 + travel), (125 - travel));
      elevator2Value = map(data.j2X, 255,0, (125 - travel), (10 + travel));
      elevator1Servo.write(elevator1Value);
      elevator2Servo.write(elevator2Value);
    }


//Right
    else if( y >= maxRange && x>= minRange &&x<= maxRange)
    {
      elevatorValue = map(data.j2Y, 0, 255, (10 + travel), (125 - travel));
      elevator1Servo.write(elevatorValue);
      elevator2Servo.write(elevatorValue);
    }

// Left
    else if( y<= minRange && x>= minRange &&x<= maxRange)
    {
      elevatorValue = map(data.j2Y, 0, 255,  (10+ travel), (125-travel));
      elevator1Servo.write(elevatorValue);
      elevator2Servo.write(elevatorValue);
    }

//No Move
    else if(y < maxRange && y > minRange &&  x< maxRange &&  x> minRange)
    {
      elevator1Value=0;
      elevator2Value=0;
      elevatorValue=0;
    }

}


void resetData() 
{
   data.j1X = 127;
   data.j1Y = 127;
   data.j2X = 127;
   data.j2Y = 127;
}
