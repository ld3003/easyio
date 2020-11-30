/*
 * File      : usart.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006-2013, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2010-03-29     Bernard      remove interrupt Tx and DMA Rx mode
 * 2013-05-13     aozima       update for kehong-lingtai.
 */

#include "stm32l1xx.h"
//#include "stm32l1xx.h"
#include "usart.h"
#include "board.h"
#include "gnss.h"
#include "app_local_conf.h"
#include <rtdevice.h>

/* USART1 */
#define UART1_GPIO_TX		GPIO_Pin_9
#define UART1_GPIO_RX		GPIO_Pin_10
#define UART1_GPIO			GPIOA

/* USART2 */
#define UART2_GPIO_TX	    GPIO_Pin_2
#define UART2_GPIO_RX	    GPIO_Pin_3
#define UART2_GPIO	    	GPIOA

/* USART3_REMAP[1:0] = 00 */
#define UART3_GPIO_TX		GPIO_Pin_10
#define UART3_GPIO_RX		GPIO_Pin_11
#define UART3_GPIO_CTS		GPIO_Pin_13
#define UART3_GPIO_RTS		GPIO_Pin_14
#define UART3_GPIO			GPIOB

/* STM32 uart driver */
struct stm32_uart
{
    USART_TypeDef* uart_device;
    IRQn_Type irq;
};

#ifdef FRQL
unsigned char UART3_USE_DMA_FLAG = 1;
#else
unsigned char UART3_USE_DMA_FLAG = 1;
#endif
unsigned char UART2_USE_DMA_FLAG = 1;

#define UART3_USE_DMA			UART3_USE_DMA_FLAG
#define UART2_USE_DMA			UART2_USE_DMA_FLAG

static unsigned char init_uart3_dma_tmr_flag = 0;
static unsigned char init_uart2_dma_tmr_flag = 0;

struct SERIAL_CFG{
	unsigned char flag;
	struct rt_serial_device *device;
	struct serial_configure *cfg;
};

struct SERIAL_CFG serial_cfg[3] = {0,0,0,0,0,0,0,0,0};
static rt_err_t stm32_configure(struct rt_serial_device *serial, struct serial_configure *cfg);

static unsigned int uart2_recv_length = 0;



#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

rt_inline void serial_ringbuffer_init(struct serial_ringbuffer *rbuffer)
{
    rt_memset(rbuffer->buffer, 0, sizeof(rbuffer->buffer));
    rbuffer->put_index = 0;
    rbuffer->get_index = 0;
}

rt_inline void serial_ringbuffer_putc(struct serial_ringbuffer *rbuffer,
                                      char                      ch)
{
    rt_base_t level;

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    rbuffer->buffer[rbuffer->put_index] = ch;
    rbuffer->put_index = (rbuffer->put_index + 1) & (RT_SERIAL_RB_BUFSZ - 1);

    /* if the next position is read index, discard this 'read char' */
    if (rbuffer->put_index == rbuffer->get_index)
    {
        rbuffer->get_index = (rbuffer->get_index + 1) & (RT_SERIAL_RB_BUFSZ - 1);
    }

    /* enable interrupt */
    rt_hw_interrupt_enable(level);
}

rt_inline int serial_ringbuffer_putchar(struct serial_ringbuffer *rbuffer,
                                        char                      ch)
{
    rt_base_t level;
    rt_uint16_t next_index;

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    next_index = (rbuffer->put_index + 1) & (RT_SERIAL_RB_BUFSZ - 1);
    if (next_index != rbuffer->get_index)
    {
        rbuffer->buffer[rbuffer->put_index] = ch;
        rbuffer->put_index = next_index;
    }
    else
    {
        /* enable interrupt */
        rt_hw_interrupt_enable(level);

        return -1;
    }

    /* enable interrupt */
    rt_hw_interrupt_enable(level);

    return 1;
}

