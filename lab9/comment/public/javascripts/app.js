

angular.module('comment', [])
.controller('MainCtrl', [
  '$scope','$http',
  function($scope,$http){
    $scope.test = 'Hello world!';
    $scope.comments = [
      {title:'Comment 1', upvotes:5},
      {title:'Comment 2', upvotes:6},
      {title:'Comment 3', upvotes:1},
      {title:'Comment 4', upvotes:4},
      {title:'Comment 5', upvotes:3}
    ];
    $scope.create = function(comment) {
    return $http.post('/comments', comment).success(function(data){
      $scope.comments.push(data);
    });
    };
    $scope.addComment = function() {
      if($scope.formContent === '') { return; }
      console.log("In addComment with "+$scope.formContent);
      $scope.create({
        title: $scope.formContent,
        upvotes: 0,
      });
      $scope.formContent = '';
    };
    $scope.upvote = function(comment) {
      return $http.put('/comments/' + comment._id + '/upvote')
        .success(function(data){
          console.log("upvote worked");
          comment.upvotes += 1;
        });
    };
    $scope.downvote = function(comment) {
      return $http.put('/comments/' + comment._id + '/downvote')
        .success(function(data){
          console.log("downvote worked");
          comment.upvotes -= 1;
        });
    };
    $scope.incrementUpvotes = function(comment) {
      $scope.upvote(comment);
    };
    $scope.decrementUpvotes = function(comment) {
      $scope.downvote(comment);
    };
    $scope.flag = function(comment) {
      return $http.put('/comments/' + comment._id + '/flag')
        .success(function(data){
          console.log("flag worked");
          comment.flagged = true;
        });
    };
    $scope.unflag = function(comment) {
      return $http.put('/comments/' + comment._id + '/unflag')
        .success(function(data){
          console.log("unflag worked");
          comment.flagged = false;
        });
    };
    $scope.flagClicked = function(comment) {
      if(comment.flagged == false)
	$scope.flag(comment);
      else
        $scope.unflag(comment);
    };
    $scope.getAll = function() {
    return $http.get('/comments').success(function(data){
      angular.copy(data, $scope.comments);
    });
    };
    $scope.getAll();
  }
]);