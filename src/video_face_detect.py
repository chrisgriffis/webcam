#!/usr/bin/python

'''\
detect faces in a live video feed and do fun things with it.

modified from 
	https://realpython.com/face-detection-in-python-using-a-webcam/
'''

import cv2
import sys
import time
import json
import numpy as np
import argparse
import math

def closest(
	x, 
	candidates, 
	metric=lambda x,y:math.sqrt(math.fsum([(p[1]-p[0])**2 for p in zip(x,y)]))
):
	'''\
	decide the closest point in the candidate set to the reference point, \
	where closest is determined by the metric space norm. \
	default metric is the quadratic mean i.e. euclidean norm 
	'''
	dist_to_cand={}
	#in c++ i'd use a std::transform/std::accumulate or better yet
	#std::experimental::parallel::transform_reduce
	#http://en.cppreference.com/w/cpp/experimental/transform_reduce
	for c in candidates:
		dist_to_cand[metric(x,c)]=c
	return dist_to_cand[min(dist_to_cand.keys())]


p = argparse.ArgumentParser(description='Process some integers.')
p.add_argument('--pathlen', type=int, action='store', default=100, help='length of the centerline trail (default: 10)')
args = p.parse_args()


cascPath = 'haarcascade_frontalface_default.xml'
faceCascade = cv2.CascadeClassifier(cascPath)

video_capture = cv2.VideoCapture(0)
frame_count = 0
facepaths=[[]]
sampler = []
facecount=0

while (cv2.waitKey(1) & 0xFF != ord('q')):
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

	if not len(faces) or facecount!=len(faces):
		facecount=len(faces)
		facepaths=[[]]
		
	if len(faces) and not all(f for f in facepaths):
		facepaths=[[(x+w/2,y+h/2)] for (x, y, w, h) in faces]
	else:
		#first, prune stale points
		for f in facepaths:
			while len(f) >= args.pathlen:
				f.pop(0)
		#process each detected face
		for i,(x, y, w, h) in enumerate(faces):
			# Draw a rectangle around the faces
			#cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
			
			boxcenter=(x+w/2,y+h/2)
			canddict = {}

			#create a lookup that relates a tail point to the index of its path in the list of paths
			[canddict.update({fps[-1] : i}) for i,fps in enumerate(facepaths) if len(fps)]
			if len(canddict.keys()):
				#append the box center to path that has closest dangler
				#find the closest point, and lookup its index to get the path its on,
				#then use that index to append the point to the owning path
				facepaths[canddict[closest(boxcenter,canddict.keys())]].append(boxcenter)
	
	#draw the respective paths
	for f in facepaths:
		pts = np.array(f, np.int32)
		pts = pts.reshape((-1,1,2))
		cv2.polylines(frame,[pts],False,(0,255,0))

	#log some samples for some not yet impld statistical analysis
	if frame_count%5 == 1:
		sampler.append([ [x,y,w,h] for (x, y, w, h) in faces])

	# Display the resulting frame
	cv2.imshow('Video', frame)

# When everything is done, release the capture
video_capture.release()
cv2.destroyAllWindows()

#stash log data
print 'frame count %d'%frame_count
print 'sampler: \n%s'%sampler
with open('../output/box_sample.txt','w') as f:
	#json.dump(sampler,f)
	f.write('%s\n'%str(sampler))
