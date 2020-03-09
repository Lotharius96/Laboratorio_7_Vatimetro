/*******************************************************************************
* File Name: i2c.h
* Version 3.50
*
* Description:
*  This file provides constants and parameter values for the I2C component.

*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_I2C_i2c_H)
#define CY_I2C_i2c_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h"

/* Check if required defines such as CY_PSOC5LP are available in cy_boot */
#if !defined (CY_PSOC5LP)
    #error Component I2C_v3_50 requires cy_boot v3.10 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*   Conditional Compilation Parameters
****************************************/

#define i2c_IMPLEMENTATION     (0u)
#define i2c_MODE               (2u)
#define i2c_ENABLE_WAKEUP      (0u)
#define i2c_ADDR_DECODE        (1u)
#define i2c_UDB_INTRN_CLOCK    (0u)


/* I2C implementation enum */
#define i2c_UDB    (0x00u)
#define i2c_FF     (0x01u)

#define i2c_FF_IMPLEMENTED     (i2c_FF  == i2c_IMPLEMENTATION)
#define i2c_UDB_IMPLEMENTED    (i2c_UDB == i2c_IMPLEMENTATION)

#define i2c_UDB_INTRN_CLOCK_ENABLED    (i2c_UDB_IMPLEMENTED && \
                                                     (0u != i2c_UDB_INTRN_CLOCK))
/* I2C modes enum */
#define i2c_MODE_SLAVE                 (0x01u)
#define i2c_MODE_MASTER                (0x02u)
#define i2c_MODE_MULTI_MASTER          (0x06u)
#define i2c_MODE_MULTI_MASTER_SLAVE    (0x07u)
#define i2c_MODE_MULTI_MASTER_MASK     (0x04u)

#define i2c_MODE_SLAVE_ENABLED         (0u != (i2c_MODE_SLAVE  & i2c_MODE))
#define i2c_MODE_MASTER_ENABLED        (0u != (i2c_MODE_MASTER & i2c_MODE))
#define i2c_MODE_MULTI_MASTER_ENABLED  (0u != (i2c_MODE_MULTI_MASTER_MASK & \
                                                            i2c_MODE))
#define i2c_MODE_MULTI_MASTER_SLAVE_ENABLED    (i2c_MODE_MULTI_MASTER_SLAVE == \
                                                             i2c_MODE)

/* Address detection enum */
#define i2c_SW_DECODE      (0x00u)
#define i2c_HW_DECODE      (0x01u)

#define i2c_SW_ADRR_DECODE             (i2c_SW_DECODE == i2c_ADDR_DECODE)
#define i2c_HW_ADRR_DECODE             (i2c_HW_DECODE == i2c_ADDR_DECODE)

/* Wakeup enabled */
#define i2c_WAKEUP_ENABLED             (0u != i2c_ENABLE_WAKEUP)

/* Adds bootloader APIs to component */
#define i2c_BOOTLOADER_INTERFACE_ENABLED   (i2c_MODE_SLAVE_ENABLED && \
                                                            ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_i2c) || \
                                                             (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface)))

/* Timeout functionality */
#define i2c_TIMEOUT_ENABLE             (0u)
#define i2c_TIMEOUT_SCL_TMOUT_ENABLE   (0u)
#define i2c_TIMEOUT_SDA_TMOUT_ENABLE   (0u)
#define i2c_TIMEOUT_PRESCALER_ENABLE   (0u)
#define i2c_TIMEOUT_IMPLEMENTATION     (0u)

/* Convert to boolean */
#define i2c_TIMEOUT_ENABLED            (0u != i2c_TIMEOUT_ENABLE)
#define i2c_TIMEOUT_SCL_TMOUT_ENABLED  (0u != i2c_TIMEOUT_SCL_TMOUT_ENABLE)
#define i2c_TIMEOUT_SDA_TMOUT_ENABLED  (0u != i2c_TIMEOUT_SDA_TMOUT_ENABLE)
#define i2c_TIMEOUT_PRESCALER_ENABLED  (0u != i2c_TIMEOUT_PRESCALER_ENABLE)

/* Timeout implementation enum. */
#define i2c_TIMEOUT_UDB    (0x00u)
#define i2c_TIMEOUT_FF     (0x01u)

#define i2c_TIMEOUT_FF_IMPLEMENTED     (i2c_TIMEOUT_FF  == \
                                                        i2c_TIMEOUT_IMPLEMENTATION)
#define i2c_TIMEOUT_UDB_IMPLEMENTED    (i2c_TIMEOUT_UDB == \
                                                        i2c_TIMEOUT_IMPLEMENTATION)

#define i2c_TIMEOUT_FF_ENABLED         (i2c_TIMEOUT_ENABLED && \
                                                     i2c_TIMEOUT_FF_IMPLEMENTED)

#define i2c_TIMEOUT_UDB_ENABLED        (i2c_TIMEOUT_ENABLED && \
                                                     i2c_TIMEOUT_UDB_IMPLEMENTED)

#define i2c_EXTERN_I2C_INTR_HANDLER    (0u)
#define i2c_EXTERN_TMOUT_INTR_HANDLER  (0u)

#define i2c_INTERN_I2C_INTR_HANDLER    (0u == i2c_EXTERN_I2C_INTR_HANDLER)
#define i2c_INTERN_TMOUT_INTR_HANDLER  (0u == i2c_EXTERN_TMOUT_INTR_HANDLER)


/***************************************
*       Type defines
***************************************/

/* Structure to save registers before go to sleep */
typedef struct
{
    uint8 enableState;

#if (i2c_FF_IMPLEMENTED)
    uint8 xcfg;
    uint8 cfg;
    uint8 addr;
    uint8 clkDiv1;
    uint8 clkDiv2;
#else
    uint8 control;
#endif /* (i2c_FF_IMPLEMENTED) */

#if (i2c_TIMEOUT_ENABLED)
    uint16 tmoutCfg;
    uint8  tmoutIntr;
#endif /* (i2c_TIMEOUT_ENABLED) */

} i2c_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void i2c_Init(void)                            ;
void i2c_Enable(void)                          ;

void i2c_Start(void)                           ;
void i2c_Stop(void)                            ;

#define i2c_EnableInt()        CyIntEnable      (i2c_ISR_NUMBER)
#define i2c_DisableInt()       CyIntDisable     (i2c_ISR_NUMBER)
#define i2c_ClearPendingInt()  CyIntClearPending(i2c_ISR_NUMBER)
#define i2c_SetPendingInt()    CyIntSetPending  (i2c_ISR_NUMBER)

void i2c_SaveConfig(void)                      ;
void i2c_Sleep(void)                           ;
void i2c_RestoreConfig(void)                   ;
void i2c_Wakeup(void)                          ;