rt_inline int serial_ringbuffer_getc(struct serial_ringbuffer *rbuffer)
{
    int ch;
    rt_base_t level;

    ch = -1;
    /* disable interrupt */
    level = rt_hw_interrupt_disable();
    if (rbuffer->get_index != rbuffer->put_index)
    {
        ch = rbuffer->buffer[rbuffer->get_index];
        rbuffer->get_index = (rbuffer->get_index + 1) & (RT_SERIAL_RB_BUFSZ - 1);
    }
    /* enable interrupt */
    rt_hw_interrupt_enable(level);

    return ch;
}

rt_inline rt_uint32_t serial_ringbuffer_size(struct serial_ringbuffer *rbuffer)
{
    rt_uint32_t size;
    rt_base_t level;

    level = rt_hw_interrupt_disable();
    size = (rbuffer->put_index - rbuffer->get_index) & (RT_SERIAL_RB_BUFSZ - 1);
    rt_hw_interrupt_enable(level);

    return size;
}

unsigned char Uart_Rx[UART_RX_LEN];
unsigned char Uart2_Rx[UART_RX_LEN];
unsigned short Uart_Rx_Length = 0;
unsigned short Uart2_Rx_Length = 0;
static struct rt_timer dma_rx_tmr;
static struct rt_timer dma2_rx_tmr;
static struct rt_serial_device *uart3dev;
static struct rt_serial_device *uart2dev;

//50ms
static void dma_rx_timeout(void *p)
{
	int i=0;
	unsigned int temp;
	unsigned char ch;
	struct rt_serial_device *serial = uart3dev;
	static unsigned short history_dma_buffer_length = 0;
	 
	temp = UART_RX_LEN - DMA_GetCurrDataCounter(DMA1_Channel3); 
	
	
	if (temp == 0)
	{
		history_dma_buffer_length = 0;
		return ;
	}
	
	
	if (history_dma_buffer_length > temp)
	{
		DMA_Cmd(DMA1_Channel3,DISABLE); 
		history_dma_buffer_length = 0;
		DMA_SetCurrDataCounter(DMA1_Channel3,UART_RX_LEN);
		DMA_Cmd(DMA1_Channel3,ENABLE); 
		
	}else
	if (temp > history_dma_buffer_length)
	{
		history_dma_buffer_length = temp;
	}else
	if (temp  == history_dma_buffer_length)
	{
		USART_Cmd(USART3, DISABLE);
		DMA_Cmd(DMA1_Channel3,DISABLE); 
		temp = UART_RX_LEN - DMA_GetCurrDataCounter(DMA1_Channel3); 
		
		//只有当DMA开启的时候才。。。
		if (UART3_USE_DMA == 1)
		{
			//
			for(i=0;i<temp;i++)
			{
				serial_ringbuffer_putc(serial->int_rx,Uart_Rx[i]);
				//
			}

			/* invoke callback */
			if (serial->parent.rx_indicate != RT_NULL)
			{
					rt_size_t rx_length;

					/* get rx length */
					rx_length = serial_ringbuffer_size(serial->int_rx);
					serial->parent.rx_indicate(&serial->parent, rx_length);
			}
			
		}	
			
		//clear counter
		history_dma_buffer_length = 0;
		
		DMA_SetCurrDataCounter(DMA1_Channel3,UART_RX_LEN);
		DMA_Cmd(DMA1_Channel3,ENABLE); 
		USART_Cmd(USART3, ENABLE);
	
	}		
}


//50ms
#include "app_ev.h"
#include "my_stdc_func/debugl.h"
static unsigned short history_dma_buffer_length2 = 0;
static unsigned short gnss_data_length = 0;
extern void put_gps_rawdata(unsigned char *xbuffer , unsigned int size);
static void __dma_process_gnss_data(void *p)
{
	int i=0;
	
	//拷贝有效数据
	memcpy(gps_buffer_tmp,Uart2_Rx,Uart2_Rx_Length);
	gps_buffer_tmp_length = Uart2_Rx_Length;
	
	#if 0
	//DEBUGL->debug("\r\n\r\n\r\n\r\n%s \r\n\r\n\r\n",gps_buffer_tmp);
	DEBUGL->debug("\r\n\r\n\r\n\r\n");
	for(i=0;i<gps_buffer_tmp_length;i++)
	{
		DEBUGL->debug("%c",gps_buffer_tmp[i]);
	}
	DEBUGL->debug("\r\n\r\n\r\n\r\n");
	#endif
	
	//处理串口数据
	parser_gps_rawdata(gps_buffer_tmp,gps_buffer_tmp_length);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	//开启中断
	
	
}

