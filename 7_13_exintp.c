#include <reg52.h>
#include <intrins.h>
#include <string.h>
#define uchar unsigned char 
#define uint  unsigned int

int flag=0;                         //外部中断标志
int z;
int timel;                          
int timeh;
int beepflag;                       //蜂鸣器中断
int n;
int bound;                          //蜂鸣器中断次数
long distance;	                    

uchar dis_smg[4]   ={0,1,2,3};      //数码管位选

uchar code smg_duan[]={0x14,0x77,0x4c,0x45,0x27,0x85,0x84,0x57,0x04,0x05};	//数码管段选

sbit led11 = P2^0;                  //工作指示灯
sbit beep = P2^1;                   //蜂鸣器
sbit smg_1 = P2^2;	                //数码管位选信号I/O口
sbit smg_2 = P2^3;
sbit smg_3 = P2^4;
sbit smg_4 = P2^5;
sbit led9 = P2^7;                   //LED发光条后两位
sbit led10 = P2^6;                  
sbit c_send   = P3^2;		            //超声波发射
sbit c_recive = P3^3;		            //超声波接收

/****************************1ms延时*******************************/
void delay3(unsigned int ms)                     
{    
  unsigned int a,b; 
  for(a=ms;a>0;a--)
  for(b=123;b>0;b--);
}

/****************************1us延时*******************************/

void delay2(int m)
{
	while(--m);
}

/***********************数码管位选函数*****************************/
void smg_we_switch(uchar i)
{
	switch(i)
	{
		case 3: smg_1 = 0;  smg_2 = 1; smg_3 = 1;  smg_4 = 1; break;
		case 2: smg_1 = 1;  smg_2 = 0; smg_3 = 1;  smg_4 = 1; break;
		case 1: smg_1 = 1;  smg_2 = 1; smg_3 = 0;  smg_4 = 1; break;
		case 0: smg_1 = 1;  smg_2 = 1; smg_3 = 1;  smg_4 = 0; break;
	}	
}

/***************************初始化*********************************/
void time_init()	  
{
	EA  = 1;	 	                
	TMOD = 0X11;	             
	ET0 = 1;		                
	TR0 = 0;		                
	EX1 = 0;
	ET1 = 1;		                
	TR1 = 0;		                 
  TF0 = 0;                     //清空溢出标志位	
}

/*******************超声波发送及测距程序***************************/
void send_wave()
{
	long temp = 888;
  TF0 = 0; 	 
	TH0 = 0;		             
	TL0 = 0;
	TR0 = 0;				           
	flag = 0;
	
	  led11=0;                        
	  TR0=1;	 
	
	  for(z=10;z>0;z--) 	 //发送5个40khz的方波（20）
		{c_send=!c_send;_nop_();_nop_();_nop_();}
		led11=1; 
		delay3(1) ;
		delay2(80);          //延时1.5ms，跳过直传信号，消除余震（80）
		
	  EX1 = 1 ;            //开外部中断
	  delay3(5) ;
		
    TR0=0;			         //若没进入中断还需关定时器和中断
		EX1=0;	
		
	if(flag==1)            //进入过外部中断
	{
		 TH1=65535;          //蜂鸣器定时
     TL1=65535;
	   TR1=1;
	   ET1=1;
		
		flag=0;
		temp=timeh*256+timel;
		distance=temp/59;
	}
	else
	{
		 flag=0;
		 distance=888;       //溢出
	}
}

/*******************LED发光条子程序***************************/
void LED(n)
{
switch(n)
    {
	  case 0:P1=0xff;led9=1;led10=1;break;
	  case 1:P1=0xff;led9=1;led10=0;break;
		case 2:P1=0xff;led9=0;led10=0;break;
		case 3:P1=0x7f;led9=0;led10=0;break;
		case 4:P1=0x3f;led9=0;led10=0;break;
		case 5:P1=0x1f;led9=0;led10=0;break;
		case 6:P1=0x0f;led9=0;led10=0;break;
		case 7:P1=0x07;led9=0;led10=0;break;
	  case 8:P1=0x03;led9=0;led10=0;break;
		case 9:P1=0x01;led9=0;led10=0;break;
		case 10:P1=0x00;led9=0;led10=0;break;
		}
}
	
/************************数码管*******************************/
void LEDsegment()        
{  
	dis_smg[0] = smg_duan[distance % 10];				
	smg_we_switch(0);
	P0 = dis_smg[0];	
	delay3(3);
	
	dis_smg[1] = smg_duan[distance / 10 % 10];			
	smg_we_switch(1);		
	P0 = dis_smg[1];		 
	delay3(3);
	
	dis_smg[2] = smg_duan[distance / 100 % 10];	 
	smg_we_switch(2);
	P0 = dis_smg[2];
	delay3(3);	
}

/************************主程序*******************************/
void main()
{
    time_init();			              
    P0=0xff;
    P1=0xff;
	  led9=1;
    led10=1;
    led11=1;
	  smg_1 = 0;	            
    smg_2 = 0;
    smg_3 = 0;
    smg_4 = 0;
    distance =95;
	while(1)
	{

		send_wave();	            //超声波发送及测距程序
		
		if (distance>=100)        //判断距离
					n=0;                //确定LED发光条亮灭状态
		else if (distance>90)
		{     n=1;
		      bound=25;           //确定蜂鸣器频率
		}//1100ms
		else if (distance>85)
		{			n=2;
		      bound=15;
		}//700ms
		else if (distance>80)
		{			n=3;
		      bound=9;
		}//420ms
		else if (distance>75)
		{		  n=4;
		      bound=7;
		}//330ms
		else if (distance>70)
		{			n=5;
		      bound=5;
		}//250ms
		else if (distance>65)
		{		  n=6;
		      bound=4;
		}//200ms
		else if (distance>60)
		{			n=7;
		      bound=3; 
		}//175ms
		else if (distance>55)
		{			n=8;
		      bound=2;
		}	//125ms
		else if (distance>50)
		{			n=9;
		      bound=1;
		}//85ms
		else
					n=10;
		
		
		LED(n);                 //LED发光条子程序
		
		LEDsegment();           //数码管显示子程序
		
    delay3(2) ;
		
	}
}

/*********************定时器0（测距）****************************/
void time0_int() interrupt 1  
{
 //TR0 = 0;
 //TF0 = 0;       			   
 //flag = 1; 
}		  

/*********************外部中断1（接收）***************************/
void ext() interrupt 2
{
		  TR0=0;
			timeh=TH0;
			timel=TL0;
			TH0=0;					    
			TL0=0;
			flag=1;						
			EX1=0;						
} 

/***********************定时器1（蜂鸣器）***************************/
void time1_int() interrupt 3
{	
	if(n==0)
   {
		 beep=1;              //蜂鸣器不响
	 }
	else if(n==10) 
	 {
		 beep=0;              //蜂鸣器长鸣
	 }
	else
	 {
	ET1=0;
	TR1=0;
	beepflag++;              //定时器计满1次标志位加1
	if(beepflag>bound)       //大于蜂鸣器标志位则电平翻转
	{
		beepflag=0;
	  TH1=65535;             //14ms
    TL1=65535;
	  beep=!beep;
	}
}
}