/* I2C Master functions prototypes */
#if (i2c_MODE_MASTER_ENABLED)
    /* Read and Clear status functions */
    uint8 i2c_MasterStatus(void)                ;
    uint8 i2c_MasterClearStatus(void)           ;

    /* Interrupt based operation functions */
    uint8 i2c_MasterWriteBuf(uint8 slaveAddress, uint8 * wrData, uint8 cnt, uint8 mode) \
                                                            ;
    uint8 i2c_MasterReadBuf(uint8 slaveAddress, uint8 * rdData, uint8 cnt, uint8 mode) \
                                                            ;
    uint8 i2c_MasterGetReadBufSize(void)       ;
    uint8 i2c_MasterGetWriteBufSize(void)      ;
    void  i2c_MasterClearReadBuf(void)         ;
    void  i2c_MasterClearWriteBuf(void)        ;

    /* Manual operation functions */
    uint8 i2c_MasterSendStart(uint8 slaveAddress, uint8 R_nW) \
                                                            ;
    uint8 i2c_MasterSendRestart(uint8 slaveAddress, uint8 R_nW) \
                                                            ;
    uint8 i2c_MasterSendStop(void)             ;
    uint8 i2c_MasterWriteByte(uint8 theByte)   ;
    uint8 i2c_MasterReadByte(uint8 acknNak)    ;

#endif /* (i2c_MODE_MASTER_ENABLED) */

/* I2C Slave functions prototypes */
#if (i2c_MODE_SLAVE_ENABLED)
    /* Read and Clear status functions */
    uint8 i2c_SlaveStatus(void)                ;
    uint8 i2c_SlaveClearReadStatus(void)       ;
    uint8 i2c_SlaveClearWriteStatus(void)      ;

    void  i2c_SlaveSetAddress(uint8 address)   ;

    /* Interrupt based operation functions */
    void  i2c_SlaveInitReadBuf(uint8 * rdBuf, uint8 bufSize) \
                                                            ;
    void  i2c_SlaveInitWriteBuf(uint8 * wrBuf, uint8 bufSize) \
                                                            ;
    uint8 i2c_SlaveGetReadBufSize(void)        ;
    uint8 i2c_SlaveGetWriteBufSize(void)       ;
    void  i2c_SlaveClearReadBuf(void)          ;
    void  i2c_SlaveClearWriteBuf(void)         ;

    /* Communication bootloader I2C Slave APIs */
    #if defined(CYDEV_BOOTLOADER_IO_COMP) && (i2c_BOOTLOADER_INTERFACE_ENABLED)
        /* Physical layer functions */
        void     i2c_CyBtldrCommStart(void) CYSMALL \
                                                            ;
        void     i2c_CyBtldrCommStop(void)  CYSMALL \
                                                            ;
        void     i2c_CyBtldrCommReset(void) CYSMALL \
                                                            ;
        cystatus i2c_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) \
                                                        CYSMALL ;
        cystatus i2c_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut)  CYSMALL \
                                                            ;

        #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_i2c)
            #define CyBtldrCommStart    i2c_CyBtldrCommStart
            #define CyBtldrCommStop     i2c_CyBtldrCommStop
            #define CyBtldrCommReset    i2c_CyBtldrCommReset
            #define CyBtldrCommWrite    i2c_CyBtldrCommWrite
            #define CyBtldrCommRead     i2c_CyBtldrCommRead
        #endif /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_i2c) */

        /* Size of Read/Write buffers for I2C bootloader  */
        #define i2c_BTLDR_SIZEOF_READ_BUFFER   (0x80u)
        #define i2c_BTLDR_SIZEOF_WRITE_BUFFER  (0x80u)
        #define i2c_MIN_UNT16(a, b)            ( ((uint16)(a) < (b)) ? ((uint16) (a)) : ((uint16) (b)) )
        #define i2c_WAIT_1_MS                  (1u)

    #endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (i2c_BOOTLOADER_INTERFACE_ENABLED) */

#endif /* (i2c_MODE_SLAVE_ENABLED) */

/* Component interrupt handlers */
CY_ISR_PROTO(i2c_ISR);
#if ((i2c_FF_IMPLEMENTED) || (i2c_WAKEUP_ENABLED))
    CY_ISR_PROTO(i2c_WAKEUP_ISR);
#endif /* ((i2c_FF_IMPLEMENTED) || (i2c_WAKEUP_ENABLED)) */


/**********************************
*   Variable with external linkage
**********************************/

extern uint8 i2c_initVar;


/***************************************
*   Initial Parameter Constants
***************************************/

#define i2c_DATA_RATE          (100u)
#define i2c_DEFAULT_ADDR       (8u)
#define i2c_I2C_PAIR_SELECTED  (0u)

/* I2C pair enum */
#define i2c_I2C_PAIR_ANY   (0x01u) /* Any pins for I2C */
#define i2c_I2C_PAIR0      (0x01u) /* I2C0: (SCL = P12[4]) && (SCL = P12[5]) */
#define i2c_I2C_PAIR1      (0x02u) /* I2C1: (SCL = P12[0]) && (SDA = P12[1]) */

#define i2c_I2C1_SIO_PAIR  (i2c_I2C_PAIR1 == i2c_I2C_PAIR_SELECTED)
#define i2c_I2C0_SIO_PAIR  (i2c_I2C_PAIR0 == i2c_I2C_PAIR_SELECTED)


/***************************************
*            API Constants
***************************************/

/* Master/Slave control constants */
#define i2c_READ_XFER_MODE     (0x01u) /* Read */
#define i2c_WRITE_XFER_MODE    (0x00u) /* Write */
#define i2c_ACK_DATA           (0x01u) /* Send ACK */
#define i2c_NAK_DATA           (0x00u) /* Send NAK */
#define i2c_OVERFLOW_RETURN    (0xFFu) /* Send on bus in case of overflow */

#if (i2c_MODE_MASTER_ENABLED)
    /* "Mode" constants for MasterWriteBuf() or MasterReadBuf() function */
    #define i2c_MODE_COMPLETE_XFER     (0x00u) /* Full transfer with Start and Stop */
    #define i2c_MODE_REPEAT_START      (0x01u) /* Begin with a ReStart instead of a Start */
    #define i2c_MODE_NO_STOP           (0x02u) /* Complete the transfer without a Stop */

    /* Master status */
    #define i2c_MSTAT_CLEAR            (0x00u) /* Clear (initialize) status value */

    #define i2c_MSTAT_RD_CMPLT         (0x01u) /* Read complete */
    #define i2c_MSTAT_WR_CMPLT         (0x02u) /* Write complete */
    #define i2c_MSTAT_XFER_INP         (0x04u) /* Master transfer in progress */
    #define i2c_MSTAT_XFER_HALT        (0x08u) /* Transfer is halted */

    #define i2c_MSTAT_ERR_MASK         (0xF0u) /* Mask for all errors */
    #define i2c_MSTAT_ERR_SHORT_XFER   (0x10u) /* Master NAKed before end of packet */
    #define i2c_MSTAT_ERR_ADDR_NAK     (0x20u) /* Slave did not ACK */
    #define i2c_MSTAT_ERR_ARB_LOST     (0x40u) /* Master lost arbitration during communication */
    #define i2c_MSTAT_ERR_XFER         (0x80u) /* Error during transfer */

    /* Master API returns */
    #define i2c_MSTR_NO_ERROR          (0x00u) /* Function complete without error */
    #define i2c_MSTR_BUS_BUSY          (0x01u) /* Bus is busy, process not started */
    #define i2c_MSTR_NOT_READY         (0x02u) /* Master not Master on the bus or */
                                                            /*  Slave operation in progress */
    #define i2c_MSTR_ERR_LB_NAK        (0x03u) /* Last Byte Naked */
    #define i2c_MSTR_ERR_ARB_LOST      (0x04u) /* Master lost arbitration during communication */
    #define i2c_MSTR_ERR_ABORT_START_GEN  (0x05u) /* Master did not generate Start, the Slave */
                                                               /* was addressed before */

