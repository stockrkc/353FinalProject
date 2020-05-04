#include <stdlib.h>
#include "ioexpander.h"
#include "io_expander.h"
#define statuscheck(st) if ((status = st) != I2C_OK) return status;
#define statuscheckbool(st) if ((status = st) != I2C_OK) return false;

io_expander_t *io_expander_last_read;

bool io_expander_init(void) {
	bool base, scl, sda, master, irq;
	i2c_status_t status;
	io_expander_last_read = malloc(sizeof(io_expander_t));
	io_expander_last_read->valid = false;
  
  base = gpio_enable_port(IO_EXPANDER_GPIO_BASE);
  	
  scl = gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN) &&
  	gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN) &&
  	gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PCTL_M, IO_EXPANDER_I2C_SCL_PIN_PCTL);
  

  sda = gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN) &&
	  gpio_config_open_drain(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN) &&
	  gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN) &&
	  gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PCTL_M, IO_EXPANDER_I2C_SDA_PIN_PCTL);
    
  master = i2cVerifyBaseAddr(IO_EXPANDER_I2C_BASE) && (initializeI2CMaster(IO_EXPANDER_I2C_BASE)== I2C_OK);
	

	// Ensure interrupts are off.
	((GPIOA_Type*)IO_EXPANDER_IRQ_GPIO_BASE)->IM &= ~IO_EXPANDER_IRQ_PIN_NUM;
	
	// Wait for availability, then start talking to mcp
  while ( I2CMasterBusy(IO_EXPANDER_I2C_BASE)) {};
	do {
			status = i2cSetSlaveAddr(IO_EXPANDER_I2C_BASE, MCP23017_DEV_ID, I2C_WRITE);
			while ( I2CMasterBusy(IO_EXPANDER_I2C_BASE)) {};
	} while (I2CMasterAdrAck(IO_EXPANDER_I2C_BASE) == false);
	
	// configure porta as output, portb as input.
	io_expander_write_reg(MCP23017_IODIRA_R, 0x00);
	io_expander_write_reg(MCP23017_IODIRB_R, 0xFF);
	
	// pullups on port B, invert the polarity so we read 1 as push,
	// default value of 00 is irrelevant as we use intcond/lastval
	io_expander_write_reg(MCP23017_GPPUB_R, 0xFF);	
	io_expander_write_reg(MCP23017_IPOLB_R, 0xFF);
	io_expander_write_reg(MCP23017_DEFVALB_R, 0x00);
	// The condition for interrupt is irreelvant because of what is to follow.
	io_expander_write_reg(MCP23017_IOCONA_R, 0x00);
	io_expander_write_reg(MCP23017_IOCONB_R, 0x00);
	
	// configure interrupts on port b, none on port A.
	io_expander_write_reg(MCP23017_INTCONA_R, 0x00);

	// generate interrupt when the thing changes value relative to its old value
	io_expander_write_reg(MCP23017_INTCONB_R, 0x00);
	// Interrupts on all switches and buttons
	io_expander_write_reg(MCP23017_GPINTENB_R, 0xFF);
	// No interrupts for those output ports, irrelevant because dir is 00 but do it anyway.
	io_expander_write_reg(MCP23017_GPINTENA_R, 0x00);




	// Now we are clear to enable interrupts on our side.
  irq = gpio_enable_port(IO_EXPANDER_IRQ_GPIO_BASE) &&
	gpio_config_falling_edge_irq(IO_EXPANDER_IRQ_GPIO_BASE, IO_EXPANDER_IRQ_PIN_NUM);
	// In the nested vector interrupt controller, too. 
	NVIC_SetPriority(GPIOF_IRQn, 2);
	NVIC_EnableIRQ(GPIOF_IRQn);	
	
	// Turn off the LEDs by default, in case of garbage
	io_expander_write_reg(MCP23017_GPIOA_R, 0x00);

	// Done
  return base && scl && sda && master && irq && (status == I2C_OK);
}
void io_expander_write_reg(uint8_t reg, uint8_t data) {
	i2c_status_t status;
		// Wait for availability, then start talking to mcp
  while ( I2CMasterBusy(IO_EXPANDER_I2C_BASE)) {};
	do {
			status = i2cSetSlaveAddr(IO_EXPANDER_I2C_BASE, MCP23017_DEV_ID, I2C_WRITE);
			while ( I2CMasterBusy(IO_EXPANDER_I2C_BASE)) {};
	} while (I2CMasterAdrAck(IO_EXPANDER_I2C_BASE) == false);
	i2cSendByte(IO_EXPANDER_I2C_BASE, reg,  
								I2C_MCS_START | I2C_MCS_RUN);
  if (!I2CMasterDatAck(IO_EXPANDER_I2C_BASE)) {
		printf("failed to ack data");
	}
			
  while ( I2CMasterBusy(IO_EXPANDER_I2C_BASE)) {};
  i2cSendByte(IO_EXPANDER_I2C_BASE, data, I2C_MCS_RUN | I2C_MCS_STOP);
	if (!I2CMasterDatAck(IO_EXPANDER_I2C_BASE)) {
		printf("failed to ack data");
	}
	if (status != I2C_OK)
		printf("i2c status not ok");
}
uint8_t io_expander_read_reg(uint8_t reg) { 
	i2c_status_t status;
	uint8_t data;
		// Wait for availability, then start talking to mcp
  while ( I2CMasterBusy(IO_EXPANDER_I2C_BASE)) {};
	do {
			status = i2cSetSlaveAddr(IO_EXPANDER_I2C_BASE, MCP23017_DEV_ID, I2C_WRITE);
			while ( I2CMasterBusy(IO_EXPANDER_I2C_BASE)) {};
	} while (I2CMasterAdrAck(IO_EXPANDER_I2C_BASE) == false);
	i2cSendByte(IO_EXPANDER_I2C_BASE, reg,  
								I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
  if (!I2CMasterDatAck(IO_EXPANDER_I2C_BASE))
			printf("failed to ack data");

	do {
				status = i2cSetSlaveAddr(IO_EXPANDER_I2C_BASE, MCP23017_DEV_ID, I2C_READ);
				while ( I2CMasterBusy(IO_EXPANDER_I2C_BASE)) {};
		} while (I2CMasterAdrAck(IO_EXPANDER_I2C_BASE) == false);
	
  statuscheckbool(i2cGetByte(IO_EXPANDER_I2C_BASE, &data, I2C_MCS_START |I2C_MCS_RUN | I2C_MCS_STOP));
	if (status != I2C_OK)
		printf("i2c status not ok");		
	return data;
}

void io_expander_set_leds(uint8_t mask) {
	io_expander_write_reg(MCP23017_GPIOA_R, mask);
}
