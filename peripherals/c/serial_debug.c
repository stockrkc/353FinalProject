//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "serial_debug.h"

static bool Rx_Interrupts_Enabled = false;
static bool Tx_Interrupts_Enabled = false;


PC_Buffer UART0_Tx_Buffer;
PC_Buffer UART0_Rx_Buffer;


//************************************************************************
// Configures the serial debug interface at 115200.
// UART IRQs can be anbled using the two paramters to the function.
//************************************************************************
bool init_serial_debug(bool enable_rx_irq, bool enable_tx_irq)
{
  
  // Initialize the GPIO pins used for the serial debug interface
	gpio_enable_port(SERIAL_DBG_GPIO_BASE);
	
	//Initializing PA0 and PA1 to be UART pins
	
	//enbling pins as digital pins
	gpio_config_digital_enable(GPIOA_BASE, PA0);
	gpio_config_digital_enable(GPIOA_BASE, PA1);

	//configuring the pins as alternate functions
	if(!gpio_config_alternate_function(GPIOA_BASE, PA0)){
		return false;
	}
	if(!gpio_config_alternate_function(GPIOA_BASE, PA1)){
		return false;
	}
	//Setting PA0 to be the recieve line of UART0
	if(!gpio_config_port_control(GPIOA_BASE, PA0, GPIO_PCTL_PA0_U0RX)){
		return false;
	}
	//Setting PA1 to be the transmit line of UART0
	if(!gpio_config_port_control(GPIOA_BASE, PA1, GPIO_PCTL_PA1_U0TX)){
		return false;
	}
	
  Rx_Interrupts_Enabled = enable_rx_irq;
  Tx_Interrupts_Enabled = enable_tx_irq;
  
  // Initialize the circular buffer(s)
  pc_buffer_init(&UART0_Tx_Buffer, UART_BUFFER_SIZE);
  pc_buffer_init(&UART0_Rx_Buffer, UART_BUFFER_SIZE); 
  
  
  if( uart_init(UART0_BASE,enable_rx_irq, enable_tx_irq) == false)
  { 
    return false;
  }
  
  return true;
}

/****************************************************************************
 *
 ****************************************************************************/
int serial_debug_rx(PC_Buffer *rx_buffer, bool block)
{
   int c;	

   while (pc_buffer_empty(rx_buffer))
   {
      if (!block)
         return -1;
   }

   // Remove data from the circular buffer.  Make sure this is an
   // atomic operation.  Disable ALL interrupts when modifying the circular 
   // buffer.  See main.c to determine how to enable/disable interrupts.

	 DisableInterrupts();	//Disabling the interrupts and beginning buffer dequeue
	 pc_buffer_remove(rx_buffer, (char *) &c);
	 EnableInterrupts();	//Buffer remove data done, re-enabling the interrupts back
	 
   return c;
}

/****************************************************************************
 *
 ****************************************************************************/
void serial_debug_tx(uint32_t uart_base, PC_Buffer *tx_buffer, int data)
{
  UART0_Type *uart = (UART0_Type *)(uart_base);
  bool  tx_buffer_full;
  bool  tx_buffer_empty;
  bool  fifo_full;
  
  // Determine if the hw fifo is full
  fifo_full = uart -> FR & UART_FR_TXFF; 
  
  // Determine if the pc_buffer is empty
  tx_buffer_empty = pc_buffer_empty(tx_buffer); 
  
  // If there is space in the hardwere FIFO, and the circular
  // buffer is empty, send the data to the FIFO.
  if( tx_buffer_empty & (!fifo_full) )
  {
    // write data to HW FIFO
		uart -> DR = data;
  }
  else
  {
    // Test to see if the circular buffer is full
    // If it is, we wait until there is space.
    do
    {
        // determine if the tx circular buffer is full
        tx_buffer_full = pc_buffer_full(tx_buffer);
    } while(tx_buffer_full);

    // Add the character to the circular buffer.   Make sure this is an
    // atomic operation.  Disable ALL interrupts when modifying the circular 
    // buffer. See main.c to determine how to enable/disable interrupts.
		DisableInterrupts();
		pc_buffer_add(tx_buffer, data);
		EnableInterrupts();
    
  }
  
  // ADD CODE
  // If you're in this function, you want to send data
  // so enable TX interrupts in the UART IM register even if they 
  // are already enabled.  
	uart -> IM |= UART_IM_TXIM;
  return;
}


