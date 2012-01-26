#!/bin/sh
. /cgi-bin/header $0\?$QUERY_STRING;header "ao32" "ao32"
eval $QUERY_STRING

if [ "$SLOT" = "" ]; then
	SLOT=$(cat /sys/class/ao32cpci/ao32cpci.0/device/cpci_slot)
fi

hook_gencb_pre "/cgi-bin/ao32_dump $SLOT"

footer ao32.cgi

