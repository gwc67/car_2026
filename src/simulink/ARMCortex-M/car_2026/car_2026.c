/*
 * File: car_2026.c
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

#include "car_2026.h"
#include <math.h>
#include "rt_nonfinite.h"
#include "rtwtypes.h"

/* Exported block parameters */
real32_T SPD_KI = 5.0F;                /* Variable: SPD_KI
                                        * Referenced by:
                                        *   '<S91>/Integral Gain'
                                        *   '<S144>/Integral Gain'
                                        */
real32_T SPD_KP = 56.0F;               /* Variable: SPD_KP
                                        * Referenced by:
                                        *   '<S99>/Proportional Gain'
                                        *   '<S152>/Proportional Gain'
                                        */
real32_T TURN_KD = 0.0F;               /* Variable: TURN_KD
                                        * Referenced by: '<S34>/Derivative Gain'
                                        */
real32_T TURN_KP = 1.0F;               /* Variable: TURN_KP
                                        * Referenced by: '<S46>/Proportional Gain'
                                        */
real32_T spd_filiter = 0.001F;         /* Variable: spd_filiter
                                        * Referenced by:
                                        *   '<S4>/Constant1'
                                        *   '<S5>/Constant1'
                                        */

/* Block signals (default storage) */
B_car_2026_T car_2026_B;

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
  int32_T tmp_0;
  real32_T DiscreteTimeIntegrator;
  real32_T rtb_Sum_k;
  boolean_T rtb_NOT_c;
  boolean_T tmp;

  /* Update the flag to indicate when data transfers from
   *  Sample time: [0.005s, 0.0s] to Sample time: [0.01s, 0.0s]  */
  (car_2026_M->Timing.RateInteraction.TID1_2)++;
  if ((car_2026_M->Timing.RateInteraction.TID1_2) > 1) {
    car_2026_M->Timing.RateInteraction.TID1_2 = 0;
  }

  /* Sum: '<S4>/Add1' incorporates:
   *  Constant: '<S4>/Constant'
   *  Constant: '<S4>/Constant1'
   *  Inport: '<Root>/spd_a'
   *  Product: '<S4>/Product1'
   *  Product: '<S4>/Product2'
   *  Sum: '<S4>/Add'
   *  UnitDelay: '<S4>/Unit Delay'
   */
  car_2026_DW.UnitDelay_DSTATE = (1.0F - spd_filiter) *
    car_2026_DW.UnitDelay_DSTATE + car_2026_U.spd_a * spd_filiter;

  /* RateTransition generated from: '<Root>/Subsystem' */
  tmp = (car_2026_M->Timing.RateInteraction.TID1_2 == 1);
  if (tmp) {
    /* RateTransition generated from: '<Root>/Subsystem' */
    car_2026_B.TmpRTBAtSubsystemOutport1 =
      car_2026_DW.TmpRTBAtSubsystemOutport1_Buffe;
  }

  /* End of RateTransition generated from: '<Root>/Subsystem' */

  /* Outputs for Atomic SubSystem: '<Root>/spd_a_pid' */
  /* Logic: '<S2>/NOT' incorporates:
   *  DataStoreRead: '<S2>/Data Store Read'
   */
  rtb_NOT_c = (car_2026_DW.running_flag == 0);

  /* Sum: '<S2>/Sum' incorporates:
   *  Inport: '<Root>/tar_spd_a'
   *  Sum: '<Root>/Add'
   *  UnitDelay: '<S4>/Unit Delay'
   */
  rtb_Sum_k = (car_2026_B.TmpRTBAtSubsystemOutport1 + car_2026_U.tar_spd_a) -
    car_2026_DW.UnitDelay_DSTATE;

  /* DiscreteIntegrator: '<S94>/Integrator' */
  if (rtb_NOT_c || (car_2026_DW.Integrator_PrevResetState_e != 0)) {
    car_2026_DW.Integrator_DSTATE_o = 0.0F;
  }

  /* Switch: '<S2>/Switch' incorporates:
   *  DataStoreRead: '<S2>/Data Store Read'
   */
  if (car_2026_DW.running_flag > 0) {
    /* Sum: '<S103>/Sum' incorporates:
     *  DiscreteIntegrator: '<S94>/Integrator'
     *  Gain: '<S99>/Proportional Gain'
     */
    DiscreteTimeIntegrator = SPD_KP * rtb_Sum_k +
      car_2026_DW.Integrator_DSTATE_o;

    /* Saturate: '<S101>/Saturation' */
    if (DiscreteTimeIntegrator > ((int16_T)PWM_MAX)) {
      DiscreteTimeIntegrator = ((int16_T)PWM_MAX);
    } else if (DiscreteTimeIntegrator < ((int16_T)PWM_MIN)) {
      DiscreteTimeIntegrator = ((int16_T)PWM_MIN);
    }

    DiscreteTimeIntegrator = floorf(DiscreteTimeIntegrator);
    if (rtIsNaNF(DiscreteTimeIntegrator)) {
      tmp_0 = 0;
    } else {
      tmp_0 = (int32_T)fmodf(DiscreteTimeIntegrator, 65536.0F);
    }

    /* Outport: '<Root>/pwm_a' incorporates:
     *  Saturate: '<S101>/Saturation'
     */
    car_2026_Y.pwm_a = (int16_T)(tmp_0 < 0 ? (int32_T)(int16_T)-(int16_T)
      (uint16_T)-(real32_T)tmp_0 : tmp_0);
  } else {
    /* Outport: '<Root>/pwm_a' incorporates:
     *  Constant: '<S2>/Constant'
     */
    car_2026_Y.pwm_a = 0;
  }

  /* End of Switch: '<S2>/Switch' */

  /* Update for DiscreteIntegrator: '<S94>/Integrator' incorporates:
   *  Gain: '<S91>/Integral Gain'
   */
  car_2026_DW.Integrator_DSTATE_o += SPD_KI * rtb_Sum_k;
  if (car_2026_DW.Integrator_DSTATE_o > SPD_INTEGRAL_MAX) {
    car_2026_DW.Integrator_DSTATE_o = SPD_INTEGRAL_MAX;
  } else if (car_2026_DW.Integrator_DSTATE_o < SPD_INTEGRAL_MIN) {
    car_2026_DW.Integrator_DSTATE_o = SPD_INTEGRAL_MIN;
  }

  car_2026_DW.Integrator_PrevResetState_e = (int8_T)rtb_NOT_c;

  /* End of Outputs for SubSystem: '<Root>/spd_a_pid' */

  /* Sum: '<S5>/Add1' incorporates:
   *  Constant: '<S5>/Constant'
   *  Constant: '<S5>/Constant1'
   *  Inport: '<Root>/spd_b'
   *  Product: '<S5>/Product1'
   *  Product: '<S5>/Product2'
   *  Sum: '<S5>/Add'
   *  UnitDelay: '<S5>/Unit Delay'
   */
  car_2026_DW.UnitDelay_DSTATE_b = (1.0F - spd_filiter) *
    car_2026_DW.UnitDelay_DSTATE_b + car_2026_U.spd_b * spd_filiter;

  /* Outputs for Atomic SubSystem: '<Root>/spd_b_pid' */
  /* Sum: '<S3>/Sum' incorporates:
   *  Inport: '<Root>/tar_spd_b'
   *  Sum: '<Root>/Add1'
   *  UnitDelay: '<S5>/Unit Delay'
   */
  rtb_Sum_k = (car_2026_U.tar_spd_b + car_2026_B.TmpRTBAtSubsystemOutport1) -
    car_2026_DW.UnitDelay_DSTATE_b;

  /* DiscreteIntegrator: '<S147>/Integrator' incorporates:
   *  DataStoreRead: '<S3>/Data Store Read'
   *  Logic: '<S3>/NOT'
   */
  if ((car_2026_DW.running_flag == 0) || (car_2026_DW.Integrator_PrevResetState
       != 0)) {
    car_2026_DW.Integrator_DSTATE = 0.0F;
  }

  /* Switch: '<S3>/Switch' incorporates:
   *  DataStoreRead: '<S3>/Data Store Read'
   */
  if (car_2026_DW.running_flag > 0) {
    /* Sum: '<S156>/Sum' incorporates:
     *  DiscreteIntegrator: '<S147>/Integrator'
     *  Gain: '<S152>/Proportional Gain'
     */
    DiscreteTimeIntegrator = SPD_KP * rtb_Sum_k + car_2026_DW.Integrator_DSTATE;

    /* Saturate: '<S154>/Saturation' */
    if (DiscreteTimeIntegrator > ((int16_T)PWM_MAX)) {
      DiscreteTimeIntegrator = ((int16_T)PWM_MAX);
    } else if (DiscreteTimeIntegrator < ((int16_T)PWM_MIN)) {
      DiscreteTimeIntegrator = ((int16_T)PWM_MIN);
    }

    DiscreteTimeIntegrator = floorf(DiscreteTimeIntegrator);
    if (rtIsNaNF(DiscreteTimeIntegrator)) {
      tmp_0 = 0;
    } else {
      tmp_0 = (int32_T)fmodf(DiscreteTimeIntegrator, 65536.0F);
    }

    /* Outport: '<Root>/pwm_b' incorporates:
     *  Saturate: '<S154>/Saturation'
     */
    car_2026_Y.pwm_b = (int16_T)(tmp_0 < 0 ? (int32_T)(int16_T)-(int16_T)
      (uint16_T)-(real32_T)tmp_0 : tmp_0);
  } else {
    /* Outport: '<Root>/pwm_b' incorporates:
     *  Constant: '<S3>/Constant'
     */
    car_2026_Y.pwm_b = 0;
  }

  /* End of Switch: '<S3>/Switch' */

  /* Update for DiscreteIntegrator: '<S147>/Integrator' incorporates:
   *  DiscreteIntegrator: '<S94>/Integrator'
   *  Gain: '<S144>/Integral Gain'
   */
  car_2026_DW.Integrator_DSTATE += SPD_KI * rtb_Sum_k;
  if (car_2026_DW.Integrator_DSTATE > SPD_INTEGRAL_MAX) {
    car_2026_DW.Integrator_DSTATE = SPD_INTEGRAL_MAX;
  } else if (car_2026_DW.Integrator_DSTATE < SPD_INTEGRAL_MIN) {
    car_2026_DW.Integrator_DSTATE = SPD_INTEGRAL_MIN;
  }

  /* Outputs for Atomic SubSystem: '<Root>/spd_a_pid' */
  car_2026_DW.Integrator_PrevResetState = (int8_T)rtb_NOT_c;

  /* End of Update for DiscreteIntegrator: '<S147>/Integrator' */
  /* End of Outputs for SubSystem: '<Root>/spd_a_pid' */
  /* End of Outputs for SubSystem: '<Root>/spd_b_pid' */

  /* Outport: '<Root>/spd_fliter_a ' incorporates:
   *  UnitDelay: '<S4>/Unit Delay'
   */
  car_2026_Y.spd_fliter_a = car_2026_DW.UnitDelay_DSTATE;

  /* Outport: '<Root>/spd_fliter_b ' incorporates:
   *  UnitDelay: '<S5>/Unit Delay'
   */
  car_2026_Y.spd_fliter_b = car_2026_DW.UnitDelay_DSTATE_b;

  /* DiscreteIntegrator: '<Root>/Discrete-Time Integrator' incorporates:
   *  Inport: '<Root>/gyroz'
   */
  rtb_Sum_k = 0.0025F * car_2026_U.gyroz;

  /* DiscreteIntegrator: '<Root>/Discrete-Time Integrator' */
  DiscreteTimeIntegrator = rtb_Sum_k + car_2026_DW.DiscreteTimeIntegrator_DSTATE;

  /* Gain: '<Root>/Gain' */
  car_2026_Y.yaw_out = DEGREE * DiscreteTimeIntegrator;

  /* RateTransition generated from: '<Root>/Subsystem' */
  if (tmp) {
    car_2026_DW.TmpRTBAtSubsystemInport1_Buffer = car_2026_Y.yaw_out;
  }

  /* Update for DiscreteIntegrator: '<Root>/Discrete-Time Integrator' */
  car_2026_DW.DiscreteTimeIntegrator_DSTATE = rtb_Sum_k + DiscreteTimeIntegrator;
}

