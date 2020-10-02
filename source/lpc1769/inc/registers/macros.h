#ifndef INC_macro_H_
#define INC_macro_H_


#include "timers.h"


#define SB(v,n) 	v |= ( 1 << n )                     //SetBit
#define CB(v,n)     v &= ~( 1 << n)                     //ClearBit
#define TB(v,n)     v ^= (1 << n)                       //ToggleBit
#define IB(v,n,x)   v = ((v & (~(1 << n))) | (x << n))  //InitializeBit
#define I2B(v,n,x)	v = ((v & (~(3 << n))) | (x << n))	//Initialize two bits
#define VB(v,n)     ((v & (1 << n)) > 0 ? 1:0)          //VerifyBit


#define ST_TIMEOUT(x) STRELOAD = (x*96 - 1)	// (((x * 10^-6) / ((96 * 10^6)^-1)) - 1)	x -> number of microseconds


#endif /* INC_macro_H_ */
