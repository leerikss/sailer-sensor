#!/bin/sh

FILENAME=SailerSensorApp
KEYSTORE=sailersensor.keystore
YOUR_ALIAS=Test
PASSWORD=password
JAR=$FILENAME.azw2
MANIFEST=$FILENAME.manifest
SSH_USER=root
SSH_IP=kindle.cable
KUAL_DIR=../Kual/extensions/

# Generate app
set -e
ant jar 
cp $FILENAME.jar $JAR
jarsigner -keystore $KEYSTORE -storepass $PASSWORD $JAR dk$YOUR_ALIAS
jarsigner -keystore $KEYSTORE -storepass $PASSWORD $JAR di$YOUR_ALIAS
jarsigner -keystore $KEYSTORE -storepass $PASSWORD $JAR dn$YOUR_ALIAS

# Copy app
scp $JAR $SSH_USER@$SSH_IP:/mnt/us/documents

# Copy Kual extensions
scp -r $KUAL_DIR/* $SSH_USER@$SSH_IP:/mnt/us/extensions