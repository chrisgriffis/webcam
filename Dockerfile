
FROM ubuntu
#MAINTAINER anon

RUN apt-get -qq update && \
    apt-get -y install python && \
    apt-get -y install python-pip python-dev build-essential && \
    apt-get clean
RUN pip install --upgrade pip
RUN pip install psutil flask requests grequests
RUN pip install flask-autodoc

ADD src /root/src
ADD setup /root/setup

RUN /root/setup/installOpenCV.sh

#set working directory in container
WORKDIR /root/src

#for compose file
#--device /dev/video0
