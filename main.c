#include "reg52.h"			
#include"temp.h"	

typedef unsigned int u16;	  
typedef unsigned char u8;

sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;


char num=0;
u8 DisplayData[8];
u8 code smgduan[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
u8 tem[4];

void delay(u16 i)
{
	while(i--);	
}




void datapros(int temp) 	 
{
   	float tp;  
	if(temp< 0)				
  	{
		DisplayData[0] = 0x40; 	  
		temp=temp-1;
		temp=~temp;
		tp=temp;
		temp=tp*0.0625*100+0.5;	

  	}
 	else
  	{			
		DisplayData[0] = 0x00;
		tp=temp;
	
		temp=tp*0.0625*100+0.5;	
		
	}
	DisplayData[1] = smgduan[temp / 10000];                         //0
	DisplayData[2] = smgduan[temp % 10000 / 1000];                 //2
	DisplayData[3] = smgduan[temp % 1000 / 100] | 0x80;             //9.
	DisplayData[4] = smgduan[temp % 100 / 10];                      //9
	DisplayData[5] = smgduan[temp % 10];                           //4
	tem[0]=temp % 10000 / 1000+48;  //2
	tem[1]=temp % 1000 / 100+48;    //9.
	tem[2]=temp % 100 / 10+48;      //9
 	tem[3]=temp % 10+48;            //4
}



void DigDisplay()
{
	u8 i;
	for(i=0;i<6;i++)
	{
		switch(i)	
		{
			case(0):
				LSA=0;LSB=0;LSC=0; break;
			case(1):
				LSA=1;LSB=0;LSC=0; break;
			case(2):
				LSA=0;LSB=1;LSC=0; break;
			case(3):
				LSA=1;LSB=1;LSC=0; break;
			case(4):
				LSA=0;LSB=0;LSC=1; break;
			case(5):
				LSA=1;LSB=0;LSC=1; break;
		}
		P0=DisplayData[5-i];
		delay(100); 
		P0=0x00;
	}		
}

void UsartInit()
{
	SCON=0X50;			
	TMOD=0X20;			
	PCON=0X80;			
	TH1=0XF3;				
	TL1=0XF3;
	ES=1;						
	EA=1;						
	TR1=1;					
}


void main()
{	
	UsartInit(); 
	while(1)
	{
		datapros(Ds18b20ReadTemp());
		DigDisplay();
	}		
}

void Usart() interrupt 4
{
	u8 receiveData;
	int i;
	RI = 0;
	receiveData=SBUF;
	i=receiveData;
	switch(i)
	{
		case 0:receiveData=tem[0]; break;
		case 1:receiveData=tem[1]; break;
		case 2:receiveData='.'; break;
		case 3:receiveData=tem[2]; break;
		case 4:receiveData=tem[3]; break;
	}

	SBUF=receiveData;
	while(!TI);			 
	TI=0;						
}
