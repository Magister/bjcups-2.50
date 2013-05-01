/*  Canon Bubble Jet Print Filter.
 *  Copyright CANON INC. 2001-2005
 *  All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define SM_SHOW_TIMEOUT_ms	1000

#define SM_INIT_BJF850	"SSR=BST,SFA,CHD,CIL,CIR,LVR,DBS,DWS,DOC,DSC,DJS;"

// #define SM_COMBUF_SIZ	256

// #define ICON_INK_GAP	20
// #define ICON_Y		(-18)
// #define ICON_X		(+4)
// #define X_BK		(17*0)
// #define X_Y	  		(17*5)
// #define X_M  		(17*4)
// #define X_C 		(17*3)
// #define X_PM		(17*2)
// #define X_PC		(17*1)
// #define INK_X  		(+7)
// #define INK_Y  		(+8)

#define DSC_USEDTANK_FULL		"5B00"

#define DBS_CLEANING			"CL"
#define DBS_CARTRIDGE_CHANGE	"CC"
#define DBS_TEST_PRINT			"TP"
#define DBS_PRINTER_USED		"DS"
#define DBS_ENDING				"SL"

#define STS_NO					"NO"
#define STS_UNKOWN				"UK"

#define DOC_PAPER_OUT			"1000"
#define DOC_PAPER_OUT03			"1003"
#define DOC_COVER_OPEN			"1200"
#define DOC_COVER_CLOSE			"1250"
#define DOC_PAPER_JAM			"1300"
#define DOC_PAPER_JAM03			"1303"
#define DOC_PAPER_JAM04			"1304"
#define DOC_CARTRIDGE_NOTHING	"1401"
#define DOC_SCANNER_DETECT		"1440"
#define DOC_PROTECTOR			"1441"	// F900
#define DOC_SCANNER_ERROR		"1442"	// F900
#define DOC_UNSUPORTED_CHD		"1485"
#define DOC_INKOUT_PRE			"16"
#define DOC_INKOUT_BLACK		"1601"
#define DOC_INKOUT_BLACK_S		"1602"	// SENRYO Black		860i
#define DOC_INKOUT_YELLOW		"1611"
#define DOC_INKOUT_MAGENTA		"1612"
#define DOC_INKOUT_CYAN			"1613"
#define DOC_INKOUT_RED			"1614"	// 990i
#define DOC_INKOUT_GREEN		"1615"	// ip8600
#define DOC_INKOUT_PHOTO_M		"1634"
#define DOC_INKOUT_PHOTO_C		"1635"
#define DOC_INKTANK_NOTHING		"1660"	// i250 i255
#define DOC_USEDTANK_ALMOST		"1700"
#define DOC_USEDTANK_ALMOST2	"1710"	// i250/i255
#define DOC_CDR_GUIDE_OPEN		"1841"	// v2.5
#define DOC_CDR_GUIDE_OPEN_P	"1846"	// v2.5
#define DOC_CDR_GUIDE_ON		"1851"
#define DOC_CDR_GUIDE_ON_P		"1856"	// v2.5
#define DOC_USBDEVICE 			"2001"	// Ver.2.4
#define DOC_HEADALIGNMENT		"2500"	// 990i

#define DWS_INKLOW_PRE			"15"
#define DWS_INKLOW_BLACK		"1501"
#define DWS_INKLOW_BLACK_S		"1502"	// SENRYO Black 	860i
#define DWS_INKLOW_YELLOW		"1511"
#define DWS_INKLOW_MAGENTA		"1512"
#define DWS_INKLOW_CYAN			"1513"
#define DWS_INKLOW_RED			"1514"	// 990i
#define DWS_INKLOW_GREEN		"1515"	// ip8600
#define DWS_INKLOW_PHOTO_M		"1534"
#define DWS_INKLOW_PHOTO_C		"1535"
#define DWS_INKLOW_24BLACK1		"1561"	// S300
#define DWS_INKLOW_24BLACK3		"1541"	// S300
#define DWS_INKLOW_24COLOR1		"1562"	// S300
#define DWS_INKLOW_24COLOR2		"1510"	// S300
#define DWS_INKLOW_24COLOR3		"1542"	// S300

#define DWS_INKLOW070_BLACK		"1507"	// 70% ink Not Printer SPEC
#define DWS_INKLOW070_YELLOW	"1571"	// Not Printer SPEC
#define DWS_INKLOW070_MAGENTA	"1572"	// Not Printer SPEC
#define DWS_INKLOW070_CYAN		"1573"	// Not Printer SPEC
#define DWS_INKLOW070_PHOTO_M	"1574"	// Not Printer SPEC
#define DWS_INKLOW070_PHOTO_C	"1575"	// Not Printer SPEC
#define DWS_INKLOW070_RED		"1576"	// Not Printer SPEC		990i
#define DWS_INKLOW070_GREEN		"1577"	// Not Printer SPEC		ip8600

#define DWS_INKLOW040_BLACK		"1508"	// 40% ink Not Printer SPEC
#define DWS_INKLOW040_YELLOW	"1581"	// Not Printer SPEC
#define DWS_INKLOW040_MAGENTA	"1582"	// Not Printer SPEC
#define DWS_INKLOW040_CYAN		"1583"	// Not Printer SPEC
#define DWS_INKLOW040_PHOTO_M	"1584"	// Not Printer SPEC
#define DWS_INKLOW040_PHOTO_C	"1585"	// Not Printer SPEC
#define DWS_INKLOW040_RED		"1586"	// Not Printer SPEC		990i
#define DWS_INKLOW040_GREEN		"1587"	// Not Printer SPEC		ip8600

#define DWS_CDR_GUIDE_ON		"1852"

#define DOC_CARTRIDGE_NOTHING2	"1400"

enum
{
	CHD_STAT_PH = 1,	
	CHD_STAT_CL,
	CHD_STAT_BK,
	CHD_STAT_Etag,
};
// #define CHD_CNT			(CHD_STAT_Etag -1)	// Pho,Col,Blk

#define MAX_INK_KIND	8	// ip8600
enum 
{
	CHD_BC_50 = 1,		// BC-50
	CHD_BC_33,		// BC-33
	CHD_BC_30,		// BC-30
	CHD_BC_34,		// BC-34
	CHD_BC_24,		// BC-24
	CHD_BC_24N,		// BC-24 no ink check
	CHD_BC_3031,	// BC-30+BC-31
	CHD_BC_3231,	// BC-32+BC-31
	CHD_BC_SC31,	// SCN+BC-31
	CHD_BC_SC,		// SCN
	CHD_BC_60,		// 950i
	CHD_860i,		// 860i
	CHD_990i,		// 990i
	CHD_ip8600,	// ip8600
	CHD_BC_Etag,
};
#define CHD_CNT2		(CHD_BC_Etag -1)	// 

//#define CHD_X_MAX		120	// by F660
#define CHD_X_MAX		142	// by ip8600
#define CHD_Y_MAX		66	// common (chd + icon)
#define CHD_AREA_MAX	10
#define CHD_Y_OFF		18

#define CHD_PHOTO_BJ		"LS"
#define CHD_SCANNER			"SC"
#define CHD_UNSUPORT		"NS"

#define CHD_PHOTO_VL		"VL"	
#define CHD_COLOR_VC		"VC"
#define CHD_COLOR_CL		"CL"
#define CHD_BLACK_BK		"BK"
// #define CHD_VL_OFF		20	
// #define CHD_VC_OFF	 	15
// #define CHD_BK_OFF		25

enum 
{
	MODEL_F850 = 0,
	MODEL_F860,
	MODEL_F870,
	MODEL_F360,
	MODEL_S600,
	MODEL_S630,
	MODEL_S6300,
	MODEL_F900,
	MODEL_F9000,
	MODEL_S500,
	MODEL_S300,
	MODEL_550i,
	MODEL_850i,
	MODEL_950i,
	MODEL_i250,
	MODEL_i255,
	MODEL_560i,
	MODEL_860i,
	MODEL_990i,
	MODEL_ip1000,
	MODEL_ip1500,
	MODEL_ip3100,
	MODEL_ip4100,
	MODEL_ip8600,
	MODEL_Etag,
};
#define 	MODEL_CNT	MODEL_Etag

#define CIL_ON			"ON"

enum
{
	INK_LOW = 0,
	INK_OUT,
	INK_LOW1,
	INK_LOW070,
	INK_LOW040,
};

enum
{
	BACK_GREEN = 0,
	BACK_BLACK,
};

enum
{
	LVR_NONE = 0,
	LVR_UD,
	LVR_RL,
};

enum
{
	I_el = 1,
	I_er,
	I_pb,
	I_bk,
	I_bb,
	I_pm,
	I_pc,
	I_cy,
	I_ma,
	I_ye,
	I_sp,
	I_24c,
	I_24b,
	I_24cf,
	I_24bf,
	I_sc,
	I_re,	// 990i
	I_gr,	// ip8600
	I_Etag,
};

#define INK_CNT			I_Etag

#define TNK_TANK_CHANGE	"AL"

struct	pr_t	{
	char	pr_name[16];	// Printer Name
	short	back_pat;	// Back Pattern Green/Black
	short	lvr_pos;	// Paper thickness lever position
	char	disp_name[16];	// Display Window Title
};

struct  pat_t	{
	short	pat_no;			// color no.
	short	p_x;			// x posi.
};
