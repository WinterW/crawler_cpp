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
from conf_crawler import LinkBaseService
from conf_crawler import StaticLinkBaseService
from conf_crawler.ttypes import *

SVR_IP = '127.0.0.1'
SVR_PORT = 44001
TIME_OUT = 10000

def LoadSeedById(seed_id, is_add_link):
  try:
    transport = TSocket.TSocket(SVR_IP, SVR_PORT)
    transport.setTimeout(TIME_OUT)
    framed_transport = TFramedTransport(transport)
    framed_transport.open()
    protocol = TBinaryProtocol.TBinaryProtocol(framed_transport)

    service = LinkBaseService.Client(protocol)
    service.load_seed_by_id(seed_id, is_add_link)
    transport.close()

    return True

  except Exception as ex:
    print "Error:%s" % (ex)
    return True

def LoadSeedByUrl(seed_url, is_add_link):
  try:
    transport = TSocket.TSocket(SVR_IP, SVR_PORT)
    transport.setTimeout(TIME_OUT)
    framed_transport = TFramedTransport(transport)
    framed_transport.open()
    protocol = TBinaryProtocol.TBinaryProtocol(framed_transport)

    service = LinkBaseService.Client(protocol)
    service.load_seed_by_url(seed_url, is_add_link)
    transport.close()

    return True

  except Exception as ex:
    print "Error:%s" % (ex)
    return True


def LoadDbTask(task_id, is_add_task):
  try:
    transport = TSocket.TSocket(SVR_IP, SVR_PORT)
    transport.setTimeout(TIME_OUT)
    framed_transport = TFramedTransport(transport)
    framed_transport.open()
    protocol = TBinaryProtocol.TBinaryProtocol(framed_transport)

    service = StaticLinkBaseService.Client(protocol)
    service.load_db_task(task_id, is_add_task)
    transport.close()

    return True

  except Exception as ex:
    print "Error:%s" % (ex)
    return True

def LoadMongoDbTask(task_id, is_add_task):
  try:
    transport = TSocket.TSocket(SVR_IP, SVR_PORT)
    transport.setTimeout(TIME_OUT)
    framed_transport = TFramedTransport(transport)
    framed_transport.open()
    protocol = TBinaryProtocol.TBinaryProtocol(framed_transport)

    service = StaticLinkBaseService.Client(protocol)
    service.load_mongodb_task(task_id, is_add_task)
    transport.close()

    return True

  except Exception as ex:
    print "Error:%s" % (ex)
    return True


def UploadBody():
  try:
    transport = TSocket.TSocket(SVR_IP, SVR_PORT)
    transport.setTimeout(TIME_OUT)
    framed_transport = TFramedTransport(transport)
    framed_transport.open()
    protocol = TBinaryProtocol.TBinaryProtocol(framed_transport)

    service = StaticLinkBaseService.Client(protocol)
    req_item = DownloadReqItem(url = 'http://www.ganji.com')
    prop_item = DownloadPropItem(seed_url = 'http://so.iautos.cn/so.jsp?modeltype2=%D0%A1%D0%CD%B3%B5&pageindex=1', depth = 0)
    downloaded_body_item = DownloadedBodyItem(req_item, prop_item, is_ok = True, body = '123')
    service.upload_body(downloaded_body_item)
    transport.close()

    return True

  except Exception as ex:
    print "Error:%s" % (ex)
    return True


if __name__ == '__main__':
  seed_url = '%58.com%'
  is_add_link = False
  LoadSeedByUrl(seed_url, is_add_link)
  sys.exit(0)

  seed_id = 1
  LoadSeedById(seed_id)
  sys.exit(0)

  sys.exit(0)
  task_id = 1
  is_add_task = False
  #LoadMongoDbTask(task_id)
  LoadDbTask(task_id, is_add_task)

  #UploadBody()
