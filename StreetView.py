

#!/usr/bin/python

# Taken from 
# https://code.google.com/p/adqmisc/source/browse/trunk/streetview/StreetView.py
# GNU Lesser GPL

import urllib
import urllib2
import libxml2
import inspect
import sys
import zlib
import base64
import struct


BaseUri = 'http://maps.google.com/cbk';

# panoid is the value from panorama metadata
# OR: supply lat/lon/radius to find the nearest pano to lat/lon within radius
def GetPanoramaMetadata(panoid = None, lat = None, lon = None, radius = 2000):
    url =  '%s?'
    url += 'output=xml'			# metadata output
    url += '&v=4'				# version
    url += '&dm=1'				# include depth map
    url += '&pm=1'				# include pano map
    if panoid == None:
        url += '&ll=%s,%s'		# lat/lon to search at
        url += '&radius=%s'		# search radius
        url = url % (BaseUri, lat, lon, radius)
    else:
        url += '&panoid=%s'		# panoid to retrieve
        url = url % (BaseUri, panoid)

    findpanoxml = GetUrlContents(url)
    if findpanoxml.find('data_properties') < 0 :
        return None
    return PanoramaMetadata(libxml2.parseDoc(findpanoxml))

# panoid is the value from the panorama metadata
# zoom range is 0->NumZoomLevels inclusively
# x/y range is 0->?
def GetPanoramaTile(panoid, zoom, x, y):
    url =  '%s?'
    url += 'output=tile'			# tile output
    url += '&panoid=%s'			# panoid to retrieve
    url += '&zoom=%s'			# zoom level of tile
    url += '&x=%i'				# x position of tile
    url += '&y=%i'				# y position of tile
    url += '&fover=2'			# ???
    url += '&onerr=3'			# ???
    url += '&renderer=spherical'		# standard speherical projection
    url += '&v=4'				# version
    url = url % (BaseUri, panoid, zoom, x, y)
    return GetUrlContents(url)

def GetUrlContents(url):
    f = urllib2.urlopen(url)
    data = f.read()
    f.close()
    return data

