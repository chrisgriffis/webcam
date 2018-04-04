docker run -it --rm --device /dev/video0 -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=unix$DISPLAY --name=fd1 fdetect python video_face_detect.py
