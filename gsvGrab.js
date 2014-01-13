// Google API V3
// TODO display copywrite from map data
var tomsKey = "AIzaSyDXBt3WuscigtM4voPOjNKGsEIznpreAsQ"; // Tom's personal API key


var gm = require('googlemaps');
var util = require('util');
var polyline = require('polyline');
var fs = require('fs');


gm.config("proxy","http://www-cache.rd.bbc.co.uk");


//var url = gm.directions("MediaCityUK","Old Trafford, Manchester, UK", function fn(d) { console.log("data is here");}, 'false');
var url = gm.directions("MediaCityUK"," Manchester Piccadilly, UK", dirCallback , 'false');
console.log("calling " +url);




function dirCallback(error,data) {
    var stepList =  [ ];

    if (error) {
        console.log("error" + error);
        return;
    } 
    console.log("I have data");
    console.log("Copyright: " + data.routes[0].copyrights);

    var steps = data.routes[0].legs[0].steps;

    for (var s=0; s<steps.length;s++) {
        console.log("step");
        console.log(steps[s].start_location);
        var stepPoints = polyline.decode(steps[s].polyline.points);
        console.log(stepPoints);
        //stepList.concat(stepPoints);
        for (var p = 0; p < stepPoints.length; p++) {

        stepList.push(stepPoints[p]);
        }
    }
    console.log("steplist is");
    console.log(stepList.length);

    //var file = fs.createWriteStream('steps.json');
    fs.writeFile('steps.json', JSON.stringify(stepList,null), function(err) {});
    //file.on('error', function(err) { console.log(err);});
    //stepList.forEach(function(v) { file.write(v.join(', ') + '\n'); });
    //file.end();
    /*
    fs.open("steps.json",'w',function(e,id) {
        fs.write(id, stepList, null, 'utf8', function() {
            fs.close(id, function() {
                console.log('written file');
            });
        });
    });
    */
}



