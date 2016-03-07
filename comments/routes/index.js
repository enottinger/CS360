var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});

router.post('/comment', function(req, res, next) {
  console.log("POST comment route");
  console.log(req.body);
  res.sendStatus(200);
});

module.exports = router;
