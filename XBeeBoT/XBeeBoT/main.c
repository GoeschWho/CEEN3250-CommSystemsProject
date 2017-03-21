/* Auth: Megan Bird
 * File: main.c
 * Course: CEEN-3250 – Communications Systems – University of Nebraska-Lincoln
 * Lab: XBee Project
 * Date: 1/18/2017
 * Desc: XBee CEENBoT program to communicate temperature between 2 bots
 */

#include "capi324v221.h"
#include "stdbool.h"

// ======================= prototypes =========================== //

typedef unsigned char byte;

BOOL CRUISE( void );
BOOL IR_AVOID( void );
byte getTemp( void );
byte receiveTemp( byte currentTemp );
byte transmitTemp( void );
void updateDisplay( byte myTemp, byte rxTemp );

// ======================== main ================================ //

void CBOT_main( void )
{
	bool CRUISING = false;
	
	byte myTemp = 0;
	byte rxTemp = 0;
	
	// Open the LCD, motors, & LEDs
	LCD_open();
	LCD_printf("Starting...");
	STEPPER_open();
	LED_open();
	
	// ADC open and setup
	ADC_open();
	ADC_set_VREF( ADC_VREF_AVCC );		// Setmk the voltage reference (we want 5V reference).
	ADC_set_channel( ADC_CHAN3 );		// Set the channel we will sample from.
	
	// Open and initialize the UART
	UART_open( UART_UART1 );
	UART_configure( UART_UART1, UART_8DBITS, UART_1SBIT, UART_NO_PARITY, 9600 );
	UART_set_timeout(UART_UART1, 1);
	UART_set_TX_state( UART_UART1, UART_ENABLE );
	UART_set_RX_state( UART_UART1, UART_ENABLE );
	
	while (1)
	{
		 // Cruise if not already cruising
		 if ( !CRUISING )
		{
			CRUISING = CRUISE();
		}
		
		// Check for Obstacles
		CRUISING = IR_AVOID();
		
		// Transmit Temperature
		myTemp = transmitTemp();
		
		// Receive Temperature
		rxTemp = receiveTemp( rxTemp );
		
		// Update Display
		updateDisplay( myTemp, rxTemp );
		
		DELAY_ms( 100 );
		
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

byte getTemp( void ) {
	
	ADC_SAMPLE sample;	// Storage for ADC code
	float volts;		// Volts on temperature sensor pin
	float tempC;
	float tempF;
	byte temp;
	
	sample = ADC_sample();					// Read sensor value
	volts = sample * ( 5.0 / 1024 );			// Convert to a voltage
	tempC = ( volts - 0.5 ) * 100.0;		// Calculate Celsius temperature
	tempF = tempC * ( 9.0 / 5.0 ) + 32.0;	// Calculate Fahrenheit temperature
	temp = ( byte ) ( tempF + 0.5 );		// Convert it to a byte
	
	// --- test --- //
	//LCD_clear();
	//LCD_printf("ADC:    %d\n", sample);
	//LCD_printf("Volts:  %f\n", volts);
	//LCD_printf("Deg F:  %f", tempF);
	
	return temp;
	
} // end getTemp()

// -------------------------------------------------------------- //

byte receiveTemp( byte currentTemp ) {
	
	byte temperature = currentTemp;
	
	// First check IF there is data to read...
	LED_clr( LED_Green );
	while ( UART1_has_data() ) {
		// Visual for success
		LED_set( LED_Green );
		UART1_receive( &temperature );		
	}
	
	// Returns previously received temperature if none received.
	return temperature;
	
} // end receiveTemp()

// -------------------------------------------------------------- //

byte transmitTemp( void ) {
	
	UART_COMM_RESULT error;
	
	// Get temperature on device
	byte temperature = getTemp();
	
	// Send data
	error = UART1_transmit( temperature );
	
	// Visual for errors
	if ( error == UART_COMM_OK ) {
		LED_set( LED_Red );
	}
	else {
		LED_clr( LED_Red );
	}
	
	return temperature;
} // end transmitTemp()

// -------------------------------------------------------------- //

void updateDisplay( byte myTemp, byte rxTemp ) {
	
	LCD_clear();
	
	// Display myTemp
	LCD_printf( "My Temp: %d\n", myTemp );
	
	// Display rxTemp
	LCD_printf( "Rx Temp: %d\n", rxTemp );
	
} // end updateDisplay()