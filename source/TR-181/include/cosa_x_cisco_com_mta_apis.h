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

    module: cosa_x_cisco_com_mta_apis.h

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    copyright:

        Cisco Systems, Inc.
        All Rights Reserved.

    -------------------------------------------------------------------

    description:

        This file defines the apis for objects to support Data Model Library.

    -------------------------------------------------------------------


    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/11/2011    initial revision.

**************************************************************************/


#ifndef  _COSA_MTA_APIS_H
#define  _COSA_MTA_APIS_H

#include "../middle_layer_src/cosa_apis.h"
#include "../middle_layer_src/plugin_main_apis.h"

/**********************************************************************
                STRUCTURE AND CONSTANT DEFINITIONS
**********************************************************************/
typedef  struct
_COSA_MTA_DHCP_INFO
{
    ANSC_IPV4_ADDRESS               IPAddress;
    char                            BootFileName[64];
    char                            FQDN[64];
    ANSC_IPV4_ADDRESS               SubnetMask;
    ANSC_IPV4_ADDRESS               Gateway;
    ULONG                           LeaseTimeRemaining;
    char                            RebindTimeRemaining[64];
    char                            RenewTimeRemaining[64];
    ANSC_IPV4_ADDRESS               PrimaryDNS;
    ANSC_IPV4_ADDRESS               SecondaryDNS;
    char                            DHCPOption3[64];
    char                            DHCPOption6[64];
    char                            DHCPOption7[64];
    char                            DHCPOption8[64];
    char                            PCVersion[64];
    char                            MACAddress[64];
    ANSC_IPV4_ADDRESS               PrimaryDHCPServer;
    ANSC_IPV4_ADDRESS               SecondaryDHCPServer;
}
COSA_MTA_DHCP_INFO, *PCOSA_MTA_DHCP_INFO;

typedef  struct
_COSA_MTA_PKTC
{
    BOOLEAN                         pktcMtaDevEnabled;
    ULONG                           pktcSigDefCallSigTos;
    ULONG                           pktcSigDefMediaStreamTos;
    ULONG                           pktcMtaDevRealmOrgName;
    ULONG                           pktcMtaDevCmsKerbRealmName;
    ULONG                           pktcMtaDevCmsIpsecCtrl;
    ULONG                           pktcMtaDevCmsSolicitedKeyTimeout;
    ULONG                           pktcMtaDevRealmPkinitGracePeriod;
}
COSA_MTA_PKTC, *PCOSA_MTA_PKTC;

typedef  struct
_COSA_MTA_SERVICE_CLASS
{
    char                            ServiceClassName[64];
}
COSA_MTA_SERVICE_CLASS, *PCOSA_MTA_SERVICE_CLASS;

typedef  struct
_COSA_MTA_SERVICE_FLOW
{
    ULONG                           SFID;
    char                            ServiceClass[256];
    char                            Direction[16];
    ULONG                           ScheduleType;
    BOOLEAN                         DefaultFlow;
    ULONG                           NomGrantInterval;
    ULONG                           UnsolicitGrantSize;
    ULONG                           TolGrantJitter;
    ULONG                           NomPollInterval;
    ULONG                           MinReservedPkt;
    ULONG                           MaxTrafficRate;
    ULONG                           MinReservedRate;
    ULONG                           MaxTrafficBurst;
    char                            TrafficType[64];
    ULONG                           NumberOfPackets;
}
COSA_MTA_SERVICE_FLOW, *PCOSA_MTA_SERVICE_FLOW;

typedef  struct
_COSA_MTA_DECT
{
    ULONG                           RegisterDectHandset;
    ULONG                           DeregisterDectHandset;
    char                            HardwareVersion[64];
    char                            RFPI[64];
    char                            SoftwareVersion[64];
}
COSA_MTA_DECT,  *PCOSA_MTA_DECT;

typedef  struct
_COSA_MTA_HANDSETS_INFO
{
    ULONG                           InstanceNumber;
    BOOLEAN                         Status;
    char                            LastActiveTime[64];
    char                            HandsetName[64];
    char                            HandsetFirmware[64];
}
COSA_MTA_HANDSETS_INFO,  *PCOSA_MTA_HANDSETS_INFO;

