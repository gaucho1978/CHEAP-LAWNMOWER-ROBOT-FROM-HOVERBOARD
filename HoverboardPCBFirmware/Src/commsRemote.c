/*
  defines the serial 232 interface of the MASTER BOARD named REMOTE on the PCB wiring diagram.
	this serial line is used for debug purposes, if connected to a terminal pc, or it is commonly used to communicate with sonar sensors via arduino board.
*/

#include "gd32f1x0.h"
#include "../Inc/it.h"
#include "../Inc/comms.h"
#include "../Inc/commsRemote.h"
#include "../Inc/setup.h"
#include "../Inc/config.h"
#include "../Inc/defines.h"
#include "../Inc/bldc.h"
#include "stdio.h"
#include "string.h"
//#include <math.h>

// Only master communicates with REMOTE device
#ifdef MASTER

#define USART_REMOTE_MASTERBOARD_TX_BYTES 198   //number of bytes to transmit
#define USART_REMOTE_MASTERBOARD_RX_BYTES 3   // Receive byte count including  start char '/' and stop character '\r\n'

extern FlagStatus panicButtonPressed;

extern uint8_t usartRemote_MasterBoard_COM_rx_buf[USART_REMOTE_COM_RX_BUFFERSIZE];
static uint8_t isRecordingDataPacket = 0;
static uint8_t sUSARTRemote_MasterBoard_RecordBuffer[USART_REMOTE_MASTERBOARD_RX_BYTES];
static uint8_t sUSARTRemote_MasterBoard_RecordBufferCounter = 0;

bool printHelpLegend=TRUE;
bool printSpeedAndSteer= FALSE;
extern bool printAccelerometerLog;
extern bool recordAccelerometerLog;
extern int16_t logImuArrayCurrentIndex;
extern float logImuArray[400];

void CheckUSART_Remote_MasterBoard_Input(uint8_t u8USARTBuffer[]);

extern int32_t steer;
extern int32_t speed;

