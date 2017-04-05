#!/usr/bin/python
#encoding: utf-8
# 监控号码图片相关服务

import os
import sys
import time 
import commands
import fnmatch
import smtp_client
import sms_client

PIDOF_PROG = ''

SVR_PATH = '/server/www/ganji/ganji_online/cra/phone_img_svr'
SCRIPT_PATH = SVR_PATH+'/script'
BIN_PATH = SVR_PATH+'/bin'
NGINX_SVR = 'nginx'
NGINX_COUNT = 16
IMG_URL_SVR = 'img_url_svr'
IMG_URL_COUNT = 32
PHONE_IMG_SVR = 'phone_img_svr'
PHONE_IMG_COUNT = 32
PORT = 20202
COUNT_LIMIT = 30
LOG_PATH = 'log'
EMAIL_INTERVAL = 180
SMS_INTERVAL = 600
SLEEP_INTERVAL = 6
CORE_FILE_LIMIT = 100

def GetPidofProg():
  cmd = 'which pidof'
  ret, pidof_prog = commands.getstatusoutput(cmd)
  if os.path.exists(pidof_prog):
    return pidof_prog
  else:
    return ''

def GetLocalIp():
  cmd = "/sbin/ifconfig|grep 'inet addr'|grep '192.168'|head -n 1|awk '{print $2}'|awk -F ':' '{ print $2 }'"
  ret, local_ip = commands.getstatusoutput(cmd)
  if ret != 0:
    return ''
  return local_ip

def IsProcessExist(process):
  pid = commands.getoutput('%s %s' % (PIDOF_PROG, process))
  if pid == "":
    return False
  else:
    return True

def IsProcessCountOK(process, count):
  pid = commands.getoutput('%s %s' % (PIDOF_PROG, process))
  if pid == "":
    return False
  else:
    item_list = pid.split(' ')
    if len(item_list) < count:
      return False
    else:
      return True

def RestartSvr(script_path, svr):
  cmd = '%s/%s.sh restart' % (script_path, svr)
  ret, status = commands.getstatusoutput(cmd)
  if ret != 0:
    WriteLog('restart %s failed:%s' % (svr, status))
    return False
  WriteLog('restart %s OK' % (svr))
  return True

def GetCoreFileCount(path):
  file_list = os.listdir(path)
  count = 0
  for file_name in file_list:
    if fnmatch.fnmatch(file_name, "core*"):
      count += 1
  return count

def GetConnCount(port):
  conn_count = commands.getoutput('netstat -na | grep :%s | grep ESTABLISHED -c' % port)
  return int(conn_count)

def WriteLog(line):
  tm = time.localtime(time.time())
  now = time.strftime("%Y%m%d%H%M%S", tm)
  today = time.strftime("%Y%m%d", tm)
  log_file = os.path.join(LOG_PATH, ('log.%s' % today))
  try:
    of = open(log_file, 'a+')
    of.write(now + "--" + line + "\n")
    of.close()
  except:
    print ("open %s failed" % log_file)

if __name__ == '__main__':
  if not os.path.exists(LOG_PATH):
    os.makedirs(LOG_PATH)

  PIDOF_PROG = GetPidofProg()
  if PIDOF_PROG == '':
    WriteLog('pidof prog invalid')
    sys.exit(1)
  else:
    WriteLog('pidof ok:%s' % PIDOF_PROG)

  local_ip = GetLocalIp()
  if local_ip == '':
    WriteLog('GetLocalIp() failed')
    sys.exit(1)

  WriteLog("Starting to monitor...")

  last_mail_time = 0
  last_sms_time = 0
  while True:
    nginx_ok = IsProcessCountOK(NGINX_SVR, NGINX_COUNT)
    img_url_ok = IsProcessCountOK(IMG_URL_SVR, IMG_URL_COUNT)
    phone_img_ok = IsProcessCountOK(PHONE_IMG_SVR, PHONE_IMG_COUNT)
    #conn_count = GetConnCount(PORT)
    wrong = False
    monitor_str = ""
    if not nginx_ok:
      wrong = True
      monitor_str += "nginx abnormal."
    if not img_url_ok:
      wrong = True
      if monitor_str != "":
        monitor_str += " "
      monitor_str += "img_url abnormal."
    if not phone_img_ok:
      wrong = True
      if monitor_str != "":
        monitor_str += " "
      monitor_str += "phone_img abnormal."

    #if conn_count < COUNT_LIMIT:
    #  wrong = True
    #  if monitor_str != "":
    #    monitor_str += " "
    #  monitor_str += ("conn:%d." % conn_count)

    # 未出错
    if not wrong:
      time.sleep(SLEEP_INTERVAL)
      continue

    # 报警
    WriteLog(monitor_str)
    now = time.time()
    tm = time.localtime(time.time())
    now_str = time.strftime("%Y%m%d%H%M%S", tm)
    # 发邮件
    if now-last_mail_time > EMAIL_INTERVAL:
      subject = '号码图片服务报警:%s' % local_ip
      body = '%s %s' % (now_str, monitor_str)
      if smtp_client.SendMail(subject, body):
        WriteLog("SendMail success")
        last_mail_time = now
      else:
        WriteLog("SendMail failed")
      
    # 发短信
    if now-last_sms_time > SMS_INTERVAL:
      content = '%s %s %s' % (now_str, local_ip, monitor_str)
      result = sms_client.SendSmss(content)
      if result:
        WriteLog("SendSms success")
        last_sms_time = now
      else:
        WriteLog("SendSms failed")

    core_count = GetCoreFileCount(BIN_PATH)
    if core_count >= CORE_FILE_LIMIT:
      WriteLog('#core:%d too much' % (core_count))
    elif img_url_ok == False:
      RestartSvr(SCRIPT_PATH, 'img_url_svr')
    elif phone_img_ok == False:
      RestartSvr(SCRIPT_PATH, 'phone_img_url')

    time.sleep(SLEEP_INTERVAL)
