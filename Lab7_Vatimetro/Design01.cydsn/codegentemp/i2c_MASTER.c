/*******************************************************************************
* File Name: i2c_MASTER.c
* Version 3.50
*
* Description:
*  This file provides the source code of APIs for the I2C component master mode.
*
*******************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "i2c_PVT.h"

#if(i2c_MODE_MASTER_ENABLED)

/**********************************
*      System variables
**********************************/

volatile uint8 i2c_mstrStatus;     /* Master Status byte  */
volatile uint8 i2c_mstrControl;    /* Master Control byte */

/* Transmit buffer variables */
volatile uint8 * i2c_mstrRdBufPtr;     /* Pointer to Master Read buffer */
volatile uint8   i2c_mstrRdBufSize;    /* Master Read buffer size       */
volatile uint8   i2c_mstrRdBufIndex;   /* Master Read buffer Index      */

/* Receive buffer variables */
volatile uint8 * i2c_mstrWrBufPtr;     /* Pointer to Master Write buffer */
volatile uint8   i2c_mstrWrBufSize;    /* Master Write buffer size       */
volatile uint8   i2c_mstrWrBufIndex;   /* Master Write buffer Index      */


/*******************************************************************************
* Function Name: i2c_MasterWriteBuf
********************************************************************************
*
* Summary:
*  Automatically writes an entire buffer of data to a slave device. Once the
*  data transfer is initiated by this function, further data transfer is handled
*  by the included ISR in byte by byte mode.
*
* Parameters:
*  slaveAddr: 7-bit slave address.
*  xferData:  Pointer to buffer of data to be sent.
*  cnt:       Size of buffer to send.
*  mode:      Transfer mode defines: start or restart condition generation at
*             begin of the transfer and complete the transfer or halt before
*             generating a stop.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  The included ISR will start a transfer after a start or restart condition is
*  generated.
*
* Global variables:
*  i2c_mstrStatus  - The global variable used to store a current
*                                 status of the I2C Master.
*  i2c_state       - The global variable used to store a current
*                                 state of the software FSM.
*  i2c_mstrControl - The global variable used to control the master
*                                 end of a transaction with or without Stop
*                                 generation.
*  i2c_mstrWrBufPtr - The global variable used to store a pointer
*                                  to the master write buffer.
*  i2c_mstrWrBufIndex - The global variable used to store current
*                                    index within the master write buffer.
*  i2c_mstrWrBufSize - The global variable used to store a master
*                                   write buffer size.
*
* Reentrant:
*  No
*
*******************************************************************************/
uint8 i2c_MasterWriteBuf(uint8 slaveAddress, uint8 * wrData, uint8 cnt, uint8 mode)
      
