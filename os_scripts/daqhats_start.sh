#!/bin/bash
echo "start daqhats instrument service. Wait 5 seconds to complete ..."
/home/pi/bin/daqhats_service 2501 2502 0 &
sleep 5
echo "start daqhats scpi service."
/home/pi/bin/daqhats_scpi_socket 2500 2501 &
echo "start sequence completed"

