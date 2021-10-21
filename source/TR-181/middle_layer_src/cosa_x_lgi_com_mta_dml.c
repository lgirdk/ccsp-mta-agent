
#include "cosa_x_lgi_com_mta_apis.h"
#include "cosa_x_lgi_com_mta_dml.h"
#include "cosa_x_lgi_com_mta_internal.h"

/***********************************************************************

 APIs for Object:

    X_LGI_COM_MTA.

    *  X_LGI_COM_MTA_GetParamStringValue

***********************************************************************/

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        X_LGI_COM_MTA_GetParamStringValue
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
X_LGI_COM_MTA_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    /* check the parameter name and return the corresponding value */
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

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        X_LGI_COM_MTA_GetParamBoolValue
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
X_LGI_COM_MTA_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_DATAMODEL_LGI_MTA       pMyObject     = (PCOSA_DATAMODEL_LGI_MTA)g_pCosaBEManager->hLgiMTA;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "SpeedUpMTA-RFLossDetection") == 0)
    {
        *pBool = pMyObject->RFLossDetection;
        return TRUE;
    }

    /* AnscTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}






/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        X_LGI_COM_MTA_SetParamBoolValue
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
X_LGI_COM_MTA_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_DATAMODEL_LGI_MTA       pMyObject     = (PCOSA_DATAMODEL_LGI_MTA)g_pCosaBEManager->hLgiMTA;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "SpeedUpMTA-RFLossDetection") == 0)
    {
        pMyObject->RFLossDetection = bValue;
        return TRUE;
    }

    return FALSE;
}



/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        X_LGI_COM_MTA_Validate
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
X_LGI_COM_MTA_Validate
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
        X_LGI_COM_MTA_Commit
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
X_LGI_COM_MTA_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_MTA       pMyObject     = (PCOSA_DATAMODEL_LGI_MTA)g_pCosaBEManager->hLgiMTA;

    CosaDmlMTABasicInfoSetSpeedUpMTARFLossDetection(pMyObject->RFLossDetection);
    return 0;
}


/*********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        X_LGI_COM_MTA_Rollback
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
X_LGI_COM_MTA_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_MTA       pMyObject     = (PCOSA_DATAMODEL_LGI_MTA)g_pCosaBEManager->hLgiMTA;
    pMyObject->RFLossDetection = CosaDmlMTABasicInfoGetSpeedUpMTARFLossDetection(NULL);
    return 0;
}

/***********************************************************************

 APIs for Object:

    Device.X_LGI_COM_MTA.LineInfo.{i}.

    *  X_LGI_COM_MTA_LineInfo_GetEntryCount
    *  X_LGI_COM_MTA_LineInfo_GetEntry
    *  X_LGI_COM_MTA_LineInfo_GetParamUlongValue
    *  X_LGI_COM_MTA_LineInfo_SetParamUlongValue
    *  X_LGI_COM_MTA_LineInfo_Validate
    *  X_LGI_COM_MTA_LineInfo_Commit
    *  X_LGI_COM_MTA_LineInfo_Rollback

***********************************************************************/
/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        X_LGI_COM_MTA_LineInfo_GetEntryCount
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
X_LGI_COM_MTA_LineInfo_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_MTA         pMyObject     = (PCOSA_DATAMODEL_LGI_MTA)g_pCosaBEManager->hLgiMTA;
    PCOSA_LGI_MTA_LINEINFO          pLineInfo     = (PCOSA_LGI_MTA_LINEINFO)pMyObject->pLineInfo;
    ULONG                           ulCount;

    ulCount = CosaDmlMTALineInfoGetNumberOfEntries(NULL);

    if ( ulCount != 0 && pLineInfo == NULL )
    {
        pLineInfo = AnscAllocateMemory(ulCount * sizeof(COSA_LGI_MTA_LINEINFO));
        if(pLineInfo != NULL)
        {
            memset( pLineInfo, 0, ulCount * sizeof(COSA_LGI_MTA_LINEINFO));

            ULONG ul=0;
            for (ul=0; ul<ulCount; ul++) {
                pLineInfo[ul].InstanceNumber = ul + 1;
            }
            pMyObject->pLineInfo = pLineInfo;
        }
    }

    return ulCount;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        X_LGI_COM_MTA_LineInfo_GetEntry
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
X_LGI_COM_MTA_LineInfo_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_LGI_MTA         pMyObject   = (PCOSA_DATAMODEL_LGI_MTA)g_pCosaBEManager->hLgiMTA;
    PCOSA_LGI_MTA_LINEINFO          pLineInfo   = (PCOSA_LGI_MTA_LINEINFO)pMyObject->pLineInfo;

    pLineInfo[nIndex].InstanceNumber = nIndex + 1;

    *pInsNumber = nIndex + 1;

    return &(pLineInfo[nIndex]); /* return the handle */
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        X_LGI_COM_MTA_LineInfo_GetParamUlongValue
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
X_LGI_COM_MTA_LineInfo_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_LGI_MTA_LINEINFO          pInfo = (PCOSA_LGI_MTA_LINEINFO)hInsContext;

    ULONG                           nIndex = pInfo->InstanceNumber - 1;

    CosaDmlMTALineInfoGetEntry(NULL, nIndex, pInfo);

    /* check the parameter name and return the corresponding value */

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
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        X_LGI_COM_MTA_LineInfo_SetParamUlongValue
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
X_LGI_COM_MTA_LineInfo_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    PCOSA_LGI_MTA_LINEINFO      pInfo = (PCOSA_LGI_MTA_LINEINFO)hInsContext;

    ULONG                       nIndex = pInfo->InstanceNumber - 1;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "AdminStatus") == 0)
    {
        CosaDmlMTALineInfoSetAdminStatus(NULL, nIndex, uValue);
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        X_LGI_COM_MTA_LineInfo_Validate
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
X_LGI_COM_MTA_LineInfo_Validate
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
        X_LGI_COM_MTA_LineInfo_Commit
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
X_LGI_COM_MTA_LineInfo_Commit
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
        X_LGI_COM_MTA_LineInfo_Rollback
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
X_LGI_COM_MTA_LineInfo_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    return 0;
}




