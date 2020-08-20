/*  Code - 18 section 3 layer Recursive 08-10-20.txt
 EFO Winder
  Controls a unipolar DC Drill Motor MOSFET drive and a 180 deg RC servo
  Use the following spreadsheet to calculate initial values:  
        EFO BOBBIN WINDER.ods
  Rewritten 08/17/2020 by Paul Lindberg
*/

// the setup function runs once when you press reset or power the board
#include <Servo.h>
Servo myservo;
const int PIN_LED_READY = 13;
const int PIN_LED_ANCHOR1 = 12;
const int PIN_LED_ANCHOR2 = 11;
const int PIN_LED_ANCHOR3 = 7;
const int PIN_LED_ANCHOR4 = 6;
const int PIN_LED_DONE = 5;
const int PIN_MAGNET = 4;
const int PIN_SERVO = 3;
const int PIN_SERVO_OBJ = 9;
const int PIN_BUTTON_WINDER = 2;

const int POS_LAYER_START =     123;
const int POS_LAYER_END =        45;
const int POS_LOAD_LEFT =       140;  //position to load from left
const int POS_LOAD_ANCHOR =     100;  //position to load first anchor tape
const int POS_INC =              15;

const int SECTIONS =              6;  // Number of sections to wind

const int TURNS_SUBSEC =    26;
const int TURNS_BRAKE =      4;   //was  12 on 7-24-20

const int SPEED_SLOW =     11;
const int SPEED_RUN =      50;   //was 64 on 7-24-20
const int SPEED_BRAKE =    0;

void setup() {
  // attach the servo on pin 9 to the servo object

  myservo.attach(PIN_SERVO_OBJ);
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_MAGNET, INPUT);
  pinMode(PIN_SERVO, OUTPUT);
  pinMode(PIN_LED_READY, OUTPUT);
  pinMode(PIN_LED_ANCHOR1, OUTPUT);
  pinMode(PIN_LED_ANCHOR2, OUTPUT);
  pinMode(PIN_LED_ANCHOR3, OUTPUT);
  pinMode(PIN_LED_ANCHOR4, OUTPUT);
  pinMode(PIN_LED_DONE, OUTPUT);
  pinMode(PIN_BUTTON_WINDER, INPUT);
}

// the loop function runs over and over again forever
void loop() {  
  //********************************************************************************************************************************************************************
  //PRE-WIND  *************************************************************************************************************************************************************

  myservo.write(POS_LOAD_LEFT);         //                    ----> Move to far left, Operator take up wire slack

  //*******************************************************************
  digitalWrite(PIN_LED_READY, HIGH);    //PIN_LED_READY ON
  
  buttonKeyPress();                     //Wait for Key Press  ----> Press to move shuttle to LOAD_ANCHOR position
  
  myservo.write(POS_LOAD_ANCHOR);
  delay(100);

  //*******************************************************************
  nextLED(PIN_LED_READY, PIN_LED_ANCHOR1);      //PIN_LED_ANCHOR1 ON
                                        //                    ----> Operator place ANCHOR tape 1
      
  buttonKeyPress();                     //Wait for Key Press  ----> Press to move shuttle to winding start
  
  myservo.write(POS_LAYER_START);
  delay(100);
  
  //********************************************************************************************************************************************************************
  //WINDING LAYER 1 ***********************************************************************************************************************************************************
  
  buttonKeyPress();                     //Wait for Key Press  ----> Press to start winding

  //*******************************************************************
  
  windSection();

  //********************************************************************************************************************************************************************
  //INSULATION LAYER 1 ****************************************************************************************************************************************************
        
  nextLED(PIN_LED_ANCHOR1, PIN_LED_ANCHOR2);      //PIN_LED_ANCHOR2 ON
  
  buttonKeyPress();                       //Wait for Key Press  ------>  Press to position winding Finish
  
  //*******************************************************************
  
  windInsulation();
  
  //********************************************************************************************************************************************************************
  //WINDING LAYER 2 ****************************************************************************************************************************************************
 
  buttonKeyPress();                       //Wait for Key Press

  //*******************************************************************
  
  windSection();

  //********************************************************************************************************************************************************************
  //INSULATION LAYER 2 ****************************************************************************************************************************************************

  nextLED(PIN_LED_ANCHOR2, PIN_LED_ANCHOR3);      //PIN_LED_ANCHOR3 ON
  
  buttonKeyPress();                       //Wait for Key Press  ------>  Press to position winding Finish
  
  //*******************************************************************
  
  windInsulation();

  //********************************************************************************************************************************************************************
  //WINDING LAYER 3 ****************************************************************************************************************************************************

  buttonKeyPress();                     //Wait for Key Press
 
  //*******************************************************************
  
  windSection();
  
  //********************************************************************************************************************************************************************
  //INSULATION LAYER 3 *************************************************************************************************************************************************
  
  nextLED(PIN_LED_ANCHOR3, PIN_LED_ANCHOR4);      //PIN_LED_ANCHOR4 ON

  buttonKeyPress();                       //Wait for Key Press  ------>  Press to position winding Finish

  //********************************************************************************************************************************************************************
  //END ****************************************************************************************************************************************************************

  nextLED(PIN_LED_ANCHOR4, PIN_LED_DONE);      //PIN_LED_DONE ON
  delay(5000);
  digitalWrite(PIN_LED_DONE, LOW);//digitalWrite(PIN_LED_ANCHOR4, LOW); //remove lines 250-251 and comment this out 
}  //Go Back to Beginning of Main Loop


void nextLED(int ledprev, int lednext)
{
  digitalWrite(ledprev, LOW);
  digitalWrite(lednext, HIGH);
}

void buttonKeyPress() {
  while(digitalRead(PIN_BUTTON_WINDER)==HIGH){};        //Wait for Key Press
  delay(100);                                           //
  while(digitalRead(PIN_BUTTON_WINDER)==LOW){};         //Wait for Key Release
  delay(100);
}

void windTurns(int wPos, int wTurns) {
  myservo.write(wPos);
  int TurnCount = 0;
  while(TurnCount<wTurns){      // Count the Turns
    while(digitalRead(PIN_MAGNET)==HIGH){};
    while(digitalRead(PIN_MAGNET)==LOW){};
    TurnCount++;
  }
}

void windSection() {
  analogWrite(PIN_SERVO, SPEED_RUN);             // Turn motor FAST
  int Section = 0;
  int pos = POS_LAYER_START;
  while(Section<SECTIONS){
    Section++;

    windTurns(pos, TURNS_SUBSEC);
    
    pos=pos-POS_INC;   
  }
  
  //*******************************************************************Slowly Brake to End of the Layer

  analogWrite(PIN_SERVO, SPEED_SLOW);             // Turn motor Slowly

  windTurns(POS_LAYER_END, TURNS_BRAKE);

  analogWrite(PIN_SERVO, SPEED_BRAKE);            // Turn motor OFF
}

void windInsulation() {
  myservo.write(POS_LAYER_START);             //Move shuttle back to start the next layer
  
  analogWrite(PIN_SERVO, SPEED_SLOW+10);          // Turn motor Slowly One Rotation
  delay(200);
  
  analogWrite(PIN_SERVO, SPEED_SLOW);
  while(digitalRead(PIN_MAGNET)==HIGH){};
  while(digitalRead(PIN_MAGNET)==LOW){};
  
  analogWrite(PIN_SERVO, SPEED_BRAKE);                      // STOP Motor
}
