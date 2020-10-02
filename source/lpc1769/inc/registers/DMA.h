#ifndef DMA_H_
#define DMA_H_


#include "stdint.h"


static uint32_t volatile * const PtrDMACIntStat = (uint32_t *) 0x50004000;
static uint32_t volatile * const PtrDMACIntTCStat = (uint32_t *) 0x50004004;
static uint32_t volatile * const PtrDMACIntTCClear = (uint32_t *) 0x50004008;
static uint32_t volatile * const PtrDMACIntErrStat = (uint32_t *) 0x5000400C;
static uint32_t volatile * const PtrDMACIntErrClr = (uint32_t *) 0x50004010;
static uint32_t volatile * const PtrDMACRawIntTCStat = (uint32_t *) 0x50004014;
static uint32_t volatile * const PtrDMACRawIntErrStat = (uint32_t *) 0x50004018;
static uint32_t volatile * const PtrDMACEnbldChns = (uint32_t *) 0x5000401C;
static uint32_t volatile * const PtrDMACSoftBReq = (uint32_t *) 0x50004020;
static uint32_t volatile * const PtrDMACSoftSReq = (uint32_t *) 0x50004024;
static uint32_t volatile * const PtrDMACSoftLBReq = (uint32_t *) 0x50004028;
static uint32_t volatile * const PtrDMACSoftLSReq = (uint32_t *) 0x5000402C;
static uint32_t volatile * const PtrDMACConfig = (uint32_t *) 0x50004030;
static uint32_t volatile * const PtrDMACSync = (uint32_t *) 0x50004034;
#define DMACIntStat *PtrDMACIntStat
#define DMACIntTCStat *PtrDMACIntTCStat
#define DMACIntTCClear *PtrDMACIntTCClear
#define DMACIntErrStat *PtrDMACIntErrStat
#define DMACIntErrClr *PtrDMACIntErrClr
#define DMACRawIntTCStat *PtrDMACRawIntTCStat
#define DMACRawIntErrStat *PtrDMACRawIntErrStat
#define DMACEnbldChns *PtrDMACEnbldChns
#define DMACSoftBReq *PtrDMACSoftBReq
#define DMACSoftSReq *PtrDMACSoftSReq
#define DMACSoftLBReq *PtrDMACSoftLBReq
#define DMACSoftLSReq *PtrDMACSoftLSReq
#define DMACConfig *PtrDMACConfig
#define DMACSync *PtrDMACSync


static uint32_t volatile * const PtrDMACC0SrcAddr = (uint32_t *) 0x50004100;
static uint32_t volatile * const PtrDMACC0DestAddr = (uint32_t *) 0x50004104;
static uint32_t volatile * const PtrDMACC0LLI = (uint32_t *) 0x50004108;
static uint32_t volatile * const PtrDMACC0Control = (uint32_t *) 0x5000410C;
static uint32_t volatile * const PtrDMACC0Config = (uint32_t *) 0x50004110;
#define DMACC0SrcAddr *PtrDMACC0SrcAddr
#define DMACC0DestAddr *PtrDMACC0DestAddr
#define DMACC0LLI *PtrDMACC0LLI
#define DMACC0Control *PtrDMACC0Control
#define DMACC0Config *PtrDMACC0Config


#endif /* DMA_H_ */
