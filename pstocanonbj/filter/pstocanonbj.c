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
#include <cups/cups.h>
#include <cups/ppd.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>


#include "paramlist.h"
#include "buflist.h"
#include "canonopt.h"

//#define DEBUG_SLEEP
//#define DEBUG_PS
//#define DEBUG_IN_PS


#if HAVE_CONFIG_H
#include <config.h>
#endif

#define FILTER_PATH    PROG_PATH
//#define FILTER_PATH    "/usr/local/bin"
#define FILTER_BIN     "bjfilter"

#define GS_PATH        "/usr/bin"
#define GS_BIN         "gs"
#define SHELL_PATH     "/bin"
#define SHELL_NAME     "sh"


#define	TABLE_BUF_SIZE	1024
#define	LINE_BUF_SIZE	1024
#define	DATA_BUF_SIZE	(1024 * 256)
#define	PARAM_BUF_SIZE	256


#define IS_BLANK(c)		(c == ' '  || c == '\t')
#define IS_RETURN(c)	(c == '\r' || c == '\n')

#define ACRO_UTIL "%%BeginResource: procset Adobe_CoolType_Utility_MAKEOCF"
#define ACRO_UTIL_LEN 55

//For InkJet filter
#define PAGE_DEV_BEGIN	"<<"
#define PAGE_DEV_END	">>setpagedevice"

int g_filter_pid = -1;
int g_signal_received = 0;

#ifdef	DEBUG_IN_PS
int g_in_log_fd = -1;
#endif


static 
int is_acro_util(char *p_data_buf, int read_bytes)
{
	if( strncmp(p_data_buf, ACRO_UTIL, ACRO_UTIL_LEN) == 0 )
		return 1;
	else
		return 0;
}

static 
int is_end_resource(char *p_data_buf, int read_bytes)
{
	if( strncmp(p_data_buf, "%%EndResource", 13) == 0 )
		return 1;
	else
		return 0;
}

static
int read_line(int fd, char *p_buf, int bytes)
{
	static char read_buf[DATA_BUF_SIZE];
	static int buf_bytes = 0;
	static int buf_pos = 0;
	int read_bytes = 0;

	while( bytes > 0 )
	{
		if( buf_bytes == 0 && fd != -1 )
		{
			buf_bytes = read(fd, read_buf, DATA_BUF_SIZE);

			if( buf_bytes > 0 )
			{
#ifdef	DEBUG_IN_PS
				if( g_in_log_fd > 0 )
					write(g_in_log_fd, read_buf, buf_bytes);
#endif
				buf_pos = 0;
			}
		}

		if( buf_bytes > 0 )
		{
			*p_buf = read_buf[buf_pos++];
			bytes--;
			buf_bytes--;
			read_bytes++;

			if( IS_RETURN(*p_buf) )
				break;

			p_buf++;
		}
		else if( buf_bytes < 0 )
		{
			if( errno == EINTR )
				continue;
		}
		else
			break;
	}

	return read_bytes; 
}

