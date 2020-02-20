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

// #ifdef CONFIG_TI_PACM
/**************************************************************************

    module: cosa_x_cisco_com_mta_dml.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file implementes back-end apis for the COSA Data Model Library

    -------------------------------------------------------------------

    environment:

        platform independent

    -------------------------------------------------------------------

    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/05/2012    initial revision.

**************************************************************************/

#include "ansc_platform.h"
#include "plugin_main_apis.h"
#include "cosa_x_cisco_com_mta_apis.h"
#include "cosa_x_cisco_com_mta_dml.h"
#include "cosa_x_cisco_com_mta_internal.h"

#include "ccsp_base_api.h"
#include "messagebus_interface_helper.h"

extern ULONG g_currentBsUpdate;

#define IS_UPDATE_ALLOWED_IN_DM(paramName, requestorStr) ({                                                                                                  \
    if ( g_currentBsUpdate == DSLH_CWMP_BS_UPDATE_firmware ||                                                                                     \
         (g_currentBsUpdate == DSLH_CWMP_BS_UPDATE_rfcUpdate && !AnscEqualString(requestorStr, BS_SOURCE_RFC_STR, TRUE)))                         \
    {                                                                                                                                             \
       CcspTraceWarning(("Do NOT allow override of param: %s bsUpdate = %d, requestor = %s\n", paramName, g_currentBsUpdate, requestorStr));      \
       return FALSE;                                                                                                                              \
    }                                                                                                                                             \
})

// If the requestor is RFC but the param was previously set by webpa, do not override it.
#define IS_UPDATE_ALLOWED_IN_JSON(paramName, requestorStr, UpdateSource) ({                                                                                \
   if (AnscEqualString(requestorStr, BS_SOURCE_RFC_STR, TRUE) && AnscEqualString(UpdateSource, BS_SOURCE_WEBPA_STR, TRUE))                         \
   {                                                                                                                                               \
      CcspTraceWarning(("Do NOT allow override of param: %s requestor = %d updateSource = %s\n", paramName, g_currentWriteEntity, UpdateSource));  \
      return FALSE;                                                                                                                                \
   }                                                                                                                                               \
})

/***********************************************************************
 IMPORTANT NOTE:

 According to TR69 spec:
 On successful receipt of a SetParameterValues RPC, the CPE MUST apply 
 the changes to all of the specified Parameters atomically. That is, either 
 all of the value changes are applied together, or none of the changes are 
 applied at all. In the latter case, the CPE MUST return a fault response 
 indicating the reason for the failure to apply the changes. 
 
 The CPE MUST NOT apply any of the specified changes without applying all 
 of them.

 In order to set parameter values correctly, the back-end is required to
 hold the updated values until "Validate" and "Commit" are called. Only after
 all the "Validate" passed in different objects, the "Commit" will be called.
 Otherwise, "Rollback" will be called instead.

 The sequence in COSA Data Model will be:

 SetParamBoolValue/SetParamIntValue/SetParamUlongValue/SetParamStringValue
 -- Backup the updated values;

 if( Validate_XXX())
 {
     Commit_XXX();    -- Commit the update all together in the same object
 }
 else
 {
     Rollback_XXX();  -- Remove the update at backup;
 }
 
***********************************************************************/
/***********************************************************************

 APIs for Object:

    X_CISCO_COM_MTA.

    *  X_CISCO_COM_MTA_GetParamBoolValue
    *  X_CISCO_COM_MTA_GetParamIntValue
    *  X_CISCO_COM_MTA_GetParamUlongValue
    *  X_CISCO_COM_MTA_GetParamStringValue
    *  X_CISCO_COM_MTA_SetParamBoolValue
    *  X_CISCO_COM_MTA_SetParamIntValue
    *  X_CISCO_COM_MTA_SetParamUlongValue
    *  X_CISCO_COM_MTA_SetParamStringValue
    *  X_CISCO_COM_MTA_Validate
    *  X_CISCO_COM_MTA_Commit
    *  X_CISCO_COM_MTA_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_MTA_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_MTA_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject     = (PCOSA_DATAMODEL_MTA )g_pCosaBEManager->hMTA;
    PCOSA_MTA_PKTC                  pPktc         = (PCOSA_MTA_PKTC      )&pMyObject->Pktc;
    PCOSA_DML_MTA_LOG               pCfg          = (PCOSA_DML_MTA_LOG   )&pMyObject->MtaLog;
        /* Coverity Issue Fix - CID:57316 : UnInitialised Variable */
	BOOL                       btemp = FALSE;
	
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "pktcMtaDevEnabled", TRUE))
    {
        /* collect value */
        CosaDmlMTAGetPktc(NULL, pPktc);
        *pBool = pPktc->pktcMtaDevEnabled;
        return TRUE;
    }

    if( AnscEqualString(ParamName, "DSXLogEnable", TRUE))
    {
        /* collect value */
        CosaDmlMTAGetDSXLogEnable(NULL, &btemp);
		*pBool = btemp;
        return TRUE;
    }

    if( AnscEqualString(ParamName, "ClearDSXLog", TRUE))
    {
        /* collect value */
        *pBool = FALSE;
        return TRUE;
    }

	if( AnscEqualString(ParamName, "CallSignallingLogEnable", TRUE))
    {
        /* collect value */
		CosaDmlMTAGetCallSignallingLogEnable(NULL, &btemp);
		*pBool = btemp;
        return TRUE;
    }

    if( AnscEqualString(ParamName, "ClearCallSignallingLog", TRUE))
    {
        /* collect value */
        *pBool = FALSE;
        return TRUE;
    }

    if( AnscEqualString(ParamName, "EnableDECTLog", TRUE))
    {
        /* collect value */
        *pBool = pCfg->EnableDECTLog;
        return TRUE;
    }

    if( AnscEqualString(ParamName, "EnableMTALog", TRUE))
    {
        /* collect value */
        *pBool = pCfg->EnableMTALog;
        return TRUE;
    }

    /* AnscTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_MTA_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_MTA_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* AnscTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_MTA_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_MTA_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    COSA_MTA_DHCP_INFO              Info = {0};

    PCOSA_DATAMODEL_MTA             pMyObject     = (PCOSA_DATAMODEL_MTA )g_pCosaBEManager->hMTA;
    PCOSA_MTA_PKTC                  pPktc         = (PCOSA_MTA_PKTC      )&pMyObject->Pktc;

    /* check the parameter name and return the corresponding value */

    if( AnscEqualString(ParamName, "pktcSigDefCallSigTos", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetPktc(NULL, pPktc) != ANSC_STATUS_SUCCESS)
            return FALSE;

        *puLong = pPktc->pktcSigDefCallSigTos;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "pktcSigDefMediaStreamTos", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetPktc(NULL, pPktc) != ANSC_STATUS_SUCCESS)
            return FALSE;

        *puLong = pPktc->pktcSigDefMediaStreamTos;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "pktcMtaDevRealmOrgName", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetPktc(NULL, pPktc) != ANSC_STATUS_SUCCESS)
            return FALSE;

        *puLong = pPktc->pktcMtaDevRealmOrgName;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "pktcMtaDevCmsKerbRealmName", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetPktc(NULL, pPktc) != ANSC_STATUS_SUCCESS)
            return FALSE;

        *puLong = pPktc->pktcMtaDevCmsKerbRealmName;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "pktcMtaDevCmsIpsecCtrl", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetPktc(NULL, pPktc) != ANSC_STATUS_SUCCESS)
            return FALSE;

        *puLong = pPktc->pktcMtaDevCmsIpsecCtrl;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "pktcMtaDevCmsSolicitedKeyTimeout", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetPktc(NULL, pPktc) != ANSC_STATUS_SUCCESS)
            return FALSE;

        *puLong = pPktc->pktcMtaDevCmsSolicitedKeyTimeout;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "pktcMtaDevRealmPkinitGracePeriod", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetPktc(NULL, pPktc) != ANSC_STATUS_SUCCESS)
            return FALSE;

        *puLong = pPktc->pktcMtaDevRealmPkinitGracePeriod;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "IPAddress", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return FALSE;

        *puLong = Info.IPAddress.Value;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "SubnetMask", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return FALSE;

        *puLong = Info.SubnetMask.Value;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "Gateway", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return FALSE;

        *puLong = Info.Gateway.Value;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "LeaseTimeRemaining", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return FALSE;

        *puLong = Info.LeaseTimeRemaining;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "PrimaryDNS", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return FALSE;

        *puLong = Info.PrimaryDNS.Value;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "SecondaryDNS", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return FALSE;

        *puLong = Info.SecondaryDNS.Value;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "PrimaryDHCPServer", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return FALSE;

        *puLong = Info.PrimaryDHCPServer.Value;
        // AnscCopyString(pValue, Info.PrimaryDHCPServer);

        return TRUE;
    }

    if( AnscEqualString(ParamName, "SecondaryDHCPServer", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return FALSE;

        *puLong = Info.SecondaryDHCPServer.Value;
        // AnscCopyString(pValue, Info.SecondaryDHCPServer);

        return TRUE;
    }

	if( AnscEqualString(ParamName, "MTAResetCount", TRUE) )
	{
		if (CosaDmlMtaGetResetCount(NULL,MTA_RESET,puLong) != ANSC_STATUS_SUCCESS)
			return FALSE;

		return TRUE;
	}

	if( AnscEqualString(ParamName, "LineResetCount", TRUE) )
	{
		if (CosaDmlMtaGetResetCount(NULL,LINE_RESET,puLong) != ANSC_STATUS_SUCCESS)
			return FALSE;

		return TRUE;
	}

	if( AnscEqualString(ParamName, "ClearLineStats", TRUE) )
	{
        *puLong = 0;

		return TRUE;
	}

    /* AnscTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_MTA_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
X_CISCO_COM_MTA_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    COSA_MTA_DHCP_INFO              Info = {0};

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "BootFileName", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return -1;

        AnscCopyString(pValue, Info.BootFileName);

        return 0;
    }

    if( AnscEqualString(ParamName, "FQDN", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return -1;

        AnscCopyString(pValue, Info.FQDN);

        return 0;
    }

    if( AnscEqualString(ParamName, "RebindTimeRemaining", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return -1;

        AnscCopyString(pValue, Info.RebindTimeRemaining);

        return 0;
    }

    if( AnscEqualString(ParamName, "RenewTimeRemaining", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return -1;

        AnscCopyString(pValue, Info.RenewTimeRemaining);

        return 0;
    }

    if( AnscEqualString(ParamName, "DHCPOption3", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return -1;

        AnscCopyString(pValue, Info.DHCPOption3);

        return 0;
    }

    if( AnscEqualString(ParamName, "DHCPOption6", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return -1;

        AnscCopyString(pValue, Info.DHCPOption6);

        return 0;
    }

    if( AnscEqualString(ParamName, "DHCPOption7", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return -1;

        AnscCopyString(pValue, Info.DHCPOption7);

        return 0;
    }

    if( AnscEqualString(ParamName, "DHCPOption8", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return -1;

        AnscCopyString(pValue, Info.DHCPOption8);

        return 0;
    }

    if( AnscEqualString(ParamName, "PCVersion", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return -1;

        AnscCopyString(pValue, Info.PCVersion);

        return 0;
    }

    if( AnscEqualString(ParamName, "MACAddress", TRUE) )
    {
        /* collect value */
        if (CosaDmlMTAGetDHCPInfo(NULL, &Info) != ANSC_STATUS_SUCCESS)
            return -1;

        AnscCopyString(pValue, Info.MACAddress);

        return 0;
    }

    /* AnscTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_MTA_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_MTA_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject     = (PCOSA_DATAMODEL_MTA )g_pCosaBEManager->hMTA;
    PCOSA_MTA_PKTC                  pPktc         = (PCOSA_MTA_PKTC      )&pMyObject->Pktc;
    PCOSA_DML_MTA_LOG               pCfg          = (PCOSA_DML_MTA_LOG   )&pMyObject->MtaLog;

    /* check the parameter name and set the corresponding value */

    if( AnscEqualString(ParamName, "pktcMtaDevEnabled", TRUE))
    {
        /* collect value */
        pPktc->pktcMtaDevEnabled = bValue;
        return TRUE;
    }

    if( AnscEqualString(ParamName, "ClearDSXLog", TRUE))
    {
        /* collect value */
        CosaDmlMTAClearDSXLog(NULL,bValue);
        return TRUE;
    }

    if( AnscEqualString(ParamName, "DSXLogEnable", TRUE))
    {
        /* collect value */
        CosaDmlMTASetDSXLogEnable(NULL, bValue);
        return TRUE;
    }

	if( AnscEqualString(ParamName, "CallSignallingLogEnable", TRUE))
    {
		CosaDmlMTASetCallSignallingLogEnable(NULL, bValue);
        return TRUE;
    }

    if( AnscEqualString(ParamName, "ClearCallSignallingLog", TRUE))
    {
         CosaDmlMTAClearCallSignallingLog(NULL,bValue);
         return TRUE;
    }

    if( AnscEqualString(ParamName, "EnableDECTLog", TRUE))
    {
        /* save update to backup */
        pCfg->EnableDECTLog = bValue;
        return TRUE;
    }

    if( AnscEqualString(ParamName, "EnableMTALog", TRUE))
    {
        /* save update to backup */
        pCfg->EnableMTALog = bValue;
        return TRUE;
    }

    /* AnscTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_MTA_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_MTA_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* AnscTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_MTA_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_MTA_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject     = (PCOSA_DATAMODEL_MTA )g_pCosaBEManager->hMTA;
    PCOSA_MTA_PKTC                  pPktc         = (PCOSA_MTA_PKTC      )&pMyObject->Pktc;

    /* check the parameter name and set the corresponding value */

    if( AnscEqualString(ParamName, "pktcSigDefCallSigTos", TRUE) )
    {
        pPktc->pktcSigDefCallSigTos = uValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "pktcSigDefMediaStreamTos", TRUE) )
    {
        pPktc->pktcSigDefMediaStreamTos = uValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "ClearLineStats", TRUE) )
    {
		CosaDmlMtaClearCalls( uValue );

        return TRUE;
    }

    /* AnscTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_MTA_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_MTA_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    /* check the parameter name and set the corresponding value */

    /* AnscTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_MTA_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation. 

                ULONG*                      puLength
                The output length of the param name. 

    return:     TRUE if there's no validation.

**********************************************************************/
BOOL
X_CISCO_COM_MTA_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_MTA_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
X_CISCO_COM_MTA_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject     = (PCOSA_DATAMODEL_MTA )g_pCosaBEManager->hMTA;
    PCOSA_MTA_PKTC                  pPktc         = (PCOSA_MTA_PKTC      )&pMyObject->Pktc;

    CosaDmlMTASetPktc(NULL, pPktc);

    return 0;
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_MTA_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a 
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
X_CISCO_COM_MTA_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject     = (PCOSA_DATAMODEL_MTA )g_pCosaBEManager->hMTA;
    PCOSA_MTA_PKTC                  pPktc         = (PCOSA_MTA_PKTC      )&pMyObject->Pktc;

    CosaDmlMTAGetPktc(NULL, pPktc);

    return 0;
}


