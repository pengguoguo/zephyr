/*
 * Copyright (c) 2019, Linaro Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <init.h>
#include "device_imx.h"

static int meerakt96_pinmux_init(struct device *dev)
{
	ARG_UNUSED(dev);

#if DT_HAS_NODE(DT_NODELABEL(gpio1))
	/* GPIO1_IO04 Mux Config */
	IOMUXC_LPSR_SW_MUX_CTL_PAD_GPIO1_IO04 = 0;
	IOMUXC_LPSR_SW_PAD_CTL_PAD_GPIO1_IO04 = 0;
	/* GPIO1_IO05 Mux Config */
	IOMUXC_LPSR_SW_MUX_CTL_PAD_GPIO1_IO05 = 0;
	IOMUXC_LPSR_SW_PAD_CTL_PAD_GPIO1_IO05 = 0;
	/* GPIO1_IO06 Mux Config */
	IOMUXC_LPSR_SW_MUX_CTL_PAD_GPIO1_IO06 = 0;
	IOMUXC_LPSR_SW_PAD_CTL_PAD_GPIO1_IO07 = 0;
	/* GPIO1_IO07 Mux Config */
	IOMUXC_LPSR_SW_MUX_CTL_PAD_GPIO1_IO07 = 0;
	IOMUXC_LPSR_SW_PAD_CTL_PAD_GPIO1_IO07 = 0;
#endif

#if DT_HAS_NODE(DT_NODELABEL(uart1))
	IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA =
		IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA_MUX_MODE(0);
	IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA =
		IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA_MUX_MODE(0);
	IOMUXC_SW_PAD_CTL_PAD_UART1_RX_DATA =
		IOMUXC_SW_PAD_CTL_PAD_UART1_RX_DATA_PE_MASK  |
		IOMUXC_SW_PAD_CTL_PAD_UART1_RX_DATA_PS(3)    |
		IOMUXC_SW_PAD_CTL_PAD_UART1_RX_DATA_HYS_MASK |
		IOMUXC_SW_PAD_CTL_PAD_UART1_RX_DATA_DSE(0);

	IOMUXC_SW_PAD_CTL_PAD_UART1_TX_DATA =
		IOMUXC_SW_PAD_CTL_PAD_UART1_TX_DATA_PE_MASK  |
		IOMUXC_SW_PAD_CTL_PAD_UART1_TX_DATA_PS(3)    |
		IOMUXC_SW_PAD_CTL_PAD_UART1_RX_DATA_HYS_MASK |
		IOMUXC_SW_PAD_CTL_PAD_UART1_TX_DATA_DSE(0);

	/* Select TX_PAD for RX data (DTE mode...) */
	IOMUXC_UART1_RX_DATA_SELECT_INPUT =
		IOMUXC_UART1_RX_DATA_SELECT_INPUT_DAISY(1);
#endif

	return 0;

}

SYS_INIT(meerakt96_pinmux_init, PRE_KERNEL_1, 0);
