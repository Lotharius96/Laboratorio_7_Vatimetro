/*******************************************************************************
* File Name: timer.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_timer_H)
#define CY_CLOCK_timer_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void timer_Start(void) ;
void timer_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void timer_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void timer_StandbyPower(uint8 state) ;
void timer_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 timer_GetDividerRegister(void) ;
void timer_SetModeRegister(uint8 modeBitMask) ;
void timer_ClearModeRegister(uint8 modeBitMask) ;
uint8 timer_GetModeRegister(void) ;
void timer_SetSourceRegister(uint8 clkSource) ;
uint8 timer_GetSourceRegister(void) ;
#if defined(timer__CFG3)
void timer_SetPhaseRegister(uint8 clkPhase) ;
uint8 timer_GetPhaseRegister(void) ;
#endif /* defined(timer__CFG3) */

#define timer_Enable()                       timer_Start()
#define timer_Disable()                      timer_Stop()
#define timer_SetDivider(clkDivider)         timer_SetDividerRegister(clkDivider, 1u)
#define timer_SetDividerValue(clkDivider)    timer_SetDividerRegister((clkDivider) - 1u, 1u)
#define timer_SetMode(clkMode)               timer_SetModeRegister(clkMode)
#define timer_SetSource(clkSource)           timer_SetSourceRegister(clkSource)
#if defined(timer__CFG3)
#define timer_SetPhase(clkPhase)             timer_SetPhaseRegister(clkPhase)
#define timer_SetPhaseValue(clkPhase)        timer_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(timer__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define timer_CLKEN              (* (reg8 *) timer__PM_ACT_CFG)
#define timer_CLKEN_PTR          ((reg8 *) timer__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define timer_CLKSTBY            (* (reg8 *) timer__PM_STBY_CFG)
#define timer_CLKSTBY_PTR        ((reg8 *) timer__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define timer_DIV_LSB            (* (reg8 *) timer__CFG0)
#define timer_DIV_LSB_PTR        ((reg8 *) timer__CFG0)
#define timer_DIV_PTR            ((reg16 *) timer__CFG0)

/* Clock MSB divider configuration register. */
#define timer_DIV_MSB            (* (reg8 *) timer__CFG1)
#define timer_DIV_MSB_PTR        ((reg8 *) timer__CFG1)

/* Mode and source configuration register */
#define timer_MOD_SRC            (* (reg8 *) timer__CFG2)
#define timer_MOD_SRC_PTR        ((reg8 *) timer__CFG2)

#if defined(timer__CFG3)
/* Analog clock phase configuration register */
#define timer_PHASE              (* (reg8 *) timer__CFG3)
#define timer_PHASE_PTR          ((reg8 *) timer__CFG3)
#endif /* defined(timer__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define timer_CLKEN_MASK         timer__PM_ACT_MSK
#define timer_CLKSTBY_MASK       timer__PM_STBY_MSK

/* CFG2 field masks */
#define timer_SRC_SEL_MSK        timer__CFG2_SRC_SEL_MASK
#define timer_MODE_MASK          (~(timer_SRC_SEL_MSK))

#if defined(timer__CFG3)
/* CFG3 phase mask */
#define timer_PHASE_MASK         timer__CFG3_PHASE_DLY_MASK
#endif /* defined(timer__CFG3) */

#endif /* CY_CLOCK_timer_H */


/* [] END OF FILE */
