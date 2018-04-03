#modified from https://realpython.com/face-detection-in-python-using-a-webcam/

import cv2
import sys
import time
import json
import numpy as np
import argparse

p = argparse.ArgumentParser(description='Process some integers.')
p.add_argument('--pathlen', type=int, action='store', default=50, help='length of the centerline trail (default: 50)')
args = p.parse_args()


cascPath = 'haarcascade_frontalface_default.xml'
faceCascade = cv2.CascadeClassifier(cascPath)

video_capture = cv2.VideoCapture(0)
frame_count = 0
facepath=[]
sampler = []

while True:
	# Capture frame-by-frame
	ret, frame = video_capture.read()
	frame_count+=1
	gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

	faces = faceCascade.detectMultiScale(
		gray,
		scaleFactor=1.1,
		minNeighbors=5,
		minSize=(30, 30),
		flags=cv2.CASCADE_SCALE_IMAGE
	)

	if len(faces):
	# Draw a rectangle around the faces
		for (x, y, w, h) in faces:
			cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
			while len(facepath) >= args.pathlen:
				facepath.pop(0)
			facepath.append([x+w/2,y+h/2])
			pts = np.array(facepath, np.int32)
			pts = pts.reshape((-1,1,2))
			cv2.polylines(frame,[pts],False,(0,255,0))
	elif len(facepath):
		facepath.pop(0)

	if frame_count%5 == 1:
		sampler.append([ [x,y,w,h] for (x, y, w, h) in faces])

	# Display the resulting frame
	cv2.imshow('Video', frame)

	if cv2.waitKey(1) & 0xFF == ord('q'):
		break

# When everything is done, release the capture
print 'frame count %d'%frame_count
print 'sampler: \n%s'%sampler
with open('../output/box_sample.txt','w') as f:
	#json.dump(sampler,f)
	f.write('%s\n'%str(sampler))
video_capture.release()
cv2.destroyAllWindows()