#endif /* (i2c_MODE_MASTER_ENABLED) */

#if (i2c_MODE_SLAVE_ENABLED)
    /* Slave Status Constants */
    #define i2c_SSTAT_RD_CMPLT     (0x01u) /* Read transfer complete */
    #define i2c_SSTAT_RD_BUSY      (0x02u) /* Read transfer in progress */
    #define i2c_SSTAT_RD_ERR_OVFL  (0x04u) /* Read overflow Error */
    #define i2c_SSTAT_RD_MASK      (0x0Fu) /* Read Status Mask */
    #define i2c_SSTAT_RD_NO_ERR    (0x00u) /* Read no Error */

    #define i2c_SSTAT_WR_CMPLT     (0x10u) /* Write transfer complete */
    #define i2c_SSTAT_WR_BUSY      (0x20u) /* Write transfer in progress */
    #define i2c_SSTAT_WR_ERR_OVFL  (0x40u) /* Write overflow Error */
    #define i2c_SSTAT_WR_MASK      (0xF0u) /* Write Status Mask  */
    #define i2c_SSTAT_WR_NO_ERR    (0x00u) /* Write no Error */

    #define i2c_SSTAT_RD_CLEAR     (0x0Du) /* Read Status clear */
    #define i2c_SSTAT_WR_CLEAR     (0xD0u) /* Write Status Clear */

#endif /* (i2c_MODE_SLAVE_ENABLED) */


/***************************************
*       I2C state machine constants
***************************************/

/* Default slave address states */
#define  i2c_SM_IDLE           (0x10u) /* Default state - IDLE */
#define  i2c_SM_EXIT_IDLE      (0x00u) /* Pass master and slave processing and go to IDLE */

/* Slave mode states */
#define  i2c_SM_SLAVE          (i2c_SM_IDLE) /* Any Slave state */
#define  i2c_SM_SL_WR_DATA     (0x11u) /* Master writes data to slave  */
#define  i2c_SM_SL_RD_DATA     (0x12u) /* Master reads data from slave */

/* Master mode states */
#define  i2c_SM_MASTER         (0x40u) /* Any master state */

#define  i2c_SM_MSTR_RD        (0x08u) /* Any master read state          */
#define  i2c_SM_MSTR_RD_ADDR   (0x49u) /* Master sends address with read */
#define  i2c_SM_MSTR_RD_DATA   (0x4Au) /* Master reads data              */

#define  i2c_SM_MSTR_WR        (0x04u) /* Any master read state           */
#define  i2c_SM_MSTR_WR_ADDR   (0x45u) /* Master sends address with write */
#define  i2c_SM_MSTR_WR_DATA   (0x46u) /* Master writes data              */

#define  i2c_SM_MSTR_HALT      (0x60u) /* Master waits for ReStart */

#define i2c_CHECK_SM_MASTER    (0u != (i2c_SM_MASTER & i2c_state))
#define i2c_CHECK_SM_SLAVE     (0u != (i2c_SM_SLAVE  & i2c_state))


/***************************************
*              Registers
***************************************/

#if (i2c_FF_IMPLEMENTED)
    /* Fixed Function registers */
    #define i2c_XCFG_REG           (*(reg8 *) i2c_I2C_FF__XCFG)
    #define i2c_XCFG_PTR           ( (reg8 *) i2c_I2C_FF__XCFG)

    #define i2c_ADDR_REG           (*(reg8 *) i2c_I2C_FF__ADR)
    #define i2c_ADDR_PTR           ( (reg8 *) i2c_I2C_FF__ADR)

    #define i2c_CFG_REG            (*(reg8 *) i2c_I2C_FF__CFG)
    #define i2c_CFG_PTR            ( (reg8 *) i2c_I2C_FF__CFG)

    #define i2c_CSR_REG            (*(reg8 *) i2c_I2C_FF__CSR)
    #define i2c_CSR_PTR            ( (reg8 *) i2c_I2C_FF__CSR)

    #define i2c_DATA_REG           (*(reg8 *) i2c_I2C_FF__D)
    #define i2c_DATA_PTR           ( (reg8 *) i2c_I2C_FF__D)

    #define i2c_MCSR_REG           (*(reg8 *) i2c_I2C_FF__MCSR)
    #define i2c_MCSR_PTR           ( (reg8 *) i2c_I2C_FF__MCSR)

    #define i2c_ACT_PWRMGR_REG     (*(reg8 *) i2c_I2C_FF__PM_ACT_CFG)
    #define i2c_ACT_PWRMGR_PTR     ( (reg8 *) i2c_I2C_FF__PM_ACT_CFG)
    #define i2c_ACT_PWR_EN         ( (uint8)  i2c_I2C_FF__PM_ACT_MSK)

    #define i2c_STBY_PWRMGR_REG    (*(reg8 *) i2c_I2C_FF__PM_STBY_CFG)
    #define i2c_STBY_PWRMGR_PTR    ( (reg8 *) i2c_I2C_FF__PM_STBY_CFG)
    #define i2c_STBY_PWR_EN        ( (uint8)  i2c_I2C_FF__PM_STBY_MSK)

    #define i2c_PWRSYS_CR1_REG     (*(reg8 *) CYREG_PWRSYS_CR1)
    #define i2c_PWRSYS_CR1_PTR     ( (reg8 *) CYREG_PWRSYS_CR1)

    #define i2c_CLKDIV1_REG    (*(reg8 *) i2c_I2C_FF__CLK_DIV1)
    #define i2c_CLKDIV1_PTR    ( (reg8 *) i2c_I2C_FF__CLK_DIV1)

    #define i2c_CLKDIV2_REG    (*(reg8 *) i2c_I2C_FF__CLK_DIV2)
    #define i2c_CLKDIV2_PTR    ( (reg8 *) i2c_I2C_FF__CLK_DIV2)

