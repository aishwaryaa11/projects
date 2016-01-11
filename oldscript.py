#!/usr/bin/env python
# -*- coding: iso-8859-1 -*-
import string, bs4, re, os, sys, math, MySQLdb, types, urllib, urllib2, datetime, random, time, unicodedata, base64, socket
from urlparse import urljoin
from urllib2 import URLError

'''

sources used:
http://www.y8.com/tags
http://www.miniclip.com/games/en/



def grabPage(url,catname):
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
  o=open(catname, "w")
  o.write(html)
  o.close()
  return




ssh gamera@54.88.34.236
pass: gamera@1234

http://54.88.34.236/

'''

hostname = '127.0.0.1'
#if running from 54.88.34.236
#hostname = '127.0.0.1'
username = 'gamera'
password = 'gamera@1234'
dbname = 'gamera'

con = MySQLdb.connect(host=hostname, port=3306,
    user=username, passwd=password, db=dbname)
cursor = con.cursor()


def parseMiniclip(txt):
  soup = bs4.BeautifulSoup(txt)
  for script in soup.findAll('script'):
    script.extract()
  divs = soup.findAll('div', {'class': 'category-list'})
  for div in divs:
    txt=re.compile(r'<[^>]+>').sub(',',div.__str__())
    txt=re.compile(r'[\n]+').sub('',txt)
    txt=re.sub(r'[\']+', '\\\'', txt)
    txt=txt.split(',')
    while '' in txt:
      txt.remove('')

  return txt


def getGameTags(txt):
  for tag in txt:
    uls = soup.findAll('ul', {'class': 'games-list inner'})
    for ul in uls:
      print ul
      '''
      TODO:
      search for " href="/games/"+gamename+"/en/#t-w-c" "
      example:
      href="/games/forest-temple/en/#t-w-c"

      store gamename in games table
      store gamename and catname in game-tag table
      '''


def parseY8(txt):
  catnames = []
  soup = bs4.BeautifulSoup(txt)
  for script in soup.findAll('script'):
    script.extract()
  uls = soup.findAll('ul', {'class': 'tag'})
  for ul in uls:
    vals = list(ul.findAll('a'))
    for val in vals:
      txt=re.compile(r'<[^>]+>').sub(',',val.__str__())
      txt=re.compile(r'[\n]+').sub('',txt)
      txt=re.sub(r'[\']+', '\\\'', txt)
      txt=re.compile(r'[  ]+').sub('',txt)
      txt=txt.split(',')
      while '' in txt:
        txt.remove('')
      catnames.append(txt[0])

  return catnames


'''
if two tags, one right after the other, are similar in terms of:
-syntax: first four or last five words matching
-semantics: first tag is in second tag (or the other way)

then add the two tags to tag_tagsy or tag_tagse accrodingly...
'''

def insertTags(names): #insert into tags table
  global cursor
  prev_name = None
  for name in names:
    #print 'testing ',name
    sql = "SELECT tagname FROM tags WHERE tagname = \'"+name+"\'"
    sql2 = "SELECT * FROM tag_tagsy WHERE tag1name = \'"+name+"\' OR tag2name = \'"+name+"\'"
    sql3 = "SELECT * FROM tag_tagse WHERE tag1name = \'"+name+"\' OR tag2name = \'"+name+"\'"
    cursor.execute(sql)
    result = cursor.fetchone()
    if result:
      #print 'exists in tags', name
      continue
    cursor.execute(sql2)
    result = cursor.fetchone()
    if result:
      #print 'exists in tag_tagsy', name
      continue
    cursor.execute(sql3)
    result = cursor.fetchone()
    if result:
      #print 'exists in tag_tagse', name
      continue

    if prev_name != None:
      if (name[:4] == prev_name[:4]) or (name[-5:] == prev_name[-4:]):
        sql2 = "INSERT INTO tag_tagsy (tag1name, tag2name) VALUES (\'"+name+"\', \'"+prev_name+"\');"
      if (name in prev_name) or (prev_name in name):
        print prev_name, name
        sql3 = "INSERT INTO tag_tagse (tag1name, tag2name) VALUES (\'"+name+"\', \'"+prev_name+"\');"

    #print 'inserting ',name
    sql = "INSERT INTO tags (tagname) VALUES (\'" + name + "\');"
    #print sql
    prev_name = name
    for this in [sql, sql2, sql3]:
      try:
        cursor.execute(this)
      except:
        print 'failed inserting ',name,this
        pass
  return


#MAIN
for f in os.listdir(os.curdir):
  if os.path.isfile(f):
    (root,ext)=os.path.splitext(f)
    if (ext.lower() == '.html'):
      i=open(f, "r")
      l = i.readlines() 
      txt = ''.join(l)
      t=''	  
      if (root == 'Tags_2'):
      	t=parseMiniclip(txt)	
      if (root == 'Tags_1'):
      	t=parseY8(txt)
      if len(t) > 0:
        insertTags(t)
      i.close()


con.close()





