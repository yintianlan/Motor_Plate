#ifndef __STC8H_PWM_H_
#define __STC8H_PWM_H_


#ifndef _STC_PWM_GLOBAL_
	#define	STC_PWM_GLOBAL	extern
#else
	#define	STC_PWM_GLOBAL
#endif

	
	
STC_PWM_GLOBAL	void PWM_config(void);

STC_PWM_GLOBAL	void PWM5_SetPwmWide(unsigned short Wide);
STC_PWM_GLOBAL	void PWM6_SetPwmWide(unsigned short Wide);
STC_PWM_GLOBAL	void PWM7_SetPwmWide(unsigned short Wide);
STC_PWM_GLOBAL	void PWM8_SetPwmWide(unsigned short Wide);


#endif
