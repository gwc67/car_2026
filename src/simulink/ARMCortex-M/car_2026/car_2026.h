/*
 * File: car_2026.h
 *
 * Code generated for Simulink model 'car_2026'.
 *
 * Model version                  : 1.4
 * Simulink Coder version         : 25.1 (R2025a) 21-Nov-2024
 * C/C++ source code generated on : Fri Sep 11 08:52:34 2026
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
#define DEGREE                         57.2957802F               /* Referenced by: '<Root>/Gain' */
#define PWM_MAX                        800                       /* Referenced by:
                                                                  * '<S101>/Saturation'
                                                                  * '<S154>/Saturation'
                                                                  */
#define PWM_MIN                        -800                      /* Referenced by:
                                                                  * '<S101>/Saturation'
                                                                  * '<S154>/Saturation'
                                                                  */
#define SPD_INTEGRAL_MAX               450.0F                    /* Referenced by:
                                                                  * '<S94>/Integrator'
                                                                  * '<S147>/Integrator'
                                                                  */
#define SPD_INTEGRAL_MIN               -450.0F                   /* Referenced by:
                                                                  * '<S94>/Integrator'
                                                                  * '<S147>/Integrator'
                                                                  */
#define TURN_SPD_MAX                   20.0F                     /* Referenced by: '<S48>/Saturation' */
#define TURN_SPD_MIN                   -20.0F                    /* Referenced by: '<S48>/Saturation' */

/* Block signals (default storage) */
typedef struct {
  real32_T RT1;                        /* '<Root>/RT1' */
  real32_T TmpRTBAtAdd1Inport2;        /* '<Root>/Subsystem' */
} B_car_2026_T;

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real32_T UnitDelay_DSTATE;           /* '<S4>/Unit Delay' */
  real32_T UnitDelay_DSTATE_b;         /* '<S5>/Unit Delay' */
  real32_T DiscreteTimeIntegrator_DSTATE;/* '<Root>/Discrete-Time Integrator' */
  real32_T Integrator_DSTATE;          /* '<S147>/Integrator' */
  real32_T Integrator_DSTATE_o;        /* '<S94>/Integrator' */
  real32_T Filter_DSTATE;              /* '<S36>/Filter' */
  real32_T RT1_Buffer0;                /* '<Root>/RT1' */
  real32_T TmpRTBAtAdd1Inport2_Buffer0;/* synthesized block */
  real32_T RT_Buffer;                  /* '<Root>/RT' */
  int8_T Integrator_PrevResetState;    /* '<S147>/Integrator' */
  int8_T Integrator_PrevResetState_e;  /* '<S94>/Integrator' */
  uint8_T running_flag;                /* '<Root>/Data Store Memory' */
} DW_car_2026_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real32_T spd_a;                      /* '<Root>/spd_a' */
  uint8_T running_flag;                /* '<Root>/running_flag' */
  real32_T tar_spd_a;                  /* '<Root>/tar_spd_a' */
  real32_T spd_b;                      /* '<Root>/spd_b' */
  real32_T tar_spd_b;                  /* '<Root>/tar_spd_b' */
  real32_T target_yaw;                 /* '<Root>/target_yaw' */
  real32_T gyroz;                      /* '<Root>/gyroz' */
} ExtU_car_2026_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  int16_T pwm_a;                       /* '<Root>/pwm_a' */
  int16_T pwm_b;                       /* '<Root>/pwm_b' */
  real32_T spd_fliter_a;               /* '<Root>/spd_fliter_a ' */
  real32_T spd_fliter_b;               /* '<Root>/spd_fliter_b ' */
  real32_T yaw_out;                    /* '<Root>/yaw_out' */
  real32_T turn_spd;                   /* '<Root>/turn_spd' */
  real32_T tar_spd_a_2;                /* '<Root>/tar_spd_a_2' */
  real32_T tar_spd_b_2;                /* '<Root>/tar_spd_b_2' */
} ExtY_car_2026_T;

/* Real-time Model Data Structure */
struct tag_RTM_car_2026_T {
  const char_T * volatile errorStatus;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    struct {
      uint8_T TID1_2;
    } RateInteraction;
  } Timing;
};

/* Block signals (default storage) */
extern B_car_2026_T car_2026_B;

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
                                        *   '<S91>/Integral Gain'
                                        *   '<S144>/Integral Gain'
                                        */
