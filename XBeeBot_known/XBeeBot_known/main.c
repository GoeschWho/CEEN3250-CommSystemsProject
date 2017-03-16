#include "capi324v221.h"
void CBOT_main( void )
{
	LCD_open();
	LCD_printf("hi ave");
	ADC_SAMPLE sample; // Storage for ADC code.
	unsigned char A = 0;
	unsigned char B = 0;
	unsigned char C = 0;
	
	// ADC
	ADC_open();
	ADC_set_VREF( ADC_VREF_AVCC );
	ADC_set_channel( ADC_CHAN3 );

	//Serial
	UART_open(UART_UART1);
	UART_configure(UART_UART1, UART_8DBITS, UART_1SBIT, UART_NO_PARITY, 9600);
	UART_set_timeout(UART_UART1, 1);
	UART_set_TX_state(UART_UART1, UART_ENABLE);
	UART_set_RX_state(UART_UART1, UART_ENABLE);
	

	//Steppers
	STEPPER_open();


	//LED
	SBD(A, 7, OUTPIN);


	while(1)
	{
		//STEPPER_move(STEPPER_STEP_BLOCK, STEPPER_BOTH,
		//STEPPER_FWD, 100, 200, 400, STEPPER_BRK_OFF, NULL,
		//STEPPER_FWD, 100, 200, 400, STEPPER_BRK_OFF, NULL);

		sample = ADC_sample();
		A = sample * .154;
		UART1_transmit(A);

		C = B;
		while(UART1_has_data())
		{
			UART1_receive(&B);
		}

		if ( B > 100)
		{
			B = C;
		}

		LCD_clear();
		LCD_printf_RC(3,0, "Big Apple Macintosh: %d", A);
		LCD_printf_RC(1,0,"Miley: %d",B);
		
		
		DELAY_ms(100);
		CBV(7, PORTA);

		//STEPPER_move(STEPPER_STEP_BLOCK, STEPPER_BOTH,
		//STEPPER_REV, 100, 200, 400, STEPPER_BRK_OFF, NULL,
		//STEPPER_REV, 100, 200, 400, STEPPER_BRK_OFF, NULL);

		DELAY_ms(100);
		SBV(7, PORTA);
	}

} // end CBOT_main()