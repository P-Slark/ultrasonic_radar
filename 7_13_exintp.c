#include <reg52.h>
#include <intrins.h>
#include <string.h>
#define uchar unsigned char 
#define uint  unsigned int

int flag=0;                         //�ⲿ�жϱ�־
int z;
int timel;                          
int timeh;
int beepflag;                       //�������ж�
int n;
int bound;                          //�������жϴ���
long distance;	                    

uchar dis_smg[4]   ={0,1,2,3};      //�����λѡ

uchar code smg_duan[]={0x14,0x77,0x4c,0x45,0x27,0x85,0x84,0x57,0x04,0x05};	//����ܶ�ѡ

sbit led11 = P2^0;                  //����ָʾ��
sbit beep = P2^1;                   //������
sbit smg_1 = P2^2;	                //�����λѡ�ź�I/O��
sbit smg_2 = P2^3;
sbit smg_3 = P2^4;
sbit smg_4 = P2^5;
sbit led9 = P2^7;                   //LED����������λ
sbit led10 = P2^6;                  
sbit c_send   = P3^2;		            //����������
sbit c_recive = P3^3;		            //����������

/****************************1ms��ʱ*******************************/
void delay3(unsigned int ms)                     
{    
  unsigned int a,b; 
  for(a=ms;a>0;a--)
  for(b=123;b>0;b--);
}

/****************************1us��ʱ*******************************/

void delay2(int m)
{
	while(--m);
}

/***********************�����λѡ����*****************************/
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

/***************************��ʼ��*********************************/
void time_init()	  
{
	EA  = 1;	 	                
	TMOD = 0X11;	             
	ET0 = 1;		                
	TR0 = 0;		                
	EX1 = 0;
	ET1 = 1;		                
	TR1 = 0;		                 
  TF0 = 0;                     //��������־λ	
}

/*******************���������ͼ�������***************************/
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
	
	  for(z=10;z>0;z--) 	 //����5��40khz�ķ�����20��
		{c_send=!c_send;_nop_();_nop_();_nop_();}
		led11=1; 
		delay3(1) ;
		delay2(80);          //��ʱ1.5ms������ֱ���źţ���������80��
		
	  EX1 = 1 ;            //���ⲿ�ж�
	  delay3(5) ;
		
    TR0=0;			         //��û�����жϻ���ض�ʱ�����ж�
		EX1=0;	
		
	if(flag==1)            //������ⲿ�ж�
	{
		 TH1=65535;          //��������ʱ
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
		 distance=888;       //���
	}
}

/*******************LED�������ӳ���***************************/
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
	
/************************�����*******************************/
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

/************************������*******************************/
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

		send_wave();	            //���������ͼ�������
		
		if (distance>=100)        //�жϾ���
					n=0;                //ȷ��LED����������״̬
		else if (distance>90)
		{     n=1;
		      bound=25;           //ȷ��������Ƶ��
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
		
		
		LED(n);                 //LED�������ӳ���
		
		LEDsegment();           //�������ʾ�ӳ���
		
    delay3(2) ;
		
	}
}

/*********************��ʱ��0����ࣩ****************************/
void time0_int() interrupt 1  
{
 //TR0 = 0;
 //TF0 = 0;       			   
 //flag = 1; 
}		  

/*********************�ⲿ�ж�1�����գ�***************************/
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

/***********************��ʱ��1����������***************************/
void time1_int() interrupt 3
{	
	if(n==0)
   {
		 beep=1;              //����������
	 }
	else if(n==10) 
	 {
		 beep=0;              //����������
	 }
	else
	 {
	ET1=0;
	TR1=0;
	beepflag++;              //��ʱ������1�α�־λ��1
	if(beepflag>bound)       //���ڷ�������־λ���ƽ��ת
	{
		beepflag=0;
	  TH1=65535;             //14ms
    TL1=65535;
	  beep=!beep;
	}
}
}
