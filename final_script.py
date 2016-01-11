#!/usr/bin/env python
# -*- coding: iso-8859-1 -*-
import string, bs4, re, os, sys, math, MySQLdb, types, urllib, urllib2, datetime, random, time, unicodedata, base64, socket
import xlrd
from urlparse import urljoin
from urllib2 import URLError


'''
ssh gsmp@54.88.34.236
pass: gamera@1234

http://54.88.34.236/
'''

hostname = '127.0.0.1' #running from 54.88.34.236
username = 'gamera'
password = 'gamera@1234'
dbname = 'gamera'

con = MySQLdb.connect(host=hostname, port=3306,
    user=username, passwd=password, db=dbname)
cursor = con.cursor()

'''
websites scraped:
http://www.g4tv.com/games/top-100-most-popular-games/
http://www.ea.com/games#20
http://en.wikipedia.org/wiki/List_of_best-selling_video_games

additional tags from:
http://www.y8.com/tags
http://www.miniclip.com/games/en/

additional tags taken from GameTags.xlsx provided by Bear
'''

def remove_tags(t, remv):
  for r in remv:
    for x in t:
      if r in x:
        t.remove(x)
  return t


def parseg4tv(txt):
  t = []
  soup = bs4.BeautifulSoup(txt)
  for script in soup.findAll('script'):
    script.extract()
  divs = soup.findAll('a')
  for div in divs:
    t.append(div.get('title'))
    t = filter(None, t)
  return t


#Validation needed for parseEA
#remove tags containing the following words:
remvEA = ['Help', 'Shop', 'Answer', 'Access', 'Blog', 'Browse', 'Upcoming', 'All ', 'Search', 'Current', 'Latest', 'PLay ']

def parseEA(txt):
  t=[]
  rels=[]
  soup = bs4.BeautifulSoup(txt)
  for script in soup.findAll('script'):
    script.extract()
  divs = soup.findAll('a')
  for div in divs:
    href = div.get('href')
    ans = div.get('title')
    if href == '#':
      if 'game' in ans.lower():
        continue
      if 'more' in ans.lower():
        continue
      rels.append(ans)
    if div.get('data-country-code'):
      continue
    else:
      if isinstance(ans, unicode):
        ans = unicodedata.normalize('NFKD', ans).encode('ascii','ignore')
      t.append(ans)

  t = filter(None, t)
  if len(rels) > 0:
    insertTagRels(rels) #populate Tag_Relationships

  #remove any tags containing r
  t = remove_tags(t, remvEA)

  return t


#Validation needed for parsewiki
#remove tags containing the following words:
remvW = ['Template', 'page', 'wikimedia', 'Wikipedia', 'Category', 'North America', 'Japan', 'Europe', 'Special', 'International', 'List of']

def parsewiki(txt):
  t=[]
  soup = bs4.BeautifulSoup(txt)
  for script in soup.findAll('script'):
    script.extract()
  divs = soup.findAll('a')
  for div in divs:
    ans = div.get('title')
    if isinstance(ans, unicode):
      ans = unicodedata.normalize('NFKD', ans).encode('ascii','ignore')
    t.append(ans)

  #GAME->PLATFORM MAPPING
  start = txt.find("Paid downloads</th>")
  end = txt.find("class=\"mw-headline\" id=\"Consoles\">Consoles")

  block_idx = txt.index('<tr>', start, end)
  end_block = txt.index('</tr>', block_idx+1, end)
  size = txt.count('<tr>', start, end)
  #print size: 42 values

  for i in range(1,size):
    all_names = re.findall(r'title=\"(.*?)\"', txt[block_idx:end_block])
    game = all_names[0]
    platforms = all_names[1:]
    for p in platforms:
      g2p = [game, p, 'Platform']
      platform_maps.append(g2p)

    block_idx = txt.index('<tr>', block_idx+1, end) #next block
    end_block = txt.index('</tr>', end_block+1, end)

  insertTagMaps(platform_maps)

  t = filter(None, t)

  #remove any tags containing r
  t = remove_tags(t, remvW)

  return t[1:1262] #change this number if remvW is changed


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


def moreTags(sheet):
  t=[]
  #print sheet.nrows
  for j in range(sheet.nrows):
    tag=sheet.cell_value(j,0)
    t.append(tag)

  return t


def insertTags(names): #insert into tags table
  global cursor
  for name in names:
    if '\\\'' not in name:
      if '\'' in name:
        name=name.replace('\'', '\\\'')

    if ',' in name:
      name = name.split(',')[0]
    #print 'testing ',name
    sql = "SELECT tag FROM Tags WHERE tag = \'" + name + "\'"
    cursor.execute(sql)
    result = cursor.fetchone()
    if result:
      #print 'exists ', name
      continue

    #print 'inserting ',name
    sql = "INSERT INTO Tags (tag) VALUES (\'" + name + "\');"
    #print sql
    try:
      cursor.execute(sql)
    except:
      print 'failed inserting ',name
      pass
  return


def insertTagRels(names): #insert into tag relationships table
  global cursor
  for name in names:
    #print 'testing ',name
    sql = "SELECT relationship FROM Tag_Relationships WHERE relationship = \'" + name + "\'"
    cursor.execute(sql)
    result = cursor.fetchone()
    if result:
      #print 'exists ', name
      continue

    #print 'inserting ',name
    sql = "INSERT INTO Tag_Relationships (relationship) VALUES (\'" + name + "\')"
    #print sql
    try:
      cursor.execute(sql)
    except:
      print 'failed inserting ',name
      pass
  return