{
    uint8 errStatus;

    errStatus = i2c_MSTR_NOT_READY;

    if(NULL != wrData)
    {
        /* Check I2C state to allow transfer: valid states are IDLE or HALT */
        if(i2c_SM_IDLE == i2c_state)
        {
            /* Master is ready for transaction: check if bus is free */
            if(i2c_CHECK_BUS_FREE(i2c_MCSR_REG))
            {
                errStatus = i2c_MSTR_NO_ERROR;
            }
            else
            {
                errStatus = i2c_MSTR_BUS_BUSY;
            }
        }
        else if(i2c_SM_MSTR_HALT == i2c_state)
        {
            /* Master is ready and waiting for ReStart */
            errStatus = i2c_MSTR_NO_ERROR;

            i2c_ClearPendingInt();
            i2c_mstrStatus &= (uint8) ~i2c_MSTAT_XFER_HALT;
        }
        else
        {
            /* errStatus = i2c_MSTR_NOT_READY was send before */
        }

        if(i2c_MSTR_NO_ERROR == errStatus)
        {
            /* Set state to start write transaction */
            i2c_state = i2c_SM_MSTR_WR_ADDR;

            /* Prepare write buffer */
            i2c_mstrWrBufIndex = 0u;
            i2c_mstrWrBufSize  = cnt;
            i2c_mstrWrBufPtr   = (volatile uint8 *) wrData;

            /* Set end of transaction flag: Stop or Halt (following ReStart) */
            i2c_mstrControl = mode;

            /* Clear write status history */
            i2c_mstrStatus &= (uint8) ~i2c_MSTAT_WR_CMPLT;

            /* Hardware actions: write address and generate Start or ReStart */
            i2c_DATA_REG = (uint8) (slaveAddress << i2c_SLAVE_ADDR_SHIFT);

            if(i2c_CHECK_RESTART(mode))
            {
                i2c_GENERATE_RESTART;
            }
            else
            {
                i2c_GENERATE_START;
            }

            /* Enable interrupt to complete transfer */
            i2c_EnableInt();
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: i2c_MasterReadBuf
********************************************************************************
*
* Summary:
*  Automatically writes an entire buffer of data to a slave device. Once the
*  data transfer is initiated by this function, further data transfer is handled
*  by the included ISR in byte by byte mode.
*
* Parameters:
*  slaveAddr: 7-bit slave address.
*  xferData:  Pointer to buffer where to put data from slave.
*  cnt:       Size of buffer to read.
*  mode:      Transfer mode defines: start or restart condition generation at
*             begin of the transfer and complete the transfer or halt before
*             generating a stop.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  The included ISR will start a transfer after start or restart condition is
*  generated.
*
* Global variables:
*  i2c_mstrStatus  - The global variable used to store a current
*                                 status of the I2C Master.
*  i2c_state       - The global variable used to store a current
*                                 state of the software FSM.
*  i2c_mstrControl - The global variable used to control the master
*                                 end of a transaction with or without
*                                 Stop generation.
*  i2c_mstrRdBufPtr - The global variable used to store a pointer
*                                  to the master write buffer.
*  i2c_mstrRdBufIndex - The global variable  used to store a
*                                    current index within the master
*                                    write buffer.
*  i2c_mstrRdBufSize - The global variable used to store a master
*                                   write buffer size.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 i2c_MasterReadBuf(uint8 slaveAddress, uint8 * rdData, uint8 cnt, uint8 mode)
      
{
    uint8 errStatus;

    errStatus = i2c_MSTR_NOT_READY;

    if(NULL != rdData)
    {
        /* Check I2C state to allow transfer: valid states are IDLE or HALT */
        if(i2c_SM_IDLE == i2c_state)
        {
            /* Master is ready to transaction: check if bus is free */
            if(i2c_CHECK_BUS_FREE(i2c_MCSR_REG))
            {
                errStatus = i2c_MSTR_NO_ERROR;
            }
            else
            {
                errStatus = i2c_MSTR_BUS_BUSY;
            }
        }
        else if(i2c_SM_MSTR_HALT == i2c_state)
        {
            /* Master is ready and waiting for ReStart */
            errStatus = i2c_MSTR_NO_ERROR;

            i2c_ClearPendingInt();
            i2c_mstrStatus &= (uint8) ~i2c_MSTAT_XFER_HALT;
        }
        else
        {
            /* errStatus = i2c_MSTR_NOT_READY was set before */
        }

        if(i2c_MSTR_NO_ERROR == errStatus)
        {
            /* Set state to start write transaction */
            i2c_state = i2c_SM_MSTR_RD_ADDR;

            /* Prepare read buffer */
            i2c_mstrRdBufIndex  = 0u;
            i2c_mstrRdBufSize   = cnt;
            i2c_mstrRdBufPtr    = (volatile uint8 *) rdData;

            /* Set end of transaction flag: Stop or Halt (following ReStart) */
            i2c_mstrControl = mode;

            /* Clear read status history */
            i2c_mstrStatus &= (uint8) ~i2c_MSTAT_RD_CMPLT;

            /* Hardware actions: write address and generate Start or ReStart */
            i2c_DATA_REG = ((uint8) (slaveAddress << i2c_SLAVE_ADDR_SHIFT) |
                                                  i2c_READ_FLAG);

            if(i2c_CHECK_RESTART(mode))
            {
                i2c_GENERATE_RESTART;
            }
            else
            {
                i2c_GENERATE_START;
            }

            /* Enable interrupt to complete transfer */
            i2c_EnableInt();
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: i2c_MasterSendStart
********************************************************************************
*
* Summary:
*  Generates Start condition and sends slave address with read/write bit.
*
* Parameters:
*  slaveAddress:  7-bit slave address.
*  R_nW:          Zero, send write command, non-zero send read command.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  i2c_state - The global variable used to store a current state of
*                           the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 i2c_MasterSendStart(uint8 slaveAddress, uint8 R_nW)
      
{
    uint8 errStatus;

    errStatus = i2c_MSTR_NOT_READY;

    /* If IDLE, check if bus is free */
    if(i2c_SM_IDLE == i2c_state)
    {
        /* If bus is free, generate Start condition */
        if(i2c_CHECK_BUS_FREE(i2c_MCSR_REG))
        {
            /* Disable interrupt for manual master operation */
            i2c_DisableInt();

            /* Set address and read/write flag */
            slaveAddress = (uint8) (slaveAddress << i2c_SLAVE_ADDR_SHIFT);
            if(0u != R_nW)
            {
                slaveAddress |= i2c_READ_FLAG;
                i2c_state = i2c_SM_MSTR_RD_ADDR;
            }
            else
            {
                i2c_state = i2c_SM_MSTR_WR_ADDR;
            }

            /* Hardware actions: write address and generate Start */
            i2c_DATA_REG = slaveAddress;
            i2c_GENERATE_START_MANUAL;

            /* Wait until address is transferred */
            while(i2c_WAIT_BYTE_COMPLETE(i2c_CSR_REG))
            {
            }

        #if(i2c_MODE_MULTI_MASTER_SLAVE_ENABLED)
            if(i2c_CHECK_START_GEN(i2c_MCSR_REG))
            {
                i2c_CLEAR_START_GEN;

                /* Start condition was not generated: reset FSM to IDLE */
                i2c_state = i2c_SM_IDLE;
                errStatus = i2c_MSTR_ERR_ABORT_START_GEN;
            }
            else
        #endif /* (i2c_MODE_MULTI_MASTER_SLAVE_ENABLED) */

        #if(i2c_MODE_MULTI_MASTER_ENABLED)
            if(i2c_CHECK_LOST_ARB(i2c_CSR_REG))
            {
                i2c_BUS_RELEASE_MANUAL;

                /* Master lost arbitrage: reset FSM to IDLE */
                i2c_state = i2c_SM_IDLE;
                errStatus = i2c_MSTR_ERR_ARB_LOST;
            }
            else
        #endif /* (i2c_MODE_MULTI_MASTER_ENABLED) */

            if(i2c_CHECK_ADDR_NAK(i2c_CSR_REG))
            {
                /* Address has been NACKed: reset FSM to IDLE */
                i2c_state = i2c_SM_IDLE;
                errStatus = i2c_MSTR_ERR_LB_NAK;
            }
            else
            {
                /* Start was sent without errors */
                errStatus = i2c_MSTR_NO_ERROR;
            }
        }
        else
        {
            errStatus = i2c_MSTR_BUS_BUSY;
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: i2c_MasterSendRestart
********************************************************************************
*
* Summary:
*  Generates ReStart condition and sends slave address with read/write bit.
*
* Parameters:
*  slaveAddress:  7-bit slave address.
*  R_nW:          Zero, send write command, non-zero send read command.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  i2c_state - The global variable used to store a current state of
*                           the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 i2c_MasterSendRestart(uint8 slaveAddress, uint8 R_nW)
      
{
    uint8 errStatus;

    errStatus = i2c_MSTR_NOT_READY;

    /* Check if START condition was generated */
    if(i2c_CHECK_MASTER_MODE(i2c_MCSR_REG))
    {
        /* Set address and read/write flag */
        slaveAddress = (uint8) (slaveAddress << i2c_SLAVE_ADDR_SHIFT);
        if(0u != R_nW)
        {
            slaveAddress |= i2c_READ_FLAG;
            i2c_state = i2c_SM_MSTR_RD_ADDR;
        }
        else
        {
            i2c_state = i2c_SM_MSTR_WR_ADDR;
        }

        /* Hardware actions: write address and generate ReStart */
        i2c_DATA_REG = slaveAddress;
        i2c_GENERATE_RESTART_MANUAL;

        /* Wait until address has been transferred */
        while(i2c_WAIT_BYTE_COMPLETE(i2c_CSR_REG))
        {
        }

    #if(i2c_MODE_MULTI_MASTER_ENABLED)
        if(i2c_CHECK_LOST_ARB(i2c_CSR_REG))
        {
            i2c_BUS_RELEASE_MANUAL;

            /* Master lost arbitrage: reset FSM to IDLE */
            i2c_state = i2c_SM_IDLE;
            errStatus = i2c_MSTR_ERR_ARB_LOST;
        }
        else
    #endif /* (i2c_MODE_MULTI_MASTER_ENABLED) */

        if(i2c_CHECK_ADDR_NAK(i2c_CSR_REG))
        {
            /* Address has been NACKed: reset FSM to IDLE */
            i2c_state = i2c_SM_IDLE;
            errStatus = i2c_MSTR_ERR_LB_NAK;
        }
        else
        {
            /* ReStart was sent without errors */
            errStatus = i2c_MSTR_NO_ERROR;
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: i2c_MasterSendStop
********************************************************************************
*
* Summary:
*  Generates I2C Stop condition on bus. Function do nothing if Start or Restart
*  condition was failed before call this function.
*
* Parameters:
*  None.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  Stop generation is required to complete the transaction.
*  This function does not wait until a Stop condition is generated.
*
* Global variables:
*  i2c_state - The global variable used to store a current state of
*                           the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 i2c_MasterSendStop(void) 
{
    uint8 errStatus;

    errStatus = i2c_MSTR_NOT_READY;

    /* Check if master is active on bus */
    if(i2c_CHECK_MASTER_MODE(i2c_MCSR_REG))
    {
        i2c_GENERATE_STOP_MANUAL;
        i2c_state = i2c_SM_IDLE;

        /* Wait until stop has been generated */
        while(i2c_WAIT_STOP_COMPLETE(i2c_CSR_REG))
        {
        }

        errStatus = i2c_MSTR_NO_ERROR;

    #if(i2c_MODE_MULTI_MASTER_ENABLED)
        if(i2c_CHECK_LOST_ARB(i2c_CSR_REG))
        {
            i2c_BUS_RELEASE_MANUAL;

            /* NACK was generated by instead Stop */
            errStatus = i2c_MSTR_ERR_ARB_LOST;
        }
    #endif /* (i2c_MODE_MULTI_MASTER_ENABLED) */
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: i2c_MasterWriteByte
********************************************************************************
*
* Summary:
*  Sends one byte to a slave. A valid Start or ReStart condition must be
*  generated before this call this function. Function do nothing if Start or
*  Restart condition was failed before call this function.
*
* Parameters:
*  data:  The data byte to send to the slave.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  i2c_state - The global variable used to store a current state of
*                           the software FSM.
*
*******************************************************************************/
uint8 i2c_MasterWriteByte(uint8 theByte) 
{
    uint8 errStatus;

    errStatus = i2c_MSTR_NOT_READY;

    /* Check if START condition was generated */
    if(i2c_CHECK_MASTER_MODE(i2c_MCSR_REG))
    {
        i2c_DATA_REG = theByte;   /* Write DATA register */
        i2c_TRANSMIT_DATA_MANUAL; /* Set transmit mode   */
        i2c_state = i2c_SM_MSTR_WR_DATA;

        /* Wait until data byte has been transmitted */
        while(i2c_WAIT_BYTE_COMPLETE(i2c_CSR_REG))
        {
        }

    #if(i2c_MODE_MULTI_MASTER_ENABLED)
        if(i2c_CHECK_LOST_ARB(i2c_CSR_REG))
        {
            i2c_BUS_RELEASE_MANUAL;

            /* Master lost arbitrage: reset FSM to IDLE */
            i2c_state = i2c_SM_IDLE;
            errStatus = i2c_MSTR_ERR_ARB_LOST;
        }
        /* Check LRB bit */
        else
    #endif /* (i2c_MODE_MULTI_MASTER_ENABLED) */

        if(i2c_CHECK_DATA_ACK(i2c_CSR_REG))
        {
            i2c_state = i2c_SM_MSTR_HALT;
            errStatus = i2c_MSTR_NO_ERROR;
        }
        else
        {
            i2c_state = i2c_SM_MSTR_HALT;
            errStatus = i2c_MSTR_ERR_LB_NAK;
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: i2c_MasterReadByte
********************************************************************************
*
* Summary:
*  Reads one byte from a slave and ACK or NACK the transfer. A valid Start or
*  ReStart condition must be generated before this call this function. Function
*  do nothing if Start or Restart condition was failed before call this
*  function.
*
* Parameters:
*  acknNack:  Zero, response with NACK, if non-zero response with ACK.
*
* Return:
*  Byte read from slave.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  i2c_state - The global variable used to store a current
*                           state of the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 i2c_MasterReadByte(uint8 acknNak) 
{
    uint8 theByte;

    theByte = 0u;

    /* Check if START condition was generated */
    if(i2c_CHECK_MASTER_MODE(i2c_MCSR_REG))
    {
        /* When address phase needs to release bus and receive byte,
        * then decide ACK or NACK
        */
        if(i2c_SM_MSTR_RD_ADDR == i2c_state)
        {
            i2c_READY_TO_READ_MANUAL;
            i2c_state = i2c_SM_MSTR_RD_DATA;
        }

        /* Wait until data byte has been received */
        while(i2c_WAIT_BYTE_COMPLETE(i2c_CSR_REG))
        {
        }

        theByte = i2c_DATA_REG;

        /* Command ACK to receive next byte and continue transfer.
        *  Do nothing for NACK. The NACK will be generated by
        *  Stop or ReStart routine.
        */
        if(acknNak != 0u) /* Generate ACK */
        {
            i2c_ACK_AND_RECEIVE_MANUAL;
        }
        else              /* Do nothing for the follwong NACK */
        {
            i2c_state = i2c_SM_MSTR_HALT;
        }
    }

    return(theByte);
}


/*******************************************************************************
* Function Name: i2c_MasterStatus
********************************************************************************
*
* Summary:
*  Returns the master's communication status.
*
* Parameters:
*  None.
*
* Return:
*  Current status of I2C master.
*
* Global variables:
*  i2c_mstrStatus - The global variable used to store a current
*                                status of the I2C Master.
*
*******************************************************************************/
uint8 i2c_MasterStatus(void) 
{
    uint8 status;

    i2c_DisableInt(); /* Lock from interrupt */

    /* Read master status */
    status = i2c_mstrStatus;

    if (i2c_CHECK_SM_MASTER)
    {
        /* Set transfer in progress flag in status */
        status |= i2c_MSTAT_XFER_INP;
    }

    i2c_EnableInt(); /* Release lock */

    return (status);
}


/*******************************************************************************
* Function Name: i2c_MasterClearStatus
********************************************************************************
*
* Summary:
*  Clears all status flags and returns the master status.
*
* Parameters:
*  None.
*
* Return:
*  Current status of I2C master.
*
* Global variables:
*  i2c_mstrStatus - The global variable used to store a current
*                                status of the I2C Master.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 i2c_MasterClearStatus(void) 
{
    uint8 status;

    i2c_DisableInt(); /* Lock from interrupt */

    /* Read and clear master status */
    status = i2c_mstrStatus;
    i2c_mstrStatus = i2c_MSTAT_CLEAR;

    i2c_EnableInt(); /* Release lock */

    return (status);
}


/*******************************************************************************
* Function Name: i2c_MasterGetReadBufSize
********************************************************************************
*
* Summary:
*  Returns the amount of bytes that has been transferred with an
*  I2C_MasterReadBuf command.
*
* Parameters:
*  None.
*
* Return:
*  Byte count of transfer. If the transfer is not yet complete, it will return
*  the byte count transferred so far.
*
* Global variables:
*  i2c_mstrRdBufIndex - The global variable stores current index
*                                    within the master read buffer.
*
*******************************************************************************/
uint8 i2c_MasterGetReadBufSize(void) 
{
    return (i2c_mstrRdBufIndex);
}


/*******************************************************************************
* Function Name: i2c_MasterGetWriteBufSize
********************************************************************************
*
* Summary:
*  Returns the amount of bytes that has been transferred with an
*  I2C_MasterWriteBuf command.
*
* Parameters:
*  None.
*
* Return:
*  Byte count of transfer. If the transfer is not yet complete, it will return
*  the byte count transferred so far.
*
* Global variables:
*  i2c_mstrWrBufIndex -  The global variable used to stores current
*                                     index within master write buffer.
*
*******************************************************************************/
uint8 i2c_MasterGetWriteBufSize(void) 
{
    return (i2c_mstrWrBufIndex);
}


/*******************************************************************************
* Function Name: i2c_MasterClearReadBuf
********************************************************************************
*
* Summary:
*  Resets the read buffer pointer back to the first byte in the buffer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  i2c_mstrRdBufIndex - The global variable used to stores current
*                                    index within master read buffer.
*  i2c_mstrStatus - The global variable used to store a current
*                                status of the I2C Master.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void i2c_MasterClearReadBuf(void) 
{
    i2c_DisableInt(); /* Lock from interrupt */

    i2c_mstrRdBufIndex = 0u;
    i2c_mstrStatus    &= (uint8) ~i2c_MSTAT_RD_CMPLT;

    i2c_EnableInt(); /* Release lock */
}


/*******************************************************************************
* Function Name: i2c_MasterClearWriteBuf
********************************************************************************
*
* Summary:
*  Resets the write buffer pointer back to the first byte in the buffer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  i2c_mstrRdBufIndex - The global variable used to stote current
*                                    index within master read buffer.
*  i2c_mstrStatus - The global variable used to store a current
*                                status of the I2C Master.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void i2c_MasterClearWriteBuf(void) 
{
    i2c_DisableInt(); /* Lock from interrupt */

    i2c_mstrWrBufIndex = 0u;
    i2c_mstrStatus    &= (uint8) ~i2c_MSTAT_WR_CMPLT;

    i2c_EnableInt(); /* Release lock */
}

#endif /* (i2c_MODE_MASTER_ENABLED) */


/* [] END OF FILE */
