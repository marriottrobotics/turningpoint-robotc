#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, dgtl1,  inRed,          sensorDigitalIn)
#pragma config(Sensor, dgtl2,  inTop,          sensorDigitalIn)
#pragma config(Sensor, dgtl11, redInd,         sensorLEDtoVCC)
#pragma config(Sensor, dgtl12, topInd,         sensorLEDtoVCC)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_4,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_5,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_6,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_7,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_8,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           lbdrive,       tmotorVex393_HBridge, openLoop, driveLeft, encoderPort, I2C_1)
#pragma config(Motor,  port2,           lfdrive,       tmotorVex393_MC29, openLoop, reversed, driveLeft, encoderPort, I2C_2)
#pragma config(Motor,  port3,           grabber,       tmotorVex393_MC29, openLoop, encoderPort, I2C_3)
#pragma config(Motor,  port4,           catipult,      tmotorVex393_MC29, openLoop, encoderPort, I2C_4)
#pragma config(Motor,  port6,           elbow,         tmotorVex393_MC29, openLoop, encoderPort, I2C_8)
#pragma config(Motor,  port7,           tower,         tmotorVex393_MC29, openLoop, encoderPort, I2C_7)
#pragma config(Motor,  port8,           rbdrive,       tmotorVex393_MC29, openLoop, reversed, encoderPort, I2C_6)
#pragma config(Motor,  port10,          rfdrive,       tmotorVex393_HBridge, openLoop, driveRight, encoderPort, I2C_5)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*        Description: Competition template for VEX EDR                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/

// This code is for the VEX cortex platform
#pragma platform(VEX2)

// Select Download method as "competition"
#pragma competitionControl(Competition)

//Main competition background code...do not modify!
#include "Vex_Competition_Includes.c"
#include "PidCore.h"
#include "GameState.h"
#include "Movement.h"

#define LOADER_UP 0
#define LOADER_DOWN -174

//627.2 Ticks/Rotation at motor = 1881.6 Ticks/Rotation at loader.
//These two numbers should add to 1881.6 (decimals don't work so 1882).
#define CATIPULT_ARM 1800
#define CATIPULT_FIRE 82

#define CATIPULT_ARMED true
#define CATIPULT_FIRED false

bool catipultState = CATIPULT_FIRED;
bool buttonReleased = true;

bool loaderUp = true;
bool loaderButtonReleased = true;

struct pid pcatipult, pgrabber;
struct pid plfdrive, plbdrive, prfdrive, prbdrive;
struct pid ptower, pelbow;

void enableDrivePid();
void autonBottom();
void autonTop();

//Adjusts the LEDs based on what's happening.
//If the Red LED is on then you are on the RED team.
//If the Green LED is on then you are at the FLAG end (top);
task setLed(){
	while(true){
		if(SensorValue[inRed] == 1){
			SensorValue[redInd] = 1;
		}else{
			SensorValue[redInd] = 0;
		}
		if(SensorValue[inTop] == 1){
			SensorValue[topInd] = 1;
		}else{
			SensorValue[topInd] = 0;
		}
		wait1Msec(100);
	}
}

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the cortex has been powered on and    */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