#gets all tags currently in Tags table
def getTags():
  tags=[]
  presql = "SELECT COUNT(*) FROM Tags"
  cursor.execute(presql)
  cnt = cursor.fetchone()[0]

  sql = "SELECT tag FROM Tags"
  cursor.execute(sql)
  for i in range(0,cnt):
    result = cursor.fetchone()[0]
    tags.append(result)

  return tags


# tags that don't make sense
del_words = ['(video', 'edition', 'unknown', 'special', 'proving', 'challenge']

def populateTagsMap():
  #game -> game mapping..
  tags = getTags()
  moretags = []
  g2g = []
  for tag in tags:
    for word in tag.split(' '):
      word = word.replace('\'', '\\\'')

      rel = 'categories'
      if len(word) < 5: # threshold value based on size of tag set, can change
        continue
      if '\'' in word:
        continue
      if word.lower() in del_words:
        continue

      sql = "SELECT * FROM `Tags` WHERE tag LIKE \'%"+word+"%\'"
      cursor.execute(sql)
      result = cursor.fetchall()

      sql2 = "SELECT * FROM `twitter_tweets` WHERE tweet LIKE \'%"+word+"%\'"
      cursor.execute(sql2)
      result2 = cursor.fetchall()

      if len(result) > 1:
        #check frequency of occurrence of tag
        if len(result2) > 0: #threshold value, can change based on size of tweets dataset
          moretags.append(word)
          #print word
          for r in result:
            if '\'' in r[0]:
              continue
            maps = [word, r[0], rel]
            g2g.append(maps)
  
  #print g2g
  insertTagMaps(g2g)
  insertTags(moretags)



def insertTagMaps(names): #insert into tag maps table
  global cursor
  for name in names:
    #print 'testing ',name
    sql = "SELECT * FROM Tag_Map WHERE tag1 = \'"+name[0]+"\' AND tag2 = \'"+name[1]+"\' AND relationship = \'"+name[2]+"\'"
    cursor.execute(sql)
    result = cursor.fetchone()
    if result:
      #print 'exists ', name
      continue

    #print 'inserting ',name
    sql = "INSERT INTO Tag_Map (tag1, tag2, relationship) VALUES (\'"+name[0]+"\', \'"+name[1]+"\', \'"+name[2]+"\');"
    #print sql
    try:
      cursor.execute(sql)
    except:
      print 'failed inserting ',name
      pass
  return



def populateTwitterTags():
  final=[]
  tags = getTags()

  presql = "SELECT COUNT(*) FROM twitter_tweets"
  cursor.execute(presql)
  cnt = cursor.fetchone()[0]

  sql = "SELECT id, tweet FROM twitter_tweets"
  cursor.execute(sql)
  errors=0
  for i in range(0,cnt):
    result = cursor.fetchone() #(id,tweet)
    try:
      #if there is a hashtag in the tweet
      hvalues = re.findall(r'\#(.*?)\ ', result[1]) #check the result of re.findall (does some wierd ****)
      if hvalues == []:
        continue
      else:
        insertTags(hvalues) #put its value in the Tags table
        for hvalue in hvalues:
          val=(result[0], hvalue) #put (id, hvalue) in Tweet_Tags table
          final.append(val)

      for t in tags:
        if t in result[1]:
          val=(result[0], t)
          final.append(val)

    except:
      errors+=1
      continue

  print str(errors)+" skipped lists of tags"
  insertTweetTags(final)


def insertTweetTags(names): #insert into tweet tag table
  global cursor
  for name in names:
    #print 'testing ',name
    sql = "SELECT * FROM Tweet_Tags WHERE tweet_id = \'" + str(name[0]) + "\' AND tag = \'" + name[1] + "\'"
    cursor.execute(sql)
    result = cursor.fetchone()
    if result:
      #print 'exists ', name
      continue

    #print 'inserting ',name
    sql = "INSERT INTO Tweet_Tags (tweet_id, tag) VALUES (\'" + str(name[0]) + "\', \'" + name[1] + "\');"
    #print sql
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
    t=''
    platform_maps = []
    
    if (ext.lower() == '.xlsx'):
      wkbk = xlrd.open_workbook(f)
      sheet1 = wkbk.sheet_by_index(0)
      t=moreTags(sheet1)
      if len(t) > 0:
        insertTags(t)
    
    if (ext.lower() == '.html'):
      i=open(f, "r")
      l = i.readlines() 
      txt = ''.join(l)
      
      if (root == 'g4tv'):
        t=parseg4tv(txt)
      if (root == 'eagamesall'):
        t=parseEA(txt)
      if (root == 'eagames'):
        t=parseEA(txt) 
      if (root == 'wikipedia'):
        t=parsewiki(txt)
      if (root == 'Tags_2'):
        t=parseMiniclip(txt)  
      if (root == 'Tags_1'):
        t=parseY8(txt)
      
      if len(t) > 0:
        insertTags(t)
      i.close()


populateTagsMap()
populateTwitterTags()

con.close()

'''
Copyright (c) 2015 Aishwarya Afzulpurkar
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
'''
