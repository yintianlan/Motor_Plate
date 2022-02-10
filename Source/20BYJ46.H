#ifndef __MT_20BYJ46_H_
#define	__MT_20BYJ46_H_

#include "Typedef.h"


extern void step_motor_init(void);
extern void step_motor_drive(uint8 dire, uint8 speed);
extern void step_motor_stop(void);

extern void step_motor_test(void);


#endif
