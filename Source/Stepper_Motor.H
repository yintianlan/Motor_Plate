#ifndef _STEPPER_MOTOR_H_
#define	_STEPPER_MOTOR_H_

#include "Typedef.h"


//#define uint unsigned int
//#define uchar unsigned char
//#define uint8 unsigned char


extern void step_motor_init(void);
extern void step_motor_drive(uint8 dire, uint8 speed);
extern void step_motor_stop(void);



extern void Motor_Init(void);
extern void shun_fun_N(void);
extern void execute(void);
extern void Coil_Test(void);



#endif
