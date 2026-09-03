/*==================================================================================================
*   Project              : RTD AUTOSAR 4.9
*   Platform             : CORTEXM
*   Peripheral           :
*   Dependencies         : none
*
*   Autosar Version      : 4.9.0
*   Autosar Revision     : ASR_REL_4_9_REV_0000
*   Autosar Conf.Variant :
*   SW Version           : 7.0.1
*   Build Version        : S32K3_RTD_7_0_1_D2602_ASR_REL_4_9_REV_0000_20260206
*
*   Copyright 2020 - 2026 NXP
*
*   NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be
*   used strictly in accordance with the applicable license terms. By expressly
*   accepting such terms or by downloading, installing, activating and/or otherwise
*   using the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms. If you do not agree to be
*   bound by the applicable license terms, then you may not retain, install,
*   activate or otherwise use the software.
==================================================================================================*/

/**
*   @file       Clock_Ip_ProgFreqSwitch.c
*   @version    7.0.1
*
*   @brief   CLOCK driver implementations.
*   @details CLOCK driver implementations.
*
*   @addtogroup CLOCK_DRIVER Clock Ip Driver
*   @{
*/


#ifdef __cplusplus
extern "C"{
#endif


/*==================================================================================================
*                                          INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/

#include "Clock_Ip_Private.h"

/*==================================================================================================
                               SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define CLOCK_IP_PROGFREQSWITCH_VENDOR_ID_C                      43
#define CLOCK_IP_PROGFREQSWITCH_AR_RELEASE_MAJOR_VERSION_C       4
#define CLOCK_IP_PROGFREQSWITCH_AR_RELEASE_MINOR_VERSION_C       9
#define CLOCK_IP_PROGFREQSWITCH_AR_RELEASE_REVISION_VERSION_C    0
#define CLOCK_IP_PROGFREQSWITCH_SW_MAJOR_VERSION_C               7
#define CLOCK_IP_PROGFREQSWITCH_SW_MINOR_VERSION_C               0
#define CLOCK_IP_PROGFREQSWITCH_SW_PATCH_VERSION_C               1

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if Clock_Ip_ProgFreqSwitch.c file and Clock_Ip_Private.h file are of the same vendor */
#if (CLOCK_IP_PROGFREQSWITCH_VENDOR_ID_C != CLOCK_IP_PRIVATE_VENDOR_ID)
    #error "Clock_Ip_ProgFreqSwitch.c and Clock_Ip_Private.h have different vendor ids"
#endif

/* Check if Clock_Ip_ProgFreqSwitch.c file and Clock_Ip_Private.h file are of the same Autosar version */
#if ((CLOCK_IP_PROGFREQSWITCH_AR_RELEASE_MAJOR_VERSION_C != CLOCK_IP_PRIVATE_AR_RELEASE_MAJOR_VERSION) || \
     (CLOCK_IP_PROGFREQSWITCH_AR_RELEASE_MINOR_VERSION_C != CLOCK_IP_PRIVATE_AR_RELEASE_MINOR_VERSION) || \
     (CLOCK_IP_PROGFREQSWITCH_AR_RELEASE_REVISION_VERSION_C != CLOCK_IP_PRIVATE_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of Clock_Ip_ProgFreqSwitch.c and Clock_Ip_Private.h are different"
#endif

/* Check if Clock_Ip_ProgFreqSwitch.c file and Clock_Ip_Private.h file are of the same Software version */
#if ((CLOCK_IP_PROGFREQSWITCH_SW_MAJOR_VERSION_C != CLOCK_IP_PRIVATE_SW_MAJOR_VERSION) || \
     (CLOCK_IP_PROGFREQSWITCH_SW_MINOR_VERSION_C != CLOCK_IP_PRIVATE_SW_MINOR_VERSION) || \
     (CLOCK_IP_PROGFREQSWITCH_SW_PATCH_VERSION_C != CLOCK_IP_PRIVATE_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of Clock_Ip_ProgFreqSwitch.c and Clock_Ip_Private.h are different"
#endif

/*==================================================================================================
*                           LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                          LOCAL MACROS
==================================================================================================*/
/*==================================================================================================
*                                         LOCAL MACROS
==================================================================================================*/
#ifdef CLOCK_IP_CGM_X_PCFS_SDUR_DIVC_DIVE_DIVS
/* Pcfs settings that are dependent on device */
#define CLOCK_IP_A_MAX_SIZE 4U

/* microA per MHz */
#define CLOCK_IP_DYNAMIC_IDD_CHANGE 2360U

#define CLOCK_IP_CONVERT_MHZ_TO_HZ                              1000000U
#define CLOCK_IP_CONVERT_MICROSECONDS_TO_SECONDS                1000000U
#define CLOCK_IP_MULTIPLY_AmaxBrut_BY_1000                      1000U
#define CLOCK_IP_MULTIPLY_dynamicIDDchange_BY_1000              1000U
#define CLOCK_IP_CONSTANT_2048000                               (1024U * 2000U)
#if (defined(CLOCK_IP_DEV_ERROR_DETECT) && (CLOCK_IP_DEV_ERROR_DETECT == STD_ON))
    #define CLOCK_IP_MAX_REGISTER_VALUE                         (uint32)0xFFFFFFFFU
#endif
#endif

/*==================================================================================================
*                                         LOCAL CONSTANTS
==================================================================================================*/

/* Clock start constant section data */
#define MCU_START_SEC_CONST_32
#include "Mcu_MemMap.h"

#ifdef CLOCK_IP_CGM_X_PCFS_SDUR_DIVC_DIVE_DIVS
static const uint32 AMax[CLOCK_IP_A_MAX_SIZE] = {5U,10U,150U,200U};
static const uint32 PcfsRate[CLOCK_IP_A_MAX_SIZE] = {12U,48U,112U,184U};
#endif

/* Clock stop constant section data */
#define MCU_STOP_SEC_CONST_32
#include "Mcu_MemMap.h"

/*==================================================================================================
*                                         LOCAL VARIABLES
==================================================================================================*/

/* Clock start initialized section data */
#define MCU_START_SEC_VAR_CLEARED_32
#include "Mcu_MemMap.h"

#ifdef CLOCK_IP_CGM_X_PCFS_SDUR_DIVC_DIVE_DIVS
static uint64 HashPfs[CLOCK_IP_PCFS_COUNT];
#endif

/* Clock stop initialized section data */
#define MCU_STOP_SEC_VAR_CLEARED_32
#include "Mcu_MemMap.h"

/*==================================================================================================
*                                        GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
/* Clock start section code */
#define MCU_START_SEC_CODE

#include "Mcu_MemMap.h"


static void Clock_Ip_ProgressiveFrequencyClockSwitchEmpty(  Clock_Ip_PcfsConfigType const* Config,
                                                            uint32 Index
                                                          );
#ifdef CLOCK_IP_CGM_X_PCFS_SDUR_DIVC_DIVE_DIVS
static void Clock_Ip_CgmXPcfsSdurDivcDiveDivs(Clock_Ip_PcfsConfigType const *Config, uint32 CfgIndex);
#endif

/* Clock stop section code */
#define MCU_STOP_SEC_CODE

#include "Mcu_MemMap.h"
/*==================================================================================================
*                                         LOCAL FUNCTIONS
==================================================================================================*/


/* Clock start section code */
#define MCU_START_SEC_CODE

#include "Mcu_MemMap.h"

static void Clock_Ip_ProgressiveFrequencyClockSwitchEmpty(  Clock_Ip_PcfsConfigType const* Config,
                                                            uint32 Index
                                                          )
{
    (void)Config;
    (void)Index;
    /* No implementation */
}

#ifdef CLOCK_IP_CGM_X_PCFS_SDUR_DIVC_DIVE_DIVS
static void Clock_Ip_CgmXPcfsSdurDivcDiveDivs(  Clock_Ip_PcfsConfigType const *Config,
                                                uint32 CfgIndex
                                              )
{
    volatile Clock_Ip_CgmPcfsType* CgmPcfsBase;
    uint32 HwIndex;

    uint64 Finput = 0U;
    uint64 Fsafe = 0U;
    uint64 AmaxBrut;
    uint64 Rate = 0U;
    uint64 Index;
    uint64 Var1;
    uint64 Var2;
    uint64 Var3;
    uint64 K;

    uint32 Sdur;
    uint32 DivcInit;
    uint32 DivcRate;
    uint32 DivStartValue;
    uint32 DivEndValue;

    if (NULL_PTR != Config)
    {
        CgmPcfsBase  = Clock_Ip_apxCgmPcfs[Clock_Ip_au8ClockFeatures[Config->SelectorName][CLOCK_IP_MODULE_INSTANCE]];
        HwIndex      = Clock_Ip_au8ClockFeatures[Config->Name][CLOCK_IP_PCFS_INDEX];

        #if (defined(CLOCK_IP_DEV_ERROR_DETECT) && (CLOCK_IP_DEV_ERROR_DETECT == STD_ON))
        CLOCK_IP_DEV_ASSERT(Config->SelectorName != RESERVED_CLK);
        #endif

        if (HashPfs[CfgIndex] != (((Config->ClockSourceFrequency) ^ (Config->MaxAllowableIDDchange) ^ ((uint32)Config->Name)  ^ ((uint32)Config->SelectorName) ^ (Config->StepDuration))))
        {
            HashPfs[CfgIndex] = (((Config->ClockSourceFrequency) ^ (Config->MaxAllowableIDDchange) ^ ((uint32)Config->Name)  ^ ((uint32)Config->SelectorName) ^ (Config->StepDuration)));

            Finput = (Config->ClockSourceFrequency);
        #if defined(CLOCK_IP_HAS_FIRC_CLK)
            Fsafe = ((*Clock_Ip_pxConfig->ConfiguredFrequencies)[Clock_Ip_FreqIds[FIRC_CLK]].ConfiguredFrequencyValue);
        #endif
        #if (defined(CLOCK_IP_HAS_P5_AE_CLK) && defined(CLOCK_IP_HAS_FIRC_AE_CLK))
            if (P5_AE_CLK == Config->Name)
            {
                Fsafe = ((*Clock_Ip_pxConfig->ConfiguredFrequencies)[Clock_Ip_FreqIds[FIRC_AE_CLK]].ConfiguredFrequencyValue);
            }
        #endif

        #if (defined(CLOCK_IP_DEV_ERROR_DETECT) && (CLOCK_IP_DEV_ERROR_DETECT == STD_ON))
            CLOCK_IP_DEV_ASSERT(Finput != 0U);
            CLOCK_IP_DEV_ASSERT(Fsafe != 0U);
            CLOCK_IP_DEV_ASSERT(Config->MaxAllowableIDDchange != 0U);
            CLOCK_IP_DEV_ASSERT(Config->StepDuration != 0U);
        #endif

            /* Calculate amax=fchg/Finput */
            AmaxBrut = ((uint64)Config->MaxAllowableIDDchange * CLOCK_IP_CONVERT_MHZ_TO_HZ * CLOCK_IP_MULTIPLY_dynamicIDDchange_BY_1000 * CLOCK_IP_MULTIPLY_AmaxBrut_BY_1000 / (Finput * CLOCK_IP_DYNAMIC_IDD_CHANGE));
            Rate = AmaxBrut;

            /* Round pcfs rate by rounding amax */
            if (AmaxBrut <= AMax[0U])
            {
                Rate = PcfsRate[0U];
            }
            else if (AmaxBrut >= AMax[CLOCK_IP_A_MAX_SIZE-1U])
            {
                Rate = PcfsRate[CLOCK_IP_A_MAX_SIZE-1U];
            }
            else
            {
                for (Index = 1U; Index < (uint8)CLOCK_IP_A_MAX_SIZE; Index++)
                {
                    if (AMax[Index-1U] < AmaxBrut)
                    {
                        Rate = PcfsRate[Index-1U];
                    }
                }
            }

            /* Calculate K by using formula k = ceil(0.5 + sqrt(0.25 - (2000 * (1 -(Finput/fsafe)) / Rate))) */

            Var1 = 256U + ((CLOCK_IP_CONSTANT_2048000 * Finput) / (Fsafe * Rate)) - (CLOCK_IP_CONSTANT_2048000 / Rate);
            Var2 = 1UL << 30U;   /* The second-to-top bit is set: use 1u << 14 for uint16 type; use 1uL<<30 for uint32 type  */
            Var3 = 0U;

            /* Implement sqrt from K formula by using a square-root computing in embedded C */

            /* "one" starts at the highest power of four <= than the argument */
            while (Var2 > Var1)
            {
                Var2 = Var2 >> 2;
            }
            /* Implement sqrt from K formula by using a square-root computing in embedded C */

            while (Var2 != 0U)
            {
                if (Var1 >= (Var3 + Var2))
                {
                    Var1 = Var1 - (Var3 + Var2);
                    Var3 = Var3 + (Var2 << 1U);
                }

                Var3 = Var3 >> 1U;
                Var2 = Var2 >> 2U;
            }

            K = (64U + 127U + (Var3 << 2U)) >> 7U;   /* Calculated K from k = ceil(0.5 + sqrt(0.25 - (2000 * (1 -(Fi/Fsafe)) / Rate))) */

        #if (defined(CLOCK_IP_DEV_ERROR_DETECT) && (CLOCK_IP_DEV_ERROR_DETECT == STD_ON))
            CLOCK_IP_DEV_ASSERT((Config->StepDuration * Fsafe / CLOCK_IP_CONVERT_MICROSECONDS_TO_SECONDS)       < CLOCK_IP_MAX_REGISTER_VALUE); /* Overflow check for Sdur */
            CLOCK_IP_DEV_ASSERT((Rate * K)                           < CLOCK_IP_MAX_REGISTER_VALUE); /* Overflow check for DivcInit */
            CLOCK_IP_DEV_ASSERT((Rate)                               < CLOCK_IP_MAX_REGISTER_VALUE); /* Overflow check for DivcRate */
            CLOCK_IP_DEV_ASSERT((999U + ((Rate * K * (K+1U)) >> 1U)) < CLOCK_IP_MAX_REGISTER_VALUE); /* Overflow check for DivStartValue */
            CLOCK_IP_DEV_ASSERT(((Finput * 1000U / Fsafe) - 1U)      < CLOCK_IP_MAX_REGISTER_VALUE); /* Overflow check for DivEndValue */
        #endif

            Sdur = (uint32)(Config->StepDuration * Fsafe / CLOCK_IP_CONVERT_MICROSECONDS_TO_SECONDS);
            DivcInit = (uint32)(Rate * K);
            DivcRate = (uint32)(Rate);
            DivStartValue = (uint32)(999U + ((Rate * K * (K+1U)) >> 1U));
            DivEndValue = (uint32)((Finput * 1000U / Fsafe) - 1U);

            /* Configure pcfs registers */
        #if (defined(CLOCK_IP_HAS_P5_AE_CLK) && defined(CLOCK_IP_HAS_FIRC_AE_CLK))
            if (P5_AE_CLK==Config->Name)
            {
                #if (defined(CLOCK_IP_DEV_ERROR_DETECT) && (CLOCK_IP_DEV_ERROR_DETECT == STD_ON))
                CLOCK_IP_DEV_ASSERT(Sdur < 256U);
                #endif
                IP_MC_CGM_AE->PCS_SDUR = MC_CGM_PCS_SDUR_SDUR(Sdur);
                IP_MC_CGM_AE->PCS_DIVC1 = MC_CGM_PCS_DIVC1_RATE(DivcRate) | MC_CGM_PCS_DIVC1_INIT(DivcInit);
                IP_MC_CGM_AE->PCS_DIVE1 = MC_CGM_PCS_DIVE1_DIVE(DivEndValue);
                IP_MC_CGM_AE->PCS_DIVS1 = MC_CGM_PCS_DIVS1_DIVS(DivStartValue);
            }
            else
            {
        #endif
                CgmPcfsBase->PCFS_SDUR = MC_CGM_PCFS_SDUR_SDUR(Sdur);
                CgmPcfsBase->PCFS[HwIndex].DIVC = MC_CGM_PCFS_DIVC_RATE(DivcRate) | MC_CGM_PCFS_DIVC_INIT(DivcInit);
                CgmPcfsBase->PCFS[HwIndex].DIVE = MC_CGM_PCFS_DIVE_DIVE(DivEndValue);
                CgmPcfsBase->PCFS[HwIndex].DIVS = MC_CGM_PCFS_DIVS_DIVS(DivStartValue);
        #if (defined(CLOCK_IP_HAS_P5_AE_CLK) && defined(CLOCK_IP_HAS_FIRC_AE_CLK))
            }
        #endif
        }
    }
    else
    {
        (void)CfgIndex;
        (void)CgmPcfsBase;
        (void)HwIndex;
        (void)Finput;
        (void)Fsafe;
        (void)AmaxBrut;
        (void)Rate;
        (void)Index;
        (void)Var1;
        (void)Var2;
        (void)Var3;
        (void)K;
        (void)Sdur;
        (void)DivcInit;
        (void)DivcRate;
        (void)DivStartValue;
        (void)DivEndValue;
    }
}
#endif
/* Clock stop section code */
#define MCU_STOP_SEC_CODE

#include "Mcu_MemMap.h"

/*==================================================================================================
*                                        GLOBAL FUNCTIONS
==================================================================================================*/


/*==================================================================================================
*                                        GLOBAL CONSTANTS
==================================================================================================*/

/* Clock start constant section data */
#define MCU_START_SEC_CONST_UNSPECIFIED

#include "Mcu_MemMap.h"

const Clock_Ip_PcfsCallbackType Clock_Ip_axPcfsCallbacks[CLOCK_IP_PCFS_CALLBACKS_COUNT] =
{
    {
        &Clock_Ip_ProgressiveFrequencyClockSwitchEmpty,     /* Set */

    },
#ifdef CLOCK_IP_CGM_X_PCFS_SDUR_DIVC_DIVE_DIVS
    {
        &Clock_Ip_CgmXPcfsSdurDivcDiveDivs,              /* Set */
    },
#endif
};

/* Clock stop constant section data */
#define MCU_STOP_SEC_CONST_UNSPECIFIED

#include "Mcu_MemMap.h"


#ifdef __cplusplus
}
#endif

/** @} */
