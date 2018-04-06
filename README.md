# webcam - objective
I was given a challenge to implement face detection on desktop computer using a webcam and "do something fun" with the output.
this is a toy-grade effort to meet the challenge and showcase polyglottal full stack development:
- modern cpp,stl,tmp
- linux and bash
- docker
- python
- webservers
- algorithms data structures 
- OO "Gang of Four" design patterns (not yet impl'd)
- code that's clean&clever but still eminently readable with self-evident reasoning

# description
(Currently tested in an Ubuntu 16.04 LTS image running in VirtualBox)

A deploy script that launches a docker container that binds to the camera and video display of the host. The container is also running a web server that's reachable from 0.0.0.0:5001.

The index endpoint '/' brings up a page that is the auto generated api doc for the web service. The doc informs you that the /start endpoint (eg 0.0.0.0:5001/start) turns on the camera and begins tracking. While tracking, upon every 5th frame, a sample of the face detect box(es) are written to disk in a data-out file. The /data endpoint shows you what's in the data-out file. The /analyze endpoint calls the analyzer cpp executable, whose raison d'être is to showcase really sophisticated cplusplus. This executable reads in the data from /data and computes the cumulative center of gravity of the boxes sampled. It then spits it out to a file whose text contents is rendered in the browser as a response. The cpp uses lots of advanced techniques that are bit overkill but demo things like SFINAE, TMP, type inference, compile time checks, etc.

the face detection aspect was grabbed from an online tutorial. I've taken that as a starting point and added the ability to create trails for each face that persist like mouse pointer trails. to make trails i accumulated points across frames and drew an unclosed polygon on the current frame sent out to display. the fun part was dealing with multiple faces; a given frame may have multiple box definitions for multiply detected faces, but it doesnt know which face it's boxing. I devised a neat little heuristic that takes each face detect box center and find the closest tail point to append itself to.  

# installation
TBD

# Usage
TBD

# example data output

# example analysis output

# screenshots