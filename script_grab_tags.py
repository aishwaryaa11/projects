#!/usr/bin/env python
# -*- coding: iso-8859-1 -*-
import string, re, os, sys, math, MySQLdb, types, urllib, urllib2, datetime, random, time, unicodedata, base64, socket
from urlparse import urljoin
from urllib2 import URLError

'''

sources used:
http://www.y8.com/tags
http://www.miniclip.com/games/en/

'''

def grabPage(url):
  req = urllib2.Request(url)
  html=''
  try:
    res = urllib2.urlopen(req)
    info = res.info()
    html = res.read()
    ok=True
  except IOError, e:
    ok=False
    print 'IOError'
    return None
	
  return html

urls=['http://www.y8.com/tags','http://www.miniclip.com/games/en/']
cnt=1
for url in urls:
  req = urllib2.Request(url)
  html=''
  try:
    res = urllib2.urlopen(req)
    info = res.info()
    html = res.read()
    ok=True
  except IOError, e:
    ok=False
    print 'IOError'
  name="Tags_"+str(cnt)+'.html'	
  o=open(name, "w")  
  o.write(html)
  o.close()
  cnt+=1
  
'''
for f in os.listdir(os.curdir):
  if os.path.isfile(f):
    (root,ext)=os.path.splitext(f)
    if (ext.lower() == '.html'):
      i=open(f, "r")
      l = i.readlines()
      txt = ''.join(l)
      if (root.lower()[-2] == 'en'):
      	t=parseMiniclip(txt)
      if (root.lower()[-2] == 'gs'):
      	t=parseY8(txt)

      if len(t) > 0:
        insertTags(t)
      i.close()

'''
con.close()





