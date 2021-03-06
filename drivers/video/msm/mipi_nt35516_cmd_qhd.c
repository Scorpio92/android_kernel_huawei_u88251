/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "msm_fb.h"
#include "mipi_dsi.h"
#include "hw_lcd_common.h"

#define LCD_DEVICE_NAME "mipi_cmd_nt35516_qhd_pt"

static lcd_panel_type lcd_panel_qhd = LCD_NONE;
/* reduce the rate of mipi transmit */
static struct mipi_dsi_phy_ctrl dsi_cmd_mode_phy_db = {
	/* DSI Bit Clock at 419 MHz, 2 lane, RGB888 */
	/* regulator */
	{0x03, 0x01, 0x01, 0x00, 0x00}, 
	/* timing */ 
	{0x79, 0x2E, 0x10, 0x00, 0x3E, 0x45, 0x15, 0x32, 
	0x14, 0x3, 0x04, 0x00}, 
	/* phy ctrl */ 
	{0x7f, 0x00, 0x00, 0x00}, 
	/* strength */ 
	{0xbb, 0x02, 0x06, 0x00}, 
	/* pll control */ 
	{0x01, 0x9e, 0x31, 0xd2, 0x00, 0x40, 0x37, 0x62, 
	0x01, 0x0f, 0x07, 
	0x05, 0x14, 0x03, 0x0, 0x0, 0x0, 0x20, 0x0, 0x02, 0x0}, 
};

static struct dsi_buf nt35516_tx_buf;
static struct sequence * nt35516_lcd_init_table_debug = NULL;

/*LCD init code*/
static const struct sequence nt35516_hvga_standby_enter_table[]= 
{
	/*set the delay time 100ms*/
	/* Fixed LCD Flicker */
	//{0x0021,MIPI_DCS_COMMAND,0},
	{0x00028,MIPI_DCS_COMMAND,0}, //28h
	{0x00010,MIPI_DCS_COMMAND,20},
	{0x00029,MIPI_TYPE_END,150}, // add new command for 
};
/* let the reset go ,so remove this code */
static const struct sequence nt35516_hvga_standby_exit_table[]= 
{
	/*set the delay time 100ms*/
	{0x00011,MIPI_DCS_COMMAND,0}, //29h
	{0x00029,MIPI_DCS_COMMAND,120},
	/* Fixed LCD Flicker */
	//{0x0020,MIPI_DCS_COMMAND,20},
	{0x00029,MIPI_TYPE_END,20}, // add new command for 
};
/*lcd resume function*/

