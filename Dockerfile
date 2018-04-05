
FROM ubuntu
MAINTAINER chrisgriffis on github

#changes to files in this folder will invalidate next layer 
#and cause a big delay (due to install time) when building a new image
#think twice when making changes!
ADD setup /root/setup

#this takes a VERY long time to run
RUN /root/setup/installOpenCV.sh

RUN apt-get -qq update && \
    apt-get -y install python && \
    apt-get -y install python-pip python-dev build-essential && \
    apt-get clean
RUN pip install --upgrade pip
RUN pip install psutil flask requests 
RUN pip install flask-autodoc

#dont need grequests... yet
#RUN pip install grequests

#this changes frequently, and changes invalidate subsequent image layers
#so dont put anything too slow to build after this
ADD src /root/src
ADD bin /root/bin

#set working directory in container
WORKDIR /root/src

#example way to start container binding camera and video display
#xhost local:root
#docker run -it --rm --device /dev/video0 -v $(pwd)/output:/root/output -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=unix$DISPLAY --name=fd1 fdetect python video_face_detect.py
