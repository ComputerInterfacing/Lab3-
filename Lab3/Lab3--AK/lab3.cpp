#include <predef.h>
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <smarttrap.h>
#include <taskmon.h>
#include <NetworkDebug.h>
#include "AD.h"
#include "DA.h"
#include "LCD.h"

extern "C" {
void UserMain(void * pd);
void StartTask1(void);
void Task1Main( void * pd);
void StartTask2(void);
void Task2Main( void * pd);
}

#define USER_MAIN_VOLTS 3.0
#define TASK1_VOLTS		2.0
#define TASK2_VOLTS		1.0

/* User task priorities always based on MAIN_PRIO */
/* The priorities between MAIN_PRIO and the IDLE_PRIO are available */
#define DUMMYTASK1_PRIO 	MAIN_PRIO + 1
#define DUMMYTASK2_PRIO 	MAIN_PRIO + 2

/* Task stacks for all the user tasks */
/* If you add a new task you'll need to add a new stack for that task */
DWORD DummyTask1Stk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD DummyTask2Stk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );

const char * AppName="Abuni AND Kathleen";

AD myAD;
DA myDA;
LCD myLCD;

void UserMain(void * pd) {
    InitializeStack();
    OSChangePrio(MAIN_PRIO);
    EnableAutoUpdate();
    StartHTTP();
    EnableTaskMonitor();

    #ifndef _DEBUG
    EnableSmartTraps();
    #endif

    #ifdef _DEBUG
    InitializeNetworkGDB_and_Wait();
    #endif

    iprintf("Application started\n");
    myLCD.Init();
    myLCD.Home();
    myLCD.Clear();
    myLCD.DrawString("Welcome");
    myAD.Init();
    myDA.Init();
    myDA.DACOutput(0);
    StartTask1();
    StartTask2();


    while (1) {
    	WORD result = 0;
        float volts = 0.0;
        myDA.Lock();
    	myDA.DACOutput(USER_MAIN_VOLTS);

    	myAD.StartAD(); //StartAD
    	while (!myAD.ADDone()){
    		OSTimeDly(TICKS_PER_SECOND/20);
    	}
       //Busy Wait until done
    		 result = myAD.GetADResult(0);//Get Result
    		 myAD.StopAD();//Stop the AD
    		 iprintf("RESULT %d\n", result);
    		 int my_result = result;
    		 int brightness[] = {50, 271,492, 713,934, 1155,1376,1597,1818,2039,2260,2481,2702};
    		 myLCD.Clear();
    		 for (int i = 0; i<12; i++){
    			 if (my_result <= brightness[i]) {
    				 myLCD.DrawBarGraph(2,i);
    				 break;
    			 }
    		 }







    	myDA.DACOutput(0);
    	myDA.Unlock();
        OSTimeDly(TICKS_PER_SECOND/20);
    }
}

/* Name: StartTask1
 * Description: Creates the task main loop.
 * Inputs: none
 * Outputs: none
 */
void StartTask1(void) {
	BYTE err = OS_NO_ERR;

	err = display_error( "StartTask1 fail:",
					OSTaskCreatewName(	Task1Main,
					(void *)NULL,
				 	(void *) &DummyTask1Stk[USER_TASK_STK_SIZE],
				 	(void *) &DummyTask1Stk[0],
				 	DUMMYTASK1_PRIO, "Dummy Task 1" ));
}

/* Name: Task1Main
 * Description: NOP loop that can be timed
 * 	Does nothing useful
 * Inputs:  void * pd -- pointer to generic data . Currently unused.
 * Outputs: none
 */
void	Task1Main( void * pd) {


	DWORD count = 0 ;


	/* place semaphore usage code inside the loop */
	while (1) {

		myDA.Lock();
		myDA.DACOutput(TASK1_VOLTS);
		for (int i = 0; i < 80000 ; i++) {
						count = count + 1;
				}
		myDA.DACOutput(0);
		myDA.Unlock();
		OSTimeDly(TICKS_PER_SECOND/20); // single tick delay is the smallest possible sleep time
	}
}

/* Name: StartTask1
 * Description: Creates the task main loop.
 * Inputs: none
 * Outputs: none
 */
void StartTask2(void) {
	BYTE err = OS_NO_ERR;

	err = display_error( "StartTask2 fail:",
					OSTaskCreatewName(	Task2Main,
					(void *)NULL,
				 	(void *) &DummyTask2Stk[USER_TASK_STK_SIZE],
				 	(void *) &DummyTask2Stk[0],
				 	DUMMYTASK2_PRIO, "Dummy Task 2" ));
}

/* Name: Task2Main
 * Description: NOP loop that can be timed
 * 	Does nothing useful
 * Inputs:  void * pd -- pointer to generic data . Currently unused.
 * Outputs: none
 */
void	Task2Main( void * pd) {

	DWORD count = 0;

	while (1) {

		myDA.Lock();
		myDA.DACOutput(TASK2_VOLTS);
		for (int i = 0; i < 40000 ; i++) {
					count = count + 1;
		}

		myDA.DACOutput(0);
		myDA.Unlock();
		OSTimeDly(TICKS_PER_SECOND/20);
	}
}
