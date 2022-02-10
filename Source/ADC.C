/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
/*---------------------------------------------------------------------*/

/*************  功能说明    **************
读ADC测量外部电压，使用内部基准计算电压.

使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

串口打印测量的电压值.

外部电压从板上测温电阻两端输入, 输入电压0~VDD, 不要超过VDD或低于0V. 

实际项目使用请串一个1K的电阻到ADC输入口, ADC输入口再并一个电容到地.

下载时, 选择默认时钟 11.0592MHZ.

******************************************/
#define	_ADC_GLOBAL_

#include "ADC.H"
#include <stdio.h>

#include "HardWare.H"
#include "VariableCfg.H"
#include "GpioDefine.H"
#include "Public.h"


//#define Cal_MODE    0   //每次测量只读1次ADC. 分辨率0.01V
#define Cal_MODE    1   //每次测量连续读16次ADC 再平均计算. 分辨率0.01V

#define	ADC_USER_MAX		3	//用户使用最大ADC通道数

//结构
AdcChnSampleStruct   AdcSameple[ADC_USER_MAX];			// adc 通道采集数据

//========================================================================
// 函数: void ADCVarInit(void)
// 描述: 配置ADC 相关变量.
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2021-12-10
//========================================================================
void ADCVarInit(void) 
{
	AdcSameple[0].adcChn = ADC1;	//版本号ADC
	AdcSameple[0].adcSampleVal = 0x0;
	AdcSameple[0].adcValue = 0x0;
	
	AdcSameple[1].adcChn = ADC8;	//电机检测ADC
	AdcSameple[1].adcSampleVal = 0x0;
	AdcSameple[1].adcValue = 0x0;
	
	AdcSameple[2].adcChn = ADC9;	//按键AD
	AdcSameple[2].adcSampleVal = 0x0;
	AdcSameple[2].adcValue = 0x0;
}

//========================================================================
// 函数: void ADC_Config(void)
// 描述: 配置ADC IO和相关寄存器.
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2021-12-10
//========================================================================
void ADC_Init(void)
{
	/* 端口模式
	 * M1M0:00--准双向口(传统8051 I/O口) 
			01--推挽输出 
			10--高阻输入 
			11--开漏(可读可输出，但需要外加上拉电阻)
	*/
//	P0M1|=0x02; P0M0&=~0x02; //P01高阻输入
//	P1M1|=0x02; P1M0&=~0x02; //P11高阻输入
	
	P_SW2 |= 0x80;
	ADCTIM = 0x3f;		//设置 ADC 内部时序，ADC采样时间建议设最大值
	P_SW2 &= 0x7f;
	ADCCFG = 0x2f;		//设置 ADC 时钟为系统时钟/2/16
	ADC_CONTR = 0x80; //使能 ADC 模块
}

//========================================================================
// 函数: uint Get_ADC12bitResult(uchar channel)
// 描述: 查询法读一次ADC结果.
// 参数: channel: 选择要转换的ADC.
// 返回: 12位ADC结果.
// 版本: V1.0, 2012-10-22
//========================================================================
uint Get_ADC12bitResult(uchar channel)  //channel = 0~15
{
    ADC_RES = 0;
    ADC_RESL = 0;

    ADC_CONTR = (ADC_CONTR & 0xF0) | 0x40 | channel;    //启动 AD 转换
    _nop_();
    _nop_();
    _nop_();
    _nop_();

    while((ADC_CONTR & 0x20) == 0)  ;   //wait for ADC finish
    ADC_CONTR &= ~0x20;     //清除ADC结束标志
    return  (((u16)ADC_RES << 8) | ADC_RESL);
}

//========================================================================
// 函数: uint Get_VoltageValue(uchar adcChn)
// 描述: 返回读取一次的ADC结果.
// 参数: channel: 选择要转换的ADC.
// 返回: 12位ADC结果, 是换算过后的ADC值，大小为实际值*100.
// 版本: V1.0, 2021-12-10
//========================================================================
//uint Get_VoltageValue(uchar adcChn)
//{
//	uchar chn = 0;	//通道索引
//    uint j = 0;		//计算的AD值
//	uint Bandgap = 0;//内部基准ADC
//	
//	//=================== 只读1次ADC, 12bit ADC. 分辨率0.01V ===============================
//	//Get_ADC12bitResult(15);  			//先读一次并丢弃结果, 让内部的采样电容的电压等于输入值.
//	Bandgap = Get_ADC12bitResult(15);   //读内部基准ADC, 读15通道
//	for(chn = 0; chn < ADC_USER_MAX; chn++)
//	{
//		if(AdcSameple[chn].adcChn == adcChn)//当前需要读取的通道
//		{
//			AdcSameple[chn].adcSampleVal = Get_ADC12bitResult(AdcSameple[chn].adcChn);  //读外部电压ADC
//			if(AdcSameple[chn].adcSampleVal < 4096)	//参数0~15,查询方式做一次ADC, 返回值就是结果, == 4096 为错误
//				AdcSameple[chn].adcValue = (uint)(AdcSameple[chn].adcSampleVal * 119 / Bandgap);//计算外部电压, Bandgap为1.19V, 测电压分辨率0.01V
//			return AdcSameple[chn].adcValue;
//		}
//	}
//	
//	return 0;
//}

