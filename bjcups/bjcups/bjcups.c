/*
 *  CUPS add-on module for Canon Bubble Jet Printer.
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

#if HAVE_CONFIG_H
#include	<config.h>
#endif	// HAVE_CONFIG_H

#include <cups/cups.h>
#include <cups/ppd.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <popt.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "cncl.h"
#include "cncldef.h"
#include "cncl2def.h"
#include "cncludef.h"
#include "cnclucom.h"

#include "paramlist.h"
#include "bjexec.h"
#include "bjipc.h"
#include "bjcups.h"
#include "bjutil.h"

#define	BJCUPS_VERSION	"bjcups Ver.2.50  Copyright CANON.INC 2002-2005\n"

#define	BJ_SOCKET		"/tmp/bjui"

#define	UI_PATH			PROG_PATH
//#define	UI_PATH			"/usr/local/bin/"
#define	UI_SLASH		"/"
#define	UI_NAME			"printui"

#define	UI_OPT_VERSION	1
#define	UI_OPT_PRINTER	2
#define	UI_OPT_OTHERS	4


char* g_printer_name = NULL;


extern int GetIPCData(LPIPCU pipc, char *sname);


int get_value_and_param_array(short id, short** value, char*** param)
{
	int count;

	switch( id )
	{
	case CNCL_MEDIATYPE:	// Media Type.
		*value = g_media_value;
		*param  = g_media_param;
		count = sizeof(g_media_value) / sizeof(short);
		break;

	case CNCL_MEDIASUPPLY:	// Media Supply.
		*value = g_supply_value;
		*param  = g_supply_param;
		count = sizeof(g_supply_value) / sizeof(short);
		break;

	case CNCL_PAPERSIZE:	// Paper Size.
		*value = g_size_value;
		*param  = g_size_param;
		count = sizeof(g_size_value) / sizeof(short);
		break;

	case CNCL_DITHER_PAT:		// Halftoning.
		*value = g_halftoning_value;
		*param  = g_halftoning_param;
		count = sizeof(g_halftoning_value) / sizeof(short);
		break;

	case CNCL_CARTRIDGE:		// Cartridge.
		*value = g_cartridge_value;
		*param  = g_cartridge_param;
		count = sizeof(g_cartridge_value) / sizeof(short);
		break;

	case CNCL_PRINTQUALITY:		// Print quality.
		*value = g_quality_value;
		*param  = g_quality_param;
		count = sizeof(g_quality_value) / sizeof(short);
		break;

	case CNCL_INPUT_GAMMA:		// Input gamma.
		*value = g_input_gamma_value;
		*param  = g_input_gamma_param;
		count = sizeof(g_input_gamma_value) / sizeof(short);
		break;

	case CNCL_GRAYSCALE:		// Grayscale.
		*value = g_grayscale_value;
		*param  = g_grayscale_param;
		count = sizeof(g_grayscale_value) / sizeof(short);
		break;

	case CNCL_PAPERGAP_COMMAND:	// Paper gap.
		*value = g_paper_gap_value;
		*param  = g_paper_gap_param;
		count = sizeof(g_paper_gap_value) / sizeof(short);
		break;

	case EXT_CNCL_INTENT:		// Intent.
		*value = g_intent_value;
		*param  = g_intent_param;
		count = sizeof(g_intent_value) / sizeof(short);
		break;

	case EXT_CNCL_LOCATION:		// Location.
		*value = g_location_value;
		*param  = g_location_param;
		count = sizeof(g_location_value) / sizeof(short);
		break;

	default:
		*value = NULL;
		*param  = NULL;
		count = 0;
		break;
	}

	return count;
}

char *value_to_param(short id, short value)
{
	short *value_array;
	char **key_array;
	int count;
	int i;

	count = get_value_and_param_array(id, &value_array, &key_array);

	for( i = 0 ; i < count ; i++ )
	{
		if( value_array[i] == value )
			break;
	}
	return (i < count)? key_array[i] :  "";
}

void add_option_param(ParamList **pp_list, char *key, int id, int value)
{
	char param[1024];

	snprintf(param, 1023, "%s=%s", key, value_to_param(id, value));
	param_list_add_multi(pp_list, "-o", param, strlen(param) + 1, 1);
}

void add_option_value(ParamList **pp_list, char *key, int value)
{
	char param[1024];

	snprintf(param, 1023, "%s=%d", key, value);
	param_list_add_multi(pp_list, "-o", param, strlen(param) + 1, 1);
}

void add_option_string(ParamList **pp_list, char *key, char *str)
{
	char param[1024];

	snprintf(param, 1023, "%s=%s", key, str);
	param_list_add_multi(pp_list, "-o", param, strlen(param) + 1, 1);
}

int init_option(ParamList **pp_list, int argc, char* argv[])
{
	static struct poptOption option_table[] =
	{
		{"version", 'v',               0, NULL, UI_OPT_VERSION},
		{NULL,      'P', POPT_ARG_STRING, NULL, UI_OPT_PRINTER},
		{NULL,      'o', POPT_ARG_STRING, NULL, UI_OPT_OTHERS },
		{NULL,        0,               0, NULL, 0}
	};

	poptContext context
		= poptGetContext("bjcups", argc, (const char**)argv, option_table, 0);
	int rc;

	while( (rc = poptGetNextOpt(context)) > 0 )
	{
		switch( rc )
		{
		case UI_OPT_VERSION:
			fputs(BJCUPS_VERSION, stderr);
			exit(1);

		case UI_OPT_PRINTER:
			g_printer_name = strdup(poptGetOptArg(context));
			break;

		case UI_OPT_OTHERS:
		{
			const char *option = poptGetOptArg(context);
			param_list_add(pp_list, "-o", option, strlen(option) + 1);
			break;
		}
		default:
			break;
		}
	}

	if( rc < -1 )
	{
		fprintf(stderr, "Unknown switch, %s\n",
			poptBadOption(context, POPT_BADOPTION_NOALIAS));

		return rc;
	}
	return 0;
}

static
char *parse_product_name(char *ppd_product, char *product, int len)
{
	char *p_product = product;
	char *p_cc;
	int count = 0;

	memset(product, 0, len);
	p_cc = ppd_product;

	while( *p_cc == '(' )
		p_cc++;

	while( *p_cc != ')' && *p_cc != '\0' && count < len - 1 )
	{
		*p_product++ = *p_cc++;
		count++;
	}
	*p_product = '\0';

	return product;
}

char *get_product_name(const char *printer_name, char *product_name, int len)
{
	const char *ppd_name = cupsGetPPD(printer_name);
	char *p_product = NULL;
	static char product[256];

	if( len < 1 )
		return NULL;

	if( ppd_name != NULL )
	{
		ppd_file_t *p_ppd;

		if( (p_ppd = ppdOpenFile(ppd_name)) != NULL )
		{
			if( !strcmp(p_ppd->manufacturer, "Canon") )
			{
				p_product = parse_product_name(p_ppd->product, product, 256);
				strncpy(product_name, product, len - 1);
			}
			ppdClose(p_ppd);
		}
	}
	return p_product;
}

void get_paper_dimension_points(IPCU *p_ipc, double *p_pw, double* p_ph)
{
	BJFLTDEVICE *p_fltdev = &p_ipc->parm.bjfltdev;
	CNCLPAPERSIZE *p_papersize = &p_ipc->parm.papersize;
	double pw_pt = (double)p_fltdev->bjfltPaperWidth * 72.0 / 2540.0;
	double ph_pt = (double)p_fltdev->bjfltPaperHeight * 72.0 / 2540.0;
	long rw_mm = (long)(pw_pt * 2540.0 / 72.0);
	long rh_mm = (long)(ph_pt * 2540.0 / 72.0);
	long minw_mm = p_papersize->nMinCustomWidth;
	long maxw_mm = p_papersize->nMaxCustomWidth;
	long minh_mm = p_papersize->nMinCustomLength;
	long maxh_mm = p_papersize->nMaxCustomLength;
	long dw = 0;
	long dh = 0;

	while( rw_mm < minw_mm )
	{
		dw++;
		pw_pt = (double)(p_fltdev->bjfltPaperWidth + dw) * 72.0 / 2540.0;
		rw_mm = (long)(pw_pt * 2540.0 / 72.0);
	}
	while( rw_mm > maxw_mm )
	{
		dw--;
		pw_pt = (double)(p_fltdev->bjfltPaperWidth + dw) * 72.0 / 2540.0;
		rw_mm = (long)(pw_pt * 2540.0 / 72.0);
	}
	while( rh_mm < minh_mm )
	{
		dh++;
		ph_pt = (double)(p_fltdev->bjfltPaperHeight + dh) * 72.0 / 2540.0;
		rh_mm = (long)(ph_pt * 2540.0 / 72.0);
	}
	while( rh_mm > maxh_mm )
	{
		dh--;
		ph_pt = (double)(p_fltdev->bjfltPaperHeight + dh) * 72.0 / 2540.0;
		rh_mm = (long)(ph_pt * 2540.0 / 72.0);
	}

	*p_pw = pw_pt;
	*p_ph = ph_pt;
}

char **make_lpr_param(ParamList **pp_list, char *printer,
						IPCU *p_ipc, char *print_file)
{
	BJFLTDEVICE *p_fltdev = &p_ipc->parm.bjfltdev;
	BJFLTCOLORSYSTEM *p_fltcol = &p_ipc->parm.bjfltcolor;
	BJFLT_UISETUP *p_uiset = &p_ipc->parm.bjflt_uisetup;
	CNCLPAPERSIZE *p_papersize = &p_ipc->parm.papersize;
	char **cmd_buf;
	int cmd_buf_index;
	int cmd_num;

	add_option_param(pp_list, "CNCartridge",
			CNCL_CARTRIDGE, p_fltdev->bjfltInkType);

	add_option_param(pp_list, "MediaType",
			CNCL_MEDIATYPE, p_fltdev->bjfltMediaType);

	if( p_fltdev->bjfltPaperSize == CND_SIZE_USER )
	{
		double pw, ph;
		char custom_size[256];

		get_paper_dimension_points(p_ipc, &pw, &ph);
		snprintf(custom_size, 255, "Custom.%lfx%lf", pw, ph);

		add_option_string(pp_list, "PageSize", custom_size);
	}
	else
	{
		add_option_param(pp_list, "PageSize",
			CNCL_PAPERSIZE, p_fltdev->bjfltPaperSize);
	}

	add_option_param(pp_list, "InputSlot",
			CNCL_MEDIASUPPLY, p_fltdev->bjfltMediaSupply);

	add_option_param(pp_list, "CNHalftoning",
			CNCL_DITHER_PAT, p_fltdev->bjfltBinMethod);

	add_option_param(pp_list, "CNGrayscale",
			CNCL_GRAYSCALE, p_fltdev->bjfltGrayScale);

	add_option_param(pp_list, "CNQuality",
			CNCL_PRINTQUALITY, p_fltdev->bjfltPrintQuality);

	add_option_param(pp_list, "CNGamma",
			CNCL_INPUT_GAMMA, p_fltcol->bjfltGamma);

	if( p_fltdev->bjfltPaperGap != CND_PGAP_CMD_NA )
	{
		add_option_param(pp_list, "CNPaperGap",
			CNCL_PAPERGAP_COMMAND, p_fltdev->bjfltPaperGap);
	}

	add_option_param(pp_list, "CNRenderIntent",
			EXT_CNCL_INTENT, p_fltcol->bjfltIntent);

	add_option_param(pp_list, "CNLocation",
			EXT_CNCL_LOCATION, p_uiset->bjflt_location);

	add_option_value(pp_list, "CNBalanceC", p_fltcol->bjfltBalanceC);
	add_option_value(pp_list, "CNBalanceM", p_fltcol->bjfltBalanceM);
	add_option_value(pp_list, "CNBalanceY", p_fltcol->bjfltBalanceY);
	add_option_value(pp_list, "CNBalanceK", p_fltcol->bjfltBalanceK);
	add_option_value(pp_list, "CNDensity",  p_fltcol->bjfltDensity);
	add_option_value(pp_list, "CNPercent",  p_uiset->bjflt_percent);
	add_option_value(pp_list, "CNCopies",   p_uiset->bjflt_copies);

	add_option_value(pp_list, "CNMinWidth",  p_papersize->nMinCustomWidth);
	add_option_value(pp_list, "CNMaxWidth",  p_papersize->nMaxCustomWidth);
	add_option_value(pp_list, "CNMinLength", p_papersize->nMinCustomLength);
	add_option_value(pp_list, "CNMaxLength", p_papersize->nMaxCustomLength);

#ifdef DEBUG_PARAM_LIST
	param_list_print(*pp_list);
#endif
	cmd_num = param_list_num(*pp_list);
	cmd_buf = (char**)malloc((cmd_num * 2 + 5) * sizeof(char*));

	if( cmd_buf )
	{
		ParamList *curs;

		cmd_buf[0] = "lpr";
		cmd_buf[1] = "-P";
		cmd_buf[2] = printer;

		cmd_buf_index = 3;

		for( curs = *pp_list ; curs != NULL ; curs = curs->next )
		{
			cmd_buf[cmd_buf_index++] = curs->key;
			cmd_buf[cmd_buf_index++] = curs->value;
		}
		cmd_buf[cmd_buf_index++] = print_file;
		cmd_buf[cmd_buf_index] = NULL;
	}

	return cmd_buf;
}

void exec_lpr(char **lpr_param)
{
	int pid = fork();

	if( pid == -1 )
		return;

	if( pid == 0 )
	{
		execv("/usr/bin/lpr", lpr_param);
		fputs("ERROR: Can't execute lpr command.\n", stderr);
	}
	else
	{
		waitpid(pid, NULL, 0);
	}
}

int main(int argc, char* argv[])
{
	ParamList *p_list = NULL;
	int ui_pid;
	char *print_file;
	char product_name[64];
//	char ui_path[256];
	char *ui_path = NULL;
	char ui_name[256];
	char socket_name[64];
	char *ui_arg[5];

	if( argc == 1 )
	{
		fputs(BJCUPS_VERSION, stderr);
		fputs("Usage: bjcups [-P printer name] <file name>\n", stderr);
		exit(1);
	}

	print_file = argv[argc - 1];

	if( init_option(&p_list, argc, argv) < 0 )
		exit(1);

	if( g_printer_name == NULL )
	{
		const char *printer_name;

		if( (printer_name = cupsGetDefault()) != NULL )
			g_printer_name = strdup(printer_name);
		else
		{
			fputs("ERROR: Can't get default printer name.\n", stderr);
			exit(1);
		}
	}

	if( get_product_name(g_printer_name, product_name, 64) == NULL )
	{
		fputs("ERROR: Illegal printer name.\n", stderr);
		exit(1);
	}

	ui_path = (char *)malloc(strlen(UI_PATH) + strlen(UI_SLASH) + strlen(UI_NAME) + strlen(product_name));
	strcpy(ui_path, UI_PATH);
	strcat(ui_path, UI_SLASH);
	strcat(ui_path, UI_NAME);
	strcat(ui_path, product_name);

	strcpy(ui_name, UI_NAME);
	strcat(ui_name, product_name);

	snprintf(socket_name, 63, "%s%d", BJ_SOCKET, getpid());
	ui_arg[0] = ui_name;
	ui_arg[1] = "-s";
	ui_arg[2] = socket_name;
	ui_arg[3] = "--cups";
	ui_arg[4] = NULL;

	ui_pid = fork();

	if( ui_pid == 0 )
	{
		execv(ui_path, ui_arg);
	}
	else
	{		
		while(1){
			IPCU ipc;
			int ret;
			ret = GetIPCData(&ipc, socket_name);
			
			if( ret == RET_PRINT )
			{
				char **lpr_param
				= make_lpr_param(&p_list, g_printer_name, &ipc, print_file);
				if( lpr_param )
				{
					exec_lpr(lpr_param);
					free(lpr_param);
				}
				break;
			}
			else if( ret == RET_PDATA ){	// For Maintenance
				char **lpr_param
						= make_pdata_lpr_param( g_printer_name, &ipc);	
					
				if( lpr_param )
				{
					exec_lpr(lpr_param);
					free_util_cmd_param( lpr_param );
				}

			}
			else if( ret == RET_FDATA ){	// For Maintenance i250/i255
				char **lpr_param
						= make_fdata_lpr_param( g_printer_name, &ipc );	

				if( lpr_param )
				{
					exec_lpr(lpr_param);
					free_util_cmd_param(lpr_param); 
					lpr_param = NULL;
				}
			}
			else if( ret == RET_WDATA ){	// For Maintenance i250/i255
				char **lpr_param
				= make_wdata_lpr_param( g_printer_name, &ipc);
				if( lpr_param )
				{
					exec_lpr(lpr_param);
					free_util_cmd_param(lpr_param); 
					lpr_param = NULL;
				}
			}
			else if( ret == RET_CANCEL ){
				break;	
	
			}
			else if ( ret == RET_ERROR ){
				break;
			}
		}
		
		if( p_list != NULL )
			param_list_free(p_list);

		waitpid(ui_pid, NULL, 0);
	}

	free(g_printer_name);
	return 0;
}
