/*******************************************************************************
* File Name: i2c_INT.c
* Version 3.50
*
* Description:
*  This file provides the source code of Interrupt Service Routine (ISR)
*  for the I2C component.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "i2c_PVT.h"
#include "cyapicallbacks.h"


/*******************************************************************************
*  Place your includes, defines and code here.
********************************************************************************/
/* `#START i2c_ISR_intc` */

/* `#END` */


/*******************************************************************************
* Function Name: i2c_ISR
********************************************************************************
*
* Summary:
*  The handler for the I2C interrupt. The slave and master operations are
*  handled here.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
CY_ISR(i2c_ISR)
{
#if (i2c_MODE_SLAVE_ENABLED)
   uint8  tmp8;
#endif  /* (i2c_MODE_SLAVE_ENABLED) */

    uint8  tmpCsr;
    
#ifdef i2c_ISR_ENTRY_CALLBACK
    i2c_ISR_EntryCallback();
#endif /* i2c_ISR_ENTRY_CALLBACK */
    

#if(i2c_TIMEOUT_FF_ENABLED)
    if(0u != i2c_TimeoutGetStatus())
    {
        i2c_TimeoutReset();
        i2c_state = i2c_SM_EXIT_IDLE;
        /* i2c_CSR_REG should be cleared after reset */
    }
#endif /* (i2c_TIMEOUT_FF_ENABLED) */


    tmpCsr = i2c_CSR_REG;      /* Make copy as interrupts clear */

#if(i2c_MODE_MULTI_MASTER_SLAVE_ENABLED)
    if(i2c_CHECK_START_GEN(i2c_MCSR_REG))
    {
        i2c_CLEAR_START_GEN;

        /* Set transfer complete and error flags */
        i2c_mstrStatus |= (i2c_MSTAT_ERR_XFER |
                                        i2c_GET_MSTAT_CMPLT);

        /* Slave was addressed */
        i2c_state = i2c_SM_SLAVE;
    }
#endif /* (i2c_MODE_MULTI_MASTER_SLAVE_ENABLED) */


#if(i2c_MODE_MULTI_MASTER_ENABLED)
    if(i2c_CHECK_LOST_ARB(tmpCsr))
    {
        /* Set errors */
        i2c_mstrStatus |= (i2c_MSTAT_ERR_XFER     |
                                        i2c_MSTAT_ERR_ARB_LOST |
                                        i2c_GET_MSTAT_CMPLT);

        i2c_DISABLE_INT_ON_STOP; /* Interrupt on Stop is enabled by write */

        #if(i2c_MODE_MULTI_MASTER_SLAVE_ENABLED)
            if(i2c_CHECK_ADDRESS_STS(tmpCsr))
            {
                /* Slave was addressed */
                i2c_state = i2c_SM_SLAVE;
            }
            else
            {
                i2c_BUS_RELEASE;

                i2c_state = i2c_SM_EXIT_IDLE;
            }
        #else
            i2c_BUS_RELEASE;

            i2c_state = i2c_SM_EXIT_IDLE;

        #endif /* (i2c_MODE_MULTI_MASTER_SLAVE_ENABLED) */
    }
