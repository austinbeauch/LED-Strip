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
const static double plus = 0xFFA857; //brightness up
const static double minus = 0xFFE01F; //brightness down

int test;
int brightness = 255;

//initial state of LEDs
bool green = false;
bool red = false;
bool blue = false;

IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  Serial.begin(9600);
  pinMode(GREEN,OUTPUT);
  pinMode(RED,OUTPUT);
  pinMode(BLUE,OUTPUT);
  irrecv.enableIRIn();
  setColourRgb(0,0,0);
}

void off(){
  digitalWrite(GREEN,LOW);
  digitalWrite(RED,LOW);
  digitalWrite(BLUE,LOW);
}

void on(){
  digitalWrite(GREEN,HIGH);
  digitalWrite(RED,HIGH);
  digitalWrite(BLUE,HIGH); 
}

void x123(int brightness){
  if(results.value == one){
    green = !green;
    green ? analogWrite(GREEN, brightness) : digitalWrite(GREEN,LOW); //turning on or off depending on state
  }
  else if(results.value == two){
    red = !red;
    red ? analogWrite(RED, brightness) : digitalWrite(RED,LOW);
  }
  else if(results.value == three){
    blue = !blue; 
    blue ? analogWrite(BLUE, brightness) : digitalWrite(BLUE,LOW);
  }
}

void updateBrightness(int brightness){
  if(green) analogWrite(GREEN, brightness);
  if(red) analogWrite(RED, brightness);
  if(blue) analogWrite(BLUE, brightness);
}

void music(){

  //TODO: Figure out why turning on any of the lights increases the read values (voltage from 12v psu might be interfering)
  //TODO: Wait for multimeter to show up so I can test voltages
  
  int volume = digitalRead(13);
  Serial.println(volume);
  
  off(); 
   
  if(green && volume>0) digitalWrite(GREEN, HIGH);
  if(red && volume>0) digitalWrite(RED, HIGH); 
  if(blue && volume>0) digitalWrite(BLUE, HIGH);   
  delay(100);
  
  off(); 
}

void cycleRGB(){

  //TODO: Work around the delay to accept more IR signals while cycling 
  //  Remove for loop and use loop functionality of the Arduino potentially
  unsigned int rgbColour[3];

  rgbColour[0] = 255;
  rgbColour[1] = 0;
  rgbColour[2] = 0;  

  //TODO: Remove these loops but maintain their functionality through the main system looping
  for (int decColour = 0; decColour < 3; decColour++) {
    int incColour = decColour == 2 ? 0 : decColour + 1;
    for(int i = 0; i < 255; i++) {   
      rgbColour[decColour] -= 1;
      rgbColour[incColour] += 1;
      setColourRgb(rgbColour[0], rgbColour[1], rgbColour[2]);
      delay(5); //right here is where most time is spent in a dead-state so receiving another button press doesn't work
    }
  }
}

void setColourRgb(unsigned int red, unsigned int green, unsigned int blue) {
  analogWrite(RED, red);
  analogWrite(GREEN, green);
  analogWrite(BLUE, blue);
 }
    
void loop() {

    //TODO: Accept more IR signals
    //TODO: Add a function to cycle whatever colours are currently turned on
    //TODO: Fix music() function

    if(irrecv.decode(&results)){ //if button has been pressed
             
      //Serial.println(results.value, HEX);
      
      if(results.value == plus && brightness<255){
        brightness += 51;
        updateBrightness(brightness);
      }
      
      else if(results.value == minus && brightness>0){
        brightness -= 51;
        updateBrightness(brightness);
      }
      
      else if(results.value == one || results.value == two || results.value == three){
        x123(brightness);
      }
      
      else if(results.value == four){  
        if(test!=4) test=4; //setting variable to maintain loop after button press
        else {
          test=0;
          updateBrightness(brightness);
        }
      } 
      
      else if(results.value == five){
        
        if(test!=5) test=5; 
        else {
          test=0;
          updateBrightness(brightness);
        }
      }
      irrecv.resume();
    }
    Serial.println(test); //test line to see if function has changed
    if(test==4) music();
    else if(test==5) cycleRGB();
}
