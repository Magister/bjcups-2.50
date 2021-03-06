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

/*** Includes ***/
#include <cups/cups.h>
#include <cups/ppd.h>
#include <cups/language.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

#include "bjcupsmon_common.h"
#include "bjcupsmon_cups.h"

#if (CUPS_VERSION_MAJOR > 1) || (CUPS_VERSION_MINOR > 5)
#define HAVE_CUPS_1_6 1
#endif

#ifndef HAVE_CUPS_1_6

#define ippSetOperation(ipp_request, ipp_op_id) ipp_request->request.op.operation_id = ipp_op_id
#define ippSetRequestId(ipp_request, ipp_rq_id) ipp_request->request.op.request_id = ipp_rq_id
#define ippGetStatusCode(ipp_request) ipp_request->request.status.status_code
#define ippFirstAttribute(ipp) ipp->attrs /* simplistic */
#define bjcups_ippNextAttribute(resp, attr) attr->next
#define ippGetGroupTag(attr)  attr->group_tag
#define ippGetName(attr) attr->name
#define ippGetString(attr, ind, lang) attr->values[ind].string.text
#define ippGetInteger(attr, ind) pAttribute->values[ind].integer
#define ippGetValueTag(attr) attr->value_tag

#else

#define bjcups_ippNextAttribute(resp, attr) ippNextAttribute(resp)

#endif


//////////////////////////////////////////////////////////////
//
// CS     :	PRIVATE cups_lang_t * bjcupsLangDefault()
// IN     : none
// OUT    : LC_ALL(locale)
// RETURN : pointer to cupa_lang_t structure
//
// This routine written for cups-1.1.19 Turbolinux10(Suzuka)
// Replace to cupsLangDefault()
//
PRIVATE cups_lang_t * bjcupsLangDefault( )
{
	cups_lang_t	*pLanguage;
	char		*tLang;

	if( (tLang = getenv("LC_ALL"))==NULL)
			tLang = getenv("LANG");

	pLanguage = cupsLangDefault();
	setlocale(LC_ALL,tLang);

	return pLanguage;
}

gint printerIsSupported(cups_option_t *options, gint num_options) {
	gint i;
	char *maker_and_model;
	char *uri;
	for (i = 0; i < num_options; i++) {
		if (strcmp(options[i].name, "printer-make-and-model") == 0) {
			maker_and_model = options[i].value;
		} else if (strcmp(options[i].name, "device-uri") == 0) {
			uri = options[i].value;
		}
	}
	if ((strcasestr(maker_and_model, "Canon PIXMA") || strcasestr(maker_and_model, "Canon PIXUS"))
	    && (strcasestr(uri, "canon-usb"))
	    ) {
		return 1;
	}
	return 0;
}

/*** Functions ***/
///////////////////////////////////////////////////////////////////////////////////////////
//
// CS     : PUBLIC gint getDefaultPrinterName(gchar *pDestName, gint bufSize)
// IN     : gint bufSize : Size of buffer for default printer name.
// OUT    : gchar *pDestName : Default printer name of CUPS.
// RETURN : ID_ERR_NO_ERROR : No error.
//          ID_ERR_CUPS_API_FAILED : Error occured in CUPS API.
//          ID_ERR_NO_PRINTER_ADDED : No printer registered into CUPS.
//
PUBLIC gint getDefaultPrinterName(gchar *pDestName, gint bufSize)
{
/*** Parameters start ***/
	cups_dest_t	*pDests;							// Destinations.
	gint		numDests;							// Number of destinations.
	gint		i;									// Counter.
	gint		retVal = ID_ERR_NO_PRINTER_ADDED;	// Return value.
/*** Parameters end ***/

	// Get all dests.
	numDests = cupsGetDests(&pDests);

	for (i = 0; i < numDests; i++) {
		if ((pDests[i].is_default != 0) && printerIsSupported(pDests[i].options, pDests[i].num_options)) {
			// Default printer found.
			strncpy(pDestName, pDests[i].name, bufSize);
			retVal = ID_ERR_NO_ERROR;
			break;
		}
	}

	if (retVal != ID_ERR_NO_ERROR) {
		// iterate over all printers and check if it is supported
		for (i = 0; i < numDests; i++) {
			if (printerIsSupported(pDests[i].options, pDests[i].num_options)) {
				strncpy(pDestName, pDests[i].name, bufSize);
				retVal = ID_ERR_NO_ERROR;
				break;
			}
		}
	}

	cupsFreeDests(numDests, pDests);

	return(retVal);
}// End getDefaultPrinterName


