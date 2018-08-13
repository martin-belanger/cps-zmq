#!/usr/bin/env bash

if [[ -z "$1" ]]; then
    echo "Syntax: nano.sh [start|stop|restart|reload]"
fi

cmd=$1

if [[ "${cmd}" == "reload" ]]; then
    systemctl reload nano-publisher@pub-alarm.service
    systemctl reload nano-publisher@pub-connection.service
    systemctl reload nano-publisher@pub-telemetry.service
    systemctl reload nano-publisher@pub-vlan.service
    systemctl reload nano-publisher@pub-weather.service
else
    systemctl ${cmd} nano-publisher@pub-alarm.service
    systemctl ${cmd} nano-publisher@pub-connection.service
    systemctl ${cmd} nano-publisher@pub-telemetry.service
    systemctl ${cmd} nano-publisher@pub-vlan.service
    systemctl ${cmd} nano-publisher@pub-weather.service

    systemctl ${cmd} nano-subscriber@sub-profile-1.service
    systemctl ${cmd} nano-subscriber@sub-profile-2.service
    systemctl ${cmd} nano-subscriber@sub-profile-3.service
    systemctl ${cmd} nano-subscriber@sub-profile-4.service
    systemctl ${cmd} nano-subscriber@sub-profile-5.service
fi
