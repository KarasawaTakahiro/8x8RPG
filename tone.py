#coding: utf-8

def conv(f):
    return int((8000000.0/(2*64*f) - 1) * 10) / 10.0

l = [130.8, 138.6, 146.8, 155.6, 164.8, 174.6, 185.0, 196.0, 207.7, 220.0, 246.9]
tone = [l]

for i in xrange(1, 5):
    t = []
    for j in xrange(len(tone[i-1])):
        t.append(int(tone[i-1][j]*2*10)/10.0)
    tone.append(t)


for i in xrange(len(tone)):
    print "{ ",
    for j in xrange(len(tone[i])):
        print str(conv(tone[i][j])) + ", " ,
    print "},"