static void init_uart2_dma(void);
static void dma2_rx_timeout(void *p)
{
	int i=0;
	unsigned int temp;
	unsigned char ch;
	struct rt_serial_device *serial = uart2dev;
	
	
	static unsigned char dma_reset_cnt = 0;
	 
	temp = UART_RX_LEN - DMA_GetCurrDataCounter(DMA1_Channel6); 
	
	if (temp == 0)
	{
		history_dma_buffer_length2 = 0;
		return ;
	}
	
	
	if (history_dma_buffer_length2 > temp)
	{
		DMA_Cmd(DMA1_Channel6,DISABLE); 
		history_dma_buffer_length2 = 0;
		DMA_SetCurrDataCounter(DMA1_Channel6,UART_RX_LEN);
		DMA_Cmd(DMA1_Channel6,ENABLE); 
		
	}else
	if (temp > history_dma_buffer_length2)
	{
		history_dma_buffer_length2 = temp;
	}else
	if (temp  == history_dma_buffer_length2)
	{
		DMA_Cmd(DMA1_Channel6,DISABLE); 
		temp = UART_RX_LEN - DMA_GetCurrDataCounter(DMA1_Channel6); 
		
		//只有当DMA2启用的时候
		if (UART2_USE_DMA == 1)
		{
			//
			for(i=0;i<temp;i++)
			{
				serial_ringbuffer_putc(serial->int_rx,Uart2_Rx[i]);
				//
			}

			/* invoke callback */
			if (serial->parent.rx_indicate != RT_NULL)
			{
					rt_size_t rx_length;

					/* get rx length */
					rx_length = serial_ringbuffer_size(serial->int_rx);
					serial->parent.rx_indicate(&serial->parent, rx_length);
			}
		}
		
		
		
		//clear counter
		history_dma_buffer_length2 = 0;

		DMA_SetCurrDataCounter(DMA1_Channel6,UART_RX_LEN);
		DMA_Cmd(DMA1_Channel6,ENABLE); 
		//fuck


	}		
}

static void uart2_rx_timeout(void *p)
{
	static unsigned int history_gps_buffer_tmp_length = 0;
	
	//如果相等，说明没有再接收数据了
	if(history_gps_buffer_tmp_length == uart2_recv_length)
	{
		
		if (uart2_recv_length > 0)
		{
			
			Uart2_Rx_Length = uart2_recv_length;
			if (post_hw_int_event(__dma_process_gnss_data,0,0,0) == RT_EOK)
			{
				USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
			}else
			{
				//clear counter
				USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
				
			}
		}
		
		//清空重新接收
		uart2_recv_length = 0;
	}
	
	history_gps_buffer_tmp_length = uart2_recv_length;
	//
}



void DMA1_Channel3_IRQHandler(void)
{
	static unsigned int LeftDataCounter;

	LeftDataCounter = DMA_GetCurrDataCounter(DMA1_Channel3);
	
	DMA_ClearFlag(DMA1_FLAG_TC3);
	DMA_ClearFlag(DMA1_FLAG_HT3);
	DMA_ClearFlag(DMA1_FLAG_TE3);
	DMA_ClearFlag(DMA1_FLAG_GL3);
}



