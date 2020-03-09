/*******************************************************************************
* File Name: i2c.c
* Version 3.50
*
* Description:
*  This file provides the source code of APIs for the I2C component.
*  The actual protocol and operation code resides in the interrupt service
*  routine file.
*
*******************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "i2c_PVT.h"


/**********************************
*      System variables
**********************************/

uint8 i2c_initVar = 0u; /* Defines if component was initialized */

volatile uint8 i2c_state;  /* Current state of I2C FSM */


/*******************************************************************************
* Function Name: i2c_Init
********************************************************************************
*
* Summary:
*  Initializes I2C registers with initial values provided from customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void i2c_Init(void) 
{
#if (i2c_FF_IMPLEMENTED)
    /* Configure fixed function block */
    i2c_CFG_REG  = i2c_DEFAULT_CFG;
    i2c_XCFG_REG = i2c_DEFAULT_XCFG;
    i2c_ADDR_REG = i2c_DEFAULT_ADDR;
    i2c_CLKDIV1_REG = LO8(i2c_DEFAULT_DIVIDE_FACTOR);
    i2c_CLKDIV2_REG = HI8(i2c_DEFAULT_DIVIDE_FACTOR);

#else
    uint8 intState;

    /* Configure control and interrupt sources */
    i2c_CFG_REG      = i2c_DEFAULT_CFG;
    i2c_INT_MASK_REG = i2c_DEFAULT_INT_MASK;

    /* Enable interrupt generation in status */
    intState = CyEnterCriticalSection();
    i2c_INT_ENABLE_REG |= i2c_INTR_ENABLE;
    CyExitCriticalSection(intState);

    /* Configure bit counter */
    #if (i2c_MODE_SLAVE_ENABLED)
        i2c_PERIOD_REG = i2c_DEFAULT_PERIOD;
    #endif  /* (i2c_MODE_SLAVE_ENABLED) */

    /* Configure clock generator */
    #if (i2c_MODE_MASTER_ENABLED)
        i2c_MCLK_PRD_REG = i2c_DEFAULT_MCLK_PRD;
        i2c_MCLK_CMP_REG = i2c_DEFAULT_MCLK_CMP;
    #endif /* (i2c_MODE_MASTER_ENABLED) */
#endif /* (i2c_FF_IMPLEMENTED) */

#if (i2c_TIMEOUT_ENABLED)
    i2c_TimeoutInit();
#endif /* (i2c_TIMEOUT_ENABLED) */

    /* Configure internal interrupt */
    CyIntDisable    (i2c_ISR_NUMBER);
    CyIntSetPriority(i2c_ISR_NUMBER, i2c_ISR_PRIORITY);
    #if (i2c_INTERN_I2C_INTR_HANDLER)
        (void) CyIntSetVector(i2c_ISR_NUMBER, &i2c_ISR);
    #endif /* (i2c_INTERN_I2C_INTR_HANDLER) */

    /* Set FSM to default state */
    i2c_state = i2c_SM_IDLE;

#if (i2c_MODE_SLAVE_ENABLED)
    /* Clear status and buffers index */
    i2c_slStatus = 0u;
    i2c_slRdBufIndex = 0u;
    i2c_slWrBufIndex = 0u;

    /* Configure matched address */
    i2c_SlaveSetAddress(i2c_DEFAULT_ADDR);
#endif /* (i2c_MODE_SLAVE_ENABLED) */

#if (i2c_MODE_MASTER_ENABLED)
    /* Clear status and buffers index */
    i2c_mstrStatus = 0u;
    i2c_mstrRdBufIndex = 0u;
    i2c_mstrWrBufIndex = 0u;
#endif /* (i2c_MODE_MASTER_ENABLED) */
}


