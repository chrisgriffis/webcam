import sys; sys.dont_write_bytecode = True; 
import os, logging, json

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

if __name__ == '__main__':
	#start up webserver
	app.logger.setLevel(logging.DEBUG)
	app.run(host='0.0.0.0', debug=True)
