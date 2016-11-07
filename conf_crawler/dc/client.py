#!/usr/bin/env python
#encoding:utf-8
 
import sys
sys.path.append('../gen-py')
sys.path.append('/usr/lib/python2.6/site-packages/')
 
from conf_crawler import DownloaderService
from conf_crawler import DCService
from conf_crawler.ttypes import *

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol
from thrift.transport.TTransport import TFramedTransport

transport = TSocket.TSocket('localhost', 44002)
framed_transport = TFramedTransport(transport)
framed_transport.open()
protocol = TBinaryProtocol.TBinaryProtocol(framed_transport)

client = DCService.Client(protocol)

 
def PushDownloadTask():
  try:
    url = 'http://www.coo8.com/interfaces/showReviewsByGoodsId.action<%param%>flag=all&goodsId=P145484&pageIndex=1'
    referer = "http://www.coo8.com/product/145484.html"
    download_task = DownloadTask()
    download_task.req_item = DownloadReqItem()
    download_task.req_item.url = url
    download_task.req_item.referer = referer
    download_task.req_item.time_out = 1000 
    download_task.prop_item = DownloadPropItem()
    download_task.prop_item.is_friendly = True 
    download_task.prop_item.interval = 60 
    ret = client.push_download_task(download_task)
    print ret
   
  except Thrift.TException, tx:
    print "%s" % (tx.message)

def GetDownloadTask():
  try:
    download_task = client.get_download_task()
    print download_task
   
  except Thrift.TException, tx:
    print "%s" % (tx.message)

def Upload():
  url = "http://www.baidu.com"
  downloaded_body_item = DownloadedBodyItem()
  downloaded_body_item.req_item = DownloadReqItem()
  downloaded_body_item.req_item.url = url
  downloaded_body_item.is_ok = False
  downloaded_body_item.body = 'haha'
  try:
    download_task = client.upload_download_task(downloaded_body_item)
    print download_task
   
  except Thrift.TException, tx:
    print "%s" % (tx.message)




if __name__ == '__main__':
  PushDownloadTask()
  #GetDownloadTask()
  #Upload()
  transport.close()
   
   
