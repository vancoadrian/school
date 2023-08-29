class Wood {
    constructor(game,position){
      this.gameWidth = game.gameWidth;
      this.gameHeight = game.gameHeight;
      this.width = 50;
      this.height = 100;
      this.position = position;
      this.speed = { x: 0, y: 2};
      this.image = new Image();
      this.image.src = "assets/wood.png";
      this.game = game;
    }
  
    update(dt){
     // if(detectCollision(this.game.car, this)){
        //this.game.car.speed.y= -this.game.car.speed.y;
      //}
      if(this.position.x >= canvas.width - this.width || this.position.x <= 0){
        this.speed.x =-this.speed.x;
      }
  
      //wall on top or bottom 
      if(this.position.y >= canvas.height - this.height + 150 || this.position.y <= 0-150){
        this.speed.y =-this.speed.y;
      }
      this.position.x += this.speed.x;
      this.position.y += this.speed.y;
    }
    draw(ctx){
        ctx.save();
        ctx.drawImage(this.image,this.position.x,this.position.y,this.width,this.height);
        ctx.restore();
    }
  }
  
  class Car{
    constructor(game){
      this.gameWidth = game.gameWidth;
      this.gameHeight = game.gameHeight;
      this.width = 50;
      this.height = 50;
      this.speed = { x: -2, y: 2};
      this.position = { x: 200, y: 100,};
      this.image = new Image();
  
      this.game = game;
    }
  
    update(dt){
      //console.log(this.game.chicken.position.x);
  
      //wall on left or right
      if(this.position.x >= canvas.width - this.width || this.position.x <= 0){
        this.speed.x =-this.speed.x;
      }
  
      //wall on top or bottom 
      if(this.position.y >= canvas.height - this.height + 100 || this.position.y <= 0-100){
        this.speed.y =-this.speed.y;
      }
  
      this.position.x += this.speed.x;
      this.position.y += this.speed.y;
    }
    draw(ctx){
        ctx.save();
        ctx.drawImage(this.image,this.position.x,this.position.y,this.width,this.height);
        ctx.restore();
    }
    
  }
  
  class Bus extends Car {
    constructor(game){
        super(game);
        this.speed.x *=0;
        this.speed.y *=2;
        this.position.x=260;
    this.image.src = "assets/bus.png";
  }
  }
  
  class Sedan extends Car {
    constructor(game){
        super(game);
        this.speed.x *=0;
        this.speed.y *=4;
    this.image.src = "assets/sedan.png";
  }
  }