typedef  struct
_COSA_MTA_CALLS
{
    char                            Codec[64];
    char                            RemoteCodec[64];
    char                            CallStartTime[64];
    char                            CallEndTime[64];
    char                            CWErrorRate[64];
    char                            PktLossConcealment[64];
    BOOLEAN                         JitterBufferAdaptive;
    BOOLEAN                         Originator;
    ANSC_IPV4_ADDRESS               RemoteIPAddress;
    ULONG                           CallDuration;
    ULONG                           CWErrors;
    ULONG                           SNR;
    ULONG                           MicroReflections;
    ULONG                           DownstreamPower;
    ULONG                           UpstreamPower;
    ULONG                           EQIAverage;
    ULONG                           EQIMinimum;
    ULONG                           EQIMaximum;
    ULONG                           EQIInstantaneous;
    ULONG                           MOS_LQ;
    ULONG                           MOS_CQ;
    ULONG                           EchoReturnLoss;
    ULONG                           SignalLevel;
    ULONG                           NoiseLevel;
    ULONG                           LossRate;
    ULONG                           DiscardRate;
    ULONG                           BurstDensity;
    ULONG                           GapDensity;
    ULONG                           BurstDuration;
    ULONG                           GapDuration;
    ULONG                           RoundTripDelay;
    ULONG                           Gmin;
    ULONG                           RFactor;
    ULONG                           ExternalRFactor;
    ULONG                           JitterBufRate;
    ULONG                           JBNominalDelay;
    ULONG                           JBMaxDelay;
    ULONG                           JBAbsMaxDelay;
    ULONG                           TxPackets;
    ULONG                           TxOctets;
    ULONG                           RxPackets;
    ULONG                           RxOctets;
    ULONG                           PacketLoss;
    ULONG                           IntervalJitter;
    ULONG                           RemoteIntervalJitter;
    ULONG                           RemoteMOS_LQ;
    ULONG                           RemoteMOS_CQ;
    ULONG                           RemoteEchoReturnLoss;
    ULONG                           RemoteSignalLevel;
    ULONG                           RemoteNoiseLevel;
    ULONG                           RemoteLossRate;
    char                            RemotePktLossConcealment[64];
    ULONG                           RemoteDiscardRate;
    ULONG                           RemoteBurstDensity;
    ULONG                           RemoteGapDensity;
    ULONG                           RemoteBurstDuration;
    ULONG                           RemoteGapDuration;
    ULONG                           RemoteRoundTripDelay;
    ULONG                           RemoteGmin;
    ULONG                           RemoteRFactor;
    ULONG                           RemoteExternalRFactor;
    BOOLEAN                         RemoteJitterBufferAdaptive;
    ULONG                           RemoteJitterBufRate;
    ULONG                           RemoteJBNominalDelay;
    ULONG                           RemoteJBMaxDelay;
    ULONG                           RemoteJBAbsMaxDelay;
}
COSA_MTA_CALLS, *PCOSA_MTA_CALLS;

typedef  struct
_COSA_MTA_LINETABLE_INFO
{
    ULONG                           InstanceNumber;

    ULONG                           LineNumber;
    ULONG                           Status; /* 1 = OnHook; 2 = OffHook */
    char                            HazardousPotential[64];
    char                            ForeignEMF[64];
    char                            ResistiveFaults[128];
    char                            ReceiverOffHook[64];
    char                            RingerEquivalency[64];
    char                            CAName[64];
    ULONG                           CAPort;
    ULONG                           MWD;
    ULONG                           CallsNumber;
    PCOSA_MTA_CALLS                 pCalls;
    ULONG                           CallsUpdateTime;
}
COSA_MTA_LINETABLE_INFO, *PCOSA_MTA_LINETABLE_INFO;

typedef  struct
_COSA_MTA_CAPPL
{
    char                            LCState[64];
    char                            CallPState[64];
    char                            LoopCurrent[64];
}
COSA_MTA_CAPPL,  *PCOSA_MTA_CAPPL;
    
typedef  struct
_COSA_MTA_DSXLOG
{
    char                            Time[64];
    char                            Description[128];
    ULONG                           ID;
    ULONG                           Level;
}
COSA_MTA_DSXLOG,  *PCOSA_MTA_DSXLOG;

/**********************************************************************
                FUNCTION PROTOTYPES
**********************************************************************/

ANSC_STATUS
CosaDmlMTAInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    );

ANSC_STATUS
CosaDmlMTAGetDHCPInfo
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_DHCP_INFO         pInfo
    );

ANSC_STATUS
CosaDmlMTAGetPktc
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_PKTC              pPktc
    );

ANSC_STATUS
CosaDmlMTASetPktc
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_PKTC              pPktc
    );

ULONG
CosaDmlMTALineTableGetNumberOfEntries
    (
        ANSC_HANDLE                 hContext
    );

ANSC_STATUS
CosaDmlMTALineTableGetEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        PCOSA_MTA_LINETABLE_INFO    pEntry
    );

ANSC_STATUS
CosaDmlMTATriggerDiagnostics
    (
        //ANSC_HANDLE               hContext
        ULONG                       nIndex
    );

ANSC_STATUS
CosaDmlMTAGetServiceClass
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_MTA_SERVICE_CLASS     *ppCfg
    );

ANSC_STATUS
CosaDmlMTAGetServiceFlow
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_MTA_SERVICE_FLOW      *ppCfg
    );

ANSC_STATUS
CosaDmlMTADectGetEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     *pBool
    );
        
