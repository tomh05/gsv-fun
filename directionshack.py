from googlemaps import GoogleMaps
gmaps = GoogleMaps() # put api key in
dir = gmaps.directions("mediacityuk","old trafford")
print dir['Directions']['Routes'][0]['Steps'][3]['Point']
