/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2012. All rights reserved.
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

#include "core/met_drv.h"
#include "core/trace.h"


#define MS_EMI_FMT	"%5lu.%06lu"
#define MS_EMI_VAL	(unsigned long)(timestamp), nano_rem/1000
void ms_emi(unsigned long long timestamp, unsigned char cnt, unsigned int *value)
{
	unsigned long nano_rem = do_div(timestamp, 1000000000);
	switch (cnt) {
	case 15: MET_PRINTK(MS_EMI_FMT FMT15, MS_EMI_VAL VAL15); break;
	}
}

#define MS_SMI_FMT	"%5lu.%06lu"
#define MS_SMI_VAL	(unsigned long)(timestamp), nano_rem/1000
void ms_smi(unsigned long long timestamp, unsigned char cnt, unsigned int *value)
{
	unsigned long nano_rem = do_div(timestamp, 1000000000);
	switch (cnt) {
	case 18: MET_PRINTK(MS_SMI_FMT FMT18, MS_SMI_VAL VAL18); break;
	case 30: MET_PRINTK(MS_SMI_FMT FMT30, MS_SMI_VAL VAL30); break;
	case 34: MET_PRINTK(MS_SMI_FMT FMT34, MS_SMI_VAL VAL34); break;
	case 37: MET_PRINTK(MS_SMI_FMT FMT37, MS_SMI_VAL VAL37); break;
	case 44: MET_PRINTK(MS_SMI_FMT FMT44, MS_SMI_VAL VAL44); break;
	case 50: MET_PRINTK(MS_SMI_FMT FMT50, MS_SMI_VAL VAL50); break;
	case 66: MET_PRINTK(MS_SMI_FMT FMT66, MS_SMI_VAL VAL66); break;
	case 82: MET_PRINTK(MS_SMI_FMT FMT82, MS_SMI_VAL VAL82); break;
	}
}

void ms_smit(unsigned long long timestamp, unsigned char cnt, unsigned int *value)
{
	unsigned long nano_rem = do_div(timestamp, 1000000000);
	switch (cnt) {
	case 10: MET_PRINTK(MS_SMI_FMT FMT10, MS_SMI_VAL VAL10); break;
	case 19: MET_PRINTK(MS_SMI_FMT FMT19, MS_SMI_VAL VAL19); break;
	case 14: MET_PRINTK(MS_SMI_FMT FMT14, MS_SMI_VAL VAL14); break;
	}
}

#define MS_TH_FMT	"%5lu.%06lu"
#define MS_TH_VAL	(unsigned long)(timestamp), nano_rem/1000
#define MS_TH_UD_FMT4	",%d,%d,%d,%d\n"
#define MS_TH_UD_VAL4	,value[0],value[1],value[2],value[3]
#define MS_TH_UD_FMT7	",%d,%d,%d,%d,%d,%d,%d\n"
#define MS_TH_UD_VAL7	,value[0],value[1],value[2],value[3],value[4],value[5],value[6]

void ms_th(unsigned long long timestamp, unsigned char cnt, unsigned int *value)
{
	unsigned long nano_rem = do_div(timestamp, 1000000000);
	switch (cnt) {
	case 4: MET_PRINTK(MS_TH_FMT MS_TH_UD_FMT4, MS_TH_VAL MS_TH_UD_VAL4); break;
	case 7: MET_PRINTK(MS_TH_FMT MS_TH_UD_FMT7, MS_TH_VAL MS_TH_UD_VAL7); break;
	}
}

#define MS_DRAMC_UD_FMT	"%x,%x,%x,%x\n"
#define MS_DRAMC_UD_VAL	value[0],value[1],value[2],value[3]
void ms_dramc(unsigned long long timestamp, unsigned char cnt, unsigned int *value)
{
	switch (cnt) {
	case 4: MET_PRINTK(MS_DRAMC_UD_FMT, MS_DRAMC_UD_VAL); break;
	}
}
