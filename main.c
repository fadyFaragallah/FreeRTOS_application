#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "pll.h"
#include "UART.h"
#include "esp8266.h"
#include "C:\Keil\ARM\INC\TI\TM4C123\TM4C123GH6PM.h"
#define BUFF_SIZE 1024
void SystemInit(void){SCB->CPACR |= 0x00F00000;}
 uint32_t thresholdTemp;
 uint32_t thresholdTime;
char Buffer[BUFF_SIZE];
uint32_t SystemCoreClock = 16000000;
 uint32_t temp;
	QueueHandle_t tempQ;
	TaskHandle_t readTempT;
	TaskHandle_t uploadTempT;
	TaskHandle_t serverListenT;
char post[] = "GET /update?api_key=2UMGTZYTFHWSLH7A2UM&field1=%d HTTP/1.1\r\nHost:api.thingspeak.comr\n\r\n";
char postTest[]="GET /t/7ndd4-1531841440/post?&field1=%d HTTP/1.1\r\nHost:ptsv2.comr\n\r\n";
char post1[]="POST /index.php HTTP/1.1\r\nHost:iottest.atwebpages.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 20\r\n\r\temp=%d\r\n\r\n";
//char fetch []="GET /channels/540679/fields/1.json?results=2 HTTP/1.1\r\nHost:api.thingspeak.com\r\n\r\n";
char fetch[] = "GET /update?key=71T5BO40F1HS3U81&field1=%d\r\nHTTP/1.1\r\nHost:api.thingspeak.com\r\n\r\nConnection: close\r\n";
//char post1[]="GET /update?key=2UMGTZYTFHWSLH7A2UM&field1=%d& HTTP/1.1\r\nHost:api.thingspeak.com\r\n\r\n";
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital I/O on PF4-0
}
void vTaskReadTemp(void * pv)
	{
		while(1) {

		temp=ADC0_readChannel();//rea-d adc value
		temp=(temp*1.22*0.66)/(10);			//convert it to temperature
		if(xQueueSendToFront(tempQ,&temp,0)== pdTRUE)//check if the task has successfuly write to the queue
		{
			//for testing i'm going to turn the red led on
			
//			GPIO_PORTF_DATA_R |=(1<<1);//red on
//			GPIO_PORTF_DATA_R &=~(1<<2);
//			GPIO_PORTF_DATA_R &=~(1<<3);
			
			
			//vTaskDelay(150);
		taskYIELD();
		
//	**for testing		
//		if(temp>=28 && temp<=35)
//		{
//			GPIO_PORTF_DATA_R |=(1<<1);
//			GPIO_PORTF_DATA_R &=~(1<<2);
//			GPIO_PORTF_DATA_R &=~(1<<3);
//		}
//		else if(temp==0)
//		{
//			GPIO_PORTF_DATA_R &=~(1<<1);
//			GPIO_PORTF_DATA_R |=(1<<2);
//			GPIO_PORTF_DATA_R &=~(1<<3);
//		}
//		else
//		{
//		GPIO_PORTF_DATA_R |=(1<<3);
//			GPIO_PORTF_DATA_R &=~(1<<1);
//			GPIO_PORTF_DATA_R &=~(1<<2);
//		}
//  **for testing	

		
	}
		
}
	}
	
	void vTaskUploadTemp(void *pv)
	{
		const portTickType ticksToWait=100/portTICK_RATE_MS;
		portBASE_TYPE status;
		uint32_t tempValue;
		
		while(1){
		

		ESP8266_MakeTCPConnection("api.thingspeak.com");
			

			status=xQueueReceive(tempQ,&tempValue,ticksToWait);
			if(status == pdTRUE)
				{
				//upload this value to the cloud

									sprintf((char*)Buffer, fetch,tempValue );
								//	vTaskDelay(1000);
					
									//printf(Buffer);
									
									if(tempValue >= thresholdTemp)
									{
										
									DelayMs((thresholdTime));//bust wait
										printf("it's too hot now , Alarm ON\n\r");
									GPIO_PORTF_DATA_R |=(1<<1);//red on
										vTaskDelay(2000);//keep the led on for 2 seconds
									}
									else if (tempValue<thresholdTemp){
										GPIO_PORTF_DATA_R &=~(1<<1);//red off
									}
									
									/*sending data to the cloud */
										ESP8266_SendTCP(Buffer);
									DelayMs(2000);
							ESP8266_CloseTCPConnection();
									vTaskDelay(20000);
						//for testing i'm going to just turn the green led on 
//						GPIO_PORTF_DATA_R |=(1<<3);//turn on
//						GPIO_PORTF_DATA_R &=~(1<<1);//turn off
//						GPIO_PORTF_DATA_R &=~(1<<2);//turn off
				}
			
		}
	}
	
	
	void vTaskEspConnect(void * pv)
	{

		while(1)	{
		ESP8266_Init(115200);      // connect to access point, set up as client
  ESP8266_GetVersionNumber();
	vTaskSuspend(NULL);
	
		}
	}
	void vTaskSystemStart(void *pv)
	{
		while(1){
		printf("System is starting...\n\r");
		printf("Welcome to the most amazing task ever:)\n\r");
		printf("please enter the threshold temperature value in the range 0 > 150 C :\n\r");
			
				thresholdTemp=UART_InUDec();
			
				printf("now ,please enter the threshold time in mSeconds :\n\r");
				thresholdTime=UART_InUDec();
			
	
		printf("Ok we will take care of this ,now relax....\n\r");
		vTaskSuspend(NULL);
		}
		
	}
int main()
{
  DisableInterrupts();
  PLL_Init(Bus80MHz); 
  Output_Init();  
	ADC0_InitSWTrigger();//initialize adc to read from PE2
	PortF_Init();
	xTaskCreate(vTaskReadTemp,"Reading Temperature",80,NULL,1,&readTempT);
	xTaskCreate(vTaskUploadTemp,"uploading to the cloud",80,NULL,2,&uploadTempT);
	xTaskCreate(vTaskEspConnect,"ESP initialize",80,NULL,4,&serverListenT);
	xTaskCreate(vTaskSystemStart,"System is starting",80,NULL,3,NULL);
	tempQ=xQueueCreate(1,sizeof(unsigned long));
	
	vTaskStartScheduler();
	
	
}
