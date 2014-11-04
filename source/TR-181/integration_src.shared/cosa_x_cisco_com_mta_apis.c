/**********************************************************************
   Copyright [2014] [Cisco Systems, Inc.]
 
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
 
       http://www.apache.org/licenses/LICENSE-2.0
 
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**********************************************************************/


/**************************************************************************

    module: cosa_x_cisco_com_mta_api.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    copyright:

        Cisco Systems, Inc.
        All Rights Reserved.

    -------------------------------------------------------------------

    description:

        This file implementes back-end apis for the COSA Data Model Library

        *  CosaDmlMTAInit
    -------------------------------------------------------------------

    environment:

        platform independent

    -------------------------------------------------------------------

    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/11/2011    initial revision.

**************************************************************************/

//!!!  This code assumes that all data structures are the SAME in middle-layer APIs and HAL layer APIs
//!!!  So it uses casting from one to the other
#include "cosa_x_cisco_com_mta_apis.h"
#include "mta_hal.h"

// #include "cosa_x_cisco_com_mta_internal.h"

ANSC_STATUS
CosaDmlMTAInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    )
{
    // PCOSA_DATAMODEL_MTA      pMyObject    = (PCOSA_DATAMODEL_MTA)phContext;

    if ( mta_hal_InitDB() == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTAGetDHCPInfo
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_DHCP_INFO         pInfo
    )
{

    if ( mta_hal_GetDHCPInfo((PMTAMGMT_MTA_DHCP_INFO)pInfo) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTAGetPktc
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_PKTC              pPktc
    )
{
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMTASetPktc
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_PKTC              pPktc
    )
{
    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlMTALineTableGetNumberOfEntries
    (
        ANSC_HANDLE                 hContext
    )
{
    return mta_hal_LineTableGetNumberOfEntries();
}

ANSC_STATUS
CosaDmlMTALineTableGetEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        PCOSA_MTA_LINETABLE_INFO    pEntry
    )

{

    if ( mta_hal_LineTableGetEntry(ulIndex, (PMTAMGMT_MTA_LINETABLE_INFO)pEntry) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTATriggerDiagnostics
    (
        ULONG                         nIndex
    )
{ 
    if ( mta_hal_TriggerDiagnostics(nIndex) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE; 
}

ANSC_STATUS
CosaDmlMTAGetServiceClass
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_MTA_SERVICE_CLASS     *ppCfg
    )
{
    *pulCount = 0;

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMTAGetServiceFlow
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_MTA_SERVICE_FLOW      *ppCfg
    )
{
    *pulCount = 0;
    PMTAMGMT_MTA_SERVICE_FLOW pInfo = NULL;
    if ( mta_hal_GetServiceFlow(pulCount, &pInfo) != RETURN_OK )
        return ANSC_STATUS_FAILURE;

    if (*pulCount > 0) {
        if( (*ppCfg = (PCOSA_MTA_SERVICE_FLOW)AnscAllocateMemory(sizeof(MTAMGMT_MTA_SERVICE_FLOW)*(*pulCount))) == NULL )
        {
            AnscTraceWarning(("AllocateMemory error %s, %d\n", __FUNCTION__, __LINE__));
            return ANSC_STATUS_FAILURE;  
        }
        AnscCopyMemory(*ppCfg, pInfo, sizeof(MTAMGMT_MTA_SERVICE_FLOW)*(*pulCount));
        free(pInfo);
    }
    return ANSC_STATUS_SUCCESS;

    /*
    if ( mta_hal_GetServiceFlow(pulCount, (PMTAMGMT_MTA_SERVICE_FLOW *)ppCfg) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
    */
}

ANSC_STATUS
CosaDmlMTADectGetEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     *pBool
    )
{
    if (mta_hal_DectGetEnable(pBool) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}
        
ANSC_STATUS
CosaDmlMTADectSetEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     bBool
    )
{
     if (mta_hal_DectSetEnable(bBool) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;

}

ANSC_STATUS
CosaDmlMTADectGetRegistrationMode
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     *pBool
    )
{
     if (mta_hal_DectGetRegistrationMode(pBool) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;

}

ANSC_STATUS
CosaDmlMTADectSetRegistrationMode
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     bBool
    )
{
     if (mta_hal_DectSetRegistrationMode(bBool) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;

}

ANSC_STATUS
CosaDmlMTADectRegisterDectHandset
    (
        ANSC_HANDLE                 hContext,
        ULONG                       uValue    
    )
{
    fprintf(stderr, "%s is not implemented!\n", __FUNCTION__);

    return ANSC_STATUS_SUCCESS;
}

CosaDmlMTADectDeregisterDectHandset
    (
        ANSC_HANDLE                 hContext,
        ULONG                       uValue
    )
{

     if (mta_hal_DectDeregisterDectHandset(uValue) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;


}

ANSC_STATUS
CosaDmlMTAGetDect
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_DECT              pDect
    )
{


     if (mta_hal_GetDect(pDect) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;


}

ANSC_STATUS
CosaDmlMTAGetDectPIN
    (
        ANSC_HANDLE                 hContext,
        char                        *pPINString
    )
{

     if (mta_hal_GetDectPIN(pPINString) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;


}

ANSC_STATUS
CosaDmlMTASetDectPIN
    (
        ANSC_HANDLE                 hContext,
        char                        *pPINString
    )
{


     if (mta_hal_SetDectPIN(pPINString) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;

}

ANSC_STATUS
CosaDmlMTAGetHandsets
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_MTA_HANDSETS_INFO     *ppHandsets
    )
{
    PMTAMGMT_MTA_HANDSETS_INFO pInfo = NULL;
    if ( mta_hal_GetHandsets( pulCount, &pInfo) != RETURN_OK )
        return ANSC_STATUS_FAILURE;

    if (*pulCount > 0) {
        if( (*ppHandsets = AnscAllocateMemory(DECT_MAX_HANDSETS * sizeof(MTAMGMT_MTA_HANDSETS_INFO))) == NULL )
        {
            AnscTraceWarning(("AllocateMemory error %s, %d\n", __FUNCTION__, __LINE__));
            return ANSC_STATUS_FAILURE;  
        }
        AnscCopyMemory(*ppHandsets, pInfo, sizeof(MTAMGMT_MTA_HANDSETS_INFO)*DECT_MAX_HANDSETS);
        free(pInfo);
    }
    return ANSC_STATUS_SUCCESS;

}

ANSC_STATUS
CosaDmlMTASetHandsets
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_HANDSETS_INFO     pHandsets
    )
{
   fprintf(stderr, "%s is not implemented!\n", __FUNCTION__);

   return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMTAVQMResetStats
    (
        ANSC_HANDLE                 hContext
    )
{
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMTAGetCalls
    (
        ANSC_HANDLE                 hContext,
        ULONG                       InstanceNumber, /* LineTable's instance number */
        PULONG                      pulCount,
        PCOSA_MTA_CALLS             *ppCfg
    )
{ 
    *pulCount = 0;
    PMTAMGMT_MTA_CALLS pInfo = NULL;
    if ( mta_hal_GetCalls(InstanceNumber, pulCount, &pInfo) != RETURN_OK )
        return ANSC_STATUS_FAILURE;

    if (*pulCount > 0) {
        if( (*ppCfg = (PCOSA_MTA_CALLS)AnscAllocateMemory(sizeof(MTAMGMT_MTA_CALLS)*(*pulCount))) == NULL )
        {
            AnscTraceWarning(("AllocateMemory error %s, %d\n", __FUNCTION__, __LINE__));
            return ANSC_STATUS_FAILURE;  
        }
        AnscCopyMemory(*ppCfg, pInfo, sizeof(MTAMGMT_MTA_CALLS)*(*pulCount));
        free(pInfo);
    }
    return ANSC_STATUS_SUCCESS;
    
    /*
    if ( mta_hal_GetCalls(InstanceNumber, (PMTAMGMT_MTA_CALLS *)ppCfg, pulCount) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else 
        return ANSC_STATUS_FAILURE;
    */
}

ANSC_STATUS
CosaDmlMTAGetCALLP
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_CAPPL             pCallp
    )
{
    PCOSA_MTA_LINETABLE_INFO pLineTable = (PCOSA_MTA_LINETABLE_INFO)hContext;

    if ( mta_hal_GetCALLP(pLineTable->LineNumber, (PMTAMGMT_MTA_CALLP)pCallp) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTAGetDSXLogs
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_MTA_DSXLOG            *ppDSXLog
    )
{
    *pulCount = 0;
    PMTAMGMT_MTA_DSXLOG pInfo = NULL;
    if ( mta_hal_GetDSXLogs(pulCount, &pInfo) != RETURN_OK )
        return ANSC_STATUS_FAILURE;

    if (*pulCount > 0) {
        if( (*ppDSXLog = (PCOSA_MTA_DSXLOG)AnscAllocateMemory(sizeof(MTAMGMT_MTA_DSXLOG)*(*pulCount))) == NULL )
        {
            AnscTraceWarning(("AllocateMemory error %s, %d\n", __FUNCTION__, __LINE__));
            return ANSC_STATUS_FAILURE;  
        }
        AnscCopyMemory(*ppDSXLog, pInfo, sizeof(MTAMGMT_MTA_DSXLOG)*(*pulCount));
        free(pInfo);
    }
    return ANSC_STATUS_SUCCESS;

    /*
    if ( mta_hal_GetDSXLogs(pulCount, (PMTAMGMT_MTA_DSXLOG *)ppDSXLog) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
    */
}

ANSC_STATUS
CosaDmlMTAGetDSXLogEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     *pBool
    )
{
    if ( mta_hal_GetDSXLogEnable(pBool) == RETURN_OK ) 
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTASetDSXLogEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     Bool
    )
{
    if ( mta_hal_SetDSXLogEnable(Bool) == RETURN_OK ) 
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTAClearDSXLog
    (
        ANSC_HANDLE                 hContext
    )
{
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMtaBatteryGetInstalled
    (
        ANSC_HANDLE                 hContext,
        PBOOL                       pValue
    )
{
    if ( mta_hal_BatteryGetInstalled(pValue) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetTotalCapacity
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    )
{
    if ( mta_hal_BatteryGetTotalCapacity(pValue) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}
    
ANSC_STATUS
CosaDmlMtaBatteryGetActualCapacity
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    )
{
    if ( mta_hal_BatteryGetActualCapacity(pValue) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetRemainingCharge
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    )
{
    if ( mta_hal_BatteryGetRemainingCharge(pValue) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetRemainingTime
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    )
{
    if ( mta_hal_BatteryGetRemainingTime(pValue) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetNumberofCycles
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    )
{
    if ( mta_hal_BatteryGetNumberofCycles(pValue) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetPowerStatus
    (
        ANSC_HANDLE                 hContext,
        PCHAR                       pValue,
        PULONG                      pSize
    )
{
    if ( mta_hal_BatteryGetPowerStatus(pValue, pSize) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetCondition
    (
        ANSC_HANDLE                 hContext,
        PCHAR                       pValue,
        PULONG                      pSize
    )
{
    if ( mta_hal_BatteryGetCondition(pValue, pSize) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetStatus
    (
        ANSC_HANDLE                 hContext,
        PCHAR                       pValue,
        PULONG                      pSize
    )
{
    if ( mta_hal_BatteryGetStatus(pValue, pSize) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetLife
    (
        ANSC_HANDLE                 hContext,
        PCHAR                       pValue,
        PULONG                      pSize
    )
{
    if ( mta_hal_BatteryGetLife(pValue, pSize) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetInfo
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_BATTERY_INFO      pInfo
    )
{
    if ( mta_hal_BatteryGetInfo((PMTAMGMT_MTA_BATTERY_INFO)pInfo) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else 
        return ANSC_STATUS_FAILURE;
}


ANSC_STATUS
CosaDmlMtaGetMtaLog
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_DML_MTALOG_FULL       *ppConf        
    )    
{
    *pulCount = 0;
    PMTAMGMT_MTA_MTALOG_FULL pInfo = NULL;
    if ( mta_hal_GetMtaLog(pulCount, &pInfo) != RETURN_OK )
        return ANSC_STATUS_FAILURE;

    if (*pulCount > 0) {
        if( (*ppConf = (PCOSA_DML_MTALOG_FULL)AnscAllocateMemory(sizeof(MTAMGMT_MTA_MTALOG_FULL)*(*pulCount))) == NULL )
        {
            AnscTraceWarning(("AllocateMemory error %s, %d\n", __FUNCTION__, __LINE__));
            return ANSC_STATUS_FAILURE;  
        }
        AnscCopyMemory(*ppConf, pInfo, sizeof(MTAMGMT_MTA_MTALOG_FULL)*(*pulCount));
        {
            int i;
            for (i=0; i<*pulCount; i++) {
                if (pInfo[i].pDescription) {
                    (*ppConf)[i].pDescription = AnscCloneString(pInfo[i].pDescription);
                    free(pInfo[i].pDescription);
                }
                else (*ppConf)[i].pDescription = NULL;
            }
        }
        free(pInfo);
    }
    return ANSC_STATUS_SUCCESS;

/*
    if ( mta_hal_GetMtaLog(pulCount, (PMTAMGMT_MTA_MTALOG_FULL *)ppConf) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
*/

}

ANSC_STATUS
CosaDmlMtaGetDectLog
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_DML_DECTLOG_FULL      *ppConf        
    )    
{
    *pulCount = 0;
    //*ppConf = (PCOSA_DML_DECTLOG_FULL)AnscAllocateMemory( sizeof(DectLog) );

    //AnscCopyMemory(*ppConf, &DectLog, sizeof(DectLog) );

    return ANSC_STATUS_SUCCESS;
}

// #endif /* CONFIG_TI_PACM */
