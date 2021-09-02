
//Pin setup
void setup() {
   Serial.begin(9600); //baud rate setting (9600)
   pinMode(12,OUTPUT);
   pinMode(11,OUTPUT);
   pinMode(10,OUTPUT);
   pinMode(9,OUTPUT);
   pinMode(8,OUTPUT);
   pinMode(7,OUTPUT);
   pinMode(6,OUTPUT);

             
}

void loop()
{  
  if(Serial.available()>0)
  {
  //Read blueotooth serial input 
  byte c = Serial.read();
  
  //Print bluetooth serial signal on console for troubleshooting
  Serial.println(c,HEX);
  delay(400);

    //Condition from main device to turn LEFT LED ON
    if (c == 'A') 
    {
       digitalWrite(12,HIGH);
       digitalWrite(7,HIGH);
       digitalWrite(6,HIGH);
    }
    //Condition from main device to turn RIGHT LED ON
    if (c == 'B') 
    {
       digitalWrite(10,HIGH);
       digitalWrite(9,HIGH);
       digitalWrite(8,HIGH);
    }
    //Condition from main device to turn BRAKE LED ON
    if (c == 'C') 
    {
      digitalWrite(11,HIGH);
    }
    //No inputs turn all outputs off
    if (c == 'D') 
    {
      digitalWrite(12,LOW);
      digitalWrite(11,LOW); 
      digitalWrite(10,LOW);
      digitalWrite(9,LOW);
      digitalWrite(8,LOW);
      digitalWrite(7,LOW);
      digitalWrite(6,LOW);
    }
  }
}
