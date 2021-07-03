#include <NewPing.h>
#include <Servo.h>
#define SERVO 10
#define ECHO 11
#define TRIG 12
#define VIB1 14//yellow
#define VIB2 6//gray
#define VIB3 7//black
#define VIB4 8//blue
#define VIB5 9//black
#define SAFE_DISTANCE 100//100
NewPing sonar(TRIG,ECHO,200);
Servo myservo;
void setup() {
  Serial.begin(9600);
  pinMode(SERVO,OUTPUT);
  pinMode(ECHO,INPUT);
  pinMode(TRIG,OUTPUT);
  vib_init();
  myservo.attach(SERVO);
}
void loop() {
  /*
  for(int i=3;i<=5;i++){
  pinMode(i,OUTPUT);
  digitalWrite(i,HIGH);
  }*/
  //myservo.write(0);
  DistanceScan();
}
#define ANGLE 8
void DistanceScan()
{
  short stepn=-1;
  int target; 
  for(int i=0;i<=180;i+=stepn){
    if(i==0||i==180) stepn*=-1;
    myservo.write(i);
    target=vibration(i);
    //Serial.print();
    if(i==72||i==108||i==144||i==180||i==37||i==73||i==109||i==36||i==145||i==0 )//
    {
      digitalWrite(target,LOW);
    }
    else if(sonar.ping_cm()<SAFE_DISTANCE&&sonar.ping_cm()>0){
      Serial.print(sonar.ping_cm());
      Serial.print('\t');
      Serial.print(i);
      Serial.print("\t");
      Serial.print(target);
      Serial.print('\n');
      digitalWrite(target,HIGH);
    }
    else
    {
      digitalWrite(target,LOW);
    }
    delay(ANGLE);
  }
}
int vibration(int n)
{
  int target=VIB1;
  if(n<=36) target=VIB1;
  else if(n<=72) target=VIB2;
  else if(n<=108) target=VIB3;
  else if(n<=144) target=VIB4;
  else if(n<=180) target=VIB5;
  //digitalWrite(target,HIGH);
  return target;
  //delay(1000);
  //digitalWrite(target,LOW);
}

void vib_init()
{
  pinMode(VIB1,OUTPUT);
  pinMode(VIB2,OUTPUT);
  pinMode(VIB3,OUTPUT);
  pinMode(VIB4,OUTPUT);
  pinMode(VIB5,OUTPUT);
  digitalWrite(VIB1,LOW);
  digitalWrite(VIB2,LOW);
  digitalWrite(VIB3,LOW);
  digitalWrite(VIB4,LOW);
  digitalWrite(VIB5,LOW);
}
