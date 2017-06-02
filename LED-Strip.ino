//LED-Strip.ino
//Author: Austin Beauchamp

//TODO: Accept more IR signals
//TODO: Add a function to cycle whatever colours are currently turned on
//TODO: Fix music() function
//TODO: Investigate weird coil whine when dimming lights or using cycleRBG() function.
  //dimming to a certain point causes loss of responsiveness and gives off a slight whine where led strip plugs into breadboard
//TODO: Implement manual cycleRGB() spectrum cycle speeds

#include <IRremote.h>

//pin numbers
const static int GREEN = 9;
const static int RED = 5;
const static int BLUE = 6;
const static int RECV_PIN = 11;

//Remote button values. Change these depending on remote
const static double one = 0xFF30CF; //green
const static double two = 0xFF18E7; //red
const static double three = 0xFF7A85; //blue
const static double four = 0xFF10EF; //music()
const static double five = 0xFF38C7; //cycleRGB()
const static double six = 0xFF5AA5; //cycleRGBvariable
const static double seven = 0xFF42BD;
const static double eight = 0xFF4AB5;
const static double nine = 0xFF52AD;

const static double plus = 0xFFA857; //brightness up
const static double minus = 0xFFE01F; //brightness down

int test = 0;
int brightness = 255;

//initial state of LEDs
bool green = false;
bool red = false;
bool blue = false;

IRrecv irrecv(RECV_PIN);
decode_results results;

//cycleRGB() variable
int bluearr[] = {1,1,1,0,0,0};
int redarr[] = {0, 0, 1, 1, 1, 0};
int greenarr[] = {1, 0, 0, 0, 1, 1};
int redCycle, greenCycle, blueCycle;
int cycleTime = 20;

void setup() {
  Serial.begin(9600);
  pinMode(GREEN,OUTPUT);
  pinMode(RED,OUTPUT);
  pinMode(BLUE,OUTPUT);
  irrecv.enableIRIn();
}

void off() {
  digitalWrite(GREEN,LOW);
  digitalWrite(RED,LOW);
  digitalWrite(BLUE,LOW);
}

void on() {
  digitalWrite(GREEN,HIGH);
  digitalWrite(RED,HIGH);
  digitalWrite(BLUE,HIGH);
}

//turns lights on accordingly
void x123(int brightness) {
  if (results.value == one) {
    green = !green;
    green ? analogWrite(GREEN, brightness) : digitalWrite(GREEN,LOW); //turning on or off depending on state
  }
  else if (results.value == two) {
    red = !red;
    red ? analogWrite(RED, brightness) : digitalWrite(RED,LOW);
  }
  else if (results.value == three) {
    blue = !blue;
    blue ? analogWrite(BLUE, brightness) : digitalWrite(BLUE,LOW);
  }
}

//purely updating brightness
void updateBrightness(int brightness) {
  if (green) analogWrite(GREEN, brightness);
  if (red) analogWrite(RED, brightness);
  if (blue) analogWrite(BLUE, brightness);
}

void music() {

  //TODO: Figure out why turning on any of the lights increases the read values (voltage from 12v psu might be interfering)
  //TODO: Test voltages with multimeter

  int volume = digitalRead(13);
  Serial.println(volume);

  off();

  if (green && volume > 0) digitalWrite(GREEN, HIGH);
  if (red && volume > 0) digitalWrite(RED, HIGH);
  if (blue && volume > 0) digitalWrite(BLUE, HIGH);
  delay(100);

  off();
}