#else
    /* UDB implementation registers */
    #define i2c_CFG_REG \
            (*(reg8 *) i2c_bI2C_UDB_SyncCtl_CtrlReg__CONTROL_REG)
    #define i2c_CFG_PTR \
            ( (reg8 *) i2c_bI2C_UDB_SyncCtl_CtrlReg__CONTROL_REG)

    #define i2c_CSR_REG        (*(reg8 *) i2c_bI2C_UDB_StsReg__STATUS_REG)
    #define i2c_CSR_PTR        ( (reg8 *) i2c_bI2C_UDB_StsReg__STATUS_REG)

    #define i2c_INT_MASK_REG   (*(reg8 *) i2c_bI2C_UDB_StsReg__MASK_REG)
    #define i2c_INT_MASK_PTR   ( (reg8 *) i2c_bI2C_UDB_StsReg__MASK_REG)

    #define i2c_INT_ENABLE_REG (*(reg8 *) i2c_bI2C_UDB_StsReg__STATUS_AUX_CTL_REG)
    #define i2c_INT_ENABLE_PTR ( (reg8 *) i2c_bI2C_UDB_StsReg__STATUS_AUX_CTL_REG)

    #define i2c_DATA_REG       (*(reg8 *) i2c_bI2C_UDB_Shifter_u0__A0_REG)
    #define i2c_DATA_PTR       ( (reg8 *) i2c_bI2C_UDB_Shifter_u0__A0_REG)

    #define i2c_GO_REG         (*(reg8 *) i2c_bI2C_UDB_Shifter_u0__F1_REG)
    #define i2c_GO_PTR         ( (reg8 *) i2c_bI2C_UDB_Shifter_u0__F1_REG)

    #define i2c_GO_DONE_REG    (*(reg8 *) i2c_bI2C_UDB_Shifter_u0__A1_REG)
    #define i2c_GO_DONE_PTR    ( (reg8 *) i2c_bI2C_UDB_Shifter_u0__A1_REG)

    #define i2c_MCLK_PRD_REG   (*(reg8 *) i2c_bI2C_UDB_Master_ClkGen_u0__D0_REG)
    #define i2c_MCLK_PRD_PTR   ( (reg8 *) i2c_bI2C_UDB_Master_ClkGen_u0__D0_REG)

    #define i2c_MCLK_CMP_REG   (*(reg8 *) i2c_bI2C_UDB_Master_ClkGen_u0__D1_REG)
    #define i2c_MCLK_CMP_PTR   ( (reg8 *) i2c_bI2C_UDB_Master_ClkGen_u0__D1_REG)

    #if (i2c_MODE_SLAVE_ENABLED)
        #define i2c_ADDR_REG       (*(reg8 *) i2c_bI2C_UDB_Shifter_u0__D0_REG)
        #define i2c_ADDR_PTR       ( (reg8 *) i2c_bI2C_UDB_Shifter_u0__D0_REG)

        #define i2c_PERIOD_REG     (*(reg8 *) i2c_bI2C_UDB_Slave_BitCounter__PERIOD_REG)
        #define i2c_PERIOD_PTR     ( (reg8 *) i2c_bI2C_UDB_Slave_BitCounter__PERIOD_REG)

        #define i2c_COUNTER_REG    (*(reg8 *) i2c_bI2C_UDB_Slave_BitCounter__COUNT_REG)
        #define i2c_COUNTER_PTR    ( (reg8 *) i2c_bI2C_UDB_Slave_BitCounter__COUNT_REG)

        #define i2c_COUNTER_AUX_CTL_REG \
                                    (*(reg8 *) i2c_bI2C_UDB_Slave_BitCounter__CONTROL_AUX_CTL_REG)
        #define i2c_COUNTER_AUX_CTL_PTR \
                                    ( (reg8 *) i2c_bI2C_UDB_Slave_BitCounter__CONTROL_AUX_CTL_REG)

    #endif /* (i2c_MODE_SLAVE_ENABLED) */

#endif /* (i2c_FF_IMPLEMENTED) */


/***************************************
*        Registers Constants
***************************************/

/* i2c_I2C_IRQ */
#define i2c_ISR_NUMBER     ((uint8) i2c_I2C_IRQ__INTC_NUMBER)
#define i2c_ISR_PRIORITY   ((uint8) i2c_I2C_IRQ__INTC_PRIOR_NUM)

/* I2C Slave Data Register */
#define i2c_SLAVE_ADDR_MASK    (0x7Fu)
#define i2c_SLAVE_ADDR_SHIFT   (0x01u)
#define i2c_DATA_MASK          (0xFFu)
#define i2c_READ_FLAG          (0x01u)

/* Block reset constants */
#define i2c_CLEAR_REG          (0x00u)
#define i2c_BLOCK_RESET_DELAY  (2u)
#define i2c_FF_RESET_DELAY     (i2c_BLOCK_RESET_DELAY)
#define i2c_RESTORE_TIMEOUT    (255u)

#if (i2c_FF_IMPLEMENTED)
    /* XCFG I2C Extended Configuration Register */
    #define i2c_XCFG_CLK_EN        (0x80u) /* Enable gated clock to block */
    #define i2c_XCFG_I2C_ON        (0x40u) /* Enable I2C as wake up source*/
    #define i2c_XCFG_RDY_TO_SLEEP  (0x20u) /* I2C ready go to sleep */
    #define i2c_XCFG_FORCE_NACK    (0x10u) /* Force NACK all incoming transactions */
    #define i2c_XCFG_NO_BC_INT     (0x08u) /* No interrupt on byte complete */
    #define i2c_XCFG_BUF_MODE      (0x02u) /* Enable buffer mode */
    #define i2c_XCFG_HDWR_ADDR_EN  (0x01u) /* Enable Hardware address match */

    /* CFG I2C Configuration Register */
    #define i2c_CFG_SIO_SELECT     (0x80u) /* Pin Select for SCL/SDA lines */
    #define i2c_CFG_PSELECT        (0x40u) /* Pin Select */
    #define i2c_CFG_BUS_ERR_IE     (0x20u) /* Bus Error Interrupt Enable */
    #define i2c_CFG_STOP_IE        (0x10u) /* Enable Interrupt on STOP condition */
    #define i2c_CFG_CLK_RATE_MSK   (0x0Cu) /* Clock rate select */
    #define i2c_CFG_CLK_RATE_100   (0x00u) /* Clock rate select 100K */
    #define i2c_CFG_CLK_RATE_400   (0x04u) /* Clock rate select 400K */
    #define i2c_CFG_CLK_RATE_050   (0x08u) /* Clock rate select 50K  */
    #define i2c_CFG_CLK_RATE_RSVD  (0x0Cu) /* Clock rate select Invalid */
    #define i2c_CFG_EN_MSTR        (0x02u) /* Enable Master operation */
    #define i2c_CFG_EN_SLAVE       (0x01u) /* Enable Slave operation */

    #define i2c_CFG_CLK_RATE_LESS_EQUAL_50 (0x04u) /* Clock rate select <= 50kHz */
    #define i2c_CFG_CLK_RATE_GRATER_50     (0x00u) /* Clock rate select > 50kHz */

    /* CSR I2C Control and Status Register */
    #define i2c_CSR_BUS_ERROR      (0x80u) /* Active high when bus error has occurred */
    #define i2c_CSR_LOST_ARB       (0x40u) /* Set to 1 if lost arbitration in host mode */
    #define i2c_CSR_STOP_STATUS    (0x20u) /* Set if Stop has been detected */
    #define i2c_CSR_ACK            (0x10u) /* ACK response */
    #define i2c_CSR_NAK            (0x00u) /* NAK response */
    #define i2c_CSR_ADDRESS        (0x08u) /* Set in firmware 0 = status bit, 1 Address is slave */
    #define i2c_CSR_TRANSMIT       (0x04u) /* Set in firmware 1 = transmit, 0 = receive */
    #define i2c_CSR_LRB            (0x02u) /* Last received bit */
    #define i2c_CSR_LRB_ACK        (0x00u) /* Last received bit was an ACK */
    #define i2c_CSR_LRB_NAK        (0x02u) /* Last received bit was an NAK */
    #define i2c_CSR_BYTE_COMPLETE  (0x01u) /* Informs that last byte has been sent */
    #define i2c_CSR_STOP_GEN       (0x00u) /* Generate a stop condition */
    #define i2c_CSR_RDY_TO_RD      (0x00u) /* Set to receive mode */

    /* MCSR I2C Master Control and Status Register */
    #define i2c_MCSR_STOP_GEN      (0x10u) /* Firmware sets this bit to initiate a Stop condition */
    #define i2c_MCSR_BUS_BUSY      (0x08u) /* Status bit, Set at Start and cleared at Stop condition */
    #define i2c_MCSR_MSTR_MODE     (0x04u) /* Status bit, Set at Start and cleared at Stop condition */
    #define i2c_MCSR_RESTART_GEN   (0x02u) /* Firmware sets this bit to initiate a ReStart condition */
    #define i2c_MCSR_START_GEN     (0x01u) /* Firmware sets this bit to initiate a Start condition */

    /* PWRSYS_CR1 to handle Sleep */
    #define i2c_PWRSYS_CR1_I2C_REG_BACKUP  (0x04u) /* Enables, power to I2C regs while sleep */