static void init_uart3_dma_tmr(void)
{
	//50ms
	if (init_uart3_dma_tmr_flag == 1)
		return;
	rt_timer_init(&dma_rx_tmr,"dma_tmr",dma_rx_timeout,0,5,RT_TIMER_FLAG_PERIODIC);
	rt_timer_start(&dma_rx_tmr);
	init_uart3_dma_tmr_flag = 1;
	
}
static void disable_uart3_dma(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE);
	DMA_Cmd(DMA1_Channel3,DISABLE);
	//采用DMA方式接收
	USART_DMACmd(USART3,USART_DMAReq_Rx,DISABLE);
	
	if (init_uart3_dma_tmr_flag == 1)
	{
		rt_timer_stop(&dma_rx_tmr);
		init_uart3_dma_tmr_flag = 0;
	}
	//
}

static void disable_uart2_dma(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE);
	DMA_Cmd(DMA1_Channel3,DISABLE);
	//采用DMA方式接收
	USART_DMACmd(USART2,USART_DMAReq_Rx,DISABLE);
	
	if (init_uart2_dma_tmr_flag == 1)
	{
		rt_timer_stop(&dma2_rx_tmr);
		init_uart2_dma_tmr_flag = 0;
	}
	//
}

static void init_uart3_dma(void)
{


	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	//串口收DMA配置  
	//启动DMA时钟
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	//DMA1通道5配置
	DMA_DeInit(DMA1_Channel3);
	//外设地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);
	//内存地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart_Rx;
	//dma传输方向单向
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	//设置DMA在传输时缓冲区的长度
	DMA_InitStructure.DMA_BufferSize = UART_RX_LEN;
	//设置DMA的外设递增模式，一个外设
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//设置DMA的内存递增模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//外设数据字长
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	//内存数据字长
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	//设置DMA的传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//设置DMA的优先级别
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	//设置DMA的2个memory中的变量互相访问
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3,&DMA_InitStructure);

	
	DMA_Cmd(DMA1_Channel3,ENABLE);
	
	//采用DMA方式接收
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
	

}


static void init_uart2_dma_tmr(void)
{

	if (init_uart2_dma_tmr_flag == 1)
		return;
	
	rt_timer_init(&dma2_rx_tmr,"dma2_tmr",dma2_rx_timeout,0,5,RT_TIMER_FLAG_PERIODIC);
	rt_timer_start(&dma2_rx_tmr);
	init_uart2_dma_tmr_flag = 1;
	//
}

static void init_uart2_it_tmr(void)
{
	
	static unsigned char init_uart2_it_tmr_flag = 0;
	if (init_uart2_it_tmr_flag == 1)
		return;
	//50ms
	rt_timer_init(&dma2_rx_tmr,"uart2_tmr",uart2_rx_timeout,0,5,RT_TIMER_FLAG_PERIODIC);
	rt_timer_start(&dma2_rx_tmr);
	init_uart2_it_tmr_flag = 1;
	//
}

static void init_uart2_dma(void)
{


	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	//串口收DMA配置  
	//启动DMA时钟
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	//DMA1通道5配置
	DMA_DeInit(DMA1_Channel6);
	//外设地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);
	//内存地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart2_Rx;
	//dma传输方向单向
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	//设置DMA在传输时缓冲区的长度
	DMA_InitStructure.DMA_BufferSize = UART_RX_LEN;
	//设置DMA的外设递增模式，一个外设
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//设置DMA的内存递增模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//外设数据字长
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	//内存数据字长
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	//设置DMA的传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//设置DMA的优先级别
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	//设置DMA的2个memory中的变量互相访问
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel6,&DMA_InitStructure);

	
	DMA_Cmd(DMA1_Channel6,ENABLE);
	
	//采用DMA方式接收
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	
	

}



void reinit_system_serial(void)
{
	int i=0;
	for(i=0;i<3;i++)
	{
		if (serial_cfg[i].flag == 1)
		{
			stm32_configure(serial_cfg[i].device,serial_cfg[i].cfg);
		}
	}
	//
}

//void reinit_uart3_baud(unsigned int baud)
//{
//	serial_cfg[2].cfg->baud_rate = baud;
//	stm32_configure(serial_cfg[2].device,serial_cfg[2].cfg);
//	
//	
//}

