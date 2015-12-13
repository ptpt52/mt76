/*
 * Copyright (C) 2015 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __MT7603_REGS_H
#define __MT7603_REGS_H

#define MT_HW_REV			0x1000
#define MT_HW_CHIPID			0x1008
#define MT_TOP_MISC2			0x1134

#define MT_MCU_BASE			0x2000
#define MT_MCU(ofs)			(MT_MCU_BASE + (ofs))

#define MT_MCU_PCIE_REMAP_1		MT_MCU(0x500)
#define MT_MCU_PCIE_REMAP_1_OFFSET	GENMASK(17, 0)
#define MT_MCU_PCIE_REMAP_1_BASE	GENMASK(31, 18)

#define MT_MCU_PCIE_REMAP_2		MT_MCU(0x504)
#define MT_MCU_PCIE_REMAP_2_OFFSET	GENMASK(18, 0)
#define MT_MCU_PCIE_REMAP_2_BASE	GENMASK(31, 19)

#define MT_HIF_BASE			0x4000
#define MT_HIF(ofs)			(MT_HIF_BASE + (ofs))

#define MT_INT_SOURCE_CSR		MT_HIF(0x200)
#define MT_INT_MASK_CSR			MT_HIF(0x204)
#define MT_DELAY_INT_CFG		MT_HIF(0x204)

#define MT_INT_RX_DONE(_n)		BIT(_n)
#define MT_INT_RX_DONE_ALL		GENMASK(1, 0)
#define MT_INT_TX_DONE_ALL		GENMASK(19, 4)
#define MT_INT_TX_DONE(_n)		BIT(_n + 4)

#define MT_INT_RX_COHERENT		BIT(20)
#define MT_INT_TX_COHERENT		BIT(21)
#define MT_INT_PRE_TBTT			BIT(27)

#define MT_INT_MCU_CMD			BIT(30)

#define MT_WPDMA_GLO_CFG		MT_HIF(0x208)
#define MT_WPDMA_GLO_CFG_TX_DMA_EN	BIT(0)
#define MT_WPDMA_GLO_CFG_TX_DMA_BUSY	BIT(1)
#define MT_WPDMA_GLO_CFG_RX_DMA_EN	BIT(2)
#define MT_WPDMA_GLO_CFG_RX_DMA_BUSY	BIT(3)
#define MT_WPDMA_GLO_CFG_DMA_BURST_SIZE	GENMASK(5, 4)
#define MT_WPDMA_GLO_CFG_TX_WRITEBACK_DONE	BIT(6)
#define MT_WPDMA_GLO_CFG_BIG_ENDIAN	BIT(7)
#define MT_WPDMA_GLO_CFG_HDR_SEG_LEN	GENMASK(15, 8)
#define MT_WPDMA_GLO_CFG_CLK_GATE_DIS	BIT(30)
#define MT_WPDMA_GLO_CFG_RX_2B_OFFSET	BIT(31)

#define MT_WPDMA_RST_IDX		MT_HIF(0x20c)

#define MT_TX_RING_BASE			MT_HIF(0x300)
#define MT_RX_RING_BASE			MT_HIF(0x400)

#define MT_TXTIME_THRESH_BASE		MT_HIF(0x500)
#define MT_TXTIME_THRESH(n)		(MT_TXTIME_THRESH_BASE + ((n) * 4))

#define MT_PAGE_COUNT_BASE		MT_HIF(0x540)
#define MT_PAGE_COUNT(n)		(MT_PAGE_COUNT_BASE + ((n) * 4))

#define MT_SCH_1			MT_HIF(0x588)
#define MT_SCH_2			MT_HIF(0x58c)
#define MT_SCH_3			MT_HIF(0x590)

#define MT_SCH_4			MT_HIF(0x594)
#define MT_SCH_4_FORCE_QID		GENMASK(4, 0)
#define MT_SCH_4_BYPASS			BIT(5)

#define MT_GROUP_THRESH_BASE		MT_HIF(0x598)
#define MT_GROUP_THRESH(n)		(MT_GROUP_THRESH_BASE + ((n) * 4))

#define MT_QUEUE_PRIORITY_1		MT_HIF(0x580)
#define MT_QUEUE_PRIORITY_2		MT_HIF(0x584)

#define MT_BMAP_0			MT_HIF(0x5b0)
#define MT_BMAP_1			MT_HIF(0x5b4)
#define MT_BMAP_2			MT_HIF(0x5b8)

#define MT_HIGH_PRIORITY_1		MT_HIF(0x5bc)
#define MT_HIGH_PRIORITY_2		MT_HIF(0x5c0)

#define MT_PRIORITY_MASK		MT_HIF(0x5c4)

#define MT_RSV_MAX_THRESH		MT_HIF(0x5c8)

#define MT_PSE_BASE			0x8000
#define MT_PSE(ofs)			(MT_PSE_BASE + (ofs))

#define MT_PSE_FC_P0			MT_PSE(0x120)
#define MT_PSE_FC_P0_MIN_RESERVE	GENMASK(11, 0)
#define MT_PSE_FC_P0_MAX_QUOTA		GENMASK(27, 16)

#define MT_PSE_FRP			MT_PSE(0x138)
#define MT_PSE_FRP_P0			GENMASK(2, 0)
#define MT_PSE_FRP_P1			GENMASK(5, 3)
#define MT_PSE_FRP_P2_RQ0		GENMASK(8, 6)
#define MT_PSE_FRP_P2_RQ1		GENMASK(11, 9)
#define MT_PSE_FRP_P2_RQ2		GENMASK(14, 12)

#define MT_WF_AGG_BASE			0x21200
#define MT_WF_AGG(ofs)			(MT_WF_AGG_BASE + (ofs))

#define MT_AGG_TMP			MT_WF_AGG(0x0d8)


#define MT_WF_DMA_BASE			0x21c00
#define MT_WF_DMA(ofs)			(MT_WF_DMA_BASE + (ofs))

#define MT_DMA_DCR0			MT_WF_DMA(0x000)
#define MT_DMA_DCR1			MT_WF_DMA(0x004)
#define MT_DMA_RCFR0			MT_WF_DMA(0x070)
#define MT_DMA_VCFR0			MT_WF_DMA(0x07c)
#define MT_DMA_TMCFR0			MT_WF_DMA(0x088)

#define MT_WF_ARB_BASE			0x21400
#define MT_WF_ARB(ofs)			(MT_WF_ARB_BASE + (ofs))

#define MT_WF_ARB_RQCR			MT_WF_ARB(0x070)
#define MT_WF_ARB_RQCR_RX_START		BIT(0)
#define MT_WF_ARB_RQCR_RXV_START	BIT(4)
#define MT_WF_ARB_RQCR_RXV_R_EN		BIT(7)
#define MT_WF_ARB_RQCR_RXV_T_EN		BIT(8)

#define MT_WF_ARB_SCR			MT_WF_ARB(0x080)
#define MT_WF_ARB_TX_DISABLE		BIT(8)
#define MT_WF_ARB_RX_DISABLE		BIT(9)

#define MT_WF_ARB_TQCR0			MT_WF_ARB(0x100)

#define MT_WF_RMAC_BASE			0x21800
#define MT_WF_RMAC(ofs)			(MT_WF_RMAC_BASE + (ofs))

#define MT_WF_RFCR			MT_WF_RMAC(0x000)
#define MT_WF_RFCR_DROP_STBC_MULTI	BIT(0)
#define MT_WF_RFCR_DROP_FCSFAIL		BIT(1)
#define MT_WF_RFCR_DROP_VERSION		BIT(3)
#define MT_WF_RFCR_DROP_PROBEREQ	BIT(4)
#define MT_WF_RFCR_DROP_MCAST		BIT(5)
#define MT_WF_RFCR_DROP_BCAST		BIT(6)
#define MT_WF_RFCR_DROP_MCAST_FILTERED	BIT(7)
#define MT_WF_RFCR_DROP_A3_MAC		BIT(8)
#define MT_WF_RFCR_DROP_A3_BSSID	BIT(9)
#define MT_WF_RFCR_DROP_A2_BSSID	BIT(10)
#define MT_WF_RFCR_DROP_OTHER_BEACON	BIT(11)
#define MT_WF_RFCR_DROP_FRAME_REPORT	BIT(12)
#define MT_WF_RFCR_DROP_CTL_RSV		BIT(13)
#define MT_WF_RFCR_DROP_CTS		BIT(14)
#define MT_WF_RFCR_DROP_RTS		BIT(15)
#define MT_WF_RFCR_DROP_DUPLICATE	BIT(16)
#define MT_WF_RFCR_DROP_OTHER_BSS	BIT(17)
#define MT_WF_RFCR_DROP_OTHER_UC	BIT(18)
#define MT_WF_RFCR_DROP_OTHER_TIM	BIT(19)
#define MT_WF_RFCR_DROP_NDPA		BIT(20)
#define MT_WF_RFCR_DROP_UNWANTED_CTL	BIT(21)

#define MT_WF_RMAC_RMACDR		MT_WF_RMAC(0x078)
#define MT_WF_RMAC_MAXMINLEN		MT_WF_RMAC(0x098)
#define MT_WF_RFCR1			MT_WF_RMAC(0x0a4)
#define MT_WF_RMAC_TMR_PA		MT_WF_RMAC(0x0e0)

#define MT_PCIE_REMAP_BASE_1		0x40000
#define MT_PCIE_REMAP_BASE_2		0x80000

#define MT_TX_HW_QUEUE_MGMT		4
#define MT_TX_HW_QUEUE_MCU		5
#define MT_TX_HW_QUEUE_BCN		7
#define MT_TX_HW_QUEUE_BMC		8

#define MT_TMAC_INFO_TEMPLATE		0x800c0040

#define MT_PSE_BASE_PHYS_ADDR		0xa0000000

#define MT_PSE_WTBL_2_PHYS_ADDR		0xa5000000


#endif