/*add dynamic gamma for G600 tiamma LCD*/
#ifdef CONFIG_FB_DYNAMIC_GAMMA
static const struct sequence tianma_nt35516_gamma_25[]= 
{
	/* Modify LCD init code in order to diaplay normal for modified mask panel */
	{0x000F0,MIPI_GEN_COMMAND,0},//page1
	{0x00055,TYPE_PARAMETER,0},
	{0x000AA,TYPE_PARAMETER,0},
	{0x00052,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x000D1,MIPI_GEN_COMMAND,0},//gamma25
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00074,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000C9,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00016,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0003E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00066,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},//8
	{0x000D2,MIPI_GEN_COMMAND,0},//D2
	{0x00001,TYPE_PARAMETER,0},
	{0x000B8,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000E5,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x0001B,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x0005D,TYPE_PARAMETER,0},//12 5f
	{0x00002,TYPE_PARAMETER,0},
	{0x00093,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000C4,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000D3,MIPI_GEN_COMMAND,0},//D3
	{0x00003,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00044,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00060,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00082,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00098,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000B5,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000DB,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F0,TYPE_PARAMETER,0},
	{0x000D4,MIPI_GEN_COMMAND,0},//D4
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000D5,MIPI_GEN_COMMAND,0},//gamma D5
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00074,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000C9,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00016,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0003E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00066,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},
	{0x000D6,MIPI_GEN_COMMAND,0},//D6
	{0x00001,TYPE_PARAMETER,0},
	{0x000B8,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000E5,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x0001B,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x0005D,TYPE_PARAMETER,0},//12 5f
	{0x00002,TYPE_PARAMETER,0},
	{0x00093,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000C4,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000D7,MIPI_GEN_COMMAND,0},//D7
	{0x00003,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00044,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00060,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00082,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00098,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000B5,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000DB,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F0,TYPE_PARAMETER,0},
	{0x000D8,MIPI_GEN_COMMAND,0},//D8
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000D9,MIPI_GEN_COMMAND,0},//gamma D9
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00074,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000C9,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00016,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0003E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00066,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},
	{0x000DD,MIPI_GEN_COMMAND,0},//DD
	{0x00001,TYPE_PARAMETER,0},
	{0x000B8,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000E5,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x0001B,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x0005D,TYPE_PARAMETER,0},//12 5f
	{0x00002,TYPE_PARAMETER,0},
	{0x00093,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000C4,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000DE,MIPI_GEN_COMMAND,0},//DE
	{0x00003,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00044,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00060,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00082,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00098,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000B5,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000DB,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F0,TYPE_PARAMETER,0},
	{0x000DF,MIPI_GEN_COMMAND,0},//DF
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000E0,MIPI_GEN_COMMAND,0},//gamma E0
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00074,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000C9,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00016,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0003E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00066,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},
	{0x000E1,MIPI_GEN_COMMAND,0},//E1
	{0x00001,TYPE_PARAMETER,0},
	{0x000B8,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000E5,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x0001B,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x0005D,TYPE_PARAMETER,0},//12 
	{0x00002,TYPE_PARAMETER,0},
	{0x00093,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000C4,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000E2,MIPI_GEN_COMMAND,0},//E2
	{0x00003,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00044,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00060,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00082,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00098,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000B5,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000DB,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F0,TYPE_PARAMETER,0},
	{0x000E3,MIPI_GEN_COMMAND,0},//E3
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000E4,MIPI_GEN_COMMAND,0},//gamma E4
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00074,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000C9,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00016,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0003E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00066,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},
	{0x000E5,MIPI_GEN_COMMAND,0},//E5
	{0x00001,TYPE_PARAMETER,0},
	{0x000B8,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000E5,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x0001B,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x0005D,TYPE_PARAMETER,0},//12 5f
	{0x00002,TYPE_PARAMETER,0},
	{0x00093,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000C4,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000E6,MIPI_GEN_COMMAND,0},//E6
	{0x00003,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00044,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00060,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00082,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00098,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000B5,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000DB,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F0,TYPE_PARAMETER,0},
	{0x000E7,MIPI_GEN_COMMAND,0},//E7
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000E8,MIPI_GEN_COMMAND,0},//gamma E8
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00074,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000C9,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00016,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0003E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00066,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},
	{0x000E9,MIPI_GEN_COMMAND,0},//E9
	{0x00001,TYPE_PARAMETER,0},
	{0x000B8,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000E5,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x0001B,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x0005D,TYPE_PARAMETER,0},//12 5f
	{0x00002,TYPE_PARAMETER,0},
	{0x00093,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000C4,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000EA,MIPI_GEN_COMMAND,0},//EA
	{0x00003,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00044,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00060,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00082,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00098,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000B5,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000DB,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F0,TYPE_PARAMETER,0},
	{0x000EB,MIPI_GEN_COMMAND,0},//EB
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0xFFFFF,MIPI_TYPE_END,0},
};