//static rt_err_t stm32_re_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
//{
//    struct stm32_uart* uart;
//    USART_InitTypeDef USART_InitStructure;

//    RT_ASSERT(serial != RT_NULL);
//    RT_ASSERT(cfg != RT_NULL);

//    uart = (struct stm32_uart *)serial->parent.user_data;
//	
//		
//		if (uart->uart_device == USART1)
//		{
//			serial_cfg[0].flag = 1;
//			serial_cfg[0].device = serial;
//			serial_cfg[0].cfg = cfg;
//			
//		}
//		if (uart->uart_device == USART2)
//		{
//			serial_cfg[1].flag = 1;
//			serial_cfg[1].device = serial;
//			serial_cfg[1].cfg = cfg;
//		}
//		if (uart->uart_device == USART3)
//		{
//			serial_cfg[2].flag = 1;
//			serial_cfg[2].device = serial;
//			serial_cfg[2].cfg = cfg;
//		}
//	

//    USART_InitStructure.USART_BaudRate = cfg->baud_rate;

//    if (cfg->data_bits == DATA_BITS_8)
//        USART_InitStructure.USART_WordLength = USART_WordLength_8b;

//    if (cfg->stop_bits == STOP_BITS_1)
//        USART_InitStructure.USART_StopBits = USART_StopBits_1;
//    else if (cfg->stop_bits == STOP_BITS_2)
//        USART_InitStructure.USART_StopBits = USART_StopBits_2;

//    USART_InitStructure.USART_Parity = USART_Parity_No;
//    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//		//如果是Uart3 则开启流控
//		if (uart->uart_device == USART3)
//		{
//			USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
//		}
//		
//    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//    USART_Init(uart->uart_device, &USART_InitStructure);

//		/* Enable USART */
//    USART_Cmd(uart->uart_device, ENABLE);
//		

//		if (uart->uart_device == USART1)
//		{
//			/* enable interrupt */
//			USART_ITConfig(uart->uart_device, USART_IT_RXNE, ENABLE);
//		}
//		
//				//disable rx interrupt
//		if (uart->uart_device == USART3)
//		{
//			uart3dev = serial;
//			if (UART3_USE_DMA == 1)
//			{
//				init_uart3_dma();
//				USART_ITConfig(uart->uart_device, USART_IT_RXNE, DISABLE);
//			}else{
//				USART_ITConfig(uart->uart_device, USART_IT_RXNE, ENABLE);
//			}
//		}
//		
//		
//		
//		//disable rx interrupt
//		if (uart->uart_device == USART2)
//		{
//			uart2dev = serial;
//			
//			if (UART2_USE_DMA == 1)
//			{
//				init_uart2_dma();
//				USART_ITConfig(uart->uart_device, USART_IT_RXNE, ENABLE);
//			}else
//			{
//				USART_ITConfig(uart->uart_device, USART_IT_RXNE, ENABLE);
//			}
//		}
//		
//		
//    return RT_EOK;
//}


