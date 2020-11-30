
#include "common.h"
#include "adc.h"
#include <rtthread.h>
#include "my_stdc_func/debugl.h"
#include "app_timer.h"
#include "gps_tracker.h"
#include "led.h"

static unsigned int ADC_ConvertedValue;

static void ADC_Config(void)
{
  ADC_InitTypeDef ADC_InitStructure;//定义ADC初始化结构体变量
	
	memset(&ADC_InitStructure,0x0,sizeof(ADC_InitStructure));
	
	
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	
	ADC_Init(ADC1,&ADC_InitStructure);
    
  ADC_Cmd(ADC1,ENABLE);
  ADC_RegularChannelConfig(ADC1,ADC_Channel_12,1,ADC_SampleTime_16Cycles);
  
  ADC_SoftwareStartConv(ADC1);
	

//  //ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1和ADC2工作在独立模式
//  ADC_InitStructure.ADC_ScanConvMode = ENABLE; //使能扫描
//  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//ADC转换工作在连续模式
//  //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//有软件控制转换 //ADC_ExternalTrigConvEdge_None
//  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//转换数据右对齐
//  //ADC_InitStructure.ADC_NbrOfChannel = 1;//转换通道为通道1
//	ADC_InitStructure.ADC_NbrOfConversion = 1;
//  ADC_Init(ADC1, &ADC_InitStructure); //初始化ADC
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_24Cycles);
//  //ADC1选择信道14,音序器等级1,采样时间239.5个周期
//  ADC_DMACmd(ADC1, ENABLE);//使能ADC1模块DMA
//  ADC_Cmd(ADC1, ENABLE);//使能ADC1
	
//	#if 0
//  ADC_ResetCalibration(ADC1); //重置ADC1校准寄存器
//  while(ADC_GetResetCalibrationStatus(ADC1));//等待ADC1校准重置完成
//  ADC_StartCalibration(ADC1);//开始ADC1校准
//  while(ADC_GetCalibrationStatus(ADC1));//等待ADC1校准完成
//  ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能ADC1软件开始转换
//	#endif
//	ADC_SoftwareStartInjectedConv(ADC1);
//	//void ADC_SoftwareStartInjectedConv(ADC_TypeDef* ADCx);
}

static void adcDMAconfig(void)
{
// DMA_InitTypeDef DMA_InitStructure;//定义DMA初始化结构体
// DMA_DeInit(DMA1_Channel1);//复位DMA通道1
// DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address; //定义 DMA通道外设基地址=ADC1_DR_Address
// DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue; //定义DMA通道存储器地址
// DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//指定外设为源地址
// DMA_InitStructure.DMA_BufferSize = 1;//定义DMA缓冲区大小1
// DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//当前外设寄存器地址不变
// DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;//当前存储器地址不变
// DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//定义外设数据宽度16位
// DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //定义存储器数据宽度16位
// DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA通道操作模式位环形缓冲模式
// DMA_InitStructure.DMA_Priority = DMA_Priority_High;//DMA通道优先级高
// DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//禁止DMA通道存储器到存储器传输
// DMA_Init(DMA1_Channel1, &DMA_InitStructure);//初始化DMA通道1
// DMA_Cmd(DMA1_Channel1, ENABLE); //使能DMA通道1
}


//void xxadc_init()
//{
//	 /* DMA1 channel1 configuration ----------------------------------------------*/
//  DMA_DeInit(DMA1_Channel1);
//  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
//  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADCConvertedValue;
//  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//  DMA_InitStructure.DMA_BufferSize = 2;
//  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
//  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

//  /* Enable DMA1 channel1 */
//  DMA_Cmd(DMA1_Channel1, ENABLE);
//	
//	/* Enable ADC1 and GPIOC clock */
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB, ENABLE);


//  /* ADC1 configuration ------------------------------------------------------*/
//  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
//  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
//  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
//  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
//  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
//  ADC_InitStructure.ADC_NbrOfChannel = 1;
//  ADC_Init(ADC1, &ADC_InitStructure);
//  /* ADC1 regular channel13 configuration */
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);

//  /* Enable ADC1 DMA */
//  ADC_DMACmd(ADC1, ENABLE);

//  /* Enable ADC1 */
//  ADC_Cmd(ADC1, ENABLE);

//  /* Enable ADC1 reset calibaration register */
//  ADC_ResetCalibration(ADC1);
//  /* Check the end of ADC1 reset calibration register */
//  while(ADC_GetResetCalibrationStatus(ADC1));

//  /* Start ADC1 calibaration */
//  ADC_StartCalibration(ADC1);
//  /* Check the end of ADC1 calibration */
//  while(ADC_GetCalibrationStatus(ADC1));

//  /* Start ADC1 Software Conversion */
//  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

