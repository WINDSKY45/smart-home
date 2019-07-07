#include <Ticker.h> //定时任务调度器，可以设置以固定时间间隔执行某个任务，在单片机中类似操作系统的调度器，用于多任务协调
//Ticker timer1;  //任务调度1
Ticker timer2;  //任务调度2

#include <ESP8266WiFi.h>
#define MAX_SRV_CLIENTS 2    //定义可连接的客户端数目最大值 不能超过2个

WiFiServer server(23);//服务器端口23
WiFiClient serverClients[MAX_SRV_CLIENTS];

int delay_time = 3000;//可控硅的延时触发数值，据说范围是0~10000
String raw_data = ""; //接收到客户端的数据
String raw_data_front_segment = "";//接收到客户端的数据，只要前4位
/* 
int an1=0; //记录按钮1状态 松开时候是0  判断是否被按过，被按过就不继续执行
int an2=0; //记录按钮2状态 松开时候是0  判断是否被按过，被按过就不继续执行
int an3=0; //记录按钮3状态 松开时候是0  判断是否被按过，被按过就不继续执行
int kg=0;  //开关状态
*/
int dd = 3000;//用于停止后恢复原来的状态
/* void anniu1();//按钮1处理事件
void anniu2();//按钮2处理事件
void anniu3();//按钮3处理事件*/
void setup()
{
  Serial.begin(115200);
  
  pinMode(2, OUTPUT);//定义led的2脚为输出模式 
  pinMode(14, INPUT);//定义14脚是输入按键1
  pinMode(12, INPUT);//定义12脚是输入按键2
  pinMode(13, INPUT);//定义13脚是输入按键3
  digitalWrite(4, HIGH);//输出高电位可控硅截止，也不是8266的LED
  digitalWrite(2, HIGH);//引脚2是8266上的LED

  //timer1.attach_ms(1, key);   //key作为任务调度1，每隔1ms执行一次
  timer2.attach_ms(1, chuli); //chuli作为任务调度2，每隔1ms执行一次

  WiFi.mode(WIFI_AP);         //设置模式为AP
  IPAddress softLocal(192,168,168,110);
  IPAddress softGateway(192,168,168,110);
  IPAddress softSubnet(255,255,255,0);
  WiFi.softAPConfig(softLocal, softGateway, softSubnet);//配置ap的网络参数
  WiFi.softAP("myap", "likelike"); //启用软ap
  Serial.print(WiFi.softAPIP());   //获得apIP    
  Serial.println(" 23' to connect");

  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
  //if(i == 21){ //超时（20x500=10000,10秒钟），提示连接失败
  //}连接失败好像没救

  //启动UART传输和服务器
  server.begin();
  server.setNoDelay(true);
  Serial.print("localip:");
  Serial.println(WiFi.localIP());//获得本地IP

  pinMode(4, OUTPUT);//定义pwm控制的4脚为输出模式
  pinMode(5, INPUT); //定义5脚是输入，作为过零同步检查
}
/* 
void key()
{
anniu1();
anniu2();
anniu3(); 
}
*/
void chuli()
{
  uint8_t i;
  //检测服务器端是否有活动的客户端连接
  if (server.hasClient()){
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      //查找空闲或者断开连接的客户端，并置为可用
      if (!serverClients[i] || !serverClients[i].connected()){
        if(serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        Serial.print("New client: "); Serial.println(i);
        continue;
      }
    }
    //若没有可用客户端，则停止连接
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }
  //检查客户端的数据
  for(i = 0; i < MAX_SRV_CLIENTS; i++){
    if (serverClients[i] && serverClients[i].connected()){
      if(serverClients[i].available()){
        //从Telnet客户端获取数据，并推送到串口
        while(serverClients[i].available())
        {
         raw_data += char(serverClients[i].read());
         //delay(2);
         }
      
        if (raw_data.length() > 0)
        {
        raw_data_front_segment = raw_data.substring(0,4);//只取前四个，默认只有前四个有用？
        delay_time = raw_data_front_segment.toInt();//在此把comdata转化成INT型数值,以备后续使用
        Serial.println(delay_time);//输出获得的延迟时间长度  
        raw_data = "";             //必须在此把comdata设为空字符,否则会导致前后字符串叠加
        }
      }
    }
  }
  //检查串口发来的数据
  if(Serial.available()){
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    //将串口数据推送到所有已连接的telnet客户端，实现双向通信
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      if (serverClients[i] && serverClients[i].connected()){
        serverClients[i].write(sbuf, len);
        // delay(1);
      }
    }
  }
}

void loop(){
//  if(! digitalRead(5))
  {
//    if(delay_time!=9500)
    {//不知道这个9500是什么含义
//      digitalWrite(4, HIGH);//输出高电位可控硅截止
//      digitalWrite(2, HIGH); 
//      delayMicroseconds(delay_time);//延时触发导通
      digitalWrite(4, LOW);//输出低电位可控硅导通
      digitalWrite(2, LOW); 

    }
//    else{
//    digitalWrite(4, HIGH);//输出高电位可控硅截止
//    digitalWrite(2, HIGH);
//    }
  }
}
/* 
void anniu1()
{
 if(! digitalRead(14))//读io14状态是否低电平
{
timer1.attach_ms(1, key);
if(an1==0)//如果按钮1没被按过
{
  if(delay_time<4000)
{
  delay_time=0;
}
  if (delay_time >= 1000  && delay_time!=9500)//延时数大于1000
{
 
delay_time=delay_time-1000;

  an1=1;//更改状态按键1按过了
}
}
}
else
{
timer1.attach_ms(200, key);
an1=0;//更改状态按键1没有被按过
}   
}
void anniu2()
{
 if(! digitalRead(12))//读io12状态是否低电平
{
  timer1.attach_ms(1, key);
if(an2==0)//如果按钮2没被按过
{
  if (kg == 0)//开关也没打开
{
delay_time=dd;
  kg =1; //开关打开了
  an2=1;//更改状态按键2按过了
 
}
else if (kg == 1)//开关打开了
{
  dd=delay_time;
delay_time=9500;//这个数值会停止
      kg =0;//更改状态开关关闭了
      an2=1;//更改状态按键2按过了
}  
}
}
else
{
timer1.attach_ms(200, key);
an2=0;//更改状态按键2没有按过

}
}

void anniu3()
{
 if(! digitalRead(13))//读io13状态是否低电平
{
timer1.attach_ms(1, key);
if(an3==0)//如果按钮3没被按过
{
  if(delay_time<4000)
{
  delay_time=4000;
}
  if (delay_time <= 8000 )//延时数小于8000
{
 
delay_time=delay_time+1000;

  an3=1;//更改状态按键3按过了
}
}
}
else
{
timer1.attach_ms(200, key);
an3=0;//更改状态按键1没有被按过

}   
}
*/
