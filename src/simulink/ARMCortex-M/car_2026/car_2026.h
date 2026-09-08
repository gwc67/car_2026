/*
 * File: car_2026.h
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

#ifndef car_2026_h_
#define car_2026_h_
#ifndef car_2026_COMMON_INCLUDES_
#define car_2026_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rt_nonfinite.h"
#include "math.h"
#endif                                 /* car_2026_COMMON_INCLUDES_ */

#include "car_2026_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Exported data define */

/* Definition for custom storage class: Define */
#define PWM_MAX                        800                       /* Referenced by:
                                                                  * '<S45>/Saturation'
                                                                  * '<S98>/Saturation'
                                                                  */
#define PWM_MIN                        -800                      /* Referenced by:
                                                                  * '<S45>/Saturation'
                                                                  * '<S98>/Saturation'
                                                                  */
#define SPD_INTEGRAL_MAX               150.0                     /* Referenced by:
                                                                  * '<S38>/Integrator'
                                                                  * '<S91>/Integrator'
                                                                  */
#define SPD_INTEGRAL_MIN               -150.0                    /* Referenced by:
                                                                  * '<S38>/Integrator'
                                                                  * '<S91>/Integrator'
                                                                  */

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real32_T Integrator_DSTATE;          /* '<S91>/Integrator' */
  real32_T Integrator_DSTATE_o;        /* '<S38>/Integrator' */
  int8_T Integrator_PrevResetState;    /* '<S91>/Integrator' */
  int8_T Integrator_PrevResetState_e;  /* '<S38>/Integrator' */
  uint8_T running_flag;                /* '<Root>/Data Store Memory' */
} DW_car_2026_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real32_T motor_a_spd;                /* '<Root>/motor_a_spd' */
  uint8_T running_flag;                /* '<Root>/running_flag' */
  real32_T tar_spd_a;                  /* '<Root>/tar_spd_a' */
  real32_T spd_b;                      /* '<Root>/spd_b' */
  real32_T tar_spd_b;                  /* '<Root>/tar_spd_b' */
} ExtU_car_2026_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  int16_T pwm_a;                       /* '<Root>/pwm_a' */
  int16_T pwm_b;                       /* '<Root>/pwm_b' */
} ExtY_car_2026_T;

/* Real-time Model Data Structure */
struct tag_RTM_car_2026_T {
  const char_T * volatile errorStatus;
};

/* Block states (default storage) */
extern DW_car_2026_T car_2026_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_car_2026_T car_2026_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_car_2026_T car_2026_Y;

/*
 * Exported Global Parameters
 *
 * Note: Exported global parameters are tunable parameters with an exported
 * global storage class designation.  Code generation will declare the memory for
 * these parameters and exports their symbols.
 *
 */
extern real32_T SPD_KI;                /* Variable: SPD_KI
                                        * Referenced by:
                                        *   '<S35>/Integral Gain'
                                        *   '<S88>/Integral Gain'
                                        */
extern real32_T SPD_KP;                /* Variable: SPD_KP
                                        * Referenced by:
                                        *   '<S43>/Proportional Gain'
                                        *   '<S96>/Proportional Gain'
                                        */

/* Model entry point functions */
extern void car_2026_initialize(void);
extern void car_2026_step0(void);      /* Sample time: [0.001s, 0.0s] */
extern void car_2026_step1(void);      /* Sample time: [0.005s, 0.0s] */
extern void car_2026_terminate(void);