///////////////////////////////////////////////////////////////////////////////////////////
//
// CS     : PUBLIC gint getProductName(gchar *pDestName, gchar *pProductName)
// IN     : gchar *pDestName : Printer name.
// OUT    : gchar *pProductName : Product name. ( ModelName )
// RETURN : ID_ERR_NO_ERROR : No error.
//          ID_ERR_UNKNOWN_PRINTER : Getting product name failed.
//
PUBLIC gint getProductName(gchar *pDestName, gchar *pProductName)
{
/*** Parameters start ***/
	const gchar	*pPPDName = NULL;					// Pointer to PPD file name.
	ppd_file_t	*pPPD;								// Pointer to PPD file.
	gint		retVal = ID_ERR_UNKNOWN_PRINTER;	// Return value.
/*** Parameters start ***/

	// Get PPD file name.
	pPPDName = cupsGetPPD(pDestName);
	if (pPPDName != NULL) {
		if ((pPPD = ppdOpenFile(pPPDName)) != NULL) {
			if (strcmp(pPPD->manufacturer, STR_MANUFACTURER_NAME) == 0) {
				strncpy(pProductName, pPPD->modelname, strlen(pPPD->modelname));	// use modelname from 22th Jan.'03
				retVal = ID_ERR_NO_ERROR;
			}
			ppdClose(pPPD);
		}
		unlink(pPPDName);	// Tora 020418: You should remove the copy of the PPD file.
	}

	return(retVal);
}// End getProductName


///////////////////////////////////////////////////////////////////////////////////////////
//
// CS     : PUBLIC gint checkPrinterAndJobState(gchar *pDestName, gboolean *pPrinterReady, gboolean *pJobExist)
// IN     : gchar *pDestName : Printer name.
// OUT    : gboolean *pPrinterReady : Any printers registered into CUPS or not.
//          gboolean *pJobExist : Active job exists or not.
// RETURN : ID_ERR_NO_ERROR : No error.
//          ID_ERR_CUPS_API_FAILED : Error occured in CUPS API.
//
PUBLIC gint checkPrinterAndJobState(gchar *pDestName, gboolean *pPrinterReady, gboolean *pJobExist)
{
/*** Parameters start ***/
	gchar	printerURI[HTTP_MAX_URI];	// Printer URI.
	gchar	serverName[HTTP_MAX_URI];	// CUPS server name.
	gint	retVal = ID_ERR_NO_ERROR;	// Return value.
/*** Parameters end ***/

	snprintf(printerURI, sizeof(printerURI), "ipp://localhost/printers/%s", pDestName);
	strncpy(serverName, "localhost", HTTP_MAX_URI-1);

	// Check printer state.
	retVal = checkPrinterState(pDestName, printerURI, serverName);
	if (retVal == ID_ERR_NO_ERROR) {
		if (pPrinterReady != NULL) {
			*pPrinterReady = TRUE;
		}
	}
	else {
		if (pPrinterReady != NULL) {
			*pPrinterReady = FALSE;
		}
	}

	if (retVal == ID_ERR_NO_ERROR && pJobExist != NULL) {
		// Check active job exist.
		retVal = getJobID(pDestName, printerURI, serverName, NULL);
		if (retVal == ID_ERR_NO_ERROR) {
			*pJobExist = TRUE;
		}
		else {
			*pJobExist = FALSE;
		}
	}

	if (retVal == ID_ERR_UNKNOWN_PRINTER || retVal == ID_ERR_PRINT_JOB_NOT_EXIST) {
		retVal = ID_ERR_NO_ERROR;
	}

	return(retVal);
}// End getPrinterAndJobState