/***********************************************************************

 APIs for Object:

    Device.X_CISCO_COM_MTA.LineTable.{i}.

    *  LineTable_GetEntryCount
    *  LineTable_GetEntry
    *  LineTable_GetParamBoolValue
    *  LineTable_GetParamIntValue
    *  LineTable_GetParamUlongValue
    *  LineTable_GetParamStringValue
    *  LineTable_SetParamBoolValue
    *  LineTable_SetParamIntValue
    *  LineTable_SetParamUlongValue
    *  LineTable_SetParamStringValue
    *  LineTable_Validate
    *  LineTable_Commit
    *  LineTable_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        LineTable_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
LineTable_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject     = (PCOSA_DATAMODEL_MTA     )g_pCosaBEManager->hMTA;
    PCOSA_MTA_LINETABLE_INFO        pLineTable    = (PCOSA_MTA_LINETABLE_INFO)pMyObject->pLineTable;
    ULONG                           ulCount;
    
    ulCount = CosaDmlMTALineTableGetNumberOfEntries(NULL);
fprintf(stderr, "### %s %d  ulCount=%ld\n", __func__, __LINE__, ulCount);
    if ( ulCount != 0 )
    {
        pLineTable = AnscAllocateMemory(ulCount * sizeof(COSA_MTA_LINETABLE_INFO));

        memset( pLineTable, 0, ulCount * sizeof(COSA_MTA_LINETABLE_INFO));

ULONG ul=0;
for (ul=0; ul<ulCount; ul++) {
pLineTable[ul].InstanceNumber = ul + 1;
}
        pMyObject->pLineTable = pLineTable;
    }    

    return ulCount;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        LineTable_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
LineTable_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject     = (PCOSA_DATAMODEL_MTA     )g_pCosaBEManager->hMTA;
    PCOSA_MTA_LINETABLE_INFO        pLineTable    = (PCOSA_MTA_LINETABLE_INFO)pMyObject->pLineTable;

    /*CosaDmlMTALineTableGetEntry(NULL, nIndex, &(pLineTable[nIndex]));*/

    pLineTable[nIndex].InstanceNumber = nIndex + 1;

    *pInsNumber = nIndex + 1;

    return &(pLineTable[nIndex]); /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        LineTable_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
