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
//#include "../Inc/bldc.h"
//#include "../Inc/commsMasterSlave.h"
#include "../Inc/navigator.h"
#include "stdio.h"
#include "string.h"
//#include <math.h>

// Only master communicates with REMOTE device
#ifdef MASTER

	#define USART_REMOTE_MASTERBOARD_TX_BYTES 198   //number of bytes to transmit
	#ifdef MAVLINK_ENABLED
		#define USART_REMOTE_MASTERBOARD_RX_BYTES 42   // Receive byte count including  start character for GPS data
		#define USART_REMOTE_MASTERBOARD_RX_MINIMUM_BYTES 42   // Receive byte count including  start char '/' and stop character '\r\n'
	#else
		#define USART_REMOTE_MASTERBOARD_RX_BYTES 9   // Receive byte count including  start char '/' and stop character '\r\n'
		#define USART_REMOTE_MASTERBOARD_RX_MINIMUM_BYTES 3   // Receive byte count including  start char '/' and stop character '\r\n'
	#endif

	uint16_t MavLinkCRC;
	int32_t GpsLatitudeWGS84; 
	int32_t GpsLongitudeWGS84;
	uint16_t GpsHdop;
	uint8_t GpsSatellitesNumber;
	
	
	extern FlagStatus panicButtonPressed;
	//extern FlagStatus chargeStateLowActive;

	extern uint8_t usartRemote_MasterBoard_COM_rx_buf[USART_REMOTE_COM_RX_BUFFERSIZE];
	static uint8_t isRecordingDataPacket = 0;
	static uint8_t sUSARTRemote_MasterBoard_RecordBuffer[USART_REMOTE_MASTERBOARD_RX_BYTES];
	static uint8_t sUSARTRemote_MasterBoard_RecordBufferCounter = 0;

	bool printHelpLegend=TRUE;
	bool printSpeedAndSteer= FALSE;
	extern bool printAccelerometerLog;
	extern bool printMovementCompleted;

	extern bool recordAccelerometerLog;
	extern int16_t logImuArrayCurrentIndex;
	extern float logImuArray[400];

	void CheckUSART_Remote_MasterBoard_Input(uint8_t u8USARTBuffer[]);

	extern int16_t steerAngle;
	extern int16_t speed_mm_per_second;

	#ifdef TERMINAL_ENABLED
		unsigned char helpArray[158]={'/','h','=','T','h','i','s',' ','h','e','l','p','\r','\n','/','l','=','I','M','U',' ','l','o','g','\r','\n','/','s','=','S','p','e','e','d','+','S','t','e','e','r','\r','\n','/','m','+','0','0','0','0','0','=','M','o','v','e',' ','F','o','r','w','a','r','d',' ','f','o','r',' ','p','o','s','i','t','i','v','e', 'V','a','l','u','e',',','B','a','c','k','w','a','r','d',' ','o','t','h','e','r','w','i','s','e','\r','\n','/','t','+','0','0','0','0','0','=','T','u','r','n',' ','L','e','f','t',' ','f','o','r',' ','n','e','g','a','t','i','v','e',' ','V','a','l','u','e',',',' ','R','i','g','h','t',' ','o','t','h','e','r','w','i','s','e','\r','\n'};
		unsigned char tableHeaderArray[71]={'g','y','r','o','X',' ',',','g','y','r','o','Y',' ',',','g','y','r','o','Z',' ',',','a','c','c','e','X',' ',',','a','c','c','e','Y',' ',',','a','c','c','e','Z',' ',',','T','i','m','e',' ','S',',','T','e','m','p','e','r',',','R','o','l','l',' ',' ',',','P','i','t','c','h',' ','\r','\n'};
		unsigned char doneArray[7]={'D','o','n','e','!','\r','\n'};
	#endif
	#ifdef TERMINAL_ENABLED_PID_TUNING
		extern float PID_PARAM_KP;
		extern float PID_PARAM_KI;
		extern float PID_PARAM_KD;		
	#endif
