
#ifndef  _COSA_LGI_MTA_DML_H
#define  _COSA_LGI_MTA_DML_H

/***********************************************************************

 APIs for Object:

    X_LGI_COM_MTA.

    *  X_LGI_COM_MTA_GetParamStringValue

***********************************************************************/
ULONG
X_LGI_COM_MTA_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );


/***********************************************************************

 APIs for Object:

    X_LGI_COM_MTA.LineInfo.{i}.

    *  X_LGI_COM_MTA_LineInfo_GetEntryCount
    *  X_LGI_COM_MTA_LineInfo_GetEntry
    *  X_LGI_COM_MTA_LineInfo_GetParamUlongValue
    *  X_LGI_COM_MTA_LineInfo_SetParamUlongValue
    *  X_LGI_COM_MTA_LineInfo_Validate
    *  X_LGI_COM_MTA_LineInfo_Commit
    *  X_LGI_COM_MTA_LineInfo_Rollback

***********************************************************************/
ULONG
X_LGI_COM_MTA_LineInfo_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    );

ANSC_HANDLE
X_LGI_COM_MTA_LineInfo_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    );

BOOL
X_LGI_COM_MTA_LineInfo_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    );

BOOL
X_LGI_COM_MTA_LineInfo_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    );

BOOL
X_LGI_COM_MTA_LineInfo_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    );

ULONG
X_LGI_COM_MTA_LineInfo_Commit
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG
X_LGI_COM_MTA_LineInfo_Rollback
    (
        ANSC_HANDLE                 hInsContext
    );

/***********************************************************************

 APIs for Object:

    X_LGI_COM_MTA.LineTable.{i}.

    *  X_LGI_COM_MTA_LineTable_GetEntryCount
    *  X_LGI_COM_MTA_LineTable_GetEntry
    *  X_LGI_COM_MTA_LineTable_GetParamUlongValue
    *  X_LGI_COM_MTA_LineTable_Validate
    *  X_LGI_COM_MTA_LineTable_Commit
    *  X_LGI_COM_MTA_LineTable_Rollback

***********************************************************************/
ULONG
X_LGI_COM_MTA_LineTable_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    );

ANSC_HANDLE
X_LGI_COM_MTA_LineTable_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    );

BOOL
X_LGI_COM_MTA_LineTable_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    );

BOOL
X_LGI_COM_MTA_LineTable_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    );

ULONG
X_LGI_COM_MTA_LineTable_Commit
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG
X_LGI_COM_MTA_MTA_LineTable_Rollback
    (
        ANSC_HANDLE                 hInsContext
    );

#endif /* _COSA_LGI_MTA_DML_H */

