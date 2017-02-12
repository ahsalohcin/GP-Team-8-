//*************************************************************************************************
// Pseudo code for Teensy *************************************************************************
//*************************************************************************************************

void setup() {
  
   if ( mode = Performance )
   {
      //fetch old track data from SD card, save this data to RAM for quick access( global variables)
   }
   
}

void loop() {

  //get Error, Speed, and Location from arduino chip using serial communication

  //set servo steering angle as a function of Error (and past Error)

  //record servo steering angle as a function of Location

  //set duty cycles to motor driver (forward, brake, or reverse) as a function of past track data or RC transmitter commands and current Speed (and past Speed)

  //handling track data
  
    if ( //deleteButton is held for x secs)
    {
      //delete last "run" of track data from RAM
    }
  
    if ( //deleteButton is held for more than x secs)
    {
      //delete all track data from RAM 
    }
  
    if ( //deleteButton is held for even longer)
    {
      //delete all track data from SD card
    }
    
    if ( //saveButton is held for x secs )
    {
      //upload track data to SD card
      // (now its safe to turn off)
    }
}


//*************************************************************************************************
// Pseudo code for Arduino ************************************************************************
//*************************************************************************************************

void setup() {

//set hall effect pin as an interrupt

}

void loop() {
//get line of camera data

//apply moving average filter

//calculate difference 

//calculate center

//write center, speed, and location

}

//define interrupt funct()
{
  //calculate location
  //dt = millis()-prev_time;
  //calculate speed;

  //prev_time = millis();  
}