LineTable_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "TriggerDiagnostics", TRUE))
    {
        /* collect value */
        *pBool = FALSE;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        LineTable_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
LineTable_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        LineTable_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
LineTable_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_MTA_LINETABLE_INFO        pInfo = (PCOSA_MTA_LINETABLE_INFO)hInsContext;

    ULONG                           nIndex = pInfo->InstanceNumber - 1;

    CosaDmlMTALineTableGetEntry(NULL, nIndex, pInfo);

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "LineNumber", TRUE))
    {
        /* collect value */
        *puLong = pInfo->LineNumber;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "Status", TRUE))
    {
        /* collect value */
        *puLong = pInfo->Status;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "CAPort", TRUE))
    {
        /* collect value */
        *puLong = pInfo->CAPort;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "MWD", TRUE))
    {
        /* collect value */
        *puLong = pInfo->MWD;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "OverCurrentFault", TRUE))
    {
        /* collect value */
        *puLong = pInfo->OverCurrentFault;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        LineTable_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
LineTable_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_MTA_LINETABLE_INFO        pInfo = (PCOSA_MTA_LINETABLE_INFO)hInsContext;

    ULONG                           nIndex = pInfo->InstanceNumber - 1;

    CosaDmlMTALineTableGetEntry(NULL, nIndex, pInfo);

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "HazardousPotential", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pInfo->HazardousPotential);

        return 0;
    }

    if( AnscEqualString(ParamName, "ForeignEMF", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pInfo->ForeignEMF);

        return 0;
    }

    if( AnscEqualString(ParamName, "ResistiveFaults", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pInfo->ResistiveFaults);

        return 0;
    }

    if( AnscEqualString(ParamName, "ReceiverOffHook", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pInfo->ReceiverOffHook);

        return 0;
    }

    if( AnscEqualString(ParamName, "RingerEquivalency", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pInfo->RingerEquivalency);

        return 0;
    }

    if( AnscEqualString(ParamName, "CAName", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pInfo->CAName);

        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        LineTable_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
LineTable_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_MTA_LINETABLE_INFO        pInfo = (PCOSA_MTA_LINETABLE_INFO)hInsContext;

    ULONG                           nIndex = pInfo->InstanceNumber - 1;	
    /* check the parameter name and set the corresponding value */
    // printf("\n\n ==> LineTable_SetParamBoolValue ParamName=%s \n\n", ParamName);
    if( AnscEqualString(ParamName, "TriggerDiagnostics", TRUE))
    {
		// printf("\n\n ==> LineTable_SetParamBoolValue bValue=%d \n", bValue);
        /* save update to backup */
        if ( bValue == TRUE )
        {
             CosaDmlMTATriggerDiagnostics(nIndex);
        }
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        LineTable_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
LineTable_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        LineTable_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
LineTable_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        LineTable_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
LineTable_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        LineTable_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation. 

                ULONG*                      puLength
                The output length of the param name. 

    return:     TRUE if there's no validation.

**********************************************************************/
BOOL
LineTable_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        LineTable_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
LineTable_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        LineTable_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a 
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
LineTable_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    return 0;
}

/***********************************************************************

 APIs for Object:

    Device.X_CISCO_COM_MTA.LineTable.{i}.CALLP.

    *  CALLP_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        CALLP_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
CALLP_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    COSA_MTA_CAPPL                  CALLP = {0};
    PCOSA_MTA_LINETABLE_INFO        pInfo = (PCOSA_MTA_LINETABLE_INFO)hInsContext;
    ULONG                           nIndex = pInfo->InstanceNumber - 1;

    CosaDmlMTALineTableGetEntry(NULL, nIndex, pInfo);

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "LCState", TRUE))
    {
        /* collect value */
        CosaDmlMTAGetCALLP(hInsContext, &CALLP);
        AnscCopyString(pValue, CALLP.LCState);

        return 0;
    }

    if( AnscEqualString(ParamName, "CallPState", TRUE))
    {
        /* collect value */
        CosaDmlMTAGetCALLP(hInsContext, &CALLP);
        AnscCopyString(pValue, CALLP.CallPState);

        return 0;
    }

    if( AnscEqualString(ParamName, "LoopCurrent", TRUE))
    {
        /* collect value */
        CosaDmlMTAGetCALLP(hInsContext, &CALLP);
        AnscCopyString(pValue, CALLP.LoopCurrent);

        return 0;
    }
    
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/***********************************************************************

 APIs for Object:

    Device.X_CISCO_COM_MTA.LineTable.{i}.VQM.

    *  VQM_GetParamBoolValue
    *  VQM_GetParamIntValue
    *  VQM_GetParamUlongValue
    *  VQM_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        VQM_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
VQM_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "ResetStats", TRUE))
    {
        /* collect value */
        *pBool = FALSE;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        VQM_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
VQM_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "ResetStats", TRUE))
    {
        /* save update to backup */
        if ( bValue == TRUE )
        {
            CosaDmlMTAVQMResetStats(NULL);
        }
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        VQM_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation. 

                ULONG*                      puLength
                The output length of the param name. 

    return:     TRUE if there's no validation.

**********************************************************************/
BOOL
VQM_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        VQM_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
VQM_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        VQM_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a 
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
VQM_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    return 0;
}


