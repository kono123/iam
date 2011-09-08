#include <hardware.h>
//#include <lmx9820.h>
#include <intrinsics.h>

#include "GAP_Commands.h"
#include "LMX_Commands.h"
#include "ERROR_Handle.h"
#include "SDAP_Commands.h"
#include "SPP_Commands.h"

#include <stdio.h>

#define bitTx 1
#define bitRx 0

#define LCD_CS_Activate()          PORT_LO(5,bitCS)
#define LCD_CS_Diactivate()        PORT_HI(5,bitCS)
#define LCD_A0_LO()                PORT_LO(5,bitA0)
#define LCD_A0_HI()                PORT_HI(5,bitA0)

// Defines
#define MAIN_TIMER_PRESCALER_1MS  8000      // 1ms @ 8MHz
#define MAIN_ONE_SECOND           1000      // 1s
#define MAIN_TIMEOUT_LMX            10      // 10ms


// Variables
LMX_Command_Protocol_t* MAIN_LMX_Command_Protocol;
uint16_t                MAIN_RX_Lenght_LMX;
uint16_t                MAIN_Help_LMX;
uint8_t                 MAIN_Data_Array[50];
uint8_t                 MAIN_UART1_TX_Buffer[200];
uint8_t                 MAIN_Config_Statecounter;
uint8_t                 MAIN_Transparent_Mode;
uint8_t*                MAIN_P_UART1_TX_Write_Pointer;
uint8_t*                MAIN_P_UART1_TX_Read_Pointer;
uint16_t                MAIN_UART_TX_Buffer_Load;
uint16_t                MAIN_Config_Timer;
uint16_t                MAIN_Protocol_Timer;

enum
{
WAIT_STX =        0,
WAIT_TYPE =       1,
WAIT_OPCODE =     2,
WAIT_LENGHT_LSB = 3,
WAIT_LENGHT_MSB = 4,
WAIT_CRC =        5,
WAIT_DATA =       6,
WAIT_ETX =        7,
}
MAIN_Check_Protocol_LMX_Statecounter;

void BlueTooth_PowerOn(void)
{
  P1OUT |= (1<<6);
  P1DIR |= (1<<6);
  delay_ms(20);
}

void BlueTooth_PowerOff(void)
{
  P1OUT &= ~(1<<6);
  P1DIR &= ~(1<<6);
}

const char StrName[]={2,0x52,4,8,0,0x5E,7,'I','A','M','-','0','1',0,3};

void BlueTooth_Init(void)
{
  uint8_t i;
  uint16_t Baud_Register;
//  float_t ybin1;
//  float_t ybin2;
  uint8_t Overflow = 1;

  printf("BT Init!!\n");

  __disable_interrupt();

  P3SEL |= 0xC0; //Primary peripheral

// UART Init  9600/8/N/1 @ 4MHz

  UCA1CTL1 |= UCSWRST; // reset

//  UCA1CTL0 =  UCSYNC; //
  UCA1CTL0 =  0;
  UCA1CTL1 |= UCSSEL_3;  // UCLK = SMCLK

//  ME2 &= ~(UTXE1 + URXE1);
//  IE2 &= ~(UTXIE1 + URXIE1);
//  UTCTL1 = SSEL1;					// UCLK = SMCLK
//  U1CTL = CHAR;

  Baud_Register = SM_CLK / 115200;

  UCA1BR0 = Baud_Register & 0xFF;
  UCA1BR1 = (Baud_Register >> 8) & 0xFF;

//  UCA1MCTL=0x21;
//  UCA1MCTL = UCBRS_6;                       // Modulation UCBRSx = 3
  UCA1MCTL = UCBRF_0 + UCBRS_4; //UCOS16  // Modulation

  UC1IE |= UCA1RXIE;                        // Enable USCI_A1 RX interrupt

  UCA1CTL1 &= ~UCSWRST;

  __enable_interrupt();

  MAIN_Transparent_Mode = 0;
  MAIN_Config_Timer = 0;
  MAIN_Check_Protocol_LMX_Statecounter = WAIT_STX;
}

