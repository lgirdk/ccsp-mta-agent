
#include "cosa_x_lgi_com_mta_apis.h"
#include "cosa_x_lgi_com_mta_dml.h"
#include "cosa_x_lgi_com_mta_internal.h"
#include "mta_hal.h"

ANSC_HANDLE CosaLgiMTACreate (void)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_MTA pMyObject = (PCOSA_DATAMODEL_LGI_MTA) NULL;

    pMyObject = (PCOSA_DATAMODEL_LGI_MTA) AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_MTA));

    if (!pMyObject)
    {
        return NULL;
    }

    pMyObject->Oid               = COSA_DATAMODEL_LGI_MTA_OID;
    pMyObject->Create            = CosaLgiMTACreate;
    pMyObject->Remove            = CosaLgiMTARemove;
    pMyObject->Initialize        = CosaLgiMTAInitialize;

    pMyObject->Initialize ((ANSC_HANDLE) pMyObject);

    return (ANSC_HANDLE) pMyObject;
}

ANSC_STATUS CosaLgiMTAInitialize (ANSC_HANDLE hThisObject)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_MTA pMyObject = (PCOSA_DATAMODEL_LGI_MTA) hThisObject;

    pMyObject->RFLossDetection = CosaDmlMTABasicInfoGetSpeedUpMTARFLossDetection(NULL);
    CosaDmlLgiMTAInit(NULL, (PANSC_HANDLE) pMyObject);

    return returnStatus;
}

ANSC_STATUS CosaLgiMTARemove (ANSC_HANDLE hThisObject)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_MTA pMyObject = (PCOSA_DATAMODEL_LGI_MTA) hThisObject;

    AnscFreeMemory((ANSC_HANDLE) pMyObject);

    return returnStatus;
}