//}
uint16_t xxget_adcval(void)
{
	
	uint16_t val = ADC_GetConversionValue(ADC1);
	DEBUGL->debug("ADC : %d \r\n",val);
	return val;
}

uint16_t get_bat_vol_fast(void)
{
	uint16_t val,vol;
	initADC();
	val = ADC_GetConversionValue(ADC1);	
	disableADC();	
	DEBUGL->debug("ADC : %d \r\n",val);
	vol = (val*2474) / 4096;
	vol *= 6;
	
	return vol;
	
}



static void sort(unsigned short *a, int left, int right)
{
	   int i = left;
    int j = right;
    int key;// = a[left];
    if(left >= right)/*如果左边索引大于或者等于右边的索引就代表已经整理完成一个组了*/
    {
        return ;
    }
    key = a[left];
     
    while(i < j)                               /*控制在当组内寻找一遍*/
    {
        while(i < j && key <= a[j])
        /*而寻找结束的条件就是，1，找到一个小于或者大于key的数（大于或小于取决于你想升
        序还是降序）2，没有符合条件1的，并且i与j的大小没有反转*/ 
        {
            j--;/*向前寻找*/
        }
         
        a[i] = a[j];
        /*找到一个这样的数后就把它赋给前面的被拿走的i的值（如果第一次循环且key是
        a[left]，那么就是给key）*/
         
        while(i < j && key >= a[i])
        /*这是i在当组内向前寻找，同上，不过注意与key的大小关系停止循环和上面相反，
        因为排序思想是把数往两边扔，所以左右两边的数大小与key的关系相反*/
        {
            i++;
        }
         
        a[j] = a[i];
    }
     
    a[i] = key;/*当在当组内找完一遍以后就把中间数key回归*/
    sort(a, left, i - 1);/*最后用同样的方式对分出来的左边的小组进行同上的做法*/
    sort(a, i + 1, right);/*用同样的方式对分出来的右边的小组进行同上的做法*/
                       /*当然最后可能会出现很多分左右，直到每一组的i = j 为止*/
}

static uint16_t bat_vol_buffer[5] = {0,0,0,0,0};
static uint16_t bat_vol_filter(uint16_t vol)
{
	bat_vol_buffer[4] = vol;
	sort(bat_vol_buffer,0,4);
	return bat_vol_buffer[4];
}


uint16_t get_bat_vol(void)
{
	static uint16_t vol = 0;
	uint16_t val;
	unsigned char tmrcnt;
	
	static unsigned int sec = 0xFFFFFFFF;
	
	//如果开机时间小于10秒，则快速采样
	if (app_timer_data.app_timer_second < 10)
	{
		tmrcnt = 2;
	}else {
		tmrcnt = 60;
	}
	

	//每 tmrcnt 秒采样一次
	if (app_timer_data.app_timer_second > (sec + tmrcnt) || sec == 0xFFFFFFFF)
	{
		
		initADC();

		val = ADC_GetConversionValue(ADC1);
		val = bat_vol_filter(val);
		
		disableADC();
		
		DEBUGL->debug("ADC : %d \r\n",val);
		vol = (val*2474) / 4096;
		vol *= 6;
		DEBUGL->debug("BAT VOL : %d \r\n",vol);
		sec = app_timer_data.app_timer_second;
	}
	
	tracker_private_data.bat_val = vol;
	return vol;
	
}


//7 FULL
//6 AC
unsigned char get_bat_pre(void)
{
	unsigned int vol;
	vol = tracker_private_data.bat_val;
	
	if (IS_CHARGE)
	{
		if (vol >= 4000)
			return 7;
		return 6;
	}
	
	if (vol >= 4000)
		return 5;
	if (vol >= 3850)
		return 4;
	if (vol >= 3700)
		return 3;
	if (vol >= 3600)
		return 2;
	if (vol >= 3450)
		return 1;
	
	return 0;

        //
}

#if 0
 if m_battery_adc >= 4000 then
    return powerApi.BAT_STATE_5
  elseif  m_battery_adc >= 3850 then
    return powerApi.BAT_STATE_4
  elseif  m_battery_adc >= 3700 then
    return powerApi.BAT_STATE_3
  elseif  m_battery_adc >= 3600 then
    return powerApi.BAT_STATE_2
  elseif  m_battery_adc >= 3450 then
    return powerApi.BAT_STATE_1
  elseif  m_battery_adc < 3450 then
    return powerApi.BAT_STATE_0
  end
#endif




void initADC(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_HSICmd(ENABLE);
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	 //定义GPIO初始化结构体
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	ADC_Config();
	//
}

void disableADC(void)
{
	RCC_HSICmd(DISABLE);
	ADC_DeInit(ADC1);
}

void readADC(void)
{
	
	//
}