static const struct sequence tianma_nt35516_gamma_22[]= 
{
	/* Modify LCD init code in order to diaplay normal for modified mask panel */
	{0x000F0,MIPI_GEN_COMMAND,0},//page1
	{0x00055,TYPE_PARAMETER,0},
	{0x000AA,TYPE_PARAMETER,0},
	{0x00052,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x000D1,MIPI_GEN_COMMAND,0},//gamma22
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0006B,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000BF,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000F3,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00017,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0003E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00053,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0008A,TYPE_PARAMETER,0},//8
	{0x000D2,MIPI_GEN_COMMAND,0},//D2
	{0x00001,TYPE_PARAMETER,0},
	{0x000AC,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x000E0,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x0000B,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x0004D,TYPE_PARAMETER,0},//12
	{0x00002,TYPE_PARAMETER,0},
	{0x00083,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00084,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000B4,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x000D3,MIPI_GEN_COMMAND,0},//D3
	{0x00003,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0002E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00043,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00067,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0007D,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00097,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000AC,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C4,TYPE_PARAMETER,0},
	{0x000D4,MIPI_GEN_COMMAND,0},//D4
	{0x00003,TYPE_PARAMETER,0},
	{0x000D9,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000D5,MIPI_GEN_COMMAND,0},//gamma D5
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0006B,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000BF,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000F3,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00017,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0003E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00053,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0008A,TYPE_PARAMETER,0},//8
	{0x000D6,MIPI_GEN_COMMAND,0},//D6
	{0x00001,TYPE_PARAMETER,0},
	{0x000AC,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x000E0,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x0000B,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x0004D,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00083,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00084,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000B4,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x000D7,MIPI_GEN_COMMAND,0},//D7
	{0x00003,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0002E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00043,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00067,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0007D,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00097,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000AC,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C4,TYPE_PARAMETER,0},
	{0x000D8,MIPI_GEN_COMMAND,0},//D8
	{0x00003,TYPE_PARAMETER,0},
	{0x000D9,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000D9,MIPI_GEN_COMMAND,0},//gamma D9
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0006B,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000BF,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000F3,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00017,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0003E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00053,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0008A,TYPE_PARAMETER,0},
	{0x000DD,MIPI_GEN_COMMAND,0},//DD
	{0x00001,TYPE_PARAMETER,0},
	{0x000AC,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x000E0,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x0000B,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x0004D,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00083,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00084,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000B4,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x000DE,MIPI_GEN_COMMAND,0},//DE
	{0x00003,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0002E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00043,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00067,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0007D,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00097,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000AC,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C4,TYPE_PARAMETER,0},
	{0x000DF,MIPI_GEN_COMMAND,0},//DF
	{0x00003,TYPE_PARAMETER,0},
	{0x000D9,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000E0,MIPI_GEN_COMMAND,0},//gamma E0
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0006B,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000BF,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000F3,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00017,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0003E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00053,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0008A,TYPE_PARAMETER,0},
	{0x000E1,MIPI_GEN_COMMAND,0},//E1
	{0x00001,TYPE_PARAMETER,0},
	{0x000AC,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x000E0,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x0000B,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x0004D,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00083,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00084,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000B4,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x000E2,MIPI_GEN_COMMAND,0},//E2
	{0x00003,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0002E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00043,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00067,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0007D,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00097,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000AC,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C4,TYPE_PARAMETER,0},
	{0x000E3,MIPI_GEN_COMMAND,0},//E3
	{0x00003,TYPE_PARAMETER,0},
	{0x000D9,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000E4,MIPI_GEN_COMMAND,0},//gamma E4
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0006B,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000BF,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000F3,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00017,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0003E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00053,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0008A,TYPE_PARAMETER,0},
	{0x000E5,MIPI_GEN_COMMAND,0},//E5
	{0x00001,TYPE_PARAMETER,0},
	{0x000AC,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x000E0,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x0000B,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x0004D,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00083,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00084,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000B4,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x000E6,MIPI_GEN_COMMAND,0},//E6
	{0x00003,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0002E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00043,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00067,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0007D,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00097,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000AC,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C4,TYPE_PARAMETER,0},
	{0x000E7,MIPI_GEN_COMMAND,0},//E7
	{0x00003,TYPE_PARAMETER,0},
	{0x000D9,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000E8,MIPI_GEN_COMMAND,0},//gamma E8
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0006B,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000BF,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000F3,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00017,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0003E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00053,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0008A,TYPE_PARAMETER,0},
	{0x000E9,MIPI_GEN_COMMAND,0},//E9
	{0x00001,TYPE_PARAMETER,0},
	{0x000AC,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x000E0,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x0000B,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x0004D,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00083,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00084,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000B4,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x000EA,MIPI_GEN_COMMAND,0},//EA
	{0x00003,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0002E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00043,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00067,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0007D,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00097,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000AC,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C4,TYPE_PARAMETER,0},
	{0x000EB,MIPI_GEN_COMMAND,0},//EB
	{0x00003,TYPE_PARAMETER,0},
	{0x000D9,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0xFFFFF,MIPI_TYPE_END,0},
};
/*Interface for dynamic gamma*/
int mipi_nt35516_set_dynamic_gamma(enum danymic_gamma_mode  gamma_mode,struct msm_fb_data_type *mfd)
{
	int ret = 0;
	switch(gamma_mode)
	{
		case GAMMA25:
			process_mipi_table(mfd,&nt35516_tx_buf,(struct sequence*)&tianma_nt35516_gamma_25,
					ARRAY_SIZE(tianma_nt35516_gamma_25), lcd_panel_qhd);
			break ;
		case GAMMA22:
			process_mipi_table(mfd,&nt35516_tx_buf,(struct sequence*)&tianma_nt35516_gamma_22,
					ARRAY_SIZE(tianma_nt35516_gamma_22), lcd_panel_qhd);
			break;
		default:
			LCD_DEBUG("%s: invalid dynamic_gamma: %d\n", __func__,gamma_mode);
			ret = -EINVAL;
			break;
	}
	LCD_DEBUG("%s: change gamma mode to %d\n",__func__,gamma_mode);
	return ret;
}
#endif
static int mipi_nt35516_lcd_on(struct platform_device *pdev)
{
	
	/*delete some lines */
	boolean para_debug_flag = FALSE;
	uint32 para_num = 0;
	struct msm_fb_data_type *mfd;
	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL; 

	/*delete some lines */
	para_debug_flag = lcd_debug_malloc_get_para( "nt35516_lcd_init_table_debug", 
            (void**)&nt35516_lcd_init_table_debug,&para_num);

    if( (TRUE == para_debug_flag) && (NULL != nt35516_lcd_init_table_debug))
    {
        process_mipi_table(mfd,&nt35516_tx_buf,nt35516_lcd_init_table_debug,
		     para_num, lcd_panel_qhd);
    }
	else
	{
		//mipi_set_tx_power_mode(1);
		process_mipi_table(mfd,&nt35516_tx_buf,(struct sequence*)&nt35516_hvga_standby_exit_table,
		 	ARRAY_SIZE(nt35516_hvga_standby_exit_table), lcd_panel_qhd);
		//mipi_set_tx_power_mode(0);
		/*delete some lines */
	}

	if((TRUE == para_debug_flag)&&(NULL != nt35516_lcd_init_table_debug))
	{
		lcd_debug_free_para((void *)nt35516_lcd_init_table_debug);
	}
	
	pr_info("leave mipi_nt35516_lcd_on \n");
	return 0;
}
/*lcd suspend function*/
static int mipi_nt35516_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	process_mipi_table(mfd,&nt35516_tx_buf,(struct sequence*)&nt35516_hvga_standby_enter_table,
		 ARRAY_SIZE(nt35516_hvga_standby_enter_table), lcd_panel_qhd);
	pr_info("leave mipi_nt35516_lcd_off \n");
	return 0;
}
#ifdef CONFIG_FB_AUTO_CABC
static struct sequence nt35516_auto_cabc_set_table[] =
{
	{0x00055,MIPI_GEN_COMMAND,0}, 
	{0x00001,TYPE_PARAMETER,0},
	/*remove diming function of lcd,let it work in video */
	{0x00053,MIPI_GEN_COMMAND,0}, 
	{0x00024,TYPE_PARAMETER,0},
	{0xFFFFF,MIPI_TYPE_END,0}, //the end flag,it don't sent to driver IC
};

