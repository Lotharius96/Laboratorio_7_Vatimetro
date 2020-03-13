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
#define SlaveAddress 0x40      //Dirección esclavo; esclavo=Recibe la señal de sincronismo y ejecuta órdenes del maestro 
uint8 result,i,Tabla[4],valorpotencia;
uint16 angulo;
uint16 voltaje_shunt,voltaje_shunt2,vbus,vbus2,Datos2,Datos,corriente,potencia;
int flag;
float factor_lsb_Corriente;
uint8 h=8,g=6;
uint8 vs1,vs2;

const uint8 string2[4]="0123";

//SDA= Aquí viajan los datos como tal 
//I2C es un protocolo síncrono. I2C usa solo 2 cables, 
//uno para el reloj (SCL) y otro para el dato (SDA). 
//Esto significa que el maestro y el esclavo envían datos por el mismo cable, 
//el cual es controlado por el maestro, que crea la señal de reloj.
//I2C no utiliza selección de esclavo, sino direccionamiento.

//CY_ISR(Rx){
//
//    char Value_Init;
//    Value_Init=UART_GetChar();
//    if(Value_Init == 'a'){
//      flag=1;
//    
//    }
//     isr_Rx_ClearPending();
//}

void Get_voltage_shunt(uint8 add_slave, uint8 add_regist){
            I2C_MasterSendRestart(add_slave,I2C_WRITE_XFER_MODE);
            I2C_MasterWriteByte(add_regist);
            I2C_MasterSendRestart(add_slave,I2C_READ_XFER_MODE);
            voltaje_shunt=I2C_MasterReadByte(add_regist);     
          
            voltaje_shunt2=I2C_MasterReadByte(I2C_ACK_DATA);
            
            voltaje_shunt=voltaje_shunt<<8;
            voltaje_shunt=voltaje_shunt2+voltaje_shunt;
            
            //voltaje_shunt=~(voltaje_shunt)+1;
            
            //I2C_MasterSendStop();    
            //return voltaje_shunt;
        }
        
void Get_voltage_bus(uint8 add_slave, uint8 add_regist){
            I2C_MasterSendRestart(add_slave,I2C_WRITE_XFER_MODE);
            I2C_MasterWriteByte(add_regist);
            I2C_MasterSendRestart(add_slave,I2C_READ_XFER_MODE);
            vbus=I2C_MasterReadByte(add_regist);
            vs1=vbus;///////////////////////////////////////////
//            UART_PutCRLF(vs1);
            vbus2=I2C_MasterReadByte(I2C_ACK_DATA);
            vs2=vbus2;//////////////////////////////////////////
//            UART_PutCRLF(vs2);
            vbus=vbus<<8;
            vbus=vbus2+vbus;  
            vbus=vbus>>3;
            
            //I2C_MasterSendStop();    
            //return voltaje_shunt;
        }
        
void Get_Corriente(){
     factor_lsb_Corriente=(3/32768);
        I2C_MasterSendRestart(SlaveAddress, I2C_WRITE_XFER_MODE);
        I2C_MasterWriteByte(0x05);
        I2C_MasterWriteByte(0x11);
        I2C_MasterWriteByte(0x7C);
        
      I2C_MasterSendRestart(SlaveAddress, I2C_WRITE_XFER_MODE);
      I2C_MasterWriteByte(0x03);  
      I2C_MasterSendRestart(SlaveAddress,I2C_READ_XFER_MODE);
      Datos=I2C_MasterReadByte(0x03);
      Datos<<=8;
      Datos+=I2C_MasterReadByte(I2C_ACK_DATA);
      corriente=Datos<<1;
     // i2c_MasterSendStop();

}

        
void Get_Potencia(){
    // factor_lsb_Corriente=(3/32768);
       // float factor_lsb_potencia=factor_lsb_Corriente*20;
    
        I2C_MasterSendRestart(SlaveAddress, I2C_WRITE_XFER_MODE);
        I2C_MasterWriteByte(0x05);
        I2C_MasterWriteByte(0x18);
        I2C_MasterWriteByte(0xf7);
        
      I2C_MasterSendRestart(SlaveAddress, I2C_WRITE_XFER_MODE);
      I2C_MasterWriteByte(0x04);  
      I2C_MasterSendRestart(SlaveAddress,I2C_READ_XFER_MODE);
      Datos=I2C_MasterReadByte(0x04);
      Datos<<=8;
      Datos+=I2C_MasterReadByte(I2C_ACK_DATA);
      potencia=Datos;
     // i2c_MasterSendStop();

}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    //isr_Rx_StartEx(Rx);

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    LCD_Start();
    I2C_Start();
    UART_Start();
    
    for(;;)
    {
        /* Place your application code here. */
        
        //LECTURA DE UN SOLO BYTE DEL SENSOR
        
        do
        {
       I2C_MasterSendStart(SlaveAddress,I2C_WRITE_XFER_MODE);
        
        }
        while(result!=I2C_MSTR_NO_ERROR);   
            
            Get_voltage_shunt(SlaveAddress,0x01);            
            Get_voltage_bus(SlaveAddress,0x02);
         
            //Get_Corriente();
            //Get_Potencia();
            I2C_MasterSendStop();
            
                I2C_MasterSendRestart(SlaveAddress, I2C_WRITE_XFER_MODE);
            I2C_MasterWriteByte(0x00);
            I2C_MasterWriteByte(0xAB);
            I2C_MasterWriteByte(0x3F);
                       
            factor_lsb_Corriente=(3/32768);
            float cor=(voltaje_shunt*4473)/4096;
            float factor_lsb_potencia=factor_lsb_Corriente*20;
            float pot=(cor*vbus)/5000;
            
        LCD_Position(0,0);
        LCD_PrintString("Vs:");
        LCD_Position(0,3);
        LCD_PrintHexUint16(voltaje_shunt);
        
        LCD_Position(0,9);
        LCD_PrintString("I:");
        LCD_Position(0,11);
        LCD_PrintHexUint16(cor);
        
        
        LCD_Position(1,0);
        LCD_PrintString("Vb:");
        LCD_Position(1,3);
        LCD_PrintHexUint16(vbus);
        
        LCD_Position(1,10);
        LCD_PrintString("P:");
        LCD_Position(1,12);
        LCD_PrintHexUint16(pot);      
        
        UART_PutChar(pot);

    }
}

/* [] END OF FILE */
