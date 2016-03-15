var express = require('express');
var router = express.Router();


/* Set up mongoose in order to connect to mongo database */
var mongoose = require('mongoose'); //Adds mongoose as a usable dependency

mongoose.connect('mongodb://localhost/commentDB'); //Connects to a mongo database called "commentDB"

var commentSchema = mongoose.Schema({ //Defines the Schema for this database
  Name: String,
  Comment: String
});

var Comment = mongoose.model('Comment', commentSchema); //Makes an object from that schema as a model

var db = mongoose.connection; //Saves the connection as a variable to use
db.on('error', console.error.bind(console, 'connection error:')); //Checks for connection errors
db.once('open', function() { //Lets us know when we're connected
  console.log('Connected');
});

mongoose.connect('mongodb://localhost/comUserDB'); //Connects to a mongo database called "comUserDB"

var userSchema = mongoose.Schema({ //Defines the Schema for this database
  Username: String,
  AvatarURL: String
});

var User = mongoose.model('User', userSchema); //Makes an object from that schema as a model

var dbU = mongoose.connection; //Saves the connection as a variable to use
dbU.on('error', console.error.bind(console, 'connection error:')); //Checks for connection errors
dbU.once('open', function() { //Lets us know when we're connected
  console.log('Connected to User Database');
});



/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});

router.post('/comment', function(req, res, next) {
  console.log("POST comment route");
  console.log(req.body);

  var newcomment = new Comment(req.body); //[3]
  console.log(newcomment); //[3]
  newcomment.save(function(err, post) { //[4]
    if (err) return console.error(err);
    console.log(post);
    res.sendStatus(200);
  });

});

/* GET comments from database */
router.get('/comment', function(req, res, next) {
  console.log("In the GET route?");
  Comment.find(function(err,commentList) { //Calls the find() method on your database
    if (err) return console.error(err); //If there's an error, print it out
    else {
      console.log(commentList); //Otherwise console log the comments you found
      res.json(commentList);
      
    }
  })
});

router.post('/users', function(req, res, next) {
  console.log("POST comment route");
  console.log(req.body);

  var newcomment = new User(req.body); //[3]
  console.log(newcomment); //[3]
  newcomment.save(function(err, post) { //[4]
    if (err) return console.error(err);
    console.log(post);
    res.sendStatus(200);
  });

});

/* GET comments from database */
router.get('/users', function(req, res, next) {
  console.log("In the GET route?");
  User.find(function(err,userList) { //Calls the find() method on your database
    if (err) return console.error(err); //If there's an error, print it out
    else {
      console.log(userList); //Otherwise console log the comments you found
      res.json(userList);
      
    }
  })
});

module.exports = router;
