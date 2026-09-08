/*
 * File: car_2026.c
 *
 * Code generated for Simulink model 'car_2026'.
 *
 * Model version                  : 1.0
 * Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
 * C/C++ source code generated on : Tue Sep  8 17:31:48 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "car_2026.h"
#include <math.h>
#include "rt_nonfinite.h"
#include "rtwtypes.h"

/* Exported block parameters */
real32_T SPD_KI = 0.0F;                /* Variable: SPD_KI
                                        * Referenced by:
                                        *   '<S35>/Integral Gain'
                                        *   '<S88>/Integral Gain'
                                        */
real32_T SPD_KP = 0.0F;                /* Variable: SPD_KP
                                        * Referenced by:
                                        *   '<S43>/Proportional Gain'
                                        *   '<S96>/Proportional Gain'
                                        */

/* Block states (default storage) */
DW_car_2026_T car_2026_DW;

/* External inputs (root inport signals with default storage) */
ExtU_car_2026_T car_2026_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_car_2026_T car_2026_Y;

/* Real-time model */
static RT_MODEL_car_2026_T car_2026_M_;
RT_MODEL_car_2026_T *const car_2026_M = &car_2026_M_;

/* Model step function for TID0 */
void car_2026_step0(void)              /* Sample time: [0.001s, 0.0s] */
{
  /* DataStoreWrite: '<Root>/Data Store Write' incorporates:
   *  Inport: '<Root>/running_flag'
   */
  car_2026_DW.running_flag = car_2026_U.running_flag;
}

