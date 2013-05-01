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

//
//  Changes from the original "parallel.c":
//    Call the Canon specific function, _canon_bj(), from the main().
//

/*
 * "$Id: canon_parallel.c,v 1.2 2004/12/07 06:59:47 teru Exp $"
 *
 *   Parallel port backend for the Common UNIX Printing System (CUPS).
 *
 *   Copyright 1997-2002 by Easy Software Products, all rights reserved.
 *
 *   These coded instructions, statements, and computer programs are the
 *   property of Easy Software Products and are protected by Federal
 *   copyright law.  Distribution and use rights are outlined in the file
 *   "LICENSE" which should have been included with this file.  If this
 *   file is missing or damaged please contact Easy Software Products
 *   at:
 *
 *       Attn: CUPS Licensing Information
 *       Easy Software Products
 *       44141 Airport View Drive, Suite 204
 *       Hollywood, Maryland 20636-3111 USA
 *
 *       Voice: (301) 373-9603
 *       EMail: cups-info@cups.org
 *         WWW: http://www.cups.org
 *
 * Contents:
 *
 *   main()         - Send a file to the specified parallel port.
 *   list_devices() - List all parallel devices.
 */

#include <cups/cups.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#ifdef WIN32
#  include <io.h>
#else
#  include <unistd.h>
#  include <fcntl.h>
#  include <termios.h>
#endif /* WIN32 */

#ifdef __sgi
#  include <invent.h>
#  ifndef INV_EPP_ECP_PLP
#    define INV_EPP_ECP_PLP	6	/* From 6.3/6.4/6.5 sys/invent.h */
#    define INV_ASO_SERIAL	14	/* serial portion of SGI ASO board */
#    define INV_IOC3_DMA	16	/* DMA mode IOC3 serial */
#    define INV_IOC3_PIO	17	/* PIO mode IOC3 serial */
#    define INV_ISA_DMA		19	/* DMA mode ISA serial -- O2 */
#  endif /* !INV_EPP_ECP_PLP */
#endif /* __sgi */

#include "canon_common_function.h"

/*
 * Local functions...
 */

void	list_devices(void);

#define PRINTER_NAME_STR "CANON_PARALLEL"

/*
 * 'main()' - Send a file to the specified parallel port.
 *
 * Usage:
 *
 *    printer-uri job-id user title copies options [file]
 */

int			/* O - Exit status */
main(int  argc,		/* I - Number of command-line arguments (6 or 7) */
     char *argv[])	/* I - Command-line arguments */
{
	char	method[255],			/* Method in URI 						*/
			hostname[1024],			/* Hostname 							*/
			username[255],			/* Username info (not used) 			*/
			resource[1024],			/* Resource info (device and options)	*/
			*options;				/* Pointer to options 					*/
			int	port;				/* Port number (not used) 				*/
			FILE	*fp;			/* Print file 							*/
			int	copies;				/* Number of copies to print 			*/
			int	fd;					/* Parallel device 						*/


	/*
	* Make sure status messages are not buffered...
	*/

	setbuf(stderr, NULL);

	/*
	* Check command-line...
	*/

	if (argc == 1)
	{
		list_devices();
		return (0);
	}
	else if (argc < 6 || argc > 7)
	{
		fputs("Usage: parallel job-id user title copies options [file]\n",
																	 stderr);
		return (1);
	}

	/*
	* If we have 7 arguments, print the file named on the command-line.
	* Otherwise, send stdin instead...
	*/

	if (argc == 6)
	{
		fp     = stdin;
		copies = 1;
	}
	else
	{
		/*
		* Try to open the print file...
		*/

		if ((fp = fopen(argv[6], "rb")) == NULL)
		{
				perror("ERROR: unable to open print file");
				return (1);
		}

		copies = atoi(argv[4]);
	}

	/*
	* Extract the device name and options from the URI...
	*/
/*
 *	httpSeparate(argv[0], method, username, hostname, &port, resource);
 *  This function is deprecated under CUPS 1.2, so we(Canon) do not
 *  use it, instead, parse the argv[0] by ourselves as following.
 */
	if ((strncmp(argv[0], "canon_parallel:/dev", 19) == 0))
	{
		memset(resource, 0, 1024);
		strncpy(resource, argv[0] + 15, 1023);
	}
	else
	{
		perror("ERROR: Illegal backend");
		return (1);
	}

	/*
	* See if there are any options...
	*/

	if ((options = strchr(resource, '?')) != NULL)
	{
		/*
		* Yup, terminate the device name string and move to the first
		* character of the options...
		*/

		*options++ = '\0';
	}

	/*
	* Open the parallel port device...
	*/

	do
	{
		if ((fd = open(resource, O_RDWR | O_EXCL)) == -1)
		{
			if (errno == EBUSY)
			{
				fputs("INFO: Parallel port busy; will retry in 30 seconds...\n", stderr);
				sleep(30);
			}
			else
			{
				fprintf(stderr, "ERROR: Unable to open parallel port device file \"%s\": %s\n",
				resource, strerror(errno));
				return (1);
			}
		}
	}
	while (fd < 0);

	_canon_bj( argc, fp, fd, copies, argv[5] ) ;

	/*
	* Close the socket connection and input file and return...
	*/
	close( fd ) ;                               /* device fd close          */
	if (fp != stdin)
		fclose(fp);

	return (0);
}


