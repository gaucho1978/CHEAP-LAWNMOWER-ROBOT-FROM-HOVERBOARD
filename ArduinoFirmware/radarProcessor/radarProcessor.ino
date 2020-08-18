const byte EchoPin[6]={2,4,6,8,10,12};
const byte TriggerPin[6]={3,5,7,9,11,13};

unsigned int DurationUsec[6];
unsigned int tmpDurationUsec;
byte tmpDistanceCm;
byte readingCycle;

char szStr[20];

void setup() {
  for(byte i=0;i<5;i++){
    pinMode(TriggerPin[i],OUTPUT);
    pinMode(EchoPin[i],INPUT);
  }
  Serial.begin(9600);
}

void loop() {
  //one cycle max duration is 2+(10+6000)*N+Txtime= 2usec+(6010usec)*6sensors*12readings +46000usec=2usec+36060usec*11readings+46000usec=2usec+432720usec+46000usec= around half second
  // ensure trigger is low and wait 2usec
  for(byte i=0;i<5;i++){
    digitalWrite(TriggerPin[i],LOW);
  }
  delayMicroseconds(2);
  //get distance for each sensor
  for(byte i=0;i<5;i++){
    //measure
    readingCycle=1;
    RepeatMeasure:
    digitalWrite(TriggerPin[i],HIGH);
    delayMicroseconds(10); //wait the ultrasonic tx 
    digitalWrite(TriggerPin[i],LOW);
    tmpDurationUsec=pulseIn(EchoPin[i],HIGH,6000); //microseconds (0 if no echo is found). 6000usec is around 102cm
    if(readingCycle<2){
      DurationUsec[i]=tmpDurationUsec;
      readingCycle++;
      goto RepeatMeasure;
    }
    if(abs((int)DurationUsec[i]-(int)tmpDurationUsec)>412){ //412usec=7cm=40km/h
      //valore non affidabile. scarta la misura
      DurationUsec[i]=0;
    }
    
  }
  Serial.print("Distance(cm) = ");
  for(byte i=0;i<5;i++){
    tmpDistanceCm=DistanceCm(DurationUsec[i]);
    Serial.print(tmpDistanceCm); 
    Serial.print("  , ");  
  }
  Serial.print("\r\n");
}  

 
byte DistanceCm(unsigned int durationTime){
  return ((durationTime * 0.034)/2);
}

