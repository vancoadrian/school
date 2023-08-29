class Chicken {
    constructor(game){
        this.image = document.getElementById("chicken");
        this.imagenumber = 0;
        this.gameWidth = game.gameWidth;
        this.gameHeight = game.gameHeight;
        this.game = game;
        this.width = 50;
        this.height = 50;
        this.maxSpeed = 7;
        this.speedX = 0;
        this.speedY = 0;
        this.position = {
            x: game.gameWidth/12  - this.width ,
            y: game.gameHeight/2,
        };

        
    }
    moveUp() {
      this.speedY = -this.maxSpeed;
    }

    moveDown() {
      this.speedY = this.maxSpeed;
    }
    moveLeft() {
      this.speedX = -this.maxSpeed;
    }
    
    moveRight() {
      this.speedX = this.maxSpeed;
    }
    
    stopX() {
      this.speedX = 0;
    }
    stopY() {
      this.speedY = 0;
    }

    draw(ctx){
        ctx.drawImage(this.image,this.position.x, this.position.y, this.width, this.height);
    }

    update(dt){
      //if(detectCollision(this, this.game.car)){this.game.lives--;}
        //console.log(this.game.chicken.position.x);
      
        let bottomOfChicken = this.position.y + this.height;
        let topOfChicken = this.position.y;
        let leftOfChicken = this.position.x;
        let rightOfChicken = this.position.x + this.width;
        let topOfCar = this.game.sedan.position.y;
        let bottomOfCar = this.game.sedan.position.y + this.game.sedan.height;
        let leftSideOfCar = this.game.sedan.position.x;
        let rightSideOfCar = this.game.sedan.position.x + this.game.sedan.width;
       
        if(leftSideOfCar<rightOfChicken && rightSideOfCar > leftOfChicken && topOfCar < bottomOfChicken && bottomOfCar > topOfChicken){
          this.game.lives--;
          this.position.x = game.gameWidth/12  - this.width;
          this.position.y = game.gameHeight/2;
        }

        this.position.x += this.speedX;
        this.position.y += this.speedY;
        if(this.imagenumber<4)
        this.imagenumber++;
        else this.imagenumber = 0;
        console.log(this.imagenumber);
        
        if (this.position.x < 0) {
          this.position.x = 0;           
        }

        if (this.position.x + this.width > this.gameWidth){
        this.position.x = this.gameWidth - this.width;
        }

        if (this.position.y < 0) {
          this.position.y = 0;           
        }

        if (this.position.y + this.height > this.gameHeight){
        this.position.y = this.gameHeight - this.height;
        }
    }
}