#!/bin/sh
PORT=9001

while [ -f $LOCK ]
do
    command=$(nc -l -p $PORT)
    
    case $command in
	"stop" )
	    sudo /etc/init.d/sailersensord stop
	;;
	"start" )
	    sudo /etc/init.d/sailersensord start
	;;
	"reboot" )
	    sudo /etc/init.d/sailersensord stop
	    sleep 2
	    sudo reboot
	;;
	"shutdown" )
	    sudo /etc/init.d/sailersensord stop
	    sleep 2
	    sudo shutdown -h now
	;;
    esac

    sleep 3
done