/***************************************************************
Function: nt35516_config_cabc
Description: Set CABC configuration
Parameters:
	struct msmfb_cabc_config cabc_cfg: CABC configuration struct
Return:
	0: success
***************************************************************/
/*remove diming function of lcd,let it work in video */
static int nt35516_config_auto_cabc(struct msmfb_cabc_config cabc_cfg,struct msm_fb_data_type *mfd)
{
	int ret = 0;

	switch(cabc_cfg.mode)
	{
		case CABC_MODE_UI:
			nt35516_auto_cabc_set_table[1].reg=0x00001;
			nt35516_auto_cabc_set_table[3].reg=0x00024;
			break;
		case CABC_MODE_MOVING:
		case CABC_MODE_STILL:
			nt35516_auto_cabc_set_table[1].reg=0x00003;
			nt35516_auto_cabc_set_table[3].reg=0x0002c;
			break;
		default:
			LCD_DEBUG("%s: invalid cabc mode: %d\n", __func__, cabc_cfg.mode);
	        ret = -EINVAL;
			break;
	}
	if(likely(0 == ret))
	{
		process_mipi_table(mfd,&nt35516_tx_buf,(struct sequence*)&nt35516_auto_cabc_set_table,
			 ARRAY_SIZE(nt35516_auto_cabc_set_table), lcd_panel_qhd);
	}

	LCD_DEBUG("%s: change cabc mode to %d\n",__func__,cabc_cfg.mode);
	return ret;
}
#endif // CONFIG_FB_AUTO_CABC

static int __devinit mipi_nt35516_lcd_probe(struct platform_device *pdev)
{
	msm_fb_add_device(pdev);

	return 0;
}

static struct sequence nt35516_cabc_enable_table[] =
{	
	{0x00051,MIPI_DCS_COMMAND,0}, 		
	{0x000ff,TYPE_PARAMETER,0},
	
