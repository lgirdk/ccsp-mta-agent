#ifndef  _COSA_LGI_MTA_APIS_H
#define  _COSA_LGI_MTA_APIS_H

#include "cosa_apis.h"
#include "plugin_main_apis.h"

#define BUFFER_LEN               8
/**********************************************************************
                STRUCTURE AND CONSTANT DEFINITIONS
**********************************************************************/

typedef  struct
_COSA_LGI_MTA_LINEINFO
{
    ULONG                           InstanceNumber;
    ULONG                           OperStatus;
    ULONG                           AdminStatus;
    ULONG                           HookStatus;
}
COSA_LGI_MTA_LINEINFO, *PCOSA_LGI_MTA_LINEINFO;


/**********************************************************************
                FUNCTION PROTOTYPES
**********************************************************************/

ANSC_STATUS
CosaDmlLgiMTAInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    );

ANSC_STATUS
CosaDmlMTAGetPacketCableRegion
    (
        ANSC_HANDLE                 hContext,
        ULONG                       *pPacketCableRegion
    );

ULONG
CosaDmlMTALineInfoGetNumberOfEntries
    (
        ANSC_HANDLE                 hContext
    );

ANSC_STATUS
CosaDmlMTALineInfoGetEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        PCOSA_LGI_MTA_LINEINFO      pEntry
    );

ANSC_STATUS
CosaDmlMTALineInfoSetAdminStatus
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        ULONG                       ulValue
    );

BOOL
CosaDmlMTABasicInfoGetSpeedUpMTARFLossDetection
    (
        ANSC_HANDLE                 hContext
    );
int
CosaDmlMTABasicInfoSetSpeedUpMTARFLossDetection
    (
        BOOL               bValue
    );

#endif /* _COSA_LGI_MTA_APIS_H */

