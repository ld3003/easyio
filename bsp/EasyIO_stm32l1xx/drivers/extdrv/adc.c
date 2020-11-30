
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
  ADC_InitTypeDef ADC_InitStructure;//����ADC��ʼ���ṹ�����
	
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
	

//  //ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1��ADC2�����ڶ���ģʽ
//  ADC_InitStructure.ADC_ScanConvMode = ENABLE; //ʹ��ɨ��
//  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//ADCת������������ģʽ
//  //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//���������ת�� //ADC_ExternalTrigConvEdge_None
//  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ת�������Ҷ���
//  //ADC_InitStructure.ADC_NbrOfChannel = 1;//ת��ͨ��Ϊͨ��1
//	ADC_InitStructure.ADC_NbrOfConversion = 1;
//  ADC_Init(ADC1, &ADC_InitStructure); //��ʼ��ADC
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_24Cycles);
//  //ADC1ѡ���ŵ�14,�������ȼ�1,����ʱ��239.5������
//  ADC_DMACmd(ADC1, ENABLE);//ʹ��ADC1ģ��DMA
//  ADC_Cmd(ADC1, ENABLE);//ʹ��ADC1
	
//	#if 0
//  ADC_ResetCalibration(ADC1); //����ADC1У׼�Ĵ���
//  while(ADC_GetResetCalibrationStatus(ADC1));//�ȴ�ADC1У׼�������
//  ADC_StartCalibration(ADC1);//��ʼADC1У׼
//  while(ADC_GetCalibrationStatus(ADC1));//�ȴ�ADC1У׼���
//  ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ʹ��ADC1�����ʼת��
//	#endif
//	ADC_SoftwareStartInjectedConv(ADC1);
//	//void ADC_SoftwareStartInjectedConv(ADC_TypeDef* ADCx);
}

static void adcDMAconfig(void)
{
// DMA_InitTypeDef DMA_InitStructure;//����DMA��ʼ���ṹ��
// DMA_DeInit(DMA1_Channel1);//��λDMAͨ��1
// DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address; //���� DMAͨ���������ַ=ADC1_DR_Address
// DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue; //����DMAͨ���洢����ַ
// DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//ָ������ΪԴ��ַ
// DMA_InitStructure.DMA_BufferSize = 1;//����DMA��������С1
// DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//��ǰ����Ĵ�����ַ����
// DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;//��ǰ�洢����ַ����
// DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�����������ݿ��16λ
// DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //����洢�����ݿ��16λ
// DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMAͨ������ģʽλ���λ���ģʽ
// DMA_InitStructure.DMA_Priority = DMA_Priority_High;//DMAͨ�����ȼ���
// DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//��ֹDMAͨ���洢�����洢������
// DMA_Init(DMA1_Channel1, &DMA_InitStructure);//��ʼ��DMAͨ��1
// DMA_Cmd(DMA1_Channel1, ENABLE); //ʹ��DMAͨ��1
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
    if(left >= right)/*�������������ڻ��ߵ����ұߵ������ʹ����Ѿ��������һ������*/
    {
        return ;
    }
    key = a[left];
     
    while(i < j)                               /*�����ڵ�����Ѱ��һ��*/
    {
        while(i < j && key <= a[j])
        /*��Ѱ�ҽ������������ǣ�1���ҵ�һ��С�ڻ��ߴ���key���������ڻ�С��ȡ����������
        ���ǽ���2��û�з�������1�ģ�����i��j�Ĵ�Сû�з�ת*/ 
        {
            j--;/*��ǰѰ��*/
        }
         
        a[i] = a[j];
        /*�ҵ�һ������������Ͱ�������ǰ��ı����ߵ�i��ֵ�������һ��ѭ����key��
        a[left]����ô���Ǹ�key��*/
         
        while(i < j && key >= a[i])
        /*����i�ڵ�������ǰѰ�ң�ͬ�ϣ�����ע����key�Ĵ�С��ϵֹͣѭ���������෴��
        ��Ϊ����˼���ǰ����������ӣ������������ߵ�����С��key�Ĺ�ϵ�෴*/
        {
            i++;
        }
         
        a[j] = a[i];
    }
     
    a[i] = key;/*���ڵ���������һ���Ժ�Ͱ��м���key�ع�*/
    sort(a, left, i - 1);/*�����ͬ���ķ�ʽ�Էֳ�������ߵ�С�����ͬ�ϵ�����*/
    sort(a, i + 1, right);/*��ͬ���ķ�ʽ�Էֳ������ұߵ�С�����ͬ�ϵ�����*/
                       /*��Ȼ�����ܻ���ֺܶ�����ң�ֱ��ÿһ���i = j Ϊֹ*/
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
	
	//�������ʱ��С��10�룬����ٲ���
	if (app_timer_data.app_timer_second < 10)
	{
		tmrcnt = 2;
	}else {
		tmrcnt = 60;
	}
	

	//ÿ tmrcnt �����һ��
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
	 //����GPIO��ʼ���ṹ��
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
