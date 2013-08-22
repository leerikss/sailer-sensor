#!/bin/sh
PORT=9001
LOG=/var/log/sailersensor.log

log()
{
    NOW=$(date +"[%d.%m.%Y %H:%M:%S]")
    echo "$NOW [command-listener.sh] $1" >> $LOG 2>&1
}

while [ -f $LOCK ]
do
    command=$(nc -l -p $PORT)
    
    case $command in
	"stop" )
	    log "Stopping sailersensor..."
	    sudo /etc/init.d/sailersensor stop
	;;
	"start" )
	    log "Starting sailersensor..."
	    sudo /etc/init.d/sailersensor start
	;;
	"reboot" )
	    log "Rebooting device!"
	    sudo reboot
	;;
	"shutdown" )
	    log "Shutting down device!"
	    sudo shutdown -h now
	;;
    esac

    sleep 3
done