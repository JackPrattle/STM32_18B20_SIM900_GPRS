#include "ds18b20.h"

#include "delay.h" 

#include "math.h"

//复位DS18B20

u8 tempbuf[8]={0};

void DS18B20_Rst(void)        

{                

    DS18B20_IO_OUT();   //SET PA0 OUTPUT					//wl

    DS18B20_DQ_OUT=0; //拉低DQ								//wl

    delay_us(750);                  //拉低750us				 //wl

    DS18B20_DQ_OUT=1; //DQ=1							 //wl

       delay_us(15);                   //15US			   //wl

}

//等待DS18B20的回应

//返回1:未检测到DS18B20的存在

//返回0:存在

u8 DS18B20_Check(void)       

{  

       u8 retry=0;

       DS18B20_IO_IN();//SET PA0 INPUT      //wl

    while (DS18B20_DQ_IN&&retry<200)

       {

              retry++;

              delay_us(1);

       };   

       if(retry>=200)return 1;

       else retry=0;

    while (!DS18B20_DQ_IN&&retry<240)

       {

              retry++;

              delay_us(1);

       };

       if(retry>=240)return 1;     

       return 0;

}

//从DS18B20读取一个位

//返回值：1/0

u8 DS18B20_Read_Bit(void)                      // read one bit

{

    u8 data;

       DS18B20_IO_OUT();//SET PA0 OUTPUT		//wl

    DS18B20_DQ_OUT=0;

       delay_us(2);

    DS18B20_DQ_OUT=1;

       DS18B20_IO_IN();//SET PA0 INPUT			//wl

       delay_us(12);

       if(DS18B20_DQ_IN)
	   
	   data=1;

    else data=0;   

    delay_us(50);          

    return data;

}

//从DS18B20读取一个字节

//返回值：读到的数据

u8 DS18B20_Read_Byte(void)    // read one byte

{       

    u8 i,j,dat;

    dat=0;

       for (i=1;i<=8;i++)

       {

        j=DS18B20_Read_Bit();

        dat=(j<<7)|(dat>>1);

    }                                           

    return dat;

}

//写一个字节到DS18B20

//dat：要写入的字节

void DS18B20_Write_Byte(u8 dat)    

 {            

    u8 j;

    u8 testb;

       DS18B20_IO_OUT();//SET PA0 OUTPUT; 	//wl

    for (j=1;j<=8;j++)

       {

        testb=dat&0x01;

        dat=dat>>1;

        if (testb)

        {

            DS18B20_DQ_OUT=0;// Write 1

            delay_us(2);                           

            DS18B20_DQ_OUT=1;

            delay_us(60);            

        }

        else

        {

            DS18B20_DQ_OUT=0;// Write 0

            delay_us(60);            

            DS18B20_DQ_OUT=1;

            delay_us(2);                         

        }

    }

}

//开始温度转换

void DS18B20_Start(void)// ds1820 start convert

{                                                            

    DS18B20_Rst();       

       DS18B20_Check();      

    DS18B20_Write_Byte(0xcc);// skip rom

    DS18B20_Write_Byte(0x44);// convert

}

//初始化DS18B20的IO口 DQ 同时检测DS的存在

//返回1:不存在

//返回0:存在       

u8 DS18B20_Init(void)

{

       RCC->APB2ENR|=1<<2;    //使能PORTA口时钟		//wl

       DS18B20_IO_OUT();//PORTA.0 推挽输出		   	//wl

       GPIOA->ODR|=1;      //输出1				//wl

       DS18B20_Rst();

       return DS18B20_Check();

} 

//从ds18b20得到温度值

//精度：0.1C

//返回值：温度值 （-550~1250）

short DS18B20_Get_Temp(void)

{

    u8 temp;

    u8 TL,TH;

       short tem;

    DS18B20_Start ();         // ds1820 start convert

    DS18B20_Rst();

    DS18B20_Check();      

    DS18B20_Write_Byte(0xcc);// skip rom

    DS18B20_Write_Byte(0xbe);// convert    

    TL=DS18B20_Read_Byte(); // LSB  

    TH=DS18B20_Read_Byte(); // MSB 

    if(TH>7)

    {

        TH=~TH;

        TL=~TL;

        temp=0;//温度为负 

    }else temp=1;//温度为正              

    tem=TH; //获得高八位

    tem<<=8;   

    tem+=TL;//获得底八位

    tem=(float)tem*0.625+0.5;//转换  (四舍五入)

       if(temp)return tem; //返回温度值

       else return -tem;   
}

/******************************************************
函数名称：short Temperaturepro(void)
返回值:short
参数： void
作用：温度处理,返回温度数值，并修改字符串全局变量
*******************************************************/
short Temperaturepro(void)
{
	short tem,temp;
	tem=DS18B20_Get_Temp();
	if (tem < 0)
	{
		temp=-tem;
		tempbuf[0]='-';//符号位
	}
	else
	{
		temp=tem;
		tempbuf[0]=' ' ;
	 }
	if (temp/1000==0)
		tempbuf[1]=' ';
	else	
		tempbuf[1]=temp/1000 + 0x30;//百位

	tempbuf[2]=temp%1000/100 + 0x30;//十位
	if 	((tempbuf[1]==' ')&&(tempbuf[2]=='0'))
		tempbuf[2]=' ';
	tempbuf[3]=temp%100/10 + 0x30; //个位
	tempbuf[4]='.';
	tempbuf[5]=temp%10 + 0x30; //小数
	tempbuf[6]=	'C'; //显示温度符
	tempbuf[7]=	'\0';
	return tem;
}