/***********************************************************************

 APIs for Object:

    Device.X_LGI_COM_MTA.LineTable.{i}.

    *  X_LGI_COM_MTA_LineTable_GetEntryCount
    *  X_LGI_COM_MTA_LineTable_GetEntry
    *  X_LGI_COM_MTA_LineTable_GetParamUlongValue
    *  X_LGI_COM_MTA_LineTable_Validate
    *  X_LGI_COM_MTA_LineTable_Commit
    *  X_LGI_COM_MTA_LineTable_Rollback

***********************************************************************/
/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        X_LGI_COM_MTA_LineTable_GetEntryCount
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
X_LGI_COM_MTA_LineTable_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    //LineTable uses pMyObject->pLineInfo to get parameter values.
    return X_LGI_COM_MTA_LineInfo_GetEntryCount(hInsContext);
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        X_LGI_COM_MTA_LineTable_GetEntry
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
X_LGI_COM_MTA_LineTable_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    //LineTable uses pMyObject->pLineInfo to get parameter values.
    return X_LGI_COM_MTA_LineInfo_GetEntry(hInsContext, nIndex, pInsNumber);
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        X_LGI_COM_MTA_LineTable_GetParamUlongValue
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
X_LGI_COM_MTA_LineTable_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_LGI_MTA_LINEINFO          pInfo = (PCOSA_LGI_MTA_LINEINFO)hInsContext;

    ULONG                           nIndex = pInfo->InstanceNumber - 1;

    CosaDmlMTALineInfoGetEntry(NULL, nIndex, pInfo);

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Status") == 0)
    {
        *puLong = pInfo->HookStatus;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        X_LGI_COM_MTA_LineTable_Validate
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
X_LGI_COM_MTA_LineTable_Validate
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
        X_LGI_COM_MTA_LineTable_Commit
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
X_LGI_COM_MTA_LineTable_Commit
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
        X_LGI_COM_MTA_LineTable_Rollback
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
X_LGI_COM_MTA_LineTable_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    return 0;
}