static rt_err_t stm32_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct stm32_uart* uart;
    USART_InitTypeDef USART_InitStructure;

    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);

    uart = (struct stm32_uart *)serial->parent.user_data;
	
		
		if (uart->uart_device == USART1)
		{
			serial_cfg[0].flag = 1;
			serial_cfg[0].device = serial;
			serial_cfg[0].cfg = cfg;
			
		}
		if (uart->uart_device == USART2)
		{
			serial_cfg[1].flag = 1;
			serial_cfg[1].device = serial;
			serial_cfg[1].cfg = cfg;
		}
		if (uart->uart_device == USART3)
		{
			serial_cfg[2].flag = 1;
			serial_cfg[2].device = serial;
			serial_cfg[2].cfg = cfg;
		}
	

    USART_InitStructure.USART_BaudRate = cfg->baud_rate;

    if (cfg->data_bits == DATA_BITS_8)
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;

    if (cfg->stop_bits == STOP_BITS_1)
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
    else if (cfg->stop_bits == STOP_BITS_2)
        USART_InitStructure.USART_StopBits = USART_StopBits_2;

    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		//如果是Uart3则开启流控
		if (uart->uart_device == USART3)
		{
			USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
		}
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(uart->uart_device, &USART_InitStructure);
		

    /* Enable USART */
    USART_Cmd(uart->uart_device, ENABLE);
		

		if (uart->uart_device == USART1)
		{
			USART_ITConfig(uart->uart_device, USART_IT_RXNE, ENABLE);
		}
		
		
		
		//disable rx interrupt
		if (uart->uart_device == USART3)
		{
			
			uart3dev = serial;
			if(UART3_USE_DMA == 1)
			{
				init_uart3_dma();
				init_uart3_dma_tmr();
				USART_ITConfig(uart->uart_device, USART_IT_RXNE, DISABLE);
			}else{
				disable_uart3_dma();
				USART_ITConfig(uart->uart_device, USART_IT_RXNE, ENABLE);
			}
		}
		
		
		
		//disable rx interrupt
		if (uart->uart_device == USART2)
		{
			
			uart2dev = serial;
			if(UART2_USE_DMA == 1)
			{
				init_uart2_dma();
				init_uart2_dma_tmr();
				USART_ITConfig(uart->uart_device, USART_IT_RXNE, ENABLE);
			}else{
				/* enable interrupt */
				disable_uart2_dma();
				USART_ITConfig(uart->uart_device, USART_IT_RXNE, ENABLE);
			}
		}
		

    return RT_EOK;
}

static rt_err_t stm32_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct stm32_uart* uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)serial->parent.user_data;

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        /* disable rx irq */
        UART_DISABLE_IRQ(uart->irq);
        break;
    case RT_DEVICE_CTRL_SET_INT:
        /* enable rx irq */
        UART_ENABLE_IRQ(uart->irq);
        break;
    }

    return RT_EOK;
}

static int stm32_putc(struct rt_serial_device *serial, char c)
{
    struct stm32_uart* uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)serial->parent.user_data;
	
		//如果是UART3 说明是开启了流控的串口，所以要做超时判断
		if (uart->uart_device == USART3)
		{
			unsigned short wait_cnt = 0;
			while (!(uart->uart_device->SR & USART_FLAG_TXE))
			{
				wait_cnt ++;
				if (wait_cnt > 0xFFFF / 2)
				{
					DEBUGL->debug("Write UART3 ERROR !\r\n");
					return 1;
				}
			}
			uart->uart_device->DR = c;
			//
		} else {

			//while (!(uart->uart_device->SR & USART_FLAG_TXE));
			uart->uart_device->DR = c;
			while (!(uart->uart_device->SR & USART_FLAG_TC));
			
		}

    return 1;
}

static int stm32_getc(struct rt_serial_device *serial)
{
    int ch;
    struct stm32_uart* uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)serial->parent.user_data;

    ch = -1;
    if (uart->uart_device->SR & USART_FLAG_RXNE)
    {
        ch = uart->uart_device->DR & 0xff;
    }

    return ch;
}

static const struct rt_uart_ops stm32_uart_ops =
{
    stm32_configure,
    stm32_control,
    stm32_putc,
    stm32_getc,
};

#if defined(RT_USING_UART1)
/* UART1 device driver structure */
struct serial_ringbuffer uart1_int_rx;
struct stm32_uart uart1 =
{
    USART1,
    USART1_IRQn,
};
struct rt_serial_device serial1;