//----------------------------------------------------------------------------
// Send message to device connected on serial port REMOTE of the master board
//----------------------------------------------------------------------------
void Send_Data_over_REMOTE_serialPort_of_MasterBoard(void)
{
	

	#ifdef TERMINAL_ENABLED 
	
		int index = 0;
		char charVal[6];
		uint8_t buffer[USART_REMOTE_MASTERBOARD_TX_BYTES];
		if (printHelpLegend ){
			printHelpLegend =FALSE;
			SendBuffer(USART_REMOTE_COM, helpArray, 158);
			return;
		}
		
		if (printAccelerometerLog ){
			if(logImuArrayCurrentIndex==-1){
				//print first line
				SendBuffer(USART_REMOTE_COM, tableHeaderArray, 71);
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
				myFtoa(speed_mm_per_second, charVal, 5); 
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
				myFtoa(steerAngle, charVal, 5); 
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
			
		if (printMovementCompleted ){
			printMovementCompleted=FALSE;
			SendBuffer(USART_REMOTE_COM, doneArray, 7);
			return;
		}
	#endif
}

//----------------------------------------------------------------------------
// Update USART REMOTE (master board) input
// this routine is used to analyze data received  via REMOTE serial port (of the master board)
// this serial port cn be used for debug purposes, connected to a terminal, or connected to sonar sensors via arduino board.
//----------------------------------------------------------------------------
void UpdateUSART_REMOTE_MASTER_BOARD_Input(void)
{
	uint8_t character = usartRemote_MasterBoard_COM_rx_buf[0];
	
	#ifdef MAVLINK_ENABLED
		// Start character is captured, start record
		if (character == 0xFD){
			sUSARTRemote_MasterBoard_RecordBufferCounter = 0;
			isRecordingDataPacket = 1;
		}
	#else
		// Start character is captured, start record
		if (character == '/')
		{
			
			sUSARTRemote_MasterBoard_RecordBufferCounter = 0;
			isRecordingDataPacket = 1;
		}
	#endif
	
	if (isRecordingDataPacket)
	{
		sUSARTRemote_MasterBoard_RecordBuffer[sUSARTRemote_MasterBoard_RecordBufferCounter] = character;
		sUSARTRemote_MasterBoard_RecordBufferCounter++;
		#ifdef MAVLINK_ENABLED
			//if buffer is full 
			if (sUSARTRemote_MasterBoard_RecordBufferCounter >= USART_REMOTE_MASTERBOARD_RX_BYTES)
			{
				
				sUSARTRemote_MasterBoard_RecordBufferCounter = 0;
				isRecordingDataPacket = 0;
				
				// Check input
				CheckUSART_Remote_MasterBoard_Input (sUSARTRemote_MasterBoard_RecordBuffer);
			}
		#else
			//if buffer is full or end of command \r identified
			if (sUSARTRemote_MasterBoard_RecordBufferCounter >= USART_REMOTE_MASTERBOARD_RX_BYTES ||
				character=='\r')
			{
				
				sUSARTRemote_MasterBoard_RecordBufferCounter = 0;
				isRecordingDataPacket = 0;
				
				// Check input
				CheckUSART_Remote_MasterBoard_Input (sUSARTRemote_MasterBoard_RecordBuffer);
			}
		#endif
	}
}

//----------------------------------------------------------------------------
// Check REMOTE USART (master board) received data packet
//----------------------------------------------------------------------------
void CheckUSART_Remote_MasterBoard_Input(uint8_t USARTBuffer[])
{
	#ifdef MAVLINK_ENABLED
		uint16_t receivedMsgCrc;
		//check crc
		MavLinkCRC= *(uint16_t*)&USARTBuffer[40]; 
		//TBDone....................................
		//add the crc_extra on last 2 bytes, taking those data from the GPS device xml implementation
	
		receivedMsgCrc = CalcCRC(0xFFFF,&USARTBuffer[1], USART_REMOTE_MASTERBOARD_RX_BYTES - 1-2);
		if (MavLinkCRC!=receivedMsgCrc){ //if wrong CRC
			return;
		}
		//check message ID (bytes from 7 to 9)
		//id 24=gps data
		if(USARTBuffer[9]==24 && USARTBuffer[8]==0 && USARTBuffer[7]==0){ //if it is GPS message
			//extract GPS data
			GpsLatitudeWGS84= *(int32_t*)&USARTBuffer[19]; 
			GpsLongitudeWGS84= *(int32_t*)&USARTBuffer[23]; 
			GpsHdop=*(uint16_t*)&USARTBuffer[31];
			GpsSatellitesNumber=*(uint8_t*)&USARTBuffer[39];
		}
		
		
		

	#else
		// Check start and stop character (stop char can be at pos.2 or 8)
		if ( USARTBuffer[0] != '/' ||
			((USARTBuffer[USART_REMOTE_MASTERBOARD_RX_BYTES - 1] != '\r') && (USARTBuffer[2] != '\r') ))	{ 
			return;
		}
		
		if (USARTBuffer[2] == '\r'){ //if it is a short command
			
			#ifdef TERMINAL_ENABLED
			
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
			#endif
			
		}

		if (USARTBuffer[USART_REMOTE_MASTERBOARD_RX_BYTES - 1] == '\r'){ //if it is a long command
			
			#ifdef TERMINAL_ENABLED
				if(USARTBuffer[1]=='m' || USARTBuffer[1]=='t'){ //move or turn
					int16_t cmdValue;
					cmdValue=(USARTBuffer[3] - '0') * 10000;
					cmdValue=cmdValue+ (USARTBuffer[4] - '0') * 1000;
					cmdValue=cmdValue+ (USARTBuffer[5] - '0') * 100;
					cmdValue=cmdValue+ (USARTBuffer[6] - '0') * 10;
					cmdValue=cmdValue+ (USARTBuffer[7] - '0');	
					if (cmdValue<0) cmdValue=0; //maybe overflow, set 0 to avoid wrong direction
					//get the sign
					//by default assume positive value
					if (USARTBuffer[2]=='-') cmdValue=-cmdValue; //negative value
					
					if (USARTBuffer[1]=='m'){ //we must go straight
						
						move(FALSE,cmdValue);
					}else{ //we must turn
						move(TRUE,cmdValue);
					}				
				}
			#endif
			
			#ifdef TERMINAL_ENABLED_PID_TUNING
				if(USARTBuffer[1]=='p' || USARTBuffer[1]=='i' || USARTBuffer[1]=='d'){ //pid parameters
					int16_t cmdValue;
					cmdValue=(USARTBuffer[3] - '0') * 10000;
					cmdValue=cmdValue+ (USARTBuffer[4] - '0') * 1000;
					cmdValue=cmdValue+ (USARTBuffer[5] - '0') * 100;
					cmdValue=cmdValue+ (USARTBuffer[6] - '0') * 10;
					cmdValue=cmdValue+ (USARTBuffer[7] - '0');	
					if (cmdValue<0) cmdValue=0; //maybe overflow, set 0 to avoid wrong direction
					//get the sign
					//by default assume positive value
					if (USARTBuffer[2]=='-') cmdValue=-cmdValue; //negative value
					
					if (USARTBuffer[1]=='p'){ //proportional constant (*1000)
						PID_PARAM_KP=cmdValue/10000.0;
					}
					if (USARTBuffer[1]=='i'){ //integral constant (*1000)
						PID_PARAM_KI=cmdValue/10000.0;
					}
					if (USARTBuffer[1]=='d'){ //differential constant (*1000)
						PID_PARAM_KD=cmdValue/10000.0;
					}
					//echo received value
					SendBuffer(USART_REMOTE_COM, USARTBuffer, 9);
				}
			#endif

		}
	#endif
}
#endif

#ifdef TERMINAL_ENABLED

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

#endif
