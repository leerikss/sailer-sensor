#!/bin/sh

FILENAME=SailerSensorApp
KEYSTORE=$HOME/Dev/Kindle/keystore/developer.keystore
YOUR_ALIAS=Test
JAR=$FILENAME.azw2
MANIFEST=$FILENAME.manifest
SSH_USER=root
SSH_IP=192.168.2.2

set -e
ant jar
cp $FILENAME.jar $JAR
jarsigner -keystore $KEYSTORE -storepass password $JAR dk$YOUR_ALIAS
jarsigner -keystore $KEYSTORE -storepass password $JAR di$YOUR_ALIAS
jarsigner -keystore $KEYSTORE -storepass password $JAR dn$YOUR_ALIAS

scp $JAR $SSH_USER@$SSH_IP:/mnt/us/documents
