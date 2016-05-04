import urllib2
import urllib
import time

data = {}
data["relay_id"] = "0086-110108-00022105-01"
data["park_id"] = "22105"
data["data"] = ""
count = 0
with open("log.txt", "w") as file:
    file.write("running....\n")
while 1:
    data["data"] = ""
    count += 1
    status = count % 2
    for i in range(8, 20):
        data["data"] = data["data"] + "0086-110108-00022105-" + str(i).zfill(4) + "|" \
                       + str(status) + ','
    data["data"] = data["data"][:-1]
    before = time.time()
    try:
        response = urllib2.urlopen("http://www.xiaoxiami.space/info/post/", urllib.urlencode(data), timeout=1)
        feedback = eval(response.read())
        print feedback
    except err:
        with open("log.txt", "a") as file:
            file.write(str(count) + ":" + err + "\n")
    after = time.time()
    delay = after - before
    if delay > 1:
        with open("log.txt", "a") as file:
            file.write(str(count) + ":" + str(delay) + "\n")
    print delay

    time.sleep(1)