/*******************************************************************************
* Function Name: i2c_Enable
********************************************************************************
*
* Summary:
*  Enables I2C operations.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  None.
*
*******************************************************************************/
void i2c_Enable(void) 
{
#if (i2c_FF_IMPLEMENTED)
    uint8 intState;

    /* Enable power to block */
    intState = CyEnterCriticalSection();
    i2c_ACT_PWRMGR_REG  |= i2c_ACT_PWR_EN;
    i2c_STBY_PWRMGR_REG |= i2c_STBY_PWR_EN;
    CyExitCriticalSection(intState);
#else
    #if (i2c_MODE_SLAVE_ENABLED)
        /* Enable bit counter */
        uint8 intState = CyEnterCriticalSection();
        i2c_COUNTER_AUX_CTL_REG |= i2c_CNT7_ENABLE;
        CyExitCriticalSection(intState);
    #endif /* (i2c_MODE_SLAVE_ENABLED) */

    /* Enable slave or master bits */
    i2c_CFG_REG |= i2c_ENABLE_MS;
#endif /* (i2c_FF_IMPLEMENTED) */

#if (i2c_TIMEOUT_ENABLED)
    i2c_TimeoutEnable();
#endif /* (i2c_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: i2c_Start
********************************************************************************
*
* Summary:
*  Starts the I2C hardware. Enables Active mode power template bits or clock
*  gating as appropriate. It is required to be executed before I2C bus
*  operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  This component automatically enables its interrupt.  If I2C is enabled !
*  without the interrupt enabled, it can lock up the I2C bus.
*
* Global variables:
*  i2c_initVar - This variable is used to check the initial
*                             configuration, modified on the first
*                             function call.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void i2c_Start(void) 
{
    if (0u == i2c_initVar)
    {
        i2c_Init();
        i2c_initVar = 1u; /* Component initialized */
    }

    i2c_Enable();
    i2c_EnableInt();
}


/*******************************************************************************
* Function Name: i2c_Stop
********************************************************************************
*
* Summary:
*  Disables I2C hardware and disables I2C interrupt. Disables Active mode power
*  template bits or clock gating as appropriate.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void i2c_Stop(void) 
{
    i2c_DisableInt();

#if (i2c_TIMEOUT_ENABLED)
    i2c_TimeoutStop();
#endif  /* End (i2c_TIMEOUT_ENABLED) */

#if (i2c_FF_IMPLEMENTED)
    {
        uint8 intState;
        uint16 blockResetCycles;

        /* Store registers effected by block disable */
        i2c_backup.addr    = i2c_ADDR_REG;
        i2c_backup.clkDiv1 = i2c_CLKDIV1_REG;
        i2c_backup.clkDiv2 = i2c_CLKDIV2_REG;

        /* Calculate number of cycles to reset block */
        blockResetCycles = ((uint16) ((uint16) i2c_CLKDIV2_REG << 8u) | i2c_CLKDIV1_REG) + 1u;

        /* Disable block */
        i2c_CFG_REG &= (uint8) ~i2c_CFG_EN_SLAVE;
        /* Wait for block reset before disable power */
        CyDelayCycles((uint32) blockResetCycles);

        /* Disable power to block */
        intState = CyEnterCriticalSection();
        i2c_ACT_PWRMGR_REG  &= (uint8) ~i2c_ACT_PWR_EN;
        i2c_STBY_PWRMGR_REG &= (uint8) ~i2c_STBY_PWR_EN;
        CyExitCriticalSection(intState);

        /* Enable block */
        i2c_CFG_REG |= (uint8) i2c_ENABLE_MS;

        /* Restore registers effected by block disable. Ticket ID#198004 */
        i2c_ADDR_REG    = i2c_backup.addr;
        i2c_ADDR_REG    = i2c_backup.addr;
        i2c_CLKDIV1_REG = i2c_backup.clkDiv1;
        i2c_CLKDIV2_REG = i2c_backup.clkDiv2;
    }
#else

    /* Disable slave or master bits */
    i2c_CFG_REG &= (uint8) ~i2c_ENABLE_MS;

#if (i2c_MODE_SLAVE_ENABLED)
    {
        /* Disable bit counter */
        uint8 intState = CyEnterCriticalSection();
        i2c_COUNTER_AUX_CTL_REG &= (uint8) ~i2c_CNT7_ENABLE;
        CyExitCriticalSection(intState);
    }
#endif /* (i2c_MODE_SLAVE_ENABLED) */

    /* Clear interrupt source register */
    (void) i2c_CSR_REG;
#endif /* (i2c_FF_IMPLEMENTED) */

    /* Disable interrupt on stop (enabled by write transaction) */
    i2c_DISABLE_INT_ON_STOP;
    i2c_ClearPendingInt();

    /* Reset FSM to default state */
    i2c_state = i2c_SM_IDLE;

    /* Clear busy statuses */
#if (i2c_MODE_SLAVE_ENABLED)
    i2c_slStatus &= (uint8) ~(i2c_SSTAT_RD_BUSY | i2c_SSTAT_WR_BUSY);
#endif /* (i2c_MODE_SLAVE_ENABLED) */
}


/* [] END OF FILE */
