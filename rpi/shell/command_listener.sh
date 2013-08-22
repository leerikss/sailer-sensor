#!/bin/sh
while [ -f $LOCK ]
do
    command=$(nc -l -p 9000)
    
    case $command in
	"stop" )
	    echo "Stopping sailersensor..."
#	    sudo /etc/init.d/sailersensor stop
	;;
	"start" )
	    echo "Starting sailersensor..."
#	    sudo /etc/init.d/sailersensor start
	;;
	"reboot" )
	    echo "Rebooting device!"
#	    sudo reboot
	;;
	"shutdown" )
	    echo "Shutting down device!"
#	    sudo shutdown -h now
	;;
    esac

    sleep 3
done