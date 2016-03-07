__author__ = 'Changxiaodong'
import urllib
import urllib2
data = {}
data["relay_id"] = "0086-110108-00022105-01"
data["data"] = "0086-110108-00022105-0005|0"
data["park_id"] = "22105"
post_data = urllib.urlencode(data)
print data
response = urllib2.urlopen("http://www.xiaoxiami.space/info/post/", post_data)
print response.read()
print "post ok"