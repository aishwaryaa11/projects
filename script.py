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


'''
for each catname:
{
go into:
"www.miniclip.com"+url[9:]
= "www.miniclip.com/games/genre-"+n+"/"+catname+"/en/#t-m-c"

search for 'games-list inner' (it will show up in 3 places)
each time it shows up:
search for " href="/games/"+gamename+"/en/#t-w-c" "
example:
href="/games/forest-temple/en/#t-w-c"

store gamename in games table
store gamename and catname in game-tag table
}

'''
def extractGame(catname):
  lala



'''
ssh gsmp@54.88.34.236
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


def parseMiniclip(txt): #txt holds a list containing values (strings) of each line
  flag = 0
  cat_tags = []
  for l in txt:
  	if "category-list" in l:
  		flag = 1
  		continue
  	if flag == 1:
  		if "<nav class=" in l:
  			return cat_tags
  		else:
  			catname = l.split('>')[1].split('<')[0]
  			cat_tags.append(catname)
  			n = l.split('cat-')[1].split('"')[0]
  			url = 'http://www.miniclip.com/games/genre-'+n+'/'+catname+'/en/#t-m-c'
  			ok=False
  			grabPage(url,catname)
  			#instead do extract_game?
  return 



def parseY8(txt):
  flag = 0
  for l in txt:
  	if '<ul class="tag">' in l:
  		flag =1
  		continue
  	if flag == 1:
  		if '</ul>' in l:
  			return cat_tags
  		if 'href' not in l:
  			continue
  		else:
  			catname = l.split('title="')[1].split('"')[0]
  			cat_tags.append(catname)

  return



'''
for each tagname,
if next tagname has same first four letters as previous, 
then store each tagname in tag-tagsy table
also,
if next tagname . length < previous tagname . length,
then check if next is subset of previous
if yes, insert next and previous in tag-tagse table
else
check if previous is subset of next
if yes, insert next and previous in tag-tagse table
'''
def insertTags(names): #insert into tags table
  global cursor
  for name in names:
    print 'testing ',name
    sql = "SELECT tagname FROM tags WHERE tagname = \'" + name + "\'"
    cursor.execute(sql)
    result = cursor.fetchone()
    if result:
      print 'exists ', name
      continue

    print 'inserting ',name
    sql = "INSERT INTO tags (tagname) VALUES (\'" + name + "\')"
    print sql
    try:
      cursor.execute(sql)
    except:
      print 'failed inserting ',name
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
      if (root.lower()[-2] == 'en'):
      	t=parseMiniclip(txt)
      if (root.lower()[-2] == 'gs'):
      	t=parseY8(txt)

      if len(t) > 0:
        insertTags(t)
      i.close()


con.close()




def populateTagsMap():
  #game -> game mapping..
  tags = getTags()
  moretags = []
  g2g = []
  for tag in tags:
    words = tag.split(' ')
    for a in range(0,len(words)-1):
      word = words[a]
      word = word.replace('\'', '\\\'')
      wordnxt = words[a+1]
      wordnxt = wordnxt.replace('\'', '\\\'')

      rel = 'categories'
      if len(word) < 5: # threshold value based on size of tag set, can change
        continue
      if len(wordnxt) < 4: # threshold value based on size of tag set, can change
        continue
      if ('\'' in word) or ('\'' in wordnxt):
        continue
      if word.lower() in del_words:
        continue

      sql = "SELECT * FROM `Tags` WHERE tag LIKE \'%"+word+"%\'"
      cursor.execute(sql)
      result = cursor.fetchall()

      sql2 = "SELECT * FROM `twitter_tweets` WHERE tweet LIKE \'%"+word+"%\'"
      cursor.execute(sql2)
      result2 = cursor.fetchall()

      sql3 = "SELECT * FROM `Tags` WHERE tag LIKE \'%"+word+" "+wordnxt+"%\'"
      cursor.execute(sql3)
      result3 = cursor.fetchall()

      sql4 = "SELECT * FROM `twitter_tweets` WHERE tweet LIKE \'%"+word+" "+wordnxt+"%\'"
      cursor.execute(sql4)
      result4 = cursor.fetchall()

      if len(result) > 1:
        #check frequency of occurrence of tag
        if len(result2) > 0: #threshold value, can change based on size of tweets dataset
          moretags.append(word)
          #print word
          for r in result:
            maps = [word, r[0], rel]
            g2g.append(maps)

      if len(result3) > 1:
        if len(result4) > 0: #threshold value, can change based on size of tweets dataset
          moretags.append(word+" "+wordnxt)
          #print word
          for r in result3:
            maps = [word+" "+wordnxt, r[0], rel]
            g2g.append(maps)
  
  print g2g
  #insertTagMaps(g2g)
  insertTags(moretags)