/* 读取16次，取平均值 */
uint Get_VoltageValue(uchar adcChn)
{
	uchar chn = 0;	//通道索引
	uchar  i = 0;
	uint j = 0;		//计算的AD值
	uint Bandgap = 0;//内部基准ADC
	
	for(chn = 0; chn < ADC_USER_MAX; chn++)
	{
		if(AdcSameple[chn].adcChn == adcChn)//当前需要读取的通道
		{
			//Get_ADC12bitResult(15);  			//先读一次并丢弃结果, 让内部的采样电容的电压等于输入值.
			for(j=0, i=0; i<16; i++)
			{
				j += Get_ADC12bitResult(15); 	//读内部基准ADC, 读15通道
			}
			Bandgap = j >> 4;   //16次平均
			

			AdcSameple[chn].adcSampleVal = 0;
			for(i=0; i<16; i++)
			{
				AdcSameple[chn].adcSampleVal += Get_ADC12bitResult(AdcSameple[chn].adcChn);	//读外部电压ADC
			}
			AdcSameple[chn].adcSampleVal = AdcSameple[chn].adcSampleVal >> 4; 				//16次平均		
			AdcSameple[chn].adcValue = (uint)(AdcSameple[chn].adcSampleVal * 119 / Bandgap);//计算外部电压, Bandgap为1.19V, 测电压分辨率0.01V
			
			return AdcSameple[chn].adcValue;
		}
	}
	
	return 0;
}

//========================================================================
// 函数: void Read_VoltageValue(void)
// 描述: 读取并计算.
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2021-12-10
//========================================================================
void Read_VoltageValue(void)
{
	static ulong adcTmer;
#if (Cal_MODE == 1)
    uchar  i = 0;
#endif
	uchar chn = 0;	//通道索引
    ulong j = 0;		//计算的AD值
	uint Bandgap = 0;//内部基准ADC

	if(ReadUserTimer(&adcTmer) >= T_100MS * 10)		//Nms到
	{
		ResetUserTimer(&adcTmer);

	#if (Cal_MODE == 0)
	//=================== 只读1次ADC, 12bit ADC. 分辨率0.01V ===============================
		//Get_ADC12bitResult(15);  			//先读一次并丢弃结果, 让内部的采样电容的电压等于输入值.
		Bandgap = Get_ADC12bitResult(15);   //读内部基准ADC, 读15通道
		for(chn = 0; chn < ADC_USER_MAX; chn++)
		{
			AdcSameple[chn].adcSampleVal = Get_ADC12bitResult(AdcSameple[chn].adcChn);  	//读外部电压ADC
			if(AdcSameple[chn].adcSampleVal < 4096)	//参数0~15,查询方式做一次ADC, 返回值就是结果, == 4096 为错误
				AdcSameple[chn].adcValue = (uint)(AdcSameple[chn].adcSampleVal * 119 / Bandgap);//计算外部电压, Bandgap为1.19V, 测电压分辨率0.01V
		}
	#endif
	//==========================================================================

	//===== 连续读16次ADC 再平均计算. 分辨率0.01V =========
	#if (Cal_MODE == 1)
		//Get_ADC12bitResult(15);  			//先读一次并丢弃结果, 让内部的采样电容的电压等于输入值.
		for(j=0, i=0; i<16; i++)
		{
			j += Get_ADC12bitResult(15); 	//读内部基准ADC, 读15通道
		}
		Bandgap = j >> 4;   //16次平均
		
		for(chn = 0; chn < ADC_USER_MAX; chn++)
		{
			AdcSameple[chn].adcSampleVal = 0;
			for(i=0; i<16; i++)
			{
				AdcSameple[chn].adcSampleVal += Get_ADC12bitResult(AdcSameple[chn].adcChn);	//读外部电压ADC
			}
			AdcSameple[chn].adcSampleVal = AdcSameple[chn].adcSampleVal >> 4; 				//16次平均		
			AdcSameple[chn].adcValue = (uint)(AdcSameple[chn].adcSampleVal * 119 / Bandgap);//计算外部电压, Bandgap为1.19V, 测电压分辨率0.01V
		}
	#endif
	//==========================================================================

		for(chn = 0; chn < ADC_USER_MAX; chn++)
		{
			printf("ADC%bd ReadVal: %.2fV\r\n", AdcSameple[chn].adcChn, AdcSameple[chn].adcValue/100.0);  	//显示外部电压值		
		}
	}
	
}