	{0x00029,MIPI_TYPE_END,0},
};
void nt35516_set_cabc_backlight(struct msm_fb_data_type *mfd,uint32 bl_level)
{	
	nt35516_cabc_enable_table[1].reg = bl_level; // 1 will be changed if modify init code

	process_mipi_table(mfd,&nt35516_tx_buf,(struct sequence*)&nt35516_cabc_enable_table,
		 ARRAY_SIZE(nt35516_cabc_enable_table), lcd_panel_qhd);
}

static struct platform_driver this_driver = {
	.probe  = mipi_nt35516_lcd_probe,
	.driver = {
		.name   = LCD_DEVICE_NAME,
	},
};
static struct msm_fb_panel_data nt35516_panel_data = {
	.on					= mipi_nt35516_lcd_on,
	.off					= mipi_nt35516_lcd_off,
	.set_backlight 		= pwm_set_backlight,
	/*add cabc control backlight*/
	.set_cabc_brightness 	= nt35516_set_cabc_backlight,
#ifdef CONFIG_FB_AUTO_CABC
	.config_cabc = nt35516_config_auto_cabc,
#endif
#ifdef CONFIG_FB_DYNAMIC_GAMMA
	.set_dynamic_gamma = mipi_nt35516_set_dynamic_gamma,
#endif
};
static struct platform_device this_device = {
	.name   = LCD_DEVICE_NAME,
	.id	= 0,
	.dev	= {
		.platform_data = &nt35516_panel_data,
	}
};

static int mipi_cmd_nt35516_qhd_pt_init(void)
{
	int ret = 0;
	struct msm_panel_info *pinfo = NULL;
	
	lcd_panel_qhd = get_lcd_panel_type();
	if (MIPI_NT35516_TIANMA_QHD != lcd_panel_qhd &&
		MIPI_NT35516_CHIMEI_QHD != lcd_panel_qhd)
	{
		return 0;
	}
	pr_info("enter mipi_cmd_nt35516_qhd_pt_init \n");
	mipi_dsi_buf_alloc(&nt35516_tx_buf, DSI_BUF_SIZE);

	ret = platform_driver_register(&this_driver);
	if (!ret)
	{
	 	pinfo = &nt35516_panel_data.panel_info;
		pinfo->xres = 540;
		pinfo->yres = 960;
		pinfo->type = MIPI_CMD_PANEL;
		pinfo->pdest = DISPLAY_1;
		pinfo->wait_cycle = 0;
		pinfo->bpp = 24;		
		pinfo->bl_max = 255;
		pinfo->bl_min = 30;		
		pinfo->fb_num = 3;
		/* increase the rate of mipi transmit */
		/* reduce the rate of mipi transmit */
		pinfo->clk_rate = 419000000;
		pinfo->lcd.refx100 = 6000; /* adjust refx100 to prevent tearing */

		pinfo->mipi.mode = DSI_CMD_MODE;
		pinfo->mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
		pinfo->mipi.vc = 0;
		pinfo->mipi.rgb_swap = DSI_RGB_SWAP_RGB;
		pinfo->mipi.data_lane0 = TRUE;
		pinfo->mipi.data_lane1 = TRUE;
		pinfo->mipi.t_clk_post = 0x7f;
		pinfo->mipi.t_clk_pre = 0x2f;
		pinfo->mipi.stream = 0; /* dma_p */
		pinfo->mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
		pinfo->mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
		/*set hw te sync*/
		pinfo->lcd.hw_vsync_mode = TRUE;
		pinfo->lcd.vsync_enable = TRUE;
		pinfo->mipi.te_sel = 1; /* TE from vsync gpio */
		pinfo->mipi.interleave_max = 1;
		pinfo->mipi.insert_dcs_cmd = TRUE;
		pinfo->mipi.wr_mem_continue = 0x3c;
		pinfo->mipi.wr_mem_start = 0x2c;
		pinfo->mipi.dsi_phy_db = &dsi_cmd_mode_phy_db;
		pinfo->mipi.tx_eot_append = 0x01;
		pinfo->mipi.rx_eot_ignore = 0;
		pinfo->mipi.dlane_swap = 0x1;

		ret = platform_device_register(&this_device);
		if (ret)
			pr_err("%s: failed to register device!\n", __func__);
	}
	return ret;
}

module_init(mipi_cmd_nt35516_qhd_pt_init);
