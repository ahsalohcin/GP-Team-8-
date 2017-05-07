void getline(double lineBuffer[])
{ 
  //digitalWrite(triggerPin2, HIGH);
  digitalWrite(pinSI, HIGH);
  digitalWrite(pinCL, HIGH);
  lineBuffer[0] = 0.0*lineBuffer[0] + 0.8*analogRead(pinAO);
  digitalWrite(pinSI, LOW);
  digitalWrite(pinCL, LOW);
  //Serial.print(" Thresh: ");
  //Serial.print(threshhold);
  //Serial.print(lineBuffer[0],0);
 int i;
  for (i=1;i<128;i++)
  {
    digitalWrite(pinCL, HIGH);
    lineBuffer[i] = analogRead(pinAO);
    digitalWrite(pinCL, LOW);
    //Serial.print(lineBuffer[i],0);
    //Serial.print(" ");
      //Serial.print(',');
      //Serial.print(lineBuffer[i],0);
    /*
    if (out[i] > threshhold)
    Serial.print("1");
    else 
    Serial.print("0");
    */
  }
      //Serial.print(';');
      //Serial.print('\n');
  //digitalWrite(triggerPin2, LOW);
}

void averageElements(double input[], int arraySize, int numsToAverage, double result[])
{
  double sum;
  int elementsUsed; //keep track of how many elements were used for sum
  double average;

  for(int i = 0; i < arraySize; i++)
  {
    sum = 0;
    elementsUsed = 0;
  
    for(int j = 1; j <= numsToAverage; j++)
    {
      //check for valid index going to left of array
      if((i-j) >= 0)
      {
        sum += input[i-j];
        elementsUsed++;  
      }

      //check for valid index going to right of array
      if((i+j) < arraySize)
      {
        sum += input[i+j];
        elementsUsed++;
      }
    }// end of inner loop

    //calculate average
    average = sum / elementsUsed;
    result[i] = average;
    
  } //end of outer loop
  
}

void diff(double input[], int arraySize, double result[])
{
  for(int i = 1; i < arraySize; i++)
    result[i-1] = input[i] - input[i-1];
}

double center(double input[], int arraySize)
{
  //Index of max and min
  int maxIndex = 0;
  int minIndex = 0;
  int maxValue = 0;
  int minValue = 1024.0;  

  //Find max and min
  for(int i = 0; i < arraySize; i++)
  {
    if(input[i] < minValue)
    {
      minValue = input[i];
      minIndex = i;
    }
    else if(input[i] > maxValue)
    {
      maxValue = input[i];
      maxIndex = i;
    }
  }

  Serial.print(" Max diff (1024): ");
  Serial.print(maxValue);
  return (minIndex + maxIndex) / 2.0;

}
