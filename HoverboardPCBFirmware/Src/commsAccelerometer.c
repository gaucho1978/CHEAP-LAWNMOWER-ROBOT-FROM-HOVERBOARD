
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

// Only master uses accelerometer data
#ifdef MASTER

extern uint8_t Accelerometer_X_High;

//periodically called by timers in it.c
void GetAccelerometerData(void){
	uint8_t bytesToReadIndex=0;
	
	//WAKE UP ACCELEROMETER (6B 00)
	
	// wait until I2C bus is idle
  while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
  // send a start condition to I2C bus 
  i2c_start_on_bus(I2C0);
  
	// wait until SBSEND bit is set
  while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
	
	// send slave address to I2C bus
  i2c_master_addressing(I2C0, I2C_ACCELEROMETER_ADDRESS, I2C_TRANSMITTER);
	
	// wait until ADDSEND bit is set
  while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
	
	//this is used only for a scan on i2c, during debug
	//while(indexTmp<40000){
	//	indexTmp++;	
	//	if(i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)){
	//		//indirizzo giusto!
	//		
	//
	//	}
	//}	
	
  // clear ADDSEND bit 
  i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    
	// send a data byte 
  i2c_data_transmit(I2C0,0x6B);  //PWR MANAGEMENT
  // wait until the transmission data register is empty
  while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
	// send a data byte 
  i2c_data_transmit(I2C0,0x00);  //WAKE UP
	// wait until the transmission data register is empty
  while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
	
	// send a stop condition to I2C bus
  i2c_stop_on_bus(I2C0);
  
	//proviamo a commentare questo
  while(I2C_CTL0(I2C0)&0x0200);
  
	
	//REQUEST ACELEROMETER DATA (3B )
	
	// wait until I2C bus is idle
  while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
  // send a start condition to I2C bus 
  i2c_start_on_bus(I2C0);
  
	// wait until SBSEND bit is set
  while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
	
	// send slave address to I2C bus
  i2c_master_addressing(I2C0, I2C_ACCELEROMETER_ADDRESS, I2C_TRANSMITTER);
	
	// wait until ADDSEND bit is set
  while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
	
	//this is used only for a scan on i2c, during debug
	//while(indexTmp<40000){
	//	indexTmp++;	
	//	if(i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)){
	//		//indirizzo giusto!
	//		
	//
	//	}
	//}	
	
  // clear ADDSEND bit 
  i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    
	// send a data byte 
  i2c_data_transmit(I2C0,0x3B);  //ACCELEROMETER REGISTER
  // wait until the transmission data register is empty
  while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
	
	
	// send a stop condition to I2C bus
  i2c_stop_on_bus(I2C0);
  
	//proviamo a commentare questo
  while(I2C_CTL0(I2C0)&0x0200);
  
	
	
	
	//now reopen the bus to receive the reply
	
	
	
	// wait until I2C bus is idle
  while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
  // send a start condition to I2C bus
  i2c_start_on_bus(I2C0);
  // wait until SBSEND bit is set 
  while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
  // send slave address to I2C bus 
  i2c_master_addressing(I2C0, I2C_ACCELEROMETER_ADDRESS, I2C_RECEIVER);
	// wait until ADDSEND bit is set 
  while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
	
	// if we receive only one byte: reset ACKEN bit
  i2c_ack_config(I2C0, I2C_ACK_DISABLE);
		
  //clear ADDSEND bit 
  i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
  
	// if we receive only one byte: send stop condition
  i2c_stop_on_bus(I2C0);
	
	//now we receive only one byte	
	for(bytesToReadIndex=0; bytesToReadIndex<1; bytesToReadIndex++){
		if(13 == bytesToReadIndex){
			/* wait until the second last data byte is received into the shift register */
			while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
			/* disable acknowledge */
      i2c_ack_config(I2C0, I2C_ACK_DISABLE);
    }
    /* wait until the RBNE bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));
		//read a data from I2C_DATA 
		Accelerometer_X_High = i2c_data_receive(I2C0);
  }
	
	// if we receive more bytes: send a stop condition to I2C bus 
  //i2c_stop_on_bus(I2C0);
  while(I2C_CTL0(I2C0)&0x0200);
  // enable acknowledge 
  i2c_ack_config(I2C0, I2C_ACK_ENABLE);
	
}

#endif
