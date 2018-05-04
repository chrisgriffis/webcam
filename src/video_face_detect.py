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


'''
	detects faces from a live video stream, draws a box around each face, and
	creates a trail from the center point of each face, effectively allowing you to 
	"draw" on the live feed by moving your face around. keeps separate trails
	for deparate faces which added a fun twist to an ostensibly straightforward idea.
'''


def closest(
	reference, 
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
	for cand in candidates:
		dist_to_cand[metric(reference,cand)]=cand
	return dist_to_cand[min(dist_to_cand.keys())]


#args, maytee.
p = argparse.ArgumentParser(description='provide optional overrides for parameters.')
p.add_argument('--pathlen', type=int, action='store', default=100, help='length of the centerline trail')
args = p.parse_args()

def do_gradient(img):
	gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
	gray_smooth = cv2.GaussianBlur(gray, (3,3),0)
	lapl = np.array([
		[0, 1, 0],
		[1,-4, 1],
		[0, 1, 0]])

	gaussian = (1/9)*np.array([
		[1, 1, 1],
		[1, 1, 1],
		[1, 1, 1]])

	sobel_x= np.array([
		[-1, 0, 1],
		[-2, 0, 2],
		[-1, 0, 1]])

	sobel_y= np.array([
		[-1,-2,-1],
		[0, 0, 0],
		[1, 2, 1]])
	gray_smooth_x = cv2.filter2D(gray_smooth,-1,sobel_x) 
	gray_smooth_y = cv2.filter2D(gray_smooth,-1,sobel_y) 
	gray_smooth_lapl = cv2.filter2D(gray_smooth,-1,lapl) 
	return


def do_canny(img):
	gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
	gray_canny = cv2.Canny(gray,50,100)
	#return cv2.threshold(gray_canny,2, 255, cv2.THRESH_BINARY_INV)[1]
	return gray_canny

def HoughOutline(img):
				
	rho = 0.5
	theta = np.pi/180
	threshold = 10
	min_line_length = 40
	max_line_gap = 2

	# Run Hough on the edge-detected image
	lines = cv2.HoughLinesP(do_canny(img), rho, theta, threshold, np.array([]),
	min_line_length, max_line_gap)
	
	for line in lines:
		for x1,y1,x2,y2 in line:
			cv2.line(img,(x1,y1),(x2,y2),(0,255,0),4)
	
	return img

def do_segment(img,k=2):
	criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 10, 0.2)
	retval, labels, centers = cv2.kmeans(np.float32(img.reshape((-1,3))), k, None, criteria, 10, cv2.KMEANS_RANDOM_CENTERS)
	centers = np.uint8(centers)
	segmented_data = centers[labels.flatten()]
	segmented_image = segmented_data.reshape((img.shape))
	#labels_reshape = labels.reshape(image.shape[0], image.shape[1])
	#print('labels_reshape\n',labels_reshape)
	return segmented_image

def run():

	cascPath = 'haarcascade_frontalface_default.xml'
	faceCascade = cv2.CascadeClassifier(cascPath)

	video_capture = cv2.VideoCapture(0)
	frame_count = 0
	facepaths=[[]]
	sampler = []
	facecount=0

	#unbuffered; immediate flush
	with open('/root/output/box_samples.txt','w', 0) as outf: 
		
		#loop forever until user presses 'q'
		while (cv2.waitKey(1) & 0xFF != ord('q')):
			
			# Capture frame-by-frame
			ret, frame = video_capture.read()
			frame_count+=1


			gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

			#detect faces in frame
			faces = faceCascade.detectMultiScale(
				gray,
				scaleFactor=1.1,
				minNeighbors=5,
				minSize=(30, 30),
				flags=cv2.CASCADE_SCALE_IMAGE
			)

			#reset the detection box center point trails if
			#the number of detected faces changes
			if not len(faces) or facecount!=len(faces):
				facecount=len(faces)
				facepaths=[[]]
			
			#reinitialize trails when needed
			#heuristic requires all trails to have at least one point in them
			if len(faces) and not all(f for f in facepaths):
				facepaths=[[(x+w/2,y+h/2)] for (x, y, w, h) in faces]
			else: #otherwise
				
				#first, prune stale points
				for f in facepaths:
					while len(f) >= args.pathlen:
						f.pop(0)
				
				#process each detected face
				for i,(x, y, w, h) in enumerate(faces):
					# Draw a rectangle around the faces
					cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
					
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
				cv2.polylines(frame,[pts],False,(128,0,127),3)

			#log some samples for some not yet impld statistical analysis
			if frame_count%5 == 1:
				sample = [[x,y,w,h] for (x, y, w, h) in faces]
				'''
				write to file in a format easy to read back in from c++
				e.g.
					2 264 125 139 139 97 112 141 141
					2 251 140 134 134 129 105 92 92
					3 81 146 134 134 241 154 143 143, 124 120 99 99
					2 249 168 144 144 157 192 52 52
					1 252 169 146 146
					2 249 179 150 150 110 266 65 65
				'''
				outf.write(\
					'%d %s\n'%\
						(	len(sample), 
							' '.join([' '.join(map(str,map(int,boxcoord))) for boxcoord in sample]) 
						)
				)

			# Display the resulting frame
			#cv2.imshow('Videox', gray_smooth_x)
			#cv2.imshow('Videox', gray_smooth_x)
			cv2.imshow('Videoc', do_canny(frame))
			#cv2.imshow('Video2', frame)
			#cv2.imshow('Videoc', do_segment(frame,3))
			'''
			if frame_count%2==0: 
				cv2.imshow('Video', gray_smooth_x) 
			else:
				cv2.imshow('Video', gray_smooth_y) 
			'''

	# When everything is done, release the capture
	video_capture.release()
	cv2.destroyAllWindows()

	#spit out some summary data
	print 'frame count %d'%frame_count