/***********************************************************************

 APIs for Object:

    Device.X_CISCO_COM_MTA.LineTable.{i}.VQM.Calls.{i}.

    *  Calls_GetEntryCount
    *  Calls_GetEntry
    *  Calls_IsUpdated
    *  Calls_Synchronize
    *  Calls_GetParamUlongValue
    *  Calls_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Calls_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
Calls_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_MTA_LINETABLE_INFO        pMyObject = (PCOSA_MTA_LINETABLE_INFO)hInsContext;
    
    return pMyObject->CallsNumber;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        Calls_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
Calls_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_MTA_LINETABLE_INFO        pMyObject = (PCOSA_MTA_LINETABLE_INFO)hInsContext;

    if (nIndex < pMyObject->CallsNumber)
    {
        *pInsNumber  = nIndex + 1;

        return &pMyObject->pCalls[nIndex];
    }

    return NULL; /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Calls_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/
BOOL
Calls_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_MTA_LINETABLE_INFO        pMyObject = (PCOSA_MTA_LINETABLE_INFO)hInsContext;

    if ( !pMyObject->CallsUpdateTime ) 
    {
        pMyObject->CallsUpdateTime = AnscGetTickInSeconds();

        return TRUE;
    }
    
    if ( pMyObject->CallsUpdateTime >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - MTA_REFRESH_INTERVAL) )
    {
        return FALSE;
    }
    else 
    {
        pMyObject->CallsUpdateTime = AnscGetTickInSeconds();

        return TRUE;
    }
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Calls_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Calls_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_MTA_LINETABLE_INFO        pMyObject = (PCOSA_MTA_LINETABLE_INFO)hInsContext;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;

    if ( pMyObject->pCalls )
    {
        AnscFreeMemory(pMyObject->pCalls);
        pMyObject->pCalls = NULL;
    }
    
    pMyObject->CallsNumber = 0;
    
    returnStatus = CosaDmlMTAGetCalls
        (
            (ANSC_HANDLE)NULL, 
            pMyObject->InstanceNumber,
            &pMyObject->CallsNumber,
            &pMyObject->pCalls
        );

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->pCalls = NULL;
        pMyObject->CallsNumber = 0;
    }
    
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Calls_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Calls_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_MTA_CALLS                 pCalls = (PCOSA_MTA_CALLS)hInsContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "JitterBufferAdaptive", TRUE))
    {
        /* collect value */
        *pBool = pCalls->JitterBufferAdaptive;
        return TRUE;
    }

    if( AnscEqualString(ParamName, "Originator", TRUE))
    {
        /* collect value */
        *pBool = pCalls->Originator;
        return TRUE;
    }

    if( AnscEqualString(ParamName, "RemoteJitterBufferAdaptive", TRUE))
    {
        /* collect value */
        *pBool = pCalls->RemoteJitterBufferAdaptive;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Calls_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Calls_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_MTA_CALLS                 pCalls = (PCOSA_MTA_CALLS)hInsContext;
    
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "RemoteIPAddress", TRUE))
    {
        /* collect value */
        *puLong = pCalls->RemoteIPAddress.Value; 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "CallDuration", TRUE))
    {
        /* collect value */
        *puLong = pCalls->CallDuration; 

        return TRUE;
    }

    

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Calls_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
Calls_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_MTA_CALLS                 pCalls = (PCOSA_MTA_CALLS)hInsContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Codec", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->Codec);

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteCodec", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteCodec);

        return 0;
    }

    if( AnscEqualString(ParamName, "CallEndTime", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->CallEndTime);

        return 0;
    }

    if( AnscEqualString(ParamName, "CallStartTime", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->CallStartTime);

        return 0;
    }

    if( AnscEqualString(ParamName, "CWErrorRate", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->CWErrorRate);

        return 0;
    }

    if( AnscEqualString(ParamName, "PktLossConcealment", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->PktLossConcealment);

        return 0;
    }

    if( AnscEqualString(ParamName, "RemotePktLossConcealment", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemotePktLossConcealment);

        return 0;
    }
	/********************zqiu >>**********************************************/
	if( AnscEqualString(ParamName, "CWErrors", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->CWErrors); 

        return 0;
    }

    if( AnscEqualString(ParamName, "SNR", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->SNR); 

        return 0;
    }

    if( AnscEqualString(ParamName, "MicroReflections", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->MicroReflections); 

        return 0;
    }

    if( AnscEqualString(ParamName, "DownstreamPower", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->DownstreamPower); 

        return 0;
    }

    if( AnscEqualString(ParamName, "UpstreamPower", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->UpstreamPower); 

        return 0;
    }

    if( AnscEqualString(ParamName, "EQIAverage", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->EQIAverage); 

        return 0;

    }
    if( AnscEqualString(ParamName, "EQIMinimum", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->EQIMinimum); 

        return 0;
    }

    if( AnscEqualString(ParamName, "EQIMaximum", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->EQIMaximum); 

        return 0;
    }

    if( AnscEqualString(ParamName, "EQIInstantaneous", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->EQIInstantaneous); 

        return 0;
    }

    if( AnscEqualString(ParamName, "MOS-LQ", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->MOS_LQ); 

        return 0;
    }

    if( AnscEqualString(ParamName, "MOS-CQ", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->MOS_CQ); 

        return 0;
    }

    if( AnscEqualString(ParamName, "EchoReturnLoss", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->EchoReturnLoss); 

        return 0;
    }

    if( AnscEqualString(ParamName, "SignalLevel", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->SignalLevel); 

        return 0;
    }

    if( AnscEqualString(ParamName, "NoiseLevel", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->NoiseLevel); 

        return 0;
    }

    if( AnscEqualString(ParamName, "LossRate", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->LossRate); 

        return 0;
    }

    if( AnscEqualString(ParamName, "DiscardRate", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->DiscardRate); 

        return 0;
    }

    if( AnscEqualString(ParamName, "BurstDensity", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->BurstDensity); 

        return 0;
    }

    if( AnscEqualString(ParamName, "GapDensity", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->GapDensity); 

        return 0;
    }

    if( AnscEqualString(ParamName, "BurstDuration", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->BurstDuration); 

        return 0;
    }

    if( AnscEqualString(ParamName, "GapDuration", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->GapDuration); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RoundTripDelay", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RoundTripDelay); 

        return 0;
    }

    if( AnscEqualString(ParamName, "Gmin", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->Gmin); 

        return 0;

    }

    if( AnscEqualString(ParamName, "RFactor", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RFactor); 

        return 0;
    }

    if( AnscEqualString(ParamName, "ExternalRFactor", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->ExternalRFactor); 

        return 0;
    }

    if( AnscEqualString(ParamName, "JitterBufRate", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->JitterBufRate); 

        return 0;
    }

    if( AnscEqualString(ParamName, "JBNominalDelay", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->JBNominalDelay); 

        return 0;
    }

    if( AnscEqualString(ParamName, "JBMaxDelay", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->JBMaxDelay); 

        return 0;

    }

    if( AnscEqualString(ParamName, "JBAbsMaxDelay", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->JBAbsMaxDelay); 

        return 0;
    }

    if( AnscEqualString(ParamName, "TxPackets", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->TxPackets); 

        return 0;
    }

    if( AnscEqualString(ParamName, "TxOctets", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->TxOctets); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RxPackets", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RxPackets); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RxOctets", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RxOctets); 

        return 0;
    }

    if( AnscEqualString(ParamName, "PacketLoss", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->PacketLoss); 

        return 0;
    }

    if( AnscEqualString(ParamName, "IntervalJitter", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->IntervalJitter); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteIntervalJitter", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteIntervalJitter); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteMOS-LQ", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteMOS_LQ); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteMOS-CQ", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteMOS_CQ); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteEchoReturnLoss", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteEchoReturnLoss);
        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteSignalLevel", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteSignalLevel); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteNoiseLevel", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteNoiseLevel); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteLossRate", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteLossRate); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteDiscardRate", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteDiscardRate); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteBurstDensity", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteBurstDensity); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteGapDensity", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteGapDensity); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteBurstDuration", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteBurstDuration); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteGapDuration", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteGapDuration); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteRoundTripDelay", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteRoundTripDelay); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteGmin", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteGmin); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteRFactor", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteRFactor); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteExternalRFactor", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteExternalRFactor); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteJitterBufRate", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteJitterBufRate); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteJBNominalDelay", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteJBNominalDelay); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteJBMaxDelay", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteJBMaxDelay); 

        return 0;
    }

    if( AnscEqualString(ParamName, "RemoteJBAbsMaxDelay", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pCalls->RemoteJBAbsMaxDelay); 

        return 0;
    }
	/********************zqiu <<**********************************************/
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/***********************************************************************

 APIs for Object:

    Device.X_CISCO_COM_MTA.ServiceClass.{i}.

    *  ServiceClass_GetEntryCount
    *  ServiceClass_GetEntry
    *  ServiceClass_IsUpdated
    *  ServiceClass_Synchronize
    *  ServiceClass_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        ServiceClass_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
ServiceClass_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    
    return pMyObject->ServiceClassNumber;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        ServiceClass_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
ServiceClass_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;

    if (nIndex < pMyObject->ServiceClassNumber)
    {
        *pInsNumber  = nIndex + 1;

        return &pMyObject->pServiceClass[nIndex];
    }

    return NULL; /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        ServiceClass_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/
BOOL
ServiceClass_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;

    if ( !pMyObject->ServiceClassUpdateTime ) 
    {
        pMyObject->ServiceClassUpdateTime = AnscGetTickInSeconds();

        return TRUE;
    }
    
    if ( pMyObject->ServiceClassUpdateTime >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - MTA_REFRESH_INTERVAL) )
    {
        return FALSE;
    }
    else 
    {
        pMyObject->ServiceClassUpdateTime = AnscGetTickInSeconds();

        return TRUE;
    }
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        ServiceClass_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
ServiceClass_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject    = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;

    if ( pMyObject->pServiceClass )
    {
        AnscFreeMemory(pMyObject->pServiceClass);
        pMyObject->pServiceClass = NULL;
    }
    
    pMyObject->ServiceClassNumber = 0;
    
    returnStatus = CosaDmlMTAGetServiceClass
        (
            (ANSC_HANDLE)NULL, 
            &pMyObject->ServiceClassNumber,
            &pMyObject->pServiceClass
        );

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->pServiceClass = NULL;
        pMyObject->ServiceClassNumber = 0;
    }
    
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        ServiceClass_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
ServiceClass_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject     = (PCOSA_DATAMODEL_MTA )g_pCosaBEManager->hMTA;
    PCOSA_MTA_SERVICE_CLASS         pServiceClass = (PCOSA_MTA_SERVICE_CLASS)&pMyObject->pServiceClass;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "ServiceClassName", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pServiceClass->ServiceClassName);

        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/***********************************************************************

 APIs for Object:

    Device.X_CISCO_COM_MTA.ServiceFlow.{i}.

    *  ServiceFlow_GetEntryCount
    *  ServiceFlow_GetEntry
    *  ServiceFlow_IsUpdated
    *  ServiceFlow_Synchronize
    *  ServiceFlow_GetParamUlongValue
    *  ServiceFlow_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        ServiceFlow_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
ServiceFlow_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    
    return pMyObject->ServiceFlowNumber;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        ServiceFlow_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
ServiceFlow_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;

    if (nIndex < pMyObject->ServiceFlowNumber)
    {
        *pInsNumber  = nIndex + 1;

        return &pMyObject->pServiceFlow[nIndex];
    }

    return NULL; /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        ServiceFlow_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/
BOOL
ServiceFlow_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;

    if ( !pMyObject->ServiceFlowUpdateTime ) 
    {
        pMyObject->ServiceFlowUpdateTime = AnscGetTickInSeconds();

        return TRUE;
    }
    
    if ( pMyObject->ServiceFlowUpdateTime >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - MTA_REFRESH_INTERVAL) )
    {
        return FALSE;
    }
    else 
    {
        pMyObject->ServiceFlowUpdateTime = AnscGetTickInSeconds();

        return TRUE;
    }
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        ServiceFlow_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
ServiceFlow_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject    = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;

    if ( pMyObject->pServiceFlow )
    {
        AnscFreeMemory(pMyObject->pServiceFlow);
        pMyObject->pServiceFlow = NULL;
    }
    
    pMyObject->ServiceFlowNumber = 0;
    
    returnStatus = CosaDmlMTAGetServiceFlow
        (
            (ANSC_HANDLE)NULL, 
            &pMyObject->ServiceFlowNumber,
            &pMyObject->pServiceFlow
        );

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->pServiceFlow = NULL;
        pMyObject->ServiceFlowNumber = 0;
    }
    
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        ServiceFlow_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
ServiceFlow_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_MTA_SERVICE_FLOW          pFlow = (PCOSA_MTA_SERVICE_FLOW)hInsContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "DefaultFlow", TRUE))
    {
        /* collect value */
        *pBool = pFlow->DefaultFlow;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        ServiceFlow_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
