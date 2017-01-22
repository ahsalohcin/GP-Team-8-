
#include <SoftwareSerial.h>

const int ledPin = 13;  //led on board
SoftwareSerial mySerial(0, 1); // RX1, TX1
const int pwmPin = 3;  //Pulse width modulation pin
int dutyCycle = 0;    //Control duty cycle of pwmPin. 1-255
int led = 0;          //State of led on board. Set to 0 or 1.

//Parse input from bluetooth
void handleInput(String str)
{
  //Data is meant to modified duty cycle
  if(str.startsWith("duty cycle"))
  {
    //Grab the last 3 characters of the string, which correspond to the value passed
    String num;
    num += str.charAt(11);
    num += str.charAt(12);
    num += str.charAt(13);

    //Convert num string to an integer
    dutyCycle = num.toInt();
    Serial.print("Duty cycle: ");
    Serial.println(dutyCycle);
  }
  //Data meant for led
  else
  {
    led = str.charAt(0);
    Serial.print("Status: ");
    if(led == '0')
      Serial.println("off");
    else
      Serial.println("on");
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(9600);
  while(!Serial) {
    
   }

  //Random output to make sure Serial is up and running
  Serial.println("Set up");
  Serial.println("OK then, you first, say something.....");
  Serial.println("Go on, type something and hit Send,");
  Serial.println("or just hit the Enter key,");
  Serial.println("then I will repeat it!");
  Serial.println("");

  //Set up software serial
  mySerial.begin(9600);
  mySerial.println("My Serial");

}

void loop() {
  
  //check if there's data to be read from the bluetooth module
  if(mySerial.available() > 0){
      //Call function to parse input
      handleInput(mySerial.readString());
    }


  //control pwm
  analogWrite(pwmPin, dutyCycle);   

  //Turn on/off led on board
  if (led == '0'){
      digitalWrite(ledPin, LOW);
    }
  else if (led == '1'){
      digitalWrite(ledPin, HIGH);
    }

}
