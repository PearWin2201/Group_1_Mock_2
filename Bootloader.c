

#include"MKL46Z4.h"
#include"driver/Flash.h"
#include"driver/Port.h"
#include"driver/UART0.h"

typedef struct
{
		uint8_t SrecType[2];
		uint16_t ByteCount;
	    uint8_t AddressNum;
		uint32_t Address;
		uint32_t F_Data;      // Could be managed via Queue
		uint8_t Checksum;
} SrecType;

typedef enum {
	    SREC_FLAG_DEFAULT     = 0U,
	    SREC_FLAG_FIRST       = 1U,
	    SREC_FLAG_TYPE        = 2U,
	    SREC_FLAG_BYTECOUNT1  = 3U,
	    SREC_FLAG_BYTECOUNT2  = 4U,
		SREC_FLAG_ADDRESS     = 5u,
	    SREC_FLAG_DATA        = 6U,
		SREC_FLAG_CHECKSUM1   = 7u,
		SREC_FLAG_CHECKSUM2   = 8u
} Srec_Flag;

#define DEFAULT_STATE       0u
#define LOAD_STATE_WAIT     1u
#define LOAD_STATE          2u
#define ERASE_STATE_WAIT    3u
#define ERASE_STATE         4u
#define RESTORE_STATE_WAIT  5u
#define RESTORE_STATE       6u

uint8_t Flash_Flag = 0;
uint8_t	Boot_State = DEFAULT_STATE;
uint8_t	Boot_Flag = SREC_FLAG_DEFAULT;
SrecType Srectype;
uint8_t count_bytecount = 0;
uint8_t count_data = 1;
uint8_t count_address = 0;
uint8_t count = 1;

uint8_t SREC_ChartoDec(uint8_t Character)
{
	uint8_t retVal;
	// '0' - '9'
	if ( ('0' <= Character) && (Character <= '9') )
	{
	 retVal = Character - '0';
	}
	else if ( ('A' <= Character) && (Character <= 'F') )
	{
	 retVal = Character - 'A' + 10;
	}
	else
	{
	 retVal = 0;
	}
	return retVal;
}

volatile uint8_t S_Data;

