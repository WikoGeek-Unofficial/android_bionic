/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2013. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/fs.h>

#include <mach/mt_spm.h>
#include "core/met_drv.h"
#include "core/trace.h"
#include "spm_twam.h"

extern struct metdevice met_spmtwam;

static char help[] = "  --spmtwam=clock:[speed|normal]        default is normal, normal mode monitors 4 channels, speed mode 2 channels\n"
"  --spmtwam=signal:selx                 selx= 0 ~ 15\n"
"  --spmtwam=scpsys:[0|1]:signal_selx    for axi_idle_to_scpsys, (0=infrasys, signal= 0~15) or (1=perisys, signal = 0~7)\n";

// 2 or 4 event counters
#define MAX_EVENT_COUNT 4
static twam_sig_t twamsig;
static unsigned int twamsig_sel[MAX_EVENT_COUNT];
static int used_count = 0;
static int start = 0;
static bool twam_clock_mode = true; //true:speed mode, false:normal mode

#define SPM_TWAM_FMT1	"%x\n"
#define SPM_TWAM_FMT2	"%x,%x\n"
#define SPM_TWAM_FMT3	"%x,%x,%x\n"
#define SPM_TWAM_FMT4	"%x,%x,%x,%x\n"

#define SPM_TWAM_VAL1	value[0]
#define SPM_TWAM_VAL2	value[0],value[1]
#define SPM_TWAM_VAL3	value[0],value[1],value[2]
#define SPM_TWAM_VAL4	value[0],value[1],value[2],value[3]

/*
 * Called from "met-cmd --start"
 */
static void spmtwam_start(void)
{
	if (twam_clock_mode && used_count > 2) {
		used_count = 2;
	}
	spm_twam_enable_monitor(&twamsig, twam_clock_mode);
	start = 1;
	return;
}

/*
 * Called from "met-cmd --stop"
 */
static void spmtwam_stop(void)
{
	spm_twam_disable_monitor();
	return;
}

static void spmtwam(twam_sig_t *ts)
{

	switch (used_count) {
	case 1:
		MET_PRINTK(SPM_TWAM_FMT1,
			((ts->sig0 & 0x000003ff) * 100) >> 10);
		break;
	case 2:
		MET_PRINTK(SPM_TWAM_FMT2,
			((ts->sig0 & 0x000003ff) * 100) >> 10,
			((ts->sig1 & 0x000003ff) * 100) >> 10);
		break;
	case 3:
		MET_PRINTK(SPM_TWAM_FMT3,
			((ts->sig0 & 0x000003ff) * 100) >> 10,
			((ts->sig1 & 0x000003ff) * 100) >> 10,
			((ts->sig2 & 0x000003ff) * 100) >> 10);
		break;
	case 4:
		MET_PRINTK(SPM_TWAM_FMT4,
			((ts->sig0 & 0x000003ff) * 100) >> 10,
			((ts->sig1 & 0x000003ff) * 100) >> 10,
			((ts->sig2 & 0x000003ff) * 100) >> 10,
			((ts->sig3 & 0x000003ff) * 100) >> 10);
		break;
	}
}

/*
 * Called from "met-cmd --help"
 */
static int spmtwam_print_help(char *buf, int len)
{
	return snprintf(buf, PAGE_SIZE, help);
	return 0;
}

static char header[] = "met-info [000] 0.0: ms_ud_sys_header: spmtwam";

static inline void reset_driver_stat(void)
{
	spm_twam_register_handler(0);
	met_spmtwam.mode = 0;
	used_count = 0;
	start = 0;
}

/*
 * It will be called back when run "met-cmd --extract" and mode is 1
 */
static int spmtwam_print_header(char *buf, int len)
{
	int i, size, total_size;

	size = snprintf(buf, PAGE_SIZE, header);
	total_size = size;
	buf += size;

	for (i=0; i<used_count; i++) {
		size = snprintf(buf, PAGE_SIZE, ",0x%02X:%s(%%)",
				twamsig_sel[i],
				twam_sig[twamsig_sel[i]].name);
		total_size += size;
		buf += size;
	}
	for (i=1; i<=used_count; i++) {
		size = snprintf(buf, PAGE_SIZE, ",x");
		total_size += size;
		buf += size;
	}
	size = snprintf(buf, PAGE_SIZE, "\n");
	total_size += size;
	buf += size;

	size = snprintf(buf, PAGE_SIZE, "met-info [000] 0.0:spmtwam_clock_mode=%s\n",twam_clock_mode == true? "speed":"normal");
	total_size += size;

	reset_driver_stat();
	return total_size;
}

