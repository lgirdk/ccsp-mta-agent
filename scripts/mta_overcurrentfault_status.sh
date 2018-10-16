#! /bin/sh

source /etc/utopia/service.d/log_capture_path.sh

# Checking MTA's PID
MTA_PID=`pidof CcspMtaAgentSsp`
if [ "$MTA_PID" = "" ]; then
   #MTA process is not running so no need to proceed further	
   exit	
fi

# Getting MTA Line entries
echo_t "Obtaining the MTA Lines"
mta_total_lines=`dmcli eRT getv Device.X_CISCO_COM_MTA.LineTableNumberOfEntries | grep uint | awk '{print $5}'`
echo_t "MTA Total_no_of_lines: $mta_total_lines"

if [ "$mta_total_lines" != "" ]; then

	loop_count=1
	while [ $loop_count -le $mta_total_lines ]
	do
		mta_OverCurrentFault=`dmcli eRT getv Device.X_CISCO_COM_MTA.LineTable.$loop_count.OverCurrentFault | grep string | awk '{print $5}'`
		echo_t "MTA_LINE_"$loop_count"_OVER_CURRENT_STATUS:$mta_OverCurrentFault"

		loop_count=`expr $loop_count + 1`
	done

fi
