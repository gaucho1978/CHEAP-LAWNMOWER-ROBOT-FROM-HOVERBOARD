//defines the intrface with the accelerometer chip (i2c serial line)

#include "gd32f1x0.h"
#include "../Inc/it.h"
#include "../Inc/comms.h"
#include "../Inc/commsSteeringPWM.h"
#include "../Inc/setup.h"
#include "../Inc/config.h"
#include "../Inc/defines.h"
#include "../Inc/bldc.h"
#include "stdio.h"
#include "string.h"
 #include <math.h>
// Only master uses accelerometer data
#ifdef MASTER
bool gyroscopeIsSleeping=TRUE;
bool accelerometerIsSleeping=TRUE;
bool magnetometerIsSleeping=FALSE; //set to FALSE since on the board there is no magnetometer
bool parametersStillToBeSet=TRUE;
bool calibrationStillToBePerformed=TRUE;
bool componentIsWakingUp=FALSE;
bool ImuNotCalibrated=TRUE;
uint16_t waitCycles=0;
uint8_t imuArray[22];
uint8_t tmpVar;

extern float pitchAngle;
extern float rollAngle;
extern float yawAngle;

float accelerationX=0;
float accelerationY=0;
float accelerationZ=1;
float gyroX=0;
float gyroY=0;
float gyroZ=0;
float temperature=23;
float timestamp=0;
float tmpDoubleVal;
extern bool recordAccelerometerLog;
extern bool printAccelerometerLog;

extern float logImuArray[400];
extern int16_t logImuArrayCurrentIndex;

uint8_t offsetDataValueToReadFromAccelerometerMemory=0;


