#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2013 Leif Eriksson
#
# Author: Leif Eriksson <leif@leif.fi>
# Mostly copied from: http://stackoverflow.com/questions/6146131/python-gps-module-reading-latest-gps-data

import threading
import time
from gps import *
 
class GpsPoller(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.session = gps(mode=WATCH_ENABLE)
        self.current_value = None
        self.running = True

    def get_current_value(self):
       return self.current_value
 
    def stop(self):
       self.running = False
 
    def run(self):
        while self.running:
            try:
                self.current_value = self.session.next()
            except:
                time.sleep(1)
