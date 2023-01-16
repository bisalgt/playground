#!/usr/bin/sh


STATUS="$(systemctl is-active redpitaya_scpi.service)"
if [ "${STATUS}" = "active" ]; then
    echo "Execute your tasks ....."
else 
    echo " Service not running.... so exiting "  
    exit 1  
fi