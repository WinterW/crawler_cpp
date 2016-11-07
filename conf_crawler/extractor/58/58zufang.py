#!/usr/bin/python
#coding=utf-8
# 测试接口

import os
import sys
import string 
import urllib
import fnmatch
import time
import re
import json
from downloader import *

reload(sys)
sys.setdefaultencoding('utf-8')
sys.path.append('/usr/lib/python2.7/site-packages/')
sys.path.append('.')
DATEPATTERN = re.compile(r'''\d+''')
DISTRICT_URL_PATTERN1 = re.compile(r'''http://\w+\.58\.com/xiaoqu/\w+/''')
CITY_URL_PATTERN1 = re.compile(r'''http://\w+\.58\.com/''')
CITY_NAME_PATTERN = re.compile(r'''>[^<]+<''')
PRICE_PATTERN = re.compile(r'''\d+.?\d+''')
MONTH_PATTERN = re.compile(r'''\d+''')
CITY_PATTERN1 = re.compile(r'''<a\s+href="http://\w+\.58\.com/"\s+onclick="co\(\'\w+\'\)">[^<]+</a>''')
DISTRICTURLPATTERN = re.compile(r'''http://\w+\.58\.com/xiaoqu/\w+/''')
URLDISTRICTNAMEPATTERN = re.compile(r'''<a\s+href="http://\w+\.58\.com/xiaoqu/\w+/"\s+target="_blank"\s+class="t">[^<]+<span''')
NEXTPAGEPATTERN = re.compile(r'''<a\s+href=\'http://\w+\.58\.com/xiaoqu/\w+/\'\s+class=\'next\'><span>下一页</span></a>''')
DATEPATTERN = re.compile(r'''chart.setDataXML\(".+?<category\s+name=.(?P<m1>.+?).\s+/>\s*<category\s+name=.(?P<m2>.+?).'''
 '''/>\s*<category\s+name=.(?P<m3>.+?).\s+/>\s*<category\s+name=.(?P<m4>.+?).\s+/>\s*<category\s+name=.(?P<m5>.+?).\s+/>\s*<category\s+name=.(?P<m6>.+?).\s+/>.+?"\);''')
DATAPATTERN = re.compile(r'''chart.setDataXML\("[^居]+hoverText=.(?P<c11>.+?)\{\w+\}(?P<price11>\d*.?\d*元/月)./><set'''
                          '''\s+value=.+?hoverText=.(?P<c21>.+?)\{br\}(?P<price21>\d*.?\d*元/月)./><set\s+value=.+?hoverText=.(?P<c31>.+?)\{br\}(?P<price31>\d*.?\d*元/月)./><set\s+value=.+?hoverText=.(?P<c41>.+?)\{br\}(?P<price41>\d*.?\d*元/月)./><set\s+value=.+?hoverText=.(?P<c51>.+?)\{br\}(?P<price51>\d*.?\d*元/月)./><set\s+value=.+?hoverText=.(?P<c61>.+?)\{br\}(?P<price61>\d*.?\d*元/月)./></dataset><dataset\s+color=.\w+.><set\s+value=.+?hoverText=.(?P<c12>.+?)\{br\}(?P<price12>\d*.?\d*元/月)./><set\s+value=.+?hoverText=.(?P<c22>.+?)\{br\}(?P<price22>\d*.?\d*元/月)./><set\s+value=.+?hoverText=.(?P<c32>.+?)\{br\}(?P<price32>\d*.?\d*元/月)./><set\s+value=.+?hoverText=.(?P<c42>.+?)\{br\}(?P<price42>\d*.?\d*元/月)./><set\s+value=.+?hoverText=.(?P<c52>.+?)\{br\}(?P<price52>\d*.?\d*元/月)./><set\s+value=.+?hoverText=.(?P<c62>.+?)\{br\}(?P<price62>\d*.?\d*元/月)./></dataset><dataset\s+color=.\w+.><set\s+value=.+?hoverText=.(?P<c13>.+?)\{br\}(?P<price13>\d*.?\d*元/月)./><set\s+value=.+?hoverText=.(?P<c23>.+?)\{br\}(?P<price23>\d*.?\d*元/月)./><set\s+value=.+?hoverText=.(?P<c33>.+?)\{br\}(?P<price33>\d*.?\d*元/月)./><set\s+value=.+?hoverText=.(?P<c43>.+?)\{br\}(?P<price43>\d*.?\d*元/月)./><set\s+value=.+?hoverText=.(?P<c53>.+?)\{br\}(?P<price53>\d*.?\d*元/月)./><set\s+value=.+?hoverText=.(?P<c63>.+?)\{br\}(?P<price63>\d*.?\d*元/月)./>.+?\);''')

