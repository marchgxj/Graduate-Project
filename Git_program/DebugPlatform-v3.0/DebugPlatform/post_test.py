__author__ = 'Changxiaodong'
import urllib
import urllib2
data = {}
data["relay_id"] = "0086-110108-00022105-01"
data["data"] = "0086-110108-00022105-0001|1,0086-110108-00022105-0002|1"
data["park_id"] = "22105"
post_data = urllib.urlencode(data)
urllib2.urlopen("http://123.57.37.66:8080/sensor/post/status", post_data)
print "post ok"