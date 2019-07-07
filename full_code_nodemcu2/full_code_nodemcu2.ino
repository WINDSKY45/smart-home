#include "ESP8266WiFi.h" 

#define E1 4 //control 3 -- D6
#define M1 5 //control 4 -- D5
//此外还需要接地 GND -- GND
const char *RemoteIp = "192.168.137.229"; // 设置远程主机Ip地址
const int RemotePort = 8266; // 设置远程主机端口号
uint16_t speed;
  
WiFiClient client; // 创建Tcp客户端

void M1_advance(uint16_t Speed) ///<Motor1 Advance
{
  digitalWrite(M1,LOW);
  analogWrite(E1,Speed);
}

void M1_back(uint16_t Speed) ///<Motor1 Back off
{
  digitalWrite(M1,HIGH);
  analogWrite(E1,Speed);
}

void motor_control(int mes){
  switch(mes){ 
    case 10:
      analogWrite(E1,0);
      speed = 0;
      break;
    case 11:
      Serial.println(speed);
      if(speed == 0){
        analogWrite(E1,700);
        speed = 700;
      }
      break;
    case 12:
      if(speed + 125 <= 1100){
        if(speed + 125 <= 700){
          speed = 700;
          analogWrite(E1,speed);
        }
        else{
          speed += 125;
          analogWrite(E1,speed);}   
      }
      break;
    case 13:
      if(speed - 125 >= 700){
         speed -= 125;
         analogWrite(E1,speed);
      }
      break;
    default:break;
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println();

  WiFi.begin("HiIoT", "19960913");  // 设置连接到的WiFi名称和密码
  Serial.print("Connecting...");
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    //Serial.print(".");
  }
  
  //Serial.println();
  Serial.println(WiFi.localIP()); // 打印NodeMCU的IP地址
    
  while (!client.connect(RemoteIp, RemotePort)) // 建立tcp连接
  {
    Serial.println("Connected failed!");
    //return;
    delay(100);
  }
  Serial.println("Connected success!");
  pinMode(E1,OUTPUT);
  pinMode(M1,OUTPUT);
  digitalWrite(M1,HIGH);
  analogWrite(E1,0);
  speed = 0;
}

void loop(){
  //Serial.println(client.status);
  if (client.available())  
  {    
    Serial.println("start read");
    int mes = client.read(); // 读取远程网络串口发来的数据
    Serial.println(mes);
    motor_control(mes);
  }
}
