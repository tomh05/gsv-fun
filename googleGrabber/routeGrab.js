// Google API V3, JSON access
// TODO display copywrite from map data

var gm = require('googlemaps');
var util = require('util');
var polyline = require('polyline'); // google polyline decoding
var fs = require('fs');
var argv = require('optimist').argv;

if (argv.from == undefined || argv.to == undefined) {
    console.log("Error: missing --from or --too in arguments");
    process.exit(code=1)
}


gm.config("proxy","http://www-cache.rd.bbc.co.uk");

//var url = gm.directions("MediaCityUK","Old Trafford, Manchester, UK", function fn(d) { console.log("data is here");}, 'false');
var url = gm.directions(argv.from,argv.to, dirCallback , 'false');


function dirCallback(error,data) {
    var stepList =  [];
    if (error) {
        console.log("error" + error);
        return;
    } 
    
    //console.log("Copyright: " + data.routes[0].copyrights);

    var steps = data.routes[0].legs[0].steps;

    for (var s=0; s<steps.length;s++) {
        var stepPoints = polyline.decode(steps[s].polyline.points);
        for (var p = 0; p < stepPoints.length; p++) {
            stepList.push(stepPoints[p]);
        }
    }

    fs.writeFile('steps.json', JSON.stringify(stepList,null), function(err) {});
}
