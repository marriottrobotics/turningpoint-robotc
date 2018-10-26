#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_4,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_5,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_6,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_7,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_8,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           lbdrive,       tmotorVex393_HBridge, openLoop, driveLeft, encoderPort, I2C_1)
#pragma config(Motor,  port2,           lfdrive,       tmotorVex393_MC29, openLoop, driveLeft, encoderPort, I2C_2)
#pragma config(Motor,  port3,           grabber,       tmotorVex393_MC29, openLoop, encoderPort, I2C_3)
#pragma config(Motor,  port4,           catipult,      tmotorVex393_MC29, openLoop, encoderPort, I2C_4)
#pragma config(Motor,  port7,           rbdrive,       tmotorVex393_MC29, openLoop, driveRight, encoderPort, I2C_6)
#pragma config(Motor,  port8,           lift,          tmotorVex393_MC29, openLoop, encoderPort, I2C_7)
#pragma config(Motor,  port9,           forks,         tmotorVex393_MC29, openLoop, encoderPort, I2C_8)
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

#define FORKS_CLOSED 0
#define FORKS_FLAT -1200
#define FORKS_DROP -2007

#define LIFT_DOWN 0
#define LIFT_UP -2601

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

struct pid plift, pforks, pcatipult, pgrabber;
struct pid plfdrive, plbdrive, prfdrive, prbdrive;

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
  bStopTasksBetweenModes = true;

  plift.mport = lift;
  pforks.mport = forks;
  pcatipult.mport = catipult;
  pgrabber.mport = grabber;

  plfdrive.mport = lfdrive;
  plbdrive.mport = lbdrive;
  prfdrive.mport = rfdrive;
  prbdrive.mport = rbdrive;

  pid_arr[pid_count++] = &plift;
  pid_arr[pid_count++] = &pforks;
  pid_arr[pid_count++] = &pcatipult;
  pid_arr[pid_count++] = &pgrabber;

  pid_arr[pid_count++] = &plfdrive;
  pid_arr[pid_count++] = &plbdrive;
  pid_arr[pid_count++] = &prfdrive;
  pid_arr[pid_count++] = &prbdrive;

  pid_init_all();

  //Set min and max after pid init
  plift.min = -2601;
  plift.max = 0;
  plift.pgain = 1.3;
  plift.igain = 0.05;
  plift.lowpass = 32;

  pforks.min = -2007;
  pforks.max = 0;
  pforks.pgain = 0.6;

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
	initGameState(RED, TOP);
  // ..........................................................................
  // Insert user code here.
  // ..........................................................................

  // Remove this function call once you have "real" code.
  AutonomousCodePlaceholderForTesting();
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
  	writeDebugStreamLine("Lift at %d target %d", nMotorEncoder[plift.mport], plift.mtarget);
  	//Perim Drive
  	short lpower = vexRT[Ch3];
  	lpower*=-1;
  	lpower = lowpassS(lpower, 16);
  	short rpower = vexRT[Ch2];
  	rpower*=-1;
  	rpower = lowpassS(rpower, 16);
  	motor[lfdrive] = lpower;
  	motor[lbdrive] = lpower;
  	motor[rfdrive] = rpower;
  	motor[rbdrive] = rpower;

  	if(vexRT[Btn8U] == 1){
  		pforks.mtarget = FORKS_CLOSED;
  	}else if(vexRT[Btn8R] == 1){
  		pforks.mtarget = FORKS_FLAT;
  	}else if(vexRT[Btn8D] == 1){
  		pforks.mtarget = FORKS_DROP;
  	}

  	if(vexRT[Btn6U] == 1){
  		plift.mtarget-=10; //TODO Tune number
  	}else if(vexRT[Btn6D] == 1){
  		plift.mtarget+=10; //TODO Tune number
  	}else if(vexRT[Btn7U] == 1){
  		plift.mtarget = LIFT_UP;
  	}else if(vexRT[Btn7D] == 1){
  		plift.mtarget = LIFT_DOWN;
  	}

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

    wait1Msec(20);
  }
}