#else
    /* CONTROL REG bits location */
    #define i2c_CTRL_START_SHIFT           (7u)
    #define i2c_CTRL_STOP_SHIFT            (6u)
    #define i2c_CTRL_RESTART_SHIFT         (5u)
    #define i2c_CTRL_NACK_SHIFT            (4u)
    #define i2c_CTRL_ANY_ADDRESS_SHIFT     (3u)
    #define i2c_CTRL_TRANSMIT_SHIFT        (2u)
    #define i2c_CTRL_ENABLE_MASTER_SHIFT   (1u)
    #define i2c_CTRL_ENABLE_SLAVE_SHIFT    (0u)
    #define i2c_CTRL_START_MASK            ((uint8) (0x01u << i2c_CTRL_START_SHIFT))
    #define i2c_CTRL_STOP_MASK             ((uint8) (0x01u << i2c_CTRL_STOP_SHIFT))
    #define i2c_CTRL_RESTART_MASK          ((uint8) (0x01u << i2c_CTRL_RESTART_SHIFT))
    #define i2c_CTRL_NACK_MASK             ((uint8) (0x01u << i2c_CTRL_NACK_SHIFT))
    #define i2c_CTRL_ANY_ADDRESS_MASK      ((uint8) (0x01u << i2c_CTRL_ANY_ADDRESS_SHIFT))
    #define i2c_CTRL_TRANSMIT_MASK         ((uint8) (0x01u << i2c_CTRL_TRANSMIT_SHIFT))
    #define i2c_CTRL_ENABLE_MASTER_MASK    ((uint8) (0x01u << i2c_CTRL_ENABLE_MASTER_SHIFT))
    #define i2c_CTRL_ENABLE_SLAVE_MASK     ((uint8) (0x01u << i2c_CTRL_ENABLE_SLAVE_SHIFT))

    /* STATUS REG bits location */
    #define i2c_STS_LOST_ARB_SHIFT         (6u)
    #define i2c_STS_STOP_SHIFT             (5u)
    #define i2c_STS_BUSY_SHIFT             (4u)
    #define i2c_STS_ADDR_SHIFT             (3u)
    #define i2c_STS_MASTER_MODE_SHIFT      (2u)
    #define i2c_STS_LRB_SHIFT              (1u)
    #define i2c_STS_BYTE_COMPLETE_SHIFT    (0u)
    #define i2c_STS_LOST_ARB_MASK          ((uint8) (0x01u << i2c_STS_LOST_ARB_SHIFT))
    #define i2c_STS_STOP_MASK              ((uint8) (0x01u << i2c_STS_STOP_SHIFT))
    #define i2c_STS_BUSY_MASK              ((uint8) (0x01u << i2c_STS_BUSY_SHIFT))
    #define i2c_STS_ADDR_MASK              ((uint8) (0x01u << i2c_STS_ADDR_SHIFT))
    #define i2c_STS_MASTER_MODE_MASK       ((uint8) (0x01u << i2c_STS_MASTER_MODE_SHIFT))
    #define i2c_STS_LRB_MASK               ((uint8) (0x01u << i2c_STS_LRB_SHIFT))
    #define i2c_STS_BYTE_COMPLETE_MASK     ((uint8) (0x01u << i2c_STS_BYTE_COMPLETE_SHIFT))

    /* AUX_CTL bits definition */
    #define i2c_COUNTER_ENABLE_MASK        (0x20u) /* Enable 7-bit counter */
    #define i2c_INT_ENABLE_MASK            (0x10u) /* Enable interrupt from status register */
    #define i2c_CNT7_ENABLE                (i2c_COUNTER_ENABLE_MASK)
    #define i2c_INTR_ENABLE                (i2c_INT_ENABLE_MASK)

#endif /* (i2c_FF_IMPLEMENTED) */


/***************************************
*        Marco
***************************************/

/* ACK and NACK for data and address checks */
#define i2c_CHECK_ADDR_ACK(csr)    ((i2c_CSR_LRB_ACK | i2c_CSR_ADDRESS) == \
                                                 ((i2c_CSR_LRB    | i2c_CSR_ADDRESS) &  \
                                                  (csr)))


#define i2c_CHECK_ADDR_NAK(csr)    ((i2c_CSR_LRB_NAK | i2c_CSR_ADDRESS) == \
                                                 ((i2c_CSR_LRB    | i2c_CSR_ADDRESS) &  \
                                                  (csr)))

#define i2c_CHECK_DATA_ACK(csr)    (0u == ((csr) & i2c_CSR_LRB_NAK))

/* MCSR conditions check */
#define i2c_CHECK_BUS_FREE(mcsr)       (0u == ((mcsr) & i2c_MCSR_BUS_BUSY))
#define i2c_CHECK_MASTER_MODE(mcsr)    (0u != ((mcsr) & i2c_MCSR_MSTR_MODE))

/* CSR conditions check */
#define i2c_WAIT_BYTE_COMPLETE(csr)    (0u == ((csr) & i2c_CSR_BYTE_COMPLETE))
#define i2c_WAIT_STOP_COMPLETE(csr)    (0u == ((csr) & (i2c_CSR_BYTE_COMPLETE | \
                                                                     i2c_CSR_STOP_STATUS)))