#endif /* (i2c_MODE_MULTI_MASTER_ENABLED) */

    /* Check for master operation mode */
    if(i2c_CHECK_SM_MASTER)
    {
    #if(i2c_MODE_MASTER_ENABLED)
        if(i2c_CHECK_BYTE_COMPLETE(tmpCsr))
        {
            switch (i2c_state)
            {
            case i2c_SM_MSTR_WR_ADDR:  /* After address is sent, write data */
            case i2c_SM_MSTR_RD_ADDR:  /* After address is sent, read data */

                tmpCsr &= ((uint8) ~i2c_CSR_STOP_STATUS); /* Clear Stop bit history on address phase */

                if(i2c_CHECK_ADDR_ACK(tmpCsr))
                {
                    /* Setup for transmit or receive of data */
                    if(i2c_state == i2c_SM_MSTR_WR_ADDR)   /* TRANSMIT data */
                    {
                        /* Check if at least one byte to transfer */
                        if(i2c_mstrWrBufSize > 0u)
                        {
                            /* Load the 1st data byte */
                            i2c_DATA_REG = i2c_mstrWrBufPtr[0u];
                            i2c_TRANSMIT_DATA;
                            i2c_mstrWrBufIndex = 1u;   /* Set index to 2nd element */

                            /* Set transmit state until done */
                            i2c_state = i2c_SM_MSTR_WR_DATA;
                        }
                        /* End of buffer: complete writing */
                        else if(i2c_CHECK_NO_STOP(i2c_mstrControl))
                        {
                            /* Set write complete and master halted */
                            i2c_mstrStatus |= (i2c_MSTAT_XFER_HALT |
                                                            i2c_MSTAT_WR_CMPLT);

                            i2c_state = i2c_SM_MSTR_HALT; /* Expect ReStart */
                            i2c_DisableInt();
                        }
                        else
                        {
                            i2c_ENABLE_INT_ON_STOP; /* Enable interrupt on Stop, to catch it */
                            i2c_GENERATE_STOP;
                        }
                    }
                    else  /* Master receive data */
                    {
                        i2c_READY_TO_READ; /* Release bus to read data */

                        i2c_state  = i2c_SM_MSTR_RD_DATA;
                    }
                }
                /* Address is NACKed */
                else if(i2c_CHECK_ADDR_NAK(tmpCsr))
                {
                    /* Set Address NAK error */
                    i2c_mstrStatus |= (i2c_MSTAT_ERR_XFER |
                                                    i2c_MSTAT_ERR_ADDR_NAK);

                    if(i2c_CHECK_NO_STOP(i2c_mstrControl))
                    {
                        i2c_mstrStatus |= (i2c_MSTAT_XFER_HALT |
                                                        i2c_GET_MSTAT_CMPLT);

                        i2c_state = i2c_SM_MSTR_HALT; /* Expect RESTART */
                        i2c_DisableInt();
                    }
                    else  /* Do normal Stop */
                    {
                        i2c_ENABLE_INT_ON_STOP; /* Enable interrupt on Stop, to catch it */
                        i2c_GENERATE_STOP;
                    }
                }
                else
                {
                    /* Address phase is not set for some reason: error */
                    #if(i2c_TIMEOUT_ENABLED)
                        /* Exit interrupt to take chance for timeout timer to handle this case */
                        i2c_DisableInt();
                        i2c_ClearPendingInt();
                    #else
                        /* Block execution flow: unexpected condition */
                        CYASSERT(0u != 0u);
                    #endif /* (i2c_TIMEOUT_ENABLED) */
                }
                break;

            case i2c_SM_MSTR_WR_DATA:

                if(i2c_CHECK_DATA_ACK(tmpCsr))
                {
                    /* Check if end of buffer */
                    if(i2c_mstrWrBufIndex  < i2c_mstrWrBufSize)
                    {
                        i2c_DATA_REG =
                                                 i2c_mstrWrBufPtr[i2c_mstrWrBufIndex];
                        i2c_TRANSMIT_DATA;
                        i2c_mstrWrBufIndex++;
                    }
                    /* End of buffer: complete writing */
                    else if(i2c_CHECK_NO_STOP(i2c_mstrControl))
                    {
                        /* Set write complete and master halted */
                        i2c_mstrStatus |= (i2c_MSTAT_XFER_HALT |
                                                        i2c_MSTAT_WR_CMPLT);

                        i2c_state = i2c_SM_MSTR_HALT;    /* Expect restart */
                        i2c_DisableInt();
                    }
                    else  /* Do normal Stop */
                    {
                        i2c_ENABLE_INT_ON_STOP;    /* Enable interrupt on Stop, to catch it */
                        i2c_GENERATE_STOP;
                    }
                }
                /* Last byte NAKed: end writing */
                else if(i2c_CHECK_NO_STOP(i2c_mstrControl))
                {
                    /* Set write complete, short transfer and master halted */
                    i2c_mstrStatus |= (i2c_MSTAT_ERR_XFER       |
                                                    i2c_MSTAT_ERR_SHORT_XFER |
                                                    i2c_MSTAT_XFER_HALT      |
                                                    i2c_MSTAT_WR_CMPLT);

                    i2c_state = i2c_SM_MSTR_HALT;    /* Expect ReStart */
                    i2c_DisableInt();
                }
                else  /* Do normal Stop */
                {
                    i2c_ENABLE_INT_ON_STOP;    /* Enable interrupt on Stop, to catch it */
                    i2c_GENERATE_STOP;

                    /* Set short transfer and error flag */
                    i2c_mstrStatus |= (i2c_MSTAT_ERR_SHORT_XFER |
                                                    i2c_MSTAT_ERR_XFER);
                }

                break;

            case i2c_SM_MSTR_RD_DATA:

                i2c_mstrRdBufPtr[i2c_mstrRdBufIndex] = i2c_DATA_REG;
                i2c_mstrRdBufIndex++;

                /* Check if end of buffer */
                if(i2c_mstrRdBufIndex < i2c_mstrRdBufSize)
                {
                    i2c_ACK_AND_RECEIVE;       /* ACK and receive byte */
                }
                /* End of buffer: complete reading */
                else if(i2c_CHECK_NO_STOP(i2c_mstrControl))
                {
                    /* Set read complete and master halted */
                    i2c_mstrStatus |= (i2c_MSTAT_XFER_HALT |
                                                    i2c_MSTAT_RD_CMPLT);

                    i2c_state = i2c_SM_MSTR_HALT;    /* Expect ReStart */
                    i2c_DisableInt();
                }
                else
                {
                    i2c_ENABLE_INT_ON_STOP;
                    i2c_NAK_AND_RECEIVE;       /* NACK and TRY to generate Stop */
                }
                break;

            default: /* This is an invalid state and should not occur */

            #if(i2c_TIMEOUT_ENABLED)
                /* Exit interrupt to take chance for timeout timer to handles this case */
                i2c_DisableInt();
                i2c_ClearPendingInt();
            #else
                /* Block execution flow: unexpected condition */
                CYASSERT(0u != 0u);
            #endif /* (i2c_TIMEOUT_ENABLED) */

                break;
            }
        }

        /* Catches Stop: end of transaction */
        if(i2c_CHECK_STOP_STS(tmpCsr))
        {
            i2c_mstrStatus |= i2c_GET_MSTAT_CMPLT;

            i2c_DISABLE_INT_ON_STOP;
            i2c_state = i2c_SM_IDLE;
        }
    #endif /* (i2c_MODE_MASTER_ENABLED) */
    }
    else if(i2c_CHECK_SM_SLAVE)
    {
    #if(i2c_MODE_SLAVE_ENABLED)

        if((i2c_CHECK_STOP_STS(tmpCsr)) || /* Stop || Restart */
           (i2c_CHECK_BYTE_COMPLETE(tmpCsr) && i2c_CHECK_ADDRESS_STS(tmpCsr)))
        {
            /* Catch end of master write transaction: use interrupt on Stop */
            /* The Stop bit history on address phase does not have correct state */
            if(i2c_SM_SL_WR_DATA == i2c_state)
            {
                i2c_DISABLE_INT_ON_STOP;

                i2c_slStatus &= ((uint8) ~i2c_SSTAT_WR_BUSY);
                i2c_slStatus |= ((uint8)  i2c_SSTAT_WR_CMPLT);

                i2c_state = i2c_SM_IDLE;
            }
        }

        if(i2c_CHECK_BYTE_COMPLETE(tmpCsr))
        {
            /* The address only issued after Start or ReStart: so check the address
               to catch these events:
                FF : sets an address phase with a byte_complete interrupt trigger.
                UDB: sets an address phase immediately after Start or ReStart. */
            if(i2c_CHECK_ADDRESS_STS(tmpCsr))
            {
            /* Check for software address detection */
            #if(i2c_SW_ADRR_DECODE)
                tmp8 = i2c_GET_SLAVE_ADDR(i2c_DATA_REG);

                if(tmp8 == i2c_slAddress)   /* Check for address match */
                {
                    if(0u != (i2c_DATA_REG & i2c_READ_FLAG))
                    {
                        /* Place code to prepare read buffer here                  */
                        /* `#START i2c_SW_PREPARE_READ_BUF_interrupt` */

                        /* `#END` */

                    #ifdef i2c_SW_PREPARE_READ_BUF_CALLBACK
                        i2c_SwPrepareReadBuf_Callback();
                    #endif /* i2c_SW_PREPARE_READ_BUF_CALLBACK */
                        
                        /* Prepare next operation to read, get data and place in data register */
                        if(i2c_slRdBufIndex < i2c_slRdBufSize)
                        {
                            /* Load first data byte from array */
                            i2c_DATA_REG = i2c_slRdBufPtr[i2c_slRdBufIndex];
                            i2c_ACK_AND_TRANSMIT;
                            i2c_slRdBufIndex++;

                            i2c_slStatus |= i2c_SSTAT_RD_BUSY;
                        }
                        else    /* Overflow: provide 0xFF on bus */
                        {
                            i2c_DATA_REG = i2c_OVERFLOW_RETURN;
                            i2c_ACK_AND_TRANSMIT;

                            i2c_slStatus  |= (i2c_SSTAT_RD_BUSY |
                                                           i2c_SSTAT_RD_ERR_OVFL);
                        }

                        i2c_state = i2c_SM_SL_RD_DATA;
                    }
                    else  /* Write transaction: receive 1st byte */
                    {
                        i2c_ACK_AND_RECEIVE;
                        i2c_state = i2c_SM_SL_WR_DATA;

                        i2c_slStatus |= i2c_SSTAT_WR_BUSY;
                        i2c_ENABLE_INT_ON_STOP;
                    }
                }
                else
                {
                    /*     Place code to compare for additional address here    */
                    /* `#START i2c_SW_ADDR_COMPARE_interruptStart` */

                    /* `#END` */

                #ifdef i2c_SW_ADDR_COMPARE_ENTRY_CALLBACK
                    i2c_SwAddrCompare_EntryCallback();
                #endif /* i2c_SW_ADDR_COMPARE_ENTRY_CALLBACK */
                    
                    i2c_NAK_AND_RECEIVE;   /* NACK address */

                    /* Place code to end of condition for NACK generation here */
                    /* `#START i2c_SW_ADDR_COMPARE_interruptEnd`  */

                    /* `#END` */

                #ifdef i2c_SW_ADDR_COMPARE_EXIT_CALLBACK
                    i2c_SwAddrCompare_ExitCallback();
                #endif /* i2c_SW_ADDR_COMPARE_EXIT_CALLBACK */
                }

            #else /* (i2c_HW_ADRR_DECODE) */

                if(0u != (i2c_DATA_REG & i2c_READ_FLAG))
                {
                    /* Place code to prepare read buffer here                  */
                    /* `#START i2c_HW_PREPARE_READ_BUF_interrupt` */

                    /* `#END` */
                    
                #ifdef i2c_HW_PREPARE_READ_BUF_CALLBACK
                    i2c_HwPrepareReadBuf_Callback();
                #endif /* i2c_HW_PREPARE_READ_BUF_CALLBACK */

                    /* Prepare next operation to read, get data and place in data register */
                    if(i2c_slRdBufIndex < i2c_slRdBufSize)
                    {
                        /* Load first data byte from array */
                        i2c_DATA_REG = i2c_slRdBufPtr[i2c_slRdBufIndex];
                        i2c_ACK_AND_TRANSMIT;
                        i2c_slRdBufIndex++;

                        i2c_slStatus |= i2c_SSTAT_RD_BUSY;
                    }
                    else    /* Overflow: provide 0xFF on bus */
                    {
                        i2c_DATA_REG = i2c_OVERFLOW_RETURN;
                        i2c_ACK_AND_TRANSMIT;

                        i2c_slStatus  |= (i2c_SSTAT_RD_BUSY |
                                                       i2c_SSTAT_RD_ERR_OVFL);
                    }

                    i2c_state = i2c_SM_SL_RD_DATA;
                }
                else  /* Write transaction: receive 1st byte */
                {
                    i2c_ACK_AND_RECEIVE;
                    i2c_state = i2c_SM_SL_WR_DATA;

                    i2c_slStatus |= i2c_SSTAT_WR_BUSY;
                    i2c_ENABLE_INT_ON_STOP;
                }

            #endif /* (i2c_SW_ADRR_DECODE) */
            }
            /* Data states */
            /* Data master writes into slave */
            else if(i2c_state == i2c_SM_SL_WR_DATA)
            {
                if(i2c_slWrBufIndex < i2c_slWrBufSize)
                {
                    tmp8 = i2c_DATA_REG;
                    i2c_ACK_AND_RECEIVE;
                    i2c_slWrBufPtr[i2c_slWrBufIndex] = tmp8;
                    i2c_slWrBufIndex++;
                }
                else  /* of array: complete write, send NACK */
                {
                    i2c_NAK_AND_RECEIVE;

                    i2c_slStatus |= i2c_SSTAT_WR_ERR_OVFL;
                }
            }
            /* Data master reads from slave */
            else if(i2c_state == i2c_SM_SL_RD_DATA)
            {
                if(i2c_CHECK_DATA_ACK(tmpCsr))
                {
                    if(i2c_slRdBufIndex < i2c_slRdBufSize)
                    {
                         /* Get data from array */
                        i2c_DATA_REG = i2c_slRdBufPtr[i2c_slRdBufIndex];
                        i2c_TRANSMIT_DATA;
                        i2c_slRdBufIndex++;
                    }
                    else   /* Overflow: provide 0xFF on bus */
                    {
                        i2c_DATA_REG = i2c_OVERFLOW_RETURN;
                        i2c_TRANSMIT_DATA;

                        i2c_slStatus |= i2c_SSTAT_RD_ERR_OVFL;
                    }
                }
                else  /* Last byte was NACKed: read complete */
                {
                    /* Only NACK appears on bus */
                    i2c_DATA_REG = i2c_OVERFLOW_RETURN;
                    i2c_NAK_AND_TRANSMIT;

                    i2c_slStatus &= ((uint8) ~i2c_SSTAT_RD_BUSY);
                    i2c_slStatus |= ((uint8)  i2c_SSTAT_RD_CMPLT);

                    i2c_state = i2c_SM_IDLE;
                }
            }
            else
            {
            #if(i2c_TIMEOUT_ENABLED)
                /* Exit interrupt to take chance for timeout timer to handle this case */
                i2c_DisableInt();
                i2c_ClearPendingInt();
            #else
                /* Block execution flow: unexpected condition */
                CYASSERT(0u != 0u);
            #endif /* (i2c_TIMEOUT_ENABLED) */
            }
        }
    #endif /* (i2c_MODE_SLAVE_ENABLED) */
    }
    else
    {
        /* The FSM skips master and slave processing: return to IDLE */
        i2c_state = i2c_SM_IDLE;
    }