ServiceFlow_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_MTA_SERVICE_FLOW          pFlow = (PCOSA_MTA_SERVICE_FLOW)hInsContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "TrafficType", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pFlow->TrafficType);

        return 0;
    }

    if( AnscEqualString(ParamName, "Direction", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pFlow->Direction);

        return 0;
    }

    if( AnscEqualString(ParamName, "ServiceClassName", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pFlow->ServiceClassName);

        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        ServiceFlow_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
ServiceFlow_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_MTA_SERVICE_FLOW          pFlow = (PCOSA_MTA_SERVICE_FLOW)hInsContext;
    
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "SFID", TRUE))
    {
        /* collect value */
        *puLong = pFlow->SFID; 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "ScheduleType", TRUE))
    {
        /* collect value */
        *puLong = pFlow->ScheduleType; 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "NomGrantInterval", TRUE))
    {
        /* collect value */
        *puLong = pFlow->NomGrantInterval; 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "UnsolicitGrantSize", TRUE))
    {
        /* collect value */
        *puLong = pFlow->UnsolicitGrantSize; 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "TolGrantJitter", TRUE))
    {
        /* collect value */
        *puLong = pFlow->TolGrantJitter; 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "NomPollInterval", TRUE))
    {
        /* collect value */
        *puLong = pFlow->NomPollInterval; 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "MinReservedPkt", TRUE))
    {
        /* collect value */
        *puLong = pFlow->MinReservedPkt; 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "MaxTrafficRate", TRUE))
    {
        /* collect value */
        *puLong = pFlow->MaxTrafficRate; 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "MinReservedRate", TRUE))
    {
        /* collect value */
        *puLong = pFlow->MinReservedRate; 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "MaxTrafficBurst", TRUE))
    {
        /* collect value */
        *puLong = pFlow->MaxTrafficBurst; 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "NumberOfPackets", TRUE))
    {
        /* collect value */
        *puLong = pFlow->NumberOfPackets; 

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/***********************************************************************

 APIs for Object:

    Device.X_CISCO_COM_MTA.Dect.

    *  Dect_GetParamBoolValue
    *  Dect_GetParamIntValue
    *  Dect_GetParamUlongValue
    *  Dect_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Dect_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Dect_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* collect value */
        CosaDmlMTADectGetEnable(hInsContext, pBool);
        return TRUE;
    }

    if( AnscEqualString(ParamName, "RegistrationMode", TRUE))
    {
        /* collect value */
        CosaDmlMTADectGetRegistrationMode(hInsContext, pBool);
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Dect_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Dect_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Dect_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Dect_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject     = (PCOSA_DATAMODEL_MTA )g_pCosaBEManager->hMTA;
    PCOSA_MTA_DECT                  pDect         = (PCOSA_MTA_DECT      )&pMyObject->Dect;

    /* check the parameter name and return the corresponding value */

    if( AnscEqualString(ParamName, "RegisterDectHandset", TRUE))
    {
        /* collect value */
        *puLong = 0;
        return TRUE;
    }

    if( AnscEqualString(ParamName, "DeregisterDectHandset", TRUE))
    {
        /* collect value */
        *puLong = 0;
        return TRUE;
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Dect_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
Dect_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject     = (PCOSA_DATAMODEL_MTA )g_pCosaBEManager->hMTA;
    PCOSA_MTA_DECT                  pDect         = (PCOSA_MTA_DECT      )&pMyObject->Dect;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "HardwareVersion", TRUE))
    {
        /* collect value */
        CosaDmlMTAGetDect(hInsContext, pDect);
        AnscCopyString(pValue, pDect->HardwareVersion);

        return 0;
    }

    if( AnscEqualString(ParamName, "RFPI", TRUE))
    {
        /* collect value */
        CosaDmlMTAGetDect(hInsContext, pDect);
        AnscCopyString(pValue, pDect->RFPI);

        return 0;
    }

    if( AnscEqualString(ParamName, "SoftwareVersion", TRUE))
    {
        /* collect value */
        CosaDmlMTAGetDect(hInsContext, pDect);
        AnscCopyString(pValue, pDect->SoftwareVersion);

        return 0;
    }
    
    if( AnscEqualString(ParamName, "PIN", TRUE))
    {
        /* collect value */
        CosaDmlMTAGetDectPIN(hInsContext, pDect->PIN);
        AnscCopyString(pValue, pDect->PIN);

        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

BOOL
Dect_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* collect value */
        CosaDmlMTADectSetEnable(hInsContext, bValue);
        return TRUE;
    }

    if( AnscEqualString(ParamName, "RegistrationMode", TRUE))
    {
        /* collect value */
        CosaDmlMTADectSetRegistrationMode(hInsContext, bValue);
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Dect_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Dect_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;

    /* check the parameter name and return the corresponding value */

    if( AnscEqualString(ParamName, "RegisterDectHandset", TRUE))
    {
        CosaDmlMTADectRegisterDectHandset(hInsContext, uValue);
        return TRUE;
    }

    if( AnscEqualString(ParamName, "DeregisterDectHandset", TRUE))
    {
        if ( ANSC_STATUS_SUCCESS == CosaDmlMTADectDeregisterDectHandset(hInsContext, uValue) )
        {
            pMyObject->HandsetsUpdateTime = 0;
        }
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

BOOL
Dect_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "PIN", TRUE))
    {
        CosaDmlMTASetDectPIN(hInsContext, pString);
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Dect_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation. 

                ULONG*                      puLength
                The output length of the param name. 

    return:     TRUE if there's no validation.

**********************************************************************/
BOOL
Dect_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Dect_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Dect_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Dect_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a 
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Dect_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    return 0;
}

/***********************************************************************

 APIs for Object:

    Device.X_CISCO_COM_MTA.Dect.Handsets.{i}

    *  Handsets_GetEntryCount
    *  Handsets_GetEntry
    *  Handsets_GetParamBoolValue
    *  Handsets_GetParamIntValue
    *  Handsets_GetParamUlongValue
    *  Handsets_GetParamStringValue
    *  Handsets_SetParamBoolValue
    *  Handsets_SetParamIntValue
    *  Handsets_SetParamUlongValue
    *  Handsets_SetParamStringValue
    *  Handsets_Validate
    *  Handsets_Commit
    *  Handsets_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Handsets_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
Handsets_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject    = (PCOSA_DATAMODEL_MTA    )g_pCosaBEManager->hMTA;
    return pMyObject->HandsetsNumber;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        Handsets_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
Handsets_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject    = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;

    if (nIndex < pMyObject->HandsetsNumber)
    {
        *pInsNumber  = nIndex + 1;

        return &pMyObject->pHandsets[nIndex];
    }

    return NULL;

}

BOOL
Handsets_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;

    if ( !pMyObject->HandsetsUpdateTime ) 
    {
        pMyObject->HandsetsUpdateTime = AnscGetTickInSeconds();

        return TRUE;
    }
    
    if ( pMyObject->HandsetsUpdateTime >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - HANDSETS_REFRESH_INTERVAL) )
    {
        return FALSE;
    }
    else 
    {
        pMyObject->HandsetsUpdateTime = AnscGetTickInSeconds();

        return TRUE;
    }
}

ULONG
Handsets_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject    = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;

    if ( pMyObject->pHandsets )
    {
        AnscFreeMemory(pMyObject->pHandsets);
        pMyObject->pHandsets = NULL;
    }
    
    pMyObject->HandsetsNumber = 0;
    
    returnStatus = CosaDmlMTAGetHandsets
        (
            (ANSC_HANDLE)NULL, 
            &pMyObject->HandsetsNumber,
            &pMyObject->pHandsets
        );

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->pHandsets = NULL;
        pMyObject->HandsetsNumber = 0;
    }
    
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Handsets_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Handsets_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_MTA_HANDSETS_INFO         pInfo = (PCOSA_MTA_HANDSETS_INFO)hInsContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Status", TRUE))
    {
        /* collect value */
        *pBool = pInfo->Status;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Handsets_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Handsets_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Handsets_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Handsets_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Handsets_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
Handsets_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_MTA_HANDSETS_INFO         pInfo = (PCOSA_MTA_HANDSETS_INFO)hInsContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "LastActiveTime", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pInfo->LastActiveTime);

        return 0;
    }

    if( AnscEqualString(ParamName, "HandsetName", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pInfo->HandsetName);

        return 0;
    }

    if( AnscEqualString(ParamName, "HandsetFirmware", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pInfo->HandsetFirmware);

        return 0;
    }

    if( AnscEqualString(ParamName, "OperatingTN", TRUE))
    {
        /* collect value */
        // At present only TN1 will be assigned to DECT phones
        //AnscCopyString(pValue, pInfo->OperatingTN);
        AnscCopyString(pValue, "TN1");

        return 0;
    }

    if( AnscEqualString(ParamName, "SupportedTN", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pInfo->SupportedTN);

        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Handsets_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Handsets_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Handsets_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Handsets_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Handsets_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Handsets_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Handsets_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Handsets_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_MTA_HANDSETS_INFO         pInfo = (PCOSA_MTA_HANDSETS_INFO)hInsContext;

    /* check the parameter name and set the corresponding value */
    if(AnscEqualString(ParamName, "OperatingTN", TRUE))
    {
        AnscCopyString(pInfo->OperatingTN, pString);

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Handsets_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation. 

                ULONG*                      puLength
                The output length of the param name. 

    return:     TRUE if there's no validation.

**********************************************************************/
BOOL
Handsets_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Handsets_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Handsets_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_MTA_HANDSETS_INFO         pInfo = (PCOSA_MTA_HANDSETS_INFO)hInsContext;

    CosaDmlMTASetHandsets(NULL, pInfo);

    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Handsets_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a 
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Handsets_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    return 0;
}

/***********************************************************************

 APIs for Object:

    Device.X_CISCO_COM_MTA.DSXLog.{i}.

    *  DSXLog_GetEntryCount
    *  DSXLog_GetEntry
    *  DSXLog_IsUpdated
    *  DSXLog_Synchronize
    *  DSXLog_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        DSXLog_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
DSXLog_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    
    return pMyObject->DSXLogNumber;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        DSXLog_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
DSXLog_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;

    if (nIndex < pMyObject->DSXLogNumber)
    {
        *pInsNumber  = nIndex + 1;

        return &pMyObject->pDSXLog[nIndex];
    }

    return NULL; /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        DSXLog_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/
BOOL
DSXLog_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;

    if ( !pMyObject->DSXLogUpdateTime ) 
    {
        pMyObject->DSXLogUpdateTime = AnscGetTickInSeconds();

        return TRUE;
    }
    
    if ( pMyObject->DSXLogUpdateTime >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - MTA_REFRESH_INTERVAL) )
    {
        return FALSE;
    }
    else 
    {
        pMyObject->DSXLogUpdateTime = AnscGetTickInSeconds();

        return TRUE;
    }
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        DSXLog_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
DSXLog_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA             pMyObject    = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;

    if ( pMyObject->pDSXLog )
    {
        AnscFreeMemory(pMyObject->pDSXLog);
        pMyObject->pDSXLog = NULL;
    }
    
    pMyObject->DSXLogNumber = 0;
    
    returnStatus = CosaDmlMTAGetDSXLogs
        (
            (ANSC_HANDLE)NULL, 
            &pMyObject->DSXLogNumber,
            &pMyObject->pDSXLog
        );

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->pDSXLog = NULL;
        pMyObject->DSXLogNumber = 0;
    }
    
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        DSXLog_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
DSXLog_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    //PCOSA_DATAMODEL_MTA             pMyObject = (PCOSA_DATAMODEL_MTA )g_pCosaBEManager->hMTA;
    //PCOSA_MTA_DSXLOG                pDSXLog   = (PCOSA_MTA_DSXLOG)&pMyObject->pDSXLog;
    PCOSA_MTA_DSXLOG                pDSXLog    = (PCOSA_MTA_DSXLOG)hInsContext;
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Time", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pDSXLog->Time);

        return 0;
    }

    if( AnscEqualString(ParamName, "Description", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pDSXLog->Description);

        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        DSXLog_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
DSXLog_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    //PCOSA_DATAMODEL_MTA             pMyObject = (PCOSA_DATAMODEL_MTA )g_pCosaBEManager->hMTA;
    //PCOSA_MTA_DSXLOG                pDSXLog   = (PCOSA_MTA_DSXLOG)&pMyObject->pDSXLog;
    PCOSA_MTA_DSXLOG                pDSXLog    = (PCOSA_MTA_DSXLOG)hInsContext;
    
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "ID", TRUE))
    {
        /* collect value */
        *puLong = pDSXLog->ID; 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "Level", TRUE))
    {
        /* collect value */
        *puLong = pDSXLog->Level; 

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}


