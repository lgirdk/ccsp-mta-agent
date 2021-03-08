#ifdef MTA_TR104SUPPORT
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <LatticeMtaTR104.h>
#include "TR104.h"
#include <rbus.h>
#include "ccsp_trace.h"
#include "ansc_platform.h"
#include "mta_hal.h"

int mta_hal_getTR104parameterValues(char **parameterNamesList, int *parameterListLen, char ***parameterValuesList);
int mta_hal_freeTR104parameterValues(char **parameterValuesList, int  parameterListLen);
int mta_hal_setTR104parameterValues(char **parameterValueList, int *parameterListLen);
int mta_hal_getTR104parameterNames(char ***parameterNamesList, int *parameterListLen);
char MTA_provision_status[2][20] = {"MTA_PROVISIONED", "MTA_NON_PROVISIONED"};
#define MTA_PROVISIONED "MTA_PROVISIONED"


/* This function is to return ParamValue for all the elements in this partial path Query */
rbusError_t TR104Services_TableHandler(rbusHandle_t handle, rbusProperty_t inProperty, rbusGetHandlerOptions_t* opts)
{
    UNREFERENCED_PARAMETER(handle);
    UNREFERENCED_PARAMETER(opts);
    rbusError_t rc = RBUS_ERROR_SUCCESS;
    int retParamSize = 1;
    const char *pParamNameList = NULL;
    char **pParamValueList = NULL;

    MTAMGMT_MTA_PROVISION_STATUS provisionStatus = 0;
    if(mta_hal_getMtaProvisioningStatus(&provisionStatus) != 0)
    {
        CcspTraceError(("%s mta_hal_getMtaProvisioningStatus fail\n",__FUNCTION__));
        return RBUS_ERROR_DESTINATION_RESPONSE_FAILURE;
    }
    if( strcmp(MTA_provision_status[provisionStatus],MTA_PROVISIONED) )
    {
        CcspTraceInfo(("%s provisionStatus = %s \n",__FUNCTION__,MTA_provision_status[provisionStatus]));
        return RBUS_ERROR_ACCESS_NOT_ALLOWED;
    }
    pParamNameList = rbusProperty_GetName(inProperty);

    retParamSize = 1;
    CcspTraceInfo(("calling HAL get\n"));
    /* Get Data From HAL and return in as rbusProperty_t */
    if(mta_hal_getTR104parameterValues((char**)&pParamNameList, &retParamSize, &pParamValueList) == 0 )
    {
        int loopCnt;
        rbusProperty_t getProp = NULL;
        rbusProperty_t lastProp = inProperty;
        for (loopCnt = 0; loopCnt < retParamSize; loopCnt++)
        {
            rbusValue_t value = NULL;
            /* Tokenize the returns value list and set the tlv and pass it to inProperty */
            char *pPropName = NULL;
            char *pStr = strdup (pParamValueList[loopCnt]);
            pPropName = strtok(pStr, ",");
            if (pPropName)
            {
                char* pPropType = NULL;
                char* pPropValue = NULL;

                /* Get Type */
                pPropType = strtok(NULL, ",");
                if (pPropType == NULL)
                {
                    CcspTraceError(("ccspMtaAgentTR104Hal: The pre-defined formatted string is not present\n"));
                    /* Free the memory */
                    free (pStr);
                    continue;
                }

                /* Get Value */
                pPropValue = strtok(NULL, "");
                if(!pPropValue)
                {
                    pPropValue = "";
                }

                rbusValue_Init(&value);
                /* Parse the incoming data */
                if(strcasecmp (pPropType, "boolean") == 0)
                {
                    rbusValue_SetFromString(value, RBUS_BOOLEAN, pPropValue);
                }
                else if(strcasecmp (pPropType, "dateTime") == 0)
                {
                    rbusValue_SetFromString(value, RBUS_DATETIME, pPropValue);
                }
                else if(strcasecmp (pPropType, "int") == 0)
                {
                    rbusValue_SetFromString(value, RBUS_INT32, pPropValue);
                }
                else if(strcasecmp (pPropType, "unsignedint") == 0)
                {
                    rbusValue_SetFromString(value, RBUS_UINT32, pPropValue);
                }
                else if(strcasecmp (pPropType, "long") == 0)
                {
                    rbusValue_SetFromString(value, RBUS_INT64, pPropValue);
                }
                else if(strcasecmp (pPropType, "unsignedLong") == 0)
                {
                    rbusValue_SetFromString(value, RBUS_UINT64, pPropValue);
                }
                else if(strcasecmp (pPropType, "string") == 0)
                {
                    rbusValue_SetFromString(value, RBUS_STRING, pPropValue);
                }
                else
                {
                    CcspTraceWarning(("ccspMtaAgentTR104Hal: Invalid format to send across..\n"));
                    rbusValue_Release(value);
                    /* Free the memory */
                    free (pStr);
                    continue;
                }

                rbusProperty_Init(&getProp, pPropName, NULL);
                rbusProperty_SetValue(getProp, value);

                rbusProperty_PushBack(lastProp, getProp);
                rbusProperty_Release(getProp);
                rbusValue_Release(value);
                lastProp = getProp;
            }
            /* Free the memory */
            free (pStr);
        }
        /* Free the memory */
        mta_hal_freeTR104parameterValues(pParamValueList, retParamSize);
    }
    else
    {
        CcspTraceError(("ccspMtaAgentTR104Hal: Failed to communicate to hal.. \n"));
        rc = RBUS_ERROR_DESTINATION_RESPONSE_FAILURE;
    }

    return rc;
}
rbusError_t TR104Services_GetHandler(rbusHandle_t handle, rbusProperty_t inProperty, rbusGetHandlerOptions_t* opts)
{
    UNREFERENCED_PARAMETER(handle);
    UNREFERENCED_PARAMETER(opts);
    rbusError_t rc = RBUS_ERROR_SUCCESS;
    int retParamSize = 1;
    const char *pParamNameList = NULL;
    char **pParamValueList = NULL;

    MTAMGMT_MTA_PROVISION_STATUS provisionStatus = 0;
    if(mta_hal_getMtaProvisioningStatus(&provisionStatus) != 0)
    {
        CcspTraceError(("%s mta_hal_getMtaProvisioningStatus fail\n",__FUNCTION__));
        return RBUS_ERROR_DESTINATION_RESPONSE_FAILURE;
    }
    if( strcmp(MTA_provision_status[provisionStatus],MTA_PROVISIONED) )
    {
        CcspTraceInfo(("%s provisionStatus = %s \n",__FUNCTION__,MTA_provision_status[provisionStatus]));
        return RBUS_ERROR_ACCESS_NOT_ALLOWED;
    }
    rbusValue_t value;
    rbusValue_Init(&value);

    pParamNameList = rbusProperty_GetName(inProperty);

    retParamSize = 1;
    CcspTraceInfo(("calling HAL get\n"));
    /* Get Data From HAL and return in as rbusProperty_t */
    if(mta_hal_getTR104parameterValues((char**)&pParamNameList, &retParamSize, &pParamValueList) == 0 )
    {
        /* Tokenize the returns value list and set the tlv and pass it to inProperty */
        char *pTmp = NULL;
        char *pStr = strdup (pParamValueList[0]);
        pTmp = strtok(pStr, ",");
        if (pTmp)
        {
            char* pType = strtok(NULL, ",");
            char* pValue = strtok(NULL, "");
            if(!pValue)
            {
                pValue = "";
            }
            if (pType == NULL)
            {
                CcspTraceError(("ccspMtaAgentTR104Hal: The pre-defined formatted string is not present\n"));
                rc = RBUS_ERROR_INVALID_INPUT; 
            }

            if (rc == RBUS_ERROR_SUCCESS)
            {
                /* Parse the Value  */
                /* Get the Type */
                if(strcasecmp (pType, "boolean") == 0)
                {
                    rbusValue_SetFromString(value, RBUS_BOOLEAN, pValue);
                }
                else if(strcasecmp (pType, "dateTime") == 0)
                {
                    rbusValue_SetFromString(value, RBUS_DATETIME, pValue);
                }
                else if(strcasecmp (pType, "int") == 0)
                {
                    rbusValue_SetFromString(value, RBUS_INT32, pValue);
                }
                else if(strcasecmp (pType, "unsignedint") == 0)
                {
                    rbusValue_SetFromString(value, RBUS_UINT32, pValue);
                }
                else if(strcasecmp (pType, "long") == 0)
                {
                    rbusValue_SetFromString(value, RBUS_INT64, pValue);
                }
                else if(strcasecmp (pType, "unsignedLong") == 0)
                {
                    rbusValue_SetFromString(value, RBUS_UINT64, pValue);
                }
                else if(strcasecmp (pType, "string") == 0)
                {
                    rbusValue_SetFromString(value, RBUS_STRING, pValue);
                }
                else
                {
                    CcspTraceError(("ccspMtaAgentTR104Hal: Invalid format to send across..\n"));
                    rc = RBUS_ERROR_INVALID_INPUT;
                }

                if (rc == RBUS_ERROR_SUCCESS)
                    rbusProperty_SetValue(inProperty, value);

                rbusValue_Release(value);
            }
        }
        /* Free the memory */
        free (pStr);
        mta_hal_freeTR104parameterValues(pParamValueList, retParamSize);
    }
    else
    {
        CcspTraceError(("ccspMtaAgentTR104Hal: Failed to communicate to hal.. \n"));
        rc = RBUS_ERROR_DESTINATION_RESPONSE_FAILURE;
    }

    return rc;
}
rbusError_t TR104Services_SetHandler(rbusHandle_t handle, rbusProperty_t inProperty, rbusSetHandlerOptions_t* opts)
{
    UNREFERENCED_PARAMETER(handle);
    UNREFERENCED_PARAMETER(opts);
    int count = 1;

    MTAMGMT_MTA_PROVISION_STATUS provisionStatus = 0;
    if(mta_hal_getMtaProvisioningStatus(&provisionStatus) != 0)
    {
        CcspTraceError(("%s mta_hal_getMtaProvisioningStatus fail\n",__FUNCTION__));
        return RBUS_ERROR_DESTINATION_RESPONSE_FAILURE;
    }
    if( strcmp(MTA_provision_status[provisionStatus],MTA_PROVISIONED) )
    {
        CcspTraceInfo(("%s provisionStatus = %s \n",__FUNCTION__,MTA_provision_status[provisionStatus]));
        return RBUS_ERROR_ACCESS_NOT_ALLOWED;
    }
    const char * name = rbusProperty_GetName(inProperty);
    rbusValue_t value = rbusProperty_GetValue(inProperty);
    rbusValueType_t type = rbusValue_GetType(value);

    char *aParamDetail = NULL;
    CcspTraceInfo(("calling HAL set\n"));
    /* Arrive at length and do malloc; for now 512; */
    aParamDetail = calloc (1, 512);
    if( aParamDetail != NULL )
    {
        char* pStrValue = rbusValue_ToString(value, NULL, 0);
        /* Make a single string */
        if (type == RBUS_BOOLEAN)
        {
            if (rbusValue_GetBoolean(value) == true)
                sprintf(aParamDetail, "%s,%s,%s", name, "boolean", "true");
            else
                sprintf(aParamDetail, "%s,%s,%s", name, "boolean", "false");
        }
        else if (type == RBUS_INT32)
        {
            sprintf(aParamDetail, "%s,%s,%s", name, "int", pStrValue);
        }
        else if (type == RBUS_UINT32)
        {
            sprintf(aParamDetail, "%s,%s,%s", name, "unsignedInt", pStrValue);
        }
        else if (type == RBUS_INT64)
        {
            sprintf(aParamDetail, "%s,%s,%s", name, "long", pStrValue);
        }
        else if (type == RBUS_UINT64)
        {
            sprintf(aParamDetail, "%s,%s,%s", name, "unsignedLong", pStrValue);
        }
        else if (type == RBUS_STRING)
        {
            sprintf(aParamDetail, "%s,%s,%s", name, "string", pStrValue);
        }
        else if (type == RBUS_DATETIME)
        {
            sprintf(aParamDetail, "%s,%s,%s", name, "dateTime", pStrValue);
        }

        /* Free the pStrValue */
        if (pStrValue)
            free (pStrValue);

        if (0 != mta_hal_setTR104parameterValues(&aParamDetail, &count))
        {
            CcspTraceError(("ccspMtaAgentTR104Hal: Set Failed\n"));
            free(aParamDetail);
            return RBUS_ERROR_INVALID_INPUT;
        }
        free(aParamDetail);
        CcspTraceInfo(("ccspMtaAgentTR104Hal: Set Succsess\n"));
        return RBUS_ERROR_SUCCESS;
    }
    return RBUS_ERROR_OUT_OF_RESOURCES;
}
int TR104_open()
{
    int i = 0;
    int halRet = 0;
    char **pParamList = NULL;
    int paramCount = 0;
    rbusError_t rc = RBUS_ERROR_SUCCESS;
    rbusHandle_t rbusHandle = NULL;
    rbusDataElement_t* dataElements = NULL;
    CcspTraceInfo(("calling mta_hal_getTR104parameterNames\n"));
    /* Get the list of properties that are supported by MTA Hal */
    halRet = mta_hal_getTR104parameterNames(&pParamList, &paramCount);
    if (halRet != 0)
    {
        CcspTraceError(("ccspMtaAgentTR104Hal: MTA Hal Returned Failure\n"));
        return RBUS_ERROR_DESTINATION_RESPONSE_FAILURE;
    }
    CcspTraceInfo(("calling rbus_open for registering with rbus\n"));
    rc = rbus_open(&rbusHandle, "ccspMtaAgentTR104Hal");
    if(rc != RBUS_ERROR_SUCCESS)
    {
        CcspTraceError(("ccspMtaAgentTR104Hal: rbus_open failed: %d\n", rc));
        return rc;
    }

    dataElements = (rbusDataElement_t*)calloc(paramCount, sizeof(rbusDataElement_t));
    if( dataElements != NULL )
    {      
        while (i < paramCount)
        {

            if (strcmp(pParamList[i] + strlen(pParamList[i])-5,".{i}.") == 0 )
            {
                dataElements[i].name = pParamList[i];
                dataElements[i].type = RBUS_ELEMENT_TYPE_TABLE;
                dataElements[i].cbTable.getHandler = TR104Services_TableHandler;
            }
            else
            {
                dataElements[i].name = pParamList[i];
                dataElements[i].type = RBUS_ELEMENT_TYPE_PROPERTY;
                dataElements[i].cbTable.getHandler = (rbusGetHandler_t)TR104Services_GetHandler;
                dataElements[i].cbTable.setHandler = (rbusSetHandler_t)TR104Services_SetHandler;
            }
            i++;
        }


        rc = rbus_regDataElements(rbusHandle, paramCount, dataElements);
        if(rc != RBUS_ERROR_SUCCESS)
        {
            CcspTraceError(("ccspMtaAgentTR104Hal: rbusPropertyProvider_Register failed: %d\n", rc));
            free(dataElements);
            rbus_close(rbusHandle);
            return rc;
        }
        CcspTraceInfo(("registered successfully\n"));
        free(dataElements);
        return RBUS_ERROR_SUCCESS;
    }

    return RBUS_ERROR_OUT_OF_RESOURCES;
}
#endif