void cycleRGBvariable() {

  bool pressed = false; //stores if a button has been pressed to signal a full break out of all loops
  //bool check = true;
  for (int i = 0; i < 6; i++) {

    if (pressed) break;

    int k = (i + 1)%6;

    for (int j = 0; j<256; j++) {

      if (pressed) break;

      Serial.println(cycleTime); //where the colours changes are determined

      if (blue) blueCycle = bluearr[i]*255+(bluearr[k] - bluearr[i])*j;
      if (red) redCycle = redarr[i]*255+(redarr[k]  - redarr[i])*j;
      if (green) greenCycle = greenarr[i]*255+(greenarr[k] - greenarr[i])*j;

      analogWrite (RED, redCycle);
      analogWrite (GREEN, greenCycle);
      analogWrite (BLUE, blueCycle);

      //acting as a 20ms delay and checking for button presses to know when to exit
      long startTime = millis();


      while (millis() - startTime < 5) {

        //just break out if any button is pressed.
        if (irrecv.decode(&results)) {

          Serial.print("EXIT: ");
          Serial.println(results.value, HEX);
          test=0;
          pressed = true;
          break; //hard breaking out of all the loops

        }
      }
    }
  }
  if (pressed) updateBrightness(brightness);//reset led states
}

void cycleRGB(int cycleTime) {

  bool pressed = false; //stores if a button has been pressed to signal a full break out of all loops
  //bool check = true;
  for (int i = 0; i < 6; i++) {

    if (pressed) break;

    int k = (i+1)%6;

    for (int j = 0; j < 256; j++) {

      if (pressed) break;

      Serial.println(cycleTime); //where the colours changes are determined

      blueCycle = bluearr[i]*255+(bluearr[k] - bluearr[i])*j;
      redCycle = redarr[i]*255+(redarr[k]  - redarr[i])*j;
      greenCycle = greenarr[i]*255+(greenarr[k] - greenarr[i])*j;

      analogWrite (RED, redCycle);
      analogWrite (GREEN, greenCycle);
      analogWrite (BLUE, blueCycle);

      //acting as a 20ms delay and checking for button presses to know when to exit
      long startTime = millis();

      //results.value doesn't reset the value after it's been pressed,
      // so the while loop keeps iterating on whatever statement it matches
      while (millis() - startTime < cycleTime) {

        if (irrecv.decode(&results) /*&& check*/) {

          Serial.println(results.value, HEX);

          //TODO: FIX ALL OF THIS
          if (results.value == plus && cycleTime<50) {
            cycleTime+=10;
            //check = false;
            //Serial.println(results.value, HEX);
          }

          else if (results.value == minus && cycleTime>1) {
            cycleTime-=10;
            //check = false;
          }

          else if (results.value != minus && results.value != plus && results.value != 0xFFFFFF) {
            Serial.print("EXIT: ");
            Serial.println(results.value, HEX);
            test=0;
            pressed = true;
            break; //hard breaking out of all the loops
          }
        }
      }
    }
  }
  if(pressed) off();//reset led states
}

void loop() {

  if (irrecv.decode(&results)) { //if button has been pressed
    Serial.println(results.value, HEX);

    if (results.value == plus && brightness<255) {
      brightness += 51;
      updateBrightness(brightness);
    }

    else if (results.value == minus && brightness>0) {
      brightness -= 51;
      updateBrightness(brightness);
    }

    else if (results.value == one || results.value == two || results.value == three) {
      x123(brightness);
    }

    else if (results.value == four) {
      if (test!=4) test = 4; //setting variable to maintain loop after button press
      else {
        test=0;
        updateBrightness(brightness);
      }
    }

    else if (results.value == five) {
      if (test != 5) test = 5;
      else {
        test=0;
        updateBrightness(brightness);
      }
    }

    else if (results.value == six) {
      if (test != 6) test = 6;
      else {
        test=0;
        updateBrightness(brightness);
      }
    }

    irrecv.resume();

  }

  switch (test) {
    case 4:
      music();
      break;
    case 5:
      cycleRGB(cycleTime);
      break;
    case 6:
      cycleRGBvariable();
      break;
  }

  // if(test==4) music();
  // else if(test==5) cycleRGB(cycleTime);
  // else if(test==6) cycleRGBvariable();

}
