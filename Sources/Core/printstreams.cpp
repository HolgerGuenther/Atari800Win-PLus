/****************************************************************************
File    : printstreams.c
/*
@(#) #SY# Atari800Win PLus
@(#) #IS# Implementation of printer output streams
@(#) #BY# Holger Guenther
@(#) #LM# 21.01.2016
*/

#include "StdAfx.h"  /* Must be included first if precompiled headers are used */

#include <string.h>
#include <winsock2.h>
#include "util.h"
#include "printstreams.h"

/*========================================================
CCommonPrintStream: Abstract base class print streams
=========================================================*/

class CCommonPrintStream
{
private:

    char FParam[PRINT_PARAMS_LENGTH + 1];   /* String param passed to constructor */
    int FConvertToText;                     /* If nonzero, ATASCII linefeeds (0x9b) are converted to '\n' */

protected:

    virtual void OpenStream() = 0;
    virtual void CloseStream() = 0;
    virtual BOOL IsStreamOpen() = 0;        /* returns nonzero if stream is open */
    virtual BOOL PutByte (UBYTE b) = 0;     /* returns nonzero if successful */

public:

    CCommonPrintStream (char * AParam, BOOL AConvertToText)
    {
        strncpy (FParam, AParam, PRINT_PARAMS_LENGTH);
        FConvertToText = AConvertToText;
    }

    virtual ~CCommonPrintStream()
    {
    }

    BOOL Open()
    {
        Close();
        OpenStream();
        return IsOpen();
    }

    void Close()
    {
        if (IsOpen()) CloseStream();
    }

    BOOL IsOpen()
    {
        return IsStreamOpen();
    }

    BOOL WriteByte (UBYTE Value)
    {
        if (!IsStreamOpen()) return FALSE;
        if (FConvertToText)
            if (Value == 0x9b)
                Value = '\n';
        return PutByte (Value);
    }

    void GetParam (char * Result, int MaxLen)
    {
        strncpy (Result, FParam, PRINT_PARAMS_LENGTH < MaxLen ? PRINT_PARAMS_LENGTH : MaxLen);
    }

    BOOL ConvertToText()
    {
        return FConvertToText;
    }
};


/*========================================================
CFilePrintStream: Sends print output to a file
=========================================================*/

class CFilePrintStream : public CCommonPrintStream
{
private:

    FILE * FFile;
    char FFilename[FILENAME_MAX];

protected:

    virtual void OpenStream() override
    {
        FFile = Util_uniqopen (FFilename, "w");
    }

    virtual void CloseStream() override
    {
  		fclose (FFile);
	    FFile = NULL;
    }

    virtual BOOL IsStreamOpen() override
    {
       	return (FFile != NULL);
    }

    virtual BOOL PutByte (UBYTE b) override
    {
       	return fputc (b, FFile) != EOF;
    }

public:

    CFilePrintStream (char * ACommand, BOOL AConvertToText)
        : CCommonPrintStream (ACommand, AConvertToText)
    {
        FFile = NULL;
        FFilename[0] = '\0';
    }

    virtual ~CFilePrintStream()
    {
        Close();
    }

    void GetFilename (char * Result, int MaxLen)
    {
        strncpy (Result, FFilename, MaxLen < FILENAME_MAX ? MaxLen : FILENAME_MAX);
    }
};


/*========================================================
C-style API
=========================================================*/

PrintStream CreateFilePrintStream (char * Filename, BOOL ConvertToText)
{
    CFilePrintStream * strm = new CFilePrintStream (Filename, ConvertToText);
    return (PrintStream) strm;
}

PrintStream CreateTCPPrintStream (char * HostAndPort, BOOL ConvertToText)
{
    return NULL;
}

void PrintStreamRelease (PrintStream * StreamPtr)
{
    if (!StreamPtr) return;
    CCommonPrintStream * strm = (CCommonPrintStream*) *StreamPtr;
    if (!strm) return;
    delete strm;
    *StreamPtr = NULL;
}

BOOL PrintStreamOpen (PrintStream Stream)
{
    if (!Stream) return FALSE;
    return ((CCommonPrintStream*) Stream)->Open();
}

void PrintStreamClose (PrintStream Stream)
{
    if (!Stream) return;
    ((CCommonPrintStream*) Stream)->Close();
}

BOOL PrintStreamWriteByte (PrintStream Stream, UBYTE Value)
{
    if (!Stream) return FALSE;
    return ((CCommonPrintStream*) Stream)->WriteByte (Value);
}

void PrintStreamProcessSpoolFile (PrintStream Stream)
{
    CFilePrintStream * strm = dynamic_cast <CFilePrintStream*> ((CCommonPrintStream*) Stream);
    if (strm)
    {
        char cmdformat[PRINT_PARAMS_LENGTH + 1];
        char spoolfile[FILENAME_MAX + 1];
        strm->GetParam (cmdformat, PRINT_PARAMS_LENGTH);
        strm->GetFilename (spoolfile, FILENAME_MAX);

#ifdef __PLUS
		if (!Misc_ExecutePrintCmd (cmdformat, spoolfile))
#endif
		{
            if (Devices_IsValidPrintCommand (cmdformat) && strlen (spoolfile) > 0)
            {
			    char command[PRINT_PARAMS_LENGTH + FILENAME_MAX]; /* PRINT_PARAMS_LENGTH for Devices_print_params + FILENAME_MAX for spool_file */
			    int retval;
			    sprintf (command, cmdformat, spoolfile);
			    retval = system (command);
#if defined(HAVE_UTIL_UNLINK) && !defined(VMS) && !defined(MACOSX)
			    if (Util_unlink(spoolfile) != 0) {
			    	perror(spoolfile);
			    }
#endif
            }
		}
	}
    _ClrFlag( g_ulAtariState, ATARI_PAUSED );
}

void PrintStreamDeleteSpoolFile (PrintStream Stream)
{
    CFilePrintStream * strm = dynamic_cast <CFilePrintStream*> ((CCommonPrintStream*) Stream);
    if (strm)
    {
        char spoolfile[FILENAME_MAX + 1];
        strm->GetFilename (spoolfile, FILENAME_MAX);

#ifdef HAVE_UTIL_UNLINK
        Util_unlink (spoolfile);
#endif
    }
}
