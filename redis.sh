#!/usr/bin/env bash

if [[ -z "$1" ]]; then
    echo "Syntax: redis.sh [start|stop|restart|reload]"
fi

cmd=$1

if [[ "${cmd}" == "reload" ]]; then
    systemctl reload redis-publisher@pub-alarm.service
    systemctl reload redis-publisher@pub-connection.service
    systemctl reload redis-publisher@pub-telemetry.service
    systemctl reload redis-publisher@pub-vlan.service
    systemctl reload redis-publisher@pub-weather.service
else
    systemctl ${cmd} redis-publisher@pub-alarm.service
    systemctl ${cmd} redis-publisher@pub-connection.service
    systemctl ${cmd} redis-publisher@pub-telemetry.service
    systemctl ${cmd} redis-publisher@pub-vlan.service
    systemctl ${cmd} redis-publisher@pub-weather.service

    systemctl ${cmd} redis-subscriber@sub-profile-1.service
    systemctl ${cmd} redis-subscriber@sub-profile-2.service
    systemctl ${cmd} redis-subscriber@sub-profile-3.service
    systemctl ${cmd} redis-subscriber@sub-profile-4.service
    systemctl ${cmd} redis-subscriber@sub-profile-5.service
fi