#define i2c_CHECK_BYTE_COMPLETE(csr)   (0u != ((csr) & i2c_CSR_BYTE_COMPLETE))
#define i2c_CHECK_STOP_STS(csr)        (0u != ((csr) & i2c_CSR_STOP_STATUS))
#define i2c_CHECK_LOST_ARB(csr)        (0u != ((csr) & i2c_CSR_LOST_ARB))
#define i2c_CHECK_ADDRESS_STS(csr)     (0u != ((csr) & i2c_CSR_ADDRESS))

/* Software start and end of transaction check */
#define i2c_CHECK_RESTART(mstrCtrl)    (0u != ((mstrCtrl) & i2c_MODE_REPEAT_START))
#define i2c_CHECK_NO_STOP(mstrCtrl)    (0u != ((mstrCtrl) & i2c_MODE_NO_STOP))

/* Send read or write completion depends on state */
#define i2c_GET_MSTAT_CMPLT ((0u != (i2c_state & i2c_SM_MSTR_RD)) ? \
                                                 (i2c_MSTAT_RD_CMPLT) : (i2c_MSTAT_WR_CMPLT))

/* Returns 7-bit slave address */
#define i2c_GET_SLAVE_ADDR(dataReg)   (((dataReg) >> i2c_SLAVE_ADDR_SHIFT) & \
                                                                  i2c_SLAVE_ADDR_MASK)

#if (i2c_FF_IMPLEMENTED)
    /* Check enable of module */
    #define i2c_I2C_ENABLE_REG     (i2c_ACT_PWRMGR_REG)
    #define i2c_IS_I2C_ENABLE(reg) (0u != ((reg) & i2c_ACT_PWR_EN))
    #define i2c_IS_ENABLED         (0u != (i2c_ACT_PWRMGR_REG & i2c_ACT_PWR_EN))

    #define i2c_CHECK_PWRSYS_I2C_BACKUP    (0u != (i2c_PWRSYS_CR1_I2C_REG_BACKUP & \
                                                                i2c_PWRSYS_CR1_REG))

    /* Check start condition generation */
    #define i2c_CHECK_START_GEN(mcsr)  ((0u != ((mcsr) & i2c_MCSR_START_GEN)) && \
                                                     (0u == ((mcsr) & i2c_MCSR_MSTR_MODE)))

    #define i2c_CLEAR_START_GEN        do{ \
                                                        i2c_MCSR_REG &=                                   \
                                                                           ((uint8) ~i2c_MCSR_START_GEN); \
                                                    }while(0)

    /* Stop interrupt */
    #define i2c_ENABLE_INT_ON_STOP     do{ \
                                                        i2c_CFG_REG |= i2c_CFG_STOP_IE; \
                                                    }while(0)

    #define i2c_DISABLE_INT_ON_STOP    do{ \
                                                        i2c_CFG_REG &=                                 \
                                                                           ((uint8) ~i2c_CFG_STOP_IE); \
                                                    }while(0)

    /* Transmit data */
    #define i2c_TRANSMIT_DATA          do{ \
                                                        i2c_CSR_REG = i2c_CSR_TRANSMIT; \
                                                    }while(0)

    #define i2c_ACK_AND_TRANSMIT       do{ \
                                                        i2c_CSR_REG = (i2c_CSR_ACK |      \
                                                                                    i2c_CSR_TRANSMIT); \
                                                    }while(0)

    #define i2c_NAK_AND_TRANSMIT       do{ \
                                                        i2c_CSR_REG = i2c_CSR_NAK; \
                                                    }while(0)

    /* Special case: udb needs to ack, ff needs to nak */
    #define i2c_ACKNAK_AND_TRANSMIT    do{ \
                                                        i2c_CSR_REG  = (i2c_CSR_NAK |      \
                                                                                     i2c_CSR_TRANSMIT); \
                                                    }while(0)
    /* Receive data */
    #define i2c_ACK_AND_RECEIVE        do{ \
                                                        i2c_CSR_REG = i2c_CSR_ACK; \
                                                    }while(0)

    #define i2c_NAK_AND_RECEIVE        do{ \
                                                        i2c_CSR_REG = i2c_CSR_NAK; \
                                                    }while(0)

    #define i2c_READY_TO_READ          do{ \
                                                        i2c_CSR_REG = i2c_CSR_RDY_TO_RD; \
                                                    }while(0)

    /* Release bus after lost arbitration */
    #define i2c_BUS_RELEASE    i2c_READY_TO_READ

    /* Master Start/ReStart/Stop conditions generation */
    #define i2c_GENERATE_START         do{ \
                                                        i2c_MCSR_REG = i2c_MCSR_START_GEN; \
                                                    }while(0)

    #define i2c_GENERATE_RESTART \
                    do{                       \
                        i2c_MCSR_REG = (i2c_MCSR_RESTART_GEN | \
                                                     i2c_MCSR_STOP_GEN);    \
                        i2c_CSR_REG  = i2c_CSR_TRANSMIT;       \
                    }while(0)

    #define i2c_GENERATE_STOP \
                    do{                    \
                        i2c_MCSR_REG = i2c_MCSR_STOP_GEN; \
                        i2c_CSR_REG  = i2c_CSR_TRANSMIT;  \
                    }while(0)

    /* Master manual APIs compatible defines */
    #define i2c_GENERATE_START_MANUAL      i2c_GENERATE_START
    #define i2c_GENERATE_RESTART_MANUAL    i2c_GENERATE_RESTART
    #define i2c_GENERATE_STOP_MANUAL       i2c_GENERATE_STOP
    #define i2c_TRANSMIT_DATA_MANUAL       i2c_TRANSMIT_DATA
    #define i2c_READY_TO_READ_MANUAL       i2c_READY_TO_READ
    #define i2c_ACK_AND_RECEIVE_MANUAL     i2c_ACK_AND_RECEIVE
    #define i2c_BUS_RELEASE_MANUAL         i2c_BUS_RELEASE

