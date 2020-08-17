//!!!  This code assumes that all data structures are the SAME in middle-layer APIs and HAL layer APIs
//!!!  So it uses casting from one to the other
#include "cosa_x_lgi_com_mta_apis.h"
#include "mta_hal.h"

ANSC_STATUS
CosaDmlLgiMTAInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    )
{
    if ( mta_hal_InitDB() == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTAGetPacketCableRegion
	(
        ANSC_HANDLE                 hContext,
        ULONG                       *pPacketCableRegion
    )
{
    if ( mta_hal_GetPacketCableRegion(pPacketCableRegion) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ULONG
CosaDmlMTALineInfoGetNumberOfEntries
    (
        ANSC_HANDLE                 hContext
    )
{
    return mta_hal_LineInfoGetNumberOfEntries();
}

ANSC_STATUS
CosaDmlMTALineInfoGetEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        PCOSA_LGI_MTA_LINEINFO      pEntry
    )

{
    if ( mta_hal_LineInfoGetEntry(ulIndex, (PMTAMGMT_MTA_LINETABLE_INFO)pEntry) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTALineInfoSetAdminStatus
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        ULONG                       ulValue
    )
{
    if ( mta_hal_LineInfoSetAdminStatus(ulIndex, ulValue) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}
