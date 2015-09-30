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

#ifndef MTA_HAL_SHORT_VALUE_LEN
#define  MTA_HAL_SHORT_VALUE_LEN   16
#endif

#ifndef MTA_HAL_LONG_VALUE_LEN
#define  MTA_HAL_LONG_VALUE_LEN   64
#endif

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
    char                            ServiceClassName[256];
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
    char                            PIN[64];
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
    char                            OperatingTN[64];
    char                            SupportedTN[64];
}
COSA_MTA_HANDSETS_INFO,  *PCOSA_MTA_HANDSETS_INFO;

typedef  struct
_COSA_MTA_CALLS
{
    CHAR                            Codec[64];
    CHAR                            RemoteCodec[64];
    CHAR                            CallStartTime[64];
    CHAR                            CallEndTime[64];
    CHAR                            CWErrorRate[MTA_HAL_SHORT_VALUE_LEN];
    CHAR                            PktLossConcealment[MTA_HAL_SHORT_VALUE_LEN];
    BOOLEAN                         JitterBufferAdaptive;
    BOOLEAN                         Originator;
    ANSC_IPV4_ADDRESS               RemoteIPAddress;
    ULONG                           CallDuration;
    CHAR                            CWErrors[16];                     /* code word errors on this channel */
    CHAR                            SNR[16];                          /* signal to noise ratio * 256 */
    CHAR                            MicroReflections[16];             /* return loss measurement */
    CHAR                            DownstreamPower[16];              /* downstream power in dbmv */
    CHAR                            UpstreamPower[16];                /* upstream power in dbmv */
    CHAR                            EQIAverage[16];                   /* EQI average */    
    CHAR                            EQIMinimum[16];                   /* EQI minimum */
    CHAR                            EQIMaximum[16];                   /* EQI maximum */
    CHAR                            EQIInstantaneous[16];             /* EQI instantaneous */
    CHAR                            MOS_LQ[16];                       /* mean opinion score of listening quality, 10-50 */
    CHAR                            MOS_CQ[16];                       /* mean opinion score of conversational quality, 10-50 */
    CHAR                            EchoReturnLoss[16];               /* residual echo return loss, in db */
    CHAR                            SignalLevel[16];                  /* voice signal relative level, in db */
    CHAR                            NoiseLevel[16];                   /* noise relative level, in db */
    CHAR                            LossRate[16];                     /* fraction of RTP data packet loss * 256 */
    CHAR                            DiscardRate[16];                  /* fraction of RTP data packet discarded * 256 */
    CHAR                            BurstDensity[16];                 /* fraction of bursting data packet * 256 */
    CHAR                            GapDensity[16];                   /* fraction of packets within inter-burst gap * 256 */
    CHAR                            BurstDuration[16];                /* mean duration of bursts, in milliseconds */
    CHAR                            GapDuration[16];                  /* mean duration of gaps, in milliseconds */
    CHAR                            RoundTripDelay[16];               /* most recent measured RTD, in milliseconds */
    CHAR                            Gmin[16];                         /* local gap threshold */
    CHAR                            RFactor[16];                      /* voice quality evaluation for this RTP session */
    CHAR                            ExternalRFactor[16];              /* voice quality evaluation for segment on network external to this RTP session */
    CHAR                            JitterBufRate[16];                /* adjustment rate of jitter buffer, in milliseconds */
    CHAR                            JBNominalDelay[16];               /* nominal jitter buffer length, in milliseconds */
    CHAR                            JBMaxDelay[16];                   /* maximum jitter buffer length, in milliseconds */
    CHAR                            JBAbsMaxDelay[16];                /* absolute maximum delay, in milliseconds */
    CHAR                            TxPackets[16];                    /* count of transmitted packets */
    CHAR                            TxOctets[16];                     /* count of transmitted octet packets */
    CHAR                            RxPackets[16];                    /* count of received packets */
    CHAR                            RxOctets[16];                     /* count of received octet packets */
    CHAR                            PacketLoss[16];                   /* count of lost packets */
    CHAR                            IntervalJitter[16];               /* stat variance of packet interarrival time, in milliseconds */
    CHAR                            RemoteIntervalJitter[16];         /* remote sie IntervalJitter (see local side) */
    CHAR                            RemoteMOS_LQ[16];                 /* remote side MOS_LQ (see local side) */
    CHAR                            RemoteMOS_CQ[16];                 /* remote side MOS_CQ (see local side) */
    CHAR                            RemoteEchoReturnLoss[16];         /* remote side EchoReturnLoss (see local side) */
    CHAR                            RemoteSignalLevel[16];            /* remote side SignalLevel (see local side) */
    CHAR                            RemoteNoiseLevel[16];             /* remote side NoiseLevel (see local side) */
    CHAR                            RemoteLossRate[16];               /* remote side LossRate (see local side) */
    CHAR                            RemotePktLossConcealment[16];     /* remote side PktLossConcealment (see local side) */
    CHAR                            RemoteDiscardRate[16];            /* remote side DiscardRate (see local side) */
    CHAR                            RemoteBurstDensity[16];           /* remote side BurstDensity (see local side) */
    CHAR                            RemoteGapDensity[16];             /* remote side GapDensity (see local side) */
    CHAR                            RemoteBurstDuration[16];          /* remote side BurstDuration (see local side) */
    CHAR                            RemoteGapDuration[16];            /* remote side GapDuration (see local side) */
    CHAR                            RemoteRoundTripDelay[16];         /* remote side RoundTripDelay (see local side) */
    CHAR                            RemoteGmin[16];                   /* remote side Gmin (see local side) */
    CHAR                            RemoteRFactor[16];                /* remote side RFactore (see local side) */
    CHAR                            RemoteExternalRFactor[16];        /* remote side ExternalRFactor (see local side) */
    BOOLEAN                         RemoteJitterBufferAdaptive;   	  /* remote side JitterBufferAdaptive (see local side) */
    CHAR                            RemoteJitterBufRate[MTA_HAL_SHORT_VALUE_LEN];          /* remote side JitterBufRate (see local side) */
    CHAR                            RemoteJBNominalDelay[MTA_HAL_SHORT_VALUE_LEN];         /* remote side JBNominalDelay (see local side) */
    CHAR                            RemoteJBMaxDelay[MTA_HAL_SHORT_VALUE_LEN];             /* remote side JBMaxDelay (see local side) */
    CHAR                            RemoteJBAbsMaxDelay[MTA_HAL_SHORT_VALUE_LEN];          /* remote side JBAbsMaxDelay (see local side) */
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
CosaDmlMTADectSetEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     bBool
    );

ANSC_STATUS
CosaDmlMTADectGetRegistrationMode
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     *pBool
    );

ANSC_STATUS
CosaDmlMTADectSetRegistrationMode
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     bBool
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
CosaDmlMTAGetDectPIN
    (
        ANSC_HANDLE                 hContext,
        char                        *pPINString
    );

ANSC_STATUS
CosaDmlMTASetDectPIN
    (
        ANSC_HANDLE                 hContext,
        char                        *pPINString
    );

ANSC_STATUS
CosaDmlMTAGetHandsets
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_MTA_HANDSETS_INFO     *ppHandsets
    );

ANSC_STATUS
CosaDmlMTASetHandsets
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_HANDSETS_INFO     pHandsets
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
