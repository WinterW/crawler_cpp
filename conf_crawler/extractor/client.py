#!/usr/bin/python
#coding=utf-8
# 测试接口

import os
import sys

sys.path.append('/usr/lib/python2.7/site-packages/')
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
SVR_PORT = 44003
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

    return True

  except Exception as ex:
    print "Error:%s" % (ex)
    return True

def Extract(url_template, template_type, depth, body_file):
  try:
    global SVR_PORT
    transport = TSocket.TSocket(SVR_IP, SVR_PORT)
    transport.setTimeout(TIME_OUT)
    framed_transport = TFramedTransport(transport)
    framed_transport.open()
    protocol = TBinaryProtocol.TBinaryProtocol(framed_transport)

    service = ExtractorService.Client(protocol)
    extract_item = ExtractItem()
    extract_item.url = 'http://api.wap.58.com/api/info/infolist/bj/zufang/1/25/?pic=1'
    extract_item.url_template = url_template
    extract_item.depth = depth
    extract_item.template_type = template_type
    file_in = body_file
    f = open(file_in, 'r')
    extract_item.body = f.read()
    f.close()
    matched_result_item = service.extract_sync(extract_item)
    print 'len(sub_result_list):%d' % len(matched_result_item.sub_result_list)
    transport.close()
    if matched_result_item.is_ok == False:
      print 'Err:%s' % (matched_result_item.err_info)
    for key, value in matched_result_item.self_result.iteritems():
      for v in value:
        print '[%s]\t%s' % (key, v)
    for item in matched_result_item.sub_result_list:
      print '-------------------'
      for key, value in item.iteritems():
        print '[%s]\t%s' % (key, value[0])

    '''
    transport = TSocket.TSocket(SVR_IP, SVR_PORT)
    transport.setTimeout(TIME_OUT)
    framed_transport = TFramedTransport(transport)
    framed_transport.open()
    protocol = TBinaryProtocol.TBinaryProtocol(framed_transport)

    service = StaticLinkBaseService.Client(protocol)
    service.upload_extract(extract_item, matched_result_item)
    transport.close()
    '''

    return True

  except Exception as ex:
    print "Error:%s" % (ex)
    return False

if __name__ == '__main__':
  if len(sys.argv) < 5:
    print 'Usage:%s template template_type depth body_file'
    sys.exit(1)

  url_template = sys.argv[1]
  template_type = int(sys.argv[2])
  depth = int(sys.argv[3])
  body_file = sys.argv[4]

  if template_type >= TemplateType.TEMPLATE_TYPE_MAX:
    print 'invalid template type:%d' % (template_type)
    sys.exit(1)
  
  LoadTemplate(url_template, template_type)
  for i in range(0, 1):
  #while True:
    if not Extract(url_template, template_type, depth, body_file):
      print 'ERROR'
      sys.exit(1)
