xhost local:root
docker run -it --rm -p 5001:5000 --device /dev/video0 -v $(pwd)/src:/root/src -v $(pwd)/bin:/root/bin -v $(pwd)/output:/root/output -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=unix$DISPLAY --name=fd1 fdetect python server.py
#export COMPOSEDIR=$(pwd) && sudo -E docker-compose up