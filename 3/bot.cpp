#include <cstdlib>
#include <climits>
#include <cmath>
#include <iostream>
#include "bot.h"

using namespace std;

const int MAX_ROBOT_NUM = 50;

int NUM;          // to remember number or robots
int ROWS, COLS;   // map dimensions

// arrays to store coordinates of robots
int robotRows[50];
int robotCols[50];

// arrays to store coordinates of fields that certain robots are targeting (debris or malfunctioned robots)
int targetRows[50];
int targetCols[50];

// array to track which robots have malfunctioned
bool malfunctioned[50];
// array to track which robots are repairing other malfunctioned robots
bool repairing[50];

//The indices of the arrays correspond to the id numbers of the robots.

/* Initialization procedure, called when the game starts: */
void onStart(int num, int rows, int cols, double mpr,
             Area &area, ostream &log)
{
	NUM = num;   // save the number of robots and the map dimensions
	ROWS = rows;
	COLS = cols;

	for(int i=0; i<50; i++){
		robotRows[i] = -1;
		robotCols[i] = -1;
		targetRows[i] = -1;
		targetCols[i] = -1;
		malfunctioned[i] = false;
		repairing[i] = false;
	}

	log << "Start" << endl;
}

void indexInRange(int index, int arraySize, ostream &log){
	if(index<0 || index>arraySize){
		log << "Error: attempting to reference index " << index << " of an array of size " << arraySize << endl;
	}
}

bool cellOpen(int row, int col){
	if(((row<0) || (row>=ROWS)) || ((col<0) || (col>=COLS))){
		return false;
	}

	for(int i=0; i<NUM; i++){
		if((robotRows[i]==row) && (robotCols[i]==col)){
			return false;
		}
	}

	return true;
}

/* Have robot with given id move towards the passed-in cell */
Action moveTowardsCell(int robotID, int cellRow, int cellCol, ostream &log){
	indexInRange(robotID, NUM, log); // debugging
	int row = robotRows[robotID];
	int col = robotCols[robotID];

	// determine if it's possible to move in each of the four directions
	bool canMoveLeft = cellOpen(row, col-1);
	bool canMoveRight = cellOpen(row, col+1);
	bool canMoveUp = cellOpen(row-1, col);
	bool canMoveDown = cellOpen(row+1, col);

	// determine the robot's distance from the target after moving in each direction
	int leftDistance = abs(cellRow-row) + abs(cellCol-(col-1));
	int rightDistance = abs(cellRow-row) + abs(cellCol-(col+1));
	int upDistance = abs(cellRow-(row-1)) + abs(cellCol-col);
	int downDistance = abs(cellRow-(row+1)) + abs(cellCol-col);

	int distanceAfterMoving[4] = {leftDistance, rightDistance, upDistance, downDistance};

	// the closest this robot could be to the target after moving
	int minDistanceAfterMoving = INT_MAX;

	for(int i=0; i<4; i++){
		if(distanceAfterMoving[i]<minDistanceAfterMoving){
			minDistanceAfterMoving = distanceAfterMoving[i];
		}
	}

	// Attempt to move in a direction that would get the robot closer to its target
	for(int i=0; i<4; i++){
		if(distanceAfterMoving[i]==minDistanceAfterMoving){
			switch(i){
				case 0:
					if(canMoveLeft){
						log << "Bot " << robotID << ": Moving left\n";
						return LEFT;
					}
					log << "Bot " << robotID << ": Can't move left\n";
					break;
				case 1:
					if(canMoveRight){
						log << "Bot " << robotID << ": Moving right\n";
						return RIGHT;
					}
					log << "Bot " << robotID << ": Can't move right\n";
					break;
				case 2:
					if(canMoveUp){
						log << "Bot " << robotID << ": Moving up\n";
						return UP;
					}
					log << "Bot " << robotID << ": Can't move up\n";
					break;
				default:
					if(canMoveDown){
						log << "Bot " << robotID << ": Moving down\n";
						return DOWN;
					}
					log << "Bot " << robotID << ": Can't move down\n";
					break;
			}
		}
	}

	log << "Bot " << robotID << ": Can't get closer to target, moving randomly\n";

	// If we cannot get closer to the target, move randomly in one of the other directions
	if(canMoveLeft){
		return LEFT;
	}
	else if(canMoveRight){
		return RIGHT;
	}
	else if(canMoveUp){
		return UP;
	}
	else {
		return DOWN;
	}
	

}

/* Returns the ID of the robot targeting a debris field at the given coordinates (other than the one with the given ID). */
int otherRobotTargetingDebris(int robotID, int debrisRow, int debrisCol, ostream &log){
	for(int i=0; i<NUM; i++){
		indexInRange(i, NUM, log); // debugging
		if(i!=robotID && ((targetRows[i] == debrisRow) && (targetCols[i] == debrisCol))){
			log << "Bot " << robotID << ": (" << debrisRow << ", " << debrisCol << ") is already being targeted by bot " << i << endl;
			return i;
		}
	}

	return -1;
}


/* Deciding robot's next move */
Action onRobotAction(int id, Loc loc, Area &area, ostream &log) {
	indexInRange(id, NUM, log);
	int row = loc.r; // current row and column
	int col = loc.c;

	// initialize coordinates of each robot into the global coordinates arrays, so all robots know
	// where others are (necessary for each robot's first action)
	robotRows[id] = row;
	robotCols[id] = col;

	// if the robot is in a debris field, collect it and clear this robot's target variables (unless
	// this robot has been sent on a repair mission)
	if (area.inspect(robotRows[id], robotCols[id]) == DEBRIS){

		// if this robot has been sent to repair, it will continue towards the malfunctioned
		// robot after it collects  This allows it to collect debris on its way to repairing.
		if(!repairing[id] && !malfunctioned[id]){
			targetRows[id] = -1;
			targetCols[id] = -1;
		}

		log << "Bot " << id << ": Collected debris\n";

		return COLLECT;

	}

	// otherwise...
	else {

		// if this robot has been assigned to repair and is adjacent to a malfunctioned robot,
		// repair it and assign the repaired robot to collect the debris in its cell
		if(repairing[id] == true){
			for(int i=0; i<NUM; i++){
				indexInRange(i, NUM, log); // debugging
				if(malfunctioned[i]){
					if((row+1==robotRows[i]) && (col==robotCols[i])){
						repairing[id] = false;
						malfunctioned[i] = false;

						targetRows[id] = -1;
						targetCols[id] = -1;

						targetRows[i] = robotRows[i];
						targetCols[i] = robotCols[i];

						log << "Bot " << i << " has been repaired.  It is now targeting debris at (" << targetRows[i] << ", " << targetCols[i] << ")\n";
						return REPAIR_DOWN;
					}
					else if((row-1==robotRows[i]) && (col==robotCols[i])){
						repairing[id] = false;
						malfunctioned[i] = false;

						targetRows[id] = -1;
						targetCols[id] = -1;

						targetRows[i] = robotRows[i];
						targetCols[i] = robotCols[i];

						log << "Bot " << i << " has been repaired.  It is now targeting debris at (" << targetRows[i] << ", " << targetCols[i] << ")\n";
						return REPAIR_UP;
					}
					else if((row==robotRows[i]) && (col+1==robotCols[i])){
						repairing[id] = false;
						malfunctioned[i] = false;

						targetRows[id] = -1;
						targetCols[id] = -1;

						targetRows[i] = robotRows[i];
						targetCols[i] = robotCols[i];

						log << "Bot " << i << " has been repaired.  It is now targeting debris at (" << targetRows[i] << ", " << targetCols[i] << ")\n";
						return REPAIR_RIGHT;
					}
					else if((row==robotRows[i]) && (col-1==robotCols[i])){
						repairing[id] = false;
						malfunctioned[i] = false;

						targetRows[id] = -1;
						targetCols[id] = -1;

						targetRows[i] = robotRows[i];
						targetCols[i] = robotCols[i];

						log << "Bot " << i << " has been repaired.  It is now targeting debris at (" << targetRows[i] << ", " << targetCols[i] << ")\n";
						return REPAIR_LEFT;
					}
				}
			}
		}


		// if this robot just collected or repaired, have this robot target a new debris field
		if(targetRows[id] == -1){

			log << "Bot " << id << ": Searching for debris\n";

			// when searching for nearby debris, the robot looks for debris this number of moves away
			int movesAway = 1;

			// look for closest debris that is NOT being targeted by another robot that is closer to it
			while(movesAway<ROWS+COLS){

				// start out looking for debris 1 move away
				for(int currentRow=row-movesAway; currentRow<=row+movesAway; currentRow++){
					// determine how many rows currentRow is away from the robot's row
					int rowsFromBot = abs(row-currentRow);

					// move enough columns left or right to make sure we are checking cells
					// the correct number of moves away.
					
					// if the robot finds debris, check to see if it is closer to other robots targeting the same debris
					if((area.inspect(currentRow, col+(movesAway-rowsFromBot)) == DEBRIS)){
						log << "Bot " << id << ": Found debris at (" << currentRow << ", " << col+(movesAway-rowsFromBot) << ")\n";

						int otherRobotID = otherRobotTargetingDebris(id, currentRow, col+(movesAway-rowsFromBot), log);

						if(otherRobotID==-1){
							log << "No other bots are targeting this debris\n";

							targetRows[id] = currentRow;
							targetCols[id] = col+(movesAway-rowsFromBot);

							log << "Bot " << id << ": Targeting (" << targetRows[id] << ", " << targetCols[id] << ")\n";
							return moveTowardsCell(id, targetRows[id], targetCols[id], log);
						}

						// otherwise, determine if this robot is closer than the other
						else {
							log << "which is already being targeted by bot " << otherRobotID << endl;

							// if this other robot is further away from the debris than this robot, this robot will take over the other's duty.
							// if the other robot is equidistant from or closer to the debris, this robot will ignore the debris
							indexInRange(otherRobotID, 50, log); // debugging
							if(abs(robotRows[otherRobotID]-currentRow)+abs(robotCols[otherRobotID]-(col+(movesAway-rowsFromBot)))>movesAway){
								log << "Bot " << otherRobotID << ": No longer targeting (" << targetRows[otherRobotID] << ", " << targetCols[otherRobotID] << ")\n";
								
								targetRows[id] = currentRow;
								targetCols[id] = col+(movesAway-rowsFromBot);

								targetRows[otherRobotID] = -1;
								targetCols[otherRobotID] = -1;

								log << "Bot " << id << ": Targeting (" << targetRows[id] << ", " << targetCols[id] << ")\n";
								return moveTowardsCell(id, targetRows[id], targetCols[id], log);

							}

						}

					}

					else if((area.inspect(currentRow, col-(movesAway-rowsFromBot)) == DEBRIS)){
						log << "Bot " << id << ": Found debris at (" << currentRow << ", " << col-(movesAway-rowsFromBot) << ")\n";

						// find the index of the robot (if any) that is also targeting this debris
						int otherRobotID = otherRobotTargetingDebris(id, currentRow, col-(movesAway-rowsFromBot), log);

						if(otherRobotID==-1){
							log << "No other bots are targeting this debris\n";

							targetRows[id] = currentRow;
							targetCols[id] = col-(movesAway-rowsFromBot);

							log << "Bot " << id << ": Targeting (" << targetRows[id] << ", " << targetCols[id] << ")\n";
							return moveTowardsCell(id, targetRows[id], targetCols[id], log);
						}

						// otherwise, determine if this robot is closer than the other
						else {
							log << "which is already being targeted by bot " << otherRobotID << endl;

							// if this other robot is further away from the debris than this robot, this robot will take over the other's duty.
							// if the other robot is equidistant from or closer to the debris, this robot will ignore the debris
							indexInRange(otherRobotID, 50, log); // debugging
							if(abs(robotRows[otherRobotID]-currentRow)+abs(robotCols[otherRobotID]-(col-(movesAway-rowsFromBot)))>movesAway){
								log << "Bot " << otherRobotID << ": No longer targeting (" << targetRows[otherRobotID] << ", " << targetCols[otherRobotID] << ")\n";
								
								targetRows[id] = currentRow;
								targetCols[id] = col-(movesAway-rowsFromBot);

								targetRows[otherRobotID] = -1;
								targetCols[otherRobotID] = -1;

								log << "Bot " << id << ": Targeting (" << targetRows[id] << ", " << targetCols[id] << ")\n";
								return moveTowardsCell(id, targetRows[id], targetCols[id], log);

							}

						}
					}			

				}

				// if no debris was found the current number of moves away, look for debris one additional move away
				movesAway++;

			}

			// if no debris was targeted, move randomly
			switch(rand() % 4) {
			case 0:
				return LEFT;
			case 1:
				return RIGHT;
			case 2:
				return UP;
			default:
				return DOWN;
			}

		}

		return moveTowardsCell(id, targetRows[id], targetCols[id], log);
		
	}
}

void onRobotMalfunction(int id, Loc loc, Area &area, ostream &log) {
	indexInRange(id, NUM, log);

	// if a robot malfunctions, send the closest functional robot to repair it
	log << "Bot " << id << " is damaged." << endl;

	// let all other robots know this robot has malfunctioned
	malfunctioned[id] = true;

	// ID of the robot closest to the malfunctioned robot
	int closestRobotID=-1;
	int closestRobotDistance = INT_MAX;

	// find the closest functional robot
	for(int i=0; i<NUM; i++){
		indexInRange(i, NUM, log); // debugging
		if(!malfunctioned[i] && (abs(robotRows[i]-loc.r)+abs(robotCols[i]-loc.c)<closestRobotDistance)){
			closestRobotID = i;
			closestRobotDistance = abs(robotRows[i]-loc.r)+abs(robotCols[i]-loc.c);
		}
	}

	log << "Bot " << closestRobotID << " has been assigned to repair it.\n";

	indexInRange(closestRobotID, NUM, log); // debugging

	targetRows[closestRobotID] = loc.r;
	targetCols[closestRobotID] = loc.c;
	repairing[closestRobotID] = true;

}

void onClockTick(int time, ostream &log) {
	if (time % 100 == 0) log << time << " ";
}


