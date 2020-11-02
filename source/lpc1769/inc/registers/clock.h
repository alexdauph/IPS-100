#ifndef CLOCK_H_
#define CLOCK_H_


/* Set power clock in PCONP & PCLK_SELn*/
static uint32_t volatile * const PtrCCLKCFG = (unsigned long *) 0x400FC104;
static uint32_t volatile * const PtrPCONP = (unsigned long *) 0x400FC0C4;
static uint32_t volatile * const PtrPCLKSEL0 = (uint32_t *) 0x400FC1A8;
static uint32_t volatile * const PtrPCLKSEL1 = (uint32_t *) 0x400FC1AC;
static uint32_t volatile * const PtrCLKOUTCFG = (uint32_t *) 0x400FC1C8;
#define CCLKCFG *PtrCCLKCFG
#define PCONP *PtrPCONP
#define PCLKSEL0 *PtrPCLKSEL0
#define PCLKSEL1 *PtrPCLKSEL1
#define CLKOUTCFG *PtrCLKOUTCFG



#endif /* CLOCK_H_ */
