/* Auth: Megan Bird
 * File: main.c
 * Course: CEEN-3250 – Communications Systems – University of Nebraska-Lincoln
 * Lab: XBee Project
 * Date: 1/18/2017
 * Desc: XBee CEENBoT program to communicate temperature between 2 bots
 */

#include "capi324v221.h"

// ======================= prototypes =========================== //

BOOL CRUISE( void );
BOOL IR_AVOID( void );
float getTemp( void );
float receiveTemp( float currentTemp );
float transmitTemp( void );
void updateDisplay( float myTemp, float rxTemp );

typedef enum { false, true } bool;

// ======================== main ================================ //

void CBOT_main( void )
{
	BOOL CRUISING = false;
	
	float myTemp = 0.0;
	float rxTemp = 0.0;
	
	// Open the LCD and Motors
	LCD_open();
	STEPPER_open();
	
	// ADC open and setup
	ADC_open();
	ADC_set_VREF( ADC_VREF_AVCC );		// Set the voltage reference (we want 5V reference).
	
	// Open and initialize the UART
	UART_open( UART_UART1 );
	UART_configure( UART_UART1, UART_8DBITS, UART_1SBIT, UART_NO_PARITY, 9600 );
	UART_set_TX_state( UART_UART1, UART_ENABLE );
	UART_set_RX_state( UART_UART1, UART_ENABLE );
	
	while (1)
	{
		// cruise if not already cruising
		//if ( !CRUISING )
		{
			//CRUISING = CRUISE();
		}
		
		// check for obstacles
		//CRUISING = IR_AVOID();
		
		// transmit
		myTemp = transmitTemp();
		DELAY_ms( 1000 );
		updateDisplay( myTemp, rxTemp );
		
		// receive
		rxTemp = receiveTemp( rxTemp );
		updateDisplay( myTemp, rxTemp );
		
		DELAY_ms( 500 );
		
	}; // Loop forever.
	
} // end CBOT_main()

// ======================= functions ============================ //

BOOL CRUISE( void )
{	
	STEPPER_move_rn( STEPPER_BOTH,
	STEPPER_FWD, 150, 450, // Left
	STEPPER_FWD, 150, 450 ); // Right
	
	return true;
} // end CRUISE()

// -------------------------------------------------------------- //

BOOL IR_AVOID( void )
{
	BOOL cruising = true;
	
	// If BOTH sensors are triggered then move back & 180-degrees.
	if ( ATTINY_get_IR_state( ATTINY_IR_LEFT ) == TRUE
	&& ATTINY_get_IR_state( ATTINY_IR_RIGHT ) == TRUE )
	{
		cruising = false;
		
		// Backup...
		STEPPER_move_stwt( STEPPER_BOTH,
		STEPPER_REV, 100, 200, 400, STEPPER_BRK_OFF, // Left
		STEPPER_REV, 100, 200, 400, STEPPER_BRK_OFF ); // Right
		
		// Turn 180...
		STEPPER_move_stwt( STEPPER_BOTH,
		STEPPER_FWD, 300, 200, 400, STEPPER_BRK_OFF, // Left
		STEPPER_REV, 300, 200, 400, STEPPER_BRK_OFF ); // Right
	}
	
	// If LEFT sensor is triggered then move RIGHT 90-degrees.
	else if ( ATTINY_get_IR_state( ATTINY_IR_LEFT ) == TRUE )
	{
		cruising = false;
		
		// Backup...
		STEPPER_move_stwt( STEPPER_BOTH,
		STEPPER_REV, 100, 200, 400, STEPPER_BRK_OFF, // Left
		STEPPER_REV, 100, 200, 400, STEPPER_BRK_OFF ); // Right
		
		// Turn LEFT...
		STEPPER_move_stwt( STEPPER_BOTH,
		STEPPER_FWD, 150, 200, 400, STEPPER_BRK_OFF, // Left
		STEPPER_REV, 150, 200, 400, STEPPER_BRK_OFF ); // Right
	}
	
	// Otherwise, if the RIGHT sensor is triggered, then move LEFT 90-degrees.
	else if ( ATTINY_get_IR_state( ATTINY_IR_RIGHT ) == TRUE )
	{
		cruising = false;
		
		// Backup...
		STEPPER_move_stwt( STEPPER_BOTH,
		STEPPER_REV, 100, 200, 400, STEPPER_BRK_OFF, // Left
		STEPPER_REV, 100, 200, 400, STEPPER_BRK_OFF ); // Right
		
		// Turn RIGHT...
		STEPPER_move_stwt( STEPPER_BOTH,
		STEPPER_REV, 150, 200, 400, STEPPER_BRK_OFF, // Left
		STEPPER_FWD, 150, 200, 400, STEPPER_BRK_OFF ); // Right
	}
	
	return cruising;
} // end IR_AVOID()

// -------------------------------------------------------------- //

float getTemp( void ) {
	
	ADC_SAMPLE sample; // Storage for ADC code.
	float temp; // Storage for 'voltage' representation of ADC code.
	
	ADC_set_channel( ADC_CHAN3 );				// Set the channel we will sample from.
	sample = ADC_sample();						// Read sensor value 1
	temp = ( ( sample * 5.0f ) / 1024 );		// Convert it to meaningful value.
	
	return temp;
	
} // end getTemp()

// -------------------------------------------------------------- //

float receiveTemp( float currentTemp ) {
	
	float temperature = currentTemp;
	unsigned int bytes_received = 0; // Bytes in.
	unsigned char temp; // Store a single byte.
	unsigned char buffer[ 10 ]; // Store multiple bytes.
	
	// First check IF there is data to read...
	if ( UART_has_data( UART_UART1 ) == TRUE ) {
		
		// Keep waiting until all 4 bytes have been received.
		do {
			if ( UART_has_data( UART_UART1 ) == TRUE ) {
				UART_receive( UART_UART1, &temp ); // Read it...
				buffer[ bytes_received ] = temp; // Store it...
				bytes_received++; // Increment byte count.
			} // end if()
		} while( bytes_received < 4 );
		
		temperature = *(float *)&buffer;		
	}
	
	return temperature;
	
} // end receiveTemp()

// -------------------------------------------------------------- //

float transmitTemp( void ) {
	
	// Get temperature on device
	float temperature = getTemp();
	
	// convert to bytes
	unsigned char *chptr;
	chptr = (unsigned char *)&temperature;
	
	// Send data
	for ( int i = 0; i < 4; i++) {
		//LCD_printf( "%d ", *chptr );
		UART_transmit( UART_UART1, *chptr++);
	}
	
	return temperature;
}

// -------------------------------------------------------------- //

void updateDisplay( float myTemp, float rxTemp ) {
	
	LCD_clear();
	
	// Display myTemp
	LCD_printf( "My Temp: %.2f\n", myTemp );
	
	// Display rxTemp
	LCD_printf( "Rx Temp: %.2f\n", rxTemp );
	
} // end updateDisplay()