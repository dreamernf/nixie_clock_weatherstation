
#include "eeprom.h"

uint8_t Write_24Cxx(uint16_t, uint8_t, uint8_t);
uint8_t Read_24Cxx(uint16_t, uint8_t);


uint8_t Write_24Cxx(uint16_t Addr, uint8_t Data, uint8_t Mem_Type)
{
	
		uint32_t timeout = I2C_TIMEOUT_MAX;
		uint8_t upper_addr,lower_addr;
		lower_addr = (uint8_t)((0x00FF)&Addr);
	
		if(Mem_Type==M24512)
		{
			Addr = Addr>>8;
			upper_addr = (uint8_t)((0x00FF)&Addr);
		}
		/* Generate the Start Condition */
		I2C_GenerateSTART(I2C2, ENABLE);

		/* Test on I2C2 EV5, Start trnsmitted successfully and clear it */
		timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
		{
			/* If the timeout delay is exeeded, exit with error code */
			if ((timeout--) == 0) return 0xFF;	
		}
   
		/* Send Memory device slave Address for write */
		I2C_Send7bitAddress(I2C2, MEM_DEVICE_WRITE_ADDR, I2C_Direction_Transmitter);
 
		/* Test on I2C2 EV6 and clear it */
		timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		{
			/* If the timeout delay is exeeded, exit with error code */
			if ((timeout--) == 0) return 0xFF;
		}
 
		if(Mem_Type==M24512)
		{
			/* Send I2C2 location address LSB */
			I2C_SendData(I2C2, upper_addr);
			/* Test on I2C2 EV8 and clear it */
			timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
			while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
			{
				/* If the timeout delay is exeeded, exit with error code */
				if ((timeout--) == 0) return 0xFF;
			}
		}
		/* Send I2C2 location address LSB */
		I2C_SendData(I2C2, lower_addr);

		/* Test on I2C2 EV8 and clear it */
		timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			/* If the timeout delay is exeeded, exit with error code */
			if ((timeout--) == 0) return 0xFF;
		}
  
		/* Send Data */
		I2C_SendData(I2C2, Data);

		/* Test on I2C2 EV8 and clear it */
		timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			/* If the timeout delay is exeeded, exit with error code */
			if ((timeout--) == 0) return 0xFF;
		}  
 
		/* Send I2C2 STOP Condition */
		I2C_GenerateSTOP(I2C2, ENABLE);
  
		/* If operation is OK, return 0 */
		return 0;
}

uint8_t Read_24Cxx(uint16_t Addr, uint8_t Mem_Type)
{
		uint32_t timeout = I2C_TIMEOUT_MAX;
		uint8_t Data = 0;
		uint8_t upper_addr,lower_addr;
	
		lower_addr = (uint8_t)((0x00FF)&Addr);
	
		if(Mem_Type==M24512)
		{
			Addr = Addr>>8;
			upper_addr = (uint8_t)((0x00FF)&Addr);
	}
  /* Generate the Start Condition */
  I2C_GenerateSTART(I2C2, ENABLE);

  /* Test on I2C2 EV5 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
  {
    /* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0x01;
  } 
  
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(I2C2, MEM_DEVICE_WRITE_ADDR, I2C_Direction_Transmitter);
 
  /* Test on I2C2 EV6 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0x02;
  } 

	if(Mem_Type==M24512)
	{
		/* Send I2C2 location address LSB */
		I2C_SendData(I2C2,upper_addr);

		/* Test on I2C2 EV8 and clear it */
		timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			/* If the timeout delay is exeeded, exit with error code */
			if ((timeout--) == 0) return 0x03;
		}
	}		
	
	  /* Send I2C2 location address LSB */
  I2C_SendData(I2C2, lower_addr);

  /* Test on I2C2 EV8 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0x04;
  } 
  
  /* Clear AF flag if arised */
  //I2C2->SR1 |= (uint16_t)0x0400;

  /* Generate the Start Condition */
  I2C_GenerateSTART(I2C2, ENABLE);
  
  /* Test on I2C2 EV6 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
  {
    /* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0x05;
  } 
  
  /* Send DCMI selcted device slave Address for write */
  I2C_Send7bitAddress(I2C2, MEM_DEVICE_READ_ADDR, I2C_Direction_Receiver);
   
  /* Test on I2C2 EV6 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
    /* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0x06;
  }  
 
	/* Prepare an NACK for the next data received */
  I2C_AcknowledgeConfig(I2C2, DISABLE);

  /* Test on I2C2 EV7 and clear it */
  timeout = I2C_TIMEOUT_MAX; /* Initialize timeout value */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))
  {
    /* If the timeout delay is exeeded, exit with error code */
		//while(1);
    if ((timeout--) == 0) return 0x07;
  }   


    
  /* Prepare Stop after receiving data */
  I2C_GenerateSTOP(I2C2, ENABLE);

  /* Receive the Data */
  Data = I2C_ReceiveData(I2C2);

  /* return the read data */
  return Data;
}