ANSC_STATUS
CosaDmlMTADectGetRegistrationMode
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     *pBool
    );

ANSC_STATUS
CosaDmlMTAGetDect
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_DECT              pDect
    );

ANSC_STATUS
CosaDmlMTASetDect
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_DECT              pDect
    );

ULONG
CosaDmlMTAHandsetsGetNumberOfEntries
    (
        ANSC_HANDLE                 hContext
    );

ANSC_STATUS
CosaDmlMTAHandsetsGetEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        PCOSA_MTA_HANDSETS_INFO     pEntry
    );

ANSC_STATUS
CosaDmlMTAVQMResetStats
    (
        ANSC_HANDLE                 hContext
    );

ANSC_STATUS
CosaDmlMTAGetCalls
    (
        ANSC_HANDLE                 hContext,
        ULONG                       InstanceNumber, /* LineTable's instance number */
        PULONG                      pulCount,
        PCOSA_MTA_CALLS             *ppCfg
    );

ANSC_STATUS
CosaDmlMTAGetCALLP
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_CAPPL             pCallp
    );

ANSC_STATUS
CosaDmlMTAGetDSXLogs
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_MTA_DSXLOG            *ppDSXLog
    );

ANSC_STATUS
CosaDmlMTAGetDSXLogEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     *pBool
    );

ANSC_STATUS
CosaDmlMTASetDSXLogEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     Bool
    );

ANSC_STATUS
CosaDmlMTAClearDSXLog
    (
        ANSC_HANDLE                 hContext
    );

typedef  struct
_COSA_DML_MTA_LOG
{
    BOOLEAN                         EnableDECTLog;
    BOOLEAN                         EnableMTALog;
}
COSA_DML_MTA_LOG,  *PCOSA_DML_MTA_LOG;

typedef  struct
_COSA_DML_MTALOG_FULL
{
    ULONG                           Index;
    ULONG                           EventID;
    CHAR                            EventLevel[64];
    CHAR                            Time[64];
    PCHAR                           pDescription;
}
COSA_DML_MTALOG_FULL,  *PCOSA_DML_MTALOG_FULL;

typedef  struct
_COSA_DML_DECTLOG_FULL
{
    ULONG                           Index;
    ULONG                           EventID;
    ULONG                           EventLevel;
    CHAR                            Time[64];
    CHAR                            Description[256];
}
COSA_DML_DECTLOG_FULL,  *PCOSA_DML_DECTLOG_FULL;

ANSC_STATUS
CosaDmlMtaGetMtaLog
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_DML_MTALOG_FULL       *ppCfg        
    );

ANSC_STATUS
CosaDmlMtaGetDectLog
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_DML_DECTLOG_FULL      *ppCfg        
    );


// #ifdef CONFIG_TI_BBU

typedef  struct
_COSA_DML_BATTERY_INFO
{
    CHAR                            ModelNumber[32];
    CHAR                            SerialNumber[32];
    CHAR                            PartNumber[32];
    CHAR                            ChargerFirmwareRevision[32];
}
COSA_DML_BATTERY_INFO,  *PCOSA_DML_BATTERY_INFO;

ANSC_STATUS
CosaDmlMtaBatteryGetInstalled
    (
        ANSC_HANDLE                 hContext,
        PBOOL                       pValue
    );

ANSC_STATUS
CosaDmlMtaBatteryGetTotalCapacity
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    );

ANSC_STATUS
CosaDmlMtaBatteryGetActualCapacity
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    );

ANSC_STATUS
CosaDmlMtaBatteryGetRemainingCharge
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    );

ANSC_STATUS
CosaDmlMtaBatteryGetRemainingTime
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    );

ANSC_STATUS
CosaDmlMtaBatteryGetNumberofCycles
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    );

ANSC_STATUS
CosaDmlMtaBatteryGetPowerStatus
    (
        ANSC_HANDLE                 hContext,
        PCHAR                       pValue,
        PULONG                      pSize
    );

ANSC_STATUS
CosaDmlMtaBatteryGetCondition
    (
        ANSC_HANDLE                 hContext,
        PCHAR                       pValue,
        PULONG                      pSize
    );

ANSC_STATUS
CosaDmlMtaBatteryGetStatus
    (
        ANSC_HANDLE                 hContext,
        PCHAR                       pValue,
        PULONG                      pSize
    );

ANSC_STATUS
CosaDmlMtaBatteryGetLife
    (
        ANSC_HANDLE                 hContext,
        PCHAR                       pValue,
        PULONG                      pSize
    );

ANSC_STATUS
CosaDmlMtaBatteryGetInfo
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_BATTERY_INFO      pInfo
    );

// #endif /* CONFIG_TI_BBU */

#endif /* _COSA_MTA_APIS_H */

//#endif /* CONFIG_TI_PACM */
