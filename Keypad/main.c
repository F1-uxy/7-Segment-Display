#include <stdio.h>
#include <string.h>
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_gpio.h"
#include "keypad.h"


#define SEGMENT_COUNT 8

// Pin Declarations
#define SEGMENT_A_PIN GPIO_PIN_7
#define SEGMENT_B_PIN GPIO_PIN_6
#define SEGMENT_C_PIN GPIO_PIN_6
#define SEGMENT_D_PIN GPIO_PIN_4
#define SEGMENT_E_PIN GPIO_PIN_7
#define SEGMENT_F_PIN GPIO_PIN_0
#define SEGMENT_G_PIN GPIO_PIN_6
#define SEGMENT_DP_PIN GPIO_PIN_3

/* As common cathode 7 seg display RESET = SEGON && SET = SEGOFF */
#define PIN_SET GPIO_PIN_RESET
#define PIN_RESET GPIO_PIN_SET


uint64_t i=0;
GPIO_InitTypeDef segA, segB, segC, segD, segE, segF, segG, segDP;	
GPIO_InitTypeDef *segments[]={&segA, &segB, &segC, &segD, &segE, &segF, &segG, &segDP};
		
void initialisePins(void)
{
		/*---- SEGMENTS ----
			segA = D0 = C7
			segB = D1 = C6
			segC = D2 = G6
			segD = D3 = B4
			segE = D4 = G7
			segF = D5 = I0
			segG = D6 = H6
			segDP = D7 = I3
		  ------------------*/
	
		// Enabling all the required ports clocks
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOG_CLK_ENABLE();
		__HAL_RCC_GPIOH_CLK_ENABLE();
		__HAL_RCC_GPIOI_CLK_ENABLE();
		
		
		// Configuring each pin with Mode, Pull and Speed
		for(i = 0; i < SEGMENT_COUNT; i++)
		{		
				segments[i]->Mode = GPIO_MODE_OUTPUT_PP;
				segments[i]->Pull = GPIO_NOPULL;
				segments[i]->Speed = GPIO_SPEED_HIGH;
		}
		
		// Assigning each segment their pin
		segments[0]->Pin = SEGMENT_A_PIN;
		segments[1]->Pin = SEGMENT_B_PIN;
		segments[2]->Pin = SEGMENT_C_PIN;
		segments[3]->Pin = SEGMENT_D_PIN;
		segments[4]->Pin = SEGMENT_E_PIN;
		segments[5]->Pin = SEGMENT_F_PIN;
		segments[6]->Pin = SEGMENT_G_PIN;
		segments[7]->Pin = SEGMENT_DP_PIN;
		
}

void segChange(char seg, GPIO_PinState state )
{
	
		GPIO_TypeDef *port;
		uint16_t pin;
	
		// Assigning port and pin variables
		
		switch(seg)
		{
				case('a'):
						port = GPIOC;
						pin = segments[0]->Pin;
						break;
			
				case('b'):
						port = GPIOC;
						pin = segments[1]->Pin;
						break;
				
				case('c'):
						port = GPIOG;
						pin = segments[2]->Pin;
						break;
				
				case('d'):
						port = GPIOB;
						pin = segments[3]->Pin;
						break;
				
				case('e'):
						port = GPIOG;
						pin = segments[4]->Pin;
						break;
			
				case('f'):
						port = GPIOI;
						pin = segments[5]->Pin;
						break;
				
				case('g'):
						port = GPIOH;
						pin = segments[6]->Pin;
						break;
				
				case('p'):
						port = GPIOI;
						pin = segments[7]->Pin;
						break;
			}
			
		
			// Writing the state to the correct GPIO pin
			HAL_GPIO_Init(port, segments[seg - 'a']); // Minus 'a' ascii value from input seg ascii value to get index
			HAL_GPIO_WritePin(port, pin, state);
			
			return;
	
}

void dispNum(int number)
{
		
		// Array of each number and characters segments to turn on
	
		const char *numberConfigs[] = {
			
				"abcdef",   // 0
				"bc",       // 1
				"abdeg",    // 2
				"abcdg",    // 3
				"bcfg",     // 4
				"acdfg",    // 5
				"acdefg",   // 6
				"abc",			// 7
				"abcdefg",  // 8
				"abcfg",		// 9
				"abcefg",		// 10/A
				"cdefg",		// 11/B
				"adef",			// 12/C
				"bcdeg"			// 13/D
    };
	
		for(i = 0; i < SEGMENT_COUNT; i++)
		{
				
				if(strchr(numberConfigs[number], 'a' + i) != NULL) // IF seg char is in numconfig then turn the seg on
				{
						segChange('a' + i, PIN_SET);
				}
		
		}
			
		return;
		
}

// Resetting all pins
void resetPins(void)
{
	
		for(i = 0; i < SEGMENT_COUNT; i++)
		{
					segChange('a' + i, PIN_RESET);
		}
		
		return;
	
}

// Empty for loop to delay given amount
void wait(int delay)
{
	
		for(i = 0; i < delay; i++){};
	
		return;

}


int main(void)
{
		int8_t membraneNum; //Signed Byte to store input
		// Setting up 7Seg and Keypad
		initialisePins();
		initializeMembranePins();

		while(1)
		{
			membraneNum = getInput();
				
			if(membraneNum != -1)
			{
					dispNum(membraneNum); // Changed keypad.c to correct A = 10, B = 11 etc...
					wait(1000000);
				  resetPins();
			}
		}

}
