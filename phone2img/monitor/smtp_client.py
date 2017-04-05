#coding:utf-8
# 发送邮件

import smtplib

fromaddr = 'no-reply@ganji.com'
toaddrs  = 'kaifa.base.extractor@ganji.com'
user = 'no-reply'
password = 'Gjhwteam$936wkg'
mail_server = 'mail.ganji.com'

def SendMail(subject, body):
  try:
    msg = ("From: %s\r\nTo: %s\r\nSubject:%s\r\n\r\n"
           % (fromaddr, toaddrs, subject))
    msg = msg + body

    server = smtplib.SMTP(mail_server,587)
    server.ehlo()
    server.starttls()
    server.ehlo()
    server.login(user, password)
    server.set_debuglevel(0)
    server.sendmail(fromaddr, toaddrs, msg)
    server.quit()
    return True
  except:
    print "SendMail failed"
    return False

if __name__ == '__main__':
  SendMail('abc', 'def')
