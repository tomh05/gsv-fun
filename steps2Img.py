#!/usr/bin/python

import StreetView as sv
import simplejson
from PIL import Image
from StringIO import StringIO
import urlSigner

stepsFile = open("steps.json","r")
steps = simplejson.load(stepsFile)


lastId = 0;

for s,step in enumerate(steps):
    print "coordinate " + str(step)
    pano = None
    pano = sv.GetPanoramaMetadata(lat=step[0], lon=step[1],radius = 6)
    #print pano[0]['panoid']
    #print pano['PanoMapPanos']
    if pano is None:
        continue

    print pano.PanoId
    if (pano.PanoId != lastId):
        lastId = pano.PanoId
        # compute direction
        #print "dir";
        #nl,nlat = steps[s+1][0] - steps[s][0], steps[s+1][1] - steps[s][1];
       
        imUrl = "http://maps.googleapis.com/maps/api/streetview?size=800x400&pano="+pano.PanoId  +"&sensor=false&client=gme-britishbroadcasting"
        print urlSigner.googleUrlSign(imUrl)




'''
panImg = Image.new("RGB", (3330,1665), "white")

for i in range(0,7):
    for j in range(0,4):
        tileData = sv.GetPanoramaTile(pano.PanoId,3, i, j)
        tileBuffer = StringIO()
        tileBuffer.write(tileData)
        tileBuffer.seek(0)
        #tileImg = Image.new("RGB", (512,512), None)
        #tileImg = Image.frombuffer("RGB",(512,512),jpg,"raw"
        #tileImg = Image.fromstring('RGB',(512,512),tileData,"jpeg",0,0)
        tileImg = Image.open(tileBuffer)
        panImg.paste(tileImg, (512*i,512*j))

panImg.save("img/" +pano.PanoId + ".jpg")


# depth map is 512x256 image, and a bunch of plane normals and distances to origin
indicesFile = open("indices.json","wb")
simplejson.dump(pano.DepthMapIndices,indicesFile)
indicesFile.close()

planesFile = open("planes.json","wb")
simplejson.dump(pano.DepthMapPlanes,planesFile);
planesFile.close()

'''