#else

    /* Masks to enable interrupts from Status register */
    #define i2c_STOP_IE_MASK           (i2c_STS_STOP_MASK)
    #define i2c_BYTE_COMPLETE_IE_MASK  (i2c_STS_BYTE_COMPLETE_MASK)

    /* FF compatibility: CSR register bit-fields */
    #define i2c_CSR_LOST_ARB       (i2c_STS_LOST_ARB_MASK)
    #define i2c_CSR_STOP_STATUS    (i2c_STS_STOP_MASK)
    #define i2c_CSR_BUS_ERROR      (0x00u)
    #define i2c_CSR_ADDRESS        (i2c_STS_ADDR_MASK)
    #define i2c_CSR_TRANSMIT       (i2c_CTRL_TRANSMIT_MASK)
    #define i2c_CSR_LRB            (i2c_STS_LRB_MASK)
    #define i2c_CSR_LRB_NAK        (i2c_STS_LRB_MASK)
    #define i2c_CSR_LRB_ACK        (0x00u)
    #define i2c_CSR_BYTE_COMPLETE  (i2c_STS_BYTE_COMPLETE_MASK)

    /* FF compatibility: MCSR registers bit-fields */
    #define i2c_MCSR_REG           (i2c_CSR_REG)  /* UDB incorporates master and slave regs */
    #define i2c_MCSR_BUS_BUSY      (i2c_STS_BUSY_MASK)      /* Is bus is busy               */
    #define i2c_MCSR_START_GEN     (i2c_CTRL_START_MASK)    /* Generate Start condition     */
    #define i2c_MCSR_RESTART_GEN   (i2c_CTRL_RESTART_MASK)  /* Generates RESTART condition  */
    #define i2c_MCSR_MSTR_MODE     (i2c_STS_MASTER_MODE_MASK)/* Define if active Master     */

    /* Data to write into TX FIFO to release FSM */
    #define i2c_PREPARE_TO_RELEASE (0xFFu)
    #define i2c_RELEASE_FSM        (0x00u)

    /* Define release command done: history of byte complete status is cleared */
    #define i2c_WAIT_RELEASE_CMD_DONE  (i2c_RELEASE_FSM != i2c_GO_DONE_REG)

    /* Check enable of module */
    #define i2c_I2C_ENABLE_REG     (i2c_CFG_REG)
    #define i2c_IS_I2C_ENABLE(reg) ((0u != ((reg) & i2c_ENABLE_MASTER)) || \
                                                 (0u != ((reg) & i2c_ENABLE_SLAVE)))

    #define i2c_IS_ENABLED         (0u != (i2c_CFG_REG & i2c_ENABLE_MS))

    /* Check start condition generation */
    #define i2c_CHECK_START_GEN(mcsr)  ((0u != (i2c_CFG_REG &        \
                                                             i2c_MCSR_START_GEN)) \
                                                    &&                                         \
                                                    (0u == ((mcsr) & i2c_MCSR_MSTR_MODE)))

    #define i2c_CLEAR_START_GEN        do{ \
                                                        i2c_CFG_REG &=                 \
                                                        ((uint8) ~i2c_MCSR_START_GEN); \
                                                    }while(0)

    /* Stop interrupt */
    #define i2c_ENABLE_INT_ON_STOP     do{ \
                                                       i2c_INT_MASK_REG |= i2c_STOP_IE_MASK; \
                                                    }while(0)

    #define i2c_DISABLE_INT_ON_STOP    do{ \
                                                        i2c_INT_MASK_REG &=                               \
                                                                             ((uint8) ~i2c_STOP_IE_MASK); \
                                                    }while(0)

    /* Transmit data */
    #define i2c_TRANSMIT_DATA \
                                    do{    \
                                        i2c_CFG_REG     = (i2c_CTRL_TRANSMIT_MASK | \
                                                                       i2c_CTRL_DEFAULT);        \
                                        i2c_GO_DONE_REG = i2c_PREPARE_TO_RELEASE;   \
                                        i2c_GO_REG      = i2c_RELEASE_FSM;          \
                                    }while(0)

    #define i2c_ACK_AND_TRANSMIT   i2c_TRANSMIT_DATA

    #define i2c_NAK_AND_TRANSMIT \
                                        do{   \
                                            i2c_CFG_REG     = (i2c_CTRL_NACK_MASK     | \
                                                                            i2c_CTRL_TRANSMIT_MASK | \
                                                                            i2c_CTRL_DEFAULT);       \
                                            i2c_GO_DONE_REG = i2c_PREPARE_TO_RELEASE;   \
                                            i2c_GO_REG      = i2c_RELEASE_FSM;          \
                                        }while(0)

    /* Receive data */
    #define i2c_READY_TO_READ  \
                                        do{ \
                                            i2c_CFG_REG     = i2c_CTRL_DEFAULT;       \
                                            i2c_GO_DONE_REG = i2c_PREPARE_TO_RELEASE; \
                                            i2c_GO_REG      = i2c_RELEASE_FSM;       \
                                        }while(0)

    #define i2c_ACK_AND_RECEIVE    i2c_READY_TO_READ

    /* Release bus after arbitration is lost */
    #define i2c_BUS_RELEASE    i2c_READY_TO_READ

    #define i2c_NAK_AND_RECEIVE \
                                        do{  \
                                            i2c_CFG_REG     = (i2c_CTRL_NACK_MASK |   \
                                                                            i2c_CTRL_DEFAULT);     \
                                            i2c_GO_DONE_REG = i2c_PREPARE_TO_RELEASE; \
                                            i2c_GO_REG      = i2c_RELEASE_FSM;       \
                                        }while(0)

    /* Master condition generation */
    #define i2c_GENERATE_START \
                                        do{ \
                                            i2c_CFG_REG     = (i2c_CTRL_START_MASK |  \
                                                                            i2c_CTRL_DEFAULT);     \
                                            i2c_GO_DONE_REG = i2c_PREPARE_TO_RELEASE; \
                                            i2c_GO_REG      = i2c_RELEASE_FSM;       \
                                        }while(0)

    #define i2c_GENERATE_RESTART \
                                        do{   \
                                            i2c_CFG_REG     = (i2c_CTRL_RESTART_MASK | \
                                                                            i2c_CTRL_NACK_MASK    | \
                                                                            i2c_CTRL_DEFAULT);      \
                                            i2c_GO_DONE_REG = i2c_PREPARE_TO_RELEASE;  \
                                            i2c_GO_REG  =     i2c_RELEASE_FSM;         \
                                        }while(0)

    #define i2c_GENERATE_STOP  \
                                        do{ \
                                            i2c_CFG_REG    = (i2c_CTRL_NACK_MASK |    \
                                                                           i2c_CTRL_STOP_MASK |    \
                                                                           i2c_CTRL_DEFAULT);      \
                                            i2c_GO_DONE_REG = i2c_PREPARE_TO_RELEASE; \
                                            i2c_GO_REG      = i2c_RELEASE_FSM;        \
                                        }while(0)

    /* Master manual APIs compatible macros */
    /* These macros wait until byte complete history is cleared after command issued */
    #define i2c_GENERATE_START_MANUAL \
                                        do{ \
                                            i2c_GENERATE_START;                  \
                                            /* Wait until byte complete history is cleared */ \
                                            while(i2c_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)
                                        
    #define i2c_GENERATE_RESTART_MANUAL \
                                        do{          \
                                            i2c_GENERATE_RESTART;                \
                                            /* Wait until byte complete history is cleared */ \
                                            while(i2c_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define i2c_GENERATE_STOP_MANUAL \
                                        do{       \
                                            i2c_GENERATE_STOP;                   \
                                            /* Wait until byte complete history is cleared */ \
                                            while(i2c_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define i2c_TRANSMIT_DATA_MANUAL \
                                        do{       \
                                            i2c_TRANSMIT_DATA;                   \
                                            /* Wait until byte complete history is cleared */ \
                                            while(i2c_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define i2c_READY_TO_READ_MANUAL \
                                        do{       \
                                            i2c_READY_TO_READ;                   \
                                            /* Wait until byte complete history is cleared */ \
                                            while(i2c_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define i2c_ACK_AND_RECEIVE_MANUAL \
                                        do{         \
                                            i2c_ACK_AND_RECEIVE;                 \
                                            /* Wait until byte complete history is cleared */ \
                                            while(i2c_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define i2c_BUS_RELEASE_MANUAL i2c_READY_TO_READ_MANUAL

#endif /* (i2c_FF_IMPLEMENTED) */


/***************************************
*     Default register init constants
***************************************/

#define i2c_DISABLE    (0u)
#define i2c_ENABLE     (1u)

#if (i2c_FF_IMPLEMENTED)
    /* i2c_XCFG_REG: bits definition */
    #define i2c_DEFAULT_XCFG_HW_ADDR_EN ((i2c_HW_ADRR_DECODE) ? \
                                                        (i2c_XCFG_HDWR_ADDR_EN) : (0u))

    #define i2c_DEFAULT_XCFG_I2C_ON    ((i2c_WAKEUP_ENABLED) ? \
                                                        (i2c_XCFG_I2C_ON) : (0u))


    #define i2c_DEFAULT_CFG_SIO_SELECT ((i2c_I2C1_SIO_PAIR) ? \
                                                        (i2c_CFG_SIO_SELECT) : (0u))


    /* i2c_CFG_REG: bits definition */
    #define i2c_DEFAULT_CFG_PSELECT    ((i2c_WAKEUP_ENABLED) ? \
                                                        (i2c_CFG_PSELECT) : (0u))

    #define i2c_DEFAULT_CLK_RATE0  ((i2c_DATA_RATE <= 50u) ?        \
                                                    (i2c_CFG_CLK_RATE_050) :     \
                                                    ((i2c_DATA_RATE <= 100u) ?   \
                                                        (i2c_CFG_CLK_RATE_100) : \
                                                        (i2c_CFG_CLK_RATE_400)))

    #define i2c_DEFAULT_CLK_RATE1  ((i2c_DATA_RATE <= 50u) ?           \
                                                 (i2c_CFG_CLK_RATE_LESS_EQUAL_50) : \
                                                 (i2c_CFG_CLK_RATE_GRATER_50))

    #define i2c_DEFAULT_CLK_RATE   (i2c_DEFAULT_CLK_RATE1)


    #define i2c_ENABLE_MASTER      ((i2c_MODE_MASTER_ENABLED) ? \
                                                 (i2c_CFG_EN_MSTR) : (0u))

    #define i2c_ENABLE_SLAVE       ((i2c_MODE_SLAVE_ENABLED) ? \
                                                 (i2c_CFG_EN_SLAVE) : (0u))

    #define i2c_ENABLE_MS      (i2c_ENABLE_MASTER | i2c_ENABLE_SLAVE)


    /* i2c_DEFAULT_XCFG_REG */
    #define i2c_DEFAULT_XCFG   (i2c_XCFG_CLK_EN         | \
                                             i2c_DEFAULT_XCFG_I2C_ON | \
                                             i2c_DEFAULT_XCFG_HW_ADDR_EN)

    /* i2c_DEFAULT_CFG_REG */
    #define i2c_DEFAULT_CFG    (i2c_DEFAULT_CFG_SIO_SELECT | \
                                             i2c_DEFAULT_CFG_PSELECT    | \
                                             i2c_DEFAULT_CLK_RATE       | \
                                             i2c_ENABLE_MASTER          | \
                                             i2c_ENABLE_SLAVE)

    /*i2c_DEFAULT_DIVIDE_FACTOR_REG */
    #define i2c_DEFAULT_DIVIDE_FACTOR  ((uint16) 1u)

#else
    /* i2c_CFG_REG: bits definition  */
    #define i2c_ENABLE_MASTER  ((i2c_MODE_MASTER_ENABLED) ? \
                                             (i2c_CTRL_ENABLE_MASTER_MASK) : (0u))

    #define i2c_ENABLE_SLAVE   ((i2c_MODE_SLAVE_ENABLED) ? \
                                             (i2c_CTRL_ENABLE_SLAVE_MASK) : (0u))

    #define i2c_ENABLE_MS      (i2c_ENABLE_MASTER | i2c_ENABLE_SLAVE)


    #define i2c_DEFAULT_CTRL_ANY_ADDR   ((i2c_HW_ADRR_DECODE) ? \
                                                      (0u) : (i2c_CTRL_ANY_ADDRESS_MASK))

    /* i2c_DEFAULT_CFG_REG */
    #define i2c_DEFAULT_CFG    (i2c_DEFAULT_CTRL_ANY_ADDR)

    /* All CTRL default bits to be used in macro */
    #define i2c_CTRL_DEFAULT   (i2c_DEFAULT_CTRL_ANY_ADDR | i2c_ENABLE_MS)

    /* Master clock generator: d0 and d1 */
    #define i2c_MCLK_PERIOD_VALUE  (0x0Fu)
    #define i2c_MCLK_COMPARE_VALUE (0x08u)

    /* Slave bit-counter: control period */
    #define i2c_PERIOD_VALUE       (0x07u)

    /* i2c_DEFAULT_INT_MASK */
    #define i2c_DEFAULT_INT_MASK   (i2c_BYTE_COMPLETE_IE_MASK)

    /* i2c_DEFAULT_MCLK_PRD_REG */
    #define i2c_DEFAULT_MCLK_PRD   (i2c_MCLK_PERIOD_VALUE)

    /* i2c_DEFAULT_MCLK_CMP_REG */
    #define i2c_DEFAULT_MCLK_CMP   (i2c_MCLK_COMPARE_VALUE)

    /* i2c_DEFAULT_PERIOD_REG */
    #define i2c_DEFAULT_PERIOD     (i2c_PERIOD_VALUE)

#endif /* (i2c_FF_IMPLEMENTED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

#define i2c_SSTAT_RD_ERR       (0x08u)
#define i2c_SSTAT_WR_ERR       (0x80u)
#define i2c_MSTR_SLAVE_BUSY    (i2c_MSTR_NOT_READY)
#define i2c_MSTAT_ERR_BUF_OVFL (0x80u)
#define i2c_SSTAT_RD_CMPT      (i2c_SSTAT_RD_CMPLT)
#define i2c_SSTAT_WR_CMPT      (i2c_SSTAT_WR_CMPLT)
#define i2c_MODE_MULTI_MASTER_ENABLE    (i2c_MODE_MULTI_MASTER_MASK)
#define i2c_DATA_RATE_50       (50u)
#define i2c_DATA_RATE_100      (100u)
#define i2c_DEV_MASK           (0xF0u)
#define i2c_SM_SL_STOP         (0x14u)
#define i2c_SM_MASTER_IDLE     (0x40u)
#define i2c_HDWR_DECODE        (0x01u)

#endif /* CY_I2C_i2c_H */


/* [] END OF FILE */