//----------------------------------------------------------------------------
// Send message to device connected on serial port REMOTE of the master board
//----------------------------------------------------------------------------
void Send_Data_over_REMOTE_serialPort_of_MasterBoard(void)
{
	int index = 0;
	char charVal[6];
	uint8_t buffer[USART_REMOTE_MASTERBOARD_TX_BYTES];

	
	if (printHelpLegend ){
		printHelpLegend =FALSE;
		buffer[index++]='/';
		buffer[index++]='h';
		buffer[index++]='=';
		buffer[index++]='T';
		buffer[index++]='h';
		buffer[index++]='i';
		buffer[index++]='s';
		buffer[index++]=' ';
		buffer[index++]='h';
		buffer[index++]='e';
		buffer[index++]='l';
		buffer[index++]='p';
		buffer[index++]='\r';
		buffer[index++]='\n';
		buffer[index++]='/';
		buffer[index++]='l';
		buffer[index++]='=';
		buffer[index++]='P';
		buffer[index++]='r';
		buffer[index++]='i';
		buffer[index++]='n';
		buffer[index++]='t';
		buffer[index++]=' ';
		buffer[index++]='a';
		buffer[index++]='c';
		buffer[index++]='c';
		buffer[index++]='e';
		buffer[index++]='l';
		buffer[index++]='e';
		buffer[index++]='r';
		buffer[index++]='o';
		buffer[index++]='m';
		buffer[index++]='e';
		buffer[index++]='t';
		buffer[index++]='e';
		buffer[index++]='r';
		buffer[index++]=' ';
		buffer[index++]='l';
		buffer[index++]='o';
		buffer[index++]='g';
		buffer[index++]='\r';
		buffer[index++]='\n';
		buffer[index++]='/';
		buffer[index++]='s';
		buffer[index++]='=';
		buffer[index++]='P';
		buffer[index++]='r';
		buffer[index++]='i';
		buffer[index++]='n';
		buffer[index++]='t';
		buffer[index++]=' ';
		buffer[index++]='S';
		buffer[index++]='p';
		buffer[index++]='e';
		buffer[index++]='e';
		buffer[index++]='d';
		buffer[index++]=' ';
		buffer[index++]='a';
		buffer[index++]='n';
		buffer[index++]='d';
		buffer[index++]=' ';
		buffer[index++]='S';
		buffer[index++]='t';
		buffer[index++]='e';
		buffer[index++]='e';
		buffer[index++]='r';
		buffer[index++] = '\r';
		buffer[index++] = '\n';	
		SendBuffer(USART_REMOTE_COM, buffer, index);
		return;
	}
	
	if (printAccelerometerLog ){
		if(logImuArrayCurrentIndex==-1){
			//print first line
			buffer[index++]='g';
			buffer[index++]='y';
			buffer[index++]='r';
			buffer[index++]='o';
			buffer[index++]='X';
			buffer[index++]=' ';
			buffer[index++]=',';
			buffer[index++]='g';
			buffer[index++]='y';
			buffer[index++]='r';
			buffer[index++]='o';
			buffer[index++]='Y';
			buffer[index++]=' ';
			buffer[index++]=',';
			buffer[index++]='g';
			buffer[index++]='y';
			buffer[index++]='r';
			buffer[index++]='o';
			buffer[index++]='Z';
			buffer[index++]=' ';
			buffer[index++]=',';
			buffer[index++]='a';
			buffer[index++]='c';
			buffer[index++]='c';
			buffer[index++]='e';
			buffer[index++]='X';
			buffer[index++]=' ';
			buffer[index++]=',';
			buffer[index++]='a';
			buffer[index++]='c';
			buffer[index++]='c';
			buffer[index++]='e';
			buffer[index++]='Y';
			buffer[index++]=' ';
			buffer[index++]=',';
			buffer[index++]='a';
			buffer[index++]='c';
			buffer[index++]='c';
			buffer[index++]='e';
			buffer[index++]='Z';
			buffer[index++]=' ';
			buffer[index++]=',';
			buffer[index++]='T';
			buffer[index++]='i';
			buffer[index++]='m';
			buffer[index++]='e';
			buffer[index++]=' ';
			buffer[index++]='S';
			buffer[index++]=',';
			buffer[index++]='T';
			buffer[index++]='e';
			buffer[index++]='m';
			buffer[index++]='p';
			buffer[index++]='e';
			buffer[index++]='r';
			buffer[index++]=',';
			buffer[index++]='R';
			buffer[index++]='o';
			buffer[index++]='l';
			buffer[index++]='l';
			buffer[index++]=' ';
			buffer[index++]=' ';
			buffer[index++]=',';
			buffer[index++]='P';
			buffer[index++]='i';
			buffer[index++]='t';
			buffer[index++]='c';
			buffer[index++]='h';
			buffer[index++]=' ';
			buffer[index++]='\r';
			buffer[index++]='\n';
			
			SendBuffer(USART_REMOTE_COM, buffer, index);
			logImuArrayCurrentIndex++;
			return;
		}
		for(;logImuArrayCurrentIndex<400;logImuArrayCurrentIndex++){
			//sprintf(charVal, "%05F", logImuArray[logImuArrayCurrentIndex]); // this increases the firmware size!
			myFtoa(logImuArray[logImuArrayCurrentIndex], charVal, 5); //
			
			buffer[index++] = charVal[0];
			buffer[index++] = charVal[1];
			buffer[index++] = charVal[2];
			buffer[index++] = charVal[3];
			buffer[index++] = charVal[4];
			buffer[index++] = charVal[5];
			buffer[index++] = ',';
			
			if(logImuArrayCurrentIndex % 10==9){
				//line end
				logImuArrayCurrentIndex++;
				buffer[index++] = '\r';
				buffer[index++] = '\n';	
				SendBuffer(USART_REMOTE_COM, buffer, index);
				return;
			}
		}
	
		
		
		printAccelerometerLog=FALSE;
		logImuArrayCurrentIndex=0;
		
		return;
	}

	if (printSpeedAndSteer ){
		printSpeedAndSteer=FALSE;
			buffer[index++] = 'S';
			buffer[index++] = 'p';
			buffer[index++] = 'e';
			buffer[index++] = 'e';
			buffer[index++] = 'd';
			buffer[index++] = '=';
			buffer[index++] = ' ';
			//sprintf(charValVerbose, "%05d", speed);
			myFtoa(speed, charVal, 5); 
			buffer[index++] = charVal[0];
			buffer[index++] = charVal[1];
			buffer[index++] = charVal[2];
			buffer[index++] = charVal[3];
			buffer[index++] = charVal[4];
			buffer[index++] = '\r';
			buffer[index++] = '\n';
			buffer[index++] = 'S';
			buffer[index++] = 't';
			buffer[index++] = 'e';
			buffer[index++] = 'e';
			buffer[index++] = 'r';
			buffer[index++] = '=';
			buffer[index++] = ' ';
			//sprintf(charValVerbose, "%05d", steer);
			myFtoa(steer, charVal, 5); 
			buffer[index++] = charVal[0];
			buffer[index++] = charVal[1];
			buffer[index++] = charVal[2];
			buffer[index++] = charVal[3];
			buffer[index++] = charVal[4];
			buffer[index++] = '\r';
			buffer[index++] = '\n';
			SendBuffer(USART_REMOTE_COM, buffer, index);
			return;
	}		
	
}

