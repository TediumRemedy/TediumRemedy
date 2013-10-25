#!/usr/bin/python

import urllib
import urllib.request 

req = urllib.request.Request('http://front5.omegle.com/start?rcs=1&firstevents=1&spid=&randid=S47QZF2N&lang=en')
response = urllib.request.urlopen(req)
resultstr = response.read()
print(resultstr)


