/*                  
  教师/工作室 灯光控制系统--------- 灯光面板部分
    按钮短按全部控制 长按单独控制
   sg90         *2
   esp8266 d1   *1

   按钮
   gnd_____/ ______D3

   sg90左
   红线------------5v
   黄线------------D1
   棕线------------gnd


   sg90右
    红线------------5v
    黄线------------D2 
    棕线------------gnd

*/
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>
ESP8266WebServer server(80);

String host = "192.168.32.11";//从机地址（ 如果是从机这里填主机地址 没有从机请忽略）
/*       必须进行舵机角度设置sg90角度误差太大 两边不平发热严重                                 */
int zdj =90;  //左舵机初始角度
int ydj =90;  //右舵机初始角度
int onz=30;   //左舵机开灯角度
int ony=130;  //右舵机开灯角度
int offz=120; //左舵机关灯角度
int offy=40;  //右舵机关灯角度








Servo myservo;
Servo myservos;
boolean dg=false;
boolean quanti=false;
boolean duzi=false;
String light_get;  
#define btn D3 
void zd()  // 
{  
  int millis_time=millis();
  int opera=0;
  while(digitalRead(btn)==LOW){
    if(millis()-millis_time>2000){
      opera=2;
      break;
    }else if (millis()-millis_time>1){
      opera=1;
    }
  }
  if(opera==1){
  //全 
  quanti=true;
  Serial.println("全体");
   //
  }else if(opera==2){
  //单个
   duzi=true;
  Serial.println("独自");

  }
}




void setup() {
  pinMode( btn, INPUT_PULLUP);
  attachInterrupt( digitalPinToInterrupt(btn), zd, CHANGE); 
  Serial.begin(115200);
  SPIFFS.begin();
 WiFi.begin("test", "11111111");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println(WiFi.localIP());
  WiFi.hostname("deng_1");
  server.begin();
  server.on("/",abc);
  server.on("/opera", opera);
  myservo.attach(D1);
  myservos.attach(D2);
  myservo.write(zdj); 
  myservos.write(ydj); 
  dg=0;
  zc();

  
}


void abc(){
  File file = SPIFFS.open("/index.html", "r");
server.streamFile(file, "text/html");
  file.close();
}

void opera() {
  String parm = server.arg("opera");
   Serial.println("收到GET报文:");
   Serial.println(parm);
   if(parm=="0"){
           dg=0;
           zc();
              delay(100);
          }else if(parm=="1"){
           dg=1;
           zc();
              delay(100);
          }else{
        quanti=true;
        Serial.println("全体操作");
        }
   
   
   
   
   

   
 
server.send(200, "text/plain", "ojbk");
}

void loop() {
  server.handleClient();
 if( quanti){
     dg=!dg;
    quanti=false;
     zc();

      HTTPClient http;
      String url = "http://"+ host+"/opera?opera="+String(dg);
      http.begin(url);
      int httpCode = http.GET();
      String payload = http.getString();
      Serial.println(payload);

  

  }
 if( duzi){
    dg=!dg;
     zc();
    duzi=false;
    }
}


void zc(){
  if(dg){
    
      myservo.write(onz);  
      myservos.write(ony);  
       
       Serial.println("开灯");
    
    }else{
         myservo.write(offz);  
      myservos.write(offy); 
    
      
      Serial.println("关灯");
      }
   delay(2000);
    myservo.write(zdj);  
    myservos.write(ydj);  
     Serial.println("舵机归位");
 
  }