///////////////////////////////////////////////////////////////////////////////////////////
//
// CS     : PRIVATE gint checkPrinterState(gchar *pDestName, gchar *pURI, gchar *pServerName)
// IN     : gchar *pDestName : Printer name.
//          gchar *pURI : Printer URI.
//          gchar *pServerName : CUPS server name.
// OUT    : None.
// RETURN : ID_ERR_NO_ERROR : No error.
//          ID_ERR_UNKNOWN_PRINTER : No printer registerd in CUPS.
//          ID_ERR_CUPS_API_FAILED : Error occured in CUPS API.
//
PRIVATE gint checkPrinterState(gchar *pDestName, gchar *pURI, gchar *pServerName)
{
/*** Parameters start ***/
	http_t			*pHTTP;								// Pointer to HTTP connection.
	ipp_t			*pRequest,							// Pointer to CUPS IPP request.
					*pResponse;							// Pointer to CUPS IPP response.
	ipp_attribute_t	*pAttribute;						// Pointer to CUPS attributes.
	cups_lang_t		*pLanguage;							// Pointer to language.
	ipp_pstate_t	printerState = IPP_PRINTER_STOPPED;	// Pointer to printer state.
	gint			retVal = ID_ERR_UNKNOWN_PRINTER;	// Return value.
/*** Parameters end ***/

	// CUPS http connect.
	if ((pHTTP = httpConnectEncrypt(pServerName, ippPort(), cupsEncryption())) == NULL) {
		retVal = ID_ERR_CUPS_API_FAILED;
	}
	else {
		pRequest = ippNew();

		ippSetOperation(pRequest, IPP_GET_PRINTER_ATTRIBUTES);
		ippSetRequestId(pRequest, 1);

		pLanguage = bjcupsLangDefault();			// cupsLangDefault() -> bjcupsLangDefault() for cups-1.1.19

		ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_CHARSET, "attributes-charset", NULL, cupsLangEncoding(pLanguage));
		ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE, "attributes-natural-language", NULL, pLanguage->language);
		ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri", NULL, pURI);

		if ((pResponse = cupsDoRequest(pHTTP, pRequest, "/")) != NULL) {
			if (ippGetStatusCode(pResponse) > IPP_OK_CONFLICT) {
				retVal = ID_ERR_CUPS_API_FAILED;
			}
			else {
				if ((pAttribute = ippFindAttribute(pResponse, "printer-state", IPP_TAG_ENUM)) != NULL) {
					printerState = (ipp_state_t) ippGetInteger(pAttribute, 0);
				}
			}

			ippDelete(pResponse);
		}
		else {
			retVal = ID_ERR_CUPS_API_FAILED;
		}

		cupsLangFree(pLanguage);
		httpClose(pHTTP);
	}

	if (printerState == IPP_PRINTER_IDLE || printerState == IPP_PRINTER_PROCESSING) {
		retVal = ID_ERR_NO_ERROR;
	}

	return(retVal);
}// End checkPrinterState


///////////////////////////////////////////////////////////////////////////////////////////
//
// CS     : PUBLIC gint getPrinterStatus(gchar *pDestName, gchar *pStatus, gint bufSize)
// IN     : gchar *pDestName : Printer name.
//          gint bufSize : Size of output buffer.
// OUT    : gchar *pStatus : Printer status string.
// RETURN : ID_ERR_NO_ERROR : No error.
//          ID_ERR_CUPS_API_FAILED : Error occured in CUPS API.
//
PUBLIC gint getPrinterStatus(gchar *pDestName, gchar *pStatus, gint bufSize)
{
/*** Parameters start ***/
	http_t			*pHTTP;						// Pointer to HTTP connection.
	ipp_t			*pRequest,					// Pointer to CUPS IPP request.
					*pResponse;					// Pointer to CUPS IPP response.
	ipp_attribute_t	*pAttribute;				// Pointer to CUPS attributes.
	cups_lang_t		*pLanguage;					// Pointer to language.
	gchar			printerURI[HTTP_MAX_URI];	// Printer URI.
	gchar			serverName[HTTP_MAX_URI];	// CUPS server name.
	gint			retVal = ID_ERR_NO_ERROR;	// Return value.
/*** Parameters end ***/

	// Initialize buffer.
	memset(printerURI, 0, sizeof(printerURI));
	memset(serverName, 0, sizeof(serverName));

	// Get printer URI and CUPS server name.
	retVal = getPrinterURI(pDestName, printerURI, serverName, HTTP_MAX_URI);
	if (retVal == ID_ERR_NO_ERROR) {
		// CUPS http connect.
		if ((pHTTP = httpConnectEncrypt(serverName, ippPort(), cupsEncryption())) == NULL) {
			retVal = ID_ERR_CUPS_API_FAILED;
		}
		else {
			pRequest = ippNew();

			ippSetOperation(pRequest, IPP_GET_PRINTER_ATTRIBUTES);
			ippSetRequestId(pRequest, 1);

			pLanguage = bjcupsLangDefault();		// cupsLangDefault() -> bjcupsLangDefault() for cups-1.1.19

			ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_CHARSET, "attributes-charset", NULL, cupsLangEncoding(pLanguage));
			ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE, "attributes-natural-language", NULL, pLanguage->language);
			ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri", NULL, printerURI);

			if ((pResponse = cupsDoRequest(pHTTP, pRequest, "/")) != NULL) {
				if (ippGetStatusCode(pResponse) > IPP_OK_CONFLICT) {
					retVal = ID_ERR_CUPS_API_FAILED;
				}
				else {
					pAttribute = ippFindAttribute(pResponse, "printer-state-message", IPP_TAG_TEXT);
					if (pAttribute != NULL) {
						strncpy(pStatus, ippGetString(pAttribute, 0, NULL), bufSize);
					}
				}
				ippDelete(pResponse);
			}
			else {
				retVal = ID_ERR_CUPS_API_FAILED;
			}

			cupsLangFree(pLanguage);
			httpClose(pHTTP);
		}
	}

	return(retVal);
}// End getPrinterStatus


