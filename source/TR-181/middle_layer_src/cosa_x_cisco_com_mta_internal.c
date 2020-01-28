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

    module: cosa_x_cisco_com_mta_internal.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file implementes back-end apis for the COSA Data Model Library

        *  CosaMTACreate
        *  CosaMTAInitialize
        *  CosaMTARemove
    -------------------------------------------------------------------

    environment:

        platform independent

    -------------------------------------------------------------------

    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/11/2011    initial revision.

**************************************************************************/

#include "plugin_main_apis.h"
#include "cosa_x_cisco_com_mta_apis.h"
#include "cosa_x_cisco_com_mta_dml.h"
#include "cosa_x_cisco_com_mta_internal.h"
#include "mta_hal.h"
#include <sysevent/sysevent.h>

static int sysevent_fd;
static token_t sysevent_token;

/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CosaMTACreate
            (
            );

    description:

        This function constructs cosa device info object and return handle.

    argument:  

    return:     newly created device info object.

**********************************************************************/

ANSC_HANDLE
CosaMTACreate
    (
        VOID
    )
{
    ANSC_STATUS                  returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_MTA          pMyObject    = (PCOSA_DATAMODEL_MTA)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_MTA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MTA));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    AnscZeroMemory(pMyObject, sizeof(COSA_DATAMODEL_MTA));

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid               = COSA_DATAMODEL_MTA_OID;
    pMyObject->Create            = CosaMTACreate;
    pMyObject->Remove            = CosaMTARemove;
    pMyObject->Initialize        = CosaMTAInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

ANSC_STATUS ConverStr2Hex(unsigned char buffer[])
{
	int i = 0, j =0; int len = 0;
	char 	tbuffer [ 64 ] = { 0 };
			len = strlen(buffer);
			strcpy(tbuffer,buffer);
			printf("len = %d\n",len);
			for(i = 0; i<len; i++)
			{
				if((buffer[i] >= 48) && (buffer[i] <= 57))
				{
					buffer[i] = buffer[i]-48;
				}
				else if((buffer[i] >= 65) && (buffer[i] <= 70))
				{
					buffer[i] = buffer[i]-55;
				}
				else if((buffer[i] >= 97) && (buffer[i] <= 102))
				{
					buffer[i] = buffer[i]-87;
				}
				else
				{
					printf("buffer = %s is not correct\n",tbuffer);
					CcspTraceError(("Unsupported format %s %s\n", tbuffer,__FUNCTION__));
					return ANSC_STATUS_FAILURE;
				}
			}
	return ANSC_STATUS_SUCCESS;

}