//----------------------------------------------------------------------------
// Update USART REMOTE (master board) input
// this routine is used to analyze data received  via REMOTE serial port (of the master board)
// this serial port cn be used for debug purposes, connected to a terminal, or connected to sonar sensors via arduino board.
//----------------------------------------------------------------------------
void UpdateUSART_REMOTE_MASTER_BOARD_Input(void)
{
	uint8_t character = usartRemote_MasterBoard_COM_rx_buf[0];
	
	// Start character is captured, start record
	if (character == '/')
	{
		
		sUSARTRemote_MasterBoard_RecordBufferCounter = 0;
		isRecordingDataPacket = 1;
	}
	
	if (isRecordingDataPacket)
	{
		sUSARTRemote_MasterBoard_RecordBuffer[sUSARTRemote_MasterBoard_RecordBufferCounter] = character;
		sUSARTRemote_MasterBoard_RecordBufferCounter++;
		
		if (sUSARTRemote_MasterBoard_RecordBufferCounter >= USART_REMOTE_MASTERBOARD_RX_BYTES)
		{
			
			sUSARTRemote_MasterBoard_RecordBufferCounter = 0;
			isRecordingDataPacket = 0;
			
			// Check input
			CheckUSART_Remote_MasterBoard_Input (sUSARTRemote_MasterBoard_RecordBuffer);
		}
	}
}

//----------------------------------------------------------------------------
// Check REMOTE USART (mater board) received data packet
//----------------------------------------------------------------------------
void CheckUSART_Remote_MasterBoard_Input(uint8_t USARTBuffer[])
{
	// Auxiliary variables
	//uint16_t crc;
	
	//echo received data for debug
	
	//SendBuffer(USART_REMOTE_COM, USARTBuffer, 4);
	
	
	// Check start and stop character
	if ( USARTBuffer[0] != '/' ||
		USARTBuffer[USART_REMOTE_MASTERBOARD_RX_BYTES - 1] != '\r')
	{ //USARTBuffer[USART_REMOTE_MASTERBOARD_RX_BYTES - 1] != '\n' ||
		return;
	}
	
	// Calculate CRC (first bytes except crc and stop byte)
	//crc = CalcCRC(USARTBuffer, USART_REMOTE_MASTERBOARD_RX_BYTES - 3);
	
	// Check CRC
	//if ( USARTBuffer[USART_REMOTE_MASTERBOARD_RX_BYTES - 3] != ((crc >> 8) & 0xFF) ||
	//	USARTBuffer[USART_REMOTE_MASTERBOARD_RX_BYTES - 2] != (crc & 0xFF))
	//{
	//	return;
	//}
	
	// Calculate result speed value -1000 to 1000
	//speed = (int16_t)((USARTBuffer[1] << 8) | USARTBuffer[2]);
	
	// Calculate result steering value -1000 to 1000
	//steer = (int16_t)((USARTBuffer[3] << 8) | USARTBuffer[4]);
	
	// Reset the pwm timout to avoid stopping motors
	//ResetTimeout();
	if(USARTBuffer[1]=='h'){
		//we shall print help legend
		printHelpLegend=TRUE;
	}
	if(USARTBuffer[1]=='l'){
		//we shall record accelerometers log 
		
		//if we are not recording neither printing, start recording
		if((printAccelerometerLog==0) && (recordAccelerometerLog==0)){ 
			logImuArrayCurrentIndex=0;
			recordAccelerometerLog=TRUE;
		}
	}
	if(USARTBuffer[1]=='s'){
		//we shall print speed and steer
		printSpeedAndSteer=TRUE;
	}
}
#endif