static int parse_num(const char *str, unsigned int *value, int len)
{
	unsigned int i;

	if (len <= 0) {
		return -1;
	}

	if ((len > 2) &&
		((str[0]=='0') &&
		((str[1]=='x') || (str[1]=='X')))) {
		for (i=2; i<len; i++) {
			if (! (((str[i] >= '0') && (str[i] <= '9'))
			   || ((str[i] >= 'a') && (str[i] <= 'f'))
			   || ((str[i] >= 'A') && (str[i] <= 'F')))) {
				return -1;
			}
		}
		sscanf(str, "%x", value);
	} else {
		for (i=0; i<len; i++) {
			if (! ((str[i] >= '0') && (str[i] <= '9'))) {
				return -1;
			}
		}
		sscanf(str, "%d", value);
	}

	return 0;
}

static int assign_slot(unsigned int event)
{
	int i;

	if (used_count == MAX_EVENT_COUNT) {
		return -1;
	}

	//check duplicated
	for (i=0; i<used_count; i++) {
		if  (twamsig_sel[i] == event) {
			return -2;
		}
	}

	twamsig_sel[used_count] = event;
	switch (used_count) {
	case 0:
		twamsig.sig0 = event; break;
	case 1:
		twamsig.sig1 = event; break;
	case 2:
		twamsig.sig2 = event; break;
	case 3:
		twamsig.sig3 = event; break;
	}
	used_count++;

	return 0;
}

/*
 * "  --spmtwam=clock:[speed|normal]          default is normal, normal mode monitors 4 channels, speed mode 2 channels\n"
 * "  --spmtwam=signal:signal_selx            signal_selx= 0 ~ 15\n"
 * "  --spmtwam=scpsys:[0|1]:signal_selx      monitor of axi_idle_to_scpsys, (0=infrasys, signal= 0~15) or (1=perisys, signal = 0~7)\n";
*/

static int spmtwam_process_argument(const char *arg, int len)
{
	unsigned int event;

	if (start == 1) {
		reset_driver_stat();
	}

	if (strncmp(arg, "clock:", 6) == 0) {
		if (strncmp(&(arg[6]), "speed", 5) == 0) {
			twam_clock_mode = true;
		} else if (strncmp(&(arg[6]), "normal", 6) == 0){
			twam_clock_mode = false;
		} else {
			return -1;
		}
	} else if (strncmp(arg, "signal:", 7) == 0) {
		if (parse_num(&(arg[7]), &event, len-7) < 0) {
			return -1;
		}
		if (assign_slot(event) < 0) {
			return -1;
		}
	} else if (strncmp(arg, "scpsys:", 7) == 0) { //TODO:
		event = 0;
		if (arg[7] == '0') {//infrasys

		} else if (arg[7] == '1') {//perisys

		} else {
			return -1;
		}
	} else {
		return -1;
	}

	met_spmtwam.mode = 1;
	return 0;
}

static int spmtwam_create_subfs(struct kobject *parent)
{
	spm_twam_register_handler(spmtwam);
	return 0;
}

static void spmtwam_delete_subfs(void)
{
	spm_twam_register_handler(0);
}

struct metdevice met_spmtwam = {
	.name = "spmtwam",
	.owner = THIS_MODULE,
	.type = MET_TYPE_BUS,
	.cpu_related = 0,
	.create_subfs = spmtwam_create_subfs,
	.delete_subfs = spmtwam_delete_subfs,
	.start = spmtwam_start,
	.stop = spmtwam_stop,
	.print_help = spmtwam_print_help,
	.print_header = spmtwam_print_header,
	.process_argument = spmtwam_process_argument
};

EXPORT_SYMBOL(met_spmtwam);
