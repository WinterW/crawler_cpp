#!/usr/bin/python
#coding=utf-8


from static_client import *
import MySQLdb
import pymongo
import time
import re

WUBA_PHONE_PATTERN = re.compile('key=(?P<phone>[0-9\- ]{7,15})')


DB_HOST = '127.0.0.1'
DB_PORT = 3306
DB_USER = 'crawlerdb'
DB_PASSWD = 'crawlerdb123!'
DB_NAME = 'static_crawler'

LOG_PATH = 'banjia_log'

MONGO_HOST = '192.168.9.12'
MONGO_PORT = 37078
MONGO_USER = 'himalayas'
MONGO_PASSWORD = '$ganji:8848'
MONGO_DB = 'banjia_db'
MONGO_CLNT = 'banjia'

NOTICE = 'notice'                                                                                 
ERROR = 'error'
WARNING = 'warning'

def connDB():
  cursor = None
  try:
    conn=MySQLdb.connect(host=DB_HOST,port=DB_PORT,user=DB_USER,passwd=DB_PASSWD,db=DB_NAME,charset="utf8")
    cursor = conn.cursor()
  except Exception as ex:
    writeLog(ERROR,"connDB error: %s" % ex)
    return False
  return cursor

def connMongo():
  conn_db = None
  try:
    conn = pymongo.Connection(host=MONGO_HOST, port=MONGO_PORT)
    conn_db = conn[MONGO_DB]
    conn_db.authenticate(MONGO_USER, MONGO_PASSWORD)
  except:
    print 'conn Mongo error'
    return None

  return conn_db

def getTask(db_conn):
  sql = 'SELECT task_id,seed_url FROM seed_table WHERE is_valid = 100 LIMIT 1'
  try:
    db_conn.execute(sql)
    task_id_row = db_conn.fetchone()
  except Exception as ex:
    writeLog(ERROR,"getTask error: %s" % ex)
    return False,False
  
  if not task_id_row :
    return False,False
  task_id = task_id_row[0]
  seed_url = task_id_row[1]
  result = WUBA_PHONE_PATTERN.search(seed_url)
  phone = ''
  if result != None:
    phone = result.group('phone')

  sql = "UPDATE seed_table SET is_valid = 1 WHERE task_id = %d and is_valid = 100" % task_id
  try:
    res = db_conn.execute(sql)
  except Exception as ex: 
    writeLog(ERROR,"UpdateTask error: %s" % ex)
    return False,False
  if res != 1:
    return False,False
  
  return task_id,phone

def addTaskId(task_id,phone):
  conn_mongo = connMongo()
  if not conn_mongo:
    print 'conn Mongo error'
    return -1
  if task_id <=0:
    return -1

  mongo_clnt = conn_mongo[MONGO_CLNT]
  doc = mongo_clnt.find_one({'task_id':task_id})
  if doc != None:
    doc['status'] = 0
    doc['phone'] = phone
    doc['start_time'] = int(time.time())
    mongo_clnt.update({'task_id':task_id},doc)
  else:
    doc = {'task_id':task_id,'status':0,'start_time':int(time.time()),'phone':phone}
    mongo_clnt.save(doc)

  return 0

def start():
  db_conn = connDB()
  while 1 :
    if not db_conn:
      db_conn = connDB()
      continue

    task_id,phone = getTask(db_conn)
    if not task_id:
      time.sleep(10)
      continue
    
    is_add_task = True
    LoadDbTask(task_id, is_add_task)
    addTaskId(task_id,phone)

def writeLog(type , line):                                                                        
  print line                                                                                      
  tm = time.localtime(time.time())
  now = time.strftime("%Y%m%d%H%M%S", tm)                                                         
  today = time.strftime("%Y%m%d", tm)                                                             

  if type == NOTICE:                                                                              
    ext = today                                                                                   
  else: 
    ext = today +".wf"
  log_file = os.path.join(LOG_PATH, ('dealbanjia.%s' % ext))                                        
  try: 
    of = open(log_file, 'a+')
    of.write("%s -- %s\n" % (now, line))                                                          
    of.flush()                                                                                    
    of.close()                                                                                    
  except: 
    print ("open %s failed" % log_file)

if __name__ == '__main__':
  start()
