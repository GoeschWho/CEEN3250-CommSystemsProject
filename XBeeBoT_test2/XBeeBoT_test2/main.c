#include "capi324v221.h"

// ======================= prototypes =========================== //
void sendData (unsigned char data);
int	receiveData (void);
void updateDisplay( int myNumber, int friendNumber );

typedef enum { false, true } bool;
unsigned char sendingData = 0;
unsigned char receivingData= 0;

// ======================== main ================================ //

void CBOT_main( void )
{
	// Open the LCD
	LCD_open();
	
	// Open the LED
	LED_open();

	// Open and initialize the UART
	UART_open( UART_UART1 );
	UART_configure( UART_UART1, UART_8DBITS, UART_1SBIT, UART_NO_PARITY, 9600 );
	UART_set_TX_state( UART_UART1, UART_ENABLE );
	UART_set_RX_state( UART_UART1, UART_ENABLE );
	
	while (1)
	{
		// transmit
		sendingData = 1;
		
		//change this to a different number for the second ceenbot
		
		sendData (sendingData);
		receivingData = receiveData();
		
		updateDisplay( sendingData, receivingData );

	}; // Loop forever.
	
} // end CBOT_main()
//--------------------------------------------------------------- //
void sendData (unsigned char data) {
	UART_COMM_RESULT error;
	
	error = UART_transmit(UART_UART1, data);
	LED_clr( LED_Red );
	
	if ( error == UART_COMM_OK ) {
		LED_set( LED_Red );
	}
}
// -------------------------------------------------------------- //
int receiveData (void) {
	unsigned char number;
	// First check IF there is data to read...
	LED_clr ( LED_Green );
	if ( UART_has_data( UART_UART1 ) == TRUE ) {
		// LED for visual on REC
		LED_set( LED_Green );
		UART_receive(UART_UART1, &number);
	}
	return number;
}

void updateDisplay( int myNumber, int friendNumber ) {
	
	LCD_clear();
	
	// Display myTemp
	LCD_printf( "My Temp: %d\n", myNumber );
	
	// Display rxTemp
	LCD_printf( "Rx Temp: %d\n", friendNumber); //is it %d for int?
	
} // end updateDisplay()