void MAIN_Msg_Handling_LMX(LMX_Command_Protocol_t* Msg)
{
  uint16_t Msg_Type;

  Msg_Type = (Msg->Op_code << 8) & 0xff00;
  Msg_Type += Msg->Packet_Type_identification;

  switch (Msg_Type)
  {
    case DEVICE_READY_IND:
        {
          // Now is the LMX ready
          //  Data don't care
            switch (MAIN_Config_Statecounter)
            {
            // First Restore LMX Factory Settings
            case 1:
                Restore_Factory_Settings_REQ();
            break;
            // Second set Device Name
            case 2:
              {
                GAP_WRITE_LOCAL_NAME_REQ_t* GAP_Request;
                GAP_Request = (GAP_WRITE_LOCAL_NAME_REQ_t*)MAIN_Data_Array;

                GAP_Request->NameLength = 12;

                GAP_Request->DeviceName[0]   = 'A';
                GAP_Request->DeviceName[1]   = 'M';
                GAP_Request->DeviceName[2]   = 'B';
                GAP_Request->DeviceName[3]   = 'E';
                GAP_Request->DeviceName[4]   = 'R';
                GAP_Request->DeviceName[5]   = ' ';
                GAP_Request->DeviceName[6]   = 'S';
                GAP_Request->DeviceName[7]   = 'l';
                GAP_Request->DeviceName[8]   = 'a';
                GAP_Request->DeviceName[9]   = 'v';
                GAP_Request->DeviceName[10]  = 'e';
                GAP_Request->DeviceName[11]  = 0x00;
                GAP_Write_Local_Name_REQ(GAP_Request);
              }
            break;
            // Third set Class of Device
            case 3:
              {
                  STORE_CLASS_OF_DEVICE_REQ_t* Request;
                  Request = (STORE_CLASS_OF_DEVICE_REQ_t*)MAIN_Data_Array;

                  Request->ClassOfDevice = 0x023000;
                  Store_Class_of_Device_REQ(Request);
              }
            break;
            // Fourth Set Event Filter
            case 4:
              {
                  SET_EVENT_FILTER_REQ_t* Request;
                  Request = (SET_EVENT_FILTER_REQ_t*)MAIN_Data_Array;

                  Request->Filter = 0x00;
                  Set_Event_Filter_REQ(Request);
              }
            break;
            //  fifth UART Speed
            case 5:
              {
                CHANGE_NVS_UART_SPEED_REQ_t Request;
                // 0x00 = 2400 Baud
                // 0x01 = 4800 Baud
                // 0x02 = 7200 Baud
                // 0x03 = 9600 Baud
                // 0x04 = 19200 Baud
                // 0x05 = 38400 Baud
                // 0x06 = 57600 Baud
                // 0x07 = 115200 Baud
                // 0x08 = 230400 Baud
                // 0x09 = 460800 Baud
                // 0x0A = 921600 Baud
                Request.UartSpeed = 0x07;

                Change_NVS_UART_Speed_REQ(&Request);
              }
            break;
            case 6:
              {
                GAP_SET_FIXED_PIN_REQ_t* Request;
                Request = (GAP_SET_FIXED_PIN_REQ_t*)MAIN_Data_Array;

                Request->Pinlength = 4;
                Request->Pincode[0] = '0';
                Request->Pincode[1] = '0';
                Request->Pincode[2] = '0';
                Request->Pincode[3] = '0';

                GAP_Set_Fixed_Pin_REQ (Request);
              }
            break;
            // Config LMX9820 OK
            case 7:
                MAIN_Config_Statecounter = 1;
            break;

            default:
                MAIN_Config_Statecounter = 1;
            break;
            }

          }


    break;

// Confirm and Indication Management

    case RESTORE_FACTORY_SETTINGS_CFM:
      {
        RESTORE_FACTORY_SETTINGS_CFM_t* Data  = (RESTORE_FACTORY_SETTINGS_CFM_t*)Msg->Packet_Data;

        if(Data->Status == ERROR_OK)
          {

            MAIN_Config_Timer = MAIN_ONE_SECOND;

            while(!MAIN_Config_Timer);

            MAIN_Config_Statecounter = 2;
            Reset_REQ();

          }
          else;

      }
   break;

   case  GAP_SET_FIXED_PIN_CFM:
      {
          GAP_SET_FIXED_PIN_CFM_t* Data  = (GAP_SET_FIXED_PIN_CFM_t*)Msg->Packet_Data;

          if(Data->Status == ERROR_OK)
          {
            MAIN_Config_Timer = MAIN_ONE_SECOND;

            while(!MAIN_Config_Timer);

            MAIN_Config_Statecounter = 7;
            Reset_REQ();

          }
          else;
      }
   break;

    case GAP_WRITE_LOCAL_NAME_CFM:
        {
          GAP_WRITE_LOCAL_NAME_CFM_t* Data  = (GAP_WRITE_LOCAL_NAME_CFM_t*)Msg->Packet_Data;

          if(Data->Status == ERROR_OK)
          {
            MAIN_Config_Timer = MAIN_ONE_SECOND;

            while(!MAIN_Config_Timer);

            MAIN_Config_Statecounter = 3;
            Reset_REQ();

          }
          else;

        }
    break;

   case STORE_CLASS_OF_DEVICE_CFM:
      {
        STORE_CLASS_OF_DEVICE_CFM_t* Data  = (STORE_CLASS_OF_DEVICE_CFM_t*)Msg->Packet_Data;

          if(Data->Status == ERROR_OK)
          {

            MAIN_Config_Timer = MAIN_ONE_SECOND;

            while(!MAIN_Config_Timer);

            MAIN_Config_Statecounter = 4;
            Reset_REQ();

          }
          else;

      }
   break;

   case SET_EVENT_FILTER_CFM:
      {
        SET_EVENT_FILTER_CFM_t* Data  = (SET_EVENT_FILTER_CFM_t*)Msg->Packet_Data;

          if(Data->Status == ERROR_OK)
          {
            MAIN_Config_Timer = MAIN_ONE_SECOND;

            while(!MAIN_Config_Timer);

            MAIN_Config_Statecounter = 5;
            Reset_REQ();

          }
          else;

      }
   break;

   case CHANGE_NVS_UART_SPEED_CFM:
      {
        CHANGE_NVS_UART_SPEED_CFM_t* Data  = (CHANGE_NVS_UART_SPEED_CFM_t*)Msg->Packet_Data;

          if(Data->Status == ERROR_OK)
          {

            MAIN_Config_Timer = MAIN_ONE_SECOND;

            while(!MAIN_Config_Timer);

            MAIN_Config_Statecounter = 6;
            Reset_REQ();

          }
          else;

      }
   break;


   case SPP_INCOMMING_LINK_ESTABLISHED_IND:
       // Now connected and in Transparent Mode!!
       MAIN_Transparent_Mode = 1;
   break;

   case SPP_TRANSPARENT_MODE_IND:
       MAIN_Transparent_Mode = 0;
   break;

   case SPP_LINK_RELEASED_IND:
      // your application - in this case break-signal is used for disconnect
   break;
   default:
      // all other messages don't care
   break;
  }
}

