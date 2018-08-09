#!/usr/bin/env bash

if [[ -z "$1" ]]; then
    echo "Syntax: zeromq.sh [start|stop|restart|reload]"
fi

cmd=$1

if [[ "${cmd}" == "reload" ]]; then
    systemctl reload zeromq-publisher@pub-alarm.service
    systemctl reload zeromq-publisher@pub-connection.service
    systemctl reload zeromq-publisher@pub-telemetry.service
    systemctl reload zeromq-publisher@pub-vlan.service
    systemctl reload zeromq-publisher@pub-weather.service
else
    systemctl ${cmd} zeromq-publisher@pub-alarm.service
    systemctl ${cmd} zeromq-publisher@pub-connection.service
    systemctl ${cmd} zeromq-publisher@pub-telemetry.service
    systemctl ${cmd} zeromq-publisher@pub-vlan.service
    systemctl ${cmd} zeromq-publisher@pub-weather.service

    systemctl ${cmd} zeromq-subscriber@sub-profile-1.service
    systemctl ${cmd} zeromq-subscriber@sub-profile-2.service
    systemctl ${cmd} zeromq-subscriber@sub-profile-3.service
    systemctl ${cmd} zeromq-subscriber@sub-profile-4.service
    systemctl ${cmd} zeromq-subscriber@sub-profile-5.service
fi
