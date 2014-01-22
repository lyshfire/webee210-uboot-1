/*
 * common/cmd_usbd.c
 *
 *  $Id: cmd_usbd.c,v 1.2 2009/01/28 00:11:42 dark0351 Exp $
 *
 * (C) Copyright 2007
 * Byungjae Lee, Samsung Erectronics, bjlee@samsung.com.
 *	- support for S3C2412, S3C2443 and S3C6400
 *
 * (C) Copyright SAMSUNG Electronics
 *      SW.LEE  <hitchcar@samsung.com>
 *      - add USB device fo S3C2440A, S3C24A0A
 *
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

/*
 * Memory Functions
 *
 * Copied from FADS ROM, Dan Malek (dmalek@jlc.net)
 */

#include <common.h>
#include <command.h>

#ifdef CONFIG_S3C_USBD

/*��ʼ�޸�DNW-USB�ĺ궨�� make by nietao email:nietaooldman@126.com*/

#define USBOTG_LINK_BASE		(0xEC000000)
#define USBOTG_PHY_BASE			(0xEC100000)

#define GINTSTS_WkUpInt		(1<<31)
#define GINTSTS_OEPInt		(1<<19)
#define GINTSTS_IEPInt		(1<<18)
#define GINTSTS_EnumDone	(1<<13)
#define GINTSTS_USBRst		(1<<12)
#define GINTSTS_USBSusp		(1<<11)
#define GINTSTS_RXFLvl		(1<<4)
#define BIT_ALLMSK			(0xFFFFFFFF)

#define S5P_OTG_GINTSTS 		(USBOTG_LINK_BASE + 0x014) /* R/W Core Interrupt Register */

/*�޸Ľ���*/
#if defined(CONFIG_S3C2412) || defined(CONFIG_S3C2442)
#include "../cpu/s3c24xx/usbd-fs.h"
#elif defined(CONFIG_S3C2443) || defined(CONFIG_S3C2450) || defined(CONFIG_S3C2416)
#include "../cpu/s3c24xx/usbd-hs.h"
#elif defined(CONFIG_S3C6400) || defined(CONFIG_S3C6410) || defined(CONFIG_S3C6430)
#include "../cpu/s3c64xx/usbd-otg-hs.h"
#elif defined(CONFIG_S5PC100)
#include "../cpu/s5pc1xx/usbd-otg-hs.h"
#elif defined(CONFIG_S5PC110)
#include "../arch/arm/cpu/armv7/usbd-otg-hs.h" /* Ϊs5pc110���� */
#elif defined(CONFIG_WEBEE210)
#include "../arch/arm/cpu/armv7/usbd-otg-hs.h" /*Ϊwebee210����*/
#elif defined(CONFIG_S5P6440)
#include "../cpu/s5p64xx/usbd-otg-hs.h"
#elif defined(CONFIG_S5P6442)
#include "../cpu/s5p644x/usbd-otg-hs.h"
#else
#error "* CFG_ERROR : you have to setup right Samsung CPU configuration"
#endif

#undef	CMD_USBD_DEBUG
#ifdef	CMD_USBD_DEBUG
#define	PRINTF(fmt,args...)	printf (fmt ,##args)
#else
#define PRINTF(fmt,args...)
#endif

static const char pszMe[] = "usbd: ";

int do_usbd_dnw ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	if (argv[0][0] == 'u') {
		DNW = 0;
	}
	else {
		DNW = 1;
		s3c_got_header = 0;
	}

	switch (argc) {
	case 1 :
		s3c_usbd_dn_addr = USBD_DOWN_ADDR;	/* Default Address */
		break;
	case 2 :
		s3c_usbd_dn_addr = simple_strtoul(argv[1], NULL, 16);
		break;
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	s3c_receive_done = 0;

	s3c_usbctl_init();
	s3c_usbc_activate();

	PRINTF("Download address 0x%08x\n", s3c_usbd_dn_addr);
	printf("Now, Waiting for DNW to transmit data\n");

	while (1) {
		if (S3C_USBD_DETECT_IRQ()) {
			s3c_udc_int_hndlr();
			S3C_USBD_CLEAR_IRQ();
		}

		if (s3c_receive_done)
			break;

#ifndef CONFIG_SMART210
		if (serial_tstc()) {
			serial_getc();
			break;
		}
#endif /* CONFIG_smart210 */
	}

	/* when operation is done, usbd must be stopped */
	s3c_usb_stop();

	return 0;
}

#if 0 /* ud command not support yet */
U_BOOT_CMD(
	ud, 3, 0, do_usbd_dnw,
	"ud      - initialize USB device and ready to receive for LINUX server (specific)\n",
	"[download address]\n"
);
#endif

U_BOOT_CMD(
	dnw, 3, 0, do_usbd_dnw,
	"dnw     - initialize USB device and ready to receive for Windows server (specific)\n",
	"[download address]\n"
);

#endif	/* CONFIG_S3C_USBD */
