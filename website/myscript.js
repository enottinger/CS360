$(function() {
	var icons = {
		header: "ui-icon-carat-1-e",
		activeHeader: "ui-icon-carat-1-s"
	};
	$( "#accordian" ).accordion({
	  collapsible: true,
	  icons: icons
	  });

});

$(document).ready(function() {
	$("#icon").click(function(){
		$("#menu").children('ul').children('li').children('ul').slideUp();
		$("#menu").children('ul').slideToggle();
	});
    $("#menu").children('ul').children('li').click(function(){
    	$(this).children('div').children('ul').slideToggle();
    });	
});
 
