/*
 * Ablakemelo.c
 *
 * Created: 2024. 10. 30. 15:07:26
 * Author : Bürger Boldizsár, Takács Imre, Kónyi Árpád Benjámin
 */ 

/******************************************************************************
 * Created: 
 * Author :
******************************************************************************/
/******************************************************************************
* Include files
******************************************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "can.h"
#include "lcd.h"
#include "peripherals.h"

#define F_CPU 8000000UL
/******************************************************************************
* Macros
******************************************************************************/

#define TRUE 1
#define FALSE 0

#define FEL 1
#define LE 0
#define STOP 0 

/******************************************************************************
* Constants
******************************************************************************/


/******************************************************************************
* Global Variables
******************************************************************************/
uint32_t adc_value;
uint8_t percentage;
uint8_t task_10ms =FALSE, task_100ms=FALSE, task_500ms=FALSE;
uint16_t timer_cnt=0;
uint8_t gyerek_zar;
uint16_t poz1;
uint16_t poz2;
uint8_t hiba;
uint8_t switch_value=0;
uint16_t PA0_cnt = 0, PA1_cnt = 0;
uint8_t PA0_pushed = 0, PA1_pushed = 0, PB0_pushed = 0;
uint8_t irany = 0, auto_fel=0;
uint16_t d_poz=0;
uint8_t cnt_4bit=0;
/******************************************************************************
* External Variables
******************************************************************************/

/******************************************************************************
* Local Function Declarations
******************************************************************************/
/******************************************************************************
* Function:         void ADC_init()
* Description:      
* Input:
* Output:
* Notes:
******************************************************************************/

/******************************************************************************
* Function:         int main(void)
* Description:      main function
* Input:
* Output:
* Notes:
******************************************************************************/

/******************************************************************************
* Function:         int main(void)
* Description:      main function
* Input:
* Output:
* Notes:
******************************************************************************/
uint32_t convert_to_percentage(uint32_t adc_value)
{
	return (adc_value * 100) / 1023;
}

/******************************************************************************
* Function:         int main(void)
* Description:      main function
* Input:
* Output:
* Notes:
******************************************************************************/					
					

/******************************************************************************
* Function:         int main(void)
* Description:      main function
* Input:
* Output:
* Notes:
******************************************************************************/


void hiba_csekk(hiba)
{
	//if(switch_value==0)
	//{
		//poz1 = percentage;
		//switch_value=1;
	//}
	//else
	//{
		//poz2 = percentage;
		//switch_value=0;
	//}
	//
	d_poz = poz1-percentage;
	poz1=percentage;
	
	if(irany == 1)
	{
		if(poz1-poz2 >=0) hiba = TRUE; else hiba = FALSE;
	}
	if(irany == 2)
	{
		if(poz1-poz2 <=0) hiba = TRUE; else hiba = FALSE;
	}
}
/******************************************************************************
* Function:         int main(void)
* Description:      main function
* Input:
* Output:
* Notes:
******************************************************************************/
void port_init()
{
	DDRA = 0x00; //A port minden bitje kimenet
	PORTA = (1<<PA1) | (1<<PA0); // PA1 és PA0 magas jelszint			PA0-FEL	PA1-LE
	
	DDRB = (1<<PB7) | (1<<PB4); //PB7 kimenet, ezen jön a pwm jel
	PORTB = (1<<PB1) | (1<<PB0);									// PB0 - gyerkzár  PB1 - egyelõre nem kell, ha így marad akkor ki kell szedni innen.
	
	DDRC = (1<<LCD_E) | (1<<LCD_RS) | (1<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4);
	PORTC = (0<<LCD_E) | (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);

	
	DDRD = 0x01;
	PORTD = (1<<PD7); // az ablak irányát jelzõ bit
	DDRE = 0xFF;
}


/******************************************************************************
* Local Function Definitions
******************************************************************************/

/******************************************************************************
* Function:         int main(void)
* Description:      main function
* Input:            
* Output:           
* Notes:            
******************************************************************************/
int main(void)


