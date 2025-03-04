#include "eeprom.h"

//*****************************************************************************
// Used to determine if the EEPROM is busy writing the last transaction to 
// non-volatile storage
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
// Returns
// I2C_OK is returned one the EEPROM is ready to write the next byte
//*****************************************************************************
static 
i2c_status_t eeprom_wait_for_write( int32_t  i2c_base)
{
  
  i2c_status_t status;
  
  if( !i2cVerifyBaseAddr(i2c_base) )
  {
    return  I2C_INVALID_BASE;
  }

	// ADDED AFTER KNOWN WORKING
  while ( I2CMasterBusy(i2c_base)) {};
	// /ADDED

  // Set the I2C address to be the EEPROM and in Write Mode
  status = i2cSetSlaveAddr(i2c_base, MCP24LC32AT_DEV_ID, I2C_WRITE);

  // Poll while the device is busy.  The  MCP24LC32AT will not ACK
  // writing an address while the write has not finished.
  do 
  {
    // The data we send does not matter.  This has been set to 0x00, but could
    // be set to anything
    status = i2cSendByte( i2c_base, 0x00, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
    
    // Wait for the address to finish transmitting
    while ( I2CMasterBusy(i2c_base)) {};
    
    // If the address was not ACKed, try again.
  } while (I2CMasterAdrAck(i2c_base) == false);

  return  status;
}
  
  
//*****************************************************************************
// Writes a single byte of data out to the  MCP24LC32AT EEPROM.  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    16-bit address of the byte being written.  Only the lower
//                12 bits is used by the EEPROM
//
//    data:       Data written to the EEPROM.
//
// Returns
// I2C_OK if the byte was written to the EEPROM.
//*****************************************************************************
#define statuscheck(st) if ((status = st) != I2C_OK) return status;
i2c_status_t eeprom_byte_write
( 
  uint32_t  i2c_base,
  uint16_t  address,
  uint8_t   data
)
{
  i2c_status_t status;
	uint8_t l, h;
	l = address & 0xFF;
  h = (address >> 8);
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};

  //==============================================================
  // Set the I2C address to be the EEPROM
	// ADD CODE
  //==============================================================
	statuscheck(i2cSetSlaveAddr(i2c_base, MCP24LC32AT_DEV_ID, I2C_WRITE));  
  // If the EEPROM is still writing the last byte written, wait
  statuscheck(eeprom_wait_for_write(i2c_base));
  
  //==============================================================
  // Send the Upper byte of the address
	// ADD CODE	
  //==============================================================
	statuscheck(i2cSendByte(i2c_base, h, I2C_MCS_START | I2C_MCS_RUN));

	if (!I2CMasterDatAck(i2c_base))
		return status;
  //==============================================================
  // Send the Lower byte of the address
	// ADD CODE
  //==============================================================		
	statuscheck(i2cSendByte(i2c_base, l, I2C_MCS_RUN));
	if (!I2CMasterDatAck(i2c_base))
		return status;

  
  //==============================================================
  // Send the Byte of data to write
	// ADD CODE
  //==============================================================
	statuscheck(i2cSendByte(i2c_base, data, I2C_MCS_RUN | I2C_MCS_STOP));
	if (!I2CMasterDatAck(i2c_base))
		return status;

  return status;
}

//*****************************************************************************
// Reads a single byte of data from the  MCP24LC32AT EEPROM.  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    16-bit address of the byte being read.  Only the lower
//                12 bits is used by the EEPROM
//
//    data:       data read from the EEPROM is returned to a uint8_t pointer.
//
// Returns
// I2C_OK if the byte was read from the EEPROM.
//*****************************************************************************
i2c_status_t eeprom_byte_read
( 
  uint32_t  i2c_base,
  uint16_t  address,
  uint8_t   *data
)
{
  i2c_status_t status;
	uint8_t l, h;
  l = address & 0xFF;
  h = (address >> 8);
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};

  // If the EEPROM is still writing the last byte written, wait
  statuscheck(eeprom_wait_for_write(i2c_base));

  //==============================================================
  // Set the I2C slave address to be the EEPROM and in Write Mode
	// ADD CODE
  //==============================================================
	statuscheck(i2cSetSlaveAddr(i2c_base, MCP24LC32AT_DEV_ID, I2C_WRITE));  
	
  //==============================================================
  // Send the Upper byte of the address
	// ADD CODE	
  //==============================================================
	statuscheck(i2cSendByte(i2c_base, h, I2C_MCS_START | I2C_MCS_RUN));

	if (!I2CMasterDatAck(i2c_base))
		return status;
  //==============================================================
  // Send the Lower byte of the address
	// ADD CODE
  //==============================================================		
	statuscheck(i2cSendByte(i2c_base, l, I2C_MCS_RUN | I2C_MCS_STOP));
	if (!I2CMasterDatAck(i2c_base))
		return status;
	
  //==============================================================
  // Set the I2C slave address to be the EEPROM and in Read Mode
	// ADD CODE
  //==============================================================
	statuscheck(i2cSetSlaveAddr(i2c_base, MCP24LC32AT_DEV_ID, I2C_READ)); 

  //==============================================================
  // Read the data returned by the EEPROM
	// ADD CODE
  //==============================================================
	statuscheck(i2cGetByte(i2c_base, data, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP));
	if (!I2CMasterDatAck(i2c_base))
		return status;
  
  return status;
}

//*****************************************************************************
// Initialize the I2C peripheral
//*****************************************************************************
bool eeprom_init(void)
{
  
  if(gpio_enable_port(EEPROM_GPIO_BASE) == false)
  {
    return false;
  }
  
  // Configure SCL 
  if(gpio_config_digital_enable(EEPROM_GPIO_BASE, EEPROM_I2C_SCL_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_alternate_function(EEPROM_GPIO_BASE, EEPROM_I2C_SCL_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_port_control(EEPROM_GPIO_BASE, EEPROM_I2C_SCL_PCTL_M, EEPROM_I2C_SCL_PIN_PCTL)== false)
  {
    return false;
  }
    

  
  // Configure SDA 
  if(gpio_config_digital_enable(EEPROM_GPIO_BASE, EEPROM_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_open_drain(EEPROM_GPIO_BASE, EEPROM_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_alternate_function(EEPROM_GPIO_BASE, EEPROM_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_port_control(EEPROM_GPIO_BASE, EEPROM_I2C_SDA_PCTL_M, EEPROM_I2C_SDA_PIN_PCTL)== false)
  {
    return false;
  }
    
  //  Initialize the I2C peripheral
  if( initializeI2CMaster(EEPROM_I2C_BASE)!= I2C_OK)
  {
    return false;
  }
  
  return true;
  
}
