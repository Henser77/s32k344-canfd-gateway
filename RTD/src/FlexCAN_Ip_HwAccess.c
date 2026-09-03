/*==================================================================================================
*   Project              : RTD AUTOSAR 4.9
*   Platform             : CORTEXM
*   Peripheral           : FLEXCAN
*   Dependencies         :
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
 *  @file FlexCAN_Ip_HwAccess.c
 *
 *  @brief FlexCAN Ip Hardware Access Source File
 *  @details Implementing functionality of FlexCAN Ip Hardware Access APIs
 *
 *  @addtogroup FlexCAN
 *  @{
 */

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "FlexCAN_Ip_HwAccess.h"
#include "SchM_Can_43_FLEXCAN.h"
#include "OsIf.h"
/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define FLEXCAN_IP_HWACCESS_VENDOR_ID_C                      43
#define FLEXCAN_IP_HWACCESS_AR_RELEASE_MAJOR_VERSION_C       4
#define FLEXCAN_IP_HWACCESS_AR_RELEASE_MINOR_VERSION_C       9
#define FLEXCAN_IP_HWACCESS_AR_RELEASE_REVISION_VERSION_C    0
#define FLEXCAN_IP_HWACCESS_SW_MAJOR_VERSION_C               7
#define FLEXCAN_IP_HWACCESS_SW_MINOR_VERSION_C               0
#define FLEXCAN_IP_HWACCESS_SW_PATCH_VERSION_C               1
/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if current file and FlexCAN_Ip_HwAccess header file are of the same vendor */
#if (FLEXCAN_IP_HWACCESS_VENDOR_ID_C != FLEXCAN_IP_HWACCESS_VENDOR_ID_H)
    #error "FlexCAN_Ip_HwAccess.c and FlexCAN_Ip_HwAccess.h have different vendor ids"
#endif
/* Check if current file and CAN header file are of the same Autosar version */
#if ((FLEXCAN_IP_HWACCESS_AR_RELEASE_MAJOR_VERSION_C    != FLEXCAN_IP_HWACCESS_AR_RELEASE_MAJOR_VERSION_H) || \
     (FLEXCAN_IP_HWACCESS_AR_RELEASE_MINOR_VERSION_C    != FLEXCAN_IP_HWACCESS_AR_RELEASE_MINOR_VERSION_H) || \
     (FLEXCAN_IP_HWACCESS_AR_RELEASE_REVISION_VERSION_C != FLEXCAN_IP_HWACCESS_AR_RELEASE_REVISION_VERSION_H) \
    )
    #error "AutoSar Version Numbers of FlexCAN_Ip_HwAccess.c and FlexCAN_Ip_HwAccess.h are different"
#endif
/* Check if current file and CAN header file are of the same Software version */
#if ((FLEXCAN_IP_HWACCESS_SW_MAJOR_VERSION_C != FLEXCAN_IP_HWACCESS_SW_MAJOR_VERSION_H) || \
     (FLEXCAN_IP_HWACCESS_SW_MINOR_VERSION_C != FLEXCAN_IP_HWACCESS_SW_MINOR_VERSION_H) || \
     (FLEXCAN_IP_HWACCESS_SW_PATCH_VERSION_C != FLEXCAN_IP_HWACCESS_SW_PATCH_VERSION_H) \
    )
    #error "Software Version Numbers of FlexCAN_Ip_HwAccess.c and FlexCAN_Ip_HwAccess.h are different"
#endif

#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
    /* Check if current file and SchM_Can header file are of the same version */
    #if ((FLEXCAN_IP_HWACCESS_AR_RELEASE_MAJOR_VERSION_C    != SCHM_CAN_43_FLEXCAN_AR_RELEASE_MAJOR_VERSION) || \
        (FLEXCAN_IP_HWACCESS_AR_RELEASE_MINOR_VERSION_C     != SCHM_CAN_43_FLEXCAN_AR_RELEASE_MINOR_VERSION) \
        )
        #error "AutoSar Version Numbers of FlexCAN_Ip_HwAccess.c and SchM_Can_43_FLEXCAN.h are different"
    #endif
    /* Check if current file and OsIf header file are of the same version */
    #if ((FLEXCAN_IP_HWACCESS_AR_RELEASE_MAJOR_VERSION_C    !=  OSIF_AR_RELEASE_MAJOR_VERSION) || \
         (FLEXCAN_IP_HWACCESS_AR_RELEASE_MINOR_VERSION_C    !=  OSIF_AR_RELEASE_MINOR_VERSION) \
        )
        #error "AutoSar Version Numbers of FlexCAN_Ip_HwAccess.c and OsIf.h are different"
    #endif
#endif
/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/
/* CAN FD extended data length DLC encoding */
#define FLEXCAN_IP_DLC_VALUE_12_BYTES              9U
#define FLEXCAN_IP_DLC_VALUE_16_BYTES              10U
#define FLEXCAN_IP_DLC_VALUE_20_BYTES              11U
#define FLEXCAN_IP_DLC_VALUE_24_BYTES              12U
#define FLEXCAN_IP_DLC_VALUE_32_BYTES              13U
#define FLEXCAN_IP_DLC_VALUE_48_BYTES              14U
#define FLEXCAN_IP_DLC_VALUE_64_BYTES              15U

#define FLEXCAN_IP_RX_FIFO_FILTER_TABLE_OFFSET      0x000000E0U
#define FLEXCAN_IP_RX_FIFO_ACCEPT_REMOTE_FRAME      1UL
#define FLEXCAN_IP_RX_FIFO_ACCEPT_EXT_FRAME         1UL

#if (FLEXCAN_IP_FEATURE_HAS_ENHANCED_RX_FIFO == STD_ON)
/* Determines the base index for Enhanced RxFIFO Extended Filter element */
#define FLEXCAN_IP_ENHANCED_RXFIFO_EXT_FILTER_TABLE_BASE       0x0U
/* Determines the base index for Enhanced RxFIFO Standard Filter element. Always in range of 128U elements */
#define FLEXCAN_IP_ENHANCED_RXFIFO_STD_FILTER_TABLE_BASE(x)    (((x) * 2U) & 0x7FU)
#endif

/* Determines the RxFIFO Filter element number */
#define FLEXCAN_IP_RXFIFO_FILTER_ELEM_NUM(x) (((x) + 1U) * 8U)
/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/
#if (FLEXCAN_IP_FEATURE_NO_CACHE_NEEDED == STD_ON)
#define CAN_43_FLEXCAN_START_SEC_VAR_CLEARED_32_NO_CACHEABLE
#else
#define CAN_43_FLEXCAN_START_SEC_VAR_CLEARED_32
#endif /* (FLEXCAN_IP_FEATURE_NO_CACHE_NEEDED == STD_ON) */
#include "Can_43_FLEXCAN_MemMap.h"

static volatile uint32 FlexCAN_Ip_au32ImaskBuff[FLEXCAN_IP_INSTANCE_COUNT][FLEXCAN_IP_FEATURE_MBDSR_COUNT];

#if (FLEXCAN_IP_FEATURE_NO_CACHE_NEEDED == STD_ON)
#define CAN_43_FLEXCAN_STOP_SEC_VAR_CLEARED_32_NO_CACHEABLE
#else
#define CAN_43_FLEXCAN_STOP_SEC_VAR_CLEARED_32
#endif /* (FLEXCAN_IP_FEATURE_NO_CACHE_NEEDED == STD_ON) */
#include "Can_43_FLEXCAN_MemMap.h"

#if (FLEXCAN_IP_FEATURE_NO_CACHE_NEEDED == STD_ON)
#define CAN_43_FLEXCAN_START_SEC_VAR_CLEARED_8_NO_CACHEABLE
#else
#define CAN_43_FLEXCAN_START_SEC_VAR_CLEARED_8
#endif /* (FLEXCAN_IP_FEATURE_NO_CACHE_NEEDED == STD_ON) */
#include "Can_43_FLEXCAN_MemMap.h"

#if (FLEXCAN_IP_FEATURE_HAS_ENHANCED_RX_FIFO == STD_ON)
static volatile uint8 FilterIdMap[FLEXCAN_IP_INSTANCE_COUNT][FLEXCAN_IP_ENHANCED_RXFIFO_FILTERDEPTH];
#endif /* (FLEXCAN_IP_FEATURE_HAS_ENHANCED_RX_FIFO == STD_ON) */

#if (FLEXCAN_IP_FEATURE_NO_CACHE_NEEDED == STD_ON)
#define CAN_43_FLEXCAN_STOP_SEC_VAR_CLEARED_8_NO_CACHEABLE
#else
#define CAN_43_FLEXCAN_STOP_SEC_VAR_CLEARED_8
#endif /* (FLEXCAN_IP_FEATURE_NO_CACHE_NEEDED == STD_ON) */
#include "Can_43_FLEXCAN_MemMap.h"
/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
#define CAN_43_FLEXCAN_START_SEC_CODE
#include "Can_43_FLEXCAN_MemMap.h"
static uint8 FlexCAN_ComputeDLCValue(uint8 PayloadSize);

static void FlexCAN_ClearRAM(FLEXCAN_Type * pBase);

static void FlexCAN_SetRxFifoFilterFormatA(FLEXCAN_Type * pBase, const Flexcan_Ip_IdTableType * IdFilterTable);

static void FlexCAN_SetRxFifoFilterFormatB(FLEXCAN_Type * pBase, const Flexcan_Ip_IdTableType * IdFilterTable);

static void FlexCAN_SetRxFifoFilterFormatC(FLEXCAN_Type * pBase, const Flexcan_Ip_IdTableType * IdFilterTable);

#if (FLEXCAN_IP_FEATURE_HAS_ENHANCED_RX_FIFO == STD_ON)
static void FlexCAN_SetEnhancedRxFifoFilterStandardFormat(volatile uint32 * pFilterStdIDTable, const Flexcan_Ip_EnhancedIdTableType * pFilterStdIDElement);

static void FlexCAN_SetEnhancedRxFifoFilterExtendedFormat(volatile uint32 * pFilterExtIDTable, const Flexcan_Ip_EnhancedIdTableType * pFilterExtIDElement);
#endif /* (FLEXCAN_IP_FEATURE_HAS_ENHANCED_RX_FIFO == STD_ON) */



/*!
 * @brief Gets the payload size of the Ramblock.
 * @param   pBase         The FlexCAN base address
 * @return  The payload size in bytes
 */
static uint8 FlexCAN_GetPayloadSize(const FLEXCAN_Type * pBase, uint8 MbdsrIdx);
#if ((FLEXCAN_IP_FEATURE_HAS_TS_ENABLE == STD_ON) && (FLEXCAN_IP_FEATURE_HAS_HR_TIMER == STD_ON) && (FLEXCAN_IP_HR_TIMESTAMP_SRC_SELECTABLE == STD_ON))
static void FlexCAN_ConfigTimestampModule(uint8 Instance, const Flexcan_Ip_TimeStampConfigType * Config);
#endif
/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/
/*FUNCTION**********************************************************************
 *
 * Function Name: FLEXCAN_ComputeDLCValue
 * Description  : Computes the DLC field value, given a payload size (in bytes).
 *
 *END**************************************************************************/
static uint8 FlexCAN_ComputeDLCValue(uint8 PayloadSize)
{
    uint32 Ret = 0xFFU;                   /* 0,  1,  2,  3,  4,  5,  6,  7,  8, */
    static const uint8 PayloadCode[65] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U,
                                           /* 9 to 12 payload have DLC Code 12 Bytes */
                                           FLEXCAN_IP_DLC_VALUE_12_BYTES, FLEXCAN_IP_DLC_VALUE_12_BYTES, FLEXCAN_IP_DLC_VALUE_12_BYTES, FLEXCAN_IP_DLC_VALUE_12_BYTES,
                                           /* 13 to 16 payload have DLC Code 16 Bytes */
                                           FLEXCAN_IP_DLC_VALUE_16_BYTES, FLEXCAN_IP_DLC_VALUE_16_BYTES, FLEXCAN_IP_DLC_VALUE_16_BYTES, FLEXCAN_IP_DLC_VALUE_16_BYTES,
                                           /* 17 to 20 payload have DLC Code 20 Bytes */
                                           FLEXCAN_IP_DLC_VALUE_20_BYTES, FLEXCAN_IP_DLC_VALUE_20_BYTES, FLEXCAN_IP_DLC_VALUE_20_BYTES, FLEXCAN_IP_DLC_VALUE_20_BYTES,
                                           /* 21 to 24 payload have DLC Code 24 Bytes */
                                           FLEXCAN_IP_DLC_VALUE_24_BYTES, FLEXCAN_IP_DLC_VALUE_24_BYTES, FLEXCAN_IP_DLC_VALUE_24_BYTES, FLEXCAN_IP_DLC_VALUE_24_BYTES,
                                           /* 25 to 32 payload have DLC Code 32 Bytes */
                                           FLEXCAN_IP_DLC_VALUE_32_BYTES, FLEXCAN_IP_DLC_VALUE_32_BYTES, FLEXCAN_IP_DLC_VALUE_32_BYTES, FLEXCAN_IP_DLC_VALUE_32_BYTES,
                                           FLEXCAN_IP_DLC_VALUE_32_BYTES, FLEXCAN_IP_DLC_VALUE_32_BYTES, FLEXCAN_IP_DLC_VALUE_32_BYTES, FLEXCAN_IP_DLC_VALUE_32_BYTES,
                                           /* 33 to 48 payload have DLC Code 48 Bytes */
                                           FLEXCAN_IP_DLC_VALUE_48_BYTES, FLEXCAN_IP_DLC_VALUE_48_BYTES, FLEXCAN_IP_DLC_VALUE_48_BYTES, FLEXCAN_IP_DLC_VALUE_48_BYTES,
                                           FLEXCAN_IP_DLC_VALUE_48_BYTES, FLEXCAN_IP_DLC_VALUE_48_BYTES, FLEXCAN_IP_DLC_VALUE_48_BYTES, FLEXCAN_IP_DLC_VALUE_48_BYTES,
                                           FLEXCAN_IP_DLC_VALUE_48_BYTES, FLEXCAN_IP_DLC_VALUE_48_BYTES, FLEXCAN_IP_DLC_VALUE_48_BYTES, FLEXCAN_IP_DLC_VALUE_48_BYTES,
                                           FLEXCAN_IP_DLC_VALUE_48_BYTES, FLEXCAN_IP_DLC_VALUE_48_BYTES, FLEXCAN_IP_DLC_VALUE_48_BYTES, FLEXCAN_IP_DLC_VALUE_48_BYTES,
                                           /* 49 to 64 payload have DLC Code 64 Bytes */
                                           FLEXCAN_IP_DLC_VALUE_64_BYTES, FLEXCAN_IP_DLC_VALUE_64_BYTES, FLEXCAN_IP_DLC_VALUE_64_BYTES, FLEXCAN_IP_DLC_VALUE_64_BYTES,
                                           FLEXCAN_IP_DLC_VALUE_64_BYTES, FLEXCAN_IP_DLC_VALUE_64_BYTES, FLEXCAN_IP_DLC_VALUE_64_BYTES, FLEXCAN_IP_DLC_VALUE_64_BYTES,
                                           FLEXCAN_IP_DLC_VALUE_64_BYTES, FLEXCAN_IP_DLC_VALUE_64_BYTES, FLEXCAN_IP_DLC_VALUE_64_BYTES, FLEXCAN_IP_DLC_VALUE_64_BYTES,
                                           FLEXCAN_IP_DLC_VALUE_64_BYTES, FLEXCAN_IP_DLC_VALUE_64_BYTES, FLEXCAN_IP_DLC_VALUE_64_BYTES, FLEXCAN_IP_DLC_VALUE_64_BYTES
                                          };

    if (PayloadSize <= 64U)
    {
        Ret = PayloadCode[PayloadSize];
    }
    else
    {
        /* The argument is not a valid payload size will return 0xFF*/
    }

    return (uint8)Ret;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_ClearRAM
 * Description   : Clears FlexCAN memory positions that require initialization.
 *
 *END**************************************************************************/