SVR_IP = '127.0.0.1'
SVR_PORT = 64005
TIME_OUT = 100000

class Tester:
  def Init(self):
    try:
      city_list = []
      return True
    except Exception as ex:
      print 'Exception:%s' % (ex)
      return False

  def GetCityList(self):
    downloader = Downloader()
    body = downloader.Get(city_url, city_url)
    if body == None:
      print 'download[%s] failed'
      return -1

    self.city_list = CITY_PATTERN.findall(body)
    return 0

  def DownloadSourceFile(self, url, filename):
    try:
      time.sleep(0.8)
      print "downloading [%s]" % (url)
      #wp = urllib.urlopen(url)
      #content = wp.read()
      downloader = Downloader()
      content = downloader.Get(url, url)
      #print content.decode('gb2312')
      filename = "%s/%s"% (os.getcwd(), filename)
      fp = open(filename, "w")
      #fp.write(content.decode('gb2312'))
      fp.write(content)
      fp.close()
      return content
    except Exception as ex:
      print 'Exception:%s' % (ex)
      return False

  def DownloadContent(self, url, refer):
    try:
      time.sleep(0.8)
      print "downloading [%s]" % (url)
      #wp = urllib.urlopen(url)
      #content = wp.read()
      downloader = Downloader()
      content = downloader.Get(url, refer)
      if type(content) != str:
         content = str(content)
      #print content.decode('gb2312')
      return content
    except Exception as ex:
      print 'Exception:%s' % (ex)
      return False

  def ExtractKeywordSecondLevel(self, content, cityname):
    try:
      content = DISTRICTURLPATTERN.findall(content)
      if len(content) <= 0:
         return ""
      start1 = content[0].find('''http''')
      end1 = content[0].find('''.''',start1 + 7)
      districtpattern = content[0][start1 + 7:end1]
      
      start2 = content[0].find('''newcode=''')
      end2 = content[0].find('''"''',start2 + 8)
      districtcode = content[0][start2 + 8:end2]

      districturl = '''http://%s.soufun.com/estimate/process/makerentchartdata.aspx?dis=&newcode=%s&city=%s&district=&commerce=''' % (districtpattern.strip(), districtcode.strip(), cityname.strip())
      #print districturl
      return districturl
    except Exception as ex:
      print 'Exception:%s' % (ex)

  def ExtractFileSecondLevel(self, wfp, cityname, content, newcodelist):
    try:
      if len(content) <= 10:
        return ""
      start1 = content.find('''<table class="tbimg"''')
      end1 =  content.find('''</table>''', start1)
      content1 = content[start1:end1]
      urlandnamelist = URLDISTRICTNAMEPATTERN.findall(content1) 
      for urlandname in urlandnamelist:
         secondlevelurl = DISTRICT_URL_PATTERN1.findall(urlandname)
         start = urlandname.find("class")
         urlandname = urlandname[start:]
         districtname = CITY_NAME_PATTERN.findall(urlandname)
         if len(secondlevelurl) == 0 or len(districtname) == 0:
           continue
         #print "=>" + districtname[0][1:-1].strip() + secondlevelurl[0]
         if len(secondlevelurl) == 0:
            continue
         newcodelist.append((districtname[0][1:-1].strip(), secondlevelurl[0]))
      pageurl = NEXTPAGEPATTERN.findall(content)
      if len(pageurl) == 0:
         return ""
      start2 = pageurl[0].find('''http''')
      end2 = pageurl[0].find("\'",start2)
      nextpageurl = pageurl[0][start2:end2]
      print "<---" + nextpageurl + "---->"
      if len(nextpageurl) > 0:
        return nextpageurl
      else:
        return ""
    except Exception as ex:
      print 'Exception:%s' % (ex)

  def DownloadAndExtractSecondLevelSourceFile(self, url, refer, cityname, filename):
    try:
      #wp = urllib.urlopen(url)
      #content = wp.read()
      filename = "%s/%s"% (os.getcwd(), filename)
      fp = open(filename, "w")
      downloader = Downloader()
      content = downloader.Get(url, url)
      content = self.DownloadContent(url, refer)
      newcodelist = []
      nextpageurl = ""
      if len(content) <= 10:
         print 'DownloadAndExtractSecondLevelSourceFile content empty [%s]'% url
         return newcodelist
      print 'DownloadAndExtractSecondLevelSourceFile: [%s] length: [%d] [%s]' % (url, len(content), content[0:20] )
      relativeurl = self.ExtractFileSecondLevel(fp, cityname, content, newcodelist)
      if relativeurl == "":
         fp.close()
         return newcodelist
      while relativeurl != "":
        print relativeurl
        content  = ""
        content = self.DownloadContent(relativeurl, refer)
        if len(content) <= 10:
          print 'DownloadAndExtractSecondLevelSourceFile content empty [%s]'% url
        print 'DownloadAndExtractSecondLevelSourceFile: [%s] length: [%d] [%s]' % (url, len(content), content[0:20] )
        relativeurl = self.ExtractFileSecondLevel(fp, cityname, content, newcodelist)
      #fp.write(newcodelist)
      fp.close()
      return newcodelist
    except Exception as ex:
      print 'Exception:%s' % (ex)
      return False

  def ExtractFileThirdLevel(self,  wfp,  contentStr, districturl, cityname, districtname, storage):
    try:
       if len(contentStr) == 0:
         return
       s = cityname + '	' + districtname
       datelist = DATEPATTERN.findall(contentStr) 
       datalist = DATAPATTERN.findall(contentStr)
       if len(datelist) <= 0 or len(datalist) <= 0:
          print "[Empty %s:%s]"% (districturl, cityname)
       monthIterator = 0
       #for (category1, price1, category2, price2, category3, price3, categroy4, price4, category5, price5, category6, price6) in datalist:
       datalist = map(list, datalist)
       datelist = map(list, datelist)
       print datalist
       print datelist
       datalist = datalist[0]
       datelist = datelist[0]
       currentYear = time.strftime('%Y', time.localtime(time.time()))
       currentMonth = time.strftime('%m', time.localtime(time.time()))
       for month in datelist:
          s += '\n'
          category1 = datalist[monthIterator]
          price1 = datalist[monthIterator + 1]
          category2 = datalist[monthIterator + 12]
          price2 = datalist[monthIterator + 13]
          category3 = datalist[monthIterator + 24]
          price3 = datalist[monthIterator + 25]
          s += category1 + '	' + price1 + '	' + category2 + '	' + price2 + '	' + category3 + '	' + price3 
          record = {}
          price = PRICE_PATTERN.findall(price1)
          if len(price) > 0:
             record["price"] = float(price[0])
          else:
             record["price"] = 0
          record["huxing"] = category1
          record["url"] = districturl
          record["city"] = str(cityname)
          record["district"] = str(districtname)
          record["crawtime"] = time.strftime('%Y%m', time.localtime(time.time()))
          monthDigit = MONTH_PATTERN.findall(month)
          if len(monthDigit) > 0 :
             if int(monthDigit[0]) > 9:
               thisMonth = '0' + monthDigit[0]
             else:
               thisMonth = monthDigit[0]
             if int(monthDigit[0]) < int(currentMonth):
                record["ym_date"] = currentYear + thisMonth
             else:
                thisYear = int(currentYear)-1 
          storage.append(record) 
          record = {}
          price = PRICE_PATTERN.findall(price2)
          if len(price) > 0:
             record["price"] = float(price[0])
          else:
             record["price"] = 0
          record["huxing"] = category2
          record["url"] = districturl
          record["city"] = str(cityname)
          record["district"] = str(districtname)
          record["crawtime"] = time.strftime('%Y%m', time.localtime(time.time()))
          monthDigit = MONTH_PATTERN.findall(month)
          if len(monthDigit) > 0 :
             if int(monthDigit[0]) > 9:
               thisMonth = '0' + monthDigit[0]
             else:
               thisMonth = monthDigit[0]
             if int(monthDigit[0]) < int(currentMonth):
                record["ym_date"] = currentYear + thisMonth
             else:
                thisYear = int(currentYear)-1 
                record["ym_date"] = str(thisYear) + thisMonth
          storage.append(record) 
          record = {}
          price = PRICE_PATTERN.findall(price3)
          if len(price) > 0:
             record["price"] = float(price[0])
          else:
             record["price"] = 0
          record["huxing"] = category3
          record["url"] = districturl
          record["city"] = str(cityname)
          record["district"] = str(districtname)
          record["crawtime"] = time.strftime('%Y%m', time.localtime(time.time()))
          monthDigit = MONTH_PATTERN.findall(month)
          if len(monthDigit) > 0 :
             if int(monthDigit[0]) > 9:
               thisMonth = '0' + monthDigit[0]
             else:
               thisMonth = monthDigit[0]
             if int(monthDigit[0]) < int(currentMonth):
                record["ym_date"] = currentYear + thisMonth
             else:
                thisYear = int(currentYear)-1 
          storage.append(record) 
          monthIterator += 1
       #print s
       print storage
       wfp.write(s)
       return
    except Exception as ex:
      print 'Exception:%s' % (ex)

  def DownloadAndExtractThirdLevelSourceFile(self, fp, districturl, cityname, districtname, storage):
    try:
      #wp = urllib.urlopen(url)
      #content = wp.read()
      #downloader = Downloader()
      #content = downloader.Get(url, url)
      content = self.DownloadContent(districturl, districturl)
      if len(content) <= 10:
         print 'DownloadAndExtractThirdLevelSourceFile content empty [%s]'%districturl
      print 'DownloadAndExtractThirdLevelSourceFile [%s] length: [%d] [%s]' % (districturl, len(content), content[0:30] )
      start = content.find('''id="dynamic_1"''')
      end = content.find('''</script>''', start)
      content = content[start:end]
      self.ExtractFileThirdLevel(fp, content, districturl, cityname, districtname, storage)
    except Exception as ex:
      print 'Exception:%s' % (ex)
      return False

  def ExtractFileFirstLevel(self, filename, word):
    try:
      rfp = open(filename, "r")
      content = rfp.read()
      startPattern = "%s" %'''<dl id="clist">'''
      start = content.find(startPattern)
      rfp.close()
      content = content[start:content.find("</dl>", start)]
      #print content
      self.city_list = CITY_PATTERN1.findall(content)
      #print self.city_list
      return self.city_list
    except Exception as ex:
      print 'Exception:%s' % (ex)