void MAIN_Check_Protocol_LMX(uint8_t Data_Byte)
{

// Transparent Mode ?
  if(MAIN_Transparent_Mode) return; // your application

// Check incoming Message
switch (MAIN_Check_Protocol_LMX_Statecounter)
  {
  case WAIT_STX:
      if (Data_Byte == STX)
        {
          MAIN_Check_Protocol_LMX_Statecounter = WAIT_TYPE;
          MAIN_Protocol_Timer = MAIN_TIMEOUT_LMX;
        }
  break;

  case WAIT_TYPE:
      if(Data_Byte == REQ || Data_Byte == CFM || Data_Byte == IND || Data_Byte == RES)
        {
          MAIN_LMX_Command_Protocol->Packet_Type_identification = Data_Byte;
          MAIN_Check_Protocol_LMX_Statecounter = WAIT_OPCODE;
          MAIN_LMX_Command_Protocol->Checksum = MAIN_LMX_Command_Protocol->Packet_Type_identification;
          MAIN_Protocol_Timer = MAIN_TIMEOUT_LMX;
        }
       else
          MAIN_Check_Protocol_LMX_Statecounter = WAIT_STX;
  break;

  case WAIT_OPCODE:
      MAIN_LMX_Command_Protocol->Op_code = Data_Byte;
      MAIN_LMX_Command_Protocol->Checksum += MAIN_LMX_Command_Protocol->Op_code;
      MAIN_Protocol_Timer = MAIN_TIMEOUT_LMX;
      MAIN_Check_Protocol_LMX_Statecounter = WAIT_LENGHT_LSB;
  break;

  case WAIT_LENGHT_LSB:
      MAIN_LMX_Command_Protocol->Data_length_LSB = Data_Byte;
      MAIN_Protocol_Timer = MAIN_TIMEOUT_LMX;
      MAIN_Check_Protocol_LMX_Statecounter = WAIT_LENGHT_MSB;
  break;

  case WAIT_LENGHT_MSB:
      MAIN_LMX_Command_Protocol->Data_length_MSB = Data_Byte;
      // Calc Length Byte
      MAIN_RX_Lenght_LMX = MAIN_LMX_Command_Protocol->Data_length_MSB;
      MAIN_RX_Lenght_LMX = MAIN_RX_Lenght_LMX << 8;
      MAIN_RX_Lenght_LMX += MAIN_LMX_Command_Protocol->Data_length_LSB;

      MAIN_LMX_Command_Protocol->Checksum += MAIN_RX_Lenght_LMX;
      MAIN_Protocol_Timer = MAIN_TIMEOUT_LMX;
      MAIN_Check_Protocol_LMX_Statecounter = WAIT_CRC;
      if(MAIN_RX_Lenght_LMX > MAX_PACKET_LENGHT)
        MAIN_Check_Protocol_LMX_Statecounter = WAIT_STX;

  break;

  case WAIT_CRC:
      if(MAIN_LMX_Command_Protocol->Checksum == Data_Byte)
        {
          MAIN_Protocol_Timer = MAIN_TIMEOUT_LMX;
          MAIN_Check_Protocol_LMX_Statecounter = WAIT_DATA;
          MAIN_Help_LMX = 0;
          if (!MAIN_RX_Lenght_LMX)
            MAIN_Check_Protocol_LMX_Statecounter = WAIT_ETX;
        }
      else
          MAIN_Check_Protocol_LMX_Statecounter = WAIT_STX;

  break;

  case WAIT_DATA:
       MAIN_LMX_Command_Protocol->Packet_Data[MAIN_Help_LMX] = Data_Byte;
       MAIN_Help_LMX++;
       MAIN_RX_Lenght_LMX--;

      if (!MAIN_RX_Lenght_LMX)
        MAIN_Check_Protocol_LMX_Statecounter = WAIT_ETX;

       MAIN_Protocol_Timer = MAIN_TIMEOUT_LMX;
  break;

  case WAIT_ETX:
      if (Data_Byte == ETX);
        {
          MAIN_Msg_Handling_LMX(MAIN_LMX_Command_Protocol);
        }
      MAIN_Check_Protocol_LMX_Statecounter = WAIT_STX;
  break;

  default:
      MAIN_Check_Protocol_LMX_Statecounter = WAIT_STX;
  break;
  }
}


