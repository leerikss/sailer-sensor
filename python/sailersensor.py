#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2013 Leif Eriksson
#
# Author: Leif Eriksson <leif@leif.fi>

import sys
import gps
# import sqlite3
import socket
import logging
import logging.handlers
import time
from gpspoller import GpsPoller

# Logging
LOG_FILE='/home/pi/.sailersensor/sailersensor.log'
logger = logging.getLogger('SailerSensor.kindle')
logger.setLevel(logging.DEBUG) # Debug
# logger.setLevel(logging.ERROR) # Production
handler = logging.handlers.RotatingFileHandler(
          LOG_FILE, maxBytes=1048576, backupCount=3)
formatter = logging.Formatter('[%(asctime)s] [%(name)s] [%(levelname)s]: %(message)s')
handler.setFormatter(formatter)
logger.addHandler(handler)

# Database
#DB = "/home/pi/.sailersensor/kindle.db"
#DDL = """CREATE TABLE IF NOT EXISTS kindle(
#             id         INTEGER PRIMARY KEY,
#             speed      REAL,
#             heading    REAL,
#             heeling    REAL);"""
#GPS_COL_ID = 10

# Loop sleep in secs
LOOP_SLEEP=1

# Socket
SOCK_HOST = "192.168.2.2"
SOCK_TIMEOUT = 5
SOCK_PORT =    9000

# Nr conversions
MS_IN_KNOTS = 1.9438444924406

def main():

#    con = None

    gpsp = GpsPoller()

    try:
        gpsp.start()
        prev_rec = None

        while (True):

            try:
            # logger.debug('Connecting to db "%s"...' % DB)
            # con = get_con()
            # init_db(con)

            # logger.debug('Getting previous db record for ID "%s"...' % GPS_COL_ID)  
            # prv = get_prev_msg(con,GPS_COL_ID)
            # logger.debug('Previous record: %s' % prv)

                logger.debug('=========================')    
                logger.debug('main() while(True) starts')              
                logger.debug('Getting GPS sensor data...')  
                next_rec = get_gps_data( gpsp.get_current_value() )
                logger.debug('GPS sensor data: "%s"' % next_rec)  

                logger.debug('Building message...')  
                msg = get_msg_str(prev_rec,next_rec)
                logger.debug('Message: "%s"' % msg)  

                logger.debug('Sending message to Kindle (%s:%s)...' % (SOCK_HOST,SOCK_PORT) )  
                send_to_kindle(msg)
                
                logger.debug('Copying data for next round...')
                prev_rec = copyRec(next_rec)

                # logger.debug('Storing GPS data into DB...')  
                # store_msg(con, GPS_COL_ID, nxt)
                logger.debug('main() while(True) ends. All good.')  
            
            except Exception, e:
                logger.exception(e)
    
#        finally:
#            if con:
#                con.close()        

        # Loop
            time.sleep(LOOP_SLEEP)
    except (KeyboardInterrupt, SystemExit):
        logger.error("Sailersensor interrupted. Killing thread(s) and quitting.")
        gpsp.stop()
        

def get_con():
    con = sqlite3.connect(DB)
    return con

def init_db(con):
    cur = con.cursor()
    cur.execute(DDL)

def get_prev_msg(con,id):
    sql = "select speed,heading,heeling from kindle where id=?";
    cur = con.cursor()
    row = cur.execute(sql,[id]).fetchone()
    if row is not None:
        return {
            "speed": row[0],
            "heading": row[1],
            "heeling": row[2]
        }
    else:
        return None

def get_gps_data(packet):

    if( packet is None ):
        return {}

    return {
        "speed":     packet.get('speed', '?'),
        "heading":     packet.get('track', '?')
        }

def get_msg_str(prv,nxt):
    msg = build_msg( 'SP', prv, nxt, 'speed', MS_IN_KNOTS, 1 ) + \
        build_msg( 'HD', prv, nxt, 'heading' ) + \
        build_msg( 'HL', prv, nxt, 'heeling' )
    return msg

def build_msg(id,prv,nxt,attr,mult=1,dig=0):
    if (nxt is None) or ( nxt.get(attr,'?') == '?' ):
        return ''
    elif ( (prv is None) or prv.get(attr,'?') == '?' or prv[attr] <> nxt[attr] ):
        val = nxt[attr]
        if(mult <> 1):
            val = val * mult
        if(dig > 0):
            val = round(val,dig)
        else:
            val = int(val)
        return '%s%s ' % ( id, val)
    else:
        return ''

def send_to_kindle(msg):

    if len(msg) == 0:
        logger.debug('Message is empty. Not sending anything to Kindle')
        return

    try:
        sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        sock.settimeout(SOCK_TIMEOUT)
        sock.connect((SOCK_HOST,SOCK_PORT))
        sock.settimeout(None)
        sock.send(msg)
    except Exception, e:
        logger.exception(e)

def store_msg(con,id,data):
    cur = con.cursor()
    sql = "REPLACE INTO kindle(id,speed,heading,heeling) VALUES (?,?,?,?)"
    cur.execute(sql,(id,data['speed'],data['heading'],data['heeling']))
    con.commit()

def copyRec(oldRec):
    rec = {'speed':'?','heading':'?','heeling':'?'}
    if( oldRec.get('speed','?') != '?' ):
        rec['speed'] = oldRec['speed']
    if( oldRec.get('heading','?') != '?' ):
        rec['heading'] = oldRec['heading']
    if( oldRec.get('heeling','?') != '?' ):
        rec['heeling'] = oldRec['heeling']
    return rec

if __name__ == '__main__': 
    main() 
