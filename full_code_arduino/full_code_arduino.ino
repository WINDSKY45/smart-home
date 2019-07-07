#include <Voice.h>
#include <SoftwareSerial.h>
#include "Syn6288.h"
//过一段不发指令重新开始
//增加控制灯的程序
#define SUM 7 //识别关键词的个数
#define LED 6 //表示聆听的led位置
#define TX 7
#define RX 8

Syn6288 syn;
SoftwareSerial ArduinoUno(TX, RX);
uint8 nAsrStatus = 0;
char sRecog[SUM][80] = {"tian mao jing ling","kai feng shan", "guan feng shan", "feng shan kuai dian","feng shan man dian","kai deng", "guan deng"};
uint8_t flag;
uint8_t ans_1[] = {0xCE,0xD2,0xD4,0xDA};//我在
uint8_t ans_2[] = {0xBA,0xC3,0xB5,0xC4};//好的
uint8_t ans_3[] = {0xCE,0xD2,0xB2,0xBB,0xCC,0xAB,0xC3,0xF7,0xB0,0xD7};//我不太明白

void finally(unsigned char n)
{
  switch(n){
    case 0://唤醒词
      syn.play(ans_1, sizeof(ans_1),16); 
      flag = 1;
      break;
    case 1://开风扇
      if(flag == 1){
        Serial.println("switch on");
        ArduinoUno.print(11);
        ArduinoUno.println("\n");
        //可能增加反馈
        syn.play(ans_2, sizeof(ans_2),16);
        flag = 0;
      }
      break;
    case 2://关风扇
    if(flag ==1){
      Serial.println("switch off");
      ArduinoUno.print(10);
      ArduinoUno.println("\n");
      syn.play(ans_2, sizeof(ans_2),16);
      flag = 0;
    }
      break;
    case 3://风扇快点
      if(flag ==1){
        ArduinoUno.print(12);
        ArduinoUno.println("\n");
        syn.play(ans_2, sizeof(ans_2),16);
        flag = 0;
      }
      break;
     case 4://风扇慢点
       if(flag == 1){
         ArduinoUno.print(13);
         ArduinoUno.println("\n");
         syn.play(ans_2, sizeof(ans_2),16);
         flag = 0; 
        }
      break;
      case 5://开灯
      break;
      case 6://关灯
      break;
    default:
      if(flag == 1){
        syn.play(ans_3, sizeof(ans_3),16);
        flag = 0;
      }
      break;
  }
  Serial.print("case:");
  Serial.println(n);
  delay(50);
}

void ExtInt0Handler()
{
  digitalWrite(LED, HIGH);
  Voice.ProcessInt0();//LD3320送出中断信号
}

void setup() {
    Serial.begin(9600);
    ArduinoUno.begin(4800);
    flag = 0;//正确是0，这里调试用
    syn.volume(15);//设置音量大小，范围0~16,0为静音
    
    Voice.Initialise(MIC, VoiceRecognitionV1);//初始化VoiceRecognition模块 
    attachInterrupt(0,ExtInt0Handler, LOW);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
}

void loop() {
  uint8_t nAsrRes;
  nAsrStatus = LD_ASR_NONE;
  while(1)
  {
    /*
    if(ArduinoUno.available()>0){//NodeMCU回执，待实现
      float val = ArduinoUno.parseFloat();
      if(ArduinoUno.read()== '\n'){
        Serial.println(val);
      }
    }*/
    //syn.play(ans_1, sizeof(ans_1),16);
    switch(nAsrStatus)
  {
    case LD_ASR_RUNING:
    case LD_ASR_ERROR:
      break;
    case LD_ASR_NONE:
    {
      nAsrStatus = LD_ASR_RUNING;
      if(Voice.RunASR(SUM,80,sRecog)==0)
      {
        nAsrStatus = LD_ASR_ERROR;
        Serial.println("ASR_ERROR");
        break;
      }
      digitalWrite(LED, LOW);
      break;
    }
    case LD_ASR_FOUNDOK:
    {
      nAsrRes = Voice.LD_GetResult();//一次识别结束，去取识别结果
      finally(nAsrRes);
      nAsrStatus = LD_ASR_NONE;
      break;
    }
    case LD_ASR_FOUNDZERO:
    default:{
      nAsrStatus = LD_ASR_NONE;
      break;
    }
  }
  delay(100);
  }
}