///////////////////////////////////////////////////////////////////////////////////////////
//
// CS     : PUBLIC gint removeJob(gchar *pDestName)
// IN     : gchar *pDestName : Printer name.
// OUT    : None.
// RETURN : ID_ERR_NO_ERROR : No error.
//          ID_ERR_CUPS_API_FAILED : Error occured in CUPS API.
//
PUBLIC gint removeJob(gchar *pDestName)
{
/*** Parameters start ***/
	http_t			*pHTTP;						// Pointer to HTTP connection.
	ipp_t			*pRequest,					// Pointer to CUPS IPP request.
					*pResponse;					// Pointer to CUPS IPP response.
	cups_lang_t		*pLanguage;					// Pointer to language.
	gchar			printerURI[HTTP_MAX_URI];	// Printer URI.
	gchar			serverName[HTTP_MAX_URI];	// CUPS server name.
	gint			jobID = 0;					// Job ID.
	gint			retVal = ID_ERR_NO_ERROR;	// Return value.
/*** Parameters end ***/

	// Initialize buffer.
	memset(printerURI, 0, sizeof(printerURI));
	memset(serverName, 0, sizeof(serverName));

	// Get printer URI and CUPS server name.
	retVal = getPrinterURI(pDestName, printerURI, serverName, HTTP_MAX_URI);
	if (retVal == ID_ERR_NO_ERROR) {
		retVal = getJobID(pDestName, printerURI, serverName, &jobID);
		if (retVal == ID_ERR_PRINT_JOB_NOT_EXIST) {
			retVal = ID_ERR_NO_ERROR;
		}

		if (retVal == ID_ERR_NO_ERROR) {
			// CUPS http connect.
			if ((pHTTP = httpConnectEncrypt(serverName, ippPort(), cupsEncryption())) == NULL) {
				retVal = ID_ERR_CUPS_API_FAILED;
			}
			else {
				pRequest = ippNew();

				ippSetOperation(pRequest, IPP_CANCEL_JOB);
				ippSetRequestId(pRequest, 1);

				pLanguage = bjcupsLangDefault();		// cupsLangDefault() -> bjcupsLangDefault() for cups-1.1.19

				ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_CHARSET, "attributes-charset", NULL, cupsLangEncoding(pLanguage));
				ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE, "attributes-natural-language", NULL, pLanguage->language);
				ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri", NULL, printerURI);
				ippAddInteger(pRequest, IPP_TAG_OPERATION, IPP_TAG_INTEGER, "job-id", jobID);
				ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_NAME, "requesting-user-name", NULL, cupsUser());

				if ((pResponse = cupsDoRequest(pHTTP, pRequest, "/jobs/")) != NULL) {
					if (ippGetStatusCode(pResponse) > IPP_OK_CONFLICT) {
						retVal = ID_ERR_CUPS_API_FAILED;
					}
					ippDelete(pResponse);
				}
				else {
					retVal = ID_ERR_CUPS_API_FAILED;
				}

				cupsLangFree(pLanguage);
				httpClose(pHTTP);
			}
		}
	}

	return(retVal);
}// End removeJob