/***********************************************************************

 APIs for Object:

    X_CISCO_COM_MLD.Group.{i}.

    *  MTALog_GetEntryCount
    *  MTALog_GetEntry
    *  MTALog_IsUpdated
    *  MTALog_Synchronize
    *  MTALog_GetParamUlongValue
    *  MTALog_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        MTALog_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
MTALog_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA      pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    
    return pMyObject->MtaLogNumber;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        MTALog_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
MTALog_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_MTA      pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;

    if (nIndex < pMyObject->MtaLogNumber)
    {
        *pInsNumber  = nIndex + 1;

        return &pMyObject->pMtaLog[nIndex];
    }

    return NULL; /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MTALog_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/
BOOL
MTALog_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA      pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;

    if ( !pMyObject->MtaLogUpdateTime ) 
    {
        pMyObject->MtaLogUpdateTime = AnscGetTickInSeconds();

        return TRUE;
    }
    
    if ( pMyObject->MtaLogUpdateTime >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - MTA_REFRESH_INTERVAL) )
    {
        return FALSE;
    }
    else 
    {
        pMyObject->MtaLogUpdateTime = AnscGetTickInSeconds();

        return TRUE;
    }
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        MTALog_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
MTALog_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA      pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    ANSC_STATUS                     ret       = ANSC_STATUS_SUCCESS;
    ULONG                           i         = 0;

    if ( pMyObject->pMtaLog )
    {
        for(i=0; i<pMyObject->MtaLogNumber; i++)
        {
            if ( pMyObject->pMtaLog[i].pDescription)
            {
                AnscFreeMemory(pMyObject->pMtaLog[i].pDescription);
                pMyObject->pMtaLog[i].pDescription = NULL;
            }
        }

        AnscFreeMemory(pMyObject->pMtaLog);
        pMyObject->pMtaLog = NULL;
    }
    
    pMyObject->MtaLogNumber = 0;
    
    ret = CosaDmlMtaGetMtaLog
        (
            (ANSC_HANDLE)NULL, 
            &pMyObject->MtaLogNumber,
            &pMyObject->pMtaLog
        );

    if ( ret != ANSC_STATUS_SUCCESS )
    {
        pMyObject->pMtaLog = NULL;
        pMyObject->MtaLogNumber = 0;
    }
    
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MTALog_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MTALog_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DML_MTALOG_FULL           pConf        = (PCOSA_DML_MTALOG_FULL)hInsContext;
    
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Index", TRUE))
    {
        /* collect value */
        *puLong = pConf->Index; 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "EventID", TRUE))
    {
        /* collect value */
        *puLong = pConf->EventID; 

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        MTALog_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 4095 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
MTALog_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_DML_MTALOG_FULL           pConf        = (PCOSA_DML_MTALOG_FULL)hInsContext;

    /* check the parameter name and return the corresponding value */    
    if( AnscEqualString(ParamName, "Description", TRUE))
    {
    
        /* collect value */
        if ( _ansc_strlen(pConf->pDescription) > *pUlSize )
        {
            *pUlSize = _ansc_strlen(pConf->pDescription);
            return 1;
        }
        
        AnscCopyString(pValue, pConf->pDescription);
        return 0;
    }

    if( AnscEqualString(ParamName, "Time", TRUE))
    {
        /* collect value */
        if ( _ansc_strlen(pConf->Time) >= *pUlSize )
        {
            *pUlSize = _ansc_strlen(pConf->Time);
            return 1;
        }
        
        AnscCopyString(pValue, pConf->Time);
        return 0;
    }

    if( AnscEqualString(ParamName, "EventLevel", TRUE))
    {
        /* collect value */
        if ( _ansc_strlen(pConf->EventLevel) >= *pUlSize )
        {
            *pUlSize = _ansc_strlen(pConf->EventLevel);
            return 1;
        }
        
        AnscCopyString(pValue, pConf->EventLevel);
        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}


/***********************************************************************

 APIs for Object:

    X_CISCO_COM_MLD.Group.{i}.

    *  DECTLog_GetEntryCount
    *  DECTLog_GetEntry
    *  DECTLog_IsUpdated
    *  DECTLog_Synchronize
    *  DECTLog_GetParamUlongValue
    *  DECTLog_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        DECTLog_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
DECTLog_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA      pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    
    return pMyObject->DectLogNumber;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        DECTLog_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
DECTLog_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_MTA      pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;

    if (nIndex < pMyObject->DectLogNumber)
    {
        *pInsNumber  = nIndex + 1;

        return &pMyObject->pDectLog[nIndex];
    }

    return NULL; /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        DECTLog_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/
BOOL
DECTLog_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA      pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;

    if ( !pMyObject->DectLogUpdateTime ) 
    {
        pMyObject->DectLogUpdateTime = AnscGetTickInSeconds();

        return TRUE;
    }
    
    if ( pMyObject->DectLogUpdateTime >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - MTA_REFRESH_INTERVAL) )
    {
        return FALSE;
    }
    else 
    {
        pMyObject->DectLogUpdateTime = AnscGetTickInSeconds();

        return TRUE;
    }
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        DECTLog_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
DECTLog_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MTA      pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    ANSC_STATUS                     ret           = ANSC_STATUS_SUCCESS;

    if ( pMyObject->pDectLog )
    {
        AnscFreeMemory(pMyObject->pDectLog);
        pMyObject->pDectLog = NULL;
    }
    
    pMyObject->DectLogNumber = 0;
    
    ret = CosaDmlMtaGetDectLog
        (
            (ANSC_HANDLE)NULL, 
            &pMyObject->DectLogNumber,
            &pMyObject->pDectLog
        );

    if ( ret != ANSC_STATUS_SUCCESS )
    {
        pMyObject->pDectLog = NULL;
        pMyObject->DectLogNumber = 0;
    }
    
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        DECTLog_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
DECTLog_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DML_DECTLOG_FULL          pConf        = (PCOSA_DML_DECTLOG_FULL)hInsContext;
    
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Index", TRUE))
    {
        /* collect value */
        *puLong = pConf->Index; 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "EventID", TRUE))
    {
        /* collect value */
        *puLong = pConf->EventID; 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "EventLevel", TRUE))
    {
        /* collect value */
        *puLong = pConf->EventLevel; 

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        DECTLog_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 4095 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
DECTLog_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_DML_DECTLOG_FULL          pConf        = (PCOSA_DML_DECTLOG_FULL)hInsContext;
    
    /* check the parameter name and return the corresponding value */    
    if( AnscEqualString(ParamName, "Description", TRUE))
    {
        /* collect value */
        if ( _ansc_strlen(pConf->Description) >= *pUlSize )
        {
            *pUlSize = _ansc_strlen(pConf->Description);
            return 1;
        }
        
        AnscCopyString(pValue, pConf->Description);
        return 0;
    }

    if( AnscEqualString(ParamName, "Time", TRUE))
    {
        /* collect value */
        if ( _ansc_strlen(pConf->Time) >= *pUlSize )
        {
            *pUlSize = _ansc_strlen(pConf->Time);
            return 1;
        }
        
        AnscCopyString(pValue, pConf->Time);
        return 0;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}


// #ifdef CONFIG_TI_BBU

/***********************************************************************

 APIs for Object:

    Battery.

    *  Battery_GetParamBoolValue
    *  Battery_GetParamIntValue
    *  Battery_GetParamUlongValue
    *  Battery_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Battery_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Battery_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    // PCOSA_DATAMODEL_MTA     pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    // PCOSA_DML_CM_LOG               pCfg      = &pMyObject->CmLog;

    /* check the parameter name and return the corresponding value */
    if (AnscEqualString(ParamName, "Installed", TRUE))
    {
        if (CosaDmlMtaBatteryGetInstalled(NULL, pBool) != ANSC_STATUS_SUCCESS)
            return FALSE;

        return TRUE;
    }    

    /* AnscTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Battery_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Battery_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* AnscTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Battery_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Battery_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "TotalCapacity", TRUE) )
    {
        /* collect value */
        if (CosaDmlMtaBatteryGetTotalCapacity(NULL, puLong) != ANSC_STATUS_SUCCESS)
            return FALSE;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "ActualCapacity", TRUE) )
    {
        /* collect value */
        if (CosaDmlMtaBatteryGetActualCapacity(NULL, puLong) != ANSC_STATUS_SUCCESS)
            return FALSE;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "RemainingCharge", TRUE) )
    {
        /* collect value */
        if (CosaDmlMtaBatteryGetRemainingCharge(NULL, puLong) != ANSC_STATUS_SUCCESS)
            return FALSE;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "RemainingTime", TRUE) )
    {
        /* collect value */
        if (CosaDmlMtaBatteryGetRemainingTime(NULL, puLong) != ANSC_STATUS_SUCCESS)
            return FALSE;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "NumberofCycles", TRUE) )
    {
        /* collect value */
        if (CosaDmlMtaBatteryGetNumberofCycles(NULL, puLong) != ANSC_STATUS_SUCCESS)
            return FALSE;

        return TRUE;
    }


    /* AnscTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Battery_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
Battery_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_DATAMODEL_MTA      pMTA   = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    PCOSA_DML_BATTERY_INFO          pInfo = (PCOSA_DML_BATTERY_INFO)&pMTA->MtaBatteryInfo;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "PowerStatus", TRUE) )
    {
        /* collect value */
        if (CosaDmlMtaBatteryGetPowerStatus(NULL, pValue, pUlSize) != ANSC_STATUS_SUCCESS)
            return -1;

        return 0;
    }

    if( AnscEqualString(ParamName, "Condition", TRUE) )
    {
        /* collect value */
        if (CosaDmlMtaBatteryGetCondition(NULL, pValue, pUlSize) != ANSC_STATUS_SUCCESS)
            return -1;

        return 0;
    }

    if( AnscEqualString(ParamName, "Status", TRUE) )
    {
        /* collect value */
        if (CosaDmlMtaBatteryGetStatus(NULL, pValue, pUlSize) != ANSC_STATUS_SUCCESS)
            return -1;

        return 0;
    }

    if( AnscEqualString(ParamName, "Life", TRUE) )
    {
        /* collect value */
        if (CosaDmlMtaBatteryGetLife(NULL, pValue, pUlSize) != ANSC_STATUS_SUCCESS)
            return -1;

        return 0;
    }

    if( AnscEqualString(ParamName, "ModelNumber", TRUE) )
    {
        /* collect value */
        if (CosaDmlMtaBatteryGetInfo(NULL, pInfo) != ANSC_STATUS_SUCCESS)
            return -1;

        _ansc_strcpy(pValue, pInfo->ModelNumber);
        *pUlSize = _ansc_strlen(pValue);

        return 0;
    }

    if( AnscEqualString(ParamName, "SerialNumber", TRUE) )
    {
        /* collect value */
        if (CosaDmlMtaBatteryGetInfo(NULL, pInfo) != ANSC_STATUS_SUCCESS)
            return -1;
        
        _ansc_strcpy(pValue, pInfo->SerialNumber);
        *pUlSize = _ansc_strlen(pValue);

        return 0;
    }

    if( AnscEqualString(ParamName, "PartNumber", TRUE) )
    {
        /* collect value */
        if (CosaDmlMtaBatteryGetInfo(NULL, pInfo) != ANSC_STATUS_SUCCESS)
            return -1;
        
        _ansc_strcpy(pValue, pInfo->PartNumber);
        *pUlSize = _ansc_strlen(pValue);

        return 0;
    }

    if( AnscEqualString(ParamName, "ChargerFirmwareRevision", TRUE) )
    {
        /* collect value */
        if (CosaDmlMtaBatteryGetInfo(NULL, pInfo) != ANSC_STATUS_SUCCESS)
            return -1;
        
        _ansc_strcpy(pValue, pInfo->ChargerFirmwareRevision);
        *pUlSize = _ansc_strlen(pValue);

        return 0;
    }

    /* AnscTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

// #endif /* CONFIG_TI_BBU */