static
ParamList *get_ps_params(int ifd, BufList **ps_data)
{
	char read_buf[DATA_BUF_SIZE];
	ParamList *p_list = NULL;
	int begin_page = 0;
	int read_bytes;
	int acro_util = 0;	
	BufList *bl = NULL;

	while( (read_bytes = read_line(ifd, read_buf, DATA_BUF_SIZE - 1)) > 0 )
	{
		int dev_len = strlen(PAGE_DEV_BEGIN);
		int end_len = strlen(PAGE_DEV_END);
		// For Acrobat Reader 
		{
			if( is_acro_util(read_buf, read_bytes) ){
				acro_util = 1;	
			}
			else if( acro_util && is_end_resource(read_buf, read_bytes) ){
				acro_util = 0;	
			}

			if( acro_util ){
				int line_bytes=0;
				while( line_bytes+29 < read_bytes ){
					if(!strncmp(&read_buf[line_bytes], 
						" ct_BadResourceImplementation?", 30)){
						strcpy(&read_buf[line_bytes], " false");
						line_bytes+=6;
						strcpy(&read_buf[line_bytes], 
							&read_buf[line_bytes+24]);
						read_bytes-=24;
					}
					else{
						line_bytes++;
					}
				}
			}
		}

		// Retain the PS data in the buffer list.
		bl = buflist_new(read_buf, read_bytes);

		if( *ps_data == NULL )
			*ps_data = bl;
		else
			buflist_add_tail(*ps_data, bl);

		if( read_bytes > 0 )
		{
			if( read_buf[read_bytes - 1] == '\n' )
				read_buf[read_bytes - 1] = '\0';
			else
				read_buf[read_bytes] = '\0';
		}
		else
		{
			read_buf[0] = '\0';
		}

		// Parse the printing option per line.
		if( strncmp(read_buf, "%%BeginFeature:", 15) == 0 )
		{
			char key_buf[MAX_KEY_LEN + 1];
			char value_buf[MAX_VALUE_LEN + 1];
			int key_len = 0;
			int value_len = 0;
			char *p_code;

			p_code = read_buf + 15;

			while( *p_code != '\0' )
			{
				if( *p_code++ == '*' )
					break;
			}
			while( *p_code != '\0' )
			{
				if( IS_BLANK(*p_code)
				 || key_len >= MAX_KEY_LEN )
					break;
				key_buf[key_len++] = *p_code++;
			}
			while( *p_code != '\0' )
			{
				if( !IS_BLANK(*p_code)  )
					break;
				*p_code++;
			}
			while( *p_code != '\0' )
			{
				if( IS_BLANK(*p_code)
				 || value_len >= MAX_VALUE_LEN )
					break;
				value_buf[value_len++] = *p_code++;
			}
			if( key_len > 0 && value_len > 0 )
			{
				key_buf[key_len] = '\0';
				value_buf[value_len] = '\0';

				param_list_add_multi(&p_list, key_buf, value_buf, value_len + 1, 1);
			}
		}
		else if( !begin_page && strncmp(read_buf, "%%Page:", 7) == 0 )
		{
			begin_page = 1;
		}
		else if( begin_page )
		{
			if( strncmp(read_buf, "%%EndPageSetup", 14) == 0 )
				break;
			else if( strncmp(read_buf, "gsave", 5) == 0 )
				break;
			else if( read_buf[0] >= '0' && read_buf[0] <= '9' )
				break;
		}
		// For InkJet <</***(...)>>setpagedevice.
		else if(strncmp(read_buf + (read_bytes - 1 - end_len), PAGE_DEV_END, end_len) == 0)
		{
			char key_buf[MAX_KEY_LEN + 1];
			char value_buf[MAX_KEY_LEN + 1];
			char *p_code;
			int pos = 0;

			p_code = read_buf + dev_len;

			while( !IS_RETURN(p_code[pos]) )
			{
				int key_pos = 0;
				int val_pos = 0;

				while( p_code[pos] != '/'
				        && !IS_RETURN(p_code[pos]) ) pos++;

				if( p_code[pos] == '/' ) pos++;
				else continue;

				while( isalnum(p_code[pos])
						&& key_pos < 255 )
					key_buf[key_pos++] = p_code[pos++];

				key_buf[key_pos++] = 0;

				if( p_code[pos] == '(' )
				{
					pos++;

					while( p_code[pos] != ')'
						&& !IS_BLANK(p_code[pos])
						&& !IS_RETURN(p_code[pos])
						&& val_pos < 255 )
						value_buf[val_pos++] = p_code[pos++];

					value_buf[val_pos++] = 0;

					if( p_code[pos] == ')' ) pos++;
					else continue;

					if( !strcmp(key_buf, "CNPageSizeName") )
						strncpy(key_buf, "PageSize", MAX_KEY_LEN);
				}
				else continue;

				param_list_add_multi(&p_list, key_buf, value_buf, val_pos+1, 1);
			}
		}
	}

	while( (read_bytes = read_line(-1, read_buf, DATA_BUF_SIZE - 1)) > 0 )
	{
		BufList *bl = buflist_new(read_buf, read_bytes);

		if( *ps_data == NULL )
			*ps_data = bl;
		else
			buflist_add_tail(*ps_data, bl);

		if( read_bytes > 0 )
		{
			if( read_buf[read_bytes - 1] == '\n' )
				read_buf[read_bytes - 1] = '\0';
			else
				read_buf[read_bytes] = '\0';
		}
		else
		{
			read_buf[0] = '\0';
		}
	}
	return p_list;
}

