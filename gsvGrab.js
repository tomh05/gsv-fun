// Google API V3
// TODO display copywrite from map data
var tomsKey = "AIzaSyDXBt3WuscigtM4voPOjNKGsEIznpreAsQ"; // Tom's personal API key


var gm = require('googlemaps');
var util = require('util');
var polyline = require('polyline');


gm.config("proxy","http://www-cache.rd.bbc.co.uk");


//var url = gm.directions("MediaCityUK","Old Trafford, Manchester, UK", function fn(d) { console.log("data is here");}, 'false');
var url = gm.directions("MediaCityUK","Old Trafford, Manchester, UK", dirCallback , 'false');
console.log("calling " +url);
function dirCallback(error,data) {

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
    }

}


