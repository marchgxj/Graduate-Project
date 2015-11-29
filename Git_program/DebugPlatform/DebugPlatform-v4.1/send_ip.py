__author__ = 'Changxiaodong'
import socket
import time
import struct
import smtplib
import urllib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.image import MIMEImage
import re,urllib2

def sendEmail(smtpserver,username,password,sender,receiver,subject,msghtml):
    msgRoot = MIMEMultipart('related')
    msgRoot["To"] = ','.join(receiver)
    msgRoot["From"] = sender
    msgRoot['Subject'] = subject
    msgText = MIMEText(msghtml,'html','utf-8')
    msgRoot.attach(msgText)
    smtp = smtplib.SMTP()
    smtp.connect(smtpserver)
    smtp.login(username, password)
    smtp.sendmail(sender, receiver, msgRoot.as_string())
    smtp.quit()

def check_network():
    while True:
        try:
            result=urllib.urlopen('http://baidu.com').read()
            print "Network is Ready!"
            break
        except Exception , e:
            print e
            print "Network is not ready,Sleep 5s...."
            time.sleep(5)


def get_ip_address():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("1.1.1.1",80))
    ipaddr=s.getsockname()[0]
    s.close()
    return ipaddr

def visit(url):
         opener = urllib2.urlopen(url)
         if url == opener.geturl():
             str = opener.read()
         return re.search('\d+\.\d+\.\d+\.\d+',str).group(0)

def get_public_ip_address():
    try:
        myip = visit("http://www.ip138.com/ip2city.asp")
    except:
        try:
            myip = visit("http://www.whereismyip.com/")
        except:
            myip = "So sorry!!!"
    return myip





if __name__ == '__main__':
    check_network()
    inerip=get_ip_address()
    pubip = get_public_ip_address()
    sendEmail('smtp.qq.com','443555619@qq.com','!@#344747','443555619@qq.com',['443555619@qq.com'],'IP Address Of Raspberry Pi',"Public IP:" + pubip +"<br>Local IP:" + inerip)
    print "IP address send"