static
int is_borderless(char* size_name)
{
	int len = strlen(size_name);

	if( !strcmp(&size_name[len - 3], ".bl") )
	{
		size_name[len - 3] = 0;
		return 1;
	}
	else
		return 0;
}

static
void mark_ps_param_options(ppd_file_t *p_ppd, ParamList *p_param)
{
	int num = param_list_num(p_param);

	if( num > 0 )
	{
		cups_option_t *options
			= (cups_option_t*)malloc(num * sizeof(cups_option_t));
		if( options )
		{
			int i;
			for( i = 0 ; i < num ; i++ )
			{
				options[i].name = p_param->key;
				options[i].value = p_param->value;
				p_param = p_param->next;
			}
			for( i = num - 1 ; i >= 0 ; i-- )
			{
				cupsMarkOptions(p_ppd, 1, &options[i]);
			}

			free(options);
		}
	}
}

static
void get_min_max_width_length(PpdToOptKey *p_table, cups_option_t *p_opt, int num_opt, long *p_minw_mm, long *p_maxw_mm, long *p_minl_mm, long *p_maxl_mm)
{
	*p_minw_mm = -1;
	*p_maxw_mm = -1;
	*p_minl_mm = -1;
	*p_maxl_mm = -1;

	while( num_opt-- > 0 )
	{
		if( !strcmp(p_opt->name, "CNMinWidth") )
			*p_minw_mm = atoi(p_opt->value);
		else if( !strcmp(p_opt->name, "CNMaxWidth") )
			*p_maxw_mm = atoi(p_opt->value);
		else if( !strcmp(p_opt->name, "CNMinLength") )
			*p_minl_mm = atoi(p_opt->value);
		else if( !strcmp(p_opt->name, "CNMaxLength") )
			*p_maxl_mm = atoi(p_opt->value);

		p_opt++;
	}
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

char** make_filter_param_list(char *model, int reso,
		ParamList *pl, ParamList **cmd_list)
{
	char* cmd_name = NULL;
	char* cmd_path = NULL;
	ParamList *curs;
	int cmd_num;
	char **cmd_buf;
	int cmd_buf_index;
	char reso_buf[MAX_VALUE_LEN + 1];

	// Make command path.
	cmd_name = (char*)malloc(strlen(FILTER_BIN) + strlen(model) + 1);
	strcpy(cmd_name, FILTER_BIN);
	strcat(cmd_name, model);

	cmd_path =  (char*)malloc(strlen(FILTER_PATH) + strlen(cmd_name));
	strcpy(cmd_path, FILTER_PATH);
	strcat(cmd_path, "/");
	strcat(cmd_path, cmd_name);

	for( curs = pl ; curs != NULL ; curs = curs->next )
	{
		char value_buf[MAX_VALUE_LEN + 1];

		if( strlen(curs->key) + 1 > MAX_KEY_LEN )
			continue;
		if( curs->value_size + 1 > MAX_VALUE_LEN )
			continue;

		memcpy(value_buf, curs->value, curs->value_size);
		value_buf[curs->value_size] = 0;

		param_list_add(cmd_list, curs->key, value_buf, curs->value_size + 1);
	}

	// Make resolution string.
	snprintf(reso_buf, 255, "%d", reso);
	param_list_add(cmd_list, "--imageres", reso_buf, strlen(reso_buf) + 1);

	cmd_num = param_list_num(*cmd_list);
	cmd_buf = (char**)malloc((cmd_num * 2 + 3) * sizeof(char*));

	cmd_buf[0] = cmd_path;
	cmd_buf[1] = cmd_name;
	cmd_buf_index = 2;

	for( curs = *cmd_list ; curs != NULL ; curs = curs->next )
	{
		if( strcmp(curs->value, "false") )
		{
			cmd_buf[cmd_buf_index++] = curs->key;
			if( strcmp(curs->value, "true") )
			{
				cmd_buf[cmd_buf_index++] = curs->value;
			}
		}
	}
	cmd_buf[cmd_buf_index] = NULL;

	return cmd_buf;
}

char* make_filter_param_line(char *model, int reso, ParamList *pl)
{
	ParamList *cmd_list = NULL;
	char *cmd_buf = NULL;
	char **cmd_param;

	cmd_param = make_filter_param_list(model, reso, pl, &cmd_list);

	if( cmd_param )
	{
		int buf_len = 0;
		int i;

		for( i = 0 ; cmd_param[i] != NULL ; i++ )
		{
			if( i == 1 )
				continue;
			buf_len += strlen(cmd_param[i]) + 2;
		}

		cmd_buf = malloc(buf_len);

		if( cmd_buf != NULL )
		{
			*cmd_buf = 0;

			for( i = 0 ; cmd_param[i] != NULL ; i++ )
			{
				if( i == 1 )
					continue;
				strcat(cmd_buf, cmd_param[i]);
				strcat(cmd_buf, " ");
			}
			cmd_buf[buf_len - 2] = 0;
		}
		free(cmd_param);
	}

	if( cmd_list != NULL )
		param_list_free(cmd_list);

	return cmd_buf;
}

static
char* make_cmd_param(cups_option_t *p_cups_opt, int num_opt,
	ParamList *p_param, char *p_product, int len)
{
#ifdef DEBUG_PPD
	char *p_ppd_name = "debug.ppd";
#else
	char *p_ppd_name = getenv("PPD");
#endif
	ppd_file_t *p_ppd;
	char *cmd_buf = NULL;
	ppd_choice_t *p_choice;
	ppd_size_t *p_size;
	int reso;
	char gs_exec_buf[256];
	char gs_cmd_buf[1024];
	char *flt_cmd_buf = NULL;

//For InkJet
	long minw_mm, maxw_mm;
	long minl_mm, maxl_mm;
	PpdToOptKey *p_opt_key_table = alloc_opt_key_table(p_ppd_name);
	PpdToOptKey *p_table = p_opt_key_table;
	ParamList *p_list = NULL;

	if( (p_ppd = ppdOpenFile(p_ppd_name)) == NULL )
		return NULL;

	if( p_opt_key_table == NULL)
		return NULL;

	ppdMarkDefaults(p_ppd);
	mark_ps_param_options(p_ppd, p_param);
	cupsMarkOptions(p_ppd, num_opt, p_cups_opt);

	// Obtain page size setting.
	p_choice = ppdFindMarkedChoice(p_ppd, "PageSize");
	p_size = ppdPageSize(p_ppd, p_choice->choice);

	// Obtain resolution setting.
	p_choice = ppdFindMarkedChoice(p_ppd, "Resolution");
	reso = atoi(p_choice->choice);

	//Obtain product name.
	parse_product_name(p_ppd->product, p_product, len);

	get_min_max_width_length(p_opt_key_table, p_cups_opt, num_opt,
		&minw_mm, &maxw_mm, &minl_mm, &maxl_mm);

// gs command
	strncpy(gs_exec_buf, GS_PATH, 256);
	strncat(gs_exec_buf, "/", 1);
	strncat(gs_exec_buf, GS_BIN, strlen(GS_BIN));
	snprintf(gs_cmd_buf, 1023,
	"%s -r%d -g%dx%d -q -dNOPROMPT -dSAFER -sDEVICE=ppmraw -sOutputFile=- -| ",
			gs_exec_buf, reso, (int)(p_size->width * (float)reso / 72.0),
			(int)(p_size->length * (float)reso / 72.0));

// For InkJet bjfilter Command 
	while( p_table->ppd_key != NULL )
	{
		p_choice = ppdFindMarkedChoice(p_ppd, p_table->ppd_key);
		if( p_choice != NULL )
		{
			char choice[256];

			strncpy(choice, p_choice->choice, 256);

			if( !strcmp(p_table->ppd_key, "PageSize")
			 && !strcmp(choice, "Custom") )
			{
				char pw[256];
				char pl[256];
				long pw_mm, pl_mm;
				pw_mm = (long)((double)p_size->width * 2540.0 / 72.0);
				pl_mm = (long)((double)p_size->length * 2540.0 / 72.0);
				if( minw_mm != -1 && pw_mm < minw_mm )
					pw_mm = minw_mm;
				if( maxw_mm != -1 && pw_mm > maxw_mm )
					pw_mm = maxw_mm;
				if( minl_mm != -1 && pl_mm < minl_mm )
					pl_mm = minl_mm;
				if( maxl_mm != -1 && pl_mm > maxl_mm )
					pl_mm = maxw_mm;
				snprintf(pw, 255, "%ld", pw_mm);
				snprintf(pl, 255, "%ld", pl_mm);
				param_list_add(&p_list, p_table->opt_key, "user", 5);
				param_list_add(&p_list,
					"--paperwidth", pw, strlen(pw) + 1);
				param_list_add(&p_list,
					"--paperheight", pl, strlen(pl) + 1);
			}
			else
			{
				if( !strcmp(p_table->ppd_key, "PageSize")
				 && is_borderless(choice) )
				{
					param_list_add(&p_list, "--borderless", "", 1);
				}
				param_list_add(&p_list, p_table->opt_key,
					choice, strlen(choice) + 1);
			}
		}
		p_table++;
	}

	while( num_opt-- > 0 )
	{
		char *opt_key = ppd_to_opt_key(p_opt_key_table, p_cups_opt->name);
		if( opt_key != NULL )
		{
			char *value = p_cups_opt->value;
			param_list_add(&p_list, opt_key, value, strlen(value) + 1);
		}
		p_cups_opt++;
	}

	{
		char bbox_buf[256];
		float dd = 1.0 - 36.0 / (float)reso;
		int left   = (int)p_size->left;
		int top    = (int)(p_size->top + dd);
		int right  = left + (int)(p_size->right - p_size->left + dd);
		int bottom = top - (int)(p_size->top - p_size->bottom + dd);

		snprintf(bbox_buf, 255, "%-d,%-d,%-d,%-d", left, bottom, right, top);
		param_list_add(&p_list, "--bbox",
				bbox_buf, strlen(bbox_buf) + 1);
	}

	flt_cmd_buf = make_filter_param_line(p_product, reso, p_list);

	if( p_list != NULL ){
		param_list_free(p_list);
		free_opt_key_table(p_opt_key_table);
	}

	ppdClose(p_ppd);

	if( flt_cmd_buf )
	{
		int buf_len;			
		
		buf_len = strlen(gs_cmd_buf) + strlen(flt_cmd_buf) + 1;

		if( (cmd_buf = (char*)malloc(buf_len)) != NULL )
		{
			strcpy(cmd_buf, gs_cmd_buf);
			strcat(cmd_buf, flt_cmd_buf);
		}
		free(flt_cmd_buf);
fprintf(stderr, "pstocanonbj: %s\n", cmd_buf);
		return cmd_buf;
	}
	else
		return NULL;
}

int exec_filter(char *cmd_buf, int ofd, int fds[2])
{
	int status = 0;
	int	child_pid = -1;
	char *filter_param[4];
	char shell_buf[256];

	if( pipe(fds) >= 0 )
	{
		child_pid = fork();

		if( child_pid == 0 )
		{

			setpgid(0, 0);
			
			close(0);
			dup2(fds[0], 0);
			close(fds[0]);
			close(fds[1]);

			if( ofd != 1 )
			{
				close(1);
				dup2(ofd, 1);
				close(ofd);
			}

			{

				strcpy(shell_buf, SHELL_PATH);
				strcat(shell_buf, "/");
				strcat(shell_buf, SHELL_NAME);

				filter_param[0] = shell_buf;
				filter_param[1] = "-c";
				filter_param[2] = cmd_buf;
				filter_param[3] = NULL;

				execv(shell_buf, filter_param);
						
				fprintf(stderr, "execl() error\n");
				status = -1;
			}
		}
		else if( child_pid != -1 )
		{
			close(fds[0]);
		}
	}
	return child_pid;
}

static void sigterm_handler(int sigcode)
{
	if( g_filter_pid != -1 )
		kill(-g_filter_pid, SIGTERM);

	g_signal_received = 1;
}

int main(int argc, char *argv[])
{
	cups_option_t *p_cups_opt = NULL;
	int num_opt = 0;
	ParamList *p_ps_param = NULL;
	BufList *p_ps_data = NULL;
	char *p_data_buf;
	int ifd = 0;
	int fds[2];
	struct sigaction sigact;
	char *cmd_buf = NULL;

	char product[256];
#ifdef	DEBUG_SLEEP
	sleep(30);
#endif

	setbuf(stderr, NULL);
	fprintf(stderr, "DEBUG: pstocanonbj start.\n");

	memset(&sigact, 0, sizeof(sigact));
	sigact.sa_handler = sigterm_handler;

	if( sigaction(SIGTERM, &sigact, NULL) )
	{
		fputs("ERROR: pstocanonbj can't register signal hander.\n", stderr);
		return 1;
	}

	if( argc < 6 || argc > 7 )
	{
		fputs("ERROR: pstocanonbj illegal parameter number.\n", stderr);
		return 1;
	}

	if( argv[5] != NULL )
	{
		num_opt = cupsParseOptions(argv[5], 0, &p_cups_opt);
		if( num_opt < 0 )
		{
			fputs("ERROR: illegal option.\n", stderr);
			return 1;
		}
	}

	if( argc == 7 )
	{
		if( (ifd = open(argv[6], O_RDONLY)) == -1 )
		{
			fputs("ERROR: can't open file.\n", stderr);
			return 1;
		}
	}

#ifdef	DEBUG_IN_PS
	g_in_log_fd = open("/tmp/debug_in_log.ps", O_WRONLY);
#endif
	p_ps_param = get_ps_params(ifd, &p_ps_data);

	if( (cmd_buf = make_cmd_param(p_cups_opt, num_opt, p_ps_param, product, 256)) == NULL )
	{
		fputs("ERROR: can't make parameter.\n", stderr);
		goto error_return;
	}

	if( g_signal_received )
		goto error_return;

	if( (p_data_buf = (char*)malloc(DATA_BUF_SIZE)) != NULL )
	{
#ifdef	DEBUG_PS
		int log_fd = open("/tmp/debug_data_log.ps", O_WRONLY);
#endif
		// Execute extended filter.
		g_filter_pid = exec_filter(cmd_buf, 1, fds);

		// Write retained PS header and data.
		buflist_write(p_ps_data, fds[1]);

#ifdef	DEBUG_PS
		buflist_write(p_ps_data, log_fd);
#endif

		// Write remaining PS data.
		while( !g_signal_received )
		{
			int read_bytes = read(ifd, p_data_buf, DATA_BUF_SIZE);

			if( read_bytes > 0 )
			{
				int write_bytes;
				char *p_data = p_data_buf;
#ifdef	DEBUG_PS
				write(log_fd, p_data_buf, read_bytes);
#endif
#ifdef	DEBUG_IN_PS
				if( g_in_log_fd > 0 )
					write(g_in_log_fd, p_data_buf, read_bytes);
#endif
				do
				{
					write_bytes = write(fds[1], p_data, read_bytes);

					if( write_bytes < 0 )
					{
						if( errno == EINTR )
							continue;
						fprintf(stderr,
							"ERROR: pstocanonbj write error,%d.\n", errno);
						goto error_exit;
					}
					read_bytes -= write_bytes;
					p_data += write_bytes;
				}
				while( !g_signal_received && read_bytes > 0 );
			}
			else if( read_bytes < 0 )
			{
				if( errno == EINTR )
					continue;
				fprintf(stderr, "ERROR: pstocanonbj read error,%d.\n", errno);
				goto error_exit;
			}
			else
				break;
		}

		free(p_data_buf);

#ifdef	DEBUG_PS
		if( log_fd != -1 ) close(log_fd);
#endif
	}

#ifdef	DEBUG_IN_PS
	if( g_in_log_fd > 0 )
		close(g_in_log_fd);
#endif

	free(cmd_buf);

	if( p_ps_param != NULL )
		param_list_free(p_ps_param);
	if( p_ps_data != NULL )
		buflist_destroy(p_ps_data);
	if( ifd != 0 )
		close(ifd);

	close(fds[1]);

	if( g_filter_pid != -1 )
		waitpid(g_filter_pid, NULL, 0);

	return 0;

error_exit:
	if( p_data_buf != NULL )
		free(p_data_buf);

error_return:
	free(cmd_buf);

	if( p_ps_param != NULL )
		param_list_free(p_ps_param);
	if( p_ps_data != NULL )
		buflist_destroy(p_ps_data);
	if( ifd != 0 )
		close(ifd);

	return 1;
}
