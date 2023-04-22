//*****************************************************************************
// sci_common.c: Common used SCI functions
// Date        : 9-19-2012
//
/* Copyright (C) 2013-2019 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
//*****************************************************************************

#include "bl_config.h"

#if defined (RM57) || defined (TMS570LC43)
#include "HL_sci.h"
#else
#include "sci.h"
#endif

#include "sci_common.h"

/**
  * @brief  Convert an Integer to a string
  * @param  str: The string
  * @param  intnum: The intger to be converted
  * @retval None
  */
void Int2Str(char* str, int intnum)
{
  uint32_t i, Div = 1000000000, j = 0, Status = 0;

  for (i = 0; i < 10; i++)
  {
    str[j++] = (intnum / Div) + 48;

    intnum = intnum % Div;
    Div /= 10;
    if ((str[j-1] == '0') & (Status == 0))
    {
      j = 0;
    }
    else
    {
      Status++;
    }
  }
}

/**
  * @brief  Convert a string to an integer
  * @param  inputstr: The string to be converted
  * @param  intnum: The intger value
  * @retval 1: Correct
  *         0: Error
  */
uint32_t Str2Int(unsigned char *inputstr, int *intnum)
{
  uint32_t i = 0, res = 0;
  uint32_t val = 0;

  if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
  {
    if (inputstr[2] == '\0')
    {
      return 0;
    }
    for (i = 2; i < 11; i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1; */
        res = 1;
        break;
      }
      if (ISVALIDHEX(inputstr[i]))
      {
        val = (val << 4) + CONVERTHEX(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* over 8 digit hex --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }
  else /* max 10-digit decimal input */
  {
    for (i = 0;i < 11;i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1 */
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
      {
        val = val << 10;
        *intnum = val;
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
      {
        val = val << 20;
        *intnum = val;
        res = 1;
        break;
      }
      else if (ISVALIDDEC(inputstr[i]))
      {
        val = val * 10 + CONVERTDEC(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 10 digit decimal --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }

  return res;
}

/**
  * @brief  Get a key from the HyperTerminal
  * @param  None
  * @retval The Key Pressed
  */
char UART_getKey(sciBASE_t *sci)
{
  char key = 0;

  /* Waiting for user input */
  while (1)
  {
	  if ((sci->FLR & SCI_RX_INT) != 0)
	  {
	    key = (char)sci->RD;
	    break;
	  }
  }
  return key;
}


/**
  * @brief  Print a character on the HyperTerminal
  * @param  c: The character to be printed
  * @retval None
  */
int UART_getChar(sciBASE_t *sci, uint32_t timeout)
{
	  char c;
	  while (timeout-- > 0)
	  {
		  if ((sci->FLR & SCI_RX_INT) != 0)
		  {
		     c = (char)sci->RD;
		     return c;
		  }
	  }
	  return (-1);
}
/**
  * @brief  Receive byte from sender
  * @param  c: Character
  * @param  timeout: Timeout
  * @retval 0: Byte received
  *         -1: Timeout
  */
int UART_rxByte (sciBASE_t *sci, char *c, uint32_t timeout)
{
  while (timeout-- > 0)
  {
	  if ((sci->FLR & SCI_RX_INT) != 0)
	  {
	    *c = (char)sci->RD;
	    return 0;
	  }
	  else
	  {
		  *c = 0x00;
	  }
  }
  return -1;
}


/**
  * @brief  Send a byte
  * @param  c: Character
  * @retval 0: Byte sent
  */
uint32_t UART_txByte (sciBASE_t *sci, char c)
{
	  while (!(sci->FLR & 0x800));
	  sci->TD = c;
      return 0;
}

/** @fn void UART_send32BitData(sciBASE_t *sci, uint32_t data)
*
*	Function used for send a 32bit data on to the SCI at the
*   configured baud rate
*/

const char *U32toStr(uint32_t val)
{
        /* Maximum number of decimal digits in u32 is 10 */
        static char num_str[11];
        int  pos = 10;
        num_str[10] = 0;

        if (val == 0) {
                /* If already zero then just return zero */
                return "0";
        }

        while ((val != 0) && (pos > 0)) {
                num_str[--pos] = (val % 10) + '0';
                val /= 10;
        }

        return &num_str[pos];
}



void UART_send32BitData(sciBASE_t *sci, uint32_t data)
{
	uint8_t c_get ;
  	volatile int i = 0;
  	for( i = 8 ; i > 0 ; i-- )
  	{
 		c_get = (data >> 28) & 15 ;
		if( c_get > 9 )
			c_get += 7 ;
		c_get += 48 ;

		while ((sci->FLR & SCI_TX_INT) == 0) { /* wait */ };
		sci->TD = c_get;
		data = data << 4 ;
	}
}

/**
  * @brief  Print a string on the HyperTerminal
  * @param  s: The string to be printed
  * @retval None
  */
void UART_putString(sciBASE_t *sci, char *s)
{
  while (*s != '\0')
  {
    UART_putChar(sci, *s);
    s++;
  }
}


/**
  * @brief  Print a character on the HyperTerminal
  * @param  c: The character to be printed
  * @retval None
  */
void UART_putChar(sciBASE_t *sci, char c)
{
	  while (!(sci->FLR & 0x800));
	  sci->TD = c;
  {
  }
}