/* Model step function for TID2 */
void car_2026_step2(void)              /* Sample time: [0.01s, 0.0s] */
{
  real32_T rtb_FilterCoefficient;
  real32_T rtb_Sum_g;

  /* Outputs for Atomic SubSystem: '<Root>/Subsystem' */
  /* Sum: '<S1>/Sum' incorporates:
   *  Inport: '<Root>/target_yaw'
   *  RateTransition generated from: '<Root>/Subsystem'
   */
  rtb_Sum_g = car_2026_U.target_yaw -
    car_2026_DW.TmpRTBAtSubsystemInport1_Buffer;

  /* Gain: '<S44>/Filter Coefficient' incorporates:
   *  DiscreteIntegrator: '<S36>/Filter'
   *  Gain: '<S34>/Derivative Gain'
   *  Sum: '<S36>/SumD'
   */
  rtb_FilterCoefficient = (TURN_KD * rtb_Sum_g - car_2026_DW.Filter_DSTATE) *
    100.0F;

  /* Sum: '<S50>/Sum' incorporates:
   *  Gain: '<S46>/Proportional Gain'
   */
  rtb_Sum_g = TURN_KP * rtb_Sum_g + rtb_FilterCoefficient;

  /* Saturate: '<S48>/Saturation' */
  if (rtb_Sum_g > TURN_SPD_MAX) {
    rtb_Sum_g = TURN_SPD_MAX;
  } else if (rtb_Sum_g < TURN_SPD_MIN) {
    rtb_Sum_g = TURN_SPD_MIN;
  }

  /* End of Saturate: '<S48>/Saturation' */

  /* Update for DiscreteIntegrator: '<S36>/Filter' */
  car_2026_DW.Filter_DSTATE += 0.01F * rtb_FilterCoefficient;

  /* End of Outputs for SubSystem: '<Root>/Subsystem' */

  /* RateTransition generated from: '<Root>/Subsystem' */
  car_2026_DW.TmpRTBAtSubsystemOutport1_Buffe = rtb_Sum_g;
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
