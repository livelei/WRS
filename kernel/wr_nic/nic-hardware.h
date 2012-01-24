/*
 * hardware-specific definitions for the White Rabbit NIC
 *
 * Copyright (C) 2010 CERN (www.cern.ch)
 * Author: Alessandro Rubini <rubini@gnudd.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __WR_NIC_HARDWARE_H__
#define __WR_NIC_HARDWARE_H__

/* This is the clock used in internal counters. */
#define REFCLK_FREQ (125000000 / 2)
#define NSEC_PER_TICK (NSEC_PER_SEC / REFCLK_FREQ)

/* The interrupt is one of those managed by our WRVIC device */
#define WRN_IRQ_BASE		192
#define WRN_IRQ_NIC		(WRN_IRQ_BASE + 0)
#define WRN_IRQ_TSTAMP		(WRN_IRQ_BASE + 1)
//#define WRN_IRQ_PPSG		(WRN_IRQ_BASE + )
//#define WRN_IRQ_RTU		(WRN_IRQ_BASE + )
//#define WRN_IRQ_RTUT		(WRN_IRQ_BASE + )

/*
 *	V3 Memory map, temporarily (Jan 2012)
 *
 *  0x00000 - 0x1ffff:    RT Subsystem
 *  0x00000 - 0x0ffff: RT Subsystem Program Memory (16 - 64 kB)
 *  0x10000 - 0x100ff: RT Subsystem UART
 *  0x10100 - 0x101ff: RT Subsystem SoftPLL-adv
 *  0x10200 - 0x102ff: RT Subsystem SPI Master
 *  0x10300 - 0x103ff: RT Subsystem GPIO
 *  0x20000 - 0x3ffff:     NIC
 *  0x20000 - 0x20fff  NIC control regs and descriptor area
 *  0x28000 - 0x2bfff  NIC packet buffer (16k)
 *  0x30000 - 0x4ffff:           Endpoints
 *  0x30000 + N * 0x400  Endpoint N control registers
 *  0x50000 - 0x50fff:  VIC
 *  0x51000 - 0x51fff:  Tstamp unit
 *  0x52000 - 0x52fff:  PPS gen
 */
/* This is the base address of all the FPGA regions (EBI1, CS0) */
#define FPGA_BASE_NIC	0x10020000
#define FPGA_SIZE_NIC	0x00010000
#define FPGA_BASE_EP	0x10030000
#define FPGA_SIZE_EP	0x00010000
#define FPGA_SIZE_EACH_EP	0x400
#define FPGA_BASE_VIC	0x10050000 /* not used here */
#define FPGA_SIZE_VIC	0x00001000
#define FPGA_BASE_TS	0x10051000
#define FPGA_SIZE_TS	0x00001000
#define FPGA_BASE_PPSG	0x10052000
#define FPGA_SIZE_PPSG	0x00001000

enum fpga_blocks {
	WRN_FB_NIC,
	WRN_FB_EP,
	WRN_FB_PPSG,
	WRN_FB_TS,
	WRN_NR_OF_BLOCKS,
};

/* In addition to the above enumeration, we scan for those many endpoints */
#define WRN_NR_ENDPOINTS		18

/* 8 tx and 8 rx descriptors */
#define WRN_NR_DESC	8
#define WRN_NR_TXDESC	WRN_NR_DESC
#define WRN_NR_RXDESC	WRN_NR_DESC

/* Magic number for endpoint */
#define WRN_EP_MAGIC 0xcafebabe

/*
 * The following headers include the register lists, and have been
 * generated by wbgen from .wb source files in svn
 */
#include "../wbgen-regs/endpoint-regs.h"
#include "../wbgen-regs/ppsg-regs.h"
#include "../wbgen-regs/calib-regs.h"
#include "../wbgen-regs/nic-regs.h"
#include "../wbgen-regs/tstamp-regs.h"

/*
 * To make thins easier, define the descriptor structures, for tx and rx
 * Use functions in nic-mem.h to get pointes to them
 */
struct wrn_txd {
	uint32_t tx1;
	uint32_t tx2;
	uint32_t tx3;
	uint32_t unused;
};

struct wrn_rxd {
	uint32_t rx1;
	uint32_t rx2;
	uint32_t rx3;
	uint32_t unused;
};

/* Some more constants */
#define WRN_MTU 1540

#define WRN_DDATA_OFFSET 2 /* data in descriptors is offset by that much */

#endif /* __WR_NIC_HARDWARE_H__ */
