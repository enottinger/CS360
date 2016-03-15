var url = "comment";
$(document).ready(function(){
    $("#serialize").click(function(){
        var myobj = {Name:$("#Name").val(),Comment:$("#Comment").val()};
        jobj = JSON.stringify(myobj);
        $("#json").text(jobj);
	var url = "comment";
	$.ajax({
	  url:url,
	  type: "POST",
	  data: jobj,
	  contentType: "application/json; charset=utf-8",
	  success: function(data,textStatus) {
	      $("#done").html(textStatus);
	  }
	})
    });
    $("#getThem").click(function() {
      $.getJSON('comment', function(data) {
        console.log(data);
        var everything = "<ul>";
        for(var comment in data) {
          com = data[comment];
          everything += "<li>Name: " + com.Name + " -- Comment: " + com.Comment + "</li>";
        }
        everything += "</ul>";
        $("#comments").html(everything);
      })
   });
$("#register").click(function(){
        var myobj = {Username:$("#Username").val(),AvatarURL:$("#Avatar").val()};
        jobj = JSON.stringify(myobj);
        $("#jsonU").text(jobj);
	var url = "users";
	$.ajax({
	  url:url,
	  type: "POST",
	  data: jobj,
	  contentType: "application/json; charset=utf-8",
	  success: function(data,textStatus) {
	      $("#doneU").html(textStatus);
	  }
	})
    });
    $("#getUsers").click(function() {
       $.getJSON('users', function(data) {
        console.log(data);
        var everything = "<ul>";
        for(var user in data) {
          us = data[user];
          everything += "<li>Name: " + us.Username + "<br><img style=\"max-width: 100px\" src = \"" + us.AvatarURL + "\"/>";
          everything += "<div id=\"" + us.Username+ "\"></div></li>"
	  
        }
        everything += "</ul>";
	everything += "<br><button id=\"loadComments\">Get Comments</button>";
	console.log(everything);
        $("#users").html(everything);
      });
   
   });

   $("#loadComments").click(function(data) {
      $.getJSON('comment', function(cdata){
	      console.log(cdata);
	      var username = us.userName
              var everything2 = "<ul>";
       	      for(var comment in cdata) {
        	 com = cdata[comment];
     

		   everything2 = "<li>Comment: " + com.Comment + "</li>";
		    $("#"+com.Name).append(everything2);
		
       	      }
             
	      console.log(everything2);
	  });   
   });

});


