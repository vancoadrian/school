function buildLevel(game, level){
    let woods = [];
  
    level.forEach((row, rowIndex) => {
      row.forEach((wood, woodIndex) => {
        if (wood === 1){
          let position = {
            x: 50 * woodIndex,
            y: 100 * rowIndex
  
          }
          woods.push(new Wood(game, position));
        }
  
      });
    });
    return woods;
  }
  
  const level1 = [
    [0, 0 ,0, 0, 0, 0, 0, 1, 1, 0],
    [0, 0 ,0, 0, 0, 0, 0, 0, 0, 1],
    [0, 0 ,0, 0, 0, 0, 0, 0, 1, 0],
    [0, 0 ,0, 0, 0, 0, 0, 1, 0, 1]
  ];