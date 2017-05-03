
bool packetAvailable()
{
  while(BTSERIAL.available() > 0)
    {
      char incomingByte = BTSERIAL.read();

      if(incomingByte == MESSAGE_BEGIN)
      {
        //clear buffer for new incoming packet
        bufferPos = 0;
      }
      else if(incomingByte == MESSAGE_END)
      {
        //full packet received
        messageBuffer[bufferPos] = '\0';
        return true;
      }
      else
      {
        messageBuffer[bufferPos] = incomingByte;
        bufferPos++;
      }
         
    }

    return false;
}

void packetParse()
{  
  char packet_name[40];
  char packet_value[40] = {0};
  long int value = 0;
  
  //get divider position
  char *separator = strchr(messageBuffer, ':');
  *separator = '\0';

  //get command name
  strcpy(packet_name, messageBuffer);

  //skip white space
  *separator = ':';
  separator += 2;

  //get value
  for(int i = 0; *separator != '\0'; i++, separator++)
    packet_value[i] = *separator;

  exec_packet_cmd(packet_name, packet_value);

  BTSERIAL.write("\b");
  BTSERIAL.write(messageBuffer);
  BTSERIAL.write("\n");
}

void exec_packet_cmd(char *cmd_name, char* cmd_value)
{

  char chg_state[] = "change_state";
  char change_kpSpeed[] = "kpSpeed";
  char change_kiSpeed[] = "kiSpeed";

  if(strcmp(chg_state, cmd_name) == 0)
  {
      long int value = strtol(cmd_value, NULL, 10);

      if(value == 1)
        myState = PLAY;
      else
        myState = PAUSE;
  }
  else if(strcmp(change_kpSpeed, cmd_name) == 0)
  {
    double value = strtod(cmd_value, NULL);

    kpSpeed = value;
  }
  else if(strcmp(change_kiSpeed, cmd_name) == 0)
  {
    double value = strtod(cmd_value, NULL);

    kiSpeed = value;
  }
  
/*  
  Serial.write("name: ");
  Serial.println(cmd_name);

  Serial.write("value: ");
  Serial.println(cmd_value);
*/
}

void telemetry()
{
  /*
  Serial.print(" motor = ");
  Serial.print(motorValue); //corresponding value of the motor that runs at the value of PotValue
  Serial.print(" brake = ");
  Serial.print(brakeValue); 

  Serial.print("transmitter (1160 to 1830) = " );     
  Serial.print(throttleValue); //integer between 1160 and 1830

  Serial.print(" servoValueMicro: ");
  Serial.print(servoValueMicro);

  Serial.print(" steerValue: ");
  Serial.print(steerValue);

  */

  
  //String strdata = String("mtr:" + String(motorValue) + ";brk:" + String(brakeValue) + ";trst:" + String(throttleValue) + ";srv:" + String(servoValueMicro) + ";spd:" + String(wheelSpeed_R_Copy));
  String strdata = String( "vRef:" +String(vRef) + ";vError:" + String(vError));
  //Serial.println("Telemetry data: ");
  //Serial.println(strdata.c_str());

  //char data[strdata.length() + 1];
  BTSERIAL.write("\b");
  BTSERIAL.write(strdata.c_str());
  BTSERIAL.write("\n");
  //BTSERIAL.write("\b");
  //BTSERIAL.write("A");
  //BTSERIAL.write("\n");  
}
