/*
bot.cpp
Author: Benjamin Prud'homme
Course: CSCI-135
Assignment: Project 3

A program involving space robots collecting debris.  This file
contains the logic for the robot's actions, including collecting debris,
searching for and moving towards nearby debris, and repairing neighboring
robots that have failed.
*/

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

// Prototypes
void onStart(int num, int rows, int cols, double mpr, Area &area, ostream &log);
Action onRobotAction(int id, Loc loc, Area &area, ostream &log);
Action moveTowardsTarget(int robotID);
bool cellOpen(int row, int col);
int otherRobotTargetingDebris(int robotID, int debrisRow, int debrisCol);
void onRobotMalfunction(int id, Loc loc, Area &area, ostream &log);
void onClockTick(int time, ostream &log);

/* Initialization procedure, called when the game starts: */
void onStart(int num, int rows, int cols, double mpr, Area &area, ostream &log){
	NUM = num;   // save the number of robots and the map dimensions
	ROWS = rows;
	COLS = cols;

	// initialize global arrays
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

/* Deciding robot's next move */
Action onRobotAction(int id, Loc loc, Area &area, ostream &log) {
	int row = loc.r; // current row and column
	int col = loc.c;

	int targetRow = targetRows[id];
	int targetCol = targetCols[id];

	// initialize coordinates of each robot into the global coordinates arrays, so all robots know
	// where others are (necessary for each robot's first action)
	robotRows[id] = row;
	robotCols[id] = col;

	// if the robot is in a debris field, collect it and clear this robot's target variables, unless
	// this robot has been sent on a repair mission.  This reduces the chance that a robot assigned to
	// repair a certain robot will malfunction on the way to repairing, which could create chaos.
	if ((area.inspect(row, col) == DEBRIS) && !repairing[id]){

		// if this robot has been sent to repair, it will continue towards the malfunctioned
		// robot after it collects  This allows it to collect debris on its way to repairing.
		if(!repairing[id] && !malfunctioned[id]){
			targetRows[id] = -1;
			targetCols[id] = -1;
		}

		return COLLECT;

	}

	// otherwise...
	else {

		// if this robot has been assigned to repair and is adjacent to a malfunctioned robot,
		// repair it and assign the repaired robot to collect the debris in its cell
		if(repairing[id] == true){
			for(int i=0; i<NUM; i++){
				if(malfunctioned[i]){
					if((row+1==robotRows[i]) && (col==robotCols[i])){
						repairing[id] = false;
						malfunctioned[i] = false;

						targetRows[id] = -1;
						targetCols[id] = -1;

						targetRows[i] = robotRows[i];
						targetCols[i] = robotCols[i];
						return REPAIR_DOWN;
					}
					else if((row-1==robotRows[i]) && (col==robotCols[i])){
						repairing[id] = false;
						malfunctioned[i] = false;

						targetRows[id] = -1;
						targetCols[id] = -1;

						targetRows[i] = robotRows[i];
						targetCols[i] = robotCols[i];
						return REPAIR_UP;
					}
					else if((row==robotRows[i]) && (col+1==robotCols[i])){
						repairing[id] = false;
						malfunctioned[i] = false;

						targetRows[id] = -1;
						targetCols[id] = -1;

						targetRows[i] = robotRows[i];
						targetCols[i] = robotCols[i];
						return REPAIR_RIGHT;
					}
					else if((row==robotRows[i]) && (col-1==robotCols[i])){
						repairing[id] = false;
						malfunctioned[i] = false;

						targetRows[id] = -1;
						targetCols[id] = -1;

						targetRows[i] = robotRows[i];
						targetCols[i] = robotCols[i];
						return REPAIR_LEFT;
					}
				}
			}
		}


		// if this robot just collected or repaired, have this robot target a new debris field
		if(targetRow == -1){

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

						// find the index of the robot (if any) that is also targeting this debris
						int otherRobotID = otherRobotTargetingDebris(id, currentRow, col+(movesAway-rowsFromBot));

						// if there is none, this robot will target the debris
						if(otherRobotID==-1){

							targetRows[id] = currentRow;
							targetCols[id] = col+(movesAway-rowsFromBot);

							return moveTowardsTarget(id);
						}

						// otherwise, determine if this robot is closer than the other
						else {
							// if this other robot is further away from the debris than this robot, this robot will take over the other's duty.
							// if the other robot is equidistant from or closer to the debris, this robot will ignore it
							if(abs(robotRows[otherRobotID]-currentRow)+abs(robotCols[otherRobotID]-(col+(movesAway-rowsFromBot)))>movesAway){
								
								targetRows[id] = currentRow;
								targetCols[id] = col+(movesAway-rowsFromBot);

								targetRows[otherRobotID] = -1;
								targetCols[otherRobotID] = -1;

								return moveTowardsTarget(id);

							}

						}

					}

					else if((area.inspect(currentRow, col-(movesAway-rowsFromBot)) == DEBRIS)){

						// find the index of the robot (if any) that is also targeting this debris
						int otherRobotID = otherRobotTargetingDebris(id, currentRow, col-(movesAway-rowsFromBot));

						// if there is none, this robot will target the debris
						if(otherRobotID==-1){
							targetRows[id] = currentRow;
							targetCols[id] = col-(movesAway-rowsFromBot);

							return moveTowardsTarget(id);
						}

						// otherwise, determine if this robot is closer than the other
						else {
							// if this other robot is further away from the debris than this robot, this robot will take over the other's duty.
							// if the other robot is equidistant from or closer to the debris, this robot will ignore the debris
							if(abs(robotRows[otherRobotID]-currentRow)+abs(robotCols[otherRobotID]-(col-(movesAway-rowsFromBot)))>movesAway){
								
								targetRows[id] = currentRow;
								targetCols[id] = col-(movesAway-rowsFromBot);

								targetRows[otherRobotID] = -1;
								targetCols[otherRobotID] = -1;

								return moveTowardsTarget(id);

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

		// if the robot already has a target, move towards it
		return moveTowardsTarget(id);
		
	}
}

/* Moves the robot with given id towards its target */
Action moveTowardsTarget(int robotID){
	// robot's current row and column
	int row = robotRows[robotID];
	int col = robotCols[robotID];

	// robot's target row and column
	int targetRow = targetRows[robotID];
	int targetCol = targetCols[robotID];

	// distance in rows and columns from the target
	int rowsAway = abs(row-targetRow);
	int colsAway = abs(col-targetCol);
	int distanceAway = rowsAway+colsAway;

	// determine if it's possible to move in each of the four directions
	bool canMoveLeft = cellOpen(row, col-1);
	bool canMoveRight = cellOpen(row, col+1);
	bool canMoveUp = cellOpen(row-1, col);
	bool canMoveDown = cellOpen(row+1, col);

	// determine the robot's distance from the target after moving in each direction
	int leftDistance = abs(targetRow-row) + abs(targetCol-(col-1));
	int rightDistance = abs(targetRow-row) + abs(targetCol-(col+1));
	int upDistance = abs(targetRow-(row-1)) + abs(targetCol-col);
	int downDistance = abs(targetRow-(row+1)) + abs(targetCol-col);

	// array to store distances from the target after moving in each direction (indexes represent different directions)
	int distanceAfterMoving[4];

	// if the robot is further away from the target vertically than horizontally, it will move vertically if possible.
	// Initialize distanceAfterMoving so that the switch statement below will check the vertical directions first.
	if(rowsAway>colsAway){
		distanceAfterMoving[0] = upDistance;
		distanceAfterMoving[1] = downDistance;
		distanceAfterMoving[2] = leftDistance;
		distanceAfterMoving[3] = rightDistance;
	}
	// otherwise, it will move horizontally if possible.
	// Initialize distanceAfterMoving so that the switch statement below will check the horizontal directions first.
	else {
		distanceAfterMoving[0] = leftDistance;
		distanceAfterMoving[1] = rightDistance;
		distanceAfterMoving[2] = upDistance;
		distanceAfterMoving[3] = downDistance;
	}

	// Attempt to move in a direction that would get the robot closer to its target
	for(int i=0; i<4; i++){
		// if the robot is further away from the target vertically than horizontally, it will move vertically if possible
		if(rowsAway>colsAway){
			if(distanceAfterMoving[i]<distanceAway){
				switch(i){
					case 0:
						if(canMoveUp){
							robotRows[robotID]--;	
							return UP;
						}
						break;
					case 1:
						if(canMoveDown){
							robotRows[robotID]++;	
							return DOWN;
						}
						break;
					case 2:
						if(canMoveLeft){
							robotCols[robotID]--;	
							return LEFT;
						}
						break;
					default:
						if(canMoveRight){
							robotCols[robotID]++;	
							return RIGHT;
						}
						break;
				}
			}
		}
		// otherwise, it will move horizontally if possible
		else {
			if(distanceAfterMoving[i]<distanceAway){
				switch(i){
					case 0:
						if(canMoveLeft){
							robotCols[robotID]--;	
							return LEFT;
						}
						break;
					case 1:
						if(canMoveRight){
							robotCols[robotID]++;	
							return RIGHT;
						}
						break;
					case 2:
						if(canMoveUp){
							robotRows[robotID]--;	
							return UP;
						}
						break;
					default:
						if(canMoveDown){
							robotRows[robotID]++;	
							return DOWN;
						}
						break;
				}
			}
		}
	}

	// If we cannot get closer to the target, move randomly in one of the other horizontal directions if we are in the
	// same row as the target, or in one of the other vertical directions otherwise.  Also have the robot target a new cell
	// afterwards.

	srand(time(0));

	if(row==targetRows[robotID]){
		targetRows[robotID] = -1;
		targetCols[robotID] = -1;
		switch(rand() % 2){
			case 0:
				if(canMoveUp){
					robotRows[robotID]--;	
					return UP;
				}
				else {
					robotRows[robotID]++;	
					return DOWN;
				}
			default:
				if(canMoveDown){
					robotRows[robotID]++;	
					return DOWN;
				}
				else {
					robotRows[robotID]--;	
					return UP;
				}

		}
	}
	else {
		targetRows[robotID] = -1;
		targetCols[robotID] = -1;
		switch(rand() % 2){
			case 0:
				if(canMoveLeft){
					robotCols[robotID]--;	
					return LEFT;
				}
				else {
					robotCols[robotID]++;	
					return RIGHT;
				}
			default:
				if(canMoveRight){
					robotCols[robotID]++;	
					return RIGHT;
				}
				else {
					robotCols[robotID]--;	
					return LEFT;
				}

		}
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

/* Returns the ID of the robot targeting a debris field at the given coordinates (other than the one with the given ID). */
int otherRobotTargetingDebris(int robotID, int debrisRow, int debrisCol){
	for(int i=0; i<NUM; i++){
		if(i!=robotID && ((targetRows[i] == debrisRow) && (targetCols[i] == debrisCol))){
			return i;
		}
	}

	return -1;
}


void onRobotMalfunction(int id, Loc loc, Area &area, ostream &log) {

	// if a robot malfunctions, send the closest functional robot to repair it
	log << "Bot " << id << " is damaged." << endl;

	// let all other robots know this robot has malfunctioned
	malfunctioned[id] = true;

	// ID of the robot closest to the malfunctioned robot
	int closestRobotID=-1;
	int closestRobotDistance = INT_MAX;

	// find the closest functional robot
	for(int i=0; i<NUM; i++){
		if(!malfunctioned[i] && (abs(robotRows[i]-loc.r)+abs(robotCols[i]-loc.c)<closestRobotDistance)){
			closestRobotID = i;
			closestRobotDistance = abs(robotRows[i]-loc.r)+abs(robotCols[i]-loc.c);
		}
	}

	log << "Bot " << closestRobotID << " has been assigned to repair it.\n";

	targetRows[closestRobotID] = loc.r;
	targetCols[closestRobotID] = loc.c;
	repairing[closestRobotID] = true;

}

void onClockTick(int time, ostream &log) {
	if (time % 100 == 0) log << time << " ";
}


