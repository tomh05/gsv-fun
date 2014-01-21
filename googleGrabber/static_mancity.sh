foldername="static-mancity"

# Route settings
from="Manchester Piccadilly Station, Manchester"
to="Piccadilly Gardens, Manchester"

# Street View Settings
mode="static"
imageWidth=1920
imageHeight=1280
fov=80

# google credentials
client="gme-britishbroadcasting"
key="ZFb0O2mmKm_HXX6Fi4vYIUj6SQU="

rm -rf $foldername
mkdir -p $foldername
node routeGrab.js --from="$from" --to="$to"
echo "Route fetched"
echo "Grabbing images"
python streetViewGrab.py --client "$client" --key "$key" -W $imageWidth -H $imageHeight --fov $fov --folder $foldername --mode $mode
rm steps.json
echo "Grabbed"