PRIVATE gchar * getJobState(gchar *pDestName, const gchar *pURI, gchar *pServerName, ipp_jstate_t *pJobState, gchar *pJobUserName)
{
/*** Parameters start ***/
	http_t			*pHTTP;									// Pointer to HTTP connection.
	ipp_t			*pRequest,								// Pointer to CUPS IPP request.
					*pResponse;								// Pointer to CUPS IPP response.
	ipp_attribute_t	*pAttribute;							// Pointer to CUPS attributes.
	cups_lang_t		*pLanguage;								// Pointer to language.
//	ipp_jstate_t	jobState = 0;							// Job state.
//	gint			jobID = 0;								// Job ID.
//	gchar			*pJobUserName = NULL;					// User name of print job.
	uid_t			userID;									// User ID.
	struct passwd	*pPasswd;								// Pointer to password structure.
	gint			retVal = ID_ERR_PRINT_JOB_NOT_EXIST;	// Return value.
/*** Parameters end ***/

	// Get login name.
	userID = getuid();
	pPasswd = getpwuid(userID);

	// CUPS http connect.
	if ((pHTTP = httpConnectEncrypt(pServerName, ippPort(), cupsEncryption())) == NULL) {
		retVal = ID_ERR_CUPS_API_FAILED;
	}
	else {
		pRequest = ippNew();

		ippSetOperation(pRequest, IPP_GET_JOB_ATTRIBUTES);
		ippSetRequestId(pRequest, 1);

		pLanguage = bjcupsLangDefault();	// cupsLangDefault() -> bjcupsLangDefault() for cups-1.1.19

		ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_CHARSET, "attributes-charset", NULL, cupsLangEncoding(pLanguage));
		ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE, "attributes-natural-language", NULL, pLanguage->language);
		ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_URI, "job-uri", NULL, pURI);
		ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_NAME, "requesting-user-name", NULL, pPasswd->pw_name);

		if ((pResponse = cupsDoRequest(pHTTP, pRequest, "/")) != NULL) {
			if (ippGetStatusCode(pResponse) > IPP_OK_CONFLICT) {
				retVal = ID_ERR_CUPS_API_FAILED;
			}
			else {
				pAttribute = ippFirstAttribute(pResponse);

				while (pAttribute != NULL) {
					while (pAttribute != NULL && ippGetGroupTag(pAttribute) != IPP_TAG_JOB) {
						pAttribute = bjcups_ippNextAttribute(pResponse, pAttribute);
					}
					if (pAttribute == NULL) {
						break;
					}

					while (pAttribute != NULL && ippGetGroupTag(pAttribute) == IPP_TAG_JOB) {
						if (strcmp(ippGetName(pAttribute), "job-state") == 0 && ippGetValueTag(pAttribute) == IPP_TAG_ENUM) {
							*pJobState = (ipp_jstate_t)ippGetInteger(pAttribute, 0);
						}
						if (strcmp(ippGetName(pAttribute), "job-originating-user-name") == 0 && ippGetValueTag(pAttribute) == IPP_TAG_NAME) {
							//pJobUserName = pAttribute->values[0].string.text;
							pJobUserName=malloc(20*sizeof(gchar));
							strncpy(pJobUserName,ippGetString(pAttribute, 0, NULL), 19);
							pJobUserName[19]='\0';

						}
						pAttribute = bjcups_ippNextAttribute(pResponse, pAttribute);
					}
					if (pAttribute != NULL)
						pAttribute = bjcups_ippNextAttribute(pResponse, pAttribute);
				}
			}

			ippDelete(pResponse);
		}
		else {
			retVal = ID_ERR_CUPS_API_FAILED;
		}

		cupsLangFree(pLanguage);
		httpClose(pHTTP);
	}


	return(pJobUserName);
}// End getJobState

///////////////////////////////////////////////////////////////////////////////////////////
//
// CS     : PRIVATE gint getJobID(gchar *pDestName, gchar *pURI, gchar *pServerName, gint *pJobID)
// IN     : gchar *pDestName : Printer name.
//          gchar *pURI : Printer URI.
//          gchar *pServerName : CUPS server name.
// OUT    : gint *pJobID : Job ID.
// RETURN : ID_ERR_NO_ERROR : No error.
//          ID_ERR_CUPS_API_FAILED : Error occured in CUPS API.
//
PRIVATE gint getJobID(gchar *pDestName, gchar *pURI, gchar *pServerName, gint *pJobID)
{
/*** Parameters start ***/
	http_t			*pHTTP;									// Pointer to HTTP connection.
	ipp_t			*pRequest,								// Pointer to CUPS IPP request.
					*pResponse;								// Pointer to CUPS IPP response.
	ipp_attribute_t	*pAttribute;							// Pointer to CUPS attributes.
	cups_lang_t		*pLanguage;								// Pointer to language.
	ipp_jstate_t	jobState = 0;							// Job state.
	gint			jobID = 0;								// Job ID.
	gchar			*pJobUserName = NULL;					// User name of print job.
	uid_t			userID;									// User ID.
	struct passwd	*pPasswd;								// Pointer to password structure.
	gint			retVal = ID_ERR_PRINT_JOB_NOT_EXIST;	// Return value.
/*** Parameters end ***/

	// Get login name.
	userID = getuid();
	pPasswd = getpwuid(userID);

	// CUPS http connect.
	if ((pHTTP = httpConnectEncrypt(pServerName, ippPort(), cupsEncryption())) == NULL) {
		retVal = ID_ERR_CUPS_API_FAILED;
	}
	else {
		pRequest = ippNew();

		ippSetOperation(pRequest, IPP_GET_JOBS);
		ippSetRequestId(pRequest, 1);

		pLanguage = bjcupsLangDefault();	// cupsLangDefault() -> bjcupsLangDefault() for cups-1.1.19

		ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_CHARSET, "attributes-charset", NULL, cupsLangEncoding(pLanguage));
		ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE, "attributes-natural-language", NULL, pLanguage->language);
		ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri", NULL, pURI);
		ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_NAME, "requesting-user-name", NULL, cupsUser());

		if ((pResponse = cupsDoRequest(pHTTP, pRequest, "/")) != NULL) {
            int statusCode = ippGetStatusCode(pResponse);
			if (statusCode > IPP_OK_CONFLICT) {
				retVal = ID_ERR_CUPS_API_FAILED;
			}
			else {
				pAttribute = ippFirstAttribute(pResponse);

				while (pAttribute != NULL) {
					while (pAttribute != NULL && ippGetGroupTag(pAttribute) != IPP_TAG_JOB) {
						pAttribute = bjcups_ippNextAttribute(pResponse, pAttribute);
					}
					if (pAttribute == NULL) {
						break;
					}

					while (pAttribute != NULL && ippGetGroupTag(pAttribute) == IPP_TAG_JOB) {
						if (strcmp(ippGetName(pAttribute), "job-id") == 0 && ippGetValueTag(pAttribute) == IPP_TAG_INTEGER) {
							jobID = ippGetInteger(pAttribute, 0);
						}
						if (strcmp(ippGetName(pAttribute), "job-uri") == 0 && ippGetValueTag(pAttribute) == IPP_TAG_URI) {
							pJobUserName=getJobState(pDestName,ippGetString(pAttribute, 0, NULL),pServerName,&jobState,pJobUserName);
						}
						pAttribute = bjcups_ippNextAttribute(pRequest, pAttribute);
					}
					if (jobState == IPP_JOB_PROCESSING) {
						if (pJobUserName != NULL) {
							if (strcmp(pPasswd->pw_name, pJobUserName) == 0) {
								retVal = ID_ERR_NO_ERROR;
							}
							else if (pJobUserName[0] == '\0') {
								retVal = ID_ERR_NO_ERROR;
							}
						}
						break;
					}

					if (pAttribute != NULL)
						pAttribute = bjcups_ippNextAttribute(pRequest, pAttribute);
				}
			}

			ippDelete(pResponse);
		}
		else {
			retVal = ID_ERR_CUPS_API_FAILED;
		}

		cupsLangFree(pLanguage);
		httpClose(pHTTP);
	}

	if (retVal == ID_ERR_NO_ERROR && pJobID != NULL) {
		*pJobID = jobID;
	}

	return(retVal);
}// End getJobID


///////////////////////////////////////////////////////////////////////////////////////////
//
// CS     : PRIVATE gint getPrinterURI(gchar *pDestName, gchar *pURI, gchar *pServerName, gint bufSize)
// IN     : gchar *pDestName : Printer name.
//          gint bufSize : Size of output buffer.
// OUT    : gchar *pURI : Printer URI.
//          gchar *pServerName : Server name.
// RETURN : ID_ERR_NO_ERROR : No error.
//          ID_ERR_CUPS_API_FAILED : Error occured in CUPS API.
//
PRIVATE gint getPrinterURI(gchar *pDestName, gchar *pURI, gchar *pServerName, gint bufSize)
{
/*** Parameters start ***/
	http_t			*pHTTP;						// Pointer to HTTP connection.
	ipp_t			*pRequest,					// Pointer to CUPS IPP request.
					*pResponse;					// Pointer to CUPS IPP response.
	ipp_attribute_t	*pAttribute;				// Pointer to CUPS attributes.
	cups_lang_t		*pLanguage;					// Pointer to language.
	gchar			*pPrinter = NULL;			// Pointer to printer name.
	gchar			*pUri = NULL;				// Pointer to printer uri.
	gchar			*pTemp = NULL;				// Temporary pointer.
	gint			i;							// Counter.
	gint			retVal = ID_ERR_NO_ERROR;	// Return value.
	const char		*attributes[] = {			// Attributes name set.
						"printer-name",
						"printer-uri-supported",
					};
/*** Parameters end ***/

	// CUPS http connect.
	if ((pHTTP = httpConnectEncrypt(cupsServer(), ippPort(), cupsEncryption())) == NULL) {
		retVal = ID_ERR_CUPS_API_FAILED;
	}
	else {
		pRequest = ippNew();

		ippSetOperation(pRequest, CUPS_GET_PRINTERS);
		ippSetRequestId(pRequest, 1);

		pLanguage = bjcupsLangDefault();	// cupsLangDefault() -> bjcupsLangDefault() for cups-1.1.19

		ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_CHARSET, "attributes-charset", NULL, cupsLangEncoding(pLanguage));
		ippAddString(pRequest, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE, "attributes-natural-language", NULL, pLanguage->language);
		ippAddStrings(pRequest, IPP_TAG_OPERATION, IPP_TAG_KEYWORD, "requested-attributes", sizeof(attributes) / sizeof(attributes[0]), NULL, attributes);

		if ((pResponse = cupsDoRequest(pHTTP, pRequest, "/")) != NULL) {
			if (ippGetStatusCode(pResponse) > IPP_OK_CONFLICT) {
				retVal = ID_ERR_CUPS_API_FAILED;
			}
			else {
				pAttribute = ippFirstAttribute(pResponse);

				while (pAttribute != NULL) {
					while (pAttribute != NULL && ippGetGroupTag(pAttribute) != IPP_TAG_PRINTER) {
						pAttribute = bjcups_ippNextAttribute(pRequest, pAttribute);
					}
					if (pAttribute == NULL) {
						break;
					}

					while (pAttribute != NULL && ippGetGroupTag(pAttribute) == IPP_TAG_PRINTER) {
						if (strcmp(ippGetName(pAttribute), "printer-name") == 0 && ippGetValueTag(pAttribute) == IPP_TAG_NAME) {
							pPrinter = ippGetString(pAttribute, 0, NULL);
						}
						if (strcmp(ippGetName(pAttribute), "printer-uri-supported") == 0 && ippGetValueTag(pAttribute) == IPP_TAG_URI) {
							pUri = ippGetString(pAttribute, 0, NULL);
						}
						pAttribute = bjcups_ippNextAttribute(pRequest, pAttribute);
					}

					// Tora 020418: Compare two printer names ignoring the character case.
					if (strcasecmp(pDestName, pPrinter) == 0) {
						strncpy(pURI, pUri, bufSize);

						pTemp = strstr(pURI, "//");
						pTemp += 2;
						for (i = 0; *pTemp != '/' && *pTemp != ':'; i++, pTemp++) {
							pServerName[i] = *pTemp;
						}

						break;
					}

					if (pAttribute != NULL)
						 pAttribute = bjcups_ippNextAttribute(pRequest, pAttribute);
				}
			}

			ippDelete(pResponse);
		}
		else {
			retVal = ID_ERR_CUPS_API_FAILED;
		}

		cupsLangFree(pLanguage);
		httpClose(pHTTP);
	}

	if (pURI[0] == '\0') {
		snprintf(pURI, bufSize, "ipp://localhost/printers/%s", pDestName);
	}
	if (pServerName[0] == '\0') {
		strncpy(pServerName, "localhost", strlen("localhost"));
	}

	return(retVal);
}// End getPrinterURI
