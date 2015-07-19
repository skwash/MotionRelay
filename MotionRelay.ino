/* 
 * //////////////////////////////////////////////////
 * //making sense of the Parallax PIR sensor's output
 * //////////////////////////////////////////////////
 *
 * Switches a LED and Relay according to the state of the sensors output pin.
 * Determines the beginning and end of continuous motion sequences.
 * Additional changes from original:
 *  - Formatting / Comments
 *  - Improved output
 *  - Blink LED during calibration
 *
 * @author: Kristian Gohlke / krigoo (_) gmail (_) com / http://krx.at
 * @date:   3. September 2006 
 *
 * @author Josh Hansen / http://www.github.com/skwash
 * @date:   19 July 2015
 *
 * kr1 (cleft) 2006 
 * released under a creative commons "Attribution-NonCommercial-ShareAlike 2.0" license
 * http://creativecommons.org/licenses/by-nc-sa/2.0/de/
 *
 *
 * The Parallax PIR Sensor is an easy to use digital infrared motion sensor module. 
 * (http://www.parallax.com/detail.asp?product_id=555-28027)
 *
 * The sensor's output pin goes to HIGH if motion is present.
 * However, even if motion is present it goes to LOW from time to time, 
 * which might give the impression no motion is present. 
 * This program deals with this issue by ignoring LOW-phases shorter than a given time, 
 * assuming continuous motion is present during these phases.
 *  
 */

/////////////////////////////
//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 10000;        

//the time when the sensor outputs a low impulse
long unsigned int lowIn;         
long unsigned int detectTime;         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 5000;  

boolean lockLow = true;
boolean takeLowTime;  

int pirPin = 3;    //the digital pin connected to the PIR sensor's output
int ledPin = 13;
int relayPin = 6;

/////////////////////////////
//SETUP
void setup() {
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  
  digitalWrite(pirPin, LOW);
  digitalWrite(relayPin, LOW);
  
  //give the sensor some time to calibrate
  Serial.print("Waiting for sensor to calibrate...");

  while (calibrationTime > millis()) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }

  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}

////////////////////////////
// MAIN LOOP
void loop() {

  if (digitalRead(pirPin) == HIGH) {
    if (lockLow) {
      //makes sure we wait for a transition to LOW before any further output is made:
      lockLow = false;
      digitalWrite(ledPin, HIGH);  // Turn the LED On
      digitalWrite(relayPin,HIGH); // Turn Relay On
      Serial.println("---");
      Serial.println("Motion Detected");
      detectTime = millis();
      delay(50);
    }
    takeLowTime = true;
  }

  if (digitalRead(pirPin) == LOW) {

    if (takeLowTime) {
      lowIn = millis();    // Save the time of the transition from high to LOW
      takeLowTime = false; // Make sure this is only done at the start of a LOW phase
    }
  
    // If the sensor is low for more than the given pause, 
    // We assume that no more motion is going to happen
    if (!lockLow && millis() - lowIn > pause) {
      // Makes sure this block of code is only executed again after
      // A new motion sequence has been detected
      lockLow = true;

      digitalWrite(ledPin, LOW);  // Turn the LED Off
      digitalWrite(relayPin,LOW); // Turn Relay Off

      Serial.print("Motion Ended After ");
      Serial.print((lowIn - detectTime)/1000);
      Serial.println(" Seconds");

      Serial.print("Relay Off After ");
      Serial.print((millis() - detectTime)/1000);
      Serial.println(" Seconds");

      delay(50);
    }
  }
}