#pragma vector = USCIAB1RX_VECTOR
__interrupt void UART1_RX_Interrupt(void)
{
  uint8_t UART_dummy;


  if (UCA1STAT & UCRXERR){

    UART_dummy = UCA1RXBUF;         // Flush RX buffer
    UART_dummy = UART_dummy >> 1; // and avoid compiler error
  }else{
    if(!MAIN_Protocol_Timer)
        MAIN_Check_Protocol_LMX_Statecounter = WAIT_STX;

    MAIN_Check_Protocol_LMX(UCA1RXBUF);
  }
}

uint16_t UART1_TX_WriteByte(uint8_t ucDataByte)
{
  if(MAIN_UART_TX_Buffer_Load > 200)
    return(0);

  *MAIN_P_UART1_TX_Write_Pointer = ucDataByte;					
  MAIN_P_UART1_TX_Write_Pointer++;								
  MAIN_UART_TX_Buffer_Load++;
  // cyclic buffer check
  if (MAIN_P_UART1_TX_Write_Pointer >= &MAIN_UART1_TX_Buffer[200])
      MAIN_P_UART1_TX_Write_Pointer = &MAIN_UART1_TX_Buffer[0];

  //if(U1TCTL & TXEPT)
  //  IFG2 |= UTXIFG1;			// Force Interrupt
  return(0);
}