#ifdef i2c_ISR_EXIT_CALLBACK
    i2c_ISR_ExitCallback();
#endif /* i2c_ISR_EXIT_CALLBACK */    
}


#if ((i2c_FF_IMPLEMENTED) && (i2c_WAKEUP_ENABLED))
    /*******************************************************************************
    * Function Name: i2c_WAKEUP_ISR
    ********************************************************************************
    *
    * Summary:
    *  The interrupt handler to trigger after a wakeup.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    CY_ISR(i2c_WAKEUP_ISR)
    {
    #ifdef i2c_WAKEUP_ISR_ENTRY_CALLBACK
        i2c_WAKEUP_ISR_EntryCallback();
    #endif /* i2c_WAKEUP_ISR_ENTRY_CALLBACK */
         
        /* Set flag to notify that matched address is received */
        i2c_wakeupSource = 1u;

        /* SCL is stretched until the I2C_Wake() is called */

    #ifdef i2c_WAKEUP_ISR_EXIT_CALLBACK
        i2c_WAKEUP_ISR_ExitCallback();
    #endif /* i2c_WAKEUP_ISR_EXIT_CALLBACK */
    }
#endif /* ((i2c_FF_IMPLEMENTED) && (i2c_WAKEUP_ENABLED)) */


/* [] END OF FILE */
