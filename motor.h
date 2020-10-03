#ifndef MOTOR_H
#define MOTOR_H

enum motorFunction  { No = 0,
                      FullForward = 1,
                      FullBackward = 2,
                      StepForward = 3,
                      StepBackward = 4,
					  StepForward8 = 5,
					  StepBackward8 = 6,
                      motorFunctionLast
                    };

enum motorMode      { Off = 0,
                      Push,
                      PushRamp,
                      ToggleStep
                    };

enum motorDirection { Forward,
                      Backward,
                      Stop
                    };

typedef struct sFunctionParameters {
  motorFunction   Function;
  int8_t          Steps;
  uint8_t         Graphics [9][12];
} tFunctionParameters;

typedef struct sSettings {
  motorFunction   Function;
  uint8_t         Strength;
  int8_t          currentValue;
} tSettings;

#endif
