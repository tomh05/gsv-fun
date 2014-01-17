#!/usr/bin/python

import StreetView as sv
import simplejson
from PIL import Image
from StringIO import StringIO
import urlSigner
import math
import sys

# command line parsing
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("-W","--width", dest="imageWidth", default=800)
parser.add_argument("-H","--height",dest="imageHeight", default=400)
parser.add_argument("-q","--fov",dest="fov", default=90)
parser.add_argument("-c","--client",dest="client")
parser.add_argument("-k","--key",dest="key")
parser.add_argument("-f","--folder",dest="foldername")
parser.add_argument("-m","--mode",dest="mode", default="static")
args = parser.parse_args()

stepsFile = open("steps.json","r")
steps = simplejson.load(stepsFile)

def bearing(a,b):
    lat1 = math.radians(a[0])
    lat2 = math.radians(b[0])
    dLon = math.radians(b[1] - a[1])

    x = math.sin(dLon) * math.cos(lat2)
    y = math.cos(lat1) * math.sin(lat2) - math.sin(lat1) * math.cos(lat2) * math.cos(dLon)
    bearingRad = math.atan2(x,y)

    return math.degrees(bearingRad)

interpSteps = []
minDist = 0.0002

for s,step in enumerate(steps[:-1]):
    interpSteps.append(step)
    distBetween = math.sqrt((steps[s][0] - steps[s+1][0])**2 + (steps[s][1] - steps[s+1][1])**2)
    #print "dist is "+str(distBetween)
    if (distBetween>(minDist*0.2)):
        n = int(distBetween/minDist)
        for i in range(1,n):
            f = float(i)/float(n)
            newPoint = [(1-f)*steps[s][0] + f*steps[s+1][0], (1-f)*steps[s][1] + f*steps[s+1][1] ] 
            #print "adding interp "+str(newPoint)
            interpSteps.append(newPoint)

interpSteps.append(steps[-1]) # append last
'''
# debug: print all steps after interpolation. 
# They can be plotted in http://www.darrinward.com/lat-long/
testfile = open('intersteps.csv','wb')
for coord in interpSteps:
    testfile.write(str(coord[0]) + ',' + str(coord[1]) + '\n')
testfile.write('\n')
testfile.close()
'''
routeFile = open(args.foldername +"/route.dat","wb")

lastId = 0;

for s,step in enumerate(interpSteps):
    pano = None
    pano = sv.GetPanoramaMetadata(lat=step[0], lon=step[1],radius = 6)
    if pano is None:
        continue

    # only download unique panoramas
    if (pano.PanoId != lastId):
        lastId = pano.PanoId

        print "Fetching image "+str(s) +" of " + str(len(interpSteps)) + " possible panorama locations     \r",
        sys.stdout.flush()
        
        if (s < len(interpSteps) - 1):
            heading = bearing(interpSteps[s],interpSteps[s+1])


        routeFileLine  = str(s) + "," + pano.PanoId + "," + \
                str(pano.Lat[0].content) + "," + str(pano.Lon[0].content) + "," + \
                str(pano.OriginalLat[0].content) + "," + str(pano.OriginalLon[0].content) + "," +\
                str(pano.ProjectionPanoYawDeg[0].content) + "," + str(pano.ProjectionTiltYawDeg[0].content) + "," +\
                str(heading) + "\n"

        routeFile.write(routeFileLine)

        if (args.mode=="static"):
            # fetch static image from API
            imUrl = "http://maps.googleapis.com/maps/api/streetview?" \
                    + "size="+str(args.imageWidth) + "x" + str(args.imageHeight) \
                    + "&pano="+pano.PanoId  \
                    + "&heading="+ str(heading) \
                    + "&fov="+ str(args.fov) \
                    + "&sensor=false&client=" + args.client   
            imUrl =  urlSigner.googleUrlSign(imUrl,args.key)

            imFromUrl = sv.GetUrlContents(imUrl)
            imFile = open(args.foldername +'/'+ str(s) + '_' +pano.PanoId+'.jpg','wb')
            imFile.write(imFromUrl)
            imFile.close()

        else:
            # panorama grabbing for 3D
            panImg = Image.new("RGB", (3330,1665), "white")

            for i in range(0,7):
                for j in range(0,4):
                    tileData = sv.GetPanoramaTile(pano.PanoId,3, i, j)
                    tileBuffer = StringIO()
                    tileBuffer.write(tileData)
                    tileBuffer.seek(0)
                    tileImg = Image.open(tileBuffer)
                    panImg.paste(tileImg, (512*i,512*j))

            panImg.save(args.foldername + "/" + str(s) + '_' + pano.PanoId + ".jpg")


            # depth map is 512x256 image, and a bunch of plane normals and distances to origin
            indicesFile = open(args.foldername + "/" + str(s) + "_" + pano.PanoId +"_indices.json","wb")
            simplejson.dump(pano.DepthMapIndices,indicesFile)
            indicesFile.close()

            planesFile = open(args.foldername + "/" + str(s) + "_" + pano.PanoId +"_planes.json","wb")
            simplejson.dump(pano.DepthMapPlanes,planesFile);
            planesFile.close()






routeFile.close()
print('\n')
'''
'''