/* Model step function for TID1 */
void car_2026_step1(void)              /* Sample time: [0.005s, 0.0s] */
{
  int32_T tmp;
  real32_T rtb_Sum_k;
  real32_T u0;
  boolean_T rtb_NOT_c;

  /* Outputs for Atomic SubSystem: '<Root>/spd_a_pid' */
  /* Logic: '<S1>/NOT' incorporates:
   *  DataStoreRead: '<S1>/Data Store Read'
   */
  rtb_NOT_c = (car_2026_DW.running_flag == 0);

  /* Sum: '<S1>/Sum' incorporates:
   *  Inport: '<Root>/motor_a_spd'
   *  Inport: '<Root>/tar_spd_a'
   */
  rtb_Sum_k = car_2026_U.tar_spd_a - car_2026_U.motor_a_spd;

  /* DiscreteIntegrator: '<S38>/Integrator' */
  if (rtb_NOT_c || (car_2026_DW.Integrator_PrevResetState_e != 0)) {
    car_2026_DW.Integrator_DSTATE_o = 0.0F;
  }

  /* Switch: '<S1>/Switch' incorporates:
   *  DataStoreRead: '<S1>/Data Store Read'
   */
  if (car_2026_DW.running_flag > 0) {
    /* Sum: '<S47>/Sum' incorporates:
     *  DiscreteIntegrator: '<S38>/Integrator'
     *  Gain: '<S43>/Proportional Gain'
     */
    u0 = SPD_KP * rtb_Sum_k + car_2026_DW.Integrator_DSTATE_o;

    /* Saturate: '<S45>/Saturation' */
    if (u0 > ((int16_T)PWM_MAX)) {
      u0 = ((int16_T)PWM_MAX);
    } else if (u0 < ((int16_T)PWM_MIN)) {
      u0 = ((int16_T)PWM_MIN);
    }

    u0 = floorf(u0);
    if (rtIsNaNF(u0)) {
      tmp = 0;
    } else {
      tmp = (int32_T)fmodf(u0, 65536.0F);
    }

    /* Outport: '<Root>/pwm_a' incorporates:
     *  Saturate: '<S45>/Saturation'
     */
    car_2026_Y.pwm_a = (int16_T)(tmp < 0 ? (int32_T)(int16_T)-(int16_T)(uint16_T)
      -(real32_T)tmp : tmp);
  } else {
    /* Outport: '<Root>/pwm_a' incorporates:
     *  Constant: '<S1>/Constant'
     */
    car_2026_Y.pwm_a = 0;
  }

  /* End of Switch: '<S1>/Switch' */

  /* Update for DiscreteIntegrator: '<S38>/Integrator' incorporates:
   *  Gain: '<S35>/Integral Gain'
   */
  car_2026_DW.Integrator_DSTATE_o += SPD_KI * rtb_Sum_k;
  if (car_2026_DW.Integrator_DSTATE_o > (real32_T)SPD_INTEGRAL_MAX) {
    car_2026_DW.Integrator_DSTATE_o = (real32_T)SPD_INTEGRAL_MAX;
  } else if (car_2026_DW.Integrator_DSTATE_o < (real32_T)SPD_INTEGRAL_MIN) {
    car_2026_DW.Integrator_DSTATE_o = (real32_T)SPD_INTEGRAL_MIN;
  }

  car_2026_DW.Integrator_PrevResetState_e = (int8_T)rtb_NOT_c;

  /* End of Outputs for SubSystem: '<Root>/spd_a_pid' */

  /* Outputs for Atomic SubSystem: '<Root>/spd_b_pid' */
  /* Sum: '<S2>/Sum' incorporates:
   *  Inport: '<Root>/spd_b'
   *  Inport: '<Root>/tar_spd_b'
   */
  rtb_Sum_k = car_2026_U.tar_spd_b - car_2026_U.spd_b;

  /* DiscreteIntegrator: '<S91>/Integrator' incorporates:
   *  DataStoreRead: '<S2>/Data Store Read'
   *  Logic: '<S2>/NOT'
   */
  if ((car_2026_DW.running_flag == 0) || (car_2026_DW.Integrator_PrevResetState
       != 0)) {
    car_2026_DW.Integrator_DSTATE = 0.0F;
  }

  /* Switch: '<S2>/Switch' incorporates:
   *  DataStoreRead: '<S2>/Data Store Read'
   */
  if (car_2026_DW.running_flag > 0) {
    /* Sum: '<S100>/Sum' incorporates:
     *  DiscreteIntegrator: '<S91>/Integrator'
     *  Gain: '<S96>/Proportional Gain'
     */
    u0 = SPD_KP * rtb_Sum_k + car_2026_DW.Integrator_DSTATE;

    /* Saturate: '<S98>/Saturation' */
    if (u0 > ((int16_T)PWM_MAX)) {
      u0 = ((int16_T)PWM_MAX);
    } else if (u0 < ((int16_T)PWM_MIN)) {
      u0 = ((int16_T)PWM_MIN);
    }

    u0 = floorf(u0);
    if (rtIsNaNF(u0)) {
      tmp = 0;
    } else {
      tmp = (int32_T)fmodf(u0, 65536.0F);
    }

    /* Outport: '<Root>/pwm_b' incorporates:
     *  Saturate: '<S98>/Saturation'
     */
    car_2026_Y.pwm_b = (int16_T)(tmp < 0 ? (int32_T)(int16_T)-(int16_T)(uint16_T)
      -(real32_T)tmp : tmp);
  } else {
    /* Outport: '<Root>/pwm_b' incorporates:
     *  Constant: '<S2>/Constant'
     */
    car_2026_Y.pwm_b = 0;
  }

  /* End of Switch: '<S2>/Switch' */

  /* Update for DiscreteIntegrator: '<S91>/Integrator' incorporates:
   *  DiscreteIntegrator: '<S38>/Integrator'
   *  Gain: '<S88>/Integral Gain'
   */
  car_2026_DW.Integrator_DSTATE += SPD_KI * rtb_Sum_k;
  if (car_2026_DW.Integrator_DSTATE > (real32_T)SPD_INTEGRAL_MAX) {
    car_2026_DW.Integrator_DSTATE = (real32_T)SPD_INTEGRAL_MAX;
  } else if (car_2026_DW.Integrator_DSTATE < (real32_T)SPD_INTEGRAL_MIN) {
    car_2026_DW.Integrator_DSTATE = (real32_T)SPD_INTEGRAL_MIN;
  }

  /* Outputs for Atomic SubSystem: '<Root>/spd_a_pid' */
  car_2026_DW.Integrator_PrevResetState = (int8_T)rtb_NOT_c;

  /* End of Update for DiscreteIntegrator: '<S91>/Integrator' */
  /* End of Outputs for SubSystem: '<Root>/spd_a_pid' */
  /* End of Outputs for SubSystem: '<Root>/spd_b_pid' */
}

/* Model initialize function */
void car_2026_initialize(void)
{
  /* (no initialization code required) */
}

/* Model terminate function */
void car_2026_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
