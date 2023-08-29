var bigImage = document.querySelector(".image-wrapper img");
var images = document.querySelectorAll(".images img");

for( var i = 0; i < images.length; i++ ){
	images[i].addEventListener("click", function(e){
		e.preventDefault();
		bigImage.setAttribute("src", this.getAttribute("src"));	
	});
}