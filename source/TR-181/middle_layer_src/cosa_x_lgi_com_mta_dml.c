
#include "cosa_x_lgi_com_mta_apis.h"
#include "cosa_x_lgi_com_mta_dml.h"
#include "cosa_x_lgi_com_mta_internal.h"

/***********************************************************************

 APIs for Object:

    X_LGI_COM_MTA.

***********************************************************************/

ULONG X_LGI_COM_MTA_GetParamStringValue (ANSC_HANDLE hInsContext, char* ParamName, char* pValue, ULONG* pUlSize)
{
    if (strcmp(ParamName, "PacketCableRegion") == 0)
    {
        ULONG value = 0;
        if (CosaDmlMTAGetPacketCableRegion(NULL, &value) != ANSC_STATUS_SUCCESS)
            return -1;

        if (value == 0)
            AnscCopyString(pValue, "northAmerican");
        else
            AnscCopyString(pValue, "european");

        return 0;
    }

    return -1;
}

BOOL X_LGI_COM_MTA_GetParamBoolValue (ANSC_HANDLE hInsContext, char* ParamName, BOOL* pBool)
{
    PCOSA_DATAMODEL_LGI_MTA pMyObject = (PCOSA_DATAMODEL_LGI_MTA) g_pCosaBEManager->hLgiMTA;

    if (strcmp(ParamName, "SpeedUpMTA-RFLossDetection") == 0)
    {
        *pBool = pMyObject->RFLossDetection;
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_MTA_SetParamBoolValue (ANSC_HANDLE hInsContext, char* ParamName, BOOL bValue)
{
    PCOSA_DATAMODEL_LGI_MTA pMyObject = (PCOSA_DATAMODEL_LGI_MTA) g_pCosaBEManager->hLgiMTA;

    if (strcmp(ParamName, "SpeedUpMTA-RFLossDetection") == 0)
    {
        pMyObject->RFLossDetection = bValue;
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_MTA_Validate (ANSC_HANDLE hInsContext, char* pReturnParamName, ULONG* puLength)
{
    return TRUE;
}

ULONG X_LGI_COM_MTA_Commit (ANSC_HANDLE hInsContext)
{
    PCOSA_DATAMODEL_LGI_MTA pMyObject = (PCOSA_DATAMODEL_LGI_MTA) g_pCosaBEManager->hLgiMTA;

    CosaDmlMTABasicInfoSetSpeedUpMTARFLossDetection(pMyObject->RFLossDetection);

    return 0;
}

ULONG X_LGI_COM_MTA_Rollback (ANSC_HANDLE hInsContext)
{
    PCOSA_DATAMODEL_LGI_MTA pMyObject = (PCOSA_DATAMODEL_LGI_MTA) g_pCosaBEManager->hLgiMTA;

    pMyObject->RFLossDetection = CosaDmlMTABasicInfoGetSpeedUpMTARFLossDetection(NULL);

    return 0;
}

/***********************************************************************

 APIs for Object:

    Device.X_LGI_COM_MTA.LineInfo.{i}.

***********************************************************************/

ULONG X_LGI_COM_MTA_LineInfo_GetEntryCount (ANSC_HANDLE hInsContext)
{
    PCOSA_DATAMODEL_LGI_MTA pMyObject = (PCOSA_DATAMODEL_LGI_MTA) g_pCosaBEManager->hLgiMTA;
    PCOSA_LGI_MTA_LINEINFO pLineInfo = (PCOSA_LGI_MTA_LINEINFO) pMyObject->pLineInfo;
    ULONG ulCount;

    ulCount = CosaDmlMTALineInfoGetNumberOfEntries(NULL);

    if (ulCount != 0 && pLineInfo == NULL)
    {
        pLineInfo = AnscAllocateMemory(sizeof(COSA_LGI_MTA_LINEINFO) * ulCount);

        if(pLineInfo != NULL)
        {
            ULONG ul = 0;
            for (ul = 0; ul < ulCount; ul++)
            {
                pLineInfo[ul].InstanceNumber = ul + 1;
            }
            pMyObject->pLineInfo = pLineInfo;
        }
    }

    return ulCount;
}

ANSC_HANDLE X_LGI_COM_MTA_LineInfo_GetEntry (ANSC_HANDLE hInsContext, ULONG nIndex, ULONG* pInsNumber)
{
    PCOSA_DATAMODEL_LGI_MTA         pMyObject   = (PCOSA_DATAMODEL_LGI_MTA)g_pCosaBEManager->hLgiMTA;
    PCOSA_LGI_MTA_LINEINFO          pLineInfo   = (PCOSA_LGI_MTA_LINEINFO)pMyObject->pLineInfo;

    pLineInfo[nIndex].InstanceNumber = nIndex + 1;

    *pInsNumber = nIndex + 1;

    return &(pLineInfo[nIndex]); /* return the handle */
}

BOOL X_LGI_COM_MTA_LineInfo_GetParamUlongValue (ANSC_HANDLE hInsContext, char* ParamName, ULONG* puLong)
{
    PCOSA_LGI_MTA_LINEINFO pInfo = (PCOSA_LGI_MTA_LINEINFO)hInsContext;
    ULONG nIndex = pInfo->InstanceNumber - 1;

    CosaDmlMTALineInfoGetEntry(NULL, nIndex, pInfo);

    if (strcmp(ParamName, "OperStatus") == 0)
    {
        *puLong = pInfo->OperStatus;
        return TRUE;
    }

    if (strcmp(ParamName, "AdminStatus") == 0)
    {
        *puLong = pInfo->AdminStatus;
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_MTA_LineInfo_SetParamUlongValue (ANSC_HANDLE hInsContext, char* ParamName, ULONG uValue)
{
    PCOSA_LGI_MTA_LINEINFO pInfo = (PCOSA_LGI_MTA_LINEINFO)hInsContext;
    ULONG nIndex = pInfo->InstanceNumber - 1;

    if (strcmp(ParamName, "AdminStatus") == 0)
    {
        CosaDmlMTALineInfoSetAdminStatus(NULL, nIndex, uValue);
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_MTA_LineInfo_Validate (ANSC_HANDLE hInsContext, char* pReturnParamName, ULONG* puLength)
{
    return TRUE;
}

ULONG X_LGI_COM_MTA_LineInfo_Commit (ANSC_HANDLE hInsContext)
{
    return 0;
}

ULONG X_LGI_COM_MTA_LineInfo_Rollback (ANSC_HANDLE hInsContext)
{
    return 0;
}


/***********************************************************************

 APIs for Object:

    Device.X_LGI_COM_MTA.LineTable.{i}.

***********************************************************************/

ULONG X_LGI_COM_MTA_LineTable_GetEntryCount (ANSC_HANDLE hInsContext)
{
    //LineTable uses pMyObject->pLineInfo to get parameter values.
    return X_LGI_COM_MTA_LineInfo_GetEntryCount(hInsContext);
}

ANSC_HANDLE X_LGI_COM_MTA_LineTable_GetEntry (ANSC_HANDLE hInsContext, ULONG nIndex, ULONG* pInsNumber)
{
    //LineTable uses pMyObject->pLineInfo to get parameter values.
    return X_LGI_COM_MTA_LineInfo_GetEntry(hInsContext, nIndex, pInsNumber);
}

BOOL X_LGI_COM_MTA_LineTable_GetParamUlongValue (ANSC_HANDLE hInsContext, char* ParamName, ULONG* puLong)
{
    PCOSA_LGI_MTA_LINEINFO pInfo = (PCOSA_LGI_MTA_LINEINFO)hInsContext;
    ULONG nIndex = pInfo->InstanceNumber - 1;

    if (strcmp(ParamName, "Status") == 0)
    {
        CosaDmlMTALineInfoGetEntry(NULL, nIndex, pInfo);
        *puLong = pInfo->HookStatus;
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_MTA_LineTable_Validate (ANSC_HANDLE hInsContext, char* pReturnParamName, ULONG* puLength)
{
    return TRUE;
}

ULONG X_LGI_COM_MTA_LineTable_Commit (ANSC_HANDLE hInsContext)
{
    return 0;
}

ULONG X_LGI_COM_MTA_LineTable_Rollback (ANSC_HANDLE hInsContext)
{
    return 0;
}
