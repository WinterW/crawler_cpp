#!/usr/bin/python
#coding:utf-8
# 下载器

import os
import sys
import urllib
import cStringIO
import pycurl
import gzip

# 网络超时(seconds)
CONNECTTIMEOUT = 60
TIMEOUT = 120
COOKIE_FILE = '/dev/null'
USER_AGENT = "Mozilla/5.0 (Windows NT 5.1; rv:6.0) Gecko/20100101 Firefox/6.0"
ACCEPT_GZIP = "gzip, deflate"

class Downloader:
  def __init__(self):
    self.c = pycurl.Curl()
    self.c.setopt(pycurl.CONNECTTIMEOUT, CONNECTTIMEOUT)
    self.c.setopt(pycurl.TIMEOUT, TIMEOUT)
    self.c.setopt(pycurl.COOKIEFILE, COOKIE_FILE)
    self.proxyip = ['192.168.22.200']  #代理服务器[IP:PORT]列表，随机使用列表中的IP
    #proxy_list = ['1110','1210','1310','1410','8899']
    self.proxyport = ['1110','1210','1310','1410','8899']
    self.connnecttimeout = 60 #获取联接超时(秒)
    self.timeout = 5 #读定超时(秒)
     

    # 是否为debug模式
    self.debug = False

  def SetDebug(self, debug=True):
    ''' 设置debug模式 '''
    self.debug = debug
    if self.debug == True:
      self.c.setopt(pycurl.VERBOSE, 1)

  def Post(self, req_url, referer, params_map):
    self.c.setopt(pycurl.POST, 1)
    params = urllib.urlencode(params_map)
    self.c.setopt(pycurl.POSTFIELDS, params)
    return self.Download(req_url, referer)

  def Get(self, req_url, referer):
    self.c.setopt(pycurl.POST, 0)
    return self.Download(req_url, referer)
    
  def Download(self, req_url, referer):
    '''
    download page
    '''
    self.c.setopt(pycurl.URL, req_url)
    http_header_list = ['User-Agent: %s' % (USER_AGENT)]
    http_header_list.append('Referer: %s' % referer)
    http_header_list.append('Accept-Encoding: %s' % ACCEPT_GZIP)
    self.c.setopt(pycurl.HTTPHEADER, http_header_list)

    body = cStringIO.StringIO()
    header = cStringIO.StringIO()
    html_body = None
    self.c.setopt(pycurl.WRITEFUNCTION, body.write)
    self.c.setopt(pycurl.HEADERFUNCTION, header.write)
    self.c.setopt(pycurl.CONNECTTIMEOUT, self.connnecttimeout)
    self.c.setopt(pycurl.TIMEOUT, self.timeout)
    #self.c.setopt(pycurl.PROXY,'192.168.22.200')
    #self.c.setopt(pycurl.PROXYPORT, 20002)

    is_ok = True
    try:
      self.c.perform()
      status = self.c.getinfo(pycurl.HTTP_CODE)

      if status >= 300:
        is_ok = False
        status = self.c.errstr()
      else:
        html_body = body.getvalue()
        try:
          ungziped_body = cStringIO.StringIO(html_body)
          gzipper = gzip.GzipFile(fileobj=ungziped_body)
          html_body = gzipper.read()
        except Exception as ex:
          pass
          #print 'ungzip failed:%s' % (ex)
    except:
      is_ok = False

    if is_ok == True:
      return html_body
    else:
      return None #str(status)

if __name__ == '__main__':
  downloader = Downloader()
  downloader.SetDebug(False)
  
  '''
  url = 'http://store.coo8.com/Web/Handler/CityGoodsStatus.ashx?goodsid=P161483&cityid=104102102'
  referer = 'http://www.coo8.com/product/150513.html'
  html_page = downloader.Download(url, referer)
  print html_page
  '''
  '''
  url = 'http://192.168.111.51:12837/get_pv'
  referer = url
  params_map = {'puid' : '193251897,194982324,195234610,193054791'}
  html_page = downloader.Post(url, referer, params_map)
  print html_page
  '''
  url = 'http://bj.58.com/zufang/10376308707843x.shtml'
  referer = url
  html_page = downloader.Get(url, referer)
  print html_page

