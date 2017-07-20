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

#ifndef _SMI_NAME_H_
#define _SMI_NAME_H_

enum SMI_DEST {SMI_DEST_ALL=0, SMI_DEST_EMI=1, SMI_DEST_INTERNAL=3, SMI_DEST_NONE=9};
enum SMI_RW {SMI_RW_ALL=0, SMI_READ_ONLY=1, SMI_WRITE_ONLY=2, SMI_RW_RESPECTIVE=3, SMI_RW_NONE=9};
enum SMI_BUS {SMI_BUS_GMC=0, SMI_BUS_AXI=1, SMI_BUS_NONE=9};
enum SMI_REQUEST {SMI_REQ_ALL=0, SMI_REQ_ULTRA=1, SMI_REQ_PREULTRA=2, SMI_NORMAL_ULTRA=3, SMI_REQ_NONE=9};

/*
typedef struct {
	unsigned long u4Master;   //SMI master 0~3
	unsigned long u4PortNo;
	unsigned long bBusType : 1;//0 for GMC, 1 for AXI
	unsigned long bDestType : 2;//0 for EMI+internal mem, 1 for EMI, 3 for internal mem
	unsigned long bRWType : 2;//0 for R+W, 1 for read, 2 for write
}SMIBMCfg;
*/

struct smi_desc {
	unsigned long port;
	char name[40];
	enum SMI_DEST desttype;
	enum SMI_RW rwtype;
	enum SMI_BUS bustype;
	//enum SMI_REQUEST requesttype;
};

/* mt6582 */
struct smi_desc larb0_desc[] = {
	{ 0,	"DISP_OVL_0",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 1,	"DISP_RDMA",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 2,	"DISP_WDMA",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 3,	"MM_CMDQ",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 4,	"MDP_RDMA",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 5,	"MDP_WDMA",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 6,	"MDP_ROTO",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 7,	"MDP_ROTCO",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 8,	"MDP_ROTVO",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC }
};
struct smi_desc larb1_desc[] = {
	{ 0,	"HW_VDEC_MC_EXT",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 1,	"HW_VDEC_PP_EXT",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 2,	"HW_VDEC_AVC_MV_EXT",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 3,	"HW_VDEC_PRED_RD_EXT",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 4,	"HW_VDEC_PRED_WR_EXT",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 5,	"HW_VDEC_VLD_EXT",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 6,	"HW_VDEC_PPWRAP_EXT",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC }
};
struct smi_desc larb2_desc[] = {
	{ 0,	"CAM_IMGO",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 1,	"CAM_IMG2O",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 2,	"CAM_LSCI",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 3,	"CAM_IMGI",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 4,	"CAM_ESFKO",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 5,	"CAM_AAO",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 6,	"JPGENC_RDMA",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 7,	"JPGENC_BSDMA",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 8,	"VENC_RD_COMV",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 9,	"VEND_SV_COMV",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 10,	"VENC_RCPU",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 11,	"VENC_REC_FRM",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 12,	"VENC_REF_LUMA",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 13,	"VENC_REF_CHROMA",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 14,	"VENC_BSDMA",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 15,	"VENC_CUR_LUMA",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC },
	{ 16,	"VENC_CUR_CHROMA",	SMI_DEST_ALL, SMI_RW_ALL, SMI_BUS_GMC }
};
struct smi_desc common_desc[] = {
	{ 0,	"AXI_SMI_1",	SMI_DEST_EMI, SMI_RW_RESPECTIVE, SMI_BUS_NONE },
	{ 1,	"AXI_LARB0",	SMI_DEST_EMI, SMI_RW_RESPECTIVE, SMI_BUS_NONE },
	{ 2,	"AXI_LARB1",	SMI_DEST_EMI, SMI_RW_RESPECTIVE, SMI_BUS_NONE },
	{ 3,	"AXI_LARB2",	SMI_DEST_EMI, SMI_RW_RESPECTIVE, SMI_BUS_NONE },
	{ 4,	"AXI_G3D",	SMI_DEST_EMI, SMI_RW_RESPECTIVE, SMI_BUS_NONE }
};

#define SMI_LARB0_DESC_COUNT (sizeof(larb0_desc) / sizeof(struct smi_desc))
#define SMI_LARB1_DESC_COUNT (sizeof(larb1_desc) / sizeof(struct smi_desc))
#define SMI_LARB2_DESC_COUNT (sizeof(larb2_desc) / sizeof(struct smi_desc))
#define SMI_COMMON_DESC_COUNT (sizeof(common_desc) / sizeof(struct smi_desc))
#define SMI_ALLPORT_COUNT (SMI_LARB0_DESC_COUNT+SMI_LARB1_DESC_COUNT+SMI_LARB2_DESC_COUNT+SMI_COMMON_DESC_COUNT)

#endif // _SMI_NAME_H_
