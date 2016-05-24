/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2015 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

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

#define PARTNERS_INFO_FILE              "/nvram/partners_defaults.json"
#define BOOTSTRAP_INFO_FILE             "/nvram/bootstrap.json"

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
        ANSC_HANDLE                 hContext,
		BOOLEAN                     ClearLog
    )
{
	
	if ( mta_hal_ClearDSXLog(ClearLog) == RETURN_OK ) 
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

// CallSignalling apis
ANSC_STATUS
CosaDmlMTAGetCallSignallingLogEnable
	(
        ANSC_HANDLE                 hContext,
        BOOLEAN                     *pBool
    )
{
	if ( mta_hal_GetCallSignallingLogEnable(pBool) == RETURN_OK ) 
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTASetCallSignallingLogEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     Bool
    )
{
    if ( mta_hal_SetCallSignallingLogEnable(Bool) == RETURN_OK ) 
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTAClearCallSignallingLog
    (
        ANSC_HANDLE                 hContext,
		BOOLEAN                     ClearLog
    )
{
    if ( mta_hal_ClearCallSignallingLog(ClearLog) == RETURN_OK ) 
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

// CallSignalling apis ends

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

ANSC_STATUS
CosaDmlMtaGetResetCount
    (
        ANSC_HANDLE                 hContext,
		MTA_RESET_TYPE              type,
        ULONG                       *pValue
    )
{
	switch(type)
	{
		case MTA_RESET:{
				mta_hal_Get_MTAResetCount(pValue);
			 }
			break;
		case LINE_RESET:{
				mta_hal_Get_LineResetCount(pValue);
			}
			break;
		default:{
			 AnscTraceWarning(("Invalide type %s, %d\n", __FUNCTION__, __LINE__));
			}
	}
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMtaClearCalls
    (
		ULONG InstanceNumber
    )
{
	mta_hal_ClearCalls( InstanceNumber );

    return ANSC_STATUS_SUCCESS;
}
// #endif /* CONFIG_TI_PACM */

ANSC_STATUS
CosaDmlMtaGetDhcpStatus
    (
		PULONG pIpv4DhcpStatus,
		PULONG pIpv6DhcpStatus
    )
{
	MTAMGMT_MTA_STATUS output_pIpv4status = MTA_ERROR;
	MTAMGMT_MTA_STATUS output_pIpv6status = MTA_ERROR;

	*pIpv4DhcpStatus = MTA_ERROR;
	*pIpv6DhcpStatus = MTA_ERROR;

#ifdef _CBR_PRODUCT_REQ_
	if ( RETURN_OK == mta_hal_getDhcpStatus( &output_pIpv4status, &output_pIpv6status ) )
	{
		*pIpv4DhcpStatus = output_pIpv4status;
		*pIpv6DhcpStatus = output_pIpv6status;
	}
#endif /* _CBR_PRODUCT_REQ_ */

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMtaGetConfigFileStatus
    (
		PULONG pConfigFileStatus
    )
{
	MTAMGMT_MTA_STATUS output_status = MTA_ERROR;

	*pConfigFileStatus = MTA_ERROR;

#ifdef _CBR_PRODUCT_REQ_
	if ( RETURN_OK == mta_hal_getConfigFileStatus( &output_status))
	{
		*pConfigFileStatus = output_status;
	}
#endif /* _CBR_PRODUCT_REQ_ */

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMtaGetLineRegisterStatus
    (
		PCHAR pcLineRegisterStatus
    )
{
#ifdef _CBR_PRODUCT_REQ_
#define max_number_of_line 8
	MTAMGMT_MTA_STATUS output_status_array[ max_number_of_line ] = { 0 };
	char 			   aoutput_status_array_string [ 4 ][ 32 ] = { "Init", "Start", "Complete", "Error" };

	if ( RETURN_OK == mta_hal_getLineRegisterStatus( output_status_array, max_number_of_line ) )
	{
		sprintf( pcLineRegisterStatus, "%s,%s,%s,%s,%s,%s,%s,%s", aoutput_status_array_string[ output_status_array[ 0 ] ],
																  aoutput_status_array_string[ output_status_array[ 1 ] ],
																  aoutput_status_array_string[ output_status_array[ 2 ] ],
																  aoutput_status_array_string[ output_status_array[ 3 ] ],
																  aoutput_status_array_string[ output_status_array[ 4 ] ],
																  aoutput_status_array_string[ output_status_array[ 5 ] ],
																  aoutput_status_array_string[ output_status_array[ 6 ] ],
																  aoutput_status_array_string[ output_status_array[ 7 ] ] );
	}
	else
#endif /* _CBR_PRODUCT_REQ_ */
	{	
		sprintf( pcLineRegisterStatus,"" );
	}

    return ANSC_STATUS_SUCCESS;
}


ANSC_STATUS
CosaDmlMTASetStartUpIpMode
    (
        PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro, 
        INT                         bInt
    )
{
     pmtaethpro->StartupIPMode.ActiveValue =bInt;
     return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMTASetPrimaryDhcpServerOptions
    (
       PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro,       
       CHAR                        buf[64],
       MTA_IP_TYPE_TR              type
    )
{
    if(type == MTA_IPV4_TR)
        {
           AnscCopyMemory(pmtaethpro->IPv4PrimaryDhcpServerOptions.ActiveValue, buf, sizeof(pmtaethpro->IPv4PrimaryDhcpServerOptions.ActiveValue));
        } else if(type == MTA_IPV6_TR) {
           AnscCopyMemory(pmtaethpro->IPv6PrimaryDhcpServerOptions.ActiveValue, buf, sizeof(pmtaethpro->IPv6PrimaryDhcpServerOptions.ActiveValue));
        } 
  return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMTASetSecondaryDhcpServerOptions
    (
        PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro,  
        CHAR                        buf[64],
        MTA_IP_TYPE_TR                 type
    )
{
    if(type == MTA_IPV4_TR)
        {
            AnscCopyMemory(pmtaethpro->IPv4SecondaryDhcpServerOptions.ActiveValue, buf, sizeof(pmtaethpro->IPv4SecondaryDhcpServerOptions.ActiveValue));
        }  else if (type == MTA_IPV6_TR) {
            AnscCopyMemory(pmtaethpro->IPv6SecondaryDhcpServerOptions.ActiveValue, buf, sizeof(pmtaethpro->IPv6SecondaryDhcpServerOptions.ActiveValue));
        }
     return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMtaResetNow
    (
		BOOLEAN	*pBool
    )
{

#ifdef _CBR_PRODUCT_REQ_
	if( RETURN_OK == mta_hal_devResetNow(pBool) )
#else
	if( /*RETURN_OK == mta_hal_devResetNow(pBool)*/ 1 )
#endif /* _CBR_PRODUCT_REQ_ */
	{
		AnscTraceWarning(("MTA reset is successful \n"));
		return ANSC_STATUS_SUCCESS;
	}

	return ANSC_STATUS_FAILURE;

}

void MtaProvisioningStatusGetFunc()
{
	unsigned int ProvisioningStatus;
	char counter=0;
	pthread_detach(pthread_self());

#ifdef _CBR_PRODUCT_REQ_
	while(1)
	{
		if(	RETURN_OK != mta_hal_getMtaOperationalStatus(&ProvisioningStatus) )
		{
			AnscTraceWarning(("mta_hal_getMtaOperationalStatus Fail \n"));
			return;
		}

		switch(ProvisioningStatus)
		{
			case COSA_MTA_INIT:
				AnscTraceWarning(("MTA provisioning init after reset  \n"));
				break;

			case COSA_MTA_START:
				AnscTraceWarning(("MTA provisioning is in progress after reset \n"));
				break;

			case COSA_MTA_COMPLETE:
				AnscTraceWarning(("MTA is operational after reset \n"));
				pthread_exit(NULL);
				return;

			case COSA_MTA_ERROR:
				AnscTraceWarning(("MTA provisioning failed after reset"));
				break;

			default :
					break;
		}
		sleep(5);
		counter = counter+5;

		/*This thread is run for 180Sec*/
		if(counter == 180)
		{
			counter = 0;
			pthread_exit(NULL);
			return;
		}
	}
	pthread_exit(NULL);
#endif /* _CBR_PRODUCT_REQ_ */
}



void CosaDmlMtaProvisioningStatusGet()
{
	int res;
	pthread_t MtaProvisioningStatusGetThread;

	res = pthread_create(&MtaProvisioningStatusGetThread, NULL, MtaProvisioningStatusGetFunc, "MtaProvisioningStatusGetFunc");
	if(res != 0)
	{
		CcspTraceError(("Create MtaProvisioningStatusGetThread error %d\n", res));
		return;
	}

}

#define PARTNER_ID_LEN 64
ANSC_STATUS fillCurrentPartnerId
        (
                char*                       pValue,
        PULONG                      pulSize
    )
{
        char buf[PARTNER_ID_LEN];
        memset(buf, 0, sizeof(buf));
    if(ANSC_STATUS_SUCCESS == syscfg_get( NULL, "PartnerID", buf, sizeof(buf)))
    {
                if( buf != NULL )
                {
                        strncpy(pValue ,buf,strlen(buf));
            *pulSize = AnscSizeOfString(pValue);
                        return ANSC_STATUS_SUCCESS;
                }
                else
                        return ANSC_STATUS_FAILURE;
    }
        else
                return ANSC_STATUS_FAILURE;

}

void FillParamUpdateSource(cJSON *partnerObj, char *key, char *paramUpdateSource)
{
    cJSON *paramObj = cJSON_GetObjectItem( partnerObj, key);
    if ( paramObj != NULL )
    {
        char *valuestr = NULL;
        cJSON *paramObjVal = cJSON_GetObjectItem(paramObj, "UpdateSource");
        if (paramObjVal)
            valuestr = paramObjVal->valuestring;
        if (valuestr != NULL)
        {
            AnscCopyString(paramUpdateSource, valuestr);
            valuestr = NULL;
        }
        else
        {
            CcspTraceWarning(("%s - %s UpdateSource is NULL\n", __FUNCTION__, key ));
        }
    }
    else
    {
        CcspTraceWarning(("%s - %s Object is NULL\n", __FUNCTION__, key ));
    }
}

void FillPartnerIDJournal
    (
        cJSON *json ,
        char *partnerID ,
        PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro
    )
{
                cJSON *partnerObj = cJSON_GetObjectItem( json, partnerID );
                if( partnerObj != NULL)
                {
                      FillParamUpdateSource(partnerObj, "Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.StartupIPMode", &pmtaethpro->StartupIPMode.UpdateSource);
                      FillParamUpdateSource(partnerObj, "Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.IPv4PrimaryDhcpServerOptions", &pmtaethpro->IPv4PrimaryDhcpServerOptions.UpdateSource);
                      FillParamUpdateSource(partnerObj, "Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.IPv4SecondaryDhcpServerOptions", &pmtaethpro->IPv4SecondaryDhcpServerOptions.UpdateSource);
                      FillParamUpdateSource(partnerObj, "Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.IPv6PrimaryDhcpServerOptions", &pmtaethpro->IPv6PrimaryDhcpServerOptions.UpdateSource);
                      FillParamUpdateSource(partnerObj, "Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.IPv6SecondaryDhcpServerOptions", &pmtaethpro->IPv6SecondaryDhcpServerOptions.UpdateSource);
                }
                else
                {
                      CcspTraceWarning(("%s - PARTNER ID OBJECT Value is NULL\n", __FUNCTION__ ));
                }
}

//Get the UpdateSource info from /nvram/bootstrap.json. This is needed to know for override precedence rules in set handlers
ANSC_STATUS
CosaMTAInitializeEthWanProvJournal
    (
        PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro
    )
{
        char *data = NULL;
        char buf[64] = {0};
        cJSON *json = NULL;
        FILE *fileRead = NULL;
        char PartnerID[PARTNER_ID_LEN] = {0};
        char cmd[512] = {0};
        ULONG size = PARTNER_ID_LEN - 1;
        int len;
        if (!pmtaethpro)
        {
                CcspTraceWarning(("%s-%d : NULL param\n" , __FUNCTION__, __LINE__ ));
                return ANSC_STATUS_FAILURE;
        }

        if (access(BOOTSTRAP_INFO_FILE, F_OK) != 0)
        {
                return ANSC_STATUS_FAILURE;
        }

         fileRead = fopen( BOOTSTRAP_INFO_FILE, "r" );
         if( fileRead == NULL )
         {
                 CcspTraceWarning(("%s-%d : Error in opening JSON file\n" , __FUNCTION__, __LINE__ ));
                 return ANSC_STATUS_FAILURE;
         }

         fseek( fileRead, 0, SEEK_END );
         len = ftell( fileRead );
         fseek( fileRead, 0, SEEK_SET );
         data = ( char* )malloc( sizeof(char) * (len + 1) );
         if (data != NULL)
         {
                memset( data, 0, ( sizeof(char) * (len + 1) ));
                fread( data, 1, len, fileRead );
         }
         else
         {
                 CcspTraceWarning(("%s-%d : Memory allocation failed \n", __FUNCTION__, __LINE__));
                 fclose( fileRead );
                 return ANSC_STATUS_FAILURE;
         }

         fclose( fileRead );

         if ( data == NULL )
         {
                CcspTraceWarning(("%s-%d : fileRead failed \n", __FUNCTION__, __LINE__));
                return ANSC_STATUS_FAILURE;
         }
         else if ( strlen(data) != 0)
         {
                 json = cJSON_Parse( data );
                 if( !json )
                 {
                         CcspTraceWarning((  "%s : json file parser error : [%d]\n", __FUNCTION__,__LINE__));
                         free(data);
                         return ANSC_STATUS_FAILURE;
                 }
                 else
                 {
                         if(ANSC_STATUS_SUCCESS == fillCurrentPartnerId(PartnerID, &size))
                         {
                                if ( PartnerID[0] != '\0' )
                                {
                                        CcspTraceWarning(("%s : Partner = %s \n", __FUNCTION__, PartnerID));
                                        FillPartnerIDJournal(json, PartnerID, pmtaethpro);
                                }
                                else
                                {
                                        CcspTraceWarning(( "Reading Deafult PartnerID Values \n" ));
                                        strcpy(PartnerID, "comcast");
                                        FillPartnerIDJournal(json, PartnerID, pmtaethpro);
                                }
                        }
                        else{
                                CcspTraceWarning(("Failed to get Partner ID\n"));
                        }
                        cJSON_Delete(json);
                }
                free(data);
                data = NULL;
         }
         else
         {
                CcspTraceWarning(("BOOTSTRAP_INFO_FILE %s is empty\n", BOOTSTRAP_INFO_FILE));
                return ANSC_STATUS_FAILURE;
         }
         return ANSC_STATUS_SUCCESS;
}

static int writeToJson(char *data, char *file)
{
    FILE *fp;
    fp = fopen(file, "w");
    if (fp == NULL)
    {
        CcspTraceWarning(("%s : %d Failed to open file %s\n", __FUNCTION__,__LINE__,file));
        return -1;
    }

    fwrite(data, strlen(data), 1, fp);
    fclose(fp);
    return 0;
}

ANSC_STATUS UpdateJsonParamLegacy
	(
		char*                       pKey,
		char*			PartnerId,
		char*			pValue
    )
{
	cJSON *partnerObj = NULL;
	cJSON *json = NULL;
	FILE *fileRead = NULL;
	char * cJsonOut = NULL;
	char* data = NULL;
	 int len ;
	 int configUpdateStatus = -1;
	 fileRead = fopen( PARTNERS_INFO_FILE, "r" );
	 if( fileRead == NULL ) 
	 {
		 CcspTraceWarning(("%s-%d : Error in opening JSON file\n" , __FUNCTION__, __LINE__ ));
		 return ANSC_STATUS_FAILURE;
	 }
	 
	 fseek( fileRead, 0, SEEK_END );
	 len = ftell( fileRead );
	 fseek( fileRead, 0, SEEK_SET );
	 data = ( char* )malloc( sizeof(char) * (len + 1) );
	 if (data != NULL) 
	 {
		memset( data, 0, ( sizeof(char) * (len + 1) ));
	 	fread( data, 1, len, fileRead );
	 } 
	 else 
	 {
		 CcspTraceWarning(("%s-%d : Memory allocation failed \n", __FUNCTION__, __LINE__));
		 fclose( fileRead );
		 return ANSC_STATUS_FAILURE;
	 }
	 
	 fclose( fileRead );
	 if ( data == NULL )
	 {
		CcspTraceWarning(("%s-%d : fileRead failed \n", __FUNCTION__, __LINE__));
		return ANSC_STATUS_FAILURE;
	 }
	 else if ( strlen(data) != 0)
	 {
		 json = cJSON_Parse( data );
		 if( !json ) 
		 {
			 CcspTraceWarning((  "%s : json file parser error : [%d]\n", __FUNCTION__,__LINE__));
			 free(data);
			 return ANSC_STATUS_FAILURE;
		 } 
		 else
		 {
			 partnerObj = cJSON_GetObjectItem( json, PartnerId );
			 if ( NULL != partnerObj)
			 {
				 if (NULL != cJSON_GetObjectItem( partnerObj, pKey) )
				 {
					 cJSON_ReplaceItemInObject(partnerObj, pKey, cJSON_CreateString(pValue));
					 cJsonOut = cJSON_Print(json);
					 CcspTraceWarning(( "Updated json content is %s\n", cJsonOut));
					 configUpdateStatus = writeToJson(cJsonOut, PARTNERS_INFO_FILE);
					 if ( !configUpdateStatus)
					 {
						 CcspTraceWarning(( "Updated Value for %s partner\n",PartnerId));
						 CcspTraceWarning(( "Param:%s - Value:%s\n",pKey,pValue));
					 }
					 else
				 	{
						 CcspTraceWarning(( "Failed to update value for %s partner\n",PartnerId));
						 CcspTraceWarning(( "Param:%s\n",pKey));
			 			 cJSON_Delete(json);
						 return ANSC_STATUS_FAILURE;						
				 	}
				 }
				else
			 	{
			 		CcspTraceWarning(("%s - OBJECT  Value is NULL %s\n", pKey,__FUNCTION__ ));
			 		cJSON_Delete(json);
			 		return ANSC_STATUS_FAILURE;
			 	}
			 
			 }
			 else
			 {
			 	CcspTraceWarning(("%s - PARTNER ID OBJECT Value is NULL\n", __FUNCTION__ ));
			 	cJSON_Delete(json);
			 	return ANSC_STATUS_FAILURE;
			 }
			cJSON_Delete(json);
		 }
	  }
	  else
	  {
		CcspTraceWarning(("PARTNERS_INFO_FILE %s is empty\n", PARTNERS_INFO_FILE));
		return ANSC_STATUS_FAILURE;
	  }
	 return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS UpdateJsonParam
        (
                char*                       pKey,
                char*                   PartnerId,
                char*                   pValue,
                char*                   pSource,
                char*                   pCurrentTime
    )
{
        cJSON *partnerObj = NULL;
        cJSON *json = NULL;
        FILE *fileRead = NULL;
        char * cJsonOut = NULL;
        char* data = NULL;
         int len ;
         int configUpdateStatus = -1;
         fileRead = fopen( BOOTSTRAP_INFO_FILE, "r" );
         if( fileRead == NULL )
         {
                 CcspTraceWarning(("%s-%d : Error in opening JSON file\n" , __FUNCTION__, __LINE__ ));
                 return ANSC_STATUS_FAILURE;
         }

         fseek( fileRead, 0, SEEK_END );
         len = ftell( fileRead );
         fseek( fileRead, 0, SEEK_SET );
         data = ( char* )malloc( sizeof(char) * (len + 1) );
         if (data != NULL)
         {
                memset( data, 0, ( sizeof(char) * (len + 1) ));
                fread( data, 1, len, fileRead );
         }
         else
         {
                 CcspTraceWarning(("%s-%d : Memory allocation failed \n", __FUNCTION__, __LINE__));
                 fclose( fileRead );
                 return ANSC_STATUS_FAILURE;
         }

         fclose( fileRead );
         if ( data == NULL )
         {
                CcspTraceWarning(("%s-%d : fileRead failed \n", __FUNCTION__, __LINE__));
                return ANSC_STATUS_FAILURE;
         }
         else if ( strlen(data) != 0)
         {
                 json = cJSON_Parse( data );
                 if( !json )
                 {
                         CcspTraceWarning((  "%s : json file parser error : [%d]\n", __FUNCTION__,__LINE__));
                         free(data);
                         return ANSC_STATUS_FAILURE;
                 }
                 else
                 {
                         partnerObj = cJSON_GetObjectItem( json, PartnerId );
                         if ( NULL != partnerObj)
                         {
                                 cJSON *paramObj = cJSON_GetObjectItem( partnerObj, pKey);
                                 if (NULL != paramObj )
                                 {
                                         cJSON_ReplaceItemInObject(paramObj, "ActiveValue", cJSON_CreateString(pValue));
                                         cJSON_ReplaceItemInObject(paramObj, "UpdateTime", cJSON_CreateString(pCurrentTime));
                                         cJSON_ReplaceItemInObject(paramObj, "UpdateSource", cJSON_CreateString(pSource));

                                         cJsonOut = cJSON_Print(json);
                                         CcspTraceWarning(( "Updated json content is %s\n", cJsonOut));
                                         configUpdateStatus = writeToJson(cJsonOut, BOOTSTRAP_INFO_FILE);
                                         if ( !configUpdateStatus)
                                         {
                                                 CcspTraceWarning(( "Bootstrap config update: %s, %s, %s, %s \n", pKey, pValue, PartnerId, pSource));
                                         }
                                         else
                                        {
                                                 CcspTraceWarning(( "Failed to update value for %s partner\n",PartnerId));
                                                 CcspTraceWarning(( "Param:%s\n",pKey));
                                                 cJSON_Delete(json);
                                                 return ANSC_STATUS_FAILURE;
                                        }
                                 }
                                else
                                {
                                        CcspTraceWarning(("%s - OBJECT  Value is NULL %s\n", pKey,__FUNCTION__ ));
                                        cJSON_Delete(json);
                                        return ANSC_STATUS_FAILURE;
                                }

                         }
                         else
                         {
                                CcspTraceWarning(("%s - PARTNER ID OBJECT Value is NULL\n", __FUNCTION__ ));
                                cJSON_Delete(json);
                                return ANSC_STATUS_FAILURE;
                         }
                        cJSON_Delete(json);
                 }
          }
          else
          {
                CcspTraceWarning(("BOOTSTRAP_INFO_FILE %s is empty\n", BOOTSTRAP_INFO_FILE));
                return ANSC_STATUS_FAILURE;
          }

          //Also update in the legacy file /nvram/partners_defaults.json for firmware roll over purposes.
          UpdateJsonParamLegacy(pKey, PartnerId, pValue);

         return ANSC_STATUS_SUCCESS;
}
