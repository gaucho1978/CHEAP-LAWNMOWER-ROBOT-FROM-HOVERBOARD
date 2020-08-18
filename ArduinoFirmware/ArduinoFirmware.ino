const byte EchoPin[8]={2,3,4,5,6,7,8,9}; //12 and 13 are free
const byte TriggerPin[8]={A0,A1,A2,A3,A4,A5,10,11}; //A6 and A7 are analog input only

unsigned int DurationUsec[8];
unsigned int tmpDurationUsec;
byte tmpDistanceCm;
byte readingCycle;
bool debugActive=false; //set this to true in order to read sensors distances readable on serial line. set it to false to send compact message as byte array + crc

char szStr[20];

void setup() {
  for(byte i=0;i<8;i++){
    pinMode( TriggerPin[i],OUTPUT);
    pinMode(EchoPin[i],INPUT);
  }
  Serial.begin(9600);
}

void loop() {
  //one cycle max duration is 2+(10+6000)*N+Txtime= 2usec+(6010usec)*6sensors*12readings +46000usec=2usec+36060usec*11readings+46000usec=2usec+432720usec+46000usec= around half second
  // ensure trigger is low and wait 2usec
  for(byte i=0;i<8;i++){
    digitalWrite(TriggerPin[i],LOW);
  }
  delayMicroseconds(2);
  //get distance for each sensor
  for(byte i=0;i<8;i++){
    //measure
    readingCycle=1;
    RepeatMeasure:
    digitalWrite(TriggerPin[i],HIGH);
    delayMicroseconds(10); //wait the ultrasonic tx 
    digitalWrite(TriggerPin[i],LOW);
    tmpDurationUsec=pulseIn(EchoPin[i],HIGH,15700); //microseconds (0 if no echo is found). 6000usec is around 102cm
    if(tmpDurationUsec==0) tmpDurationUsec=15000;
    if(readingCycle<2){
      DurationUsec[i]=tmpDurationUsec;
      readingCycle++;
      goto RepeatMeasure;
    }
    
    if(abs((int)DurationUsec[i]-(int)tmpDurationUsec)>500){ //412usec=7cm=40km/h
      //valore non affidabile. scarta la misura
      
      DurationUsec[i]=1;
    }
    
  }
  if (debugActive){
    Serial.print("Distance(cm) = ");
    for(byte i=0;i<8;i++){
      tmpDistanceCm=DistanceCm(DurationUsec[i]);
      Serial.print(tmpDistanceCm); 
      Serial.print("  , ");  
    }
    Serial.print("\r\n");
  }else{
    byte DistanceCmArray[8];
    byte i=0;
    for(i=0;i<8;i++){
      DistanceCmArray[i]=DistanceCm(DurationUsec[i]);
      Serial.write(DistanceCmArray[i]);
    }
    unsigned int crc;
    crc = CalcCRC(0x0,DistanceCmArray, i);
    byte low = crc;
    byte high = crc >> 8;
    Serial.write(high);
    Serial.write(low);
    //Serial.write(13);
    //Serial.write(10);
    
  }
}  

 
byte DistanceCm(unsigned int durationTime){
  return ((( durationTime * 0.034))/2);
}

unsigned int CalcCRC(unsigned int  crc, byte *ptr, int count)
{
  byte i;
  
  while (--count >= 0)
  {
    crc = crc ^ (unsigned int) *ptr++ << 8;
    i = 8;
    do
    {
      if (crc & 0x8000)
      {
        crc = crc << 1 ^ 0x1021;
      }
      else
      {
        crc = crc << 1;
      }
    } while(--i);
  }
  return (crc);
}

