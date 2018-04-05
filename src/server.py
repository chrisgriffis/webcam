import sys; sys.dont_write_bytecode = True; 
import os, logging, json
from os.path import abspath, basename, join as pjoin, exists as pexists
from os import path as pa

import flask
from flask import jsonify

from subprocess import call
from multiprocessing import Process #, Queue, Value

app = flask.Flask(__name__)
from flask_autodoc import Autodoc
auto = Autodoc(app)

#daemon process global handle
p = None


@app.route('/')
@auto.doc(groups=['public','private'])
def index():
	''' returns api doc page '''
	return auto.html('public')

@app.route('/start', methods=('GET',))
@auto.doc(groups=['public','private'])
def start():
	''' \
	starts the webcam face detect app. \
	press q with the running app in the foreground to stop. \
	go to /kill endpoint to clean up handle \
	'''

	def launch():
		''' callback for starting up the face detection '''
		#could import module and call directly, 
		#but will launch as a subprocess so its on its own thread
		call(['/root/src/video_face_detect.py'])
	
	global p
	if not p:
		p = Process(target=launch)
		p.daemon = True
		p.start()
		return jsonify(endpoint='start',markers='started')
	else:
		return jsonify(endpoint='start',markers='already_running')
	
@app.route('/kill', methods=('GET',))
@auto.doc(groups=['public','private'])
def stop():
	''' orphans the daemon process that is hosting the facedetect tool '''

	global p
	#orphans child processes... only used here for demonstration
	if p:
		p.terminate() #p.join would wait for video loop to die, freezing the webserver
		p=None
		return jsonify(endpoint='kill',markers='orphaned')
	else:
		return jsonify(endpoint='kill',markers='null_handle')

@app.route('/data', methods=('GET',))
@auto.doc(groups=['public','private'])
def data():
	''' look at the text file made during a framegrab run. refresh browser to get updated view; \
	    during a detection run the file is being modified in realtime. \
	'''
	if pexists(pjoin('/root/output', 'box_samples.txt')):
		return flask.send_from_directory('/root/output', 'box_samples.txt')
	else:
		return flask.Response(404,'file not found or path doesn\'t exist.')

@app.route('/analyze', methods=('GET',))
@auto.doc(groups=['public','private'])
def analyze():
	''' analyze the data in the data file. '''
	if pexists(pjoin('/root/output', 'box_samples.txt')):
		if pexists(pjoin('/root/output', 'analysis.txt')):
			os.remove(pjoin('/root/output', 'analysis.txt'))
		call(['/root/bin/analyzer.out','/root/output/box_samples.txt','/root/output/analysis.txt'])
		return flask.send_from_directory('/root/output', 'analysis.txt')
	else:
		return flask.Response(404,'box_samples.txt not found or path doesn\'t exist.')

if __name__ == '__main__':
	#start up webserver
	app.logger.setLevel(logging.DEBUG)
	app.run(host='0.0.0.0', debug=True)
