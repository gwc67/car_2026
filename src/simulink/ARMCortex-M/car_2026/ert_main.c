/*
 * File: ert_main.c
 *
 * Code generated for Simulink model 'car_2026'.
 *
 * Model version                  : 1.2
 * Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
 * C/C++ source code generated on : Wed Sep  9 21:41:58 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include <stddef.h>
#include <stdio.h>            /* This example main program uses printf/fflush */
#include "car_2026.h"                  /* Model header file */

/*
 * Associating rt_OneStep with a real-time clock or interrupt service routine
 * is what makes the generated code "real-time".  The function rt_OneStep is
 * always associated with the base rate of the model.  Subrates are managed
 * by the base rate from inside the generated code.  Enabling/disabling
 * interrupts and floating point context switches are target specific.  This
 * example code indicates where these should take place relative to executing
 * the generated code step function.  Overrun behavior should be tailored to
 * your application needs.  This example simply sets an error status in the
 * real-time model and returns from rt_OneStep.
 */
void rt_OneStep(void);
void rt_OneStep(void)
{
  static boolean_T OverrunFlags[3] = { 0, 0, 0 };

  static boolean_T eventFlags[3] = { 0, 0, 0 };/* Model has 3 rates */

  static int_T taskCounter[3] = { 0, 0, 0 };

  int_T i;

  /* Disable interrupts here */

  /* Check base rate for overrun */
  if (OverrunFlags[0]) {
    rtmSetErrorStatus(car_2026_M, "Overrun");
    return;
  }

  OverrunFlags[0] = true;

  /* Save FPU context here (if necessary) */
  /* Re-enable timer or interrupt here */

  /*
   * For a bare-board target (i.e., no operating system), the
   * following code checks whether any subrate overruns,
   * and also sets the rates that need to run this time step.
   */
  for (i = 1; i < 3; i++) {
    if (taskCounter[i] == 0) {
      if (eventFlags[i]) {
        OverrunFlags[0] = false;
        OverrunFlags[i] = true;

        /* Sampling too fast */
        rtmSetErrorStatus(car_2026_M, "Overrun");
        return;
      }

      eventFlags[i] = true;
    }
  }

  taskCounter[1]++;
  if (taskCounter[1] == 5) {
    taskCounter[1]= 0;
  }

  taskCounter[2]++;
  if (taskCounter[2] == 10) {
    taskCounter[2]= 0;
  }

  /* Set model inputs associated with base rate here */

  /* Step the model for base rate */
  car_2026_step0();

  /* Get model outputs here */

  /* Indicate task for base rate complete */
  OverrunFlags[0] = false;

  /* Step the model for any subrate */
  for (i = 1; i < 3; i++) {
    /* If task "i" is running, do not run any lower priority task */
    if (OverrunFlags[i]) {
      return;
    }

    if (eventFlags[i]) {
      OverrunFlags[i] = true;

      /* Set model inputs associated with subrates here */

      /* Step the model for subrate "i" */
      switch (i) {
       case 1 :
        car_2026_step1();

        /* Get model outputs here */
        break;

       case 2 :
        car_2026_step2();

        /* Get model outputs here */
        break;

       default :
        break;
      }

      /* Indicate task complete for subrate "i" */
      OverrunFlags[i] = false;
      eventFlags[i] = false;
    }
  }

  /* Disable interrupts here */
  /* Restore FPU context here (if necessary) */
  /* Enable interrupts here */
}

/*
 * The example main function illustrates what is required by your
 * application code to initialize, execute, and terminate the generated code.
 * Attaching rt_OneStep to a real-time clock is target specific. This example
 * illustrates how you do this relative to initializing the model.
 */
int_T main(int_T argc, const char *argv[])
{
  /* Unused arguments */
  (void)(argc);
  (void)(argv);

  /* Initialize model */
  car_2026_initialize();

  /* Attach rt_OneStep to a timer or interrupt service routine with
   * period 0.001 seconds (base rate of the model) here.
   * The call syntax for rt_OneStep is
   *
   *  rt_OneStep();
   */
  printf("Warning: The simulation will run forever. "
         "Generated ERT main won't simulate model step behavior. "
         "To change this behavior select the 'MAT-file logging' option.\n");
  fflush((NULL));
  while (rtmGetErrorStatus(car_2026_M) == (NULL)) {
    /*  Perform application tasks here */
  }

  /* Terminate model */
  car_2026_terminate();
  return 0;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
