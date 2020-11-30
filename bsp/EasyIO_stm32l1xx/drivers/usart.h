/*
 * File      : usart.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

#ifndef __USART_H__
#define __USART_H__

#include <rthw.h>
#include <rtthread.h>

#define UART_ENABLE_IRQ(n)            NVIC_EnableIRQ((n))
#define UART_DISABLE_IRQ(n)           NVIC_DisableIRQ((n))

#define UART_RX_LEN 2048
unsigned char Uart_Rx[UART_RX_LEN];

void rt_hw_usart_init(void);
void reinit_system_serial(void);
void reinit_uart3_baud(unsigned int baud);

extern unsigned char UART3_USE_DMA_FLAG;
extern unsigned char UART2_USE_DMA_FLAG;

#endif
