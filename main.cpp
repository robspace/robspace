#include <Arduino.h>
/* engine_remote_control 
 *  
 *  Using servos to shift gear and work throttle on un outboard enginge using a single lever (potensiometer) as input. 
 *  Created by Robspace 2021 v0.1
 */

#include <Servo.h>

//Global Static Variables

#define REV -1
#define NEU 0
#define FOR 1 

// Pin_Layout
const int gearServoPin = 9;
const int thrServoPin = 10;
const int thrLeverPin = A0; //connected to A0


// Servo Positions and travel  (Modify values to fit physical application) (0 - 180) 
const int gearServo_rev = 0; 
const int gearServo_neutral = 90;
const int gearServo_for = 180; 

const int thrServo_idle = 50;
const int thrServo_full = 140;

// Throttle lever posisions (check using serial monitor) (0 to 1023)
const int thrLever_full_rev = 27;
const int thrLever_idle_rev = 375;
const int thrLever_neutral = 446;
const int thrLever_idle_for = 516; 
const int thrLever_full_for = 946; 

//Global variables and objects. 
int thrLeverPos = 0;
int throttle;
int currentGear = NEU;
Servo gearServo; 
Servo thrServo; 

// ----------------------------------------------------------------------------------------------------------------------------

void shift(int newGear){
  
  
  if(currentGear != newGear) {
    thrServo.write(thrServo_idle);                                                                          // set throttle to idle before shifting
    if(currentGear == FOR && newGear == REV){                       // from for to reverse (special case)
      delay(500);
      gearServo.write(gearServo_neutral);
      delay(500);
      gearServo.write(gearServo_rev); 
      currentGear = REV;
      delay(1000);
    }
     else if(currentGear == REV && newGear == FOR){                       // from reverse to for (special case)
      delay(500);
      gearServo.write(gearServo_neutral);
      delay(500);
      gearServo.write(gearServo_for); 
      currentGear = FOR;
      delay(1000);
    }
    else {                                                        // normal shifting (one gear at a time)
      // delay(500);
      if (newGear == REV){
       gearServo.write(gearServo_rev); 
      }
      if (newGear == NEU){
        gearServo.write(gearServo_neutral); 
      }
      if (newGear == FOR){
        gearServo.write(gearServo_for); 
      }
      delay(1000);
      currentGear = newGear;
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------

void setup() {
  gearServo.attach(gearServoPin);
  thrServo.attach(thrServoPin);
  thrServo.write(thrServo_idle);
  Serial.begin(9600);
  
  thrLeverPos = analogRead(thrLeverPin);                  // Set servos in neutral no throttle
  gearServo.write(gearServo_neutral);

  while(thrLeverPos < (thrLever_neutral - 20) || thrLeverPos > (thrLever_neutral + 20)){              //Wait for throttle lever to be in neutral before doing anything....
    Serial.print("Throttle not in neutral"); 
    Serial.println(thrLeverPos);
    delay(1000);
    thrLeverPos = analogRead(thrLeverPin);
  }  
}

// -----------------------------------------------------------------------------------------------------------------------------

void loop() {
  thrLeverPos = analogRead(thrLeverPin);
  
  Serial.print("Throttle lever is: " );
  Serial.print(thrLeverPos);
  Serial.print("/t Throttle Servo is: " );
  Serial.print(thrServo.read());
  Serial.print("/t current Gear is: ");
  Serial.println(currentGear);


  // Checking shifting...

  if (thrLeverPos >= (thrLever_idle_for-10)){
    shift(FOR);
  }
  
  if (thrLeverPos <= (thrLever_idle_rev+10)){
    shift(REV);
  }
  
  if (thrLeverPos > (thrLever_idle_rev + 10) && thrLeverPos < (thrLever_idle_for - 10))  {
    shift(NEU);
  }

  //Checking throttling
  if(thrLeverPos >= thrLever_idle_for){
    // We are moving forward....
    throttle = thrLeverPos - thrLever_idle_for;                                                               //Calculate trottle - movement from gearselection
    throttle = map (throttle, 0, (thrLever_full_for - thrLever_idle_for),thrServo_idle, thrServo_full);       // Map throttle from the throttlemovement of the full range of movement to the servo movement...
    thrServo.write(throttle);
  }
  if(thrLeverPos <= thrLever_idle_rev){
    // We are moving backwards....
    throttle = thrLever_idle_rev - thrLeverPos;                                                               //Calculate trottle - movement minus gears election
    throttle = map (throttle, 0, (thrLever_idle_rev - thrLever_full_rev),thrServo_idle, thrServo_full);       // Map throttle from the throttlemovement of the full range of movement to the servo movement...
    thrServo.write(throttle);
  } 
  delay (100);
}
  
// -----------------------------------------------------------------------------------------------------------------------------


