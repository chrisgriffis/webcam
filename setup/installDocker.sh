#!/bin/bash

apt-get -qq update && \
    apt-get -y install docker.io && \
    apt-get clean

#apt-get -y install python-pip python-dev build-essential docker.io && \
#apt-get -y install python && \
#pip install --upgrade pip
#pip install docker --user


