
#ifndef _COSA_LGI_MTA_INTERNAL_H
#define _COSA_LGI_MTA_INTERNAL_H

#include "cosa_apis.h"
#include "plugin_main_apis.h"
#include "cosa_x_lgi_com_mta_apis.h"

#define  COSA_DATAMODEL_LGI_MTA_CLASS_CONTENT                               \
    /* duplication of the base object class content */                      \
    COSA_BASE_CONTENT                                                       \
    /* start of Lan config security object class content */                 \
    PCOSA_LGI_MTA_LINEINFO                      pLineInfo;                  \
    ULONG                                       PacketCableRegion;          \
    BOOL                                        RFLossDetection;            \

typedef  struct _COSA_DATAMODEL_LGI_MTA_CLASS_CONTENT
{
    COSA_DATAMODEL_LGI_MTA_CLASS_CONTENT
}
COSA_DATAMODEL_LGI_MTA, *PCOSA_DATAMODEL_LGI_MTA;

ANSC_HANDLE CosaLgiMTACreate (void);
ANSC_STATUS CosaLgiMTAInitialize (ANSC_HANDLE hThisObject);
ANSC_STATUS CosaLgiMTARemove (ANSC_HANDLE hThisObject);

#endif