float myPow(int a,int b){
      if(b<0){      
        return (1.0/a)*(myPow(a,b+1));
			}else if(b==0){
        return 1;
			}else if(b==1){
        return a;
      }else{
        return a*myPow(a,b-1);
			}
    }

		/*
		
		// reverses a string 'str' of length 'len' 
void reverse(char *str, int len) 
{ 
    int i=0, j=len-1, temp; 
    while (i<j) 
    { 
        temp = str[i]; 
        str[i] = str[j]; 
        str[j] = temp; 
        i++; j--; 
    } 
} 
// Converts a given integer x to string str[].  d is the number 
 // of digits required in output. If d is more than the number 
 // of digits in x, then 0s are added at the beginning. 

int intToStr(int x, char str[], int d) 
{ 
		int i = 0; 
		//if(x<0){
		//	str[i++]='-';
		//	x=-x;
		//}
    
		
    while (x) { 
        str[i++] = (x%10) + '0'; 
        x = x/10; 
    } 
  
    // If number of digits required is more, then 
    // add 0s at the beginning 
    while (i < d) 
        str[i++] = '0'; 
  
    reverse(str, i); 
    str[i] = '\0'; 
    return i;
		
} 

// Converts a floating point number to string. 
void myFtoa(float n, char *res, int afterpoint){ 
    // Extract integer part 
    int ipart = (int)n; 
  
    // Extract floating part 
    float fpart = n - (float)ipart; 
  
    // convert integer part to string 
    int i = intToStr(ipart, res, 0); 
		//int i= itoa(ipart, res, 0);
    // check for display option after point 
    if (afterpoint != 0) 
    { 
        res[i] = '.';  // add dot 
  
        // Get the value of fraction part upto given no. 
        // of points after dot. The third parameter is needed 
        // to handle cases like 233.007 
        fpart = fpart * myPow(10, afterpoint); 
  
        intToStr((int)fpart, res + i + 1, afterpoint); 
			
    } 
} 

*/

void myFtoa(float f, char *str, int precision){ 


	//str should be made of 30 char
	int a,b,c,l=0,m,i=0,k;
	k=precision;
	// check for negetive float
	if(f<0.0){
		str[i++]='-';
		f*=-1;
	}
	a=f;	// extracting whole number
	f-=a;	// extracting decimal part
	// number of digits in whole number
	while(k>-1){
		l = myPow(10,k);
		m = a/l;
		if(m>0){
			break;
		}
		k--;
	}
	// number of digits in whole number are k+1
	/*
	extracting most significant digit i.e. right most digit , and concatenating to string
	obtained as quotient by dividing number by 10^k where k = (number of digit -1)
	*/
	for(l=k+1;l>0;l--){
		b = myPow(10,l-1);
		c = a/b;
		str[i++]=c+48;
		a%=b;
	}
	str[i++] = '.';
	/* extracting decimal digits till precision */
	for(l=0;l<precision;l++){
		f*=10.0;
		b = f;
		str[i++]=b+48;
		f-=b;
	}
	str[i]='\0';
}
