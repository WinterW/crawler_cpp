#!/usr/bin/python
#coding=utf-8
# 测试接口

import os
import sys

sys.path.append('/usr/lib/python2.6/site-packages/')
from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.transport.TTransport import TFramedTransport
from thrift.protocol import TBinaryProtocol
from thrift.server import TServer
from thrift.server import TNonblockingServer

sys.path.append('../gen-py')
from conf_crawler import ExtractorService
from conf_crawler import StaticLinkBaseService
from conf_crawler.ttypes import *

SVR_IP = '127.0.0.1'
SVR_PORT = 44303
TIME_OUT = 100000

def LoadTemplate(url_template, template_type):
  try:
    global SVR_PORT
    transport = TSocket.TSocket(SVR_IP, SVR_PORT)
    transport.setTimeout(TIME_OUT)
    framed_transport = TFramedTransport(transport)
    framed_transport.open()
    protocol = TBinaryProtocol.TBinaryProtocol(framed_transport)

    service = ExtractorService.Client(protocol)
    ret = service.load_template(url_template, template_type)
    transport.close()

    print ret
    return True

  except Exception as ex:
    print "Error:%s" % (ex)
    return True

if __name__ == '__main__':
  if len(sys.argv) < 3:
    print 'Usage:%s template template_type'
    sys.exit(1)

  url_template = sys.argv[1]
  template_type = int(sys.argv[2])

  '''
  if template_type == 0:
    template_type = TemplateType.CSS_SELECTOR_TYPE
  elif template_type == 1:
    template_type = TemplateType.PLAIN_HTML_TYPE
  else:
    print 'invalid template type:%d' % (template_type)
    sys.exit(1)
  '''
  
  LoadTemplate(url_template, template_type)