void pre_auton()
{
  // Set bStopTasksBetweenModes to false if you want to keep user created tasks
  // running between Autonomous and Driver controlled modes. You will need to
  // manage all user created tasks if set to false.
	// If this is enabled our pid task shuts down. We need this to false.
  bStopTasksBetweenModes = false;

  pcatipult.mport = catipult;
  pgrabber.mport = grabber;

  plfdrive.mport = lfdrive;
  plbdrive.mport = lbdrive;
  prfdrive.mport = rfdrive;
  prbdrive.mport = rbdrive;

  ptower.mport = tower;
  pelbow.mport = elbow;

  pid_arr[pid_count++] = &pcatipult;
  pid_arr[pid_count++] = &pgrabber;

  pid_arr[pid_count++] = &plfdrive;
  pid_arr[pid_count++] = &plbdrive;
  pid_arr[pid_count++] = &prfdrive;
  pid_arr[pid_count++] = &prbdrive;

  pid_arr[pid_count++] = &ptower;
  pid_arr[pid_count++] = &pelbow;

  pid_init_all();

  pcatipult.pgain = 1.0;
  pcatipult.dgain = 0.5;

  pgrabber.max = 0;
  pgrabber.min = -174;
  pgrabber.pgain = 2.0;
  pgrabber.igain = 0.05;
  pgrabber.lowpass = 30;

  //Setup the api for moving things
  setupMovements(&plfdrive, &plbdrive, &prfdrive, &prbdrive, 392, 10.16, 36.35375);

  //Should almost always be the last thing that happens.
  startTask(pid_run_loops, 50);
  startTask(setLed, 50);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

task autonomous()
{
	if(plfdrive.enable == false){
		enableDrivePid();
	}
	bool end = false;
	bool alliance = false;
	if(SensorValue[inTop] == 1){
		end = TOP;
	}else{
		end = BOTTOM;
	}
	if(SensorValue[inRed] == 1){
		alliance = RED;
	}else{
		alliance = BLUE;
	}
	initGameState(alliance, end);

  if(isStartBottom()){
  	autonBottom();
	}else{
		autonTop();
	}
}

void enableDrivePid(){
	plfdrive.enable = true;
	plbdrive.enable = true;
	prfdrive.enable = true;
	prbdrive.enable = true;
}

void autonBottom(){
	writeDebugStreamLine("Bottom");
	pgrabber.mtarget = LOADER_DOWN;
	driveForwards(700+280, true);
	pgrabber.mtarget = LOADER_UP;
	wait_ontarget(&pgrabber);
	driveBackwards(300, true);

}

void autonTop(){
	writeDebugStreamLine("Top");
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/
task usercontrol()
{
  // User control code here, inside the loop

	//Setup Stuff
	//Disable drive Pid Control
	plfdrive.enable = false;
	plbdrive.enable = false;
	prfdrive.enable = false;
	prbdrive.enable = false;

  while (true)
  {
  	//Perim Drive
  	short lpower = vexRT[Ch3];
  	//lpower*=-1;
  	lpower = lowpassS(lpower, 16);
  	short rpower = vexRT[Ch2];
  	//rpower*=-1;
  	rpower = lowpassS(rpower, 16);
  	motor[lfdrive] = lpower;
  	motor[lbdrive] = lpower;
  	motor[rfdrive] = rpower;
  	motor[rbdrive] = rpower;

  	if(vexRT[Btn8L] == 1 && buttonReleased){
  		if(catipultState == CATIPULT_FIRED){
  			pcatipult.mtarget+=CATIPULT_ARM;
  			catipultState = CATIPULT_ARMED;
  		}else if(catipultState == CATIPULT_ARMED){
  			pcatipult.mtarget+=CATIPULT_FIRE;
  			catipultState = CATIPULT_FIRED;
  		}
  		buttonReleased = false;
  	}else if(vexRT[Btn8L] == 0){
  		buttonReleased = true;
  	}

  	if(vexRT[Btn7L] == 1 && loaderButtonReleased){
  		if(loaderUp){
  			pgrabber.mtarget = LOADER_DOWN;
 				loaderUp = false;
  		}else{
  			pgrabber.mtarget = LOADER_UP;
  			loaderUp = true;
  		}
  		loaderButtonReleased = false;
  	}else if(vexRT[Btn7L] == 0){
  		loaderButtonReleased = true;
  	}
  	if(vexRT[Btn5U] == 1){
  		pgrabber.mtarget += 5;
  	}else if(vexRT[Btn5D] == 1){
  		pgrabber.mtarget -= 5;
  	}
  	if(pgrabber.mtarget == LOADER_DOWN){
  		loaderUp = false;
  	}else if(pgrabber.mtarget == LOADER_UP){
  		loaderUp = true;
  	}

  	if(vexRT[Btn6U] == 1){
  		ptower.mtarget += 10;
  	}else if(vexRT[Btn6D] == 1){
  		ptower.mtarget -= 10;
  	}

  	if(vexRT[Btn8U] == 1){
  		pelbow.mtarget+=10;
  	}else if(vexRT[Btn8D] == 1){
  		pelbow.mtarget-=10;
  	}


    wait1Msec(20);
  }
}
