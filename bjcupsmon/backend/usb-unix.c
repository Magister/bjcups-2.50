/*
 * "$Id: usb-unix.c 9793 2011-05-20 03:49:49Z mike $"
 *
 *   USB port backend for CUPS.
 *
 *   This file is included from "usb.c" when compiled on UNIX/Linux.
 *
 *   Copyright 2007-2011 by Apple Inc.
 *   Copyright 1997-2007 by Easy Software Products, all rights reserved.
 *
 *   These coded instructions, statements, and computer programs are the
 *   property of Apple Inc. and are protected by Federal copyright
 *   law.  Distribution and use rights are outlined in the file "LICENSE.txt"
 *   "LICENSE" which should have been included with this file.  If this
 *   file is missing or damaged, see the license at "http://www.cups.org/".
 *
 *   This file is subject to the Apple OS-Developed Software exception.
 *
 * Contents:
 *
 *   print_device() - Print a file to a USB device.
 *   list_devices() - List all USB devices.
 *   open_device()  - Open a USB device...
 *   side_cb()      - Handle side-channel requests...
 */

/*
 * Include necessary headers.
 */

#include "string-private.h"
#include <sys/select.h>
#include <cups/http.h>
#include <cups/backend.h>
#include <cups/sidechannel.h>
#include <errno.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include "canon_common_function.h"


/*
 * Local functions...
 */

static int	open_device(const char *uri, int *use_bc);
/*static int	side_cb(int print_fd, int device_fd, int snmp_fd,
                http_addr_t *addr, int use_bc);*/


/*
 * 'print_device()' - Print a file to a USB device.
 */

int					/* O - Exit status */
print_device(const char *uri,		/* I - Device URI */
             const char *hostname,	/* I - Hostname/manufacturer */
             const char *resource,	/* I - Resource/modelname */
	     char       *options,	/* I - Device options/serial number */
         FILE*      print_fd,	/* I - File descriptor to print */
	     int        copies,		/* I - Copies to print */
	     int	argc,		/* I - Number of command-line arguments (6 or 7) */
	     char	*argv[])	/* I - Command-line arguments */
{
  int		use_bc;			/* Use backchannel path? */
  int		device_fd;		/* USB device */
  ssize_t	tbytes;			/* Total number of bytes written */
  struct termios opts;			/* Parallel port options */


  (void)argc;
  (void)argv;

 /*
  * Open the USB port device...
  */

  fputs("STATE: +connecting-to-device\n", stderr);

  do
  {
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
   /*
    * *BSD's ulpt driver currently does not support the
    * back-channel, incorrectly returns data ready on a select(),
    * and locks up on read()...
    */

    use_bc = 0;

#elif defined(__sun)
   /*
    * CUPS STR #3028: Solaris' usbprn driver apparently does not support
    * select() or poll(), so we can't support backchannel...
    */

    use_bc = 0;

#else
   /*
    * Disable backchannel data when printing to Brother, Canon, or
    * Minolta USB printers - apparently these printers will return
    * the IEEE-1284 device ID over and over and over when they get
    * a read request...
    */

    use_bc = _cups_strcasecmp(hostname, "Brother") &&
             _cups_strcasecmp(hostname, "Canon") &&
             _cups_strncasecmp(hostname, "Konica", 6) &&
             _cups_strncasecmp(hostname, "Minolta", 7);
#endif /* __FreeBSD__ || __NetBSD__ || __OpenBSD__ || __DragonFly__ */

    if ((device_fd = open_device(uri, &use_bc)) == -1)
    {
      if (getenv("CLASS") != NULL)
      {
       /*
        * If the CLASS environment variable is set, the job was submitted
	* to a class and not to a specific queue.  In this case, we want
	* to abort immediately so that the job can be requeued on the next
	* available printer in the class.
	*/

        _cupsLangPrintFilter(stderr, "INFO",
			     _("Unable to contact printer, queuing on next "
			       "printer in class."));

       /*
        * Sleep 5 seconds to keep the job from requeuing too rapidly...
	*/

	sleep(5);

        return (CUPS_BACKEND_FAILED);
      }

      if (errno == EBUSY)
      {
        _cupsLangPrintFilter(stderr, "INFO",
			     _("Printer busy, will retry in 10 seconds."));
	sleep(10);
      }
      else if (errno == ENXIO || errno == EIO || errno == ENOENT ||
               errno == ENODEV)
      {
        _cupsLangPrintFilter(stderr, "INFO",
			     _("Printer not connected, will retry in 30 "
			       "seconds."));
	sleep(30);
      }
      else
      {
	_cupsLangPrintError("ERROR", _("Unable to open device file"));
	return (CUPS_BACKEND_FAILED);
      }
    }
  }
  while (device_fd < 0);

  fputs("STATE: -connecting-to-device\n", stderr);

  _canon_bj( argc, print_fd, device_fd, copies, argv[5] ) ;

 /*
  * Close the USB port and return...
  */

  close(device_fd);

  return (CUPS_BACKEND_OK);
}