extern real32_T SPD_KP;                /* Variable: SPD_KP
                                        * Referenced by:
                                        *   '<S99>/Proportional Gain'
                                        *   '<S152>/Proportional Gain'
                                        */
extern real32_T TURN_KD;               /* Variable: TURN_KD
                                        * Referenced by: '<S34>/Derivative Gain'
                                        */
extern real32_T TURN_KP;               /* Variable: TURN_KP
                                        * Referenced by: '<S46>/Proportional Gain'
                                        */
extern real32_T spd_filiter;           /* Variable: spd_filiter
                                        * Referenced by:
                                        *   '<S4>/Constant1'
                                        *   '<S5>/Constant1'
                                        */

/* Model entry point functions */
extern void car_2026_initialize(void);
extern void car_2026_step0(void);      /* Sample time: [0.001s, 0.0s] */
extern void car_2026_step1(void);      /* Sample time: [0.005s, 0.0s] */
extern void car_2026_step2(void);      /* Sample time: [0.01s, 0.0s] */
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
 * '<S1>'   : 'car_2026/Subsystem'
 * '<S2>'   : 'car_2026/spd_a_pid'
 * '<S3>'   : 'car_2026/spd_b_pid'
 * '<S4>'   : 'car_2026/Ò»½×µÍÍ¨ÂË²¨'
 * '<S5>'   : 'car_2026/Ò»½×µÍÍ¨ÂË²¨1'
 * '<S6>'   : 'car_2026/Subsystem/PID Controller'
 * '<S7>'   : 'car_2026/Subsystem/PID Controller/Anti-windup'
 * '<S8>'   : 'car_2026/Subsystem/PID Controller/D Gain'
 * '<S9>'   : 'car_2026/Subsystem/PID Controller/External Derivative'
 * '<S10>'  : 'car_2026/Subsystem/PID Controller/Filter'
 * '<S11>'  : 'car_2026/Subsystem/PID Controller/Filter ICs'
 * '<S12>'  : 'car_2026/Subsystem/PID Controller/I Gain'
 * '<S13>'  : 'car_2026/Subsystem/PID Controller/Ideal P Gain'
 * '<S14>'  : 'car_2026/Subsystem/PID Controller/Ideal P Gain Fdbk'
 * '<S15>'  : 'car_2026/Subsystem/PID Controller/Integrator'
 * '<S16>'  : 'car_2026/Subsystem/PID Controller/Integrator ICs'
 * '<S17>'  : 'car_2026/Subsystem/PID Controller/N Copy'
 * '<S18>'  : 'car_2026/Subsystem/PID Controller/N Gain'
 * '<S19>'  : 'car_2026/Subsystem/PID Controller/P Copy'
 * '<S20>'  : 'car_2026/Subsystem/PID Controller/Parallel P Gain'
 * '<S21>'  : 'car_2026/Subsystem/PID Controller/Reset Signal'
 * '<S22>'  : 'car_2026/Subsystem/PID Controller/Saturation'
 * '<S23>'  : 'car_2026/Subsystem/PID Controller/Saturation Fdbk'
 * '<S24>'  : 'car_2026/Subsystem/PID Controller/Sum'
 * '<S25>'  : 'car_2026/Subsystem/PID Controller/Sum Fdbk'
 * '<S26>'  : 'car_2026/Subsystem/PID Controller/Tracking Mode'
 * '<S27>'  : 'car_2026/Subsystem/PID Controller/Tracking Mode Sum'
 * '<S28>'  : 'car_2026/Subsystem/PID Controller/Tsamp - Integral'
 * '<S29>'  : 'car_2026/Subsystem/PID Controller/Tsamp - Ngain'
 * '<S30>'  : 'car_2026/Subsystem/PID Controller/postSat Signal'
 * '<S31>'  : 'car_2026/Subsystem/PID Controller/preInt Signal'
 * '<S32>'  : 'car_2026/Subsystem/PID Controller/preSat Signal'
 * '<S33>'  : 'car_2026/Subsystem/PID Controller/Anti-windup/Disabled'
 * '<S34>'  : 'car_2026/Subsystem/PID Controller/D Gain/Internal Parameters'
 * '<S35>'  : 'car_2026/Subsystem/PID Controller/External Derivative/Error'
 * '<S36>'  : 'car_2026/Subsystem/PID Controller/Filter/Disc. Forward Euler Filter'
 * '<S37>'  : 'car_2026/Subsystem/PID Controller/Filter ICs/Internal IC - Filter'
 * '<S38>'  : 'car_2026/Subsystem/PID Controller/I Gain/Disabled'
 * '<S39>'  : 'car_2026/Subsystem/PID Controller/Ideal P Gain/Passthrough'
 * '<S40>'  : 'car_2026/Subsystem/PID Controller/Ideal P Gain Fdbk/Disabled'
 * '<S41>'  : 'car_2026/Subsystem/PID Controller/Integrator/Disabled'
 * '<S42>'  : 'car_2026/Subsystem/PID Controller/Integrator ICs/Disabled'
 * '<S43>'  : 'car_2026/Subsystem/PID Controller/N Copy/Disabled'
 * '<S44>'  : 'car_2026/Subsystem/PID Controller/N Gain/Internal Parameters'
 * '<S45>'  : 'car_2026/Subsystem/PID Controller/P Copy/Disabled'
 * '<S46>'  : 'car_2026/Subsystem/PID Controller/Parallel P Gain/Internal Parameters'
 * '<S47>'  : 'car_2026/Subsystem/PID Controller/Reset Signal/Disabled'
 * '<S48>'  : 'car_2026/Subsystem/PID Controller/Saturation/Enabled'
 * '<S49>'  : 'car_2026/Subsystem/PID Controller/Saturation Fdbk/Disabled'
 * '<S50>'  : 'car_2026/Subsystem/PID Controller/Sum/Sum_PD'
 * '<S51>'  : 'car_2026/Subsystem/PID Controller/Sum Fdbk/Disabled'
 * '<S52>'  : 'car_2026/Subsystem/PID Controller/Tracking Mode/Disabled'
 * '<S53>'  : 'car_2026/Subsystem/PID Controller/Tracking Mode Sum/Passthrough'
 * '<S54>'  : 'car_2026/Subsystem/PID Controller/Tsamp - Integral/TsSignalSpecification'
 * '<S55>'  : 'car_2026/Subsystem/PID Controller/Tsamp - Ngain/Passthrough'
 * '<S56>'  : 'car_2026/Subsystem/PID Controller/postSat Signal/Forward_Path'
 * '<S57>'  : 'car_2026/Subsystem/PID Controller/preInt Signal/Internal PreInt'
 * '<S58>'  : 'car_2026/Subsystem/PID Controller/preSat Signal/Forward_Path'
 * '<S59>'  : 'car_2026/spd_a_pid/PID Controller'
 * '<S60>'  : 'car_2026/spd_a_pid/PID Controller/Anti-windup'
 * '<S61>'  : 'car_2026/spd_a_pid/PID Controller/D Gain'
 * '<S62>'  : 'car_2026/spd_a_pid/PID Controller/External Derivative'
 * '<S63>'  : 'car_2026/spd_a_pid/PID Controller/Filter'
 * '<S64>'  : 'car_2026/spd_a_pid/PID Controller/Filter ICs'
 * '<S65>'  : 'car_2026/spd_a_pid/PID Controller/I Gain'
 * '<S66>'  : 'car_2026/spd_a_pid/PID Controller/Ideal P Gain'
 * '<S67>'  : 'car_2026/spd_a_pid/PID Controller/Ideal P Gain Fdbk'
 * '<S68>'  : 'car_2026/spd_a_pid/PID Controller/Integrator'
 * '<S69>'  : 'car_2026/spd_a_pid/PID Controller/Integrator ICs'
 * '<S70>'  : 'car_2026/spd_a_pid/PID Controller/N Copy'
 * '<S71>'  : 'car_2026/spd_a_pid/PID Controller/N Gain'
 * '<S72>'  : 'car_2026/spd_a_pid/PID Controller/P Copy'
 * '<S73>'  : 'car_2026/spd_a_pid/PID Controller/Parallel P Gain'
 * '<S74>'  : 'car_2026/spd_a_pid/PID Controller/Reset Signal'
 * '<S75>'  : 'car_2026/spd_a_pid/PID Controller/Saturation'
 * '<S76>'  : 'car_2026/spd_a_pid/PID Controller/Saturation Fdbk'
 * '<S77>'  : 'car_2026/spd_a_pid/PID Controller/Sum'
 * '<S78>'  : 'car_2026/spd_a_pid/PID Controller/Sum Fdbk'
 * '<S79>'  : 'car_2026/spd_a_pid/PID Controller/Tracking Mode'
 * '<S80>'  : 'car_2026/spd_a_pid/PID Controller/Tracking Mode Sum'
 * '<S81>'  : 'car_2026/spd_a_pid/PID Controller/Tsamp - Integral'
 * '<S82>'  : 'car_2026/spd_a_pid/PID Controller/Tsamp - Ngain'
 * '<S83>'  : 'car_2026/spd_a_pid/PID Controller/postSat Signal'
 * '<S84>'  : 'car_2026/spd_a_pid/PID Controller/preInt Signal'
 * '<S85>'  : 'car_2026/spd_a_pid/PID Controller/preSat Signal'
 * '<S86>'  : 'car_2026/spd_a_pid/PID Controller/Anti-windup/Passthrough'
 * '<S87>'  : 'car_2026/spd_a_pid/PID Controller/D Gain/Disabled'
 * '<S88>'  : 'car_2026/spd_a_pid/PID Controller/External Derivative/Disabled'
 * '<S89>'  : 'car_2026/spd_a_pid/PID Controller/Filter/Disabled'
 * '<S90>'  : 'car_2026/spd_a_pid/PID Controller/Filter ICs/Disabled'
 * '<S91>'  : 'car_2026/spd_a_pid/PID Controller/I Gain/Internal Parameters'
 * '<S92>'  : 'car_2026/spd_a_pid/PID Controller/Ideal P Gain/Passthrough'
 * '<S93>'  : 'car_2026/spd_a_pid/PID Controller/Ideal P Gain Fdbk/Disabled'
 * '<S94>'  : 'car_2026/spd_a_pid/PID Controller/Integrator/Discrete'
 * '<S95>'  : 'car_2026/spd_a_pid/PID Controller/Integrator ICs/Internal IC'
 * '<S96>'  : 'car_2026/spd_a_pid/PID Controller/N Copy/Disabled wSignal Specification'
 * '<S97>'  : 'car_2026/spd_a_pid/PID Controller/N Gain/Disabled'
 * '<S98>'  : 'car_2026/spd_a_pid/PID Controller/P Copy/Disabled'
 * '<S99>'  : 'car_2026/spd_a_pid/PID Controller/Parallel P Gain/Internal Parameters'
 * '<S100>' : 'car_2026/spd_a_pid/PID Controller/Reset Signal/External Reset'
 * '<S101>' : 'car_2026/spd_a_pid/PID Controller/Saturation/Enabled'
 * '<S102>' : 'car_2026/spd_a_pid/PID Controller/Saturation Fdbk/Disabled'
 * '<S103>' : 'car_2026/spd_a_pid/PID Controller/Sum/Sum_PI'
 * '<S104>' : 'car_2026/spd_a_pid/PID Controller/Sum Fdbk/Disabled'
 * '<S105>' : 'car_2026/spd_a_pid/PID Controller/Tracking Mode/Disabled'
 * '<S106>' : 'car_2026/spd_a_pid/PID Controller/Tracking Mode Sum/Passthrough'
 * '<S107>' : 'car_2026/spd_a_pid/PID Controller/Tsamp - Integral/TsSignalSpecification'
 * '<S108>' : 'car_2026/spd_a_pid/PID Controller/Tsamp - Ngain/Passthrough'
 * '<S109>' : 'car_2026/spd_a_pid/PID Controller/postSat Signal/Forward_Path'
 * '<S110>' : 'car_2026/spd_a_pid/PID Controller/preInt Signal/Internal PreInt'
 * '<S111>' : 'car_2026/spd_a_pid/PID Controller/preSat Signal/Forward_Path'
 * '<S112>' : 'car_2026/spd_b_pid/PID Controller'
 * '<S113>' : 'car_2026/spd_b_pid/PID Controller/Anti-windup'
 * '<S114>' : 'car_2026/spd_b_pid/PID Controller/D Gain'
 * '<S115>' : 'car_2026/spd_b_pid/PID Controller/External Derivative'
 * '<S116>' : 'car_2026/spd_b_pid/PID Controller/Filter'
 * '<S117>' : 'car_2026/spd_b_pid/PID Controller/Filter ICs'
 * '<S118>' : 'car_2026/spd_b_pid/PID Controller/I Gain'
 * '<S119>' : 'car_2026/spd_b_pid/PID Controller/Ideal P Gain'
 * '<S120>' : 'car_2026/spd_b_pid/PID Controller/Ideal P Gain Fdbk'
 * '<S121>' : 'car_2026/spd_b_pid/PID Controller/Integrator'
 * '<S122>' : 'car_2026/spd_b_pid/PID Controller/Integrator ICs'
 * '<S123>' : 'car_2026/spd_b_pid/PID Controller/N Copy'
 * '<S124>' : 'car_2026/spd_b_pid/PID Controller/N Gain'
 * '<S125>' : 'car_2026/spd_b_pid/PID Controller/P Copy'
 * '<S126>' : 'car_2026/spd_b_pid/PID Controller/Parallel P Gain'
 * '<S127>' : 'car_2026/spd_b_pid/PID Controller/Reset Signal'
 * '<S128>' : 'car_2026/spd_b_pid/PID Controller/Saturation'
 * '<S129>' : 'car_2026/spd_b_pid/PID Controller/Saturation Fdbk'
 * '<S130>' : 'car_2026/spd_b_pid/PID Controller/Sum'
 * '<S131>' : 'car_2026/spd_b_pid/PID Controller/Sum Fdbk'
 * '<S132>' : 'car_2026/spd_b_pid/PID Controller/Tracking Mode'
 * '<S133>' : 'car_2026/spd_b_pid/PID Controller/Tracking Mode Sum'
 * '<S134>' : 'car_2026/spd_b_pid/PID Controller/Tsamp - Integral'
 * '<S135>' : 'car_2026/spd_b_pid/PID Controller/Tsamp - Ngain'
 * '<S136>' : 'car_2026/spd_b_pid/PID Controller/postSat Signal'
 * '<S137>' : 'car_2026/spd_b_pid/PID Controller/preInt Signal'
 * '<S138>' : 'car_2026/spd_b_pid/PID Controller/preSat Signal'
 * '<S139>' : 'car_2026/spd_b_pid/PID Controller/Anti-windup/Passthrough'
 * '<S140>' : 'car_2026/spd_b_pid/PID Controller/D Gain/Disabled'
 * '<S141>' : 'car_2026/spd_b_pid/PID Controller/External Derivative/Disabled'
 * '<S142>' : 'car_2026/spd_b_pid/PID Controller/Filter/Disabled'
 * '<S143>' : 'car_2026/spd_b_pid/PID Controller/Filter ICs/Disabled'
 * '<S144>' : 'car_2026/spd_b_pid/PID Controller/I Gain/Internal Parameters'
 * '<S145>' : 'car_2026/spd_b_pid/PID Controller/Ideal P Gain/Passthrough'
 * '<S146>' : 'car_2026/spd_b_pid/PID Controller/Ideal P Gain Fdbk/Disabled'
 * '<S147>' : 'car_2026/spd_b_pid/PID Controller/Integrator/Discrete'
 * '<S148>' : 'car_2026/spd_b_pid/PID Controller/Integrator ICs/Internal IC'
 * '<S149>' : 'car_2026/spd_b_pid/PID Controller/N Copy/Disabled wSignal Specification'
 * '<S150>' : 'car_2026/spd_b_pid/PID Controller/N Gain/Disabled'
 * '<S151>' : 'car_2026/spd_b_pid/PID Controller/P Copy/Disabled'
 * '<S152>' : 'car_2026/spd_b_pid/PID Controller/Parallel P Gain/Internal Parameters'
 * '<S153>' : 'car_2026/spd_b_pid/PID Controller/Reset Signal/External Reset'
 * '<S154>' : 'car_2026/spd_b_pid/PID Controller/Saturation/Enabled'
 * '<S155>' : 'car_2026/spd_b_pid/PID Controller/Saturation Fdbk/Disabled'
 * '<S156>' : 'car_2026/spd_b_pid/PID Controller/Sum/Sum_PI'
 * '<S157>' : 'car_2026/spd_b_pid/PID Controller/Sum Fdbk/Disabled'
 * '<S158>' : 'car_2026/spd_b_pid/PID Controller/Tracking Mode/Disabled'
 * '<S159>' : 'car_2026/spd_b_pid/PID Controller/Tracking Mode Sum/Passthrough'
 * '<S160>' : 'car_2026/spd_b_pid/PID Controller/Tsamp - Integral/TsSignalSpecification'
 * '<S161>' : 'car_2026/spd_b_pid/PID Controller/Tsamp - Ngain/Passthrough'
 * '<S162>' : 'car_2026/spd_b_pid/PID Controller/postSat Signal/Forward_Path'
 * '<S163>' : 'car_2026/spd_b_pid/PID Controller/preInt Signal/Internal PreInt'
 * '<S164>' : 'car_2026/spd_b_pid/PID Controller/preSat Signal/Forward_Path'
 */
#endif                                 /* car_2026_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