static void FlexCAN_ClearRAM(FLEXCAN_Type * pBase)
{
    uint32 DataByte;
    uint32 RAMSize   = (uint32)FlexCAN_GetMaxMbNum(pBase) * (uint32)4U;
    uint32 RXIMRSize = FlexCAN_GetMaxMbNum(pBase);
    /* Address of pBase + ram offset to point to MB start address */
    volatile uint32 * RAM = &(((volatile uint32 *)pBase)[FLEXCAN_IP_FEATURE_RAM_OFFSET >> 2U]);

    /* Clear MB region */
    for (DataByte = 0U; DataByte < RAMSize; DataByte++)
    {
        RAM[DataByte] = 0x0U;
    }
    RAM = (volatile uint32 *)pBase->RXIMR;
    /* Clear RXIMR region */
    for (DataByte = 0U; DataByte < RXIMRSize; DataByte++)
    {
        RAM[DataByte] = 0x0U;
    }

#if (FLEXCAN_IP_FEATURE_HAS_MEM_ERR_DET == STD_ON)
    /* Set WRMFRZ bit in CTRL2 Register to grant write access to memory */
    pBase->CTRL2 = (pBase->CTRL2 & ~FLEXCAN_CTRL2_WRMFRZ_MASK) | FLEXCAN_CTRL2_WRMFRZ(1U);
    /* Clear RXMGMASK, RXFGMASK, RX14MASK, RX15MASK RAM mapping */
    pBase->RXMGMASK = 0U;
    pBase->RXFGMASK = 0U;
    pBase->RX14MASK = 0U;
    pBase->RX15MASK = 0U;
#if (FLEXCAN_IP_FEATURE_HAS_FD == STD_ON)
    RAM = &(((volatile uint32 *)pBase)[FLEXCAN_IP_FEATURE_RAM_OFFSET >> 2U]);
    /* Clear SMB FD region */
    for (DataByte = 0U; DataByte < (uint32)1U; DataByte++)
    {
        RAM[DataByte] = 0U;
    }
#endif
    /* Clear WRMFRZ bit in CTRL2 Register to restrict write access to memory */
    pBase->CTRL2 = (pBase->CTRL2 & ~FLEXCAN_CTRL2_WRMFRZ_MASK) | FLEXCAN_CTRL2_WRMFRZ(0U);
#endif /* if FLEXCAN_IP_FEATURE_HAS_MEM_ERR_DET */
}


/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_SetRxFifoFilterFormatA
 * Description   : Set RX FIFO ID filter table elements format A.
 *
 *END**************************************************************************/
