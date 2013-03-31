#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# meerkat.probes.kindle
#
# Copyright 2013 Leif Eriksson
#
# Author: Leif Eriksson <leif@leif.fi>

import sys
import sqlite3
import json
import socket

DDL = """CREATE TABLE IF NOT EXISTS kindle(
             speed      REAL,
             heading    REAL,
             heel       REAL);"""
DB = "../../../data/meerkat.db"
#SOCK_HOST = "192.168.2.2"
SOCK_HOST = "127.0.0.1"
SOCK_PORT = 8080

def main():
    con = None
    try:
        con = get_con()
        init_db(con)
        gps = get_probe_data(con,'meerkat.probe.gps_info')

        speed = gps[0]['speed']
        head = gps[0]['heading']
        heel = 0 # TODO: Retrieve

        # TODO: Add logic when to save to kindle
        send_to_kindle(speed,head,heel)
        store_kindle_msg(con,speed,head,0)


#    except:
#        print "Error %s:" % sys.exc_info()[0]
#        sys.exit(1)
    
    finally:
        if con:
            con.close()        

def get_con():
    con = sqlite3.connect(DB)
    return con

def init_db(con):
    cur = con.cursor()
    cur.execute(DDL)

def get_probe_data(con,probe_id):
    sql = "select data from probe_data where probe_id=? order by timestamp desc limit 1";
    cur = con.cursor()
    for row in cur.execute(sql,[probe_id]):
        return json.loads( str( row[0] ) )

def send_to_kindle(speed,head,heel):
    msg = ("SP%f HD%f HL%f" % (speed,head,heel))
#    print msg
    sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    sock.connect((SOCK_HOST,SOCK_PORT))
    sock.send(msg)

def store_kindle_msg(con,speed,head,heel):
    cur = con.cursor()
    cur.execute("DELETE FROM kindle")
    sql = "INSERT INTO kindle(speed,heading,heel) VALUES (?,?,?)"
    cur.execute(sql,(speed,head,heel))
    con.commit()

if __name__ == '__main__': 
    main() 
