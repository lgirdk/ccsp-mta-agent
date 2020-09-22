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

#ifdef __GNUC__
#if (!defined _BUILD_ANDROID) && (!defined _NO_EXECINFO_H_)
#include <execinfo.h>
#endif
#endif

#include "ssp_global.h"
#include "stdlib.h"
#ifdef INCLUDE_BREAKPAD
#include "breakpad_wrapper.h"
#endif
#include "ccsp_dm_api.h"
#ifdef USE_PCD_API_EXCEPTION_HANDLING
#include "pcdapi.h"
#endif
//#include <docsis_ext_interface.h>
#include "safec_lib_common.h"

#define DEBUG_INI_NAME  "/etc/debug.ini"



#ifdef MTA_TR104SUPPORT
#include "TR104.h"
#endif
PDSLH_CPE_CONTROLLER_OBJECT     pDslhCpeController      = NULL;
PCOMPONENT_COMMON_DM            g_pComponent_Common_Dm  = NULL;
char                            g_Subsystem[32]         = {0};
PCCSP_COMPONENT_CFG             gpPnmStartCfg           = NULL;
PCCSP_FC_CONTEXT                pPnmFcContext           = (PCCSP_FC_CONTEXT           )NULL;
PCCSP_CCD_INTERFACE             pPnmCcdIf               = (PCCSP_CCD_INTERFACE        )NULL;
PCCC_MBI_INTERFACE              pPnmMbiIf               = (PCCC_MBI_INTERFACE         )NULL;
BOOL                            g_bActive               = FALSE;

int  cmd_dispatch(int  command)
{
    ULONG                           ulInsNumber        = 0;
    parameterValStruct_t            val[3]             = {0};
    char*                           pParamNames[]      = {"Device.X_CISCO_COM_MTA."};
    parameterValStruct_t**          ppReturnVal        = NULL;
    parameterInfoStruct_t**         ppReturnValNames   = NULL;
    parameterAttributeStruct_t**    ppReturnvalAttr    = NULL;
    ULONG                           ulReturnValCount   = 0;
    ULONG                           i                  = 0;

    switch ( command )
    {
            case	'e' :

#ifdef _ANSC_LINUX
                CcspTraceInfo(("Connect to bus daemon...\n"));

            {
                char                            CName[256];

                if ( g_Subsystem[0] != 0 )
                {
                    _ansc_sprintf(CName, "%s%s", g_Subsystem, gpPnmStartCfg->ComponentId);
                }
                else
                {
                    _ansc_sprintf(CName, "%s", gpPnmStartCfg->ComponentId);
                }

                ssp_PnmMbi_MessageBusEngage
                    ( 
                        CName,
                        CCSP_MSG_BUS_CFG,
                        gpPnmStartCfg->DbusPath
                    );
            }

#endif

                ssp_create_pnm(gpPnmStartCfg);
                ssp_engage_pnm(gpPnmStartCfg);

                g_bActive = TRUE;

                CcspTraceInfo(("MTA Agent Module loaded successfully...\n"));

            break;

            case    'r' :

            CcspCcMbi_GetParameterValues
                (
                    DSLH_MPA_ACCESS_CONTROL_ACS,
                    pParamNames,
                    1,
                    &ulReturnValCount,
                    &ppReturnVal,
                    NULL
                );



            for ( i = 0; i < ulReturnValCount; i++ )
            {
                CcspTraceWarning(("Parameter %d name: %s value: %s \n", i+1, ppReturnVal[i]->parameterName, ppReturnVal[i]->parameterValue));
            }
            
                break;

        case    'm':

                AnscPrintComponentMemoryTable(pComponentName);

                break;

        case    't':

                AnscTraceMemoryTable();

                break;

        case    'c':

                ssp_cancel_pnm(gpPnmStartCfg);

                break;

        default:
            break;
    }

    return 0;
}

