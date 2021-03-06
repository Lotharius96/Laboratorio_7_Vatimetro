/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#define SLAVE_ADRESS 0x40
uint8 result=0;

int16 Datos,Datos2,Voltaje_Shunt=0;
float32 factor_lsb_Corriente=0;
int16 Corriente,Voltaje,Potencia=0;
int flag=0;
//INTERRUPCION DE RECEPCION///


CY_ISR(Rx){

    char Value_Init;
    Value_Init=UART_GetChar();
    if(Value_Init == 'a'){
      flag=1;
    
    }
     isr_Rx_ClearPending();
}


////INTERRUPCION DE TRANSMISION SERIAL



//SE BUSCA UNA RESOLUCION DE 40MV
void Calc_Factor_LSB(int max_Current_Wait){
factor_lsb_Corriente=(max_Current_Wait/32768);
}
void Write_Configuration(){
        i2c_MasterSendRestart(SLAVE_ADRESS, i2c_WRITE_XFER_MODE);  
        i2c_MasterWriteByte(0x00);
        i2c_MasterWriteByte(0xA4);
        i2c_MasterWriteByte(0x1F);
}
void Write_Calibracion(){
        i2c_MasterSendRestart(SLAVE_ADRESS, i2c_WRITE_XFER_MODE);
        i2c_MasterWriteByte(0x05);
        i2c_MasterWriteByte(0x18);
        i2c_MasterWriteByte(0xf7);

}
void Read_Voltage_Shut(){
    i2c_MasterSendRestart(SLAVE_ADRESS, i2c_READ_XFER_MODE);
    Datos2=i2c_MasterReadByte(0x01);
    Datos2<<=8;
    Datos2+=i2c_MasterReadByte(i2c_ACK_DATA);
    //esta es la operacion para convertir el dato el signo y magnitud
    if((0x8000 & Datos2) == 0x8000){
       Datos2-=1;
       Datos2=~Datos2;
       Datos2*=-1; 
    }
    Voltaje_Shunt=Datos2;
  //  Datos2>>=3;
    
}
void Write_Mediciones(){
      int16 value_base=0x0fa0;    
     i2c_MasterSendRestart(SLAVE_ADRESS, i2c_WRITE_XFER_MODE);
        i2c_MasterWriteByte(0x02);  
      CyDelay(1);
       i2c_MasterSendRestart(SLAVE_ADRESS,i2c_WRITE_XFER_MODE);
       i2c_MasterWriteByte(0x01);
       i2c_MasterSendRestart(SLAVE_ADRESS, i2c_WRITE_XFER_MODE);
        i2c_MasterWriteByte(0x03);
        CyDelay(1);
        i2c_MasterSendRestart(SLAVE_ADRESS,i2c_WRITE_XFER_MODE);
        i2c_MasterWriteByte(0x04);
        CyDelay(1);
}
void Read_Values(int Address_Measure){
        uint8 num2=0;
        if(Address_Measure==0x02){
   
           num2=3&255;
          
        }
        i2c_MasterSendRestart(SLAVE_ADRESS,i2c_READ_XFER_MODE);
        Datos=i2c_MasterReadByte(Address_Measure);
        Datos<<=8;
        Datos+=i2c_MasterReadByte(i2c_ACK_DATA);
        Datos>>=num2;
        switch(Address_Measure){
          case 0x02:
            Voltaje=Datos;
         //   Voltaje*=0.004; //obtener la medicion Voltios
            break;
          case 0x03:
            Corriente=Datos;
          //  Corriente*=factor_lsb_Corriente; //Obtener la medicion en amperes
            break;
          case 0x04:
            Potencia=Datos;
           // Potencia*=20*factor_lsb_Corriente; //Obtener la medicion en Watts
            break;
          default:
            break;
        }
       //datos convertidos
  }
int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    isr_Rx_StartEx(Rx);

     /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_Start();
    i2c_Start();
    LCD_Start();
    for(;;)
    {
       Calc_Factor_LSB(3);
        do{
        i2c_MasterSendStart(SLAVE_ADRESS, i2c_WRITE_XFER_MODE);
        }while(result!=i2c_MSTR_NO_ERROR);
           Write_Configuration();
           Write_Calibracion();
           Write_Mediciones();
           Read_Voltage_Shut();
           Read_Values(0x02);
           Read_Values(0x03);
           Read_Values(0x04);
           i2c_MasterSendStop();
           if(flag==1){
              int8 temp=0; 
              UART_PutChar(Voltaje);
              CyDelay(1);
              temp=(Voltaje>>8);
              UART_PutChar(temp);
              CyDelay(1);
              UART_PutChar(Corriente);
              CyDelay(1);
              temp=(Corriente>>8);
              UART_PutChar(temp);
              CyDelay(1);
              UART_PutChar(Potencia);
              CyDelay(1);
              temp=(Potencia>>8);
              UART_PutChar(temp);
           }
         LCD_Position(0,0);
         char shunt[2];
         sprintf(shunt,"%d",Voltaje_Shunt);
         LCD_PrintString(shunt);
     /// ENVIAR DATOS POR MODULO UART
        ///1-ESTABLECER COMUNICACION
        
        ///2-DETECTAR LA SEÑAL PARA ENVIAR LOS DATOS
        
        
        ///3-A ENVIAR LA PRIMERA TRAMA CON LOS BITS MAS SIGNIFICATIVOS DE BUS_VOLTAGE
        
        ///3-B ENVIAR LA SEGUNDA TRAMA CON LOS BITS MENOS SIGNIFICATIVOS DE BUS_VOLTAGE
        
        ///4-A ENVIAR LA PRIMERA TRAMA CON LOS BITS MAS SIGINIFICATIVOS DE CORRIENTE
        
        ///4-B ENVIAR LA SEGUNDA TRAMA CON LOS BITS MENOS SIGNIFICATIVOS DE CORRIENTE
        
        ///5-A ENVIAR LA PRIMERA TRAMA CON LOS BITS MAS SIGNIFICATIVOS DE POTENCIA
        
        
        ///5-B ENVIAR LA SEGUNDA TRAMA CON LOS BITS MENOS SIGNIFICATIVOS DE VOLTAJE
        
        ///HACER UN RETARDO DE 996 MS
     ///Establecer la   
     
        
   
     
      
             
    
        //i2c_MasterWriteByte(
        
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
