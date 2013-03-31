#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# meerkat.probes.gps_info
#
# Copyright 2012 Konrad Markus
#
# Author: Konrad Markus <konker@gmail.com>
#

import sys
import gps
import json

NUM_PACKET_THRESHOLD = 20


def main():
    session = gps.gps(mode=gps.WATCH_ENABLE)

    n = 0
    while True:
        packet = session.next()
        if packet['class'] == 'TPV':
            session.close()
            ret = {
                "latitude":  packet.get('lat', '?'),
                "longitude": packet.get('lon', '?'),
                "utc_time":  packet.get('time', '?'),
                "altitude":  packet.get('alt', '?'),
                "epv":       packet.get('epv', '?'),
                "ept":       packet.get('ept', '?'),
                "epx":       packet.get('epx', '?'),
                "epy":       packet.get('epy', '?'),
                "speed":     packet.get('speed', '?'),
                "climb":     packet.get('climb', '?'),
                "heading":     packet.get('track', '?')
            }

            session.close()
            print json.dumps(ret)
            return
        #else:
        #    print "rejected:"
        #    print packet

        # Bail out after NUM_PACKET_THRESHOLD loops
        # although it will block anyway if there is no fix
        n = n + 1
        if n > NUM_PACKET_THRESHOLD:
            ret = {}
            session.close()
            print json.dumps(ret)
            return

if __name__ == '__main__':
    main()