static void _print_stack_backtrace(void)
{
#ifdef __GNUC__
#if (!defined _BUILD_ANDROID) && (!defined _NO_EXECINFO_H_) && (!defined _COSA_SIM_)
        void* tracePtrs[100];
        char** funcNames = NULL;
        int i, count = 0;

        int fd;
        const char* path = "/nvram/MTaAgentSsp_backtrace";
        fd = open(path, O_RDWR | O_CREAT);
        if (fd < 0)
        {
            fprintf(stderr, "failed to open backtrace file: %s", path);
            return;
        }

        count = backtrace( tracePtrs, 100 );

        backtrace_symbols_fd( tracePtrs, count, fd);
   
        close(fd);
 
        funcNames = backtrace_symbols( tracePtrs, count );

        if ( funcNames ) {
            // Print the stack trace
            for( i = 0; i < count; i++ )
                printf("%s\n", funcNames[i] );

            // Free the string pointers
            free( funcNames );
        }
#endif
#endif
}

#if defined(_ANSC_LINUX)
static void daemonize(void) {
	int fd;
	switch (fork()) {
	case 0:
		break;
	case -1:
		// Error
		CcspTraceInfo(("Error daemonizing (fork)! %d - %s\n", errno, strerror(
				errno)));
		exit(0);
		break;
	default:
		_exit(0);
	}

	if (setsid() < 	0) {
		CcspTraceInfo(("Error demonizing (setsid)! %d - %s\n", errno, strerror(errno)));
		exit(0);
	}

    /*
     *  What is the point to change current directory?
     *
    chdir("/");
     */

#ifndef  _DEBUG

	fd = open("/dev/null", O_RDONLY);
	if (fd != 0) {
		dup2(fd, 0);
		close(fd);
	}
	fd = open("/dev/null", O_WRONLY);
	if (fd != 1) {
		dup2(fd, 1);
		close(fd);
	}
	fd = open("/dev/null", O_WRONLY);
	if (fd != 2) {
		dup2(fd, 2);
		close(fd);
	}
#endif
}

