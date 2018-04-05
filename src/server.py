import sys; sys.dont_write_bytecode = True; 
import os, logging, json

import flask
from flask import jsonify
from flask import request, redirect

from subprocess import call
from multiprocessing import Process #, Queue, Value
#from time import sleep
from os import path as pa
from os.path import join as pjoin, exists as pexists

app = flask.Flask(__name__)
#from flask_autodoc import Autodoc
#auto = Autodoc(app)
p = None


@app.route('/')
#@auto.doc(groups=['public','private'])
def index():
	'''returns main page '''
	app.logger.info('index')

	return jsonify(endpoint='index',markers='foo')

	'''
	query=dict()
	if pa.exists(progress_marker_dir+'busy'):
		with open(progress_marker_dir+'busy', 'r') as f:
		query=json.load(f)
			return jsonify(state='busy',markers=markers,query=query)
	else:
		return jsonify(state='ready',markers=markers,query=query)
	return flask.make_response(
		flask.render_template(
			'maindash.template.html',
		)
	)
	'''

@app.route('/start', methods=('GET',))
def start():
	''' more here \
	'''

	def launch():
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
	'''
	response = flask.make_response(
		flask.render_template(
			'process.template.html', 
		)
	)
	#response.headers['Cache-Control'] = 'no-cache'
	return response
	'''
@app.route('/kill', methods=('GET',))
def stop():
	''' more here \
	'''

	global p
	#orphans child processes... only used here for demonstration
	if p:
		p.terminate() #p.join would wait for video loop to die, freezing the webserver
		p=None
		return jsonify(endpoint='kill',markers='orphaned')
	else:
		return jsonify(endpoint='kill',markers='null_handle')
	'''
	response = flask.make_response(
		flask.render_template(
			'process.template.html', 
		)
	)
	#response.headers['Cache-Control'] = 'no-cache'
	return response
	'''

'''
@app.route('/apidoc')
@auto.doc(groups='all') #default, but being explicit here
def apidoc():
    return auto.html('public')
'''
	
if __name__ == '__main__':
	#now start up webserver
	app.logger.setLevel(logging.DEBUG)
	app.run(host='0.0.0.0', debug=True)
