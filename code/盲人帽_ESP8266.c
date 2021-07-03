#include <SoftwareSerial.h>
#include <AliyunIoTSDK.h>
#include <ESP8266WiFi.h>
#include <string.h>

/*WiFi*/
#define WIFI_SSID "******"
#define WIFI_PASSWD "******"


/*loT device*/
#define PRODUCT_KEY "************"
#define DEVICE_NAME "************"
#define DEVICE_SECRET "***********************"
#define REGION_ID "cn-shanghai"
#define LED_PIN 5
#define GPS_RX 14
#define GPS_TX 12
#define MAX_BUFFER 600
WiFiClient espClient;
unsigned int interval=60;//GPS上报间隔，单位s
SoftwareSerial mySerial(GPS_RX,GPS_TX);
static unsigned long lastTimeGPS=0;
static char GPSbuffer[MAX_BUFFER];
static char Longitude[12]="",Latitude[12]="",Altitude[12]="";
static char Longitude_flag,Latitude_flag;
static unsigned int n;
unsigned int calchar(unsigned int i,unsigned int count,char c=',');
void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  wifiInit(WIFI_SSID,WIFI_PASSWD);
  AliyunIoTSDK::begin(espClient,PRODUCT_KEY,DEVICE_NAME,DEVICE_SECRET,REGION_ID);
  AliyunIoTSDK::bindData("LEDState",LEDbtn);
  AliyunIoTSDK::send("WiFiState",1);
  AliyunIoTSDK::bindData("GPS_Interval",ChangeInterval);
  delay(100);
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,1);
  pinMode(GPS_RX,INPUT);
  pinMode(GPS_TX,OUTPUT);
}
void loop() {
  AliyunIoTSDK::loop();//send the data to Aliyun IoT
  if(millis()-lastTimeGPS>=interval*1000){
    lastTimeGPS=millis();
    GPSDataProcessing();
    //printf("Longitude=%d,Latitude=%d,Altitude=%d\n",Longitude,Latitude,Altitude);
    if(n>30){//strlen(Longitude)>0&&strlen(Latitude)>0&&strlen(Altitude)>0
      char s[100]="{\"GeoLocation\":{\"Longitude\":";
      if(Longitude_flag=='W') strcat(s,"-");//东经添加负号
      strcat(s,Longitude);
      strcat(s,",\"Latitude\":");
      if(Latitude_flag=='S') strcat(s,"-");//南纬添加负号
      strcat(s,Latitude);
      strcat(s,",\"Altitude\":");
      strcat(s,Altitude);
      strcat(s,",\"CoordinateSystem\":2}}");
      Serial.print(Longitude);
      Serial.print('\t');
      Serial.println(Latitude);
      Serial.println(s);
      AliyunIoTSDK::send(s);
    }
    else Serial.print("None\n");
  }
}

void wifiInit(const char* ssid,const char* passwd){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,passwd);
  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
}
void LEDbtn(JsonVariant p){
  int btn=p["LEDState"];
  Serial.printf("btn=%d\n",btn);
  //AliyunIoTSDK::send("LEDState",digitalRead(LED_PIN));
  digitalWrite(LED_PIN,btn);
}
void ChangeInterval(JsonVariant p)
{
  interval=p["GPS_Interval"];
  Serial.printf("Change GPS_Interval=%d\n",interval);
}
void GPSDataProcessing()//北斗模块数据处理
{
  unsigned i=0,j=0,k=0;
  n=0;
  //if(mySerial.available())
    //Serial.print("Receive\n");
  while(mySerial.available()&&n<MAX_BUFFER-5){
    GPSbuffer[n++]=mySerial.read();//read MAX_BUFFER byte data
    Serial.print(GPSbuffer[n-1]);
  }
  if(n<30) return;//读取数据不完整直接返回
  while(i<n-4){//i<n-2定位到GGA部分
    if(GPSbuffer[i]=='G'&&GPSbuffer[i+1]=='G'&&GPSbuffer[i+2]=='A') break;
    i++;
  }
  //Latitude
  i=calchar(i,2);
  j=calchar(i,1);
  k=calchar(i,1,'.');
  for(unsigned int l=k-1;l>i+2;l--) GPSbuffer[l]=GPSbuffer[l-1];
  GPSbuffer[i+2]='.';
  memcpy(Latitude,&GPSbuffer[i],j-i-1);
  //Serial.print("\nj-i-1=");Serial.print(j-i-1);Serial.print('\n');
  Latitude[j-i-1]='\0';
  Latitude_flag=GPSbuffer[j];
  //Longitude
  i=j+2;
  j=calchar(i,1);
  k=calchar(i,1,'.');
  for(unsigned int l=k-1;l>i+3;l--) GPSbuffer[l]=GPSbuffer[l-1];
  GPSbuffer[i+3]='.';
  memcpy(Longitude,&GPSbuffer[i],j-i-1);
  Longitude[j-i-1]='\0';
  Longitude_flag=GPSbuffer[j];
  //Altitude
  i=calchar(i,5);
  j=calchar(i,1);
  memcpy(Altitude,&GPSbuffer[i],j-i-1);
  Altitude[j-i-1]=0;
}
unsigned int calchar(unsigned int i,unsigned int count,char c)//从当前位置i开始遍历数count个字符c
{
  unsigned int cnt=0;
  while(1)
  {
    if(cnt==count||i>=n||GPSbuffer[i]=='\0') break;
    if(GPSbuffer[i]==c) cnt++;
    i++;
  }
  return i;
}
