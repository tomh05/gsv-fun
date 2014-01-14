#!/usr/bin/python


import StreetView as sv
import simplejson
from PIL import Image
from StringIO import StringIO
import random

print "loading pano"

pano = None

#pano = sv.GetPanoramaMetadata(None,54.977275,1.614743,20000)
#pano = sv.GetPanoramaMetadata(lat=27.683528, lon=-99.580078)
#pano = sv.GetPanoramaMetadata(lat=(random.uniform(-90,90)), lon=(random.uniform(-180,180)))


#pano = sv.GetPanoramaMetadata(lat=54.977275, lon=-1.614743) # NCL Grey Street

mode = "london"
while pano is None:
    if (mode=="london"):
        ranLat = random.uniform(51.3,51.6)
        ranLon = random.uniform(-0.2,0.1)
    elif (mode=="uk"):
        ranLat = random.uniform(50,60)
        ranLon = random.uniform(-10,3)
    else:
        ranLat = random.uniform(-90,90)
        ranLon = random.uniform(-180,180)
    pano = sv.GetPanoramaMetadata(lat=ranLat, lon=ranLon )

#pano = sv.GetPanoramaMetadata(lat=55.001227, lon=-1.631196, radius = 50) #elgy
#pano = sv.GetPanoramaMetadata(lat=42.345601, lon = -71.098348, radius = 50) #USA


#print "Your destination is (" + str(ranLat) + "," + str(ranLon) + ")\n"

if pano is None:
   print "empty panorama"
   exit()

#panImg = Image.new("RGB", (1668,832), "white")
panImg = Image.new("RGB", (3330,1665), "white")
# range 4,2 for zoom level 2

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

panImg.save("pan1.jpg")


# depth map is 512x256 image, and a bunch of plane normals and distances to origin
indicesFile = open("indices.json","wb")
simplejson.dump(pano.DepthMapIndices,indicesFile)
indicesFile.close()

planesFile = open("planes.json","wb")
simplejson.dump(pano.DepthMapPlanes,planesFile);
planesFile.close()