static void FlexCAN_SetRxFifoFilterFormatA(FLEXCAN_Type * pBase, const Flexcan_Ip_IdTableType * IdFilterTable)
{
    uint32 Idx;
    uint32 Val = 0UL;
    volatile uint32 * FilterTable = &(((volatile uint32 *)pBase)[FLEXCAN_IP_RX_FIFO_FILTER_TABLE_OFFSET >> 2U]);
    uint32 NumOfFilters = (((pBase->CTRL2) & FLEXCAN_CTRL2_RFFN_MASK) >> FLEXCAN_CTRL2_RFFN_SHIFT);

    /* One full ID (standard and extended) per ID Filter Table element.*/
    for (Idx = 0U; Idx < FLEXCAN_IP_RXFIFO_FILTER_ELEM_NUM(NumOfFilters); Idx++)
    {
        Val = 0UL;

        if (IdFilterTable[Idx].isRemoteFrame)
        {
            Val = FLEXCAN_IP_RX_FIFO_ACCEPT_REMOTE_FRAME << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATAB_RTR_SHIFT;
        }

        if (IdFilterTable[Idx].isExtendedFrame)
        {
            Val |= FLEXCAN_IP_RX_FIFO_ACCEPT_EXT_FRAME << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATAB_IDE_SHIFT;
            FilterTable[Idx] = Val + ((IdFilterTable[Idx].id << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATA_EXT_SHIFT) &
                                       FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATA_EXT_MASK
                                     );
        }
        else
        {
            FilterTable[Idx] = Val + ((IdFilterTable[Idx].id << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATA_STD_SHIFT) &
                                       FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATA_STD_MASK
                                     );
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_SetRxFifoFilterFormatB
 * Description   : Set RX FIFO ID filter table elements format B.
 *
 *END**************************************************************************/
static void FlexCAN_SetRxFifoFilterFormatB(FLEXCAN_Type * pBase, const Flexcan_Ip_IdTableType * IdFilterTable)
{
    uint32 Idx1;
    uint32 Idx2;
    uint32 Val1 = 0UL;
    uint32 Val2 = 0UL;
    volatile uint32 * FilterTable = &(((volatile uint32 *)pBase)[FLEXCAN_IP_RX_FIFO_FILTER_TABLE_OFFSET >> 2U]);
    uint32 NumOfFilters = (((pBase->CTRL2) & FLEXCAN_CTRL2_RFFN_MASK) >> FLEXCAN_CTRL2_RFFN_SHIFT);

    /* Two full standard IDs or two partial 14-bit (standard and extended) IDs*/
    /* per ID Filter Table element.*/
    Idx2 = 0U;
    for (Idx1 = 0U; Idx1 < FLEXCAN_IP_RXFIFO_FILTER_ELEM_NUM(NumOfFilters); Idx1++)
    {
        Val1 = 0U;
        Val2 = 0U;

        if (IdFilterTable[Idx2].isRemoteFrame)
        {
            Val1 = FLEXCAN_IP_RX_FIFO_ACCEPT_REMOTE_FRAME << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATAB_RTR_SHIFT;
        }

        if (IdFilterTable[Idx2 + 1U].isRemoteFrame)
        {
            Val2 = FLEXCAN_IP_RX_FIFO_ACCEPT_REMOTE_FRAME << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATB_RTR_SHIFT;
        }

        if (IdFilterTable[Idx2].isExtendedFrame)
        {
            Val1 |= FLEXCAN_IP_RX_FIFO_ACCEPT_EXT_FRAME << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATAB_IDE_SHIFT;

            FilterTable[Idx1] = Val1 + (((IdFilterTable[Idx2].id & FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATB_EXT_MASK) >>
                                          FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATB_EXT_CMP_SHIFT
                                        ) << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATB_EXT_SHIFT1
                                       );
        }
        else
        {
            FilterTable[Idx1] = Val1 + ((IdFilterTable[Idx2].id & FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATB_STD_MASK) <<
                                         FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATB_STD_SHIFT1
                                       );
        }

        if (IdFilterTable[Idx2 + 1U].isExtendedFrame)
        {
            Val2 |= FLEXCAN_IP_RX_FIFO_ACCEPT_EXT_FRAME << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATB_IDE_SHIFT;

            FilterTable[Idx1] |= Val2 + (((IdFilterTable[Idx2 + 1U].id & FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATB_EXT_MASK) >>
                                           FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATB_EXT_CMP_SHIFT
                                         ) << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATB_EXT_SHIFT2
                                        );
        }
        else
        {
            FilterTable[Idx1] |= Val2 + ((IdFilterTable[Idx2 + 1U].id & FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATB_STD_MASK) <<
                                          FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATB_STD_SHIFT2
                                        );
        }

        Idx2 = Idx2 + 2U;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_SetRxFifoFilterFormatC
 * Description   : Set RX FIFO ID filter table elements format C.
 *
 *END**************************************************************************/
static void FlexCAN_SetRxFifoFilterFormatC(FLEXCAN_Type * pBase, const Flexcan_Ip_IdTableType * IdFilterTable)
{
    uint32 Idx1;
    uint32 Idx2;
    volatile uint32 * FilterTable = &(((volatile uint32 *)pBase)[FLEXCAN_IP_RX_FIFO_FILTER_TABLE_OFFSET >> 2U]);
    uint32 NumOfFilters = (((pBase->CTRL2) & FLEXCAN_CTRL2_RFFN_MASK) >> FLEXCAN_CTRL2_RFFN_SHIFT);

    /* Four partial 8-bit Standard IDs per ID Filter Table element.*/
    Idx2 = 0U;
    for (Idx1 = 0U; Idx1 < FLEXCAN_IP_RXFIFO_FILTER_ELEM_NUM(NumOfFilters); Idx1++)
    {
        if (IdFilterTable[Idx2].isExtendedFrame)
        {
            FilterTable[Idx1] = (((IdFilterTable[Idx2].id & FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_EXT_MASK) >>
                                   FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_EXT_CMP_SHIFT
                                 ) << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_SHIFT1
                                );
        }
        else
        {
            FilterTable[Idx1] = (((IdFilterTable[Idx2].id & FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_STD_MASK) >>
                                   FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_STD_CMP_SHIFT
                                 ) << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_SHIFT1
                                );
        }

        if (IdFilterTable[Idx2 + 1U].isExtendedFrame)
        {
            FilterTable[Idx1] |= (((IdFilterTable[Idx2 + 1U].id & FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_EXT_MASK) >>
                                    FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_EXT_CMP_SHIFT
                                  ) << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_SHIFT2
                                 );
        }
        else
        {
            FilterTable[Idx1] |= (((IdFilterTable[Idx2 + 1U].id & FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_STD_MASK) >>
                                    FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_STD_CMP_SHIFT
                                  ) << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_SHIFT2
                                 );
        }

        if (IdFilterTable[Idx2 + 2U].isExtendedFrame)
        {
            FilterTable[Idx1] |= (((IdFilterTable[Idx2 + 2U].id & FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_EXT_MASK) >>
                                    FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_EXT_CMP_SHIFT
                                  ) << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_SHIFT3
                                 );
        }
        else
        {
            FilterTable[Idx1] |= (((IdFilterTable[Idx2 + 2U].id & FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_STD_MASK) >>
                                    FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_STD_CMP_SHIFT
                                  ) << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_SHIFT3
                                 );
        }

        if (IdFilterTable[Idx2 + 3U].isExtendedFrame)
        {
            FilterTable[Idx1] |= (((IdFilterTable[Idx2 + 3U].id & FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_EXT_MASK) >>
                                    FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_EXT_CMP_SHIFT
                                  ) << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_SHIFT4
                                 );
        }
        else
        {
            FilterTable[Idx1] |= (((IdFilterTable[Idx2 + 3U].id & FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_STD_MASK) >>
                                    FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_STD_CMP_SHIFT
                                  ) << FLEXCAN_IP_RX_FIFO_ID_FILTER_FORMATC_SHIFT4
                                 );
        }

        Idx2 = Idx2 + 4U;
    }
}

#if (FLEXCAN_IP_FEATURE_HAS_ENHANCED_RX_FIFO == STD_ON)
/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_SetEnhancedRxFifoFilterStandardFormat
 * Description   : Configure Enhanced RX FIFO ID filter standard format element.
 *
 *END**************************************************************************/
static void FlexCAN_SetEnhancedRxFifoFilterStandardFormat(volatile uint32 * pFilterStdIDTable, const Flexcan_Ip_EnhancedIdTableType * pFilterStdIDElement)
{
    uint32 RtrFilter = 0U;

    if (pFilterStdIDElement->rtr2)
    {
        RtrFilter = FLEXCAN_IP_RX_FIFO_ACCEPT_REMOTE_FRAME << FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_STD_RTR2_SHIFT;
    }

    if (pFilterStdIDElement->rtr1)
    {
        RtrFilter |= FLEXCAN_IP_RX_FIFO_ACCEPT_REMOTE_FRAME << FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_STD_RTR1_SHIFT;
    }

    *pFilterStdIDTable = ((pFilterStdIDElement->id2 & FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_STD_MASK) << FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_STD_SHIFT2) |
                         ((pFilterStdIDElement->id1 & FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_STD_MASK) << FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_STD_SHIFT1) | RtrFilter;

    switch (pFilterStdIDElement->filterType)
    {
        case FLEXCAN_IP_ENHANCED_RX_FIFO_ONE_ID_FILTER :
        {
            *pFilterStdIDTable |= (uint32)FLEXCAN_IP_ENHANCED_RX_FIFO_ONE_ID_FILTER << FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_FSCH_SHIFT;
            break;
        }
        case FLEXCAN_IP_ENHANCED_RX_FIFO_RANGE_ID_FILTER :
        {
            *pFilterStdIDTable |= (uint32)FLEXCAN_IP_ENHANCED_RX_FIFO_RANGE_ID_FILTER << FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_FSCH_SHIFT;
            break;
        }
        case FLEXCAN_IP_ENHANCED_RX_FIFO_TWO_ID_FILTER :
        {
            *pFilterStdIDTable |= (uint32)FLEXCAN_IP_ENHANCED_RX_FIFO_TWO_ID_FILTER << FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_FSCH_SHIFT;
            break;
        }
        default:
        {
    #if (FLEXCAN_IP_DEV_ERROR_DETECT == STD_ON)
            DevAssert(FALSE);
            /* Should not get here */
    #endif
            break;
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_SetEnhancedRxFifoFilterExtendedFormat
 * Description   : Configure Enhanced RX FIFO ID filter extended format element.
 *
 *END**************************************************************************/
static void FlexCAN_SetEnhancedRxFifoFilterExtendedFormat(volatile uint32 * pFilterExtIDTable, const Flexcan_Ip_EnhancedIdTableType * pFilterExtIDElement)
{
    uint32 RtrFilter1 = 0U;
    uint32 RtrFilter2 = 0U;

    if (pFilterExtIDElement->rtr2)
    {
        RtrFilter2 = FLEXCAN_IP_RX_FIFO_ACCEPT_REMOTE_FRAME << FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_EXT_RTR_SHIFT;
    }
    if (pFilterExtIDElement->rtr1)
    {
        RtrFilter1 = FLEXCAN_IP_RX_FIFO_ACCEPT_REMOTE_FRAME << FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_EXT_RTR_SHIFT;
    }
    *pFilterExtIDTable    = ((pFilterExtIDElement->id2 & FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_EXT_MASK) << FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_EXT_SHIFT) | RtrFilter2;
    pFilterExtIDTable[1U] = ((pFilterExtIDElement->id1 & FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_EXT_MASK) << FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_EXT_SHIFT) | RtrFilter1;

    switch (pFilterExtIDElement->filterType)
    {
        case FLEXCAN_IP_ENHANCED_RX_FIFO_ONE_ID_FILTER :
        {
            *pFilterExtIDTable |= (uint32)FLEXCAN_IP_ENHANCED_RX_FIFO_ONE_ID_FILTER << FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_FSCH_SHIFT;
            break;
        }
        case FLEXCAN_IP_ENHANCED_RX_FIFO_RANGE_ID_FILTER :
        {
            *pFilterExtIDTable |= (uint32)FLEXCAN_IP_ENHANCED_RX_FIFO_RANGE_ID_FILTER << FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_FSCH_SHIFT;
            break;
        }
        case FLEXCAN_IP_ENHANCED_RX_FIFO_TWO_ID_FILTER :
        {
            *pFilterExtIDTable |= (uint32)FLEXCAN_IP_ENHANCED_RX_FIFO_TWO_ID_FILTER << FLEXCAN_IP_ENHANCED_RX_FIFO_ID_FILTER_FSCH_SHIFT;
            break;
        }
        default :
        {
    #if (FLEXCAN_IP_DEV_ERROR_DETECT == STD_ON)
            DevAssert(FALSE);
            /* Should not get here */
    #endif
            break;
        }
    }
}
#endif /* (FLEXCAN_IP_FEATURE_HAS_ENHANCED_RX_FIFO == STD_ON) */

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_ComputePayloadSize
 * Description   : Computes the maximum payload size (in bytes), given a DLC
 * field value.
 *
 *END**************************************************************************/
#if (FLEXCAN_IP_FEATURE_HAS_ENHANCED_RX_FIFO == STD_ON)
uint8 FlexCAN_ComputePayloadSize(uint8 DlcValue)
#else
inline static uint8 FlexCAN_ComputePayloadSize(uint8 DlcValue)
#endif /* (FLEXCAN_IP_FEATURE_HAS_ENHANCED_RX_FIFO == STD_ON) */
{
    uint8 Ret = 8U;

    if (DlcValue <= 8U)
    {
        Ret = DlcValue;
    }
#if (FLEXCAN_IP_FEATURE_HAS_FD == STD_ON)
    else
    {
        switch (DlcValue)
        {
            case FLEXCAN_IP_DLC_VALUE_12_BYTES:
                Ret = 12U;
                break;
            case FLEXCAN_IP_DLC_VALUE_16_BYTES:
                Ret = 16U;
                break;
            case FLEXCAN_IP_DLC_VALUE_20_BYTES:
                Ret = 20U;
                break;
            case FLEXCAN_IP_DLC_VALUE_24_BYTES:
                Ret = 24U;
                break;
            case FLEXCAN_IP_DLC_VALUE_32_BYTES:
                Ret = 32U;
                break;
            case FLEXCAN_IP_DLC_VALUE_48_BYTES:
                Ret = 48U;
                break;
            case FLEXCAN_IP_DLC_VALUE_64_BYTES:
                Ret = 64U;
                break;
            default:
                /* The argument is not a valid DLC size */
                break;
        }
    }
#endif /* FLEXCAN_IP_FEATURE_HAS_FD */

    return Ret;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_GetMsgBuffRegion
 * Description   : Returns the start of a MB area, based on its index.
 *
 *END**************************************************************************/
volatile uint32 * FlexCAN_GetMsgBuffRegion(FLEXCAN_Type * pBase, uint32 MsgBuffIdx)
{
    uint8 Idx = 0U;
    uint8 PayloadSize = 0U;
    uint8 ArbitrationFieldSize = 8U;
    uint16 MbSize = 0U;
    uint16 RamBlockOffset = 0;
    uint32 RamBlockSize = 512U;
    uint32 SubMsgBuffIdx = MsgBuffIdx;
    uint32 MaxMbNum = 0U;
    uint32 RamIdx = 0U;
    volatile uint32 * RAM = &(((volatile uint32 *)pBase)[FLEXCAN_IP_FEATURE_RAM_OFFSET >> 2U]);
    volatile uint32 * pAddressRet = NULL_PTR;

    for (Idx = 0; Idx < (uint8)FLEXCAN_IP_FEATURE_MBDSR_COUNT; Idx++)
    {
        PayloadSize = FlexCAN_GetPayloadSize(pBase, Idx);
        MbSize = ((uint16)PayloadSize + (uint16)ArbitrationFieldSize);
        MaxMbNum = (RamBlockSize / (uint32)MbSize);
        if (MaxMbNum > SubMsgBuffIdx)
        {
            break;
        }
        else if (RamBlockOffset < ((uint16)UINT16_MAX - 128U))
        {
            RamBlockOffset += 128U;
            SubMsgBuffIdx -= MaxMbNum;
        }
        else
        {
            /* Do nothing. Don't expect to reach here */
        }
    }
    {
        /* Multiply the MB index by the MB size (in words) */
        RamIdx = (uint32)RamBlockOffset + (uint32)(((uint64)SubMsgBuffIdx * ((uint64)MbSize >> 2U)) & (uint64)UINT16_MAX);
        pAddressRet = &(RAM[RamIdx]);
    }

    return pAddressRet;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_GetMaxMbNum
 * Description   : Computes the maximum RAM size occupied by MBs.
 *
 *END**************************************************************************/
uint16 FlexCAN_GetMaxMbNum(const FLEXCAN_Type * pBase)
{
    uint8 Idx;
    uint16 Ret = 0U;
    static FLEXCAN_Type * const FlexcanBase[] = FLEXCAN_IP_BASE_PTRS;
    static const uint16 MaxMbNum[] = FLEXCAN_IP_FEATURE_MAX_MB_NUM_ARRAY;

    for (Idx = 0u; Idx < FLEXCAN_IP_INSTANCE_COUNT; Idx++)
    {
        if (pBase == FlexcanBase[Idx])
        {
            Ret = MaxMbNum[Idx];
        }
    }
    return Ret;
}



/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_EnterFreezeMode
 * Description   : Enter the freeze mode.
 *
 *END**************************************************************************/
Flexcan_Ip_StatusType FlexCAN_EnterFreezeMode(FLEXCAN_Type * pBase)
{
    uint32 TimeStart = 0U;
    uint32 TimeElapsed = 0U;
    uint32 Us2Ticks = OsIf_MicrosToTicks(FLEXCAN_IP_TIMEOUT_DURATION, FLEXCAN_IP_SERVICE_TIMEOUT_TYPE);
    Flexcan_Ip_StatusType ReturnResult = FLEXCAN_STATUS_SUCCESS;

    /* Start critical section: implementation depends on integrator */
    SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_02();
    pBase->MCR = (pBase->MCR & ~FLEXCAN_MCR_FRZ_MASK) | FLEXCAN_MCR_FRZ(1U);
    pBase->MCR = (pBase->MCR & ~FLEXCAN_MCR_HALT_MASK) | FLEXCAN_MCR_HALT(1U);
    if (((pBase->MCR & FLEXCAN_MCR_MDIS_MASK) >> FLEXCAN_MCR_MDIS_SHIFT) != 0U)
    {
        pBase->MCR &= ~FLEXCAN_MCR_MDIS_MASK;
    }
    /* End critical section: implementation depends on integrator */
    SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_02();
    /* Wait for entering the freeze mode */
    TimeStart = OsIf_GetCounter(FLEXCAN_IP_SERVICE_TIMEOUT_TYPE);
    while (0U == ((pBase->MCR & FLEXCAN_MCR_FRZACK_MASK) >> FLEXCAN_MCR_FRZACK_SHIFT))
    {
        TimeElapsed = OsIf_GetElapsed(&TimeStart, FLEXCAN_IP_SERVICE_TIMEOUT_TYPE);
        if (TimeElapsed >= Us2Ticks)
        {
            ReturnResult = FLEXCAN_STATUS_TIMEOUT;
            break;
        }
        else
        {
            /* Adjust remaining timeout */
            Us2Ticks -=  TimeElapsed;
        }
    }

    return ReturnResult;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_Enable
 * Description   : Enable the clock for FlexCAN Module.
 *
 *END**************************************************************************/
Flexcan_Ip_StatusType FlexCAN_Enable(FLEXCAN_Type * pBase)
{
    uint32 TimeStart = 0U;
    uint32 TimeElapsed = 0U;
    uint32 Us2Ticks = OsIf_MicrosToTicks(FLEXCAN_IP_TIMEOUT_DURATION, FLEXCAN_IP_SERVICE_TIMEOUT_TYPE);
    Flexcan_Ip_StatusType ReturnValue = FLEXCAN_STATUS_SUCCESS;

    /* Start critical section: implementation depends on integrator */
    SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_03();

    /* Enable Module */
    pBase->MCR &= ~FLEXCAN_MCR_MDIS_MASK;
    /* End critical section: implementation depends on integrator */
    SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_03();
    /* Wait for entering the freeze mode */
    TimeStart = OsIf_GetCounter(FLEXCAN_IP_SERVICE_TIMEOUT_TYPE);
    while (0U == ((pBase->MCR & FLEXCAN_MCR_FRZACK_MASK) >> FLEXCAN_MCR_FRZACK_SHIFT))
    {
        TimeElapsed = OsIf_GetElapsed(&TimeStart, FLEXCAN_IP_SERVICE_TIMEOUT_TYPE);
        if (TimeElapsed >= Us2Ticks)
        {
            ReturnValue = FLEXCAN_STATUS_TIMEOUT;
            break;
        }
        else
        {
            /* Adjust remaining timeout */
            Us2Ticks -=  TimeElapsed;
        }
    }
    return ReturnValue;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_ExitFreezeMode
 * Description   : Exit of freeze mode.
 *
 *END**************************************************************************/
Flexcan_Ip_StatusType FlexCAN_ExitFreezeMode(FLEXCAN_Type * pBase)
{
    uint32 TimeStart = 0U;
    uint32 TimeElapsed = 0U;
    uint32 Us2Ticks = OsIf_MicrosToTicks(FLEXCAN_IP_TIMEOUT_DURATION, FLEXCAN_IP_SERVICE_TIMEOUT_TYPE);
    Flexcan_Ip_StatusType ReturnValue = FLEXCAN_STATUS_SUCCESS;

    /* Start critical section: implementation depends on integrator */
    SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_04();
    pBase->MCR = (pBase->MCR & ~FLEXCAN_MCR_HALT_MASK) | FLEXCAN_MCR_HALT(0U);
    pBase->MCR = (pBase->MCR & ~FLEXCAN_MCR_FRZ_MASK) | FLEXCAN_MCR_FRZ(0U);
    /* End critical section: implementation depends on integrator */
    SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_04();
    /* Wait till exit freeze mode */
    TimeStart = OsIf_GetCounter(FLEXCAN_IP_SERVICE_TIMEOUT_TYPE);
    while (((pBase->MCR & FLEXCAN_MCR_FRZACK_MASK) >> FLEXCAN_MCR_FRZACK_SHIFT) != 0U)
    {
        TimeElapsed = OsIf_GetElapsed(&TimeStart, FLEXCAN_IP_SERVICE_TIMEOUT_TYPE);
        if (TimeElapsed >= Us2Ticks)
        {
            ReturnValue = FLEXCAN_STATUS_TIMEOUT;
            break;
        }
        else
        {
            /* Adjust remaining timeout */
            Us2Ticks -=  TimeElapsed;
        }
    }
    return ReturnValue;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_Disable
 * Description   : Disable FlexCAN module.
 * This function will disable FlexCAN module.
 *
 *END**************************************************************************/
Flexcan_Ip_StatusType FlexCAN_Disable(FLEXCAN_Type * pBase)
{
    uint32 TimeStart = 0U;
    uint32 TimeElapsed = 0U;
    uint32 Us2Ticks = OsIf_MicrosToTicks(FLEXCAN_IP_TIMEOUT_DURATION, FLEXCAN_IP_SERVICE_TIMEOUT_TYPE);
    Flexcan_Ip_StatusType ReturnResult = FLEXCAN_STATUS_SUCCESS;

    /* To access the memory mapped registers */
    /* Enter disable mode (hard reset). */
    if (0U == ((pBase->MCR & FLEXCAN_MCR_MDIS_MASK) >> FLEXCAN_MCR_MDIS_SHIFT))
    {
        /* Start critical section: implementation depends on integrator */
        SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_05();
        /* Clock disable (module) */
        pBase->MCR = (pBase->MCR & ~FLEXCAN_MCR_MDIS_MASK) | FLEXCAN_MCR_MDIS(1U);
        /* End critical section: implementation depends on integrator */
        SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_05();
        /* Wait until disable mode acknowledged */
        TimeStart = OsIf_GetCounter(FLEXCAN_IP_SERVICE_TIMEOUT_TYPE);
        while (0U == ((pBase->MCR & FLEXCAN_MCR_LPMACK_MASK) >> FLEXCAN_MCR_LPMACK_SHIFT))
        {
            TimeElapsed = OsIf_GetElapsed(&TimeStart, FLEXCAN_IP_SERVICE_TIMEOUT_TYPE);
            if (TimeElapsed >= Us2Ticks)
            {
                ReturnResult = FLEXCAN_STATUS_TIMEOUT;
                break;
            }
            else
            {
                /* Adjust remaining timeout */
                Us2Ticks -=  TimeElapsed;
            }
        }
    }
    return ReturnResult;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_SetErrIntCmd
 * Description   : Enable the error interrupts.
 * This function will enable Error interrupt.
 *
 *END**************************************************************************/
void FlexCAN_SetErrIntCmd(FLEXCAN_Type * pBase, Flexcan_Ip_ErrorIntMaskType ErrType, boolean Enable)
{
    uint32 Temp = (uint32)ErrType;

    /* Start critical section: implementation depends on integrator */
    SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_06();
    if (Enable)
    {
#if (FLEXCAN_IP_FEATURE_HAS_FD == STD_ON)
        if (FLEXCAN_INT_ERR_FAST == ErrType)
        {
            pBase->CTRL2 = (pBase->CTRL2 & ~FLEXCAN_CTRL2_ERRMSK_FAST_MASK) | FLEXCAN_CTRL2_ERRMSK_FAST(1U);
            (void)Temp;
        }
        else
#endif
        {
            if ((FLEXCAN_INT_RX_WARNING == ErrType) || (FLEXCAN_INT_TX_WARNING == ErrType))
            {
                pBase->MCR = (pBase->MCR & ~FLEXCAN_MCR_WRNEN_MASK) | FLEXCAN_MCR_WRNEN(1U);
            }
            (pBase->CTRL1) = ((pBase->CTRL1) | (Temp));
        }
    }
    else
    {
#if (FLEXCAN_IP_FEATURE_HAS_FD == STD_ON)
        if (FLEXCAN_INT_ERR_FAST == ErrType)
        {
            pBase->CTRL2 = (pBase->CTRL2 & ~FLEXCAN_CTRL2_ERRMSK_FAST_MASK) | FLEXCAN_CTRL2_ERRMSK_FAST(0U);
            (void)Temp;
        }
        else
#endif
        {
            (pBase->CTRL1) = ((pBase->CTRL1) & ~(Temp));
            Temp = pBase->CTRL1;
            if ((0U == (Temp & (uint32)FLEXCAN_INT_RX_WARNING)) && (0U == (Temp & (uint32)FLEXCAN_INT_TX_WARNING)))
            {
                /* If WRNEN disabled then both FLEXCAN_INT_RX_WARNING and FLEXCAN_INT_TX_WARNING will be disabled */
                pBase->MCR = (pBase->MCR & ~FLEXCAN_MCR_WRNEN_MASK) | FLEXCAN_MCR_WRNEN(0U);
            }
        }
    }
    /* End critical section: implementation depends on integrator */
    SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_06();
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_Init
 * Description   : Initialize FlexCAN module.
 * This function will reset FlexCAN module, initialize all message buffers as inactive,
 * mask all mask bits, and disable all MB interrupts.
 *
 *END**************************************************************************/
Flexcan_Ip_StatusType FlexCAN_Init(FLEXCAN_Type * pBase)
{
    uint32 TimeStart = 0U;
    uint32 TimeElapsed = 0U;
    uint32 Us2Ticks = OsIf_MicrosToTicks(FLEXCAN_IP_TIMEOUT_DURATION, FLEXCAN_IP_SERVICE_TIMEOUT_TYPE);
    Flexcan_Ip_StatusType ReturnResult = FLEXCAN_STATUS_SUCCESS;

    /* Reset the FLEXCAN */
    pBase->MCR = (pBase->MCR & ~FLEXCAN_MCR_SOFTRST_MASK) | FLEXCAN_MCR_SOFTRST(1U);
    /* Wait for reset cycle to complete */
    TimeStart = OsIf_GetCounter(FLEXCAN_IP_SERVICE_TIMEOUT_TYPE);
    while (((pBase->MCR & FLEXCAN_MCR_SOFTRST_MASK) >> FLEXCAN_MCR_SOFTRST_SHIFT) != 0U)
    {
        TimeElapsed = OsIf_GetElapsed(&TimeStart, FLEXCAN_IP_SERVICE_TIMEOUT_TYPE);
        if (TimeElapsed >= Us2Ticks)
        {
            ReturnResult = FLEXCAN_STATUS_TIMEOUT;
            break;
        }
        else
        {
            /* Adjust remaining timeout */
            Us2Ticks -=  TimeElapsed;
        }
    }
    if (FLEXCAN_STATUS_SUCCESS == ReturnResult)
    {
        /* Avoid Abort Transmission, use Inactive MB */
        pBase->MCR = (pBase->MCR & ~FLEXCAN_MCR_AEN_MASK) | FLEXCAN_MCR_AEN(1U);
        /* Clear FlexCAN memory */
        FlexCAN_ClearRAM(pBase);
        /* Rx global mask*/
        (pBase->RXMGMASK) = (uint32)(FLEXCAN_RXMGMASK_MG_MASK);
        /* Rx reg 14 mask*/
        (pBase->RX14MASK) =  (uint32)(FLEXCAN_RX14MASK_RX14M_MASK);
        /* Rx reg 15 mask*/
        (pBase->RX15MASK) = (uint32)(FLEXCAN_RX15MASK_RX15M_MASK);
        /* Disable all MB interrupts */
        (pBase->IMASK1) = 0x0;
        /* Clear all MB interrupt flags */
        (pBase->IFLAG1) = FLEXCAN_IMASK1_BUF31TO0M_MASK;
#if FLEXCAN_IP_FEATURE_MAX_MB_NUM > 32U
        if (FlexCAN_GetMaxMbNum(pBase) > 32U)
        {
            (pBase->IMASK2) = 0x0;
            (pBase->IFLAG2) = FLEXCAN_IMASK2_BUF63TO32M_MASK;
        }
#endif
#if FLEXCAN_IP_FEATURE_MAX_MB_NUM > 64U
        if (FlexCAN_GetMaxMbNum(pBase) > 64U)
        {
            (pBase->IMASK3) = 0x0;
            (pBase->IFLAG3) = FLEXCAN_IMASK3_BUF95TO64M_MASK;
        }
#endif
#if FLEXCAN_IP_FEATURE_MAX_MB_NUM > 96U
        if (FlexCAN_GetMaxMbNum(pBase) > 96U)
        {
            (pBase->IMASK4) = 0x0;
            (pBase->IFLAG4) = FLEXCAN_IMASK4_BUF127TO96M_MASK;
        }
#endif
        /* Clear all error interrupt flags */
        (pBase->ESR1) = FLEXCAN_IP_ALL_INT;
        /* clear registers which are not effected by soft reset */
        pBase->CTRL1 = FLEXCAN_IP_CTRL1_DEFAULT_VALUE_U32;
        pBase->CTRL2 = FLEXCAN_IP_CTRL2_DEFAULT_VALUE_U32;
        pBase->CBT   = FLEXCAN_IP_CBT_DEFAULT_VALUE_U32;

#if (FLEXCAN_IP_FEATURE_HAS_ENHANCE_CBT == STD_ON)
        pBase->EPRS  = FLEXCAN_IP_EPRS_DEFAULT_VALUE_U32;
        pBase->ENCBT = FLEXCAN_IP_ENCBT_DEFAULT_VALUE_U32;
#endif
#if (FLEXCAN_IP_FEATURE_HAS_FD == STD_ON)
    {
#if (FLEXCAN_IP_FEATURE_HAS_ENHANCE_CBT == STD_ON)
        pBase->EDCBT = FLEXCAN_IP_EDCBT_DEFAULT_VALUE_U32;
        pBase->ETDC  = FLEXCAN_IP_ETDC_DEFAULT_VALUE_U32;
#endif
        pBase->FDCBT = FLEXCAN_IP_FDCBT_DEFAULT_VALUE_U32;
        pBase->FDCTRL = FLEXCAN_IP_FDCTRL_DEFAULT_VALUE_U32;
    }
#endif /* (FLEXCAN_IP_FEATURE_HAS_FD == STD_ON) */
#ifdef FLEXCAN_IP_FEATURE_HAS_FLTCONF_INT
    pBase->FLTCONF_IE = FLEXCAN_IP_FLTCONF_IE_DEFAULT_VALUE_U32;
#endif
    }
    return ReturnResult;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_EnableRxFifo
 * Description   : Enable Rx FIFO feature.
 * This function will enable the Rx FIFO feature.
 *
 *END**************************************************************************/
Flexcan_Ip_StatusType FlexCAN_EnableRxFifo(FLEXCAN_Type * pBase,
                                         uint32 NumOfFilters)
{
    uint16 Idx;
    uint16 NoOfMbx = FlexCAN_GetMaxMbNum(pBase);
    Flexcan_Ip_StatusType Status = FLEXCAN_STATUS_ERROR;

#if (FLEXCAN_IP_FEATURE_HAS_FD == STD_ON)
    boolean IsFDEnabled = FlexCAN_IsFDEnabled(pBase);
    /* RxFIFO cannot be enabled if FD is enabled */
    if (FALSE == IsFDEnabled)
#endif
    {
        /* Enable RX FIFO */
        pBase->MCR = (pBase->MCR & ~FLEXCAN_MCR_RFEN_MASK) | FLEXCAN_MCR_RFEN(1U);
        /* Set the number of the RX FIFO filters needed */
        pBase->CTRL2 = (pBase->CTRL2 & ~FLEXCAN_CTRL2_RFFN_MASK) | ((NumOfFilters << FLEXCAN_CTRL2_RFFN_SHIFT) & FLEXCAN_CTRL2_RFFN_MASK);
        /* RX FIFO global mask, take in consideration all filter fields*/
        (pBase->RXFGMASK) = FLEXCAN_RXFGMASK_FGM_MASK;

        for (Idx = 0U; Idx < NoOfMbx; Idx++)
        {
            /* RX individual mask */
            pBase->RXIMR[Idx] = (FLEXCAN_RXIMR_MI_MASK << FLEXCAN_IP_ID_EXT_SHIFT) & (FLEXCAN_IP_ID_STD_MASK | FLEXCAN_IP_ID_EXT_MASK);
        }
        Status = FLEXCAN_STATUS_SUCCESS;
    }

    return Status;
}


#if (FLEXCAN_IP_FEATURE_HAS_FD == STD_ON)
/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_SetPayloadSize
 * Description   : Sets the payload size of the MBs.
 *
 *END**************************************************************************/
void FlexCAN_SetPayloadSize(FLEXCAN_Type * pBase,
                            const Flexcan_Ip_PayloadSizeType * PayloadSize)
{
#if (FLEXCAN_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert((boolean)(FlexCAN_IsFDEnabled(pBase) || (FLEXCAN_PAYLOAD_SIZE_8 == PayloadSize->payloadBlock0)));
    #if (FLEXCAN_IP_FEATURE_MBDSR_COUNT > 1U)
    DevAssert((boolean)(FlexCAN_IsFDEnabled(pBase) || (FLEXCAN_PAYLOAD_SIZE_8 == PayloadSize->payloadBlock1)));
    #endif
    #if (FLEXCAN_IP_FEATURE_MBDSR_COUNT > 2U)
    DevAssert((boolean)(FlexCAN_IsFDEnabled(pBase) || (FLEXCAN_PAYLOAD_SIZE_8 == PayloadSize->payloadBlock2)));
    #endif
    #if (FLEXCAN_IP_FEATURE_MBDSR_COUNT > 3U)
    DevAssert((boolean)(FlexCAN_IsFDEnabled(pBase) || (FLEXCAN_PAYLOAD_SIZE_8 == PayloadSize->payloadBlock3)));
    #endif
#endif
    uint32 Temp;

    /* If FD is not enabled, only 8 bytes payload is supported */
    if (FlexCAN_IsFDEnabled(pBase))
    {
        Temp = pBase->FDCTRL;
        Temp &= ~(FLEXCAN_FDCTRL_MBDSR0_MASK);
        Temp |= ((uint32)PayloadSize->payloadBlock0) << FLEXCAN_FDCTRL_MBDSR0_SHIFT;
#if (FLEXCAN_IP_FEATURE_MBDSR_COUNT > 1U)
        Temp &= ~(FLEXCAN_FDCTRL_MBDSR1_MASK);
        Temp |= ((uint32)PayloadSize->payloadBlock1) << FLEXCAN_FDCTRL_MBDSR1_SHIFT;
#endif
#if (FLEXCAN_IP_FEATURE_MBDSR_COUNT > 2U)
        Temp &= ~(FLEXCAN_FDCTRL_MBDSR2_MASK);
        Temp |= ((uint32)PayloadSize->payloadBlock2) << FLEXCAN_FDCTRL_MBDSR2_SHIFT;
#endif
#if (FLEXCAN_IP_FEATURE_MBDSR_COUNT > 3U)
        Temp &= ~(FLEXCAN_FDCTRL_MBDSR3_MASK);
        Temp |= ((uint32)PayloadSize->payloadBlock3) << FLEXCAN_FDCTRL_MBDSR3_SHIFT;
#endif
        pBase->FDCTRL = Temp;
    }
}

#endif /* End FLEXCAN_IP_FEATURE_HAS_FD */

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_GetPayloadSize
 * Description   : Returns the payload size of the MBs (in bytes).
 *
 *END**************************************************************************/
static uint8 FlexCAN_GetPayloadSize(const FLEXCAN_Type * pBase, uint8 MbdsrIdx)
{
    uint32 PayloadSize;

#if (FLEXCAN_IP_FEATURE_HAS_FD == STD_ON)
    {
        switch (MbdsrIdx)
        {
            case 0 : {  PayloadSize = 8UL << ((pBase->FDCTRL & FLEXCAN_FDCTRL_MBDSR0_MASK) >> FLEXCAN_FDCTRL_MBDSR0_SHIFT); } break;
        #if (FLEXCAN_IP_FEATURE_MBDSR_COUNT > 1U)
            case 1 : {  PayloadSize = 8UL << ((pBase->FDCTRL & FLEXCAN_FDCTRL_MBDSR1_MASK) >> FLEXCAN_FDCTRL_MBDSR1_SHIFT); } break;
        #endif
        #if (FLEXCAN_IP_FEATURE_MBDSR_COUNT > 2U)
            case 2 : {  PayloadSize = 8UL << ((pBase->FDCTRL & FLEXCAN_FDCTRL_MBDSR2_MASK) >> FLEXCAN_FDCTRL_MBDSR2_SHIFT); } break;
        #endif
        #if (FLEXCAN_IP_FEATURE_MBDSR_COUNT > 3U)
            case 3 : {  PayloadSize = 8UL << ((pBase->FDCTRL & FLEXCAN_FDCTRL_MBDSR3_MASK) >> FLEXCAN_FDCTRL_MBDSR3_SHIFT); } break;
        #endif
            default :{  PayloadSize = 8UL << ((pBase->FDCTRL & FLEXCAN_FDCTRL_MBDSR0_MASK) >> FLEXCAN_FDCTRL_MBDSR0_SHIFT); } break;
        }
    }
#else
    PayloadSize = 8U;
#endif /* FLEXCAN_IP_FEATURE_HAS_FD */

    return (uint8)(PayloadSize & (uint32)UINT8_MAX);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_GetMbPayloadSize
 * Description   : Returns the actual payload size of the MBs (in bytes).
 *
 *END**************************************************************************/
uint8 FlexCAN_GetMbPayloadSize(const FLEXCAN_Type * pBase, uint32 MsgBuffIdx)
{
    uint8 Idx=0U;
    uint8 FlexcanRealPayload = 8U;
    uint8 ArbitrationFieldSize = 8U;
    uint16 MbSize = 0U;
    uint32 RamBlockSize = 512U;
    uint32 MaxMbNum = 0U;
    uint32 MaxMbBlockNum = 0U;

    for (Idx = 0; Idx < (uint8)FLEXCAN_IP_FEATURE_MBDSR_COUNT; Idx++)
    {
        FlexcanRealPayload = FlexCAN_GetPayloadSize(pBase, Idx);
        MbSize = ((uint16)FlexcanRealPayload + (uint16)ArbitrationFieldSize);
        MaxMbNum = (RamBlockSize / (uint32)MbSize);
        if (MaxMbBlockNum < ((uint32)UINT32_MAX - MaxMbNum))
        {
            MaxMbBlockNum += MaxMbNum;
            if (MaxMbBlockNum > MsgBuffIdx)
            {
                break;
            }
        }
    }

    return FlexcanRealPayload;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_LockRxMsgBuff
 * Description   : Lock the RX message buffer.
 * This function will lock the RX message buffer.
 *
 *END**************************************************************************/
void FlexCAN_LockRxMsgBuff(FLEXCAN_Type * pBase, uint32 MsgBuffIdx)
{
    volatile const uint32 * FlexcanMb = FlexCAN_GetMsgBuffRegion(pBase, MsgBuffIdx);

    /* Lock the mailbox by reading it */
    (void)*FlexcanMb;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_SetMsgBuffIntCmd
 * Description   : Enable/Disable the corresponding Message Buffer interrupt.
 *
 *END**************************************************************************/
Flexcan_Ip_StatusType FlexCAN_SetMsgBuffIntCmd(FLEXCAN_Type * pBase,
                                               uint8 u8Instance,
                                               uint32 MsgBuffIdx,
                                               boolean Enable,
                                               boolean bIsIntActive
                                              )
{
    uint32 Temp;
    Flexcan_Ip_StatusType Stat = FLEXCAN_STATUS_SUCCESS;

        /* Enable the corresponding message buffer Interrupt */
        Temp = 1UL << (MsgBuffIdx % 32U);
        if (MsgBuffIdx < 32U)
        {
            if (Enable)
            {
                /* Start critical section: implementation depends on integrator */
                SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_18();
                FlexCAN_Ip_au32ImaskBuff[u8Instance][0U] = ((FlexCAN_Ip_au32ImaskBuff[u8Instance][0U]) | (Temp));
                if (TRUE == bIsIntActive)
                {
                    pBase->IMASK1 = FlexCAN_Ip_au32ImaskBuff[u8Instance][0U];
                }
                /* End critical section: implementation depends on integrator */
                SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_18();
            }
            else
            {
                /* Start critical section: implementation depends on integrator */
                SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_18();
                FlexCAN_Ip_au32ImaskBuff[u8Instance][0U] = ((FlexCAN_Ip_au32ImaskBuff[u8Instance][0U]) & ~(Temp));
                pBase->IMASK1 = FlexCAN_Ip_au32ImaskBuff[u8Instance][0U];
                /* End critical section: implementation depends on integrator */
                SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_18();
            }
        }
#if FLEXCAN_IP_FEATURE_MAX_MB_NUM > 32U
        if ((MsgBuffIdx >= 32U) && (MsgBuffIdx < 64U))
        {
            if (Enable)
            {
                /* Start critical section: implementation depends on integrator */
                SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_18();
                FlexCAN_Ip_au32ImaskBuff[u8Instance][1U] = ((FlexCAN_Ip_au32ImaskBuff[u8Instance][1U]) | (Temp));
                if (TRUE == bIsIntActive)
                {
                    pBase->IMASK2 = FlexCAN_Ip_au32ImaskBuff[u8Instance][1U];
                }
                /* End critical section: implementation depends on integrator */
                SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_18();
            }
            else
            {
                /* Start critical section: implementation depends on integrator */
                SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_18();
                FlexCAN_Ip_au32ImaskBuff[u8Instance][1U] = ((FlexCAN_Ip_au32ImaskBuff[u8Instance][1U]) & ~(Temp));
                pBase->IMASK2 = FlexCAN_Ip_au32ImaskBuff[u8Instance][1U];
                /* End critical section: implementation depends on integrator */
                SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_18();
            }
        }
#endif /* if FLEXCAN_IP_FEATURE_MAX_MB_NUM > 32U */
#if FLEXCAN_IP_FEATURE_MAX_MB_NUM > 64U
        if ((MsgBuffIdx >= 64U) && (MsgBuffIdx < 96U))
        {
            if (Enable)
            {
                /* Start critical section: implementation depends on integrator */
                SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_18();
                FlexCAN_Ip_au32ImaskBuff[u8Instance][2U] = ((FlexCAN_Ip_au32ImaskBuff[u8Instance][2U]) | (Temp));
                if (TRUE == bIsIntActive)
                {
                    pBase->IMASK3 = FlexCAN_Ip_au32ImaskBuff[u8Instance][2U];
                }
                /* End critical section: implementation depends on integrator */
                SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_18();
            }
            else
            {
                /* Start critical section: implementation depends on integrator */
                SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_18();
                FlexCAN_Ip_au32ImaskBuff[u8Instance][2U] = ((FlexCAN_Ip_au32ImaskBuff[u8Instance][2U]) & ~(Temp));
                pBase->IMASK3 = FlexCAN_Ip_au32ImaskBuff[u8Instance][2U];
                /* End critical section: implementation depends on integrator */
                SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_18();
            }
        }
#endif /* if FLEXCAN_IP_FEATURE_MAX_MB_NUM > 64U */
#if FLEXCAN_IP_FEATURE_MAX_MB_NUM > 96U
        if (MsgBuffIdx >= 96U)
        {
            if (Enable)
            {
                /* Start critical section: implementation depends on integrator */
                SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_18();
                FlexCAN_Ip_au32ImaskBuff[u8Instance][3U] = ((FlexCAN_Ip_au32ImaskBuff[u8Instance][3U]) | (Temp));
                if (TRUE == bIsIntActive)
                {
                    pBase->IMASK4 = FlexCAN_Ip_au32ImaskBuff[u8Instance][3U];
                }
                /* End critical section: implementation depends on integrator */
                SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_18();
            }
            else
            {
                /* Start critical section: implementation depends on integrator */
                SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_18();
                FlexCAN_Ip_au32ImaskBuff[u8Instance][3U] = ((FlexCAN_Ip_au32ImaskBuff[u8Instance][3U]) & ~(Temp));
                pBase->IMASK4 = FlexCAN_Ip_au32ImaskBuff[u8Instance][3U];
                /* End critical section: implementation depends on integrator */
                SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_18();
            }
        }
#endif /* if FLEXCAN_IP_FEATURE_MAX_MB_NUM > 96U */

    return Stat;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_ClearMsgBuffIntCmd
 * Description   : Disable the corresponding Message Buffer interrupt only if interrupts are active
 *
 *END**************************************************************************/
void FLEXCAN_ClearMsgBuffIntCmd(FLEXCAN_Type * pBase,
                                uint8 u8Instance,
                                uint32 MbIdx,
                                boolean bIsIntActive
                               )
{
    uint32 Temp = (1UL << (MbIdx % 32U));

 /* Stop the running transfer. */
    if (MbIdx < 32U)
    {
          /* Start critical section: implementation depends on integrator */
          SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_11();
          FlexCAN_Ip_au32ImaskBuff[u8Instance][0U] = (pBase->IMASK1 & (~Temp));
          if (TRUE == bIsIntActive)
          {
              pBase->IMASK1 = FlexCAN_Ip_au32ImaskBuff[u8Instance][0U];
          }
          /* End critical section: implementation depends on integrator */
          SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_11();
    }
#if FLEXCAN_IP_FEATURE_MAX_MB_NUM > 32U
    if ((MbIdx >= 32U) && (MbIdx < 64U))
    {
        /* Start critical section: implementation depends on integrator */
        SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_11();
        FlexCAN_Ip_au32ImaskBuff[u8Instance][1U] = (pBase->IMASK2 & (~Temp));
        if (TRUE == bIsIntActive)
        {
            pBase->IMASK2 = FlexCAN_Ip_au32ImaskBuff[u8Instance][1U];
        }
        /* End critical section: implementation depends on integrator */
        SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_11();
    }
#endif
#if FLEXCAN_IP_FEATURE_MAX_MB_NUM > 64U
    if ((MbIdx >= 64U) && (MbIdx < 96U))
    {
        /* Start critical section: implementation depends on integrator */
        SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_11();
        FlexCAN_Ip_au32ImaskBuff[u8Instance][2U] = (pBase->IMASK3 & (~Temp));
        if (TRUE == bIsIntActive)
        {
            pBase->IMASK3 = FlexCAN_Ip_au32ImaskBuff[u8Instance][2U];
        }
        /* End critical section: implementation depends on integrator */
        SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_11();
    }
#endif /* FLEXCAN_IP_FEATURE_MAX_MB_NUM > 64U */
#if FLEXCAN_IP_FEATURE_MAX_MB_NUM > 96U
    if (MbIdx >= 96U)
    {
        /* Start critical section: implementation depends on integrator */
        SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_11();
        FlexCAN_Ip_au32ImaskBuff[u8Instance][3U] = (pBase->IMASK4 & (~Temp));
        if (TRUE == bIsIntActive)
        {
            pBase->IMASK4 = FlexCAN_Ip_au32ImaskBuff[u8Instance][3U];
        }
        /* End critical section: implementation depends on integrator */
        SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_11();
    }
#endif /* #if FLEXCAN_IP_FEATURE_MAX_MB_NUM > 96U */
}


void FlexCAN_DisableInterrupts(FLEXCAN_Type * pBase)
{
    pBase->IMASK1 = 0U;
#if (FLEXCAN_IP_FEATURE_MAX_MB_NUM > 32U)
    if (FlexCAN_GetMaxMbNum(pBase) > 32U)
    {
        pBase->IMASK2 = 0U;
    }
#endif /* (FLEXCAN_IP_FEATURE_MAX_MB_NUM > 32U) */
#if (FLEXCAN_IP_FEATURE_MAX_MB_NUM > 64U)
    if (FlexCAN_GetMaxMbNum(pBase) > 64U)
    {
        pBase->IMASK3 = 0U;
    }
#endif /* (FLEXCAN_IP_FEATURE_MAX_MB_NUM > 64U) */
#if (FLEXCAN_IP_FEATURE_MAX_MB_NUM > 96U)
    if (FlexCAN_GetMaxMbNum(pBase) > 96U)
    {
        pBase->IMASK4 = 0U;
    }
#endif /* (FLEXCAN_IP_FEATURE_MAX_MB_NUM > 96U) */
}

void FlexCAN_EnableInterrupts(FLEXCAN_Type * pBase, uint8 u8Instance)
{
    pBase->IMASK1 = FlexCAN_Ip_au32ImaskBuff[u8Instance][0U];
#if (FLEXCAN_IP_FEATURE_MAX_MB_NUM > 32U)
    if (FlexCAN_GetMaxMbNum(pBase) > 32U)
    {
        pBase->IMASK2 = FlexCAN_Ip_au32ImaskBuff[u8Instance][1U];
    }
#endif /* (FLEXCAN_IP_FEATURE_MAX_MB_NUM > 32U) */
#if (FLEXCAN_IP_FEATURE_MAX_MB_NUM > 64U)
    if (FlexCAN_GetMaxMbNum(pBase) > 64U)
    {
        pBase->IMASK3 = FlexCAN_Ip_au32ImaskBuff[u8Instance][2U];
    }
#endif /* (FLEXCAN_IP_FEATURE_MAX_MB_NUM > 64U) */
#if (FLEXCAN_IP_FEATURE_MAX_MB_NUM > 96U)
    if (FlexCAN_GetMaxMbNum(pBase) > 96U)
    {
        pBase->IMASK4 = FlexCAN_Ip_au32ImaskBuff[u8Instance][3U];
    }
#endif /* (FLEXCAN_IP_FEATURE_MAX_MB_NUM > 96U) */
}
/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_SetRxMsgBuff
 * Description   : Configure a message buffer for receiving.
 * This function will first check if RX FIFO is enabled. If RX FIFO is enabled,
 * the function will make sure if the MB requested is not occupied by RX FIFO
 * and ID filter table. Then this function will configure the message buffer as
 * required for receiving.
 *
 *END**************************************************************************/
void FlexCAN_SetRxMsgBuff(FLEXCAN_Type * pBase,
                          uint32 MsgBuffIdx,
                          const Flexcan_Ip_MsbuffCodeStatusType * Cs,
                          uint32 MsgId
                         )
{
#if (FLEXCAN_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert((boolean)(Cs != NULL_PTR));
#endif
    volatile uint32 * FlexcanMb = FlexCAN_GetMsgBuffRegion(pBase, MsgBuffIdx);
    volatile uint32 * FlexcanMbId = &FlexcanMb[1];

    /* Clean up the arbitration field area */
    *FlexcanMb = 0;
    *FlexcanMbId = 0;

    /* Set the ID according the format structure */
    if (FLEXCAN_MSG_ID_EXT == Cs->msgIdType)
    {
        /* Set IDE */
        *FlexcanMb |= FLEXCAN_IP_CS_IDE_MASK;

        /* Clear SRR bit */
        *FlexcanMb &= ~FLEXCAN_IP_CS_SRR_MASK;

        /* ID [28-0] */
        *FlexcanMbId &= ~(FLEXCAN_IP_ID_STD_MASK | FLEXCAN_IP_ID_EXT_MASK);
        *FlexcanMbId |= (MsgId & (FLEXCAN_IP_ID_STD_MASK | FLEXCAN_IP_ID_EXT_MASK));
    }

    if (FLEXCAN_MSG_ID_STD == Cs->msgIdType)
    {
        /* Make sure IDE and SRR are not set */
        *FlexcanMb &= ~(FLEXCAN_IP_CS_IDE_MASK | FLEXCAN_IP_CS_SRR_MASK);

        /* ID[28-18] */
        *FlexcanMbId &= ~FLEXCAN_IP_ID_STD_MASK;
        *FlexcanMbId |= (MsgId << FLEXCAN_IP_ID_STD_SHIFT) & FLEXCAN_IP_ID_STD_MASK;
    }

    /* Set MB CODE */
    if ((uint32)FLEXCAN_RX_NOT_USED != Cs->code)
    {
        *FlexcanMb &= ~FLEXCAN_IP_CS_CODE_MASK;
        *FlexcanMb |= (Cs->code << FLEXCAN_IP_CS_CODE_SHIFT) & FLEXCAN_IP_CS_CODE_MASK;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_GetMsgBuffTimestamp
 * Description   : Get a message buffer timestamp value.
 *
 *END**************************************************************************/
uint32 FlexCAN_GetMsgBuffTimestamp(FLEXCAN_Type * pBase, uint32 MsgBuffIdx)
{
    uint32 TimeStamp = 0U;
    volatile const uint32 * FlexcanMb = FlexCAN_GetMsgBuffRegion(pBase, MsgBuffIdx);

#if (FLEXCAN_IP_FEATURE_HAS_HR_TIMER == STD_ON)
    if (FLEXCAN_IsHRTimeStampEnabled(pBase))
    {
        /* Extract the Time Stamp */
        TimeStamp = (uint32)pBase->HR_TIME_STAMP[MsgBuffIdx];
    }
    else
#endif /* EATURE_CAN_HAS_HR_TIMER */
    {
        TimeStamp = (uint32)((*FlexcanMb & FLEXCAN_IP_CS_TIME_STAMP_MASK) >> FLEXCAN_IP_CS_TIME_STAMP_SHIFT);
    }
    return TimeStamp;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_GetMsgBuff
 * Description   : Get a message buffer field values.
 * This function will first check if RX FIFO is enabled. If RX FIFO is enabled,
 * the function will make sure if the MB requested is not occupied by RX FIFO
 * and ID filter table. Then this function will get the message buffer field
 * values and copy the MB data field into user's buffer.
 *
 *END**************************************************************************/
void FlexCAN_GetMsgBuff(FLEXCAN_Type * pBase,
                        uint32 MsgBuffIdx,
                        Flexcan_Ip_MsgBuffType * MsgBuff
                       )
{
#if (FLEXCAN_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert((boolean)(MsgBuff != NULL_PTR));
#endif
    uint8 Idx;
    volatile const uint32 * FlexcanMb = FlexCAN_GetMsgBuffRegion(pBase, MsgBuffIdx);
    volatile const uint32 * FlexcanMbId   = &FlexcanMb[1];
    volatile const uint8 * FlexcanMbData = (volatile const uint8 *)(&FlexcanMb[2]);
    volatile const uint32 * FlexcanMbData32 = &FlexcanMb[2];
    uint32 * MsgBuffData32 = NULL_PTR;
    uint32 MbWord;

    uint8 FlexcanMbDlcValue = (uint8)(((*FlexcanMb) & FLEXCAN_IP_CS_DLC_MASK) >> 16);
    uint8 PayloadSize = FlexCAN_ComputePayloadSize(FlexcanMbDlcValue);

    /* Asign after NULL Check */
    MsgBuffData32 = (uint32 *)(MsgBuff->data);
#if (FLEXCAN_IP_FEATURE_HAS_FD == STD_ON)
    /* Payload Size is based on MBDSR for 1 MBDSR corresponds 32 * 8Bytes MBs  */
    if (PayloadSize > FlexCAN_GetMbPayloadSize(pBase, MsgBuffIdx))
    {
        PayloadSize = FlexCAN_GetMbPayloadSize(pBase, MsgBuffIdx);
    }
#endif /* FLEXCAN_IP_FEATURE_HAS_FD */

    MsgBuff->dataLen = PayloadSize;
    /* Get a MB field values */
    MsgBuff->cs = *FlexcanMb;
    if ((MsgBuff->cs & FLEXCAN_IP_CS_IDE_MASK) != 0U)
    {
        MsgBuff->msgId = (*FlexcanMbId);
    }
    else
    {
        MsgBuff->msgId = (*FlexcanMbId) >> FLEXCAN_IP_ID_STD_SHIFT;
    }

#if (FLEXCAN_IP_FEATURE_HAS_HR_TIMER == STD_ON)
    if (FLEXCAN_IsHRTimeStampEnabled(pBase))
    {
        /* Extract the Time Stamp */
        MsgBuff->time_stamp = (uint32)pBase->HR_TIME_STAMP[MsgBuffIdx];
    }
    else
#endif /* EATURE_CAN_HAS_HR_TIMER */
    {
        MsgBuff->time_stamp = (uint32)((MsgBuff->cs & FLEXCAN_IP_CS_TIME_STAMP_MASK) >> FLEXCAN_IP_CS_TIME_STAMP_SHIFT);
    }

    {
        for (Idx = 0U; Idx < (PayloadSize & ~3U); Idx += 4U)
        {
            MbWord = FlexcanMbData32[Idx >> 2U];
            FLEXCAN_IP_SWAP_BYTES_IN_WORD(MbWord, MsgBuffData32[Idx >> 2U]);
        }
    }

    for (; Idx < PayloadSize; Idx++)
    {   /* Max allowed value for index is 63 */
        MsgBuff->data[Idx] = FlexcanMbData[FLEXCAN_IP_SWAP_BYTES_IN_WORD_INDEX(Idx)];
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_SetTxMsgBuff
 * Description   : Configure a message buffer for transmission.
 * This function will first check if RX FIFO is enabled. If RX FIFO is enabled,
 * the function will make sure if the MB requested is not occupied by RX FIFO
 * and ID filter table. Then this function will copy user's buffer into the
 * message buffer data area and configure the message buffer as required for
 * transmission.
 *
 *END**************************************************************************/
void FlexCAN_SetTxMsgBuff(volatile uint32 * const pMbAddr,
                          const Flexcan_Ip_MsbuffCodeStatusType * Cs,
                          uint32 MsgId,
                          const uint8 * MsgData,
                          const boolean IsRemote
                         )
{
#if (FLEXCAN_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert((boolean)(Cs != NULL_PTR));
#endif
    uint32 FlexcanMbConfig = 0;
    uint8 DataByte;
    uint8 DlcValue;
    uint8 PayloadSize;
    volatile uint32 * FlexcanMb = pMbAddr;
    volatile uint32 * FlexcanMbId   = &FlexcanMb[1];
    volatile uint8 * FlexcanMbData = (volatile uint8*)(&FlexcanMb[2]);
    volatile uint32 * FlexcanMbData32 = &FlexcanMb[2];
    const uint32 * MsgData32 = (const uint32*)MsgData;

        /* Clean up the arbitration field area and set TxMB Inactive */
        *FlexcanMb = (uint32)((((uint32)FLEXCAN_TX_INACTIVE & (uint32)0x1F) << (uint8)FLEXCAN_IP_CS_CODE_SHIFT) & (uint32)FLEXCAN_IP_CS_CODE_MASK);
        *FlexcanMbId = 0;

        /* Compute the value of the DLC field */
        DlcValue = FlexCAN_ComputeDLCValue(Cs->dataLen);
        /* Copy user's buffer into the message buffer data area */
        if (MsgData != NULL_PTR)
        {
            for (DataByte = 0; DataByte < (Cs->dataLen & ~3U); DataByte += 4U)
            {
                FLEXCAN_IP_SWAP_BYTES_IN_WORD((MsgData32[DataByte >> 2U]), (FlexcanMbData32[DataByte >> 2U]));
            }
            for ( ; DataByte < Cs->dataLen; DataByte++)
            {
                FlexcanMbData[FLEXCAN_IP_SWAP_BYTES_IN_WORD_INDEX(DataByte)] =  MsgData[DataByte];
            }
        #if (FLEXCAN_IP_FEATURE_HAS_FD == STD_ON)
            PayloadSize = FlexCAN_ComputePayloadSize(DlcValue);
            /* Add padding, if needed */
            for (DataByte = Cs->dataLen; DataByte < PayloadSize; DataByte++)
            {
                FlexcanMbData[FLEXCAN_IP_SWAP_BYTES_IN_WORD_INDEX(DataByte)] = Cs->fd_padding;
            }
        #endif /* FLEXCAN_IP_FEATURE_HAS_FD */
        }
        /* Set the ID according the format structure */
        if (FLEXCAN_MSG_ID_EXT == Cs->msgIdType)
        {
            /* ID [28-0] */
            *FlexcanMbId &= ~(FLEXCAN_IP_ID_STD_MASK | FLEXCAN_IP_ID_EXT_MASK);
            *FlexcanMbId |= (MsgId & (FLEXCAN_IP_ID_STD_MASK | FLEXCAN_IP_ID_EXT_MASK));
            /* Set IDE and SRR bit*/
            FlexcanMbConfig |= (FLEXCAN_IP_CS_IDE_MASK | FLEXCAN_IP_CS_SRR_MASK);
        }
        if (FLEXCAN_MSG_ID_STD == Cs->msgIdType)
        {
            /* ID[28-18] */
            *FlexcanMbId &= ~FLEXCAN_IP_ID_STD_MASK;
            *FlexcanMbId |= (MsgId << FLEXCAN_IP_ID_STD_SHIFT) & FLEXCAN_IP_ID_STD_MASK;
            /* make sure IDE and SRR are not set */
            FlexcanMbConfig &= ~(FLEXCAN_IP_CS_IDE_MASK | FLEXCAN_IP_CS_SRR_MASK);
        }
        /* Set the length of data in bytes */
        FlexcanMbConfig &= ~FLEXCAN_IP_CS_DLC_MASK;
        FlexcanMbConfig |= ((uint32)DlcValue << FLEXCAN_IP_CS_DLC_SHIFT) & FLEXCAN_IP_CS_DLC_MASK;
        /* Set MB CODE */
        if (Cs->code != (uint32)FLEXCAN_TX_NOT_USED)
        {
            if ((uint32)FLEXCAN_TX_REMOTE == Cs->code)
            {
                /* Set RTR bit */
                FlexcanMbConfig |= FLEXCAN_IP_CS_RTR_MASK;
            }
            else
            {
                if (TRUE == IsRemote)
                {
                    /* Set RTR bit */
                    FlexcanMbConfig |= FLEXCAN_IP_CS_RTR_MASK;
                }
            }
            /* Reset the code */
            FlexcanMbConfig &= ~FLEXCAN_IP_CS_CODE_MASK;
            /* Set the code */
            if (Cs->fd_enable)
            {
                FlexcanMbConfig |= ((Cs->code << FLEXCAN_IP_CS_CODE_SHIFT) & FLEXCAN_IP_CS_CODE_MASK) | FLEXCAN_IP_MB_EDL_MASK;
                /* In case of FD frame not supported RTR */
                FlexcanMbConfig &= ~FLEXCAN_IP_CS_RTR_MASK;
            }
            else
            {
                FlexcanMbConfig |= (Cs->code << FLEXCAN_IP_CS_CODE_SHIFT) & FLEXCAN_IP_CS_CODE_MASK;
            }

            if (Cs->enable_brs)
            {
                FlexcanMbConfig |= FLEXCAN_IP_MB_BRS_MASK;
            }
            *FlexcanMb = FlexcanMbConfig;
        }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_SetMaxMsgBuffNum
 * Description   : Set the number of the last Message Buffers.
 * This function will define the number of the last Message Buffers
 *
 *END***************************************************************************/
Flexcan_Ip_StatusType FlexCAN_SetMaxMsgBuffNum(FLEXCAN_Type * pBase, uint32 MaxMsgBuffNum)
{
#if (FLEXCAN_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert((boolean)(MaxMsgBuffNum>0U));
#endif
    uint32 MsgBuffIdx;
    uint32 DataByte;
    uint32 MaxMbNum = FlexCAN_GetMaxMbNum(pBase);
    uint32 RamSize = (uint32)FlexCAN_GetMaxMbNum(pBase) * (uint32)4U;
    const volatile uint32 * RAM = &(((volatile uint32 *)pBase)[FLEXCAN_IP_FEATURE_RAM_OFFSET >> 2U]);
    const volatile uint32 * ValEndMbPointer = NULL_PTR;
    volatile uint32 *FlexcanMb = NULL_PTR;
    volatile uint32 *FlexcanMbId   = NULL_PTR ;
    volatile uint8  *FlexcanMbData = NULL_PTR;
    uint8 ArbitrationFieldSize = 8U;
    uint8 FlexcanRealPayload = FlexCAN_GetMbPayloadSize(pBase, MaxMsgBuffNum - (uint32)1U);
    Flexcan_Ip_PtrSizeType ValEndMb = 0U;
    Flexcan_Ip_PtrSizeType ValEndRam = 0U;
    Flexcan_Ip_StatusType Status = FLEXCAN_STATUS_SUCCESS;

    ValEndMbPointer = FlexCAN_GetMsgBuffRegion(pBase, (MaxMsgBuffNum - (uint32)1U));

    ValEndMb = (Flexcan_Ip_PtrSizeType)ValEndMbPointer + FlexcanRealPayload + ArbitrationFieldSize;
    {
        ValEndRam = (Flexcan_Ip_PtrSizeType)&RAM[RamSize];
    }

    if ((ValEndMb > ValEndRam) || (MaxMsgBuffNum > MaxMbNum))
    {
        Status = FLEXCAN_STATUS_BUFF_OUT_OF_RANGE;
    }

    if (FLEXCAN_STATUS_SUCCESS == Status)
    {
        /* Set the maximum number of MBs*/
        pBase->MCR = (pBase->MCR & ~FLEXCAN_MCR_MAXMB_MASK) | (((MaxMsgBuffNum-1U) << FLEXCAN_MCR_MAXMB_SHIFT) & FLEXCAN_MCR_MAXMB_MASK);
        if (FALSE == FlexCAN_IsRxFifoEnabled(pBase))
        {
            /* Initialize all message buffers as inactive */
            for (MsgBuffIdx = 0; MsgBuffIdx < MaxMsgBuffNum; MsgBuffIdx++)
            {
                FlexcanMb = FlexCAN_GetMsgBuffRegion(pBase, MsgBuffIdx);
                FlexcanMbId   = &FlexcanMb[1];
                FlexcanMbData = (volatile uint8*)(&FlexcanMb[2]);
                *FlexcanMb = 0x0U;
                *FlexcanMbId = 0x0U;
                FlexcanRealPayload = FlexCAN_GetMbPayloadSize(pBase, MsgBuffIdx);
                for (DataByte = 0; DataByte < FlexcanRealPayload; DataByte++)
                {
                   FlexcanMbData[DataByte] = 0x0U;
                }
            }
        }
    }
    return Status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_SetOperationMode
 * Description   : Enable a FlexCAN operation mode.
 * This function will enable one of the modes listed in Flexcan_Ip_ModesType.
 *
 *END**************************************************************************/
void FlexCAN_SetOperationMode(FLEXCAN_Type * pBase, Flexcan_Ip_ModesType Mode)
{
    switch (Mode)
    {
        case FLEXCAN_NORMAL_MODE:
            pBase->CTRL1 = (pBase->CTRL1 & ~FLEXCAN_CTRL1_LOM_MASK) | FLEXCAN_CTRL1_LOM(0U);
            pBase->CTRL1 = (pBase->CTRL1 & ~FLEXCAN_CTRL1_LPB_MASK) | FLEXCAN_CTRL1_LPB(0U);
            break;
        case FLEXCAN_LISTEN_ONLY_MODE:
            pBase->CTRL1 = (pBase->CTRL1 & ~FLEXCAN_CTRL1_LOM_MASK) | FLEXCAN_CTRL1_LOM(1U);
            break;
        case FLEXCAN_LOOPBACK_MODE:
            pBase->CTRL1 = (pBase->CTRL1 & ~FLEXCAN_CTRL1_LPB_MASK) | FLEXCAN_CTRL1_LPB(1U);
            /* Enable Self Reception */
            FlexCAN_SetSelfReception(pBase, TRUE);
            break;
        default:
            /* Should not get here */
            break;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_SetRxFifoFilter
 * Description   : Configure RX FIFO ID filter table elements.
 *
 *END**************************************************************************/
void FlexCAN_SetRxFifoFilter(FLEXCAN_Type * pBase,
                             Flexcan_Ip_RxFifoIdElementFormatType IdFormat,
                             const Flexcan_Ip_IdTableType * IdFilterTable
                            )
{
    /* Set Legacy Rx Fifo acceptance mode */
    /* Start critical section: implementation depends on integrator */
    SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_13();
    pBase->MCR = (((pBase->MCR) & ~(FLEXCAN_MCR_IDAM_MASK)) | ((((uint32)(((uint32)(IdFormat)) << FLEXCAN_MCR_IDAM_SHIFT)) & FLEXCAN_MCR_IDAM_MASK)));
    /* End critical section: implementation depends on integrator */
    SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_13();

    /* Set Legacy Rx Fifo filter */
    switch (IdFormat)
    {
        case (FLEXCAN_RX_FIFO_ID_FORMAT_A):
            FlexCAN_SetRxFifoFilterFormatA(pBase, IdFilterTable);
            break;
        case (FLEXCAN_RX_FIFO_ID_FORMAT_B):
            FlexCAN_SetRxFifoFilterFormatB(pBase, IdFilterTable);
            break;
        case (FLEXCAN_RX_FIFO_ID_FORMAT_C):
            FlexCAN_SetRxFifoFilterFormatC(pBase, IdFilterTable);
            break;
        default:
            /* All frames are rejected with format D */
            break;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_ReadRxFifo
 * Description   : Read Rx FIFO data.
 * This function will copy MB[0] data field into user's buffer.
 *
 *END**************************************************************************/
void FlexCAN_ReadRxFifo(const FLEXCAN_Type * pBase, Flexcan_Ip_MsgBuffType * RxFifo)
{
#if (FLEXCAN_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert((boolean)(RxFifo != NULL_PTR));
#endif
    uint32 DataByte;
    uint32 MbWord;
    volatile const uint32 * FlexcanMb = &(((volatile const uint32 *)pBase)[FLEXCAN_IP_FEATURE_RAM_OFFSET >> 2U]);
    volatile const uint32 * FlexcanMbId = &FlexcanMb[1];
    volatile const uint32 * FlexcanMbData32 = &FlexcanMb[2];
    uint32 * MsgData32 = NULL_PTR;
    uint8 FlexcanMbDlcValue = (uint8)(((*FlexcanMb) & FLEXCAN_IP_CS_DLC_MASK) >> 16);
    uint8 FlexcanRealPayload = FlexCAN_ComputePayloadSize(FlexcanMbDlcValue);

    MsgData32 = (uint32 *)(RxFifo->data);
    /*
       Check if the length of received data packet bigger than the maximum length accepted,
       then processing flow shall continue with the maximum length defined by configuration.
       Legacy FIFO just support in normal mode.
    */
    /* no need to check if FD enabled or not because this function just is invoked when legacy fifo enabled only ! */
    if (FlexcanRealPayload > 8U)
    {
        FlexcanRealPayload = 8U;
    }

    RxFifo->dataLen = FlexcanRealPayload;
    RxFifo->cs = *FlexcanMb;
    if ((RxFifo->cs & FLEXCAN_IP_CS_IDE_MASK) != 0U)
    {
        RxFifo->msgId = *FlexcanMbId;
    }
    else
    {
        RxFifo->msgId = (*FlexcanMbId) >> FLEXCAN_IP_ID_STD_SHIFT;
    }
    /* Extract the IDHIT */
    RxFifo->id_hit = (uint8)(pBase->RXFIR & (uint32)UINT8_MAX);
    /* Extract the Time Stamp */
    RxFifo->time_stamp = (uint32)((RxFifo->cs & FLEXCAN_IP_CS_TIME_STAMP_MASK) >> FLEXCAN_IP_CS_TIME_STAMP_SHIFT);
    {
        /* Copy MB[0] data field into user's buffer */
        for (DataByte = 0U; DataByte < FlexcanRealPayload; DataByte += 4U)
        {
            MbWord = FlexcanMbData32[DataByte >> 2U];
            FLEXCAN_IP_SWAP_BYTES_IN_WORD(MbWord, MsgData32[DataByte >> 2U]);
        }
    }
}
#if (FLEXCAN_IP_FEATURE_HAS_ENHANCED_RX_FIFO == STD_ON)
/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_ReadEnhancedRxFifo
 * Description   : Read Enhanced Rx FIFO data.
 * This function will copy Enhanced Rx FIFO data output into user's buffer.
 *
 *END**************************************************************************/
void FlexCAN_ReadEnhancedRxFifo(const uint8 u8Instance, Flexcan_Ip_MsgBuffType * RxFifo)
{
#if (FLEXCAN_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert((boolean)(RxFifo != NULL_PTR));
#endif
    uint32 DataByte;
    uint32 MbWord;
    uint8 IdhitOffset;
    static FLEXCAN_Type * const FlexcanBase[] = FLEXCAN_IP_BASE_PTRS_HAS_ENHANCED_RX_FIFO;
    const FLEXCAN_Type * pBase = FlexcanBase[u8Instance];
    volatile const uint32 * FlexcanMb = &(((volatile const uint32 *)pBase)[FLEXCAN_IP_FEATURE_ENHANCED_FIFO_RAM_OFFSET >> 2U]);
    volatile const uint32 * FlexcanMbId = &FlexcanMb[1];
    volatile const uint32 * FlexcanMbData32 = &FlexcanMb[2];
    uint32 * MsgData32 = NULL_PTR;
    uint8 IdHitaux = 0U;
    uint8 FilterElement = 0U;
    uint8 IdHitVal = 0U;

    /* Compute payload size */
    uint8 FlexcanMbDlcValue = (uint8)(((*FlexcanMb) & FLEXCAN_IP_CS_DLC_MASK) >> FLEXCAN_IP_CS_DLC_SHIFT);
    uint8 FlexcanRealPayload = FlexCAN_ComputePayloadSize(FlexcanMbDlcValue);

    /* Read the IdHit Value */
    if (((*FlexcanMb) & FLEXCAN_IP_CS_RTR_MASK) != 0U)
    {
        FlexcanRealPayload = 0U;
    }
    IdhitOffset = (FlexcanRealPayload >> 2U) + (((FlexcanRealPayload % 4U) != 0U) ? 1U : 0U);
    IdHitaux = (uint8)(((FlexcanMbData32[IdhitOffset]) & FLEXCAN_IP_ENHANCED_IDHIT_MASK) >> FLEXCAN_IP_ENHANCED_IDHIT_SHIFT);
    FilterElement = (((*FlexcanMb) & FLEXCAN_IP_CS_IDE_MASK) == 0U) ? ((uint8)(((pBase->ERFCR) & FLEXCAN_ERFCR_NEXIF_MASK) >> FLEXCAN_ERFCR_NEXIF_SHIFT) + IdHitaux) : IdHitaux;
    /* Check if FilterElement is not greater than filter depth in order to access FilterIdMap. Always in range of 128U elements */
    FilterElement = (uint8)(FilterElement & (uint8)0x7FU);
    IdHitVal  = FilterIdMap[u8Instance][FilterElement];

    MsgData32 = (uint32 *)(RxFifo->data);
    RxFifo->dataLen = FlexcanRealPayload;
    RxFifo->cs = *FlexcanMb;
    if ((RxFifo->cs & FLEXCAN_IP_CS_IDE_MASK) != 0U)
    {
        RxFifo->msgId = *FlexcanMbId;
    }
    else
    {
        RxFifo->msgId = (*FlexcanMbId) >> FLEXCAN_IP_ID_STD_SHIFT;
    }
    /* Extract the IDHIT */
    RxFifo->id_hit = IdHitVal;

#if (FLEXCAN_IP_FEATURE_HAS_HR_TIMER == STD_ON)
    /* Extract the Time Stamp */
    if (FLEXCAN_IsHRTimeStampEnabled(pBase))
    {
        RxFifo->time_stamp = (uint32)(FlexcanMbData32[IdhitOffset + 1U]);
    }
    else
#endif
    {
        RxFifo->time_stamp = (uint32)((RxFifo->cs & FLEXCAN_IP_CS_TIME_STAMP_MASK) >> FLEXCAN_IP_CS_TIME_STAMP_SHIFT);
    }

    /* Copy EnhancedRxFIFO data field into user's buffer */
    for (DataByte = 0U; DataByte < FlexcanRealPayload; DataByte += 4U)
    {
        MbWord = FlexcanMbData32[DataByte >> 2U];
        FLEXCAN_IP_SWAP_BYTES_IN_WORD((MbWord), (MsgData32[DataByte >> 2U]));
    }
}
#endif /* FLEXCAN_IP_FEATURE_HAS_ENHANCED_RX_FIFO */
/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_IsMbOutOfRange
 * Description   : Check if the mb index is out of range or not.
 *
 *END**************************************************************************/
boolean FlexCAN_IsMbOutOfRange
(
    const FLEXCAN_Type * pBase,
    uint32 u32MbIndex,
    boolean bIsLegacyFifoEn,
    uint32 u32MaxMbNum
)
{
    boolean ReturnValue = FALSE;
    uint32 u32NumOfFiFoElement = 0U;
    uint32 u32NumOfMbOccupiedByFiFo = 0U;

    if (u32MbIndex >= u32MaxMbNum)
    {
       ReturnValue = TRUE;
    }
    /* Check if RX FIFO is enabled*/
    else if (TRUE == bIsLegacyFifoEn)
    {
        /* Get the number of RX FIFO Filters*/
        u32NumOfFiFoElement = (((pBase->CTRL2) & FLEXCAN_CTRL2_RFFN_MASK) >> FLEXCAN_CTRL2_RFFN_SHIFT);
        /* Get the number if MBs occupied by RX FIFO and ID filter table*/
        /* the Rx FIFO occupies the memory space originally reserved for MB0-5*/
        /* Every number of RFFN means 8 number of RX FIFO filters*/
        /* and every 4 number of RX FIFO filters occupied one MB*/
        u32NumOfMbOccupiedByFiFo = 5U + ((((u32NumOfFiFoElement) + 1U) * 8U) / 4U);
        if (u32MbIndex <= u32NumOfMbOccupiedByFiFo)
        {
            ReturnValue = TRUE;
        }
    }
    else
    {
        ReturnValue = FALSE;
    }

    return ReturnValue;
}

#if (FLEXCAN_IP_FEATURE_HAS_ENHANCED_RX_FIFO == STD_ON)
/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_IsEnhancedRxFifoAvailable
 * Description   : Checks if FlexCAN has Enhanced Rx FIFO.
 * This function is private.
 *
 *END**************************************************************************/
boolean FlexCAN_IsEnhancedRxFifoAvailable(const FLEXCAN_Type * pBase)
{
    uint32 Idx=0U;
    static FLEXCAN_Type * const FlexcanBase[] = FLEXCAN_IP_BASE_PTRS_HAS_ENHANCED_RX_FIFO;
    boolean ReturnValue = FALSE;

    for (Idx = 0U; Idx < FLEXCAN_IP_FEATURE_ENHANCED_RX_FIFO_NUM; Idx++)
    {
        if (pBase == FlexcanBase[Idx])
        {
            ReturnValue = TRUE;
            break;
        }
    }

    return ReturnValue;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_IsEnhancedRxFifoEnabled
 * Description   : Checks if Enhanced Rx FIFO is enabled.
 * This function always return false if current FlexCAN doesn't has Enhanced Fifo support.
 *
 *END**************************************************************************/
boolean FlexCAN_IsEnhancedRxFifoEnabled(const FLEXCAN_Type * pBase)
{
    boolean ReturnValue = FALSE;

    if (TRUE == FlexCAN_IsEnhancedRxFifoAvailable(pBase))
    {
        ReturnValue = ((((pBase->ERFCR & FLEXCAN_ERFCR_ERFEN_MASK) >> FLEXCAN_ERFCR_ERFEN_SHIFT) != 0U) ? (TRUE): (FALSE));
    }

    return ReturnValue;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_EnableEnhancedRxFifo
 * Description   : Enable Enhanced Rx FIFO feature.
 * This function will enable the Enhanced Rx FIFO feature.
 *
 *END**************************************************************************/
Flexcan_Ip_StatusType FlexCAN_EnableEnhancedRxFifo(FLEXCAN_Type * pBase,
                                                   uint32 NumOfStdIDFilters,
                                                   uint32 NumOfExtIDFilters,
                                                   uint32 NumOfWatermark
                                                  )
{
#if (FLEXCAN_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert((boolean)(NumOfStdIDFilters <= FLEXCAN_IP_ENHANCED_RXFIFO_STD_FILTER_MAX));
    DevAssert((boolean)(NumOfExtIDFilters <= FLEXCAN_IP_ENHANCED_RXFIFO_EXT_FILTER_MAX));
#endif
    Flexcan_Ip_StatusType Stat = FLEXCAN_STATUS_ERROR;
    uint32 NumOfEnhancedFilters = 0U;

    /*Enhanced RX FIFO and Legacy RX FIFO cannot be enabled at the same time.*/
    if (FALSE == FlexCAN_IsRxFifoEnabled(pBase))
    {
        if (TRUE == FlexCAN_IsEnhancedRxFifoAvailable(pBase))
        {
            if ((0U == NumOfStdIDFilters) && (0U == NumOfExtIDFilters))
            {
                Stat = FLEXCAN_STATUS_ERROR;
            }
            /* If the no of Std Filters is odd */
            else if (1U == (NumOfStdIDFilters & 1U))
            {
                Stat = FLEXCAN_STATUS_ERROR;
            }
            else
            {
                /* NumOfEnhancedFilters equals (NumOfStdIDFilters/2) + NumOfExtIDFilters - 1U */
                NumOfEnhancedFilters = (NumOfStdIDFilters >> 1U) + NumOfExtIDFilters - 1U;
                /* Enable Enhanced Rx FIFO */
                pBase->ERFCR = (pBase->ERFCR & ~FLEXCAN_ERFCR_ERFEN_MASK) | FLEXCAN_ERFCR_ERFEN(1U);
                /* Reset Enhanced Rx FIFO engine */
                pBase->ERFSR = (pBase->ERFSR & ~FLEXCAN_ERFSR_ERFCLR_MASK) | FLEXCAN_ERFSR_ERFCLR(1U);
                /* Clear the status bits of the Enhanced RX FIFO */
                pBase->ERFSR = pBase->ERFSR & ~(FLEXCAN_ERFSR_ERFUFW_MASK | FLEXCAN_ERFSR_ERFOVF_MASK | FLEXCAN_ERFSR_ERFWMI_MASK | FLEXCAN_ERFSR_ERFDA_MASK);
                /* Set the total number of enhanced Rx FIFO filter elements */
                pBase->ERFCR = (pBase->ERFCR & ~FLEXCAN_ERFCR_NFE_MASK) | ((NumOfEnhancedFilters << FLEXCAN_ERFCR_NFE_SHIFT) & FLEXCAN_ERFCR_NFE_MASK);
                /* Set the number of extended ID filter elements */
                pBase->ERFCR = (pBase->ERFCR & ~FLEXCAN_ERFCR_NEXIF_MASK) | ((NumOfExtIDFilters << FLEXCAN_ERFCR_NEXIF_SHIFT) & FLEXCAN_ERFCR_NEXIF_MASK);
                /* Set the Enhanced Rx FIFO watermark */
                pBase->ERFCR = (pBase->ERFCR & ~FLEXCAN_ERFCR_ERFWM_MASK) | ((NumOfWatermark << FLEXCAN_ERFCR_ERFWM_SHIFT) & FLEXCAN_ERFCR_ERFWM_MASK);

                Stat = FLEXCAN_STATUS_SUCCESS;
            }
        }
    }
    return Stat;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_SetEnhancedRxFifoFilter
 * Description   : Configure Enhanced RX FIFO ID filter table elements.
 *
 *END**************************************************************************/
void FlexCAN_SetEnhancedRxFifoFilter(const uint8 u8Instance, const Flexcan_Ip_EnhancedIdTableType * IdFilterTable)
{
    uint8 IDFilterIdx;
    uint8 StdIDFilterIdx = 0U;
    uint8 ExtIDFilterIdx = 0U;

    static FLEXCAN_Type * const FlexcanBase[] = FLEXCAN_IP_BASE_PTRS_HAS_ENHANCED_RX_FIFO;
    FLEXCAN_Type * pBase = FlexcanBase[u8Instance];

    uint8 NumOfEnhancedFilter = (uint8)(((pBase->ERFCR) & FLEXCAN_ERFCR_NFE_MASK) >> FLEXCAN_ERFCR_NFE_SHIFT);
    uint8 NumOfExtIDFilter = (uint8)(((pBase->ERFCR) & FLEXCAN_ERFCR_NEXIF_MASK) >> FLEXCAN_ERFCR_NEXIF_SHIFT);
    uint8 NumOfStdIDFilter = (NumOfExtIDFilter > NumOfEnhancedFilter) ? 0U : (2U * (NumOfEnhancedFilter - NumOfExtIDFilter + 1U));

    volatile uint32 * FilterExtIDTable = (volatile uint32 *)&pBase->ERFFEL[FLEXCAN_IP_ENHANCED_RXFIFO_EXT_FILTER_TABLE_BASE];
    volatile uint32 * FilterStdIDTable = (volatile uint32 *)&pBase->ERFFEL[FLEXCAN_IP_ENHANCED_RXFIFO_STD_FILTER_TABLE_BASE(NumOfExtIDFilter)];

    for (IDFilterIdx = 0U; IDFilterIdx < (NumOfExtIDFilter + NumOfStdIDFilter); IDFilterIdx++)
    {
        if (!(IdFilterTable[IDFilterIdx].isExtendedFrame))
        {
            FilterIdMap[u8Instance][NumOfExtIDFilter + StdIDFilterIdx] = IDFilterIdx;
            FlexCAN_SetEnhancedRxFifoFilterStandardFormat(&FilterStdIDTable[StdIDFilterIdx], &IdFilterTable[IDFilterIdx]);
            if (StdIDFilterIdx < (uint8)(FLEXCAN_IP_ENHANCED_RXFIFO_FILTERDEPTH - 1U))
            {
                StdIDFilterIdx += 1U;
            }
        }
        else
        {
            FilterIdMap[u8Instance][ExtIDFilterIdx >> 1U] = IDFilterIdx;
            FlexCAN_SetEnhancedRxFifoFilterExtendedFormat(&FilterExtIDTable[ExtIDFilterIdx], &IdFilterTable[IDFilterIdx]);
            if (ExtIDFilterIdx < (uint8)(FLEXCAN_IP_ENHANCED_RXFIFO_FILTERDEPTH - 2U))
            {
                ExtIDFilterIdx += 2U;
            }
        }
    }
}

#if (FLEXCAN_IP_FEATURE_HAS_DMA_ENABLE == STD_ON)
/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_ClearOutputEnhanceFIFO
 * Description   : Clear output of enhance fifo.
 *
 *END**************************************************************************/
void FlexCAN_ClearOutputEnhanceFIFO(FLEXCAN_Type * pBase)
{
    volatile const uint32 * Ram = (uint32 *)((Flexcan_Ip_PtrSizeType)pBase);
    uint32 LastWordOffset = ((uint32)0x204C) / ((uint32)4U); /* fixed, because DMALW is always = 19 */
    uint8 u8TimeOut = 0;

    /* If Enhanced Rx FIFO has Pending Request that generated error,
     * the EnhancedRxFIFO need to be empty to activate DMA */
    if ((uint8)1U == FlexCAN_GetEnhancedRxFIFOIntStatusFlag(pBase, FLEXCAN_IP_ENHANCED_RXFIFO_FRAME_AVAILABLE))
    {
        /* Enter CAN in freeze Mode to allow Enhanced Rx FIFO Clear */
        (void)FlexCAN_EnterFreezeMode(pBase);
        FlexCAN_ClearEnhancedFIFO(pBase);

        do
        {
            if ((uint32)FLEXCAN_MCR_DMA_MASK == (pBase->MCR & ((uint32)FLEXCAN_MCR_DMA_MASK)))
            {
                /* Read Enhanced Output to clear DMA pending request */
                (void)Ram[LastWordOffset];
            }
            else
            {
                FlexCAN_ClearEnhancedRxFifoIntStatusFlag(pBase, FLEXCAN_IP_ENHANCED_RXFIFO_FRAME_AVAILABLE);
            }
            u8TimeOut++;
        }
        while (((uint8)1U == FlexCAN_GetEnhancedRxFIFOIntStatusFlag(pBase, FLEXCAN_IP_ENHANCED_RXFIFO_FRAME_AVAILABLE)) && (u8TimeOut <= ((uint8)64U))); /* avoid blocking */

        FlexCAN_ClearEnhancedRxFifoIntStatusFlag(pBase, FLEXCAN_IP_ENHANCED_RXFIFO_OVERFLOW);
        FlexCAN_ClearEnhancedRxFifoIntStatusFlag(pBase, FLEXCAN_IP_ENHANCED_RXFIFO_WATERMARK);
        FlexCAN_ClearEnhancedRxFifoIntStatusFlag(pBase, FLEXCAN_IP_ENHANCED_RXFIFO_FRAME_AVAILABLE);
        FlexCAN_ClearEnhancedRxFifoIntStatusFlag(pBase, FLEXCAN_IP_ENHANCED_RXFIFO_UNDERFLOW);

        /* Return CAN to normal Mode */
        (void)FlexCAN_ExitFreezeMode(pBase);
    }
}
#endif /* (FLEXCAN_IP_FEATURE_HAS_DMA_ENABLE == STD_ON) */
#endif /* (FLEXCAN_IP_FEATURE_HAS_ENHANCED_RX_FIFO == STD_ON) */

#if (FLEXCAN_IP_FEATURE_HAS_DMA_ENABLE == STD_ON)
/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_ClearOutputLegacyFIFO
 * Description   : Clear output of legacy fifo.
 *
 *END**************************************************************************/
void FlexCAN_ClearOutputLegacyFIFO(FLEXCAN_Type * pBase)
{
    volatile const uint32 * Ram = (uint32 *)((Flexcan_Ip_PtrSizeType)pBase);
    uint32 LastWordOffset = ((uint32)0x8C) / ((uint32)4U); /* fixed, dma last word */
    uint8 Idx = 0;

    /* Check if FIFO has Pending Request that generated error,
    * the RxFIFO need to be empty to activate DMA */
    if ((uint8)1U == FlexCAN_GetMsgBuffIntStatusFlag(pBase, FLEXCAN_IP_LEGACY_RXFIFO_FRAME_AVAILABLE))
    {
        /* Enter CAN in freeze Mode to allow FIFO Clear */
        (void)FlexCAN_EnterFreezeMode(pBase);
        FlexCAN_ClearFIFO(pBase);
        if ((uint32)FLEXCAN_MCR_DMA_MASK == (pBase->MCR & ((uint32)FLEXCAN_MCR_DMA_MASK)))
        {
            do
            {
                /* Read Offset 0x8C to clear DMA pending request */
                (void)Ram[LastWordOffset];
                Idx++;
            }
            while (((uint8)1U == FlexCAN_GetMsgBuffIntStatusFlag(pBase, FLEXCAN_IP_LEGACY_RXFIFO_FRAME_AVAILABLE)) && (Idx <= (uint8)12U)); /* avoid blocking */
        }

        FlexCAN_ClearMsgBuffIntStatusFlag(pBase, FLEXCAN_IP_LEGACY_RXFIFO_FRAME_AVAILABLE);
        FlexCAN_ClearMsgBuffIntStatusFlag(pBase, FLEXCAN_IP_LEGACY_RXFIFO_WARNING);
        FlexCAN_ClearMsgBuffIntStatusFlag(pBase, FLEXCAN_IP_LEGACY_RXFIFO_OVERFLOW);

        /* Return CAN to normal Mode */
        (void)FlexCAN_ExitFreezeMode(pBase);
    }
}
#endif /* FLEXCAN_IP_FEATURE_HAS_DMA_ENABLE */

#if (FLEXCAN_IP_FEATURE_HAS_TS_ENABLE == STD_ON)
#if (FLEXCAN_IP_FEATURE_HAS_HR_TIMER == STD_ON)
#if (FLEXCAN_IP_HR_TIMESTAMP_SRC_SELECTABLE == STD_ON)
/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_ConfigTimestampModule
 * Description   : Configure High Resolution Timestamp Source.
 *
 *END**************************************************************************/
static void FlexCAN_ConfigTimestampModule(uint8 Instance, const Flexcan_Ip_TimeStampConfigType * Config)
{
    #ifdef FLEXCAN_IP_TIMESTAMP_HR_SOURCE_TBS_USED
        CAN_BASE_TBS_TYPE * const CanTBSBase[] = FLEXCAN_IP_CAN_TBS_BASE_PTRS;
        CAN_BASE_TBS_TYPE * pCanTBSBase = CanTBSBase[Instance];

        pCanTBSBase->CAN_TS_SEL &= ~(FLEXCAN_IP_TIMESTAMP_EN_MASK | FLEXCAN_IP_TIMESTAMP_SEL_MASK);
        pCanTBSBase->CAN_TS_SEL |= FLEXCAN_IP_TIMESTAMP_SEL(Config->hrSrc)|FLEXCAN_IP_TIMESTAMP_EN_MASK;
    #elif defined(FLEXCAN_IP_TIMESTAMP_HR_SOURCE_GPR_SELECT)
        if (Instance < 17U)
        {
            FLEXCAN_IP_TIMESTAMP_EN_REG |= FLEXCAN_IP_TIMESTAMP_x_EN_MASK(Instance);
            FLEXCAN_IP_TIMESTAMP_SEL_REG &= ~FLEXCAN_IP_TIMESTAMP_x_SEL_MASK(Instance);
            FLEXCAN_IP_TIMESTAMP_SEL_REG |= FLEXCAN_IP_TIMESTAMP_x_SEL(Instance, Config->hrSrc);
        }
        else
        {
            FLEXCAN_IP_TIMESTAMP_REG &= ~(FLEXCAN_IP_TIMESTAMP_EN_MASK | FLEXCAN_IP_TIMESTAMP_SEL_MASK);
            FLEXCAN_IP_TIMESTAMP_REG |= FLEXCAN_IP_TIMESTAMP_SEL(Config->hrSrc)|FLEXCAN_IP_TIMESTAMP_EN_MASK;
        }
    #else
        FLEXCAN_IP_TIMESTAMP_REG &= ~(FLEXCAN_IP_TIMESTAMP_EN_MASK | FLEXCAN_IP_TIMESTAMP_SEL_MASK);
        FLEXCAN_IP_TIMESTAMP_REG |= FLEXCAN_IP_TIMESTAMP_SEL(Config->hrSrc)|FLEXCAN_IP_TIMESTAMP_EN_MASK;
    #endif
    /* Prevent compiler warning */
    (void)Instance;
}
#endif /* (FLEXCAN_IP_HR_TIMESTAMP_SRC_SELECTABLE == STD_ON) */
#endif /* (FLEXCAN_IP_FEATURE_HAS_HR_TIMER == STD_ON) */

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_ConfigTimestamp
 * Description   : Configures the timestamp feature.
 *
 *END**************************************************************************/
void FlexCAN_ConfigTimestamp(uint8 Instance, const Flexcan_Ip_TimeStampConfigType * Config)
{
    uint32 TimeStampConfig = 0U;
    static FLEXCAN_Type * const FlexcanBase[] = FLEXCAN_IP_BASE_PTRS;
    FLEXCAN_Type * pBase = FlexcanBase[Instance];

    SchM_Enter_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_19();
    TimeStampConfig = FLEXCAN_CTRL2_TIMER_SRC(Config->timeStampSurce);
#if (FLEXCAN_IP_FEATURE_HAS_HR_TIMER == STD_ON)
    TimeStampConfig |= FLEXCAN_CTRL2_MBTSBASE(Config->msgBuffTimeStampType);
    TimeStampConfig |= FLEXCAN_CTRL2_TSTAMPCAP(Config->hrConfigType);
    pBase->CTRL2 &= ~(FLEXCAN_CTRL2_TIMER_SRC_MASK | FLEXCAN_CTRL2_MBTSBASE_MASK | FLEXCAN_CTRL2_TSTAMPCAP_MASK);
#else
    pBase->CTRL2 &= ~(FLEXCAN_CTRL2_TIMER_SRC_MASK);
#endif /* (FLEXCAN_IP_FEATURE_HAS_HR_TIMER == STD_ON) */
    pBase->CTRL2 |= TimeStampConfig;
    SchM_Exit_Can_43_FLEXCAN_CAN_EXCLUSIVE_AREA_19();

#if (FLEXCAN_IP_FEATURE_HAS_HR_TIMER == STD_ON)
#if (FLEXCAN_IP_HR_TIMESTAMP_SRC_SELECTABLE == STD_ON)
    if (Config->hrConfigType != FLEXCAN_TIMESTAMPCAPTURE_DISABLE)
    {
    #if (STD_ON == FLEXCAN_IP_SET_USER_ACCESS_ALLOWED_AVAILABLE)
        OsIf_Trusted_Call2params(FlexCAN_ConfigTimestampModule, Instance, Config);
    #else
        FlexCAN_ConfigTimestampModule(Instance, Config);
    #endif
    }
#endif /* (FLEXCAN_IP_HR_TIMESTAMP_SRC_SELECTABLE == STD_ON) */
#endif /* (FLEXCAN_IP_FEATURE_HAS_HR_TIMER == STD_ON) */
}
#endif /* (FLEXCAN_IP_FEATURE_HAS_TS_ENABLE == STD_ON) */

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_ConfigCtrlOptions
 * Description   : configure controller depending on options
 * note: should be call after FD configuration.
 *
 *END**************************************************************************/
void FlexCAN_ConfigCtrlOptions(FLEXCAN_Type * pBase, uint32 u32Options)
{
#if (FLEXCAN_IP_FEATURE_SWITCHINGISOMODE == STD_ON)
    /* If the FD feature is enabled, in order to be ISO-compliant. */
    if ((u32Options & FLEXCAN_IP_ISO_U32) != 0U)
    {
        FlexCAN_SetIsoCan(pBase, TRUE);
    }
    else
    {
        /* This maybe don't have sense if the Deinit returns the state of registers at init values */
        FlexCAN_SetIsoCan(pBase, FALSE);
    }
#endif /*(FLEXCAN_IP_FEATURE_SWITCHINGISOMODE == STD_ON) */
    /* Set Entire Frame Arbitration Field Comparison. */
    if ((u32Options & FLEXCAN_IP_EACEN_U32) != 0U)
    {
        FlexCAN_SetEntireFrameArbitrationFieldComparison(pBase, TRUE);
    }
    else
    {
        FlexCAN_SetEntireFrameArbitrationFieldComparison(pBase, FALSE);
    }
#if (FLEXCAN_IP_FEATURE_PROTOCOLEXCEPTION == STD_ON)
    /* Set protocol Exception */
    if ((u32Options & FLEXCAN_IP_PROTOCOL_EXCEPTION_U32) != 0U)
    {
        FlexCAN_SetProtocolException(pBase, TRUE);
    }
    else
    {
        FlexCAN_SetProtocolException(pBase, FALSE);
    }
#endif /* Endif  (FLEXCAN_IP_FEATURE_PROTOCOLEXCEPTION == STD_ON)  */
    /* Set CAN Bit Sampling */
    if (((u32Options & FLEXCAN_IP_THREE_SAMPLES_U32) != 0U) && (0U == (pBase->MCR & FLEXCAN_MCR_FDEN_MASK)))
    {
        FlexCAN_CanBitSampling(pBase, TRUE);
    }
    else
    {
        FlexCAN_CanBitSampling(pBase, FALSE);
    }

    /* Set AutoBusOff Recovery */
    if ((u32Options & FLEXCAN_IP_BUSOFF_RECOVERY_U32) != 0U)
    {
        FlexCAN_SetBusOffAutorecovery(pBase, TRUE);
    }
    else
    {
        FlexCAN_SetBusOffAutorecovery(pBase, FALSE);
    }
    /* Set Remote Request Store for received of Remote Request Frames */
    if ((u32Options & FLEXCAN_IP_REM_STORE_U32) != 0U)
    {
        FlexCAN_SetRemoteReqStore(pBase, TRUE);
    }
    else
    {
        FlexCAN_SetRemoteReqStore(pBase, FALSE);
    }
#if (FLEXCAN_IP_FEATURE_EDGEFILTER == STD_ON)
    /* Set Edge Filter */
    if ((u32Options & FLEXCAN_IP_EDGE_FILTER_U32) != 0U)
    {
        FlexCAN_SetEdgeFilter(pBase, TRUE);
    }
    else
    {
        FlexCAN_SetEdgeFilter(pBase, FALSE);
    }
#endif /* End of (FLEXCAN_IP_FEATURE_EDGEFILTER == STD_ON)  */
    /* Set Lowest Buffer Transmitted First */
    if ((u32Options & FLEXCAN_IP_LOWEST_BUFF_FIRST_U32) != 0U)
    {
        FlexCAN_SetLowestBufferTransmittedFirst(pBase, TRUE);
    }
    else
    {
        FlexCAN_SetLowestBufferTransmittedFirst(pBase, FALSE);
    }
}


/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_ResetImaskBuff (uses in FlexCAN_Ip_Init function only)
 * Description   : Reset Imask Buffers.
 *
 *END**************************************************************************/
void FlexCAN_ResetImaskBuff(uint8 Instance)
{
    uint8 ImaskCnt = 0U;

    for (ImaskCnt = 0U; ImaskCnt < FLEXCAN_IP_FEATURE_MBDSR_COUNT; ImaskCnt++)
    {
        FlexCAN_Ip_au32ImaskBuff[Instance][ImaskCnt] = 0U;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_GetStopMode.
 * Description   : Get Stop mode.
 *
 *END**************************************************************************/
uint8 FlexCAN_GetStopMode(const FLEXCAN_Type * pBase)
{
    return ((FLEXCAN_MCR_LPMACK_MASK == (pBase->MCR & FLEXCAN_MCR_LPMACK_MASK)) ? 1U : 0U);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_GetStartMode.
 * Description   : Get Start mode.
 *
 *END**************************************************************************/
uint8 FlexCAN_GetStartMode(const FLEXCAN_Type * pBase)
{
    return ((0U == (pBase->MCR & (FLEXCAN_MCR_LPMACK_MASK | FLEXCAN_MCR_FRZACK_MASK))) ? 1U : 0U);
}

#if (FLEXCAN_IP_FEATURE_MEM_ERR_DET_ENABLED == STD_ON)
/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_EnableMemErrorDetection
 * Description   : Enable Memory Error Detection and Correction in FlexCAN memory.
 * This function will Enable Memory Error Detection and Correction feature.
 *
 *END**************************************************************************/
void FlexCAN_EnableMemErrorDetection(FLEXCAN_Type * pBase, boolean IsFreezeMode)
{
    /* Enable error correction configuration register write */
    pBase->CTRL2 |=  FLEXCAN_CTRL2_ECRWRE(1);
    /* Enable error configuration register write */
    pBase->MECR &= ~FLEXCAN_MECR_ECRWRDIS_MASK;
    /* disable all default error interrupts. user will Enable which they need. */
    pBase->MECR &= ~FLEXCAN_MECR_ERROR_INT_MASK;
    /* setting to put FlexCAN into freeze Mode or not when no-correctable error is detected. */
    if (TRUE == IsFreezeMode)
    {
        pBase->MECR |= FLEXCAN_MECR_NCEFAFRZ(1);
    }
    else
    {
        pBase->MECR &= ~FLEXCAN_MECR_NCEFAFRZ_MASK;
    }
    /* Enable error detection and correction mechanism */
    pBase->MECR &= ~FLEXCAN_MECR_ECCDIS_MASK;
    /* Enable error report */
    pBase->MECR &= ~FLEXCAN_MECR_RERRDIS_MASK;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_SetMemErrorDetectionIntCmd
 * Description   : Set Memory Error Detection and Correction interrupts.
 * This function will Enable/disable Memory Error Detection and Correction interrupt for corresponding error source.
 *
 *END**************************************************************************/
void FlexCAN_SetMemErrorDetectionIntCmd(FLEXCAN_Type * pBase,
                                        Flexcan_Ip_MemErrDetectionIntMaskType ErrorMaskType,
                                        boolean IsEnable)
{
    uint32 TempVal = (uint32)ErrorMaskType;

    /* Enable interrupt */
    if (IsEnable)
    {
        pBase->MECR |= TempVal;
    }
    /* disable interrupt */
    else
    {
        pBase->MECR &= ~TempVal;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_EnableMemErrorInjection
 * Description   : Set Memory Error Injection info.
 * This function will set Memory Error Injection info for corresponding error source.
 *
 *END**************************************************************************/
void FlexCAN_SetMemErrorInjectionInfo(FLEXCAN_Type * pBase,
                                      uint32 u32InjectionAddr,
                                      uint32 u32InjectionData,
                                      uint32 u32InjectionParity)
{
    /* set error injection information */
    pBase->ERRIAR  = u32InjectionAddr;
    pBase->ERRIDPR = u32InjectionData;
    pBase->ERRIPPR = u32InjectionParity;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_SetMemErrorInjectionCmd
 * Description   : Enable/disable Memory Error Injection.
 * This function will Enable/disable Memory Error Injection for corresponding error source.
 *
 *END**************************************************************************/
void FlexCAN_SetMemErrorInjectionCmd(FLEXCAN_Type * pBase,
                                     Flexcan_Ip_MemErrInjectionMaskType ErrorMaskType,
                                     boolean IsEnable)
{
    uint32 TempVal = (uint32)ErrorMaskType;

    /* Enable injection */
    if (IsEnable)
    {
        pBase->MECR |= TempVal;
    }
    /* disable injection */
    else
    {
        pBase->MECR &= ~TempVal;
    }
}
#endif /* (FLEXCAN_IP_FEATURE_MEM_ERR_DET_ENABLED == STD_ON) */
#if (FLEXCAN_IP_FEATURE_HAS_MEM_ERR_DET == STD_ON)
/*FUNCTION**********************************************************************
 *
 * Function Name : FlexCAN_DisableMemErrorDetection
 * Description   : Disable Memory Error Detection and Correction in FlexCAN memory.
 * This function will disable Memory Error Detection and Correction feature.
 *
 *END**************************************************************************/
void FlexCAN_DisableMemErrorDetection(FLEXCAN_Type * pBase)
{
    /* Enable error correction configuration register write */
    pBase->CTRL2 |=  FLEXCAN_CTRL2_ECRWRE(1);
    /* Enable error configuration register write */
    pBase->MECR &= ~FLEXCAN_MECR_ECRWRDIS_MASK;
    /* disable put FlexCAN into freeze Mode when no-correctable error is detected */
    pBase->MECR &= ~FLEXCAN_MECR_NCEFAFRZ_MASK;
    /* disable error detection and correction mechanism */
    pBase->MECR |= FLEXCAN_MECR_ECCDIS(1);
    /* disable error report */
    pBase->MECR |= FLEXCAN_MECR_RERRDIS(1);
    /* disable error correction configuration register write. this also disable error configuration register write */
    pBase->CTRL2 &= ~FLEXCAN_CTRL2_ECRWRE_MASK;
}

#endif /* (FLEXCAN_IP_FEATURE_HAS_MEM_ERR_DET == STD_ON) */

#define CAN_43_FLEXCAN_STOP_SEC_CODE
#include "Can_43_FLEXCAN_MemMap.h"

#ifdef __cplusplus
}
#endif

/** @} */