/*
 * 'list_devices()' - List all USB devices.
 */

void
list_devices(void)
{
#ifdef __linux
  int	i;				/* Looping var */
  int	fd;				/* File descriptor */
  char	device[255],			/* Device filename */
	device_id[1024],		/* Device ID string */
	device_uri[1024],		/* Device URI string */
    make_model[1024],		/* Make and model */
    report_name[1024];


 /*
  * Try to open each USB device...
  */

  for (i = 0; i < 16; i ++)
  {
   /*
    * Linux has a long history of changing the standard filenames used
    * for USB printer devices.  We get the honor of trying them all...
    */

    sprintf(device, "/dev/usblp%d", i);

    if ((fd = open(device, O_RDWR | O_EXCL)) < 0)
    {
      if (errno != ENOENT)
	continue;

      sprintf(device, "/dev/usb/lp%d", i);

      if ((fd = open(device, O_RDWR | O_EXCL)) < 0)
      {
	if (errno != ENOENT)
	  continue;

	sprintf(device, "/dev/usb/usblp%d", i);

    	if ((fd = open(device, O_RDWR | O_EXCL)) < 0)
	  continue;
      }
    }

    if (!backendGetDeviceID(fd, device_id, sizeof(device_id),
                            make_model, sizeof(make_model),
                "canon-usb", device_uri, sizeof(device_uri))) {

        snprintf(report_name, 1024, "%s with status readback", make_model);
        cupsBackendReport("direct", device_uri, make_model, report_name,
                        device_id, NULL);
    }

    close(fd);
  }
#elif defined(__sgi)
#elif defined(__sun) && defined(ECPPIOC_GETDEVID)
  int	i;			/* Looping var */
  int	fd;			/* File descriptor */
  char	device[255],		/* Device filename */
	device_id[1024],	/* Device ID string */
	device_uri[1024],	/* Device URI string */
	make_model[1024];	/* Make and model */


 /*
  * Open each USB device...
  */

  for (i = 0; i < 8; i ++)
  {
    sprintf(device, "/dev/usb/printer%d", i);

    if ((fd = open(device, O_WRONLY | O_EXCL)) >= 0)
    {
      if (!backendGetDeviceID(fd, device_id, sizeof(device_id),
                              make_model, sizeof(make_model),
			      "canon-usb", device_uri, sizeof(device_uri)))
	cupsBackendReport("direct", device_uri, make_model, make_model,
			  device_id, NULL);

      close(fd);
    }
  }
#elif defined(__hpux)
#elif defined(__osf)
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__) || defined(__FreeBSD_kernel__)
  int   i;                      /* Looping var */
  char  device[255];            /* Device filename */


  for (i = 0; i < 8; i ++)
  {
    sprintf(device, "/dev/ulpt%d", i);
    if (!access(device, 0))
      printf("direct usb:%s \"Unknown\" \"USB Printer #%d\"\n", device, i + 1);

    sprintf(device, "/dev/unlpt%d", i);
    if (!access(device, 0))
      printf("direct usb:%s \"Unknown\" \"USB Printer #%d (no reset)\"\n", device, i + 1);
  }
#endif
}


/*
 * 'open_device()' - Open a USB device...
 */