void USART1_IRQHandler(void)
{
    struct stm32_uart* uart;

    uart = &uart1;

    /* enter interrupt */
    rt_interrupt_enter();
    if(USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET)
    {
        rt_hw_serial_isr(&serial1);
        /* clear interrupt */
        USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);
    }
    if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET)
    {
        /* clear interrupt */
        USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
    }
		
		if (USART_GetITStatus(uart->uart_device, USART_IT_IDLE) != RESET)
    {
				USART_ClearITPendingBit(uart->uart_device, USART_IT_IDLE);
    }
		
		if (USART_GetITStatus(uart->uart_device, USART_IT_CTS) != RESET)
    {
				USART_ClearITPendingBit(uart->uart_device, USART_IT_CTS);
    }
		
		if (USART_GetITStatus(uart->uart_device, USART_IT_LBD) != RESET)
    {  
				USART_ClearITPendingBit(uart->uart_device, USART_IT_LBD);
    }
		
	 if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)    //接收溢出中断
	 {

rt_hw_serial_isr(&serial1);
		 (u16)(USART1->DR & (u16)0x01FF);   
	 }

		 

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* RT_USING_UART1 */

#if defined(RT_USING_UART2)
/* UART1 device driver structure */
struct serial_ringbuffer uart2_int_rx;
struct stm32_uart uart2 =
{
    USART2,
    USART2_IRQn,
};
struct rt_serial_device serial2;
void USART2_IRQHandler(void)
{
    struct stm32_uart* uart;

    uart = &uart2;

    /* enter interrupt */
    rt_interrupt_enter();
    if(USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET)
    {
				if(UART2_USE_DMA == 0)
				{
					//收到的数据往缓冲区里面放
					if (uart2_recv_length < BUFFER_LEN)
					{
						Uart2_Rx[uart2_recv_length++] = serial2.ops->getc(&serial2);
					}
					//rt_hw_serial_isr(&serial2);
				}
        /* clear interrupt */
        USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);
    }
    if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET)
    {
        USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
    }
		
		if (USART_GetITStatus(uart->uart_device, USART_IT_IDLE) != RESET)
    {
				USART_ClearITPendingBit(uart->uart_device, USART_IT_IDLE);
    }
		
		if (USART_GetITStatus(uart->uart_device, USART_IT_CTS) != RESET)
    {
				USART_ClearITPendingBit(uart->uart_device, USART_IT_CTS);
    }
		
		if (USART_GetITStatus(uart->uart_device, USART_IT_LBD) != RESET)
    {  
				USART_ClearITPendingBit(uart->uart_device, USART_IT_LBD);
    }
		
		 if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)    //接收溢出中断
     {


       (u16)(USART2->DR & (u16)0x01FF);   
     }


    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* RT_USING_UART2 */

#if defined(RT_USING_UART3)
/* UART1 device driver structure */
struct serial_ringbuffer uart3_int_rx;
struct stm32_uart uart3 =
{
    USART3,
    USART3_IRQn,
};
struct rt_serial_device serial3;

void USART3_IRQHandler(void)
{
		unsigned int temp;
    struct stm32_uart* uart;

    uart = &uart3;

    /* enter interrupt */
    rt_interrupt_enter();
	
    if(USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET)
    {
				if(UART3_USE_DMA == 0)
				{
					rt_hw_serial_isr(&serial3);
				}
        USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);
    }
    if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET)
    {
        USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
    }
		
		if (USART_GetITStatus(uart->uart_device, USART_IT_IDLE) != RESET)
    {
				USART_ClearITPendingBit(uart->uart_device, USART_IT_IDLE);
    }
		
		if (USART_GetITStatus(uart->uart_device, USART_IT_CTS) != RESET)
    {
				USART_ClearITPendingBit(uart->uart_device, USART_IT_CTS);
    }
		
		if (USART_GetITStatus(uart->uart_device, USART_IT_LBD) != RESET)
    {  
				USART_ClearITPendingBit(uart->uart_device, USART_IT_LBD);
    }
		
		if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)    //接收溢出中断
     {


       (u16)(USART3->DR & (u16)0x01FF);   
     }
		
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* RT_USING_UART3 */