// #endif /* CONFIG_TI_PACM */

/***********************************************************************

 APIs for Object:

    X_RDKCENTRAL_COM_MTA.

    *  X_RDKCENTRAL_COM_MTA_GetParamUlongValue
    *  X_RDKCENTRAL_COM_MTA_GetParamStringValue

***********************************************************************/

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_RDKCENTRAL_COM_MTA_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_RDKCENTRAL_COM_MTA_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
	/* check the parameter name and return the corresponding value */

	if( AnscEqualString(ParamName, "Ipv4DhcpStatus", TRUE) )
	{
		ULONG Ipv4DhcpStatus = 0, 
		      Ipv6DhcpStatus = 0;
		
        CosaDmlMtaGetDhcpStatus( &Ipv4DhcpStatus, &Ipv6DhcpStatus );
		*puLong = Ipv4DhcpStatus;
		return TRUE;
	}

	if( AnscEqualString(ParamName, "Ipv6DhcpStatus", TRUE) )
	{
		ULONG Ipv4DhcpStatus = 0, 
		      Ipv6DhcpStatus = 0;
		
        CosaDmlMtaGetDhcpStatus( &Ipv4DhcpStatus, &Ipv6DhcpStatus );
		*puLong = Ipv6DhcpStatus;
		return TRUE;
	}

	if( AnscEqualString(ParamName, "ConfigFileStatus", TRUE) )
	{
        CosaDmlMtaGetConfigFileStatus( puLong );
		return TRUE;
	}

    /* AnscTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_RDKCENTRAL_COM_MTA_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
X_RDKCENTRAL_COM_MTA_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
	/* check the parameter name and return the corresponding value */

    if( AnscEqualString(ParamName, "LineRegisterStatus", TRUE) )
    {
		CHAR acLineRegisterStatus[ 128 ] = { 0 };

        /* collect value */

		CosaDmlMtaGetLineRegisterStatus( acLineRegisterStatus );
        if ( AnscSizeOfString( acLineRegisterStatus ) < *pUlSize)
        {
            AnscCopyString(pValue, acLineRegisterStatus );
		    return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString( acLineRegisterStatus )+1;
            return 1;
        }
    }

    /* AnscTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

#define BS_SOURCE_WEBPA_STR "webpa"
#define BS_SOURCE_RFC_STR "rfc"
#define  PARTNER_ID_LEN  64

char * getRequestorString()
{
   switch(g_currentWriteEntity)
   {
      case 0x0A: //CCSP_COMPONENT_ID_WebPA from webpa_internal.h(parodus2ccsp)
      case 0x0B: //CCSP_COMPONENT_ID_XPC
         return BS_SOURCE_WEBPA_STR;

      case 0x08: //DSLH_MPA_ACCESS_CONTROL_CLI
      case 0x10: //DSLH_MPA_ACCESS_CONTROL_CLIENTTOOL
         return BS_SOURCE_RFC_STR;

      default:
         return "unknown";
   }
}

char * getTime()
{
    time_t timer;
    static char buffer[50];
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, 50, "%Y-%m-%d %H:%M:%S ", tm_info);
    return buffer;
}

/********************
X_RDKCENTRAL-COM_EthernetWAN_MTA
***************/
BOOL EthernetWAN_MTA_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         pInt
    )
{
        PCOSA_DATAMODEL_MTA             pMyObject     = (PCOSA_DATAMODEL_MTA )g_pCosaBEManager->hMTA;
        if( AnscEqualString(ParamName, "StartupIPMode", TRUE))
        {
           char isEthEnabled[64]={'\0'};
	   char buff[8] = {'\0'};
           char * requestorStr = getRequestorString();
           char * currentTime = getTime();

           IS_UPDATE_ALLOWED_IN_DM(ParamName, requestorStr);
           IS_UPDATE_ALLOWED_IN_JSON(ParamName, requestorStr, pMyObject->pmtaprovinfo->StartupIPMode.UpdateSource);

          if( (0 == syscfg_get( NULL, "eth_wan_enabled", isEthEnabled, sizeof(isEthEnabled))) &&
                      ((isEthEnabled[0] != '\0') && (strncmp(isEthEnabled, "true", strlen("true")) == 0)))
            {
		       snprintf(buff,sizeof(buff),"%d",pInt);
               if (syscfg_set(NULL, "StartupIPMode", buff) != 0) {
                   AnscTraceWarning(("syscfg_set failed\n"));
               } else {
                   if (syscfg_commit() != 0) {
                      AnscTraceWarning(("syscfg_commit failed\n"));
                   }
               }
               CosaDmlMTASetStartUpIpMode(pMyObject->pmtaprovinfo, pInt);

               memset( pMyObject->pmtaprovinfo->StartupIPMode.UpdateSource, 0, sizeof( pMyObject->pmtaprovinfo->StartupIPMode.UpdateSource ));
               AnscCopyString( pMyObject->pmtaprovinfo->StartupIPMode.UpdateSource, requestorStr );

               char PartnerID[PARTNER_ID_LEN] = {0};
               if((CCSP_SUCCESS == getPartnerId(PartnerID) ) && (PartnerID[ 0 ] != '\0') )
                   UpdateJsonParam("Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.StartupIPMode",PartnerID, buff, requestorStr, currentTime);

               // set startup ip mode
               CosaSetMTAHal(pMyObject->pmtaprovinfo);
               return TRUE;
            } 
        }
    return FALSE;
}

