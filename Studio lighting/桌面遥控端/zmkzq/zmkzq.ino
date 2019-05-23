/*                  
  教师/工作室 灯光控制系统--------- 桌面遥控端
   data需要上传文件系统
   RCWL-0516    *1
   esp8266 d1   *1

   按钮
   gnd_____/ ______D3

   0516 (注意0516摆放位置)
   vin------------5v
   out------------D5
   gnd------------gnd

*/
#include <Arduino.h>
#include <Time.h>
#include <TimeLib.h> 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <FS.h>
ESP8266WebServer server(80);
#define btn D3 
boolean qiyong=false;      //是否开启人走关灯人来开灯 （可在web设置 注 没有用到eerom断电不会保存设置） 
const long interval = 10000; //人走关灯判断时间(二倍关系 例设定10000毫秒 在20000毫秒内无人将会关灯)
int shijian=0;              //人来开灯启用时间 （24小时格式    例如 20表示 下午8点到次日0点有效  0全天有效 ）
String zhudeng="192.168.32.10"; // 主灯局域网地址
String congdeng="192.168.32.11";// 从灯局域网地址（只一个灯可忽略）

IPAddress timeServer(120, 25, 115, 19); // 阿里云ntp服务器 如果失效可以使用 120.25.115.19   120.25.115.20
#define STD_TIMEZONE_OFFSET +8 //设置中国
const int timeZone = 8;     // 修改北京时区
WiFiUDP Udp;
unsigned int localPort = 8888;  // 修改udp 有些路由器端口冲突时修改
int servoLift = 1500;
boolean kz=false;
boolean zdp=false;
unsigned long previousMillis = 0;
const long interval2 = 5000;
unsigned long previousMillis2 = 0;
int oneg=1;
int onek=0;
boolean ren=false;
boolean er=false;
int p = 1;

void zdb()  // 中断按钮 
{  
  int millis_time=millis();
  int opera=0;
  while(digitalRead(btn)==LOW){
    if(millis()-millis_time>2000){
      opera=2;
      break;
    }else if (millis()-millis_time>20){
      opera=1;
    }
  }
  if(opera==1){
    zdp=true;
     
  Serial.println("发送请求");
   
  }else if(opera==2){
  
  Serial.println("无效操作");
  }
}
void setup() {
  Serial.begin(115200);
pinMode( btn, INPUT_PULLUP);

 pinMode(D1, INPUT);

Serial.println("load.....");  
attachInterrupt( digitalPinToInterrupt(btn), zdb, CHANGE); 
  SPIFFS.begin();
  WiFi.mode(WIFI_STA);
 WiFi.begin("test", "11111111");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println(WiFi.localIP());
  Udp.begin(localPort);
    setSyncProvider(getNtpTime);
    Serial.print(hour());//输出当前网络小时
 Serial.println();
  server.begin();
  server.on("/",abc);
  server.on("/opera", opera);
}


void abc(){
  File file = SPIFFS.open("/index.html", "r");
server.streamFile(file, "text/html");
  file.close();
}

void opera() {
  String parm = server.arg("opera");
   if(parm == "qbk"){
    kz=true;
    qb();
  Serial.println("全部开灯");
  }else if(parm == "qbg"){
    kz=false;
    qb();
 Serial.println("全部关灯");
  }else if(parm == "z1"){
    kz=true;
    zd();
  Serial.println("前灯开");
  }else if(parm == "z0"){
    kz=false;
    zd();
  Serial.println("前灯关");
  }else if(parm == "c1"){
    kz=true;
    cd();
  Serial.println("后灯开");
  }else if(parm == "c0"){
    kz=false;
    cd();
  Serial.println("后灯关");
  }else if(parm == "qy"){
    qiyong=true;
  
  Serial.println("启用人来开灯");
  }else if(parm == "gb"){
    qiyong=false;
  
  Serial.println("关闭人来开灯");
  }
 
server.send(200, "text/plain", "ok");
}




void loop() {
  server.handleClient();
 
    if(zdp){ kz=!kz; qb(); zdp=false; }
    int s =digitalRead(D1);
     delay(100);
  // Serial.println(digitalRead(D1));
 
//人来开关灯判断
if(qiyong){

 

     if(onek==0&&(hour()>=shijian)){
     if(p==0){
       onek=1;
       oneg=0;
       kz=true;
       qb();
     Serial.println("人来开灯"); 
     }else  Serial.println("不满足开灯条件"); 

     }


  unsigned long  currentMillis = millis();

/*if(s==1&&onek==0&&(hour()>=shijian)){
     if(p==0){
       onek=1;
       oneg=0;
       kz=true;
       qb();
     Serial.println("人来开灯"); 
     }
    }*/

   if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
     setSyncProvider(getNtpTime);//更新时间
     
    if(s==0&&oneg==0&&(hour()>=shijian)){
        oneg=1;
        onek=0;
        Serial.println("检测到无人准备关灯"); 
         kz=false;
         qb();
         p=1;
         }else {
          Serial.println("不满足关灯条件"); 
          p=0;
         }
          s=0;
    
  }


}








  
}

  void qb (){
      HTTPClient http;
      String url = "http://" +zhudeng+ "/opera?opera="+String(kz);
      String ur2 = "http://"+congdeng+"/opera?opera="+String(kz);
      http.begin(url);
      int httpCode = http.GET();
      String payload = http.getString();
      Serial.println(payload);
      
      http.begin(ur2);
      httpCode = http.GET();
      payload = http.getString();
      Serial.println(payload);
  }

 void zd (){
      HTTPClient http;
      String url = "http://"+ zhudeng+"/opera?opera="+String(kz);
      http.begin(url);
      int httpCode = http.GET();
      String payload = http.getString();
      Serial.println(payload);
     
  }
 void cd (){
      HTTPClient http;
      String url = "http://" +congdeng + "/opera?opera="+String(kz);
      http.begin(url);
      int httpCode = http.GET();
      String payload = http.getString();
      Serial.println(payload);
     
  }
  void digitalClockDisplay(){
  //
  Serial.print(hour());
  printDigits(minute());
 Serial.println();

}
void printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("连接时间服务器");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("时间服务器应答");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;    
  packetBuffer[3] = 0xEC; 

  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;                
  Udp.beginPacket(address, 123);
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
