#!/bin/sh
#
# KUAL SailerSensor actions helper script
#
# $Id: usbnet.sh 9394 2013-07-02 01:40:46Z NiLuJe $
#
##

# Source external functions & config
. /mnt/us/usbnet/bin/libkh
. /mnt/us/extensions/sailersensor/scripts/config

# WiFi methods
enable_wifi()
{
    /etc/init.d/wifid stop
    /usr/sbin/wifid -n && echo $! >> /var/run/wifid.pid

    # According to a post this daemon interfers with the connection...
    /etc/init.d/netwatchd stop

    kh_msg "WiFi is now enabled          " I v
}

test_rpi_wifi()
{
    kh_msg "Testing...           " I v
    if ping_test $RPI_WLAN_IP ; then
	kh_msg "The RPI is connected!          " I v
    else
	kh_msg "The RPI is -NOT- connected          " I v
    fi
}

disable_wifi()
{
    kill -TERM $(cat /var/run/wifid.pid)
    rm /var/run/wifid.pid
    kh_msg "WiFi is now disabled          " I v
}

# Usb methods, nicked from Usbnetwork Hack Kual extensions script

usbnet_status()
{
    if check_is_in_usbnet "quiet" ; then
	kh_msg "USBNetwork is now enabled           " I v
    else
	kh_msg "USBNetwork is now disabled          " I v
    fi
}

## Try to toggle USBNetwork
toggle_usbnet()
{
	# All kinds of weird stuff happens if we try to toggle USBNet while plugged in, so, well, don't do it ;)
	if check_is_plugged_in ; then
		return 1
	fi

	kh_msg "Toggle USBNetwork" I v
	/mnt/us/usbnet/bin/usbnetwork

	# According to a post this daemon interfers with the connection...
	/etc/init.d/netwatchd stop

	usbnet_status
}

test_rpi_usbnet()
{
    kh_msg "Testing...           " I v
    if ping_test $RPI_USBNET_IP ; then
	kh_msg "The RPI is connected!          " I v
    else
	kh_msg "The RPI is -NOT- connected          " I v
    fi
}

prevent_screensaver()
{
    lipc-set-prop com.lab126.powerd preventScreenSaver 1
    kh_msg "Screensaver prevented          " I v
}

allow_screensaver()
{  
    lipc-set-prop com.lab126.powerd preventScreenSaver 0
    kh_msg "Screensaver allowed          " I v
}

stop_service()
{  
    kh_msg "Stopping Service         " I v
    rpi_send "stop"
}

start_service()
{  
    kh_msg "Starting Service         " I v
    rpi_send "start"
}

reboot_rpi()
{  
    kh_msg "Rebooting RPI         " I v
    rpi_send "reboot"
}

shutdown_rpi()
{  
    kh_msg "Shutting down RPI         " I v
    rpi_send "shutdown"
}

# Misc methods

rpi_send()
{
    echo $1 | nc $RPI_USBNET_IP $RPI_PORT
    echo $1 | nc $RPI_WLAN_IP $RPI_PORT
}

ping_test()
{
    ping -q -c1 $1 > /dev/null 2>&1
    if [ $? -eq 0 ] ; then
	return 0;
    else
	return 1;
    fi
}

## Check if we're plugged in to something
check_is_plugged_in()
{
	# Cheap trick. If we're plugged in, we're usually charging ;). NOTE: Only seems to be accurate while in USBNet mode... :/
	if [ "$(/usr/bin/lipc-get-prop -i -e -- com.lab126.powerd isCharging)" == "1" ] ; then
		kh_msg "will not toggle usbnet while plugged in, unplug your kindle" W v "must not be plugged in to safely do that"
		return 0
	fi

	# All god, (apparently) not plugged in to anything
	return 1
}

## Check if we're in USBMS mode
# Arg 1 enables a quiet detection (don't log anything)
check_is_in_usbnet()
{
	if lsmod | grep g_ether > /dev/null ; then
		if [ "${1}" != "quiet" ] ; then
			kh_msg "will not edit usbnet config file in usbnet mode, switch to usbms" W v "must be in usbms mode to safely do this"
		fi
		return 0
	fi

	# All good, we're in USBMS mode
	return 1
}

## Main
case "${1}" in
	"enable_wifi" )
		${1}
	;;
        "test_rpi_wifi" )
		${1}
	;;
	"disable_wifi" )
		${1}
	;;
        "toggle_usbnet" )
		${1}
	;;
        "test_rpi_usbnet" )
		${1}
	;;
        "prevent_screensaver" )
		${1}
	;;
        "allow_screensaver" )
		${1}
	;;
        "stop_service" )
		${1}
	;;
        "start_service" )
		${1}
	;;
        "reboot_rpi" )
		${1}
	;;
        "shutdown_rpi" )
		${1}
	;;
	* )
		kh_msg "invalid action (${1})" W v "invalid action"
	;;
esac
