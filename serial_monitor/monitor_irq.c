/*
 This work was not produced in affiliation with any of the device manufactures and is, and is intended to be,
 an independent, third-party research project.

 This work is presented for research and educational purposes only. Any use or reproduction of this work is at
 your sole risk. The work is provided “as is” and “as available”, and without warranties of any kind, whether
 express or implied, including, but not limited to, implied warranties of merchantability, non-infringement of
 third party rights, or fitness for a particular purpose.
*/

#include <stdint.h>
#include "monitor_addr.h"

#include "stm32f4xx.h"

/*
Serial Protocol:

 - All binary for speed
 - Very very limited support
 - Uses CRC8 to validate commands
 - Byte stuffing - 0xFE used as start of frame, 0x1B used as escape sequence

*/
#define SOF        0xFE
#define ESC        0x1B
#define ESC_SOF    0x01
#define ESC_EOT    0x02
#define ESC_ESC    0x03
#define CMD_MEMWR  0x10
#define CMD_MEMRD1 0x11
#define RSP_MEMRD1 0x12

#define FLAGS_ESCMODE (1<<1)
#define FLAGS_TX      (1<<3)

#define NEEDS_PACKING(a) ( (a == SOF) || (a == ESC) )
#define PACK_REPLACE(a)  if (a == SOF) {a = ESC_SOF;} else if (a == ESC) {a = ESC_ESC;}


#define USART3_RXNEIE_enable()  {USART3->CR1 |= (1<<5);}
#define USART3_RXNEIE_disable() {USART3->CR1 &= ~(1<<5);}
#define USART3_TXEIE_enable()   {USART3->CR1 |= (1<<7);}
#define USART3_TXEIE_disable()  {USART3->CR1 &= ~(1<<7);}

void USART3_IRQn_rxne(void);
void USART3_IRQn_txe(void);

void start(void)
{
    //TXE
    if(USART3->SR & (1 << 7)) {
        if (USART3->CR1 & (1<<7)){
            USART3_IRQn_txe();
        }
        USART3->SR |= (1 << 7);
    }

    //RXNE
    if(USART3->SR & (1 << 5)) {
        if (USART3->CR1 & (1<<5)){
            USART3_IRQn_rxne();
        }
        USART3->SR |= (1 << 5);
    }
}