void UART0_IRQHandler(void)
{
	 S_Data = UART0->D;

	if (Boot_State == LOAD_STATE)
	{
	        // SREC S113A00000E8FF1F
	        // Could be used switch
	    if (S_Data == 'S')
	    {
	    	Boot_Flag = SREC_FLAG_FIRST;
	    }

		switch(Boot_Flag)
		{
		    case SREC_FLAG_FIRST:
		    	{
		    		Srectype.SrecType[0] = S_Data;
		    		Boot_Flag = SREC_FLAG_TYPE;
		    		break;
		    	}
		    case SREC_FLAG_TYPE:
		    	Srectype.SrecType[1] = S_Data;
		    	if ((Srectype.SrecType[1]) == 1)
		    	{
		    		Srectype.AddressNum = 2;
		    	}
		    	if ((Srectype.SrecType[1]) == 2)
		    	{
		    		Srectype.AddressNum = 3;
		    	}
		    	if ((Srectype.SrecType[1]) == 3)
		    	{
		    		Srectype.AddressNum = 4;
		    	}
		    	count_address = ((Srectype.AddressNum) * 2);
		    	Boot_Flag = SREC_FLAG_BYTECOUNT1;
		    	break;
		    case SREC_FLAG_BYTECOUNT1:
		        {
		        	Srectype.ByteCount |= SREC_ChartoDec(S_Data) << 4;
		        	Boot_Flag = SREC_FLAG_BYTECOUNT2 ;
		        	break;
		        }
		    case SREC_FLAG_BYTECOUNT2:
		        {
		    	    Srectype.ByteCount |= SREC_ChartoDec(S_Data);
		    	    Boot_Flag = SREC_FLAG_ADDRESS;
		    	    count_bytecount = (((Srectype.ByteCount) - (Srectype.AddressNum + 1)) * 2);
		    	    break;
		        }
		    case SREC_FLAG_ADDRESS:
		    	switch(count)
		    	{
		    	    case 1:
		    	    {
		    	    	Srectype.Address |= (SREC_ChartoDec(S_Data)<<(((count_address) - count)*4));
		    	    	count++;
		    	    	count_address--;
		    	    	 break;
		    	    }
		    	    case 2:
		    	    {
		    	    	Srectype.Address |= (SREC_ChartoDec(S_Data)<<(((count_address) - count)*4));
		    	    	count++;
		    	    	count_address--;
		    	    	 break;
		    	    }
		    	    case 3:
		    	    {
		    	    	Srectype.Address |= (SREC_ChartoDec(S_Data)<<(((count_address) - count)*4));
		    	    	count++;
		    	    	count_address--;
		    	    	 break;
		    	    }
		    	    case 4:
		    	    {
		    	    	Srectype.Address |= (SREC_ChartoDec(S_Data)<<(((count_address) - count)*4));
		    	    	count++;
		    	    	count_address--;
		    	    	if(count_address == 0)
		    	    	{
		    	    		count = 1;
		    	    		Boot_Flag = SREC_FLAG_DATA;
		    	    	}
		    	    	 break;
		    	    }
		    	    case 5:
		    	    {
		    	    	Srectype.Address |= (SREC_ChartoDec(S_Data)<<(((count_address) - count)*4));
		    	    	count++;
		    	    	count_address--;
		    	    	break;
		    	    }
		    	    case 6:
		    	    {
		    	    	Srectype.Address |= (SREC_ChartoDec(S_Data)<<(((count_address) - count)*4));
		    	    	count++;
		    	    	count_address--;
		    	    	if(count_address == 0)
		    	    	{
		    	    		count = 1;
		    	    		Boot_Flag = SREC_FLAG_DATA;
		    	    	}
		    	    	break;
		    	    }
		    	    case 7:
		    	    {
		    	    	Srectype.Address |= (SREC_ChartoDec(S_Data)<<(((count_address) - count)*4));
		    	    	count++;
		    	    	count_address--;
		    	    	break;
		    	    }
		    	    case 8:
		    	    {
		    	    	Srectype.Address |= (SREC_ChartoDec(S_Data)<<(((count_address) - count)*4));
		    	    	count++;
		    	    	count_address--;
		    	    	{
		    	    		count = 1;
		    	    		Boot_Flag = SREC_FLAG_DATA;
		    	    	}
		    	    	break;
		    	    }
		    	    break;
		    	}
		    case SREC_FLAG_DATA:
		        {
                    switch(count_data)
                    {
                        case 1:
					    {
					    	Srectype.F_Data |= (SREC_ChartoDec(S_Data) << (32 - (count_data * 4)));
					    	count_data++;
					    	count_bytecount--;
					    	break;
					    }
                        case 2:
                        {
					    	Srectype.F_Data |= (SREC_ChartoDec(S_Data) << (32 - (count_data * 4)));
					    	count_data++;
					    	count_bytecount--;
					    	break;
                        }
                        case 3:
                        {
					    	Srectype.F_Data |= (SREC_ChartoDec(S_Data) << (32 - (count_data * 4)));
					    	count_data++;
					    	count_bytecount--;
					    	break;
                        }
                        case 4:
                        {
					    	Srectype.F_Data |= (SREC_ChartoDec(S_Data) << (32 - (count_data * 4)));
					    	count_data++;
					    	count_bytecount--;
					    	break;
                        }
                        case 5:
                        {
					    	Srectype.F_Data |= (SREC_ChartoDec(S_Data) << (32 - (count_data * 4)));
					    	count_data++;
					    	count_bytecount--;
					    	break;
                        }
                        case 6:
                        {
					    	Srectype.F_Data |= (SREC_ChartoDec(S_Data) << (32 - (count_data * 4)));
					    	count_data++;
					    	count_bytecount--;
					    	break;
                        }
                        case 7:
                        {
					    	Srectype.F_Data |= (SREC_ChartoDec(S_Data) << (32 - (count_data * 4)));
					    	count_data++;
					    	count_bytecount--;
					    	break;
                        }
                        case 8:
                        {
					    	Srectype.F_Data |= (SREC_ChartoDec(S_Data) << (32 - (count_data * 4)));
					    	count_data = 1;
					    	Flash_Flag = 1;
					    	count_bytecount--;
                        	if (count_bytecount == 0)
                        	{
                        		Boot_Flag = SREC_FLAG_CHECKSUM1;
                        		Srectype.Address = 0;
                        	}
					    	break;
                        }
                        break;
                    }
		        }

		    case SREC_FLAG_CHECKSUM1:
		        {
		        	Srectype.Checksum |= SREC_ChartoDec(S_Data)<<4;
		        	Boot_Flag = SREC_FLAG_CHECKSUM2;
		        	break;
		        }
		    case SREC_FLAG_CHECKSUM2:
		        {
		        	Srectype.Checksum |= SREC_ChartoDec(S_Data);
		        	Boot_Flag = SREC_FLAG_DEFAULT;
		        	Srectype.Address = 0;
		        	Srectype.ByteCount = 0;
		        	break;
		        }
		}
	}
	else
	{   // Command
	    switch (S_Data)
	    {
	        case 'L':
	        	{
	            Boot_State = LOAD_STATE_WAIT ;
	            Boot_State++;                //LOAD_STATE (2)
	            break;
	        	}
	        case 'E':
	        	{
	            Boot_State = ERASE_STATE_WAIT ;
	            Boot_State++;                // ERASE_STATE (4)
	            break;
	        	}
	        case 'R':
	            {
	            Boot_State = RESTORE_STATE_WAIT ;
	            Boot_State++;                // RESTORE_STATE (6)
	            break;
	            }
	        default:
	        {
	            if (Boot_State != DEFAULT_STATE)
	            {
	               uint8_t Start_Sector = S_Data;
	               Boot_State++;
	            }
	            break;
	        }
	    }
	}
}

void main()
{
	UART0_Init();
	UART_ReceiveINT();
	UART_SendString("HLL", 4);

	while(1)
	{
	    switch (Boot_State)
	    {
	        case ERASE_STATE:
	        	Flash_EraseSector(10);
	             break;
	        case RESTORE_STATE:
	        	Run_In_Application();
	             break;
	        case LOAD_STATE:
	        	if(Flash_Flag == 1)
	        	{
	        		// write data to flash
	        		Flash_WriteWord(Srectype.Address, Srectype.F_Data);
	        		Srectype.Address += 0x4 ;
	        		Srectype.F_Data = 0;
	        	}
	        	break;
	     }
	 }
}



