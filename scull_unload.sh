#!/bin/sh
module="scull"

/sbin/rmmod $module $* || exit 1

rm -f /dev/${device}[0-3]

