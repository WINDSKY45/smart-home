#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
//tx --- rx,rx --- tx
SoftwareSerial NodeMCU(4,5);//D2, D1
const int LocalPort = 8266;

WiFiServer server(LocalPort);
void setup(){
  Serial.begin(9600);
  WiFi.begin("HiIoT", "19960913");  // 设置连接到的WiFi名称和密码
  Serial.println("Connecting...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
  //Serial.println();
  Serial.println(WiFi.localIP()); // 打印NodeMCU的IP地址
  server.begin();  
  NodeMCU.begin(4800);
  pinMode(4,INPUT);
  pinMode(5,OUTPUT);
}

void loop(){
  //Serial.println("OKk");
  /*
  if(NodeMCU.available()>0){
        Serial.println("NodeMCU OK");
        int val = NodeMCU.parseInt();
        if(NodeMCU.read()== '\n'){
          Serial.println(val);
        }
    }*/
  WiFiClient client = server.available();//等待客户端连接
  Serial.println("OK");
  if (client){
   Serial.println("client ok");
    while(client.connected()){
      Serial.println("connect OK");
      if(NodeMCU.available()>0){
        Serial.println("NodeMCU OK");
        int val = NodeMCU.parseInt();
        if(NodeMCU.read()== '\n'){
          Serial.println(client.write(val));
          Serial.println(val);
        }
      }
      delay(100);
  /*接收控制器信息，发给arduino
  if(client.available()){
    int mes = client.read();
  }*/
    //client.write(11);
    }
  }
}