ANSC_STATUS
CosaMTAInitializeEthWanProvDhcpOption
    (
        ANSC_HANDLE                 hThisObject
    )
{

	 MTA_IP_TYPE_TR ip_type;
	 char 	buffer [ 128 ] = { 0 };
	 int	MtaIPMode = 0,IP_Pref_Mode_Received=0;
	 int i = 0, j =0; int len = 0;
	 PMTAMGMT_MTA_PROVISIONING_PARAMS pMtaProv = NULL;
	 pMtaProv = (PMTAMGMT_MTA_PROVISIONING_PARAMS)malloc(sizeof(MTAMGMT_PROVISIONING_PARAMS));

	 PCOSA_DATAMODEL_MTA      pMyObject    = (PCOSA_DATAMODEL_MTA)hThisObject;
	 pMyObject->pmtaprovinfo = (PCOSA_MTA_ETHWAN_PROV_INFO)AnscAllocateMemory(sizeof(COSA_MTA_ETHWAN_PROV_INFO));
	 memset (pMyObject->pmtaprovinfo, 0, sizeof(COSA_MTA_ETHWAN_PROV_INFO));

	 char Ip_Pref [ 6 ] = { 0 }, Ipv4_Primary[16] = {0}, Ipv4_Secondary[16] = {0}, Ipv6_Primary[64] = {0} , Ipv6_Secondary[64] = {0};
	 memset (Ip_Pref, 0, sizeof(Ip_Pref));
	 memset ( Ipv4_Primary, 0, sizeof( Ipv4_Primary));
	 memset (Ipv4_Secondary, 0, sizeof(Ipv4_Secondary));
	 memset (Ipv6_Primary, 0, sizeof(Ipv6_Primary));
	 memset (Ipv6_Secondary, 0, sizeof(Ipv6_Secondary));
	CosaMTAInitializeEthWanProvJournal(pMyObject->pmtaprovinfo);

	sysevent_get(sysevent_fd, sysevent_token, "MTA_IP_PREF", Ip_Pref, sizeof(Ip_Pref));
	sysevent_get(sysevent_fd, sysevent_token, "MTA_DHCPv4_PrimaryAddress", Ipv4_Primary, sizeof(Ipv4_Primary));
	sysevent_get(sysevent_fd, sysevent_token, "MTA_DHCPv4_SecondaryAddress", Ipv4_Secondary, sizeof(Ipv4_Secondary));
	sysevent_get(sysevent_fd, sysevent_token, "MTA_DHCPv6_PrimaryAddress", Ipv6_Primary, sizeof(Ipv6_Primary));
	sysevent_get(sysevent_fd, sysevent_token, "MTA_DHCPv6_SecondaryAddress", Ipv6_Secondary, sizeof(Ipv6_Secondary));

    CcspTraceInfo(("%s MTA values returned from dhcp server are \n",__FUNCTION__));

    CcspTraceInfo(("%s MTA_IP_PREF = %s \n",__FUNCTION__,Ip_Pref));
    CcspTraceInfo(("%s MTA_DHCPv4_PrimaryAddress = %s,MTA_DHCPv4_SecondaryAddress = %s  \n",__FUNCTION__,Ipv4_Primary,Ipv4_Secondary));
    CcspTraceInfo(("%s MTA_DHCPv6_PrimaryAddress = %s,MTA_DHCPv6_SecondaryAddress = %s  \n",__FUNCTION__,Ipv6_Primary,Ipv6_Secondary));

	if ( ( 0 == strncmp(Ipv4_Primary,"00000000",8) ) || ( 0 == strncmp(Ipv6_Primary,"00000000",8) ) )
	{
	    CcspTraceWarning(("%s Received 0's from dhcp sever ,not initializing MTA \n",__FUNCTION__));
		return ANSC_STATUS_FAILURE;
	} 

	if(pMtaProv)
	{

		memset(buffer,0,sizeof(buffer));

		if(Ip_Pref[0] != '\0') {
			sscanf( Ip_Pref, "%d", &IP_Pref_Mode_Received );
			if ( IP_Pref_Mode_Received == 01 )
				MtaIPMode = MTA_IPV4;
			else if ( IP_Pref_Mode_Received == 02 )
				MtaIPMode = MTA_IPV6;
			else if ( ( IP_Pref_Mode_Received == 05 ) || ( IP_Pref_Mode_Received == 06 ) )
				MtaIPMode = MTA_DUAL_STACK;
			else
			{
				CcspTraceWarning(("%s Value received from server is invalid , using default value \n",__FUNCTION__));

				if( 0 == syscfg_get( NULL, "StartupIPMode", buffer, sizeof(buffer)))
				{
				   if(buffer[0] != '\0')
				   {
						sscanf( buffer, "%d", &MtaIPMode );
	     				CcspTraceInfo(("%s Default MtaIPMode is %d \n",__FUNCTION__,MtaIPMode));
				   }
				}
			}

			CcspTraceInfo(("MtaIPMode = %d\n",MtaIPMode));
		}
		else
		{
			// deduce MtaIPMode value from dhcp server  if one of the ip is received , if both are received use default, if not received then use default
			if ( ( (Ipv4_Primary[0] != '\0')  && (Ipv6_Primary[0] != '\0') ) || ( (Ipv4_Primary[0] == '\0')  && (Ipv6_Primary[0] == '\0') ) )
			{

				if( 0 == syscfg_get( NULL, "StartupIPMode", buffer, sizeof(buffer)))
				{
				   if(buffer[0] != '\0')
				   {
						sscanf( buffer, "%d", &MtaIPMode );
	     				CcspTraceInfo(("%s Default MtaIPMode is %d \n",__FUNCTION__,MtaIPMode));
				   }
				}
		       }

			else if ( Ipv4_Primary[0] != '\0')
			{
	     		CcspTraceInfo(("%s Received only Ipv4_Primary from dhcp server , setting IP Preference mode to ipv4 \n",__FUNCTION__));
				MtaIPMode=MTA_IPV4;

			}
			else if ( Ipv6_Primary[0] != '\0')
			{
	     		CcspTraceInfo(("%s Received only Ipv6_Primary from dhcp server , setting IP Preference mode to ipv6  \n",__FUNCTION__));
				MtaIPMode=MTA_IPV6;
			}

		}

		pMtaProv->MtaIPMode = MtaIPMode;
		CosaDmlMTASetStartUpIpMode(pMyObject->pmtaprovinfo,MtaIPMode);
		ip_type = MTA_IPV4_TR;

		memset(buffer,0,sizeof(buffer));
		memset(pMtaProv->DhcpOption122Suboption1,0,MTA_DHCPOPTION122SUBOPTION1_MAX);
		if (Ipv4_Primary[0] != '\0' )
			{
				strcpy(buffer,Ipv4_Primary);
			}
			else 
			{
				syscfg_get( NULL, "IPv4PrimaryDhcpServerOptions", buffer, sizeof(buffer));
			}	
				
				   if(buffer[0] != '\0')
				   {
					len = strlen(buffer);
					CosaDmlMTASetPrimaryDhcpServerOptions(pMyObject->pmtaprovinfo, buffer, ip_type);
					if((MtaIPMode ==  MTA_IPV4) || (MtaIPMode == MTA_DUAL_STACK))
					{
						if(ConverStr2Hex(buffer) == ANSC_STATUS_SUCCESS)
						{
							for(i = 0,j= 0;i<len; i++,j++)
							{
								if(j<MTA_DHCPOPTION122SUBOPTION1_MAX)
								{
									pMtaProv->DhcpOption122Suboption1[j] |= (unsigned char)(((buffer[i])<<4) + (buffer[++i]));
								}
								else
									break;
							}
	
							printf("pMtaProv->DhcpOption122Suboption1[0] = %X %d\n",pMtaProv->DhcpOption122Suboption1[0],pMtaProv->DhcpOption122Suboption1[0]);
							printf("pMtaProv->DhcpOption122Suboption1[1] = %X %d\n",pMtaProv->DhcpOption122Suboption1[1],pMtaProv->DhcpOption122Suboption1[1]);
							printf("pMtaProv->DhcpOption122Suboption1[2] = %X %d\n",pMtaProv->DhcpOption122Suboption1[2],pMtaProv->DhcpOption122Suboption1[2]);
							printf("pMtaProv->DhcpOption122Suboption1[3] = %X %d\n",pMtaProv->DhcpOption122Suboption1[3],pMtaProv->DhcpOption122Suboption1[3]);
						}
					}
				   }


			memset(buffer,0,sizeof(buffer));
			memset(pMtaProv->DhcpOption122Suboption2,0,MTA_DHCPOPTION122SUBOPTION2_MAX);
			if (Ipv4_Secondary[0] != '\0' )
			{
				strcpy(buffer,Ipv4_Secondary);
			}
			else
			{
				syscfg_get( NULL, "IPv4SecondaryDhcpServerOptions", buffer, sizeof(buffer));
			}
				   
				   if(buffer[0] != '\0')
				   {
					len = strlen(buffer);
					CosaDmlMTASetSecondaryDhcpServerOptions(pMyObject->pmtaprovinfo, buffer, ip_type);
					if((MtaIPMode ==  MTA_IPV4) || (MtaIPMode == MTA_DUAL_STACK))
					{
						if(ConverStr2Hex(buffer) == ANSC_STATUS_SUCCESS)
						{
					   		for(i = 0,j= 0;i<len; i++,j++)
							{
								if(j<MTA_DHCPOPTION122SUBOPTION2_MAX)
								{
									pMtaProv->DhcpOption122Suboption2[j] |= (unsigned char)(((buffer[i])<<4) + (buffer[++i]));
								}
								else
									break;
							}
							printf("pMtaProv->DhcpOption122Suboption2[0] = %X %d\n",pMtaProv->DhcpOption122Suboption2[0],pMtaProv->DhcpOption122Suboption2[0]);
							printf("pMtaProv->DhcpOption122Suboption2[1] = %X %d\n",pMtaProv->DhcpOption122Suboption2[1],pMtaProv->DhcpOption122Suboption2[1]);
							printf("pMtaProv->DhcpOption122Suboption2[2] = %X %d\n",pMtaProv->DhcpOption122Suboption2[2],pMtaProv->DhcpOption122Suboption2[2]);
							printf("pMtaProv->DhcpOption122Suboption2[3] = %X %d\n",pMtaProv->DhcpOption122Suboption2[3],pMtaProv->DhcpOption122Suboption2[3]);
						}
					}

				   }
	    ip_type = MTA_IPV6_TR;

				memset(buffer,0,sizeof(buffer));
				memset(pMtaProv->DhcpOption2171CccV6DssID1,0,MTA_DHCPOPTION122CCCV6DSSID1_MAX);
			if ( Ipv6_Primary[0] != '\0' ) 
			{
				strcpy(buffer,Ipv6_Primary);
			}

			else
			{
				syscfg_get( NULL, "IPv6PrimaryDhcpServerOptions", buffer, sizeof(buffer));
			}
				
				   if(buffer[0] != '\0')
				   {
					len = strlen(buffer);
					CosaDmlMTASetPrimaryDhcpServerOptions(pMyObject->pmtaprovinfo, buffer, ip_type);
					if((MtaIPMode == MTA_IPV6) || (MtaIPMode == MTA_DUAL_STACK))
					{	
						if(ConverStr2Hex(buffer) == ANSC_STATUS_SUCCESS)
						{
							printf("Buffer is %s\n",buffer);
								for(i = 0,j= 0;i<len; i++,j++)
										{
								if(j<MTA_DHCPOPTION122CCCV6DSSID1_MAX)
								{
									pMtaProv->DhcpOption2171CccV6DssID1[j] |= (unsigned char)(((buffer[i])<<4) + (buffer[++i]));
								}
								else
									break;
							}
	
							printf("pMtaProv->DhcpOption2171CccV6DssID1[0] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[0],pMtaProv->DhcpOption2171CccV6DssID1[0]);
							printf("pMtaProv->DhcpOption2171CccV6DssID1[1] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[1],pMtaProv->DhcpOption2171CccV6DssID1[1]);
							printf("pMtaProv->DhcpOption2171CccV6DssID1[2] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[2],pMtaProv->DhcpOption2171CccV6DssID1[2]);
							printf("pMtaProv->DhcpOption2171CccV6DssID1[3] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[3],pMtaProv->DhcpOption2171CccV6DssID1[3]);
							pMtaProv->DhcpOption2171CccV6DssID1Len = j;
							printf("pMtaProv->DhcpOption2171CccV6DssID1Len = %d\n",pMtaProv->DhcpOption2171CccV6DssID1Len);
						}
					}

				   }

				memset(buffer,0,sizeof(buffer));
				memset(pMtaProv->DhcpOption2171CccV6DssID2,0,MTA_DHCPOPTION122CCCV6DSSID2_MAX);
			if ( Ipv6_Secondary[0] != '\0' )   
			{
				strcpy(buffer,Ipv6_Secondary);
			}
			else
			{
				syscfg_get( NULL, "IPv6SecondaryDhcpServerOptions", buffer, sizeof(buffer));
			}   
				   if(buffer[0] != '\0')
				   {
					len = strlen(buffer);
					CosaDmlMTASetSecondaryDhcpServerOptions(pMyObject->pmtaprovinfo, buffer, ip_type);
					if((MtaIPMode == MTA_IPV6) || (MtaIPMode == MTA_DUAL_STACK))
				      {	
						if(ConverStr2Hex(buffer) == ANSC_STATUS_SUCCESS)
						{
							printf("Buffer is %s\n",buffer);
					   		for(i = 0,j= 0;i<len; i++,j++)
							{
								if(j<MTA_DHCPOPTION122CCCV6DSSID2_MAX)
								{
									pMtaProv->DhcpOption2171CccV6DssID2[j] |= (unsigned char)(((buffer[i])<<4) + (buffer[++i]));
								}
								else
									break;
							}
							printf("pMtaProv->DhcpOption2171CccV6DssID2[0] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[0],pMtaProv->DhcpOption2171CccV6DssID2[0]);
							printf("pMtaProv->DhcpOption2171CccV6DssID2[1] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[1],pMtaProv->DhcpOption2171CccV6DssID2[1]);
							printf("pMtaProv->DhcpOption2171CccV6DssID2[2] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[2],pMtaProv->DhcpOption2171CccV6DssID2[2]);
							printf("pMtaProv->DhcpOption2171CccV6DssID2[3] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[3],pMtaProv->DhcpOption2171CccV6DssID2[3]);
							pMtaProv->DhcpOption2171CccV6DssID2Len = j;
							printf("pMtaProv->DhcpOption2171CccV6DssID2Len = %d\n",pMtaProv->DhcpOption2171CccV6DssID2Len);
						}
					}

				   }
    #if defined (_COSA_BCM_ARM_) 
		// call hal to start provisioning
		if(mta_hal_start_provisioning(pMtaProv) == RETURN_OK)
		{
			CcspTraceError(("mta_hal_start_provisioning succeded '%s'\n", __FUNCTION__));
			return ANSC_STATUS_SUCCESS;
		}
		else
		{
			CcspTraceError(("mta_hal_start_provisioning Failed '%s'\n", __FUNCTION__));
			return ANSC_STATUS_FAILURE;
		}
                free(pMtaProv);
     #endif
}
else
	{
		printf("Memory Alloction Failed '%s'\n", __FUNCTION__);
		CcspTraceError(("Memory Alloction Failed '%s'\n", __FUNCTION__));
		return ANSC_STATUS_FAILURE;
	}  
}

void WaitForDhcpOption()
{
 unsigned char dhcp_option[10] = {0};
int count=0;

//  wait for 1 min to receive MTA options/Offer , otherwise time out 
 while ( 12 >= count )
 {
	memset (dhcp_option, 0, sizeof(dhcp_option));
	sysevent_get(sysevent_fd, sysevent_token, "dhcp_mta_option", dhcp_option, sizeof(dhcp_option));
	if ((strcmp(dhcp_option, "received") == 0))
	{
		CcspTraceWarning(("%s dhcp_option's received,breaking the loop  \n",__FUNCTION__));
		break;

	}
	sleep(5);
	count++;
 }
 
}

ANSC_STATUS Mta_Sysevent_thread_Dhcp_Option(ANSC_HANDLE  hThisObject)

{

 PCOSA_DATAMODEL_MTA      pMyObject    = (PCOSA_DATAMODEL_MTA)hThisObject;

 unsigned char current_wan_state[10] = {0}, dhcp_option[10] = {0};

 int err;
 unsigned char name[25]={0}, val[10]={0};
 async_id_t getnotification_asyncid;
 memset (current_wan_state, 0, sizeof(current_wan_state));
 memset (dhcp_option, 0, sizeof(dhcp_option));
 async_id_t wan_state_asyncid;

 int retValue=0,  namelen=0, vallen =0 ;
 sysevent_set_options(sysevent_fd, sysevent_token, "current_wan_state", TUPLE_FLAG_EVENT);
 retValue=sysevent_setnotification(sysevent_fd, sysevent_token, "current_wan_state",  &wan_state_asyncid);
 CcspTraceWarning(("%s Return value is   %d\n",__FUNCTION__,retValue));

 sysevent_get(sysevent_fd, sysevent_token, "current_wan_state", current_wan_state, sizeof(current_wan_state));
 sysevent_get(sysevent_fd, sysevent_token, "dhcp_mta_option", dhcp_option, sizeof(dhcp_option));

 if((strcmp(current_wan_state, "up")) == 0 && (strcmp(dhcp_option, "received") == 0))
 {
    CcspTraceWarning(("%s current_wan_state up, Initializing MTA \n",__FUNCTION__));
    CosaMTAInitializeEthWanProvDhcpOption(pMyObject);

 }

 else if((strcmp(current_wan_state, "up")) == 0 && (strcmp(dhcp_option, "received") != 0))
 {
    CcspTraceWarning(("%s current_wan_state up, but dhcp_option's not received.  \n",__FUNCTION__));
    WaitForDhcpOption();

    CosaMTAInitializeEthWanProvDhcpOption(pMyObject);

 }


  while (1)
  {

    namelen = sizeof(name);
    vallen  = sizeof(val);

	memset (name, 0, sizeof(name));
	memset (val, 0, sizeof(val));
    err = sysevent_getnotification(sysevent_fd, sysevent_token, name, &namelen, val, &vallen, &getnotification_asyncid);

    if (!err)
    {
        CcspTraceWarning(("%s Recieved notification event  %s, state %s\n",__FUNCTION__,name,val));
        if(( strcmp(name,"current_wan_state") == 0 ) && ( strcmp(val, "up") == 0 ) )
        {
		    WaitForDhcpOption();
         
            CosaMTAInitializeEthWanProvDhcpOption(pMyObject);
        }


     }
   }
}

ANSC_STATUS
CosaMTAInitializeEthWanProv
    (
        ANSC_HANDLE                 hThisObject
    )
{

 MTA_IP_TYPE_TR ip_type;
 char 	buffer [ 128 ] = { 0 };
 int	MtaIPMode = 0;
 int i = 0, j =0; int len = 0;
 PMTAMGMT_MTA_PROVISIONING_PARAMS pMtaProv = NULL;
 pMtaProv = (PMTAMGMT_MTA_PROVISIONING_PARAMS)malloc(sizeof(MTAMGMT_PROVISIONING_PARAMS));

 PCOSA_DATAMODEL_MTA      pMyObject    = (PCOSA_DATAMODEL_MTA)hThisObject;
 pMyObject->pmtaprovinfo = (PCOSA_MTA_ETHWAN_PROV_INFO)AnscAllocateMemory(sizeof(COSA_MTA_ETHWAN_PROV_INFO));
 memset (pMyObject->pmtaprovinfo, 0, sizeof(COSA_MTA_ETHWAN_PROV_INFO));

 CosaMTAInitializeEthWanProvJournal(pMyObject->pmtaprovinfo);

if(pMtaProv)
{

	memset(buffer,0,sizeof(buffer));
	if( 0 == syscfg_get( NULL, "StartupIPMode", buffer, sizeof(buffer)))
	{
	   if(buffer[0] != '\0')
	   {
		sscanf( buffer, "%d", &MtaIPMode );
		printf("MtaIPMode = %d\n",MtaIPMode);
		pMtaProv->MtaIPMode = MtaIPMode;
	   }
	}
        CosaDmlMTASetStartUpIpMode(pMyObject->pmtaprovinfo,MtaIPMode);
        ip_type = MTA_IPV4_TR;
		memset(buffer,0,sizeof(buffer));
		memset(pMtaProv->DhcpOption122Suboption1,0,MTA_DHCPOPTION122SUBOPTION1_MAX);
		if( 0 == syscfg_get( NULL, "IPv4PrimaryDhcpServerOptions", buffer, sizeof(buffer)))
		{
		   if(buffer[0] != '\0')
		   {
			len = strlen(buffer);
			CosaDmlMTASetPrimaryDhcpServerOptions(pMyObject->pmtaprovinfo, buffer, ip_type);
			if((MtaIPMode ==  MTA_IPV4) || (MtaIPMode == MTA_DUAL_STACK))
			{
				if(ConverStr2Hex(buffer) == ANSC_STATUS_SUCCESS)
				{

					for(i = 0,j= 0;i<len; i++,j++)
					{
						if(j<MTA_DHCPOPTION122SUBOPTION1_MAX)
						{
							pMtaProv->DhcpOption122Suboption1[j] |= (unsigned char)(((buffer[i])<<4) + (buffer[++i]));
						}
						else
							break;
					}
	
					printf("pMtaProv->DhcpOption122Suboption1[0] = %X %d\n",pMtaProv->DhcpOption122Suboption1[0],pMtaProv->DhcpOption122Suboption1[0]);
					printf("pMtaProv->DhcpOption122Suboption1[1] = %X %d\n",pMtaProv->DhcpOption122Suboption1[1],pMtaProv->DhcpOption122Suboption1[1]);
					printf("pMtaProv->DhcpOption122Suboption1[2] = %X %d\n",pMtaProv->DhcpOption122Suboption1[2],pMtaProv->DhcpOption122Suboption1[2]);
					printf("pMtaProv->DhcpOption122Suboption1[3] = %X %d\n",pMtaProv->DhcpOption122Suboption1[3],pMtaProv->DhcpOption122Suboption1[3]);
				}
			}

		   }
		}
		memset(buffer,0,sizeof(buffer));
		memset(pMtaProv->DhcpOption122Suboption2,0,MTA_DHCPOPTION122SUBOPTION2_MAX);
		if( 0 == syscfg_get( NULL, "IPv4SecondaryDhcpServerOptions", buffer, sizeof(buffer)))
		{
		   
		   if(buffer[0] != '\0')
		   {
			len = strlen(buffer);
			CosaDmlMTASetSecondaryDhcpServerOptions(pMyObject->pmtaprovinfo, buffer, ip_type);
			if((MtaIPMode ==  MTA_IPV4) || (MtaIPMode == MTA_DUAL_STACK))
			{
				if(ConverStr2Hex(buffer) == ANSC_STATUS_SUCCESS)
				{
			   		for(i = 0,j= 0;i<len; i++,j++)
					{
						if(j<MTA_DHCPOPTION122SUBOPTION2_MAX)
						{
							pMtaProv->DhcpOption122Suboption2[j] |= (unsigned char)(((buffer[i])<<4) + (buffer[++i]));
						}
						else
							break;
					}
					printf("pMtaProv->DhcpOption122Suboption2[0] = %X %d\n",pMtaProv->DhcpOption122Suboption2[0],pMtaProv->DhcpOption122Suboption2[0]);
					printf("pMtaProv->DhcpOption122Suboption2[1] = %X %d\n",pMtaProv->DhcpOption122Suboption2[1],pMtaProv->DhcpOption122Suboption2[1]);
					printf("pMtaProv->DhcpOption122Suboption2[2] = %X %d\n",pMtaProv->DhcpOption122Suboption2[2],pMtaProv->DhcpOption122Suboption2[2]);
					printf("pMtaProv->DhcpOption122Suboption2[3] = %X %d\n",pMtaProv->DhcpOption122Suboption2[3],pMtaProv->DhcpOption122Suboption2[3]);
				}
			}

		   }
		}
  
        ip_type = MTA_IPV6_TR;
		memset(buffer,0,sizeof(buffer));
		memset(pMtaProv->DhcpOption2171CccV6DssID1,0,MTA_DHCPOPTION122CCCV6DSSID1_MAX);
		if( 0 == syscfg_get( NULL, "IPv6PrimaryDhcpServerOptions", buffer, sizeof(buffer)))
		{
		   if(buffer[0] != '\0')
		   {
			len = strlen(buffer);
			CosaDmlMTASetPrimaryDhcpServerOptions(pMyObject->pmtaprovinfo, buffer, ip_type);
			if((MtaIPMode == MTA_IPV6) || (MtaIPMode == MTA_DUAL_STACK))
			{
				if(ConverStr2Hex(buffer) == ANSC_STATUS_SUCCESS)
				{

					for(i = 0,j= 0;i<len; i++,j++)
					{
						if(j<MTA_DHCPOPTION122CCCV6DSSID1_MAX)
						{
							pMtaProv->DhcpOption2171CccV6DssID1[j] |= (unsigned char)(((buffer[i])<<4) + (buffer[++i]));
						}
						else
							break;
					}
	
					printf("pMtaProv->DhcpOption2171CccV6DssID1[0] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[0],pMtaProv->DhcpOption2171CccV6DssID1[0]);
					printf("pMtaProv->DhcpOption2171CccV6DssID1[1] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[1],pMtaProv->DhcpOption2171CccV6DssID1[1]);
					printf("pMtaProv->DhcpOption2171CccV6DssID1[2] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[2],pMtaProv->DhcpOption2171CccV6DssID1[2]);
					printf("pMtaProv->DhcpOption2171CccV6DssID1[3] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[3],pMtaProv->DhcpOption2171CccV6DssID1[3]);
					pMtaProv->DhcpOption2171CccV6DssID1Len = j;
					printf("pMtaProv->DhcpOption2171CccV6DssID1Len = %d\n",pMtaProv->DhcpOption2171CccV6DssID1Len);
				}

			}
		   }
		}
		memset(buffer,0,sizeof(buffer));
		memset(pMtaProv->DhcpOption2171CccV6DssID2,0,MTA_DHCPOPTION122CCCV6DSSID2_MAX);
		pMtaProv->DhcpOption2171CccV6DssID2Len = 0;
		if( 0 == syscfg_get( NULL, "IPv6SecondaryDhcpServerOptions", buffer, sizeof(buffer)))
		{
		   
		   if(buffer[0] != '\0')
		   {
			len = strlen(buffer);
			CosaDmlMTASetSecondaryDhcpServerOptions(pMyObject->pmtaprovinfo, buffer, ip_type);
			if((MtaIPMode == MTA_IPV6) || (MtaIPMode == MTA_DUAL_STACK))
			{
				if(ConverStr2Hex(buffer) == ANSC_STATUS_SUCCESS)
				{
			   		for(i = 0,j= 0;i<len; i++,j++)
					{
						if(j<MTA_DHCPOPTION122CCCV6DSSID2_MAX)
						{
							pMtaProv->DhcpOption2171CccV6DssID2[j] |= (unsigned char)(((buffer[i])<<4) + (buffer[++i]));
						}
						else
							break;
					}
					printf("pMtaProv->DhcpOption2171CccV6DssID2[0] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[0],pMtaProv->DhcpOption2171CccV6DssID2[0]);
					printf("pMtaProv->DhcpOption2171CccV6DssID2[1] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[1],pMtaProv->DhcpOption2171CccV6DssID2[1]);
					printf("pMtaProv->DhcpOption2171CccV6DssID2[2] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[2],pMtaProv->DhcpOption2171CccV6DssID2[2]);
					printf("pMtaProv->DhcpOption2171CccV6DssID2[3] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[3],pMtaProv->DhcpOption2171CccV6DssID2[3]);
					pMtaProv->DhcpOption2171CccV6DssID2Len = j;
					printf("pMtaProv->DhcpOption2171CccV6DssID2Len = %d\n",pMtaProv->DhcpOption2171CccV6DssID2Len);
				}

			}

		   }
		}
    #ifdef _COSA_BCM_ARM_	
		// call hal to start provisioning
		if(mta_hal_start_provisioning(pMtaProv) == RETURN_OK)
		{
			return ANSC_STATUS_SUCCESS;
		}
		else
		{
			CcspTraceError(("mta_hal_start_provisioning Failed '%s'\n", __FUNCTION__));
			return ANSC_STATUS_FAILURE;
		}
                free(pMtaProv);
     #endif
}
else
	{
		printf("Memory Alloction Failed '%s'\n", __FUNCTION__);
		CcspTraceError(("Memory Alloction Failed '%s'\n", __FUNCTION__));
		return ANSC_STATUS_FAILURE;
	}  
}

ANSC_STATUS
CosaSetMTAHal
    (
        PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro
    )
{
 char 	buffer [ 128 ] = { 0 };
 int	MtaIPMode = 0;
 int i = 0, j =0; int len = 0;
 PMTAMGMT_MTA_PROVISIONING_PARAMS pMtaProv = NULL;
 pMtaProv = (PMTAMGMT_MTA_PROVISIONING_PARAMS)malloc(sizeof(MTAMGMT_PROVISIONING_PARAMS));

if(pMtaProv)
{

	MtaIPMode = pmtaethpro->StartupIPMode.ActiveValue;
	pMtaProv->MtaIPMode = MtaIPMode;
	printf("pMtaProv->MtaIPMode = %d\n", pMtaProv->MtaIPMode);
	memset(buffer,0,sizeof(buffer));
	memset(pMtaProv->DhcpOption122Suboption1,0,MTA_DHCPOPTION122SUBOPTION1_MAX);
		if( 0 != strlen(pmtaethpro->IPv4PrimaryDhcpServerOptions.ActiveValue))
		{
		   _ansc_strcpy(buffer,pmtaethpro->IPv4PrimaryDhcpServerOptions.ActiveValue);
		   if(buffer[0] != '\0')
		   {
			len = strlen(buffer);
			if((MtaIPMode ==  MTA_IPV4) || (MtaIPMode == MTA_DUAL_STACK))
			{
				if(ConverStr2Hex(buffer) == ANSC_STATUS_SUCCESS)
				{

					for(i = 0,j= 0;i<len; i++,j++)
					{
						if(j<MTA_DHCPOPTION122SUBOPTION1_MAX)
						{
							pMtaProv->DhcpOption122Suboption1[j] |= (unsigned char)(((buffer[i])<<4) + (buffer[++i]));
						}
						else
							break;
					}
	
					printf("pMtaProv->DhcpOption122Suboption1[0] = %X %d\n",pMtaProv->DhcpOption122Suboption1[0],pMtaProv->DhcpOption122Suboption1[0]);
					printf("pMtaProv->DhcpOption122Suboption1[1] = %X %d\n",pMtaProv->DhcpOption122Suboption1[1],pMtaProv->DhcpOption122Suboption1[1]);
					printf("pMtaProv->DhcpOption122Suboption1[2] = %X %d\n",pMtaProv->DhcpOption122Suboption1[2],pMtaProv->DhcpOption122Suboption1[2]);
					printf("pMtaProv->DhcpOption122Suboption1[3] = %X %d\n",pMtaProv->DhcpOption122Suboption1[3],pMtaProv->DhcpOption122Suboption1[3]);
				}
			}

		   }
		}
		memset(buffer,0,sizeof(buffer));
		memset(pMtaProv->DhcpOption122Suboption2,0,MTA_DHCPOPTION122SUBOPTION2_MAX);
		if( 0 != strlen(pmtaethpro->IPv4SecondaryDhcpServerOptions.ActiveValue))
		{
		   _ansc_strcpy(buffer,pmtaethpro->IPv4SecondaryDhcpServerOptions.ActiveValue);
		   if(buffer[0] != '\0')
		   {
			len = strlen(buffer);
			if((MtaIPMode ==  MTA_IPV4) || (MtaIPMode == MTA_DUAL_STACK))
			{
				if(ConverStr2Hex(buffer) == ANSC_STATUS_SUCCESS)
				{
			   		for(i = 0,j= 0;i<len; i++,j++)
					{
						if(j<MTA_DHCPOPTION122SUBOPTION2_MAX)
						{
							pMtaProv->DhcpOption122Suboption2[j] |= (unsigned char)(((buffer[i])<<4) + (buffer[++i]));
						}
						else
							break;
					}
					printf("pMtaProv->DhcpOption122Suboption2[0] = %X %d\n",pMtaProv->DhcpOption122Suboption2[0],pMtaProv->DhcpOption122Suboption2[0]);
					printf("pMtaProv->DhcpOption122Suboption2[1] = %X %d\n",pMtaProv->DhcpOption122Suboption2[1],pMtaProv->DhcpOption122Suboption2[1]);
					printf("pMtaProv->DhcpOption122Suboption2[2] = %X %d\n",pMtaProv->DhcpOption122Suboption2[2],pMtaProv->DhcpOption122Suboption2[2]);
					printf("pMtaProv->DhcpOption122Suboption2[3] = %X %d\n",pMtaProv->DhcpOption122Suboption2[3],pMtaProv->DhcpOption122Suboption2[3]);
				}
			}

		   }
		}
  
		memset(buffer,0,sizeof(buffer));
		memset(pMtaProv->DhcpOption2171CccV6DssID1,0,MTA_DHCPOPTION122CCCV6DSSID1_MAX);
		pMtaProv->DhcpOption2171CccV6DssID1Len = 0;
		if(0 != strlen(pmtaethpro->IPv6PrimaryDhcpServerOptions.ActiveValue))
		{
		   _ansc_strcpy(buffer,pmtaethpro->IPv6PrimaryDhcpServerOptions.ActiveValue);
		   if(buffer[0] != '\0')
		   {
			len = strlen(buffer);
			if((MtaIPMode == MTA_IPV6) || (MtaIPMode == MTA_DUAL_STACK))
			{
				if(ConverStr2Hex(buffer) == ANSC_STATUS_SUCCESS)
				{

					for(i = 0,j= 0;i<len; i++,j++)
					{
						if(j<MTA_DHCPOPTION122CCCV6DSSID1_MAX)
						{
							pMtaProv->DhcpOption2171CccV6DssID1[j] |= (unsigned char)(((buffer[i])<<4) + (buffer[++i]));
						}
						else
							break;
					}
	
					printf("pMtaProv->DhcpOption2171CccV6DssID1[0] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[0],pMtaProv->DhcpOption2171CccV6DssID1[0]);
					printf("pMtaProv->DhcpOption2171CccV6DssID1[1] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[1],pMtaProv->DhcpOption2171CccV6DssID1[1]);
					printf("pMtaProv->DhcpOption2171CccV6DssID1[2] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[2],pMtaProv->DhcpOption2171CccV6DssID1[2]);
					printf("pMtaProv->DhcpOption2171CccV6DssID1[3] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[3],pMtaProv->DhcpOption2171CccV6DssID1[3]);
					pMtaProv->DhcpOption2171CccV6DssID1Len = j;
					printf("pMtaProv->DhcpOption2171CccV6DssID1Len = %d\n",pMtaProv->DhcpOption2171CccV6DssID1Len);
				}
			}
		   }
		}
		memset(buffer,0,sizeof(buffer));
		memset(pMtaProv->DhcpOption2171CccV6DssID2,0,MTA_DHCPOPTION122CCCV6DSSID2_MAX);
		pMtaProv->DhcpOption2171CccV6DssID2Len = 0;
		if(0 != strlen(pmtaethpro->IPv6SecondaryDhcpServerOptions.ActiveValue))
		{
		   _ansc_strcpy(buffer,pmtaethpro->IPv6SecondaryDhcpServerOptions.ActiveValue);
		   if(buffer[0] != '\0')
		   {
			len = strlen(buffer);
			if((MtaIPMode == MTA_IPV6) || (MtaIPMode == MTA_DUAL_STACK))
			{
				if(ConverStr2Hex(buffer) == ANSC_STATUS_SUCCESS)
				{
			   		for(i = 0,j= 0;i<len; i++,j++)
					{
						if(j<MTA_DHCPOPTION122CCCV6DSSID2_MAX)
						{
							pMtaProv->DhcpOption2171CccV6DssID2[j] |= (unsigned char)(((buffer[i])<<4) + (buffer[++i]));
						}
						else
							break;
					}
					printf("pMtaProv->DhcpOption2171CccV6DssID2[0] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[0],pMtaProv->DhcpOption2171CccV6DssID2[0]);
					printf("pMtaProv->DhcpOption2171CccV6DssID2[1] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[1],pMtaProv->DhcpOption2171CccV6DssID2[1]);
					printf("pMtaProv->DhcpOption2171CccV6DssID2[2] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[2],pMtaProv->DhcpOption2171CccV6DssID2[2]);
					printf("pMtaProv->DhcpOption2171CccV6DssID2[3] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[3],pMtaProv->DhcpOption2171CccV6DssID2[3]);
					pMtaProv->DhcpOption2171CccV6DssID2Len = j;
					printf("pMtaProv->DhcpOption2171CccV6DssID2Len = %d\n",pMtaProv->DhcpOption2171CccV6DssID2Len);
				}
			}

		   }
		}
#ifdef _COSA_BCM_ARM_
		// call hal to start provisioning
		if(mta_hal_start_provisioning(pMtaProv) == RETURN_OK)
		{
			return ANSC_STATUS_SUCCESS;
		}
		else
		{
			CcspTraceError(("mta_hal_start_provisioning Failed '%s'\n", __FUNCTION__));
			return ANSC_STATUS_FAILURE;
		}
                free(pMtaProv);
#endif
}
else
	{
		printf("Memory Alloction Failed '%s'\n", __FUNCTION__);
		CcspTraceError(("Memory Alloction Failed '%s'\n", __FUNCTION__));
		return ANSC_STATUS_FAILURE;
	}  
}

ANSC_STATUS Mta_Sysevent_thread(ANSC_HANDLE  hThisObject)

{

 PCOSA_DATAMODEL_MTA      pMyObject    = (PCOSA_DATAMODEL_MTA)hThisObject;
#if 0
 static int sysevent_fd;
         static token_t sysevent_token;
         sysevent_fd = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "WAN State", &sysevent_token);
#endif
         unsigned char current_wan_state[10] = {0};
        async_id_t getnotification_asyncid;
          int err;
          unsigned char name[25]={0}, val[10]={0};
          int namelen=0, vallen=0;
         async_id_t wan_state_asyncid;
         sysevent_set_options(sysevent_fd, sysevent_token, "current_wan_state", TUPLE_FLAG_EVENT);
         sysevent_setnotification(sysevent_fd, sysevent_token, "current_wan_state",  &wan_state_asyncid);

         sysevent_get(sysevent_fd, sysevent_token, "current_wan_state", current_wan_state, sizeof(current_wan_state));
         if(strcmp(current_wan_state, "up") == 0)
         {
                CcspTraceWarning(("%s current_wan_state up, Initializing MTA \n",__FUNCTION__));

      		CosaMTAInitializeEthWanProv(pMyObject);

         }

        while (1)
        {

          	namelen = sizeof(name);
          	vallen  = sizeof(val);
			memset (name, 0, sizeof(name));
			memset (val, 0, sizeof(val));
                err = sysevent_getnotification(sysevent_fd, sysevent_token, name, &namelen, val, &vallen, &getnotification_asyncid);

                if (!err)
                {
                        CcspTraceWarning(("%s Recieved notification event  %s, state %s\n",__FUNCTION__,name,val));
                        if(( strcmp(name,"current_wan_state") == 0 ) && ( strcmp(val, "up") == 0 ) )
                        {

                            CcspTraceWarning(("%s Initializing/Reinitializing MTA\n",__FUNCTION__));
                            CosaMTAInitializeEthWanProv(pMyObject);
                        }

                }
        }



}


/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaMTAInitialize
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
CosaMTAInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                  returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_MTA          pMyObject    = (PCOSA_DATAMODEL_MTA)hThisObject;
    char isEthEnabled[64]={'\0'};
    /* Initiation all functions */

    /* Initialize middle layer for Device.DeviceInfo.  */
    CosaDmlMTAInit(NULL, (PANSC_HANDLE)pMyObject);

    if (syscfg_init() != 0)
    {
        CcspTraceError(("syscfg init Failed '%s'\n", __FUNCTION__));
    }

#ifdef _COSA_BCM_ARM_ 

   if( (0 == syscfg_get( NULL, "eth_wan_enabled", isEthEnabled, sizeof(isEthEnabled))) && ((isEthEnabled[0] != '\0') && (strncmp(isEthEnabled, "true", strlen("true")) == 0)))
        {
        	sysevent_fd = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "WAN State", &sysevent_token);
    		pthread_t MtaInit;
	 	#if defined (EROUTER_DHCP_OPTION_MTA)
			pthread_create(&MtaInit, NULL, &Mta_Sysevent_thread_Dhcp_Option, (ANSC_HANDLE) hThisObject);
		#else
		 	pthread_create(&MtaInit, NULL, &Mta_Sysevent_thread, (ANSC_HANDLE) hThisObject);
		#endif
	}
   else { 
             printf("\nEthernet Wan mode is disabled\n");  
         }
  //  CosaMTAInitializeEthWanProv(hThisObject);

#endif
    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaMTARemove
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
CosaMTARemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_MTA             pMyObject    = (PCOSA_DATAMODEL_MTA)hThisObject;

    /* Remove necessary resounce */

    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return returnStatus;
}


//#endif