/*
 * 'list_devices()' - List all parallel devices.
 */

void
list_devices(void)
{
#if defined(__hpux) || defined(__sgi) || defined(__sun)
  static char	*funky_hex = "0123456789abcdefghijklmnopqrstuvwxyz";
				/* Funky hex numbering used for some devices */
#endif /* __hpux || __sgi || __sun */

#ifdef __linux
  int	i;			/* Looping var */
  int	fd;			/* File descriptor */
  char	device[255],		/* Device filename */
	probefile[255],		/* Probe filename */
	basedevice[255];	/* Base device filename for ports */
  FILE	*probe;			/* /proc/parport/n/autoprobe file */
  char	line[1024],		/* Line from file */
	*delim,			/* Delimiter in file */
	make[IPP_MAX_NAME],	/* Make from file */
	model[IPP_MAX_NAME];	/* Model from file */


  for (i = 0; i < 4; i ++)
  {
   /*
    * First open the device to make sure the driver module is loaded...
    */

    if ((fd = open("/dev/parallel/0", O_WRONLY)) >= 0)
    {
      close(fd);
      strcpy(basedevice, "/dev/parallel/");
    }
    else
    {
      sprintf(device, "/dev/lp%d", i);
      if ((fd = open(device, O_WRONLY)) >= 0)
      {
	close(fd);
	strcpy(basedevice, "/dev/lp");
      }
      else
      {
	sprintf(device, "/dev/par%d", i);
	if ((fd = open(device, O_WRONLY)) >= 0)
	{
	  close(fd);
	  strcpy(basedevice, "/dev/par");
	}
	else
	{
	  sprintf(device, "/dev/printers/%d", i);
	  if ((fd = open(device, O_WRONLY)) >= 0)
	  {
	    close(fd);
	    strcpy(basedevice, "/dev/printers/");
	  }
	  else
	    strcpy(basedevice, "/dev/unknown-parallel");
	}
      }
    }

   /*
    * Then try looking at the probe file...
    */

    sprintf(probefile, "/proc/parport/%d/autoprobe", i);
    if ((probe = fopen(probefile, "r")) == NULL)
    {
     /*
      * Linux 2.4 kernel has different path...
      */

      sprintf(probefile, "/proc/sys/dev/parport/parport%d/autoprobe", i);
      probe = fopen(probefile, "r");
    }

    if (probe != NULL)
    {
     /*
      * Found a probe file!
      */

      memset(make, 0, sizeof(make));
      memset(model, 0, sizeof(model));
      strcpy(model, "Unknown");

      while (fgets(line, sizeof(line), probe) != NULL)
      {
       /*
        * Strip trailing ; and/or newline.
	*/

        if ((delim = strrchr(line, ';')) != NULL)
	  *delim = '\0';
	else if ((delim = strrchr(line, '\n')) != NULL)
	  *delim = '\0';

       /*
        * Look for MODEL and MANUFACTURER lines...
	*/

        if (strncmp(line, "MODEL:", 6) == 0 &&
	    strncmp(line, "MODEL:Unknown", 13) != 0)
	  strncpy(model, line + 6, sizeof(model) - 1);
	else if (strncmp(line, "MANUFACTURER:", 13) == 0 &&
	         strncmp(line, "MANUFACTURER:Unknown", 20) != 0)
	  strncpy(make, line + 13, sizeof(make) - 1);
      }

      fclose(probe);

      if (make[0])
	printf("direct canon_parallel:%s%d \"%s %s\" \"Parallel Port #%d with status readback for Canon BJ\"\n",
	       basedevice, i, make, model, i + 1);
      else
	printf("direct canon_parallel:%s%d \"%s\" \"Parallel Port #%d with status readback for Canon BJ\"\n",
	       basedevice, i, model, i + 1);
    }
    else if (fd >= 0)
    {
     /*
      * No probe file, but we know the port is there...
      */

      printf("direct canon_parallel:%s \"Unknown\" \"Parallel Port #%d with status readback for Canon BJ\"\n", device, i + 1);
    }
  }
#elif defined(__sgi)
  int		i, j, n;	/* Looping vars */
  char		device[255];	/* Device filename */
  inventory_t	*inv;		/* Hardware inventory info */


 /*
  * IRIX maintains a hardware inventory of most devices...
  */

  setinvent();

  while ((inv = getinvent()) != NULL)
  {
    if (inv->inv_class == INV_PARALLEL &&
        (inv->inv_type == INV_ONBOARD_PLP ||
         inv->inv_type == INV_EPP_ECP_PLP))
    {
     /*
      * Standard parallel port...
      */

      puts("direct canon_parallel:/dev/plp \"Unknown\" \"Onboard Parallel Port\"");
    }
    else if (inv->inv_class == INV_PARALLEL &&
             inv->inv_type == INV_EPC_PLP)
    {
     /*
      * EPC parallel port...
      */

      printf("direct canon_parallel:/dev/plp%d \"Unknown\" \"Integral EPC parallel port, Ebus slot %d\"\n",
             inv->inv_controller, inv->inv_controller);
    }
  }

  endinvent();

 /*
  * Central Data makes serial and parallel "servers" that can be
  * connected in a number of ways.  Look for ports...
  */

  for (i = 0; i < 10; i ++)
    for (j = 0; j < 8; j ++)
      for (n = 0; n < 32; n ++)
      {
        if (i == 8)		/* EtherLite */
          sprintf(device, "/dev/lpn%d%c", j, funky_hex[n]);
        else if (i == 9)	/* PCI */
          sprintf(device, "/dev/lpp%d%c", j, funky_hex[n]);
        else			/* SCSI */
          sprintf(device, "/dev/lp%d%d%c", i, j, funky_hex[n]);

	if (access(device, 0) == 0)
	{
	  if (i == 8)
	    printf("direct canon_parallel:%s \"Unknown\" \"Central Data EtherLite Parallel Port, ID %d, port %d\"\n",
	           device, j, n);
	  else if (i == 9)
	    printf("direct canon_parallel:%s \"Unknown\" \"Central Data PCI Parallel Port, ID %d, port %d\"\n",
	           device, j, n);
  	  else
	    printf("direct canon_parallel:%s \"Unknown\" \"Central Data SCSI Parallel Port, logical bus %d, ID %d, port %d\"\n",
	           device, i, j, n);
	}
      }
#elif defined(__sun)
  int		i, j, n;	/* Looping vars */
  char		device[255];	/* Device filename */


 /*
  * Standard parallel ports...
  */

  for (i = 0; i < 10; i ++)
  {
    sprintf(device, "/dev/ecpp%d", i);
    if (access(device, 0) == 0)
      printf("direct canon_parallel:%s \"Unknown\" \"Sun IEEE-1284 Parallel Port #%d with status readback for Canon BJ\"\n",
             device, i + 1);
  }

  for (i = 0; i < 10; i ++)
  {
    sprintf(device, "/dev/bpp%d", i);
    if (access(device, 0) == 0)
      printf("direct canon_parallel:%s \"Unknown\" \"Sun Standard Parallel Port #%d with status readback for Canon BJ\"\n",
             device, i + 1);
  }

  for (i = 0; i < 3; i ++)
  {
    sprintf(device, "/dev/lp%d", i);

    if (access(device, 0) == 0)
      printf("direct canon_parallel:%s \"Unknown\" \"PC Parallel Port #%d with status readback for Canon BJ\"\n",
             device, i + 1);
  }

 /*
  * MAGMA parallel ports...
  */

  for (i = 0; i < 40; i ++)
  {
    sprintf(device, "/dev/pm%02d", i);
    if (access(device, 0) == 0)
      printf("direct canon_parallel:%s \"Unknown\" \"MAGMA Parallel Board #%d Port #%d with status readback for Canon BJ\"\n",
             device, (i / 10) + 1, (i % 10) + 1);
  }

 /*
  * Central Data parallel ports...
  */

  for (i = 0; i < 9; i ++)
    for (j = 0; j < 8; j ++)
      for (n = 0; n < 32; n ++)
      {
        if (i == 8)	/* EtherLite */
          sprintf(device, "/dev/sts/lpN%d%c", j, funky_hex[n]);
        else
          sprintf(device, "/dev/sts/lp%c%d%c", i + 'C', j,
                  funky_hex[n]);

	if (access(device, 0) == 0)
	{
	  if (i == 8)
	    printf("direct canon_parallel:%s \"Unknown\" \"Central Data EtherLite Parallel Port, ID %d, port %d\"\n",
	           device, j, n);
  	  else
	    printf("direct canon_parallel:%s \"Unknown\" \"Central Data SCSI Parallel Port, logical bus %d, ID %d, port %d\"\n",
	           device, i, j, n);
	}
      }
#elif defined(__hpux)
  int		i, j, n;	/* Looping vars */
  char		device[255];	/* Device filename */


 /*
  * Standard parallel ports...
  */

  if (access("/dev/rlp", 0) == 0)
    puts("direct canon_parallel:/dev/rlp \"Unknown\" \"Standard Parallel Port (/dev/rlp)\"");

  for (i = 0; i < 7; i ++)
    for (j = 0; j < 7; j ++)
    {
      sprintf(device, "/dev/c%dt%dd0_lp", i, j);
      if (access(device, 0) == 0)
	printf("direct canon_parallel:%s \"Unknown\" \"Parallel Port #%d,%d with status readback for Canon BJ\"\n",
	       device, i, j);
    }

 /*
  * Central Data parallel ports...
  */

  for (i = 0; i < 9; i ++)
    for (j = 0; j < 8; j ++)
      for (n = 0; n < 32; n ++)
      {
        if (i == 8)	/* EtherLite */
          sprintf(device, "/dev/lpN%d%c", j, funky_hex[n]);
        else
          sprintf(device, "/dev/lp%c%d%c", i + 'C', j,
                  funky_hex[n]);

	if (access(device, 0) == 0)
	{
	  if (i == 8)
	    printf("direct canon_parallel:%s \"Unknown\" \"Central Data EtherLite Parallel Port, ID %d, port %d\"\n",
	           device, j, n);
  	  else
	    printf("direct canon_parallel:%s \"Unknown\" \"Central Data SCSI Parallel Port, logical bus %d, ID %d, port %d\"\n",
	           device, i, j, n);
	}
      }
#elif defined(__osf__)
  int	i;			/* Looping var */
  int	fd;			/* File descriptor */
  char	device[255];		/* Device filename */


  for (i = 0; i < 3; i ++)
  {
    sprintf(device, "/dev/lp%d", i);
    if ((fd = open(device, O_WRONLY)) >= 0)
    {
      close(fd);
      printf("direct canon_parallel:%s \"Unknown\" \"Parallel Port #%d with status readback for Canon BJ\"\n", device, i + 1);
    }
  }
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
  int	i;			/* Looping var */
  int	fd;			/* File descriptor */
  char	device[255];		/* Device filename */


  for (i = 0; i < 3; i ++)
  {
    sprintf(device, "/dev/lpt%d", i);
    if ((fd = open(device, O_WRONLY)) >= 0)
    {
      close(fd);
      printf("direct canon_parallel:%s \"Unknown\" \"Parallel Port #%d (interrupt-driven) with status readback for Canon BJ\"\n", device, i + 1);
    }

    sprintf(device, "/dev/lpa%d", i);
    if ((fd = open(device, O_WRONLY)) >= 0)
    {
      close(fd);
      printf("direct canon_parallel:%s \"Unknown\" \"Parallel Port #%d (polled) with status readback for Canon BJ\"\n", device, i + 1);
    }
  }
#elif defined(_AIX)
  int	i;			/* Looping var */
  int	fd;			/* File descriptor */
  char	device[255];		/* Device filename */


  for (i = 0; i < 8; i ++)
  {
    sprintf(device, "/dev/lp%d", i);
    if ((fd = open(device, O_WRONLY)) >= 0)
    {
      close(fd);
      printf("direct canon_parallel:%s \"Unknown\" \"Parallel Port #%d with status readback for Canon BJ\"\n", device, i + 1);
    }
  }
#endif
}

