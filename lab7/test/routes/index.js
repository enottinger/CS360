var express = require('express');
var router = express.Router();
var fs = require('fs');
var request = require('request');

/* GET home page. */
router.get('/', function(req, res) {
   res.sendFile('weather.html', { root:  'public' });
});

router.get('/getcity', function(req, res){
  console.log("In Getcity");
  console.log("req.query");
  var myRe = new RegExp("^" + req.query.q);
  console.log(myRe);
  fs.readFile(__dirname + '/cities.dat.txt',function(err,data) {
    if(err) throw err;
    var cities = data.toString().split("\n");
    var jsonresult = [];
    /*console.log(cities);*/
    for(var i = 0; i < cities.length; i++){
      var result = cities[i].search(myRe);
      if(result != -1) {
        jsonresult.push({city:cities[i]});
        /*console.log(cities[i]);*/
      }
    }
    /*console.log(jsonresult);*/
    res.status(200).json(jsonresult);
  });
});

/*
var weather = "https://api.wunderground.com/api/f6d07c6d9fb913e3/geolookup/conditions/q/UT"
router.get('/weather', function(req,res) {
   console.log("In weather");

   request(weather + req.query.q).pipe(res);
});

var pokemon = "http://pokeapi.co/api/v2/pokemon"
router.get('/pokemon', function(req,res) {
   console.log("In pokemon");
   console.log(req);
   request(pokemon).pipe(res);
});
*/
module.exports = router;