/* Real-time Model object */
extern RT_MODEL_car_2026_T *const car_2026_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'car_2026'
 * '<S1>'   : 'car_2026/spd_a_pid'
 * '<S2>'   : 'car_2026/spd_b_pid'
 * '<S3>'   : 'car_2026/spd_a_pid/PID Controller'
 * '<S4>'   : 'car_2026/spd_a_pid/PID Controller/Anti-windup'
 * '<S5>'   : 'car_2026/spd_a_pid/PID Controller/D Gain'
 * '<S6>'   : 'car_2026/spd_a_pid/PID Controller/External Derivative'
 * '<S7>'   : 'car_2026/spd_a_pid/PID Controller/Filter'
 * '<S8>'   : 'car_2026/spd_a_pid/PID Controller/Filter ICs'
 * '<S9>'   : 'car_2026/spd_a_pid/PID Controller/I Gain'
 * '<S10>'  : 'car_2026/spd_a_pid/PID Controller/Ideal P Gain'
 * '<S11>'  : 'car_2026/spd_a_pid/PID Controller/Ideal P Gain Fdbk'
 * '<S12>'  : 'car_2026/spd_a_pid/PID Controller/Integrator'
 * '<S13>'  : 'car_2026/spd_a_pid/PID Controller/Integrator ICs'
 * '<S14>'  : 'car_2026/spd_a_pid/PID Controller/N Copy'
 * '<S15>'  : 'car_2026/spd_a_pid/PID Controller/N Gain'
 * '<S16>'  : 'car_2026/spd_a_pid/PID Controller/P Copy'
 * '<S17>'  : 'car_2026/spd_a_pid/PID Controller/Parallel P Gain'
 * '<S18>'  : 'car_2026/spd_a_pid/PID Controller/Reset Signal'
 * '<S19>'  : 'car_2026/spd_a_pid/PID Controller/Saturation'
 * '<S20>'  : 'car_2026/spd_a_pid/PID Controller/Saturation Fdbk'
 * '<S21>'  : 'car_2026/spd_a_pid/PID Controller/Sum'
 * '<S22>'  : 'car_2026/spd_a_pid/PID Controller/Sum Fdbk'
 * '<S23>'  : 'car_2026/spd_a_pid/PID Controller/Tracking Mode'
 * '<S24>'  : 'car_2026/spd_a_pid/PID Controller/Tracking Mode Sum'
 * '<S25>'  : 'car_2026/spd_a_pid/PID Controller/Tsamp - Integral'
 * '<S26>'  : 'car_2026/spd_a_pid/PID Controller/Tsamp - Ngain'
 * '<S27>'  : 'car_2026/spd_a_pid/PID Controller/postSat Signal'
 * '<S28>'  : 'car_2026/spd_a_pid/PID Controller/preInt Signal'
 * '<S29>'  : 'car_2026/spd_a_pid/PID Controller/preSat Signal'
 * '<S30>'  : 'car_2026/spd_a_pid/PID Controller/Anti-windup/Passthrough'
 * '<S31>'  : 'car_2026/spd_a_pid/PID Controller/D Gain/Disabled'
 * '<S32>'  : 'car_2026/spd_a_pid/PID Controller/External Derivative/Disabled'
 * '<S33>'  : 'car_2026/spd_a_pid/PID Controller/Filter/Disabled'
 * '<S34>'  : 'car_2026/spd_a_pid/PID Controller/Filter ICs/Disabled'
 * '<S35>'  : 'car_2026/spd_a_pid/PID Controller/I Gain/Internal Parameters'
 * '<S36>'  : 'car_2026/spd_a_pid/PID Controller/Ideal P Gain/Passthrough'
 * '<S37>'  : 'car_2026/spd_a_pid/PID Controller/Ideal P Gain Fdbk/Disabled'
 * '<S38>'  : 'car_2026/spd_a_pid/PID Controller/Integrator/Discrete'
 * '<S39>'  : 'car_2026/spd_a_pid/PID Controller/Integrator ICs/Internal IC'
 * '<S40>'  : 'car_2026/spd_a_pid/PID Controller/N Copy/Disabled wSignal Specification'
 * '<S41>'  : 'car_2026/spd_a_pid/PID Controller/N Gain/Disabled'
 * '<S42>'  : 'car_2026/spd_a_pid/PID Controller/P Copy/Disabled'
 * '<S43>'  : 'car_2026/spd_a_pid/PID Controller/Parallel P Gain/Internal Parameters'
 * '<S44>'  : 'car_2026/spd_a_pid/PID Controller/Reset Signal/External Reset'
 * '<S45>'  : 'car_2026/spd_a_pid/PID Controller/Saturation/Enabled'
 * '<S46>'  : 'car_2026/spd_a_pid/PID Controller/Saturation Fdbk/Disabled'
 * '<S47>'  : 'car_2026/spd_a_pid/PID Controller/Sum/Sum_PI'
 * '<S48>'  : 'car_2026/spd_a_pid/PID Controller/Sum Fdbk/Disabled'
 * '<S49>'  : 'car_2026/spd_a_pid/PID Controller/Tracking Mode/Disabled'
 * '<S50>'  : 'car_2026/spd_a_pid/PID Controller/Tracking Mode Sum/Passthrough'
 * '<S51>'  : 'car_2026/spd_a_pid/PID Controller/Tsamp - Integral/TsSignalSpecification'
 * '<S52>'  : 'car_2026/spd_a_pid/PID Controller/Tsamp - Ngain/Passthrough'
 * '<S53>'  : 'car_2026/spd_a_pid/PID Controller/postSat Signal/Forward_Path'
 * '<S54>'  : 'car_2026/spd_a_pid/PID Controller/preInt Signal/Internal PreInt'
 * '<S55>'  : 'car_2026/spd_a_pid/PID Controller/preSat Signal/Forward_Path'
 * '<S56>'  : 'car_2026/spd_b_pid/PID Controller'
 * '<S57>'  : 'car_2026/spd_b_pid/PID Controller/Anti-windup'
 * '<S58>'  : 'car_2026/spd_b_pid/PID Controller/D Gain'
 * '<S59>'  : 'car_2026/spd_b_pid/PID Controller/External Derivative'
 * '<S60>'  : 'car_2026/spd_b_pid/PID Controller/Filter'
 * '<S61>'  : 'car_2026/spd_b_pid/PID Controller/Filter ICs'
 * '<S62>'  : 'car_2026/spd_b_pid/PID Controller/I Gain'
 * '<S63>'  : 'car_2026/spd_b_pid/PID Controller/Ideal P Gain'
 * '<S64>'  : 'car_2026/spd_b_pid/PID Controller/Ideal P Gain Fdbk'
 * '<S65>'  : 'car_2026/spd_b_pid/PID Controller/Integrator'
 * '<S66>'  : 'car_2026/spd_b_pid/PID Controller/Integrator ICs'
 * '<S67>'  : 'car_2026/spd_b_pid/PID Controller/N Copy'
 * '<S68>'  : 'car_2026/spd_b_pid/PID Controller/N Gain'
 * '<S69>'  : 'car_2026/spd_b_pid/PID Controller/P Copy'
 * '<S70>'  : 'car_2026/spd_b_pid/PID Controller/Parallel P Gain'
 * '<S71>'  : 'car_2026/spd_b_pid/PID Controller/Reset Signal'
 * '<S72>'  : 'car_2026/spd_b_pid/PID Controller/Saturation'
 * '<S73>'  : 'car_2026/spd_b_pid/PID Controller/Saturation Fdbk'
 * '<S74>'  : 'car_2026/spd_b_pid/PID Controller/Sum'
 * '<S75>'  : 'car_2026/spd_b_pid/PID Controller/Sum Fdbk'
 * '<S76>'  : 'car_2026/spd_b_pid/PID Controller/Tracking Mode'
 * '<S77>'  : 'car_2026/spd_b_pid/PID Controller/Tracking Mode Sum'
 * '<S78>'  : 'car_2026/spd_b_pid/PID Controller/Tsamp - Integral'
 * '<S79>'  : 'car_2026/spd_b_pid/PID Controller/Tsamp - Ngain'
 * '<S80>'  : 'car_2026/spd_b_pid/PID Controller/postSat Signal'
 * '<S81>'  : 'car_2026/spd_b_pid/PID Controller/preInt Signal'
 * '<S82>'  : 'car_2026/spd_b_pid/PID Controller/preSat Signal'
 * '<S83>'  : 'car_2026/spd_b_pid/PID Controller/Anti-windup/Passthrough'
 * '<S84>'  : 'car_2026/spd_b_pid/PID Controller/D Gain/Disabled'
 * '<S85>'  : 'car_2026/spd_b_pid/PID Controller/External Derivative/Disabled'
 * '<S86>'  : 'car_2026/spd_b_pid/PID Controller/Filter/Disabled'
 * '<S87>'  : 'car_2026/spd_b_pid/PID Controller/Filter ICs/Disabled'
 * '<S88>'  : 'car_2026/spd_b_pid/PID Controller/I Gain/Internal Parameters'
 * '<S89>'  : 'car_2026/spd_b_pid/PID Controller/Ideal P Gain/Passthrough'
 * '<S90>'  : 'car_2026/spd_b_pid/PID Controller/Ideal P Gain Fdbk/Disabled'
 * '<S91>'  : 'car_2026/spd_b_pid/PID Controller/Integrator/Discrete'
 * '<S92>'  : 'car_2026/spd_b_pid/PID Controller/Integrator ICs/Internal IC'
 * '<S93>'  : 'car_2026/spd_b_pid/PID Controller/N Copy/Disabled wSignal Specification'
 * '<S94>'  : 'car_2026/spd_b_pid/PID Controller/N Gain/Disabled'
 * '<S95>'  : 'car_2026/spd_b_pid/PID Controller/P Copy/Disabled'
 * '<S96>'  : 'car_2026/spd_b_pid/PID Controller/Parallel P Gain/Internal Parameters'
 * '<S97>'  : 'car_2026/spd_b_pid/PID Controller/Reset Signal/External Reset'
 * '<S98>'  : 'car_2026/spd_b_pid/PID Controller/Saturation/Enabled'
 * '<S99>'  : 'car_2026/spd_b_pid/PID Controller/Saturation Fdbk/Disabled'
 * '<S100>' : 'car_2026/spd_b_pid/PID Controller/Sum/Sum_PI'
 * '<S101>' : 'car_2026/spd_b_pid/PID Controller/Sum Fdbk/Disabled'
 * '<S102>' : 'car_2026/spd_b_pid/PID Controller/Tracking Mode/Disabled'
 * '<S103>' : 'car_2026/spd_b_pid/PID Controller/Tracking Mode Sum/Passthrough'
 * '<S104>' : 'car_2026/spd_b_pid/PID Controller/Tsamp - Integral/TsSignalSpecification'
 * '<S105>' : 'car_2026/spd_b_pid/PID Controller/Tsamp - Ngain/Passthrough'
 * '<S106>' : 'car_2026/spd_b_pid/PID Controller/postSat Signal/Forward_Path'
 * '<S107>' : 'car_2026/spd_b_pid/PID Controller/preInt Signal/Internal PreInt'
 * '<S108>' : 'car_2026/spd_b_pid/PID Controller/preSat Signal/Forward_Path'
 */
#endif                                 /* car_2026_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