static void RCC_Configuration(void)
{
	
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	
#ifdef RT_USING_UART1
    /* Enable UART GPIO clocks */
    RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    /* Enable UART clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
#endif /* RT_USING_UART1 */

#ifdef RT_USING_UART2
    /* Enable UART GPIO clocks */
    RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    /* Enable UART clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
#endif /* RT_USING_UART2 */

#ifdef RT_USING_UART3
    /* Enable UART GPIO clocks */
    RCC_APB1PeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    /* Enable UART clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
#endif /* RT_USING_UART3 */
}

static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 

#ifdef RT_USING_UART1
	
		/* Connect PXx to USARTx_Tx */
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,GPIO_AF_USART1);//PA9-TX 
		/* Connect PXx to USARTx_Rx */
		//GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);//PA10-RX
	
    /* Configure USART Rx/tx PIN */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_Pin = UART1_GPIO_RX;
    GPIO_Init(UART1_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = UART1_GPIO_TX;
    GPIO_Init(UART1_GPIO, &GPIO_InitStructure);
	
	
#endif /* RT_USING_UART1 */

#ifdef RT_USING_UART2

		/* Connect PXx to USARTx_Tx */
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2,GPIO_AF_USART2);//PA2-TX 

		/* Connect PXx to USARTx_Rx */
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3,GPIO_AF_USART2);//PA3-RX

    /* Configure USART Rx/tx PIN */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = UART2_GPIO_RX;
    GPIO_Init(UART2_GPIO, &GPIO_InitStructure);

		
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = UART2_GPIO_TX;
    GPIO_Init(UART2_GPIO, &GPIO_InitStructure);
#endif /* RT_USING_UART2 */

#ifdef RT_USING_UART3




		/* Connect PXx to USARTx_Tx */
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource10,GPIO_AF_USART3);//PB10-TX 

		/* Connect PXx to USARTx_Rx */
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource11,GPIO_AF_USART3);//PB11-RX
		
		#ifdef ENABLE_CTSRTS
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource13,GPIO_AF_USART3);//PB11-RX cts
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource14,GPIO_AF_USART3);//PB11-RX
		#endif

    /* Configure USART Rx/tx PIN */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = UART3_GPIO_RX;
    GPIO_Init(UART3_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = UART3_GPIO_TX;
    GPIO_Init(UART3_GPIO, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = UART3_GPIO_RTS;
    GPIO_Init(UART3_GPIO, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = UART3_GPIO_CTS;
    GPIO_Init(UART3_GPIO, &GPIO_InitStructure);
		
#endif /* RT_USING_UART3 */
}

static void NVIC_Configuration(struct stm32_uart* uart)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = uart->irq;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void rt_hw_usart_init(void)
{
    struct stm32_uart* uart;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    RCC_Configuration();
    GPIO_Configuration();

#ifdef RT_USING_UART1
    uart = &uart1;
    config.baud_rate = BAUD_RATE_115200;

    serial1.ops    = &stm32_uart_ops;
    serial1.int_rx = &uart1_int_rx;
    serial1.config = config;

    NVIC_Configuration(&uart1);

    /* register UART1 device */
    rt_hw_serial_register(&serial1, "uart1",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_STREAM,
                          uart);
#endif /* RT_USING_UART1 */

#ifdef RT_USING_UART2
    uart = &uart2;

    config.baud_rate = BAUD_RATE_9600;

    serial2.ops    = &stm32_uart_ops;
    serial2.int_rx = &uart2_int_rx;
    serial2.config = config;

    NVIC_Configuration(&uart2);

    /* register UART1 device */
    rt_hw_serial_register(&serial2, "uart2",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                          uart);
#endif /* RT_USING_UART2 */

#ifdef RT_USING_UART3
    uart = &uart3;
		
    config.baud_rate = BAUD_RATE_115200;

    serial3.ops    = &stm32_uart_ops;
    serial3.int_rx = &uart3_int_rx;
    serial3.config = config;

    NVIC_Configuration(&uart3);

    /* register UART1 device */
    rt_hw_serial_register(&serial3, "uart3",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                          uart);
#endif /* RT_USING_UART3 */
}