if __name__ == '__main__':
  tester = Tester()
  ret = tester.Init()
  if ret == False:
    sys.exit(1)
  ipattern = '*.html'
  opattern = '*.dump'
  for fileNameToDel in os.listdir(os.getcwd()):
    if fnmatch.fnmatch(fileNameToDel.strip(), ipattern.strip()):
      os.remove(fileNameToDel.strip())
    if fnmatch.fnmatch(fileNameToDel.strip(), opattern.strip()):
      os.remove(fileNameToDel.strip())
  storage = []
  outputFileName = os.path.join(os.getcwd(), "taocheResult.dump") 
  wfp = open(outputFileName, "w") 
  identify = string.maketrans('', '')
  delCStr = '《》（）&%￥#@！{}【】' 
  chineseLimitation = list(string.digits + string.ascii_lowercase + string.ascii_uppercase + string.punctuation) 
  chineseLimitation = ''.join(chineseLimitation)
  numberLimitation = list(string.digits + string.punctuation) 
  numberLimitation = ''.join(numberLimitation)
  url1 = 'http://www.58.com/changecity.aspx'
  tester.DownloadSourceFile(url1, "58.html")
  firstLevelText = tester.ExtractFileFirstLevel("58.html", "")
  #print firstLevelText
  for fileNameToDel in os.listdir(os.getcwd()): #delete src.html
    if fnmatch.fnmatch(fileNameToDel.strip(), ipattern.strip()):
      os.remove(fileNameToDel.strip())
  for firstLevelWord in firstLevelText:
    firstLevelURLKeyword = CITY_URL_PATTERN1.findall(firstLevelWord)
    if len(firstLevelURLKeyword) == 0 or firstLevelWord.find("其他") >= 0:
      continue
    start3 = firstLevelWord.find("onclick")
    firstLevelWord = firstLevelWord[start3:]
    cityname = CITY_NAME_PATTERN.findall(firstLevelWord)
    firstLevelCityNameKeyword = ""
    if len(cityname) > 0:
      firstLevelCityNameKeyword = cityname[0][1:-1]

    firstLevelStartCur = firstLevelURLKeyword[0].find('''http''')
    firstLevelEndCur = firstLevelURLKeyword[0].find(".", firstLevelStartCur + 7)
    firstLevelCityKeyword = firstLevelURLKeyword[0][firstLevelStartCur + 7 :firstLevelEndCur]
    detailFileName1 = firstLevelCityKeyword + ".city"
    firstLevelURLKeyword[0] += "xiaoqu/"
    SecondLevelDistrictCodeKeywordlist = tester.DownloadAndExtractSecondLevelSourceFile(firstLevelURLKeyword[0], url1, firstLevelCityKeyword, detailFileName1)
    print SecondLevelDistrictCodeKeywordlist
    FileNameToDelete = os.path.join(os.getcwd(), detailFileName1)
    os.remove(FileNameToDelete.strip())
    for (SecondLevelDistrictNameKeyword, districturl) in SecondLevelDistrictCodeKeywordlist:
      print  str(SecondLevelDistrictNameKeyword), str(firstLevelCityNameKeyword), districturl
      tester.DownloadAndExtractThirdLevelSourceFile(wfp, districturl, firstLevelCityNameKeyword, SecondLevelDistrictNameKeyword, storage)
  wfp.close()
