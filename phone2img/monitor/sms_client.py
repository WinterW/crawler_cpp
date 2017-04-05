#coding:utf-8
# 发送短信

import urllib
import urllib2

PHONE_LIST = ['18600015328','15010313658']
SMS_SERVER = 'as.dns.ganji.com:20000'

httpHandler = urllib2.HTTPHandler(debuglevel=0)
httpsHandler = urllib2.HTTPSHandler(debuglevel=0)
opener = urllib2.build_opener(httpHandler, httpsHandler)
urllib2.install_opener(opener)

def SendSms(phone, content):
  url = "http://%s/WebGate/ShortMsg.aspx?opt=send&uniqueId=&serviceId=MainSite-PPC&phones=%s&content=%s" % (SMS_SERVER, phone, content)

  try:
    request = urllib2.Request(url)
    response = urllib2.urlopen(request)

    return True
  except:
    return False

def SendSmss(content):
  content = urllib.quote(content)
  for phone in PHONE_LIST:
    result = SendSms(phone, content)
    if result == False:
      return False
  return True

if __name__ == '__main__':
  content = '测试 test'
  result = SendSmss(content)
  if result:
    print "OK"
  else:
    print "failed"
