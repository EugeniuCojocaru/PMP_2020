#include "Point.h"

Point::Point(){
	id = -1;
	x = -1;
	y = -1;	
}

Point::Point(short newID, short newX, short newY){
	id = newID;
	x = newX;
	y = newY;
}

/*void Projectile::Print(){
	
	Serial.print("Bullet: ");
	Serial.print(this->id);
	Serial.print(" at pos(x,y): (");
	Serial.print(this->x);
	Serial.print(',');
	Serial.print(this->y);
	Serial.print(")\n");
}*/