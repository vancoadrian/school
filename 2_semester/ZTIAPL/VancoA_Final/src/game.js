class Game{
  constructor(gameWidth, gameHeight){
    this.gameWidth = gameWidth;
    this.gameHeight = gameHeight;
    this.gamestate = GAMESTATE.MENU ;
    this.chicken = new Chicken(this); 
    this.bus = new Bus(this);
    this.sedan = new Sedan(this);
    this.gameObjects = [];
    this.lives= 3;
    this.music=new Sound("./sounds/ukulele.mp3");
    new InputHandler(this.chicken,this);
  }
  
  start(){
    if(this.gamestate !== GAMESTATE.MENU) return;
    
    let woods = buildLevel(this, level1);
    this.gameObjects = [ this.bus, this.sedan, ...woods];
    this.chicken;
    this.gamestate = GAMESTATE.RUNNING;
    this.startBtn = document.getElementById("startBtn");
    this.startBtn.style.display = "none";
    this.instrBtn = document.getElementById("instrBtn");
    this.instrBtn.style.display = "none";
    this.instrBtn = document.getElementById("soundBtn");
    this.instrBtn.style.display = "none";
  }
  
  update(dt){
    if( this.lives === 0) this.gamestate = GAMESTATE.OVER;

    if(this.gamestate === GAMESTATE.PAUSED || this.gamestate === GAMESTATE.MENU || this.gamestate === GAMESTATE.OVER) return;
    //this.chicken.update(dt);
    //this.instrBtn = document.getElementById("instrBtn");
    //this.instrBtn.style.display = "none";
    //if(detectCollision(this, this.game.chicken)){}
    this.gameObjects.forEach((object) => object.update(dt));
    this.chicken.update(dt);
  }
  
  draw(ctx){
    //this.chicken.draw(ctx);
    this.instrBtn = document.getElementById("soundBtn");
    this.instrBtn.style.display = "none";
    ctx.drawImage(document.getElementById("gameBack"),0, 0, canvas.width, canvas.height);
    this.gameObjects.forEach((object) => object.draw(ctx));
    this.chicken.draw(ctx);

    if(this.gamestate == GAMESTATE.MENU){
      //ctx.rect(0, 0, this.gameWidth, this.gameHeight);
      //ctx.fillStyle = ("rgba(0,0,0,1)");
      //ctx.fill();
      ctx.drawImage(document.getElementById("menuBack"),0, 0, canvas.width, canvas.height+15);
      this.startBtn = document.getElementById("startBtn");
      this.startBtn.style.display = "block";
      this.instrBtn = document.getElementById("instrBtn");
      this.instrBtn.style.display = "block";
      this.instrBtn = document.getElementById("soundBtn");
      this.instrBtn.style.display = "block";
      this.instrBtn = document.getElementById("backBtn");
      this.instrBtn.style.display = "none";
    }

    if(this.gamestate == GAMESTATE.PAUSED){
      ctx.drawImage(document.getElementById("pauseBack"),0, 0, canvas.width, canvas.height);
      //this.instrBtn = document.getElementById("instrBtn");
      //this.instrBtn.style.display = "block";
      this.instrBtn = document.getElementById("soundBtn");
      this.instrBtn.style.display = "block";
    }

    if(this.gamestate == GAMESTATE.OVER){
      ctx.drawImage(document.getElementById("overBack"),0, 0, canvas.width, canvas.height);
      //ctx.font = "30px Arial";
      //ctx.fillStyle = "red";
      //ctx.textAlign = "center";
      //ctx.fillText("GAME OVER", this.gameWidth / 2, this.gameHeight / 2);
    }

    if(this.gamestate == GAMESTATE.INSTRUCTIONS){
      
      ctx.drawImage(document.getElementById("instrBack"),0, 0, canvas.width, canvas.height);
        this.startBtn = document.getElementById("startBtn");
        this.startBtn.style.display = "none";
        this.instrBtn = document.getElementById("instrBtn");
        this.instrBtn.style.display = "none";
        this.instrBtn = document.getElementById("soundBtn");
        this.instrBtn.style.display = "none";
        this.instrBtn = document.getElementById("backBtn");
        this.instrBtn.style.display = "block";
      }

  }

  togglePause(){
    if(this.gamestate == GAMESTATE.PAUSED){
      this.gamestate = GAMESTATE.RUNNING;
    } else {
      this.gamestate = GAMESTATE.PAUSED;
    }
  }

  playMusic(){
    if(game.music.play_started){
      game.music.stop();
    }else{
      game.music.play();
    }
  }
}