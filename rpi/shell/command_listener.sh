#!/bin/sh
PORT=9001
SLEEP=4

while [ -f $LOCK ]
do
    command=$(nc -l -p $PORT)
    
    case $command in
	"stop" )
	    sudo /etc/init.d/sailersensord stop
	    sleep $SLEEP
	;;
	"start" )
	    sudo /etc/init.d/sailersensord start
	;;
	"reboot" )
	    sudo /etc/init.d/sailersensord stop
	    sleep $SLEEP
	    sudo reboot
	;;
	"shutdown" )
	    sudo /etc/init.d/sailersensord stop
	    sleep $SLEEP
	    sudo shutdown -h now
	;;
    esac

    sleep 3
done