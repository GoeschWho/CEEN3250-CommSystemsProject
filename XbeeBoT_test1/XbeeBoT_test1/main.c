#include "capi324v221.h"

// ======================= prototypes =========================== //
void sendData(unsigned char data);
void updateDisplay( unsigned char sentValue );

// ======================== main ================================ //

void CBOT_main( void )
{
	SUBSYS_STATUS opstat;
	// Open the LCD
	LCD_open();

	// Open and initialize the UART
	UART_close( UART_UART1 );
	opstat = UART_open( UART_UART1 );
	while(opstat != SUBSYS_OPEN);
	UART_configure( UART_UART1, UART_8DBITS, UART_1SBIT, UART_NO_PARITY, 9600 );
	//UART_set_timeout(UART_UART1, 1);
	UART_set_TX_state( UART_UART1, UART_ENABLE );

	unsigned char data = 0x05;
	
	sendData(data);
	//updateDisplay(data);
}	
void sendData(unsigned char data){
	UART_COMM_RESULT error;
	error = UART_COMM_OK;
	error = UART1_transmit(0x04);
	if (error == UART_COMM_OK) {
		LCD_printf("OK");
	}
	else if (error == UART_COMM_ERROR) {
		LCD_printf("Error");
	}
	else if (error == UART_COMM_TX_FULL) {
		LCD_printf("TX Full");
	}
	else if (error == UART_COMM_TIMEOUT) {
		LCD_printf("Timeout");
	}
}
void updateDisplay( unsigned char sentValue ) {
	
	LCD_clear();
	
	// Display myTemp
	LCD_printf( "My Temp: %d\n", sentValue );
	
} // end updateDisplay()