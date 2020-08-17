#include "cosa_x_lgi_com_mta_apis.h"
#include "cosa_x_lgi_com_mta_dml.h"
#include "cosa_x_lgi_com_mta_internal.h"
#include "mta_hal.h"

/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CosaLgiMTACreate
            (
            );

    description:

        This function constructs cosa device info object and return handle.

    argument:

    return:     newly created device info object.

**********************************************************************/

ANSC_HANDLE
CosaLgiMTACreate
    (
        VOID
    )
{
    ANSC_STATUS                  returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_MTA      pMyObject    = (PCOSA_DATAMODEL_LGI_MTA)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_LGI_MTA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_MTA));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    AnscZeroMemory(pMyObject, sizeof(COSA_DATAMODEL_LGI_MTA));

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid               = COSA_DATAMODEL_LGI_MTA_OID;
    pMyObject->Create            = CosaLgiMTACreate;
    pMyObject->Remove            = CosaLgiMTARemove;
    pMyObject->Initialize        = CosaLgiMTAInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaLgiMTAInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa device info object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaLgiMTAInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                  returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_MTA      pMyObject    = (PCOSA_DATAMODEL_LGI_MTA)hThisObject;

    /* Initiation all functions */

    CosaDmlLgiMTAInit(NULL, (PANSC_HANDLE)pMyObject);

    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaLgiMTARemove
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa device info object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaLgiMTARemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_MTA         pMyObject    = (PCOSA_DATAMODEL_LGI_MTA)hThisObject;

    /* Remove necessary resounce */

    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return returnStatus;
}

