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
uint16_t adc_value;
uint8_t percentage;
uint8_t task_10ms =FALSE, task_100ms=FALSE, task_500ms=FALSE;
uint16_t timer_cnt=0;
uint8_t gyerek_zar;
uint16_t poz1;
uint16_t poz2;
uint8_t hiba;
uint8_t switch_value=0;
uint8_t PA0_cnt = 0, PA1_cnt = 0;
uint8_t PA0_pushed = 0, PA1_pushed = 0, PB0_pushed = 0;

/******************************************************************************
* External Variables
******************************************************************************/
/*
void timer0_init(void)
{
	TCCR0A = (0<<WGM00) | (1<<WGM01) | (0<<CS02) | (1<<CS01) | (1<<CS00);
	OCR0A = 124;
	TIMSK0 = (1<<OCIE0A);
}

void timer2_init(void)
{
	TCCR2A = (1<<WGM20) | (0<<WGM21) | (0<<CS22) | (1<<CS21) | (0<<CS20);
	OCR2A = STOP; 
}
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

void adc_init(void);

/******************************************************************************
* Function:         int main(void)
* Description:      main function
* Input:
* Output:
* Notes:
******************************************************************************/
uint16_t adc_read()
{
	ADCSRA |= (1 << ADSC);               
	while (ADCSRA & (1 << ADSC));        
	return ADC;                         
}

/******************************************************************************
* Function:         int main(void)
* Description:      main function
* Input:
* Output:
* Notes:
******************************************************************************/
uint8_t convert_to_percentage(uint16_t adc_value)
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
	if(switch_value==0)
	{
		poz1 = percentage;
		switch_value=1;
	}
	else
	{
		poz2 = percentage;
		switch_value=0;
	}
		
	if((FEL<<PD7))
	{
		if(poz1-poz2 >=0) hiba = TRUE; else hiba = FALSE;
	}
	if((LE<<PD7))
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
	
	DDRB = (1<<PB4); //PB4 kimenet, ezen jön a pwm jel
	PORTB = (1<<PB1) | (1<<PB0);									// PB0 - gyerkzár  PB1 - egyelőre nem kell, ha így marad akkor ki kell szedni innen.
	
	DDRC = (1<<LCD_E) | (1<<LCD_RS) | (1<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4);
	PORTC = (0<<LCD_E) | (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);

	
	DDRD = 0x00;
	PORTD = (1<<PD7); // az ablak irányát jelző bit
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
		//adc_init();
		timer0_init();
		timer2_init();
		lcd_init();
		
		sei();
		
	
		while (1)
		{
			if(task_10ms == TRUE)
			{
					if(gyerek_zar == FALSE)
					{
						if((PINA & (1<<PA0)) == 0 && (PA0_pushed == 0)) //ablak fel
						{
							PA0_cnt++;
							PA0_pushed = 1;
							if(PA0_cnt >=30) // 3 sec nyomás után auto fel
							{
								if(percentage < 100)
								{
								
									(FEL<<PD7);
									OCR2A = 170;						
								}
								else // végállásban pwm 0
								{
									OCR2A = STOP;
								}
							}
							else //kézi fel
							{
								if(percentage < 100)
								{
									(FEL<<PD7);
									OCR2A = 170;
								}
								else //végállásban pwm 0
								{
									OCR2A = STOP;
								}				
							}			
						}
						else if(PA0_pushed == 1) // gomb felenged
						{
							PA0_pushed = 0; 
							PA0_cnt = 0; //auto fel cnt nulláz
							OCR2A = STOP;
						}
				
								
						if((PINA & (1<<PA1)) == 0 && (PA1_pushed == 0)) //ablak le
						{
							PA1_cnt++;
							PA1_pushed = 1;
							if(PA1_cnt >=30) // 3 sec nyomás után auto le
							{
								if(percentage > 0)
								{
									
									(LE<<PD7);
									OCR2A = 170;
								}
								else // végállásban pwm 0
								{
									OCR2A = STOP;
								}
							}
							else //kézi le
							{
								if(percentage > 0)
								{
									(LE<<PD7);
									OCR2A = 170;
								}
								else //végállásban pwm 0
								{
									OCR2A = STOP;
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
							gyerek_zar = TRUE;
							PB0_pushed = 1;
						}
						if((PINB & (1<<PB0)) == 0 && (PB0_pushed == 1)) // gyerek zár ki
						{
							gyerek_zar = FALSE;
							PB0_pushed = 0;
						}
				
				
					task_10ms = FALSE;
			}
				
			if(task_100ms == TRUE)
			{
				//if(hiba == FALSE)
				//{
					//
					//char write_string[50];
					//adc_value = adc_read();
					//percentage = convert_to_percentage(adc_value);
					//lcd_clear_display();
					//sprintf(write_string,"%3d",adc_value);
					//lcd_set_cursor_position(0);
					//lcd_write_string(write_string);
				//}
				//else
				//{
					//char write_string[50];
					//sprintf(write_string,"HIBAÜZENET");
					//lcd_set_cursor_position(0);
					//lcd_write_string(write_string);
				//}				
					PORTE ^=0x01;
				task_100ms = FALSE;
			}
			
			if(task_500ms == TRUE)
			{
				
				char write_string[50];
				adc_value = adc_read();
				percentage = convert_to_percentage(adc_value);
			
				sprintf(write_string,"%3d",354);
				lcd_set_cursor_position(0);
				lcd_write_string(write_string);
				hiba_csekk();
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
	if((timer_cnt % 10) == 0) task_10ms = TRUE;
	if((timer_cnt % 100) == 0) task_100ms =TRUE;
	if((timer_cnt % 500) == 0) task_500ms =TRUE;
}