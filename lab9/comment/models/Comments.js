var mongoose = require('mongoose');
var CommentSchema = new mongoose.Schema({
  title: String,
  upvotes: {type: Number, default: 0},
  flagged: {type: Boolean, default: false}
});
CommentSchema.methods.upvote = function(cb) {
  this.upvotes += 1;
  this.save(cb);
};

CommentSchema.methods.downvote = function(cb) {
  this.upvotes -= 1;
  this.save(cb);
};

CommentSchema.methods.flag = function(cb) {
  this.flagged = true;
  this.save(cb);
}

CommentSchema.methods.unflag = function(cb) {
  this.flagged = false;
  this.save(cb);
}
mongoose.model('Comment', CommentSchema);