static int				/* O - File descriptor or -1 on error */
open_device(const char *uri,		/* I - Device URI */
            int        *use_bc)		/* O - Set to 0 for unidirectional */
{
  int	fd;				/* File descriptor */


 /*
  * The generic implementation just treats the URI as a device filename...
  * Specific operating systems may also support using the device serial
  * number and/or make/model.
  */

  if (!strncmp(uri, "canon-usb:/dev/", 15))
#ifdef __linux
  {
   /*
    * Do not allow direct devices anymore...
    */

    errno = ENODEV;
    return (-1);
  }
  else if (!strncmp(uri, "canon-usb://", 12))
  {
   /*
    * For Linux, try looking up the device serial number or model...
    */

    int		i;			/* Looping var */
    int		busy;			/* Are any ports busy? */
    char	device[255],		/* Device filename */
		device_id[1024],	/* Device ID string */
		make_model[1024],	/* Make and model */
		device_uri[1024];	/* Device URI string */


   /*
    * Find the correct USB device...
    */

    for (;;)
    {
      for (busy = 0, i = 0; i < 16; i ++)
      {
       /*
	* Linux has a long history of changing the standard filenames used
	* for USB printer devices.  We get the honor of trying them all...
	*/

	sprintf(device, "/dev/usblp%d", i);

	if ((fd = open(device, O_RDWR | O_EXCL)) < 0 && errno == ENOENT)
	{
	  sprintf(device, "/dev/usb/lp%d", i);

	  if ((fd = open(device, O_RDWR | O_EXCL)) < 0 && errno == ENOENT)
	  {
	    sprintf(device, "/dev/usb/usblp%d", i);

    	    if ((fd = open(device, O_RDWR | O_EXCL)) < 0 && errno == ENOENT)
	      continue;
	  }
	}

	if (fd >= 0)
	{
	  backendGetDeviceID(fd, device_id, sizeof(device_id),
                             make_model, sizeof(make_model),
			     "canon-usb", device_uri, sizeof(device_uri));
	}
	else
	{
	 /*
	  * If the open failed because it was busy, flag it so we retry
	  * as needed...
	  */

	  if (errno == EBUSY)
	    busy = 1;

	  device_uri[0] = '\0';
        }

        if (!strcmp(uri, device_uri))
	{
	 /*
	  * Yes, return this file descriptor...
	  */

	  fprintf(stderr, "DEBUG: Printer using device file \"%s\"...\n",
		  device);

	  return (fd);
	}

       /*
	* This wasn't the one...
	*/

        if (fd >= 0)
	  close(fd);
      }

     /*
      * If we get here and at least one of the printer ports showed up
      * as "busy", then sleep for a bit and retry...
      */

      if (busy)
	_cupsLangPrintFilter(stderr, "INFO",
			     _("Printer is busy, will retry in 5 seconds."));

      sleep(5);
    }
  }
#elif defined(__sun) && defined(ECPPIOC_GETDEVID)
  {
   /*
    * Do not allow direct devices anymore...
    */

    errno = ENODEV;
    return (-1);
  }
  else if (!strncmp(uri, "canon-usb://", 6))
  {
   /*
    * For Solaris, try looking up the device serial number or model...
    */

    int		i;			/* Looping var */
    int		busy;			/* Are any ports busy? */
    char	device[255],		/* Device filename */
		device_id[1024],	/* Device ID string */
		make_model[1024],	/* Make and model */
		device_uri[1024];	/* Device URI string */


   /*
    * Find the correct USB device...
    */

    do
    {
      for (i = 0, busy = 0; i < 8; i ++)
      {
	sprintf(device, "/dev/usb/printer%d", i);

	if ((fd = open(device, O_WRONLY | O_EXCL)) >= 0)
	  backendGetDeviceID(fd, device_id, sizeof(device_id),
                             make_model, sizeof(make_model),
			     "canon-usb", device_uri, sizeof(device_uri));
	else
	{
	 /*
	  * If the open failed because it was busy, flag it so we retry
	  * as needed...
	  */

	  if (errno == EBUSY)
	    busy = 1;

	  device_uri[0] = '\0';
        }

        if (!strcmp(uri, device_uri))
	{
	 /*
	  * Yes, return this file descriptor...
	  */

          fputs("DEBUG: Setting use_bc to 0!\n", stderr);

          *use_bc = 0;

	  return (fd);
	}

       /*
	* This wasn't the one...
	*/

        if (fd >= 0)
	  close(fd);
      }

     /*
      * If we get here and at least one of the printer ports showed up
      * as "busy", then sleep for a bit and retry...
      */

      if (busy)
      {
	_cupsLangPrintFilter(stderr, "INFO",
			     _("Printer is busy, will retry in 5 seconds."));
	sleep(5);
      }
    }
    while (busy);

   /*
    * Couldn't find the printer, return "no such device or address"...
    */

    errno = ENODEV;

    return (-1);
  }
#else
  {
    if (*use_bc)
      fd = open(uri + 4, O_RDWR | O_EXCL);
    else
      fd = -1;

    if (fd < 0)
    {
      fd      = open(uri + 4, O_WRONLY | O_EXCL);
      *use_bc = 0;
    }

    return (fd);
  }
#endif /* __linux */
  else
  {
    errno = ENODEV;
    return (-1);
  }
}

/*
 * End of "$Id: usb-unix.c 9793 2011-05-20 03:49:49Z mike $".
 */
