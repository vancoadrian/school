function detectCollision(car, gameObject){

    let bottomOfCar = car.position.y + car.height;
    let topOfCar = car.position.y;
  
    let topOfObject = gameObject.position.y;
    let leftSideOfObject = gameObject.position.x;
    let rightSideOfObject = gameObject.position.x + gameObject.width;
    let bottomOfObject = gameObject.position.y + gameObject.height;
  
    if (
      bottomOfCar >= topOfObject &&
      topOfCar <= bottomOfObject &&
      car.position.x >= leftSideOfObject &&
      car.position.x + car.height <= rightSideOfObject
    ) {
      return true;
    } else {
      return false;
    }
  }