foldername="mcuk_trafford"

# Route settings
from="MediaCityUk"
to="Old Trafford Stadium"

# Street View Settings
mode="static"
imageWidth=800
imageHeight=600
fov=80

# google credentials
client="gme-britishbroadcasting"
key="ZFb0O2mmKm_HXX6Fi4vYIUj6SQU="

rm -r $foldername
mkdir -p $foldername
node routeGrab.js --from="$from" --to="$to"
echo "Route fetched"
echo "Grabbing images"
python streetViewGrab.py --client "$client" --key "$key" -W $imageWidth -H $imageHeight --fov $fov --folder $foldername
rm steps.json
echo "Grabbed"
