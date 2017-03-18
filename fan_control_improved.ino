/*
   This code is to control the fan to tune the speed and turn on.

   当风扇停止时，触动一次开关会顺次打开3档-停五秒-2档-停五秒-一档。
   如果在五秒停顿时间内再次触发，将会停在当前档位。
   如果风扇在某一档运行，触发开关会降一档。
 
   LED颜色：蓝色-2档；绿色-3档；红色-1档

   成功  2014/5/8
   */


#include <avr/wdt.h>  //看门狗头文件 

boolean on = LOW;  //both relay and LED low is on
boolean off = HIGH;
int speed = 0;
int active=0;        //红外线有触发=1，无触发=0
int i=0;
int a;
int b;
int c;


# define brown_line  3
# define purple_line 4
# define gray_line   5

# define led_red     6
# define led_green   7
# define led_blue    8

# define body_sensor 2
# define trigger     12
# define buzzer      9



void setup() 
 {

  Serial.begin(9600);

  wdt_enable(WDTO_8S);  //定义看门狗8秒超时。

  pinMode(brown_line, OUTPUT);
  pinMode(gray_line, OUTPUT);
  pinMode(purple_line, OUTPUT);
  pinMode(led_blue, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_red, OUTPUT);

  pinMode(body_sensor, INPUT);
  pinMode(trigger, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(A0, INPUT);

  speed0();
  
 }

void loop()
{
   
   if (digitalRead(body_sensor)==HIGH)   //有人
   {     
    ir_senser();

        if(active==1)
          {speed_change();             //换挡 
           delay(1000);               //必要，防止连续换挡
          }                           
   }
   else                               //没有人
   {      if(speed>0)                 //运行中？
             {bibibibibi();}          //叫人

          delay(2000);                 //省电，不要总是运行；    

          if (digitalRead(body_sensor)==LOW)   //没人理
            {    speed0();                            //关机
                 wdt_reset();                         //喂狗
            }
   }                      

}
void blink_blue()
  {digitalWrite(led_blue, off);
   delay(50);
   digitalWrite(led_blue,on);
   delay(50);
   Serial.println("BLINK BLUE");
 }

void blink_green()
 {digitalWrite(led_green, off);
   delay(50);
   digitalWrite(led_green,on);
   delay(50);
   Serial.println("BLINK GREEN");
 }

void blink_red()
 {digitalWrite(led_red, on);
   delay(50);
   digitalWrite(led_red,off);
   delay(50);
   Serial.println("BLINK RED");
 }

void bi()
 {
  digitalWrite(buzzer, HIGH);
  delay(20);
  digitalWrite(buzzer, LOW);
 }

void biiiii()
  {
  digitalWrite(buzzer, HIGH);
  delay(2000);
  digitalWrite(buzzer, LOW);
  wdt_reset();                         //喂狗
 }

void bibibibibi()
 {
  for(int j=1; j<200; j++)
  {
  digitalWrite(buzzer, HIGH);
  delay(300-1.5*j);
  digitalWrite(buzzer, LOW);
  delay(5000/j);
  wdt_reset();                         //喂狗
  }
 }

void speed1()  
 {   
   delay (20);
     digitalWrite(brown_line, off);   //slow fan speed, only purple line on
     digitalWrite(gray_line, off);
     digitalWrite(purple_line, on); 
     digitalWrite(led_blue, off);
     
     digitalWrite(led_red, on);
     speed =1;
     Serial.println("SPEED 1");

  }

void speed2()
   {   
     delay (20);
     digitalWrite(gray_line, off);  //off fast fan
     delay(500);                    //给继电器动作时间
     digitalWrite(led_green, off);

     digitalWrite(brown_line, on); //middle fan speed
     digitalWrite(purple_line, on); 
     digitalWrite(led_blue, on);
     
     speed =2;
     Serial.println("SPEED 2");

   }   

void speed3()
   {      
     delay (20);
     digitalWrite(brown_line, off);
     digitalWrite(purple_line, off);
     digitalWrite(gray_line, on);  //fast fan speed

     digitalWrite(led_green, on);
     speed=3;
     Serial.println("SPEED 3");

   }

void speed0()
   {    
     delay (20);
     digitalWrite(gray_line, off);
     digitalWrite(brown_line, off);
     digitalWrite(purple_line, off);
     
     digitalWrite(led_red, off);
     digitalWrite(led_blue, off);
     digitalWrite(led_green, off);
     speed=0;
     Serial.println("OFF");

   }

void normal_on()
 {    
    delay (20);
    switch(speed)
    {
      case 0:
           speed3();
           delay(1000);                   //风扇加速，等待保持信号
           ir_senser();                    //读取红外线状态
           if(active==1)                    //有触发
           {
                 blink_green();             //闪绿灯
                 bi();
                 active=0;
                 break;                     //跳出，保持，不再降档
            }                     
            delay(4000);  
           wdt_reset();                       //喂狗    
           
      case 3:
           speed2();
           delay(1000);                   //风扇加速，等待保持信号
           ir_senser();                    //读取红外线状态
           if(active==1)                    //有触发
           {                                
                 blink_blue();             //闪蓝灯
                 bi();
                 active=0;
                 break;                    //跳出，保持，不再降档
            }                     
           delay(4000);
           wdt_reset();                       //喂狗      
           

      case 2:
           speed1();
      }
         
 }  

void speed_change()
 {   bi();
    active=0;
    switch(speed)
      {
          case 0:
            normal_on();
            break;
          case 1:
            speed0();
            break;
          case 2:
            speed1();
            break;
          case 3:
            speed2();
            break;
      }
        
 }

void ir_senser()
   {   wdt_reset();   //喂狗    
       
           for(int i=0; i<6; i++)
       {
 
  
         digitalWrite(trigger, HIGH);
         delay(4);
         int a = analogRead(A0); //读出LED亮时ADC端口的值
  
         digitalWrite(trigger, LOW);
         delay(4);
         int b = analogRead(A0); //读出LED 灭时ADC端口的值
  
         c=b-a; //取2次检测值之差，避开环境光干扰
            if(c>15)
            {
              Serial.print("i="); 
              Serial.println(i);
 
              if(i == 5)      //连续5 次检测，以避开干扰
               {
                 active=1;
                 
                // Serial.print(c);
                 //Serial.print("  ");
                 //Serial.print(b);
                 //Serial.print("  ");
                 //Serial.println(a);    
               }   
            }
            else {i=6;}
           }

   }