void sig_handler(int sig)
{

    CcspBaseIf_deadlock_detection_log_print(sig);
	extern ANSC_HANDLE bus_handle;
    if ( sig == SIGINT ) {
    	signal(SIGINT, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGINT received!\n"));
        exit(0);
    }
    else if ( sig == SIGUSR1 ) {
    	signal(SIGUSR1, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGUSR1 received!\n"));
    }
    else if ( sig == SIGUSR2 ) {
    	CcspTraceInfo(("SIGUSR2 received!\n"));
    }
    else if ( sig == SIGCHLD ) {
    	signal(SIGCHLD, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGCHLD received!\n"));
    }
    else if ( sig == SIGPIPE ) {
    	signal(SIGPIPE, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGPIPE received!\n"));
    }
    else if ( sig == SIGTERM )
    {
        CcspTraceInfo(("SIGTERM received!\n"));
        exit(0);
    }
    else if ( sig == SIGKILL )
    {
        CcspTraceInfo(("SIGKILL received!\n"));
        exit(0);
    }
	else if ( sig == SIGALRM ) {

    	signal(SIGALRM, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGALRM received!\n"));
		#ifndef DISABLE_LOGAGENT
		RDKLogEnable = GetLogInfo(bus_handle,"eRT.","Device.LogAgent.X_RDKCENTRAL-COM_LoggerEnable");
		RDKLogLevel = (char)GetLogInfo(bus_handle,"eRT.","Device.LogAgent.X_RDKCENTRAL-COM_LogLevel");
		MTA_RDKLogLevel = GetLogInfo(bus_handle,"eRT.","Device.LogAgent.X_RDKCENTRAL-COM_MTA_LogLevel");
		MTA_RDKLogEnable = (char)GetLogInfo(bus_handle,"eRT.","Device.LogAgent.X_RDKCENTRAL-COM_MTA_LoggerEnable");
		#endif
}
    else {
    	/* get stack trace first */
    	_print_stack_backtrace();
    	CcspTraceInfo(("Signal %d received, exiting!\n", sig));
    	exit(0);
    }
}

static int is_core_dump_opened(void)
{
    FILE *fp;
    char path[256];
    char line[1024];
    char *start, *tok, *sp;
#define TITLE   "Max core file size"

    snprintf(path, sizeof(path), "/proc/%d/limits", getpid());
    if ((fp = fopen(path, "rb")) == NULL)
        return 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        if ((start = strstr(line, TITLE)) == NULL)
            continue;

        start += strlen(TITLE);
        if ((tok = strtok_r(start, " \t\r\n", &sp)) == NULL)
            break;

        fclose(fp);
        return (tok[0] == '0' && tok[1] == '\0') ? 0 : 1;
    }

    fclose(fp);
    return 0;
}

int write_pid_file(const char *file)
{
    FILE *fp;
    pid_t pid;

    if ((fp = fopen(file, "w")) == NULL)
        return -1;

    pid = getpid();
    fprintf(fp, "%d\n", pid);
    fclose(fp);
    return 0;
}

#endif

int main(int argc, char* argv[])
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    int                             cmdChar            = 0;
    BOOL                            bRunAsDaemon       = TRUE;
    int                             idx                = 0;
    char                            cmd[1024]          = {0};
    FILE                           *fd                 = NULL;
    DmErr_t                         err;
    char                            *subSys            = NULL;
    extern ANSC_HANDLE bus_handle;
    errno_t rc       = -1;
    int     ind      = -1;

#ifdef FEATURE_SUPPORT_RDKLOG
    RDK_LOGGER_INIT();
#endif

    /*
     *  Load the start configuration
     */
    gpPnmStartCfg = (PCCSP_COMPONENT_CFG)AnscAllocateMemory(sizeof(CCSP_COMPONENT_CFG));
    
    if ( gpPnmStartCfg )
    {
        CcspComponentLoadCfg(CCSP_PNM_START_CFG_FILE, gpPnmStartCfg);
    }
    else
    {
        printf("Insufficient resources for start configuration, quit!\n");
        exit(1);
    }
    
    /* Set the global pComponentName */
    pComponentName = gpPnmStartCfg->ComponentName;

#if defined(_DEBUG) && defined(_COSA_SIM_)
    AnscSetTraceLevel(CCSP_TRACE_LEVEL_INFO);
#endif

    for (idx = 1; idx < argc; idx++)
    {
        rc = strcmp_s("-subsys",strlen("-subsys"),argv[idx],&ind);
        ERR_CHK(rc);
        if ((!ind) && (rc == EOK))
        {
            rc = strcpy_s(g_Subsystem,sizeof(g_Subsystem),argv[idx+1]);
            if(rc != EOK)
            {
               ERR_CHK(rc);
               return ANSC_STATUS_FAILURE;
            }
            CcspTraceWarning(("\nSubsystem is %s\n", g_Subsystem));
         }
        else
        {
           rc = strcmp_s("-c",strlen("-c"),argv[idx],&ind);
           ERR_CHK(rc);
           
            if ((!ind) && (rc == EOK))
            {
               bRunAsDaemon = FALSE;
            }
         }
    }

#if  defined(_ANSC_WINDOWSNT)

    AnscStartupSocketWrapper(NULL);

    display_info();

    cmd_dispatch('e');

    while ( cmdChar != 'q' )
    {
        cmdChar = getchar();
        /*Coverity Fix CID:79619 CHECKED_RETURN */
           if (cmdChar == EOF ) 
           {  
                  CcspTraceWarning(("Error:%s:%d cmdChar return as EOF \n",__FUNCTION__,__LINE__));
                  
           }
           else
           {
                 cmd_dispatch(cmdChar);
           }
                    
    }
    
#elif defined(_ANSC_LINUX)

    if ( bRunAsDaemon )
        daemonize();

    /*This is used for ccsp recovery manager */
    if (write_pid_file("/var/tmp/CcspMtaAgent.pid") != 0)
        fprintf(stderr, "%s: fail to write PID file\n", argv[0]);
#ifdef INCLUDE_BREAKPAD
    breakpad_ExceptionHandler();
#else
    if (is_core_dump_opened())
    {
        signal(SIGUSR1, sig_handler);
        CcspTraceWarning(("Core dump is opened, do not catch signal\n"));
    }
    else
    {
        CcspTraceWarning(("Core dump is NOT opened, backtrace if possible\n"));
    signal(SIGTERM, sig_handler);
    signal(SIGINT, sig_handler);
    signal(SIGUSR1, sig_handler);
    signal(SIGUSR2, sig_handler);

    signal(SIGSEGV, sig_handler);
    signal(SIGBUS, sig_handler);
    signal(SIGKILL, sig_handler);
    signal(SIGFPE, sig_handler);
    signal(SIGILL, sig_handler);
    signal(SIGQUIT, sig_handler);
    signal(SIGHUP, sig_handler);
	signal(SIGALRM, sig_handler);
    }
#endif

#ifdef USE_PCD_API_EXCEPTION_HANDLING
    printf("Registering PCD exception handler CcspMTAAgent\n");
    PCD_api_register_exception_handlers( argv[0], NULL );
#endif

    cmd_dispatch('e');

	#ifndef DISABLE_LOGAGENT
	RDKLogEnable = GetLogInfo(bus_handle,"eRT.","Device.LogAgent.X_RDKCENTRAL-COM_LoggerEnable");
	RDKLogLevel = (char)GetLogInfo(bus_handle,"eRT.","Device.LogAgent.X_RDKCENTRAL-COM_LogLevel");
	MTA_RDKLogLevel = GetLogInfo(bus_handle,"eRT.","Device.LogAgent.X_RDKCENTRAL-COM_MTA_LogLevel");
	MTA_RDKLogEnable = (char)GetLogInfo(bus_handle,"eRT.","Device.LogAgent.X_RDKCENTRAL-COM_MTA_LoggerEnable");
	#endif
// printf("Calling Docsis\n");

    // ICC_init();
    // DocsisIf_StartDocsisManager();

#ifdef _COSA_SIM_
    subSys = "";        /* PC simu use empty string as subsystem */
#else
    subSys = NULL;      /* use default sub-system */
#endif
    err = Cdm_Init(bus_handle, subSys, NULL, NULL, pComponentName);
    if (err != CCSP_SUCCESS)
    {
        fprintf(stderr, "Cdm_Init: %s\n", Cdm_StrError(err));
        exit(1);
    }

    system("touch /tmp/mta_initialized");

    CcspTraceWarning(("RDKB_SYSTEM_BOOT_UP_LOG : Entering MTA loop\n"));
    printf("Entering MTA loop\n");

    if ( bRunAsDaemon )
    {
        while(1)
        {
            sleep(30);
        }
    }
    else
    {
        while ( cmdChar != 'q' )
        {
            cmdChar = getchar();

        /*Coverity Fix CID:79619 CHECKED_RETURN */
            if (cmdChar == EOF ) 
           {  
                  CcspTraceWarning(("Error:%s:%d cmdChar return as EOF \n",__FUNCTION__,__LINE__));
                  
           }
           else
           {
                 cmd_dispatch(cmdChar);
           }
      
        }
    }
#endif

    err = Cdm_Term();
    if (err != CCSP_SUCCESS)
    {
        fprintf(stderr, "Cdm_Term: %s\n", Cdm_StrError(err));
        exit(1);
    }

    if ( g_bActive )
    {
        ssp_cancel_pnm(gpPnmStartCfg);

        g_bActive = FALSE;
    }

    return 0;
}