BOOL
EthernetWAN_MTA_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */
    PCOSA_DATAMODEL_MTA             pMyObject     = (PCOSA_DATAMODEL_MTA )g_pCosaBEManager->hMTA;
    if( AnscEqualString(ParamName, "StartupIPMode", TRUE))
    {
        /* collect value */
            char isEthEnabled[64]={'\0'};

          if( (0 == syscfg_get( NULL, "eth_wan_enabled", isEthEnabled, sizeof(isEthEnabled))) &&
                      ((isEthEnabled[0] != '\0') && (strncmp(isEthEnabled, "true", strlen("true")) == 0)))
            {
               *pInt = pMyObject->pmtaprovinfo->StartupIPMode.ActiveValue;
                return TRUE;
            } else {
                AnscTraceWarning(("Eth_wan not enabled : Invalid request\n"));
                return FALSE;
             }
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

ULONG
EthernetWAN_MTA_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
  /* check the parameter name and return the corresponding value */
    PCOSA_DATAMODEL_MTA             pMyObject     = (PCOSA_DATAMODEL_MTA )g_pCosaBEManager->hMTA;
    char isEthEnabled[64]={'\0'};
    if( (0 == syscfg_get( NULL, "eth_wan_enabled", isEthEnabled, sizeof(isEthEnabled))) &&
                      ((isEthEnabled[0] != '\0') && (strncmp(isEthEnabled, "true", strlen("true")) == 0)))   
    {         
           if( AnscEqualString(ParamName, "IPv4PrimaryDhcpServerOptions", TRUE))
            {
                AnscCopyString(pValue,pMyObject->pmtaprovinfo->IPv4PrimaryDhcpServerOptions.ActiveValue);
                return 0;
            }

            if( AnscEqualString(ParamName, "IPv4SecondaryDhcpServerOptions", TRUE))
            {
               AnscCopyString(pValue,pMyObject->pmtaprovinfo->IPv4SecondaryDhcpServerOptions.ActiveValue);
                return 0;
            }
            if( AnscEqualString(ParamName, "IPv6PrimaryDhcpServerOptions", TRUE))
            {
               AnscCopyString(pValue,pMyObject->pmtaprovinfo->IPv6PrimaryDhcpServerOptions.ActiveValue);
                return 0;
            }

            if( AnscEqualString(ParamName, "IPv6SecondaryDhcpServerOptions", TRUE))
            {
               AnscCopyString(pValue,pMyObject->pmtaprovinfo->IPv6SecondaryDhcpServerOptions.ActiveValue);
                return 0;
            }
 
   } else {
                AnscTraceWarning(("Eth_wan not enabled : Invalid request\n"));
                return -1;
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}


BOOL
EthernetWAN_MTA_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
          MTA_IP_TYPE_TR ip_type;
          PCOSA_DATAMODEL_MTA             pMyObject     = (PCOSA_DATAMODEL_MTA )g_pCosaBEManager->hMTA;
          char isEthEnabled[64]={'\0'};
          char * requestorStr = getRequestorString();
          char * currentTime = getTime();

          IS_UPDATE_ALLOWED_IN_DM(ParamName, requestorStr);

          if( (0 == syscfg_get( NULL, "eth_wan_enabled", isEthEnabled, sizeof(isEthEnabled))) &&
                      ((isEthEnabled[0] != '\0') && (strncmp(isEthEnabled, "true", strlen("true")) == 0)))
            {
               ip_type = MTA_IPV4_TR;
               if( AnscEqualString(ParamName, "IPv4PrimaryDhcpServerOptions", TRUE))
                  {
                      IS_UPDATE_ALLOWED_IN_JSON(ParamName, requestorStr, pMyObject->pmtaprovinfo->IPv4PrimaryDhcpServerOptions.UpdateSource);

                      if (syscfg_set(NULL, "IPv4PrimaryDhcpServerOptions", pString) != 0) 
                          {
                              AnscTraceWarning(("syscfg_set failed\n"));
                          } else {
                             if (syscfg_commit() != 0) 
                                {
                                   AnscTraceWarning(("syscfg_commit failed\n"));
                                }
                          }
                      CosaDmlMTASetPrimaryDhcpServerOptions(pMyObject->pmtaprovinfo, pString, ip_type);

                      memset( pMyObject->pmtaprovinfo->IPv4PrimaryDhcpServerOptions.UpdateSource, 0, sizeof( pMyObject->pmtaprovinfo->IPv4PrimaryDhcpServerOptions.UpdateSource ));
                      AnscCopyString( pMyObject->pmtaprovinfo->IPv4PrimaryDhcpServerOptions.UpdateSource, requestorStr );

                      char PartnerID[PARTNER_ID_LEN] = {0};
                      if((CCSP_SUCCESS == getPartnerId(PartnerID) ) && (PartnerID[ 0 ] != '\0') )
                          UpdateJsonParam("Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.IPv4PrimaryDhcpServerOptions",PartnerID, pString, requestorStr, currentTime);

                      return TRUE;
                  }
              if( AnscEqualString(ParamName, "IPv4SecondaryDhcpServerOptions", TRUE))
                  {
                      IS_UPDATE_ALLOWED_IN_JSON(ParamName, requestorStr, pMyObject->pmtaprovinfo->IPv4SecondaryDhcpServerOptions.UpdateSource);

                      if (syscfg_set(NULL, "IPv4SecondaryDhcpServerOptions", pString) != 0) 
                          {
                              AnscTraceWarning(("syscfg_set failed\n"));
                          } else {
                             if (syscfg_commit() != 0) 
                                {
                                   AnscTraceWarning(("syscfg_commit failed\n"));
                                }
                          }
                      CosaDmlMTASetSecondaryDhcpServerOptions(pMyObject->pmtaprovinfo, pString, ip_type);

                      memset( pMyObject->pmtaprovinfo->IPv4SecondaryDhcpServerOptions.UpdateSource, 0, sizeof( pMyObject->pmtaprovinfo->IPv4SecondaryDhcpServerOptions.UpdateSource ));
                      AnscCopyString( pMyObject->pmtaprovinfo->IPv4SecondaryDhcpServerOptions.UpdateSource, requestorStr );

                      char PartnerID[PARTNER_ID_LEN] = {0};
                      if((CCSP_SUCCESS == getPartnerId(PartnerID) ) && (PartnerID[ 0 ] != '\0') )
                          UpdateJsonParam("Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.IPv4SecondaryDhcpServerOptions",PartnerID, pString, requestorStr, currentTime);

                      return TRUE;
                  }

              ip_type = MTA_IPV6_TR;

              if( AnscEqualString(ParamName, "IPv6PrimaryDhcpServerOptions", TRUE))
                  {
                      IS_UPDATE_ALLOWED_IN_JSON(ParamName, requestorStr, pMyObject->pmtaprovinfo->IPv6PrimaryDhcpServerOptions.UpdateSource);

                      if (syscfg_set(NULL, "IPv6PrimaryDhcpServerOptions", pString) != 0)
                          {
                              AnscTraceWarning(("syscfg_set failed\n"));
                          } else {
                             if (syscfg_commit() != 0)
                                {
                                   AnscTraceWarning(("syscfg_commit failed\n"));
                                }
                          }
                      CosaDmlMTASetPrimaryDhcpServerOptions(pMyObject->pmtaprovinfo, pString, ip_type);

                      memset( pMyObject->pmtaprovinfo->IPv6PrimaryDhcpServerOptions.UpdateSource, 0, sizeof( pMyObject->pmtaprovinfo->IPv6PrimaryDhcpServerOptions.UpdateSource ));
                      AnscCopyString( pMyObject->pmtaprovinfo->IPv6PrimaryDhcpServerOptions.UpdateSource, requestorStr );

                      char PartnerID[PARTNER_ID_LEN] = {0};
                      if((CCSP_SUCCESS == getPartnerId(PartnerID) ) && (PartnerID[ 0 ] != '\0') )
                          UpdateJsonParam("Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.IPv6PrimaryDhcpServerOptions",PartnerID, pString, requestorStr, currentTime);

                      return TRUE;
                  }
               if( AnscEqualString(ParamName, "IPv6SecondaryDhcpServerOptions", TRUE))
                  {
                      IS_UPDATE_ALLOWED_IN_JSON(ParamName, requestorStr, pMyObject->pmtaprovinfo->IPv6SecondaryDhcpServerOptions.UpdateSource);

                      if (syscfg_set(NULL, "IPv6SecondaryDhcpServerOptions", pString) != 0)
                          {
                              AnscTraceWarning(("syscfg_set failed\n"));
                          } else {
                             if (syscfg_commit() != 0)
                                {
                                   AnscTraceWarning(("syscfg_commit failed\n"));
                                }
                          }
                      CosaDmlMTASetSecondaryDhcpServerOptions(pMyObject->pmtaprovinfo, pString, ip_type);

                      memset( pMyObject->pmtaprovinfo->IPv6SecondaryDhcpServerOptions.UpdateSource, 0, sizeof( pMyObject->pmtaprovinfo->IPv6SecondaryDhcpServerOptions.UpdateSource ));
                      AnscCopyString( pMyObject->pmtaprovinfo->IPv6SecondaryDhcpServerOptions.UpdateSource, requestorStr );

                      char PartnerID[PARTNER_ID_LEN] = {0};
                      if((CCSP_SUCCESS == getPartnerId(PartnerID) ) && (PartnerID[ 0 ] != '\0') )
                          UpdateJsonParam("Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.IPv6SecondaryDhcpServerOptions",PartnerID, pString, requestorStr, currentTime);

                      return TRUE;
                  }

           } else {
                AnscTraceWarning(("Eth_wan not enabled : Invalid request\n"));
                return FALSE;
           }

}

BOOL
X_RDKCENTRAL_COM_MTA_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
	if( AnscEqualString(ParamName, "pktcMtaDevResetNow", TRUE) )
	{
		*pBool = false;
		return TRUE;
	}

	return FALSE;
}

BOOL
X_RDKCENTRAL_COM_MTA_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{

	if( AnscEqualString(ParamName, "pktcMtaDevResetNow", TRUE) )
	{
		if(pBool == true)
		{
			if(	ANSC_STATUS_SUCCESS == CosaDmlMtaResetNow(pBool)	)
			{
				AnscTraceWarning(("MTA Module is Reset ParamName: '%s'  Value: '%d' \n",ParamName,pBool));
				CosaDmlMtaProvisioningStatusGet();
				return TRUE;
			}
			else
			{
				AnscTraceWarning(("MTA Module is not Reset ParamName: '%s'  Value: '%d' \n",ParamName,pBool));
				return FALSE;
			}
		}
		else
		{
			AnscTraceWarning(("parameter '%s' value: '%d' \n", ParamName,pBool));
			return TRUE;
		}
	}
	return FALSE;

}