//****************************************************************************
//  This function is called from MicroLIB's stdio library.  By implementing
//  this function, MicroLIB's getchar(), scanf(), etc will now work.
// ****************************************************************************/
int fgetc(FILE* stream)
{
   char c;

   
   if ( Rx_Interrupts_Enabled)
   {
    c = serial_debug_rx(&UART0_Rx_Buffer, true);
   }
   else
   {
     c = uart_rx_poll(UART0_BASE,true);
   }

   if (c == '\r')
      c = '\n';

   fputc(c, stdout);

   return c;
}

//****************************************************************************
// This function is called from MicroLIB's stdio library.  By implementing
// this function, MicroLIB's putchar(), puts(), printf(), etc will now work.
// ****************************************************************************/
int fputc(int c, FILE* stream)
{
   uint32_t uart_base;
   PC_Buffer *tx_buffer;

   if ( Tx_Interrupts_Enabled)
   {
      serial_debug_tx(UART0_BASE, &UART0_Tx_Buffer, c);
   }
   else
   {
      uart_tx_poll(UART0_BASE, c);
   }

   if (c == '\n')
   {
      if ( Tx_Interrupts_Enabled)
      {
        serial_debug_tx(UART0_BASE, &UART0_Tx_Buffer,'\r');
      }
      else
      {
         uart_tx_poll(UART0_BASE, '\r');
      }
   }

   return c;
}

//*****************************************************************************
//*****************************************************************************
// UART ISR Handler
//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
// Rx Portion of the UART ISR Handler
//*****************************************************************************
__INLINE static void UART_Rx_Flow(uint32_t uart_base, PC_Buffer *rx_buffer)
{
  UART0_Type *uart = (UART0_Type *)(uart_base);
  
  // Remove entries from the RX FIFO and place them in the circular buffer.  Return
  // once the RX FIFO is empty.
	while(!(uart -> FR & UART_FR_RXFE)){
		//add in the buffer only if it is not full
		if(! pc_buffer_full(rx_buffer)){
		pc_buffer_add(rx_buffer, uart -> DR);
		}
	}
  // Clear the RX interrupts so it can trigger again when the hardware 
  // FIFO becomes full
  uart -> ICR |= UART_ICR_RXIC | UART_ICR_RTIC;
	
}

//*****************************************************************************
// Tx Portion of the UART ISR Handler
//*****************************************************************************
__INLINE static void UART_Tx_Flow(uint32_t uart_base, PC_Buffer *tx_buffer)
{
    UART0_Type *uart = (UART0_Type *)(uart_base);    
    char c;
    bool tx_buffer_empty;
  
   // Check to see if we have any data in the circular buffer
    tx_buffer_empty = pc_buffer_empty(tx_buffer);
  
    if( !tx_buffer_empty)
    {
        // Move data from the circular queue to the hardware FIFO
        // until the hardware FIFO is full OR the circular buffer
        // becomes empty.
			do{
					pc_buffer_remove(tx_buffer, (char *) &c);
					uart -> DR = c;
					tx_buffer_empty = pc_buffer_empty(tx_buffer);
				
				if( (uart -> FR & UART_FR_TXFF) ){
					uart -> ICR = UART_ICR_TXIC;
					return;
				}
		}while(!tx_buffer_empty);
	}
    else
    {
      // Disable the TX interrupts.
			uart -> IM &= ~UART_IM_TXIM;
    }
    
    // Clear the TX interrupt so it can trigger again when the hardware
    // FIFO is empty
		uart -> ICR |= UART_ICR_TXIC;

}

//*****************************************************************************
// UART0 Interrupt Service handler
//*****************************************************************************
void UART0_Handler(void)
{
    uint32_t  status;
    uint32_t  rx_mask   = 0 ;
		uint32_t  tx_mask 	= 0;
	
    // Read the interrupt status of the UART
    status = UART0 -> MIS;

    // set rx_mask to detect both Rx related interrupts.
    rx_mask = UART_MIS_RXMIS | UART_MIS_RTMIS; 	//MIGHT BE WRONG IDK /*modify*/
		
		//set tx_mask to detect Tx related interrupts
		tx_mask = UART_MIS_TXMIS;
	
    if ( status & rx_mask)
    {
       UART_Rx_Flow(UART0_BASE, &UART0_Rx_Buffer);
    }
		else if (status & tx_mask){
			 UART_Tx_Flow(UART0_BASE, &UART0_Tx_Buffer);
		}
    
    return;
}
