class InputHandler{
    constructor(chicken, game){
        document.addEventListener("keydown", event => {
            switch (event.keyCode) {
              case 37:
                chicken.moveLeft();
                break;

              case 38:
                chicken.moveUp();
                break;
      
              case 39:
                chicken.moveRight();
                break;

              case 40:
                chicken.moveDown();
                break;
  
              case 32:
                game.togglePause();
                break;
            }
          });
  
          document.addEventListener("keyup", event => {
            switch (event.keyCode) {
              case 37:
                if (chicken.speedX < 0) chicken.stopX();
                break;

              case 38:
                if (chicken.speedY < 0) chicken.stopY();
                break;
      
              case 39:
                if (chicken.speedX > 0) chicken.stopX();
                break;

              case 40:
                if (chicken.speedY > 0) chicken.stopY();
                break;
            }
          });


          document.addEventListener("mousemove", event => {
            var rect = canvas.getBoundingClientRect();
            var mousePositionx = event.clientX-rect.left;
            var mousePositiony = event.clientY-rect.top;
            console.log(mousePositionx);
            console.log(mousePositiony);
          });
  

    }
  }