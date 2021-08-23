#ifndef MOTORFUNCTIONS_H
#define MOTORFUNCTIONS_H

#include <stdint.h> 

enum motorFunction  { No = 0,
                      FullForward = 1,
                      FullBackward = 2,
                      StepForward = 3,
                      StepBackward = 4,
                      StepForward8 = 5,
                      StepBackward8 = 6,
                      SteeringForward = 7,
                      SteeringBackward = 8,
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

static const tFunctionParameters functionParameters [motorFunctionLast] =
{
  { No,          0,  { [0] = {  0,  0,  0,   0,  0,  0,   0,  0,  0,    0,  0,  0 }}},
  { FullForward, 1,  {
      [0] = {  0,  0,  0,   0,  0,  0,   0,  0,  0,    0,  0,  0 },
      [1] = {  0, 255,  0,   0, 255,  0,   0, 255,  0,    0, 255,  0 }
    }
  },
  { FullBackward, 1, {
      [0] = {  0,  0,  0,   0,  0,  0,   0,  0,  0,    0,  0,  0 },
      [1] = {255,  0,  0, 255,  0,  0, 255,  0,  0,  255,  0,  0 }
    }
  },
  { StepForward,  4, {
      [0] = {  0,  0,  0,   0,  0,  0,   0,  0,  0,    0,  0,  0 },
      [1] = {  0, 64,  0,   0,  0,  0,   0,  0,  0,    0,  0,  0 },
      [2] = {  0, 64,  0,   0, 128,  0,   0,  0,  0,    0,  0,  0 },
      [3] = {  0, 64,  0,   0, 128,  0,   0, 192,  0,    0,  0,  0 },
      [4] = {  0, 64,  0,   0, 128,  0,   0, 192,  0,    0, 255,  0 }
    }
  },
  { StepBackward, 4, {
      [0] = {  0,  0,  0,   0,  0,  0,   0,  0,  0,    0,  0,  0 },
      [1] = {  64, 0,  0,  0,  0,   0,   0,  0,  0,    0,  0,  0 },
      [2] = {  64, 0,  0, 128,  0,  0,   0,  0,  0,    0,  0,  0 },
      [3] = {  64, 0,  0, 128,  0,  0, 192,  0,  0,    0,  0,  0 },
      [4] = {  64, 0,  0, 128,  0,  0, 192,  0,  0,  255,  0,  0 }
    }
  },
  { StepForward8,  8, {
      [0] = {  0,  0,  0,   0,  0,   0,   0,  0,   0,    0,   0,  0 },
      [1] = {  0, 32,  0,   0,  0,   0,   0,  0,   0,    0,   0,  0 },
      [2] = {  0, 64,  0,   0,  0,   0,   0,  0,   0,    0,   0,  0 },
      [3] = {  0, 64,  0,   0, 64,   0,   0,  0,   0,    0,   0,  0 },
      [4] = {  0, 64,  0,   0, 128,  0,   0,  0,   0,    0,   0,  0 },
      [5] = {  0, 64,  0,   0, 128,  0,   0, 128,  0,    0,   0,  0 },
      [6] = {  0, 64,  0,   0, 128,  0,   0, 192,  0,    0,   0,  0 },
      [7] = {  0, 64,  0,   0, 128,  0,   0, 192,  0,    0, 192,  0 },
      [8] = {  0, 64,  0,   0, 128,  0,   0, 192,  0,    0, 255,  0 },

    }
  },
  { StepBackward8, 8, {
      [0] = {  0,  0,  0,   0,  0,  0,   0,  0,  0,    0,  0,  0 },
      [1] = {  32, 0,  0,  0,  0,   0,   0,  0,  0,    0,  0,  0 },
      [2] = {  64, 0,  0,  0,  0,   0,   0,  0,  0,    0,  0,  0 },
      [3] = {  64, 0,  0, 64,  0,   0,   0,  0,  0,    0,  0,  0 },
      [4] = {  64, 0,  0, 128,  0,  0,   0,  0,  0,    0,  0,  0 },
      [5] = {  64, 0,  0, 128,  0,  0, 128,  0,  0,    0,  0,  0 },
      [6] = {  64, 0,  0, 128,  0,  0, 192,  0,  0,    0,  0,  0 },
      [7] = {  64, 0,  0, 128,  0,  0, 192,  0,  0,  192,  0,  0 },
      [8] = {  64, 0,  0, 128,  0,  0, 192,  0,  0,  255,  0,  0 },
    }
  },
  { SteeringForward, 1,  {
      [0] = {  255,  255,  0,  255,  255,  0, 0,  0,  0, 0,  0,  0},
      [1] = {  0,  0,  0, 0,  0,  0, 255,  255,  0,  255,  255,  0}
    }
  },
  { SteeringBackward, 1, {
      [0] = {  0,  0,  0, 0,  0,  0, 255,  255,  0,  255,  255,  0},
      [1] = {  255, 255,  0,   255, 255,  0,   0, 0,  0,    0, 0,  0 }
    }
  }
};

int8_t MotorFunctionFullForward  (int8_t currentValue, motorDirection Action, bool buttonPressed, int8_t Steps);
int8_t MotorFunctionFullBackward (int8_t currentValue, motorDirection Action, bool buttonPressed, int8_t Steps);
int8_t MotorFunctionStepForward  (int8_t currentValue, motorDirection Action, bool buttonPressed, int8_t Steps);
int8_t MotorFunctionStepBackward (int8_t currentValue, motorDirection Action, bool buttonPressed, int8_t Steps);
int8_t MotorFunctionNo  (int8_t currentValue, motorDirection Action, bool buttonPressed, int8_t Steps);

void updateMotorFunction (uint8_t buttonId, uint8_t portId, bool reverse = false);


#endif
