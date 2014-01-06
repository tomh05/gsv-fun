#!/usr/bin/python


import StreetView as sv
import simplejson


print "loading pano"
#pano = sv.GetPanoramaMetadata(None,54.977275,1.614743,20000)
#pano = sv.GetPanoramaMetadata(lat=27.683528, lon=-99.580078)
pano = sv.GetPanoramaMetadata(lat=54.977275, lon=-1.614743)
#pano = sv.GetPanoramaMetadata(lat=55.001227, lon=-1.631196, radius = 50)
#pano = sv.GetPanoramaMetadata(lat=42.345601, lon = -71.098348, radius = 50)
if pano is None:
   print "empty panorama"
   exit()

print "loading tile"
tile = sv.GetPanoramaTile(pano.PanoId,0, 0, 0);

print "opening file "
output = open("pan1.jpg","wb")

print "writing file "
output.write(tile)
output.close()


# depth map is 512x256 image

print pano.DepthMapPlanes

indicesFile = open("indices.json","wb")
simplejson.dump(pano.DepthMapIndices,indicesFile)
indicesFile.close()

planesFile = open("planes.json","wb")
simplejson.dump(pano.DepthMapPlanes,planesFile);
planesFile.close()