class PanoramaMetadata:

    def __init__(self, panodoc):
        self.PanoDoc = panodoc
        panoDocCtx = self.PanoDoc.xpathNewContext()
        self.PanoId = self.getXmlVal(panoDocCtx,"/panorama/data_properties/@pano_id")

        self.ImageWidth = self.getXmlVal(panoDocCtx,"/panorama/data_properties/@image_width")
        self.ImageHeight = self.getXmlVal(panoDocCtx,"/panorama/data_properties/@image_height")
        self.TileWidth = panoDocCtx.xpathEval("/panorama/data_properties/@tile_width")
        self.TileHeight = panoDocCtx.xpathEval("/panorama/data_properties/@tile_height")
        self.NumZoomLevels = panoDocCtx.xpathEval("/panorama/data_properties/@num_zoom_levels")
        self.Lat = panoDocCtx.xpathEval("/panorama/data_properties/@lat")
        self.Lon = panoDocCtx.xpathEval("/panorama/data_properties/@lng")
        self.OriginalLat = panoDocCtx.xpathEval("/panorama/data_properties/@original_lat")
        self.OriginalLon = panoDocCtx.xpathEval("/panorama/data_properties/@original_lng")
        self.Copyright = panoDocCtx.xpathEval("/panorama/data_properties/copyright/text()")
        self.Text = panoDocCtx.xpathEval("/panorama/data_properties/text/text()")
        self.Region = panoDocCtx.xpathEval("/panorama/data_properties/region/text()")
        self.Country = panoDocCtx.xpathEval("/panorama/data_properties/country/text()")

        self.ProjectionType = panoDocCtx.xpathEval("/panorama/projection_properties/@projection_type")
        self.ProjectionPanoYawDeg = panoDocCtx.xpathEval("/panorama/projection_properties/@pano_yaw_deg")
        self.ProjectionTiltYawDeg = panoDocCtx.xpathEval("/panorama/projection_properties/@tilt_yaw_deg")
        self.ProjectionTiltPitchDeg = panoDocCtx.xpathEval("/panorama/projection_properties/@tilt_pitch_deg")

        self.AnnotationLinks = []
        for cur in panoDocCtx.xpathEval("/panorama/annotation_properties/link"):
            self.AnnotationLinks.append({ 'YawDeg': cur.xpathEval("@yaw_deg")[0].content,
                'PanoId': self.getXmlVal(cur,"@pano_id"),
                'RoadARGB':self.getXmlVal(cur,"@road_argb"),
                'Text': self.getXmlVal(cur,"link_text/text()"),
                })

            tmp = panoDocCtx.xpathEval("/panorama/model/pano_map/text()")
        if len(tmp) > 0:
            tmp = tmp[0].content
            tmp = zlib.decompress(base64.urlsafe_b64decode(tmp + self.MakePadding(tmp)))			
            self.DecodePanoMap(tmp)

        tmp = panoDocCtx.xpathEval("/panorama/model/depth_map/text()")
        if len(tmp) > 0:
            tmp = tmp[0].content
            tmp = zlib.decompress(base64.urlsafe_b64decode(tmp + self.MakePadding(tmp)))
            self.DecodeDepthMap(tmp)

    def getXmlVal(this,xml,path):
        res = xml.xpathEval(path)
        if len(res) >0:
            return res[0].content
        else:
            return ""


    def MakePadding(self, s):
        return (4 - (len(s) % 4)) * '='


    def DecodePanoMap(self, raw):
        pos = 0

        (headerSize, numPanos, panoWidth, panoHeight, panoIndicesOffset) = struct.unpack('<BHHHB', raw[0:8])
        if headerSize != 8 or panoIndicesOffset != 8:
            print "Invalid panomap data"
            return
        pos += headerSize

        self.PanoMapIndices = [ord(x) for x in raw[panoIndicesOffset:panoIndicesOffset + (panoWidth * panoHeight)]]
        pos += len(self.PanoMapIndices)

        self.PanoMapPanos = []
        for i in xrange(0, numPanos - 1):
            self.PanoMapPanos.append({ 'panoid': raw[pos: pos+ 22]})
            pos += 22

        for i in xrange(0, numPanos - 1):
            (x, y) = struct.unpack('<ff', raw[pos:pos+8])
            self.PanoMapPanos[i]['x'] = x
            self.PanoMapPanos[i]['y'] = y
            pos+=8

    def DecodeDepthMap(self, raw):
        pos = 0

        (headerSize, numPlanes, panoWidth, panoHeight, planeIndicesOffset) = struct.unpack('<BHHHB', raw[0:8])
        if headerSize != 8 or planeIndicesOffset != 8:
            print "Invalid depthmap data"
            return
        pos += headerSize

        self.DepthMapIndices = [ord(x) for x in raw[planeIndicesOffset:planeIndicesOffset + (panoWidth * panoHeight)]]
        pos += len(self.DepthMapIndices)

        self.DepthMapPlanes = []
        for i in xrange(0, numPlanes - 1):
            #(d, nx, ny, nz) = struct.unpack('<ffff', raw[pos:pos+16])
            (nx, ny, nz, d) = struct.unpack('<ffff', raw[pos:pos+16])

            self.DepthMapPlanes.append({ 'd': d, 'nx': nx, 'ny': ny, 'nz': nz }) # nx/ny/nz = unit normal, d = distance from origin
            pos += 16

    def __str__(self):
        tmp = ''
        for x in inspect.getmembers(self):
            if x[0].startswith("__") or inspect.ismethod(x[1]):
                continue

            tmp += "%s: %s\n" % x
        return tmp

#pano = GetPanoramaMetadata(lat=27.683528, lon=-99.580078)
#print pano.PanoId
#print pano.PanoMapPanos
#print pano.DepthMapPlanes
#print GetPanoramaTile(pano.PanoId, 2, 0, 0)