void USART3_IRQn_rxne(void)
{
    uint8_t * charbuf = (uint8_t *)ADDR_CHARBUF;
    uint32_t * bufcnt = (uint32_t *)ADDR_BUFCNT;
    uint32_t * txcnt = (uint32_t *)ADDR_TXCNT;
    uint32_t * badcnt = (uint32_t *)ADDR_BADCNT;
    uint32_t * flags = (uint32_t *)ADDR_FLAGS;
    uint8_t c;
    uint8_t crc;
    
    //Table implementation of CRC8 is much faster...
    static const uint8_t CRC_8_TABLE[256] = 
    {
    0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
    35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
    190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
    70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
    219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
    17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
    50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
    87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53
    };
    
    
    (*badcnt)++;
    
    /* badcnt is how many characters received without an OK CRC result */
    if (*badcnt > 200) {
        //Shut it down boys
        USART3_RXNEIE_disable();
        goto fail;
    }
    
    if (*flags & FLAGS_TX){
        return;
    }
    
    if (*bufcnt > 10) {
        goto fail;
    }

    /* Read from USART */
    c = USART3->DR;
    
    if (c == 0x1B){
        *flags |= FLAGS_ESCMODE;
        return;
    }
    
    /* escmode means we are byte stuffing */
    if (*flags & FLAGS_ESCMODE) {
        *flags &= ~FLAGS_ESCMODE;
        if (c == ESC_SOF){
            /* 0xFE character (SOF) was in stream */
            *(charbuf + *bufcnt) = 0xFE;
            (*bufcnt)++;
            
        } else if (c == ESC_ESC) {
            /* ESC the escape */
            *(charbuf + *bufcnt) = ESC;
            (*bufcnt)++;
            
        } else if (c == ESC_EOT){
            /* EOT means check received stuff now */
            goto run;
            
        } else {
            /* Should not get here - bail out */
            goto fail;
        }
            
    } else {
        /* Check for SOF which overrides any other character in normal mode */
        if (c == SOF){
            /* SOF - start again. */
            *bufcnt = 0;
            *flags = 0;
        } else {
            /* Keep adding to array */
            *(charbuf + *bufcnt) = c;
            (*bufcnt)++;
        }
    }
    
    /* Nothing bad happened - waiting for more data */
    return;
    
    run:
        /* Try command */
        crc = 0;
        for(unsigned int i = 0; i < (*bufcnt); i++){
            crc = CRC_8_TABLE[crc ^ *(charbuf + i)];
        }
        
        /* CRC8 of CRC byte should result in 0 */
        if (crc != 0){
            goto fail;
        }
        
        uint32_t tempptr, tempdata;

        /* First byte is command... */
        switch(*(charbuf + 0)) {
            
            /* Command: Write to single memory address */
            case CMD_MEMWR:
                if (*bufcnt != 0xA) {
                    goto fail;
                }
            
                tempptr = *(charbuf + 1);
                tempptr |= *(charbuf + 2) << 8;
                tempptr |= *(charbuf + 3) << 16;
                tempptr |= *(charbuf + 4) << 24;
                
                tempdata = *(charbuf + 5);
                tempdata |= *(charbuf + 6) << 8;
                tempdata |= *(charbuf + 7) << 16;
                tempdata |= *(charbuf + 8) << 24;
                
                *flags = 0;  
                *((uint32_t *)tempptr) = tempdata;
                *bufcnt = 0;
                break;

            /* Command: Read from single memory address */
            case CMD_MEMRD1:
                if (*bufcnt < 5) {
                    goto fail;
                }
            
                tempptr = *(charbuf + 1);
                tempptr |= *(charbuf + 2) << 8;
                tempptr |= *(charbuf + 3) << 16;
                tempptr |= *(charbuf + 4) << 24;
                
                tempdata = *((uint32_t *)tempptr);
                *bufcnt = 2;
                
                *(charbuf + 0) = SOF;
                *(charbuf + 1) = RSP_MEMRD1;
                
                uint8_t b;
                
                crc = 0;
                crc = CRC_8_TABLE[crc ^ RSP_MEMRD1];
                crc = CRC_8_TABLE[crc ^ (tempdata & 0xff)];
                crc = CRC_8_TABLE[crc ^ ((tempdata >> 8) & 0xff)];
                crc = CRC_8_TABLE[crc ^ ((tempdata >> 16) & 0xff)];
                crc = CRC_8_TABLE[crc ^ ((tempdata >> 24)& 0xff)];
                

                /* Deal with possible byte stuffing if ESC or 0x1B in stream */

                //LSB
                b = tempdata & 0xff;
                if( NEEDS_PACKING(b) ) {
                    *(charbuf + *bufcnt) = ESC;
                    (*bufcnt)++;
                    PACK_REPLACE(b);
                }
                *(charbuf + *bufcnt) = b;
                (*bufcnt)++;
                
                //LSB2
                b = (tempdata >> 8)& 0xff;
                if( NEEDS_PACKING(b) ) {
                    *(charbuf + *bufcnt) = ESC;
                    (*bufcnt)++;
                    PACK_REPLACE(b);
                }
                *(charbuf + *bufcnt) = b;
                (*bufcnt)++;
                
                //MSB2
                b = (tempdata >> 16)& 0xff;
                if( NEEDS_PACKING(b) ) {
                    *(charbuf + *bufcnt) = ESC;
                    (*bufcnt)++;
                    PACK_REPLACE(b);
                }
                *(charbuf + *bufcnt) = b;
                (*bufcnt)++;
                
                //MSB
                b = (tempdata >> 24)& 0xff;
                if( NEEDS_PACKING(b) ) {
                    *(charbuf + *bufcnt) = ESC;
                    (*bufcnt)++;
                    PACK_REPLACE(b);
                }
                *(charbuf + *bufcnt) = b;
                (*bufcnt)++;
                
                //CRC
                if( NEEDS_PACKING(crc) ) {
                    *(charbuf + *bufcnt) = ESC;
                    (*bufcnt)++;
                    PACK_REPLACE(crc);
                }
                *(charbuf + *bufcnt) = crc;
                (*bufcnt)++;
                
                //Add EOT
                *(charbuf + *bufcnt) = ESC;
                (*bufcnt)++;
                                
                *(charbuf + *bufcnt) = ESC_EOT;
                (*bufcnt)++;
                
                *txcnt = 0;

                //Turn off RX mode for now
                USART3_RXNEIE_disable();

                //TX mode now
                *flags = FLAGS_TX; 
                
                //Turn on interrupts for TX empty
                USART3_TXEIE_enable();
                break;
                
            default:
                goto fail;
        }
        
        *badcnt = 0;
        return;
    
    /* Non-critical failure */
    fail:
        *bufcnt = 0;
        *flags = 0;
        return;
}

void USART3_IRQn_txe(void)
{
    uint8_t * charbuf = (uint8_t *)ADDR_CHARBUF;
    uint32_t * bufcnt = (uint32_t *)ADDR_BUFCNT;
    uint32_t * txcnt = (uint32_t *)ADDR_TXCNT;
    uint32_t * flags = (uint32_t *)ADDR_FLAGS;
    
    if (*txcnt < *bufcnt){        
        USART3->DR = *(charbuf + *txcnt);        
        (*txcnt)++;
    } else {
        USART3_TXEIE_disable();        
        *flags = 0;
        *bufcnt = 0;
        *txcnt = 0;
        USART3_RXNEIE_enable();
    }
}

