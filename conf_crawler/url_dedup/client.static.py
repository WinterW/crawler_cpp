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
from conf_crawler import DedupService
from conf_crawler.ttypes import *

SVR_IP = '127.0.0.1'
SVR_PORT = 44005
TIME_OUT = 100000

class Tester:
  def Init(self):
    try:
      transport = TSocket.TSocket(SVR_IP, SVR_PORT)
      transport.setTimeout(TIME_OUT)
      framed_transport = TFramedTransport(transport)
      framed_transport.open()
      protocol = TBinaryProtocol.TBinaryProtocol(framed_transport)

      self.service = DedupService.Client(protocol)

      return True
    except Exception as ex:
      print 'Exception:%s' % (ex)
      return False

  def IsExists(self, url):
    try:
      ret = self.service.is_exists(url)
      return ret
    except Exception as ex:
      print 'Exception:%s' % (ex)
      return False

  def Insert(self, url):
    try:
      self.service.insert(url)
    except Exception as ex:
      print 'Exception:%s' % (ex)

  def Remove(self, url):
    try:
      self.service.remove(url)
    except Exception as ex:
      print 'Exception:%s' % (ex)

  def BatchRemove(self, url_patterh):
    ret = None
    try:
      ret = self.service.batch_remove(url_pattern)
    except Exception as ex:
      print 'Exception:%s' % (ex)
    finally:
      return ret

  def Resize(self, bucket_count):
    try:
      self.service.resize(bucket_count)
    except Exception as ex:
      print 'Exception:%s' % (ex)

if __name__ == '__main__':
  tester = Tester()
  ret = tester.Init()
  if ret == False:
    sys.exit(1)

  url_pattern = '.*coo8.*'
  ret = tester.BatchRemove(url_pattern)

  sys.exit(0)

  bucket_count = 10000000
  tester.Resize(bucket_count)
  url1 = 'http://bj.ganji.com'
  tester.Insert(url1)
  url2 = 'http://sh.ganji.com'
  tester.Insert(url2)
  url3 = 'http://sh.ganji.cn'
  tester.Insert(url3)
  ret = tester.IsExists(url1)
  ret = tester.IsExists(url1)
  print ret
  #url_pattern = '.*g{anji.com'
  url_pattern = '.*ganji.com'
  ret = tester.BatchRemove(url_pattern)
  print ret
  ret = tester.IsExists(url1)
  print ret
  ret = tester.IsExists(url2)
  print ret
  ret = tester.IsExists(url3)
  print ret
