#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2013 Leif Eriksson
#
# Author: Leif Eriksson <leif@leif.fi>

import sys
import gps
import sqlite3
import socket
import logging
import logging.handlers
import time

# Logging
LOG_FILE='/home/pi/.sailersensor/sailersensor.log'
logger = logging.getLogger('SailerSensor.kindle')
# logger.setLevel(logging.DEBUG) # Debug
logger.setLevel(logging.ERROR) # Production
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

# GPS
GPS_MIN_TRIES=5
GPS_MAX_TRIES=10

# Socket
SOCK_HOST = "192.168.2.2"
SOCK_TIMEOUT = 5
SOCK_PORT =    9000

def main():

#    con = None

    prev_rec = None

    while (True):
        try:
            # logger.debug('Connecting to db "%s"...' % DB)
            # con = get_con()
            # init_db(con)

            # logger.debug('Getting previous db record for ID "%s"...' % GPS_COL_ID)  
            # prv = get_prev_msg(con,GPS_COL_ID)
            # logger.debug('Previous record: %s' % prv)
        
            logger.debug('Getting GPS sensor data...')  
            next_rec = get_gps_data()
            logger.debug('GPS sensor data: %s' % next_rec)  

            logger.debug('Getting message...')  
            msg = get_msg_str(prev_rec,next_rec)
            logger.debug('Message: %s' % msg)  

            logger.debug('Sending message to Kindle (%s:%s)...' % (SOCK_HOST,SOCK_PORT) )  
            send_to_kindle(msg)

            prev_rec = next_rec

            # logger.debug('Storing GPS data into DB...')  
            # store_msg(con, GPS_COL_ID, nxt)
            # logger.debug('All successfull! Quitting.')  
            
        except Exception, e:
            logger.exception(e)
    
#        finally:
#            if con:
#                con.close()        

        # Loop
        time.sleep(LOOP_SLEEP)

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

def get_gps_data():

    i=1;
    session = gps.gps(mode=gps.WATCH_ENABLE)    

    while True:

        packet = session.next()

        if packet['class'] == 'TPV':

            session.close()

            ret = {
                "speed":     packet.get('speed', '?'),
                "heading":     packet.get('track', '?'),
                "heeling": 0 # TODO: Maybe add this somewhere else
            }

            return ret

        else:
            # TODO: Make this better
            i+=1

            if( i > GPS_MIN_TRIES):
                    time.sleep(1)
                    logger.debug("get_gps_data(): Try nr %s. Running session.next()..." % (i-GPS_MIN_TRIES) )


            if(i > GPS_MAX_TRIES):
                session.close()
                raise Exception("Unable to retrieve GPS data. Quitting loop.")
            

def get_msg_str(prv,nxt):
    msg = build_msg( 'SP', prv, nxt, 'speed', 2, 1 ) + \
        build_msg( 'HD', prv, nxt, 'heading' ) + \
        build_msg( 'HL', prv, nxt, 'heeling' )
    return msg

def build_msg(id,prv,nxt,attr,mult=1,dig=0):
    try:
        if (prv is None) or ( prv[attr] <> nxt[attr] ):
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
    except:
        return ''

def send_to_kindle(msg):

    if len(msg) == 0:
        return

    sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    sock.settimeout(SOCK_TIMEOUT)
    sock.connect((SOCK_HOST,SOCK_PORT))
    sock.settimeout(None)
    sock.send(msg)

def store_msg(con,id,data):
    cur = con.cursor()
    sql = "REPLACE INTO kindle(id,speed,heading,heeling) VALUES (?,?,?,?)"
    cur.execute(sql,(id,data['speed'],data['heading'],data['heeling']))
    con.commit()

if __name__ == '__main__': 
    main() 