//periodically called by timers in it.c
void GetAccelerometerData(void){
	uint8_t bytesToReadIndex=0;
	

	//first time we should
	//WAKE UP the device
	if(componentIsWakingUp){
		//we wait at least 81msec
		if(waitCycles<20){
			waitCycles++; 
			
		}else{
			waitCycles=0;
			componentIsWakingUp=FALSE;
		}
	}else if(gyroscopeIsSleeping){
		gyroscopeIsSleeping=FALSE;
		componentIsWakingUp=TRUE; //we use this to wait some time before to send more commands
		
		
		//SEND GYRO WAKEUP COMMAND 0x15
				// wait until I2C bus is idle
		while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
		// send a start condition to I2C bus 
		i2c_start_on_bus(I2C0);
		// wait until SBSEND bit is set
		while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
		// send slave address to I2C bus
		i2c_master_addressing(I2C0, I2C_IMU_ADDRESS, I2C_TRANSMITTER);
		// wait until ADDSEND bit is set
		while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
		// clear ADDSEND bit 
		i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
		// send a data byte 
		i2c_data_transmit(I2C0,0x7e); //register 0x7e
		// wait until the transmission data register is empty
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
		// send a data byte 
		i2c_data_transmit(I2C0,0x15);  //WAKE UP gyro
		// wait until the transmission data register is empty
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
		// send a stop condition to I2C bus
		i2c_stop_on_bus(I2C0);
		while(I2C_CTL0(I2C0)&0x0200);
		
	}else if(accelerometerIsSleeping){
		accelerometerIsSleeping=FALSE;
		componentIsWakingUp=TRUE; //we use this to wait some time before to send more commands

		//SEND ACCELEROMETER WAKEUP COMMAND 0x11
		// wait until I2C bus is idle
		while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
		// send a start condition to I2C bus 
		i2c_start_on_bus(I2C0);
		
		// wait until SBSEND bit is set
		while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
		// send slave address to I2C bus
		i2c_master_addressing(I2C0, I2C_IMU_ADDRESS, I2C_TRANSMITTER);
		// wait until ADDSEND bit is set
		while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
		// clear ADDSEND bit 
		i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
		// send a data byte 
		i2c_data_transmit(I2C0,0x7e); //register 0x7e
		// wait until the transmission data register is empty
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
		// send a data byte 
		i2c_data_transmit(I2C0,0x11);  //WAKE UP accelerometer
		// wait until the transmission data register is empty
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
		// send a stop condition to I2C bus
		i2c_stop_on_bus(I2C0);
		while(I2C_CTL0(I2C0)&0x0200);

	}else if(magnetometerIsSleeping){
		magnetometerIsSleeping=FALSE;
		componentIsWakingUp=TRUE; //we use this to wait some time before to send more commands
		//SEND MAGNOTOMETER WAKEUP COMMAND 0x19 (this command doesn't work, cause on the board there is no magnetometer connected to the accelerometer chip.
		// wait until I2C bus is idle
		while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
		// send a start condition to I2C bus 
		i2c_start_on_bus(I2C0);
		
		// wait until SBSEND bit is set
		while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
		// send slave address to I2C bus
		i2c_master_addressing(I2C0, I2C_IMU_ADDRESS, I2C_TRANSMITTER);
		// wait until ADDSEND bit is set
		while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
		// clear ADDSEND bit 
		i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
		// send a data byte 
		i2c_data_transmit(I2C0,0x7e); //register 0x7e
		// wait until the transmission data register is empty
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
		// send a data byte 
		i2c_data_transmit(I2C0,0x19);  //WAKE UP magnetometer
		// wait until the transmission data register is empty
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
		// send a stop condition to I2C bus
		i2c_stop_on_bus(I2C0);
		while(I2C_CTL0(I2C0)&0x0200);
	}else if(parametersStillToBeSet){
		parametersStillToBeSet=FALSE;
		
		//Set Gyroscope range, from 2000 degrees per second to 125 degrees per second - register 0x43 bit 2:0 '100' (262,4 LSB/degree/s <-> 3,8millidegree/s/LSB)
		// wait until I2C bus is idle
		while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
		// send a start condition to I2C bus 
		i2c_start_on_bus(I2C0);
		// wait until SBSEND bit is set
		while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
		// send slave address to I2C bus
		i2c_master_addressing(I2C0, I2C_IMU_ADDRESS, I2C_TRANSMITTER);
		// wait until ADDSEND bit is set
		while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
		// clear ADDSEND bit 
		i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
		// send a data byte 
		i2c_data_transmit(I2C0,0x43); //register 0x43 - gyroscope range
		// wait until the transmission data register is empty
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
		// send a data byte 
		i2c_data_transmit(I2C0,0x04);  //125 degrees per second
		// wait until the transmission data register is empty
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
		// send a stop condition to I2C bus
		i2c_stop_on_bus(I2C0);
		while(I2C_CTL0(I2C0)&0x0200);
		
		//enable offset correction for accelerometer and gyroscope. we should write on register 0x77 the value 0xC0
		// wait until I2C bus is idle
		while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
		// send a start condition to I2C bus 
		i2c_start_on_bus(I2C0);
		// wait until SBSEND bit is set
		while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
		// send slave address to I2C bus
		i2c_master_addressing(I2C0, I2C_IMU_ADDRESS, I2C_TRANSMITTER);
		// wait until ADDSEND bit is set
		while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
		// clear ADDSEND bit 
		i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
		// send a data byte 
		i2c_data_transmit(I2C0,0x77); //register 0x77
		// wait until the transmission data register is empty
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
		// send a data byte 
		i2c_data_transmit(I2C0,0xC0);  //enable  offset correction
		// wait until the transmission data register is empty
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
		// send a stop condition to I2C bus
		i2c_stop_on_bus(I2C0);
		while(I2C_CTL0(I2C0)&0x0200);
		
		//Fast Offset Calibration enabling. we should write on record 0x69 the value 0x7D ( gyroFOCenable(bit6=1), accX=0g (bit54=11), accY=0g(bit32=11), accZ=1g(bit10=01) ). 
		// wait until I2C bus is idle
		while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
		// send a start condition to I2C bus 
		i2c_start_on_bus(I2C0);
		// wait until SBSEND bit is set
		while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
		// send slave address to I2C bus
		i2c_master_addressing(I2C0, I2C_IMU_ADDRESS, I2C_TRANSMITTER);
		// wait until ADDSEND bit is set
		while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
		// clear ADDSEND bit 
		i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
		// send a data byte 
		i2c_data_transmit(I2C0,0x69); //register 0x69
		// wait until the transmission data register is empty
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
		// send a data byte 
		i2c_data_transmit(I2C0,0x7D);  //Fast Offset Calibration enabling
		// wait until the transmission data register is empty
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
		// send a stop condition to I2C bus
		i2c_stop_on_bus(I2C0);
		while(I2C_CTL0(I2C0)&0x0200);
		
		
	}else if(ImuNotCalibrated){
		
		ImuNotCalibrated=FALSE;
		componentIsWakingUp=TRUE; //we use this to wait some time before to query data
		//Perform automatic offset calibration on accelerometer and gyroscope. command 0x03
				// wait until I2C bus is idle
		while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
		// send a start condition to I2C bus 
		i2c_start_on_bus(I2C0);
		
		// wait until SBSEND bit is set
		while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
		// send slave address to I2C bus
		i2c_master_addressing(I2C0, I2C_IMU_ADDRESS, I2C_TRANSMITTER);
		// wait until ADDSEND bit is set
		while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
		// clear ADDSEND bit 
		i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
		// send a data byte 
		i2c_data_transmit(I2C0,0x7e); //register 0x7e
		// wait until the transmission data register is empty
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
		// send a data byte 
		i2c_data_transmit(I2C0,0x03);  //perform offset correction
		// wait until the transmission data register is empty
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
		// send a stop condition to I2C bus
		i2c_stop_on_bus(I2C0);
		while(I2C_CTL0(I2C0)&0x0200);

		
	}else{

		
		//REQUEST DATA
		
		// wait until I2C bus is idle
		while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
		// send a start condition to I2C bus 
		i2c_start_on_bus(I2C0);
		// wait until SBSEND bit is set
		while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
		// send slave address to I2C bus
		i2c_master_addressing(I2C0, I2C_IMU_ADDRESS, I2C_TRANSMITTER);
		// wait until ADDSEND bit is set
		while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
		// clear ADDSEND bit 
		i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
		// send a data byte 
		i2c_data_transmit(I2C0,0x0C + offsetDataValueToReadFromAccelerometerMemory);  //first REGISTER to read 0x0C
		
		// wait until the transmission data register is empty
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
		// send a stop condition to I2C bus
		i2c_stop_on_bus(I2C0);
		while(I2C_CTL0(I2C0)&0x0200);
		//now reopen the bus to receive the reply
		// wait until I2C bus is idle
		while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
		// send a start condition to I2C bus
		i2c_start_on_bus(I2C0);
		// wait until SBSEND bit is set 
		while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
		// send slave address to I2C bus 
		i2c_master_addressing(I2C0, I2C_IMU_ADDRESS, I2C_RECEIVER);
		// wait until ADDSEND bit is set 
		while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
		// if we receive only one byte: reset ACKEN bit
		//i2c_ack_config(I2C0, I2C_ACK_DISABLE);
		//clear ADDSEND bit 
		i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
		// if we receive only one byte: send stop condition
		//i2c_stop_on_bus(I2C0);
		//now we receive 	
		for(bytesToReadIndex=0; bytesToReadIndex<2; bytesToReadIndex++){ //era 22
			if(2-2 == bytesToReadIndex){
				// wait until the second last data byte is received into the shift register
				while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
				// disable acknowledge
				i2c_ack_config(I2C0, I2C_ACK_DISABLE);
			}
			// wait until the RBNE bit is set
			while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));
			tmpVar=i2c_data_receive(I2C0);
			imuArray[bytesToReadIndex+offsetDataValueToReadFromAccelerometerMemory]=tmpVar;
			
		}
			
		switch(offsetDataValueToReadFromAccelerometerMemory){
			
			case 0:
				gyroX=(int16_t)( imuArray[0]  | imuArray[1]  << 8) * 0.0038; // 125degrees per second/2^15
				break;
			case 2:
				gyroY=( (int16_t)( imuArray[2] | imuArray[3] << 8) * 0.0038)  ;
				break;
			case 4:
				gyroZ=( (int16_t)( imuArray[4] | imuArray[5] << 8) * 0.0038)  ;
			case 6:
				accelerationX=( (int16_t)( imuArray[6] | imuArray[7] << 8) * 0.000061)  ; // 2g /2^15 = 0.000061
				break;
			case 8:
				accelerationY=( (int16_t)( imuArray[8] | imuArray[9] << 8) * 0.000061)  ;
				break;
			case 10:
				accelerationZ=( (int16_t)( imuArray[10] | imuArray[11] << 8) * 0.000061)  ;
				//calculate pich
				//and integrate in time in order to do not consider accelerations due to speed
				//This introduces a delay, but it is not a problem since we don't need instant knowledge of roll and pitch
				pitchAngle = 0.95 * pitchAngle + 0.05 * 180 * atan((accelerationX/sqrt((accelerationY*accelerationY) + (accelerationZ*accelerationZ))))/ (M_PI);
				//calculat roll
				rollAngle = 0.95 * rollAngle + 0.05 * 180 * atan (accelerationY/sqrt(accelerationX*accelerationX + accelerationZ*accelerationZ))/M_PI;
			
				break;
			case 14:
				timestamp= (imuArray[12] + imuArray[13] *256 + imuArray[14] * 256*256 )   * 0.000039; //seconds
				break;
			case 20:
				temperature=( (int16_t)( imuArray[20] | imuArray[21] << 8) * 0.00195312) +23 ;  //celsius degrees - we could set an alarm if temperature goes over 50 celsius degrees
			
				
				//now we have entire array updated, let's record it, if requested
				//log memory
				if(recordAccelerometerLog){
					logImuArray[logImuArrayCurrentIndex]=gyroX;
					logImuArrayCurrentIndex++;
					logImuArray[logImuArrayCurrentIndex]=gyroY;
					logImuArrayCurrentIndex++;
					logImuArray[logImuArrayCurrentIndex]=gyroZ;
					logImuArrayCurrentIndex++;
					logImuArray[logImuArrayCurrentIndex]=accelerationX;
					logImuArrayCurrentIndex++;
					logImuArray[logImuArrayCurrentIndex]=accelerationY;
					logImuArrayCurrentIndex++;
					logImuArray[logImuArrayCurrentIndex]=accelerationZ;
					logImuArrayCurrentIndex++;
					logImuArray[logImuArrayCurrentIndex]=timestamp;
					logImuArrayCurrentIndex++;
					logImuArray[logImuArrayCurrentIndex]=temperature;
					logImuArrayCurrentIndex++;
					logImuArray[logImuArrayCurrentIndex]=rollAngle;
					logImuArrayCurrentIndex++;
					logImuArray[logImuArrayCurrentIndex]=pitchAngle;
					logImuArrayCurrentIndex++;
					
					if(logImuArrayCurrentIndex>391){
						//buffer is full, print it
						recordAccelerometerLog=FALSE;
						printAccelerometerLog=TRUE;
						logImuArrayCurrentIndex=-1;
					}
				}
				break;
		}
		offsetDataValueToReadFromAccelerometerMemory+=2;
		if(offsetDataValueToReadFromAccelerometerMemory==12) offsetDataValueToReadFromAccelerometerMemory=0;
		// ^
		// |
		// |
		//set limit to 22 if you want to read also temperature and timestamp
		
		// if we receive more bytes: send a stop condition to I2C bus 
		i2c_stop_on_bus(I2C0);
		
		while(I2C_CTL0(I2C0)&0x0200);
		// enable acknowledge 
		i2c_ack_config(I2C0, I2C_ACK_ENABLE);
	
		//accelerationX = (signed int)(((signed int)rawData_X) * 3.9);
		//accelerationY = (signed int)(((signed int)rawData_Y) * 3.9);
		//accelerationZ = (signed int)(((signed int)rawData_Z) * 3.9);
		//yaw = 180 * atan (accelerationZ/sqrt(accelerationX*accelerationX + accelerationZ*accelerationZ))/M_PI;
		}
}

#endif
