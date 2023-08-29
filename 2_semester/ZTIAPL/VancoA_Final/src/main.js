let canvas = document.getElementById("canvas");

let ctx = canvas.getContext("2d");

const GAME_WIDTH = 800;
const GAME_HEIGHT = 600;

const GAMESTATE = {

  PAUSED: 0,

  RUNNING: 1,

  MENU: 2,

  OVER: 3,

  INSTRUCTIONS: 4
}


let game = new Game(GAME_WIDTH,GAME_HEIGHT);

let lastTime = 0;

function gameLoop(timestamp){
    let dt = timestamp - lastTime;
    lastTime = timestamp;

    ctx.clearRect(0, 0, GAME_WIDTH, GAME_HEIGHT);
    game.update(dt);
    game.draw(ctx); 
    
    
    requestAnimationFrame(gameLoop);
}

requestAnimationFrame(gameLoop);