{
		port_init();
		adc_init();
		timer0_init();
		timer2_init();
		lcd_init();
		can_init();
		
		sei();
		
	
		while (1)
		{
			if(task_10ms == TRUE)
			{
					if(gyerek_zar == FALSE)
					{
						if(percentage < 100)
						{	
							OCR2A = 170;	//sebesség vizsgálat
						}
						else // végállásban pwm 0
						{
							OCR2A = STOP;
							//irany = 0;
							auto_fel=0;
						}
						
						if(auto_fel==1)
						{
							OCR2A = 170;
						}
						if((PINA & (1<<PA0)) == 0 && (PA0_pushed == 0)) //ablak fel
						{
							//irany = 1;
							PA0_cnt++;
							OCR2A = 170;
							//PA0_pushed = 1;
							if(PA0_cnt >=300) // 3 sec nyomás után auto fel
							{
								auto_fel=1;
							}
						}
						if((PINA & (1<<PA0)) == 1) 
						{
							PA0_cnt=0;
							if(auto_fel==0) OCR2A = STOP;
						}
				
								
						if((PINA & (1<<PA1)) == 0 && (PA1_pushed == 0)) //ablak le
						{
							irany = 2;
							PA1_cnt++;
							PA1_pushed = 1;
							if(PA1_cnt >=30) // 3 sec nyomás után auto le
							{
								if(percentage > 0)
								{
									
									//(LE<<PD7);
									OCR2A = 170;
								}
								else // végállásban pwm 0
								{
									OCR2A = STOP;
									irany = 0;
								}
							}
							else //kézi le
							{
								if(percentage > 0)
								{
									//(LE<<PD7);
									OCR2A = 170;
								}
								else //végállásban pwm 0
								{
									OCR2A = STOP;
									irany  =0;
								}
							}
						}
						else if(PA1_pushed == 1) // gomb felenged
						{
							PA1_pushed = 0;
							PA1_cnt = 0; //auto le cnt nulláz
							OCR2A = STOP;
						}
					}
			
						if((PINB & (1<<PB0)) == 0 && (PB0_pushed == 0)) // gyerek zár be
						{
							if(gyerek_zar == TRUE) gyerek_zar = FALSE; else gyerek_zar = TRUE; 
							PB0_pushed = 1;
						}
						
						if((PINB & (1<<PB0)) == 1 && (PB0_pushed == 1)) PB0_pushed = 0;

				
				
					task_10ms = FALSE;
			}
				
			if(task_100ms == TRUE)
			{
				if(gyerek_zar == FALSE)
				{
				ADCSRA = ADCSRA | (1<<ADSC);
				}
				cnt_4bit++;
				if (cnt_4bit>=15)
				{
					cnt_4bit=0;
				}
				
					
					PORTE ^=0x01;
					
				char write_string[50];
				percentage = convert_to_percentage(adc_value);
				lcd_clear_display();
				sprintf(write_string,"%4d %d %d",percentage, OCR2A,d_poz);
				lcd_set_cursor_position(0);
				lcd_write_string(write_string);
				hiba_csekk();
				uint8_t tx_length=3;
				
				uint8_t can_tx_data[tx_length];
				//gyerek_zar=1;
				hiba=1;
				can_tx_data[0]=poz1  | gyerek_zar<<7;
				can_tx_data[1]=cnt_4bit;
				
				uint8_t can_tx_data2;
				can_tx_data2=hiba;
				
				//CAN_SendMob(1,0x1FFFFFFF,TRUE,2,can_tx_data);
				CAN_SendMob(1,0x100,FALSE,1,&can_tx_data2);
				CAN_SendMob(2,0x55,FALSE,2,&can_tx_data);
				
				
				
				task_100ms = FALSE;
			}
			
			if(task_500ms == TRUE)
			{
				
					PORTD ^=0x01;
				
				
				
				task_500ms = FALSE;
			}
			
		}
	
}

/******************************************************************************
*Interrupt Routines
******************************************************************************/
ISR(TIMER0_COMP_vect)
{
	timer_cnt++;
	if((timer_cnt % 1) == 0) task_10ms = TRUE;
	if((timer_cnt % 10) == 0) task_100ms =TRUE;
	if((timer_cnt % 50) == 0) task_500ms =TRUE;
}

ISR(ADC_vect)
{
	adc_value = ADC;
}