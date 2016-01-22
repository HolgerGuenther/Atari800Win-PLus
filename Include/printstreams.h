/****************************************************************************
File    : printstreams.h
/*
@(#) #SY# Atari800Win PLus
@(#) #IS# File and network streams for printer output
@(#) #BY# Holger Guenther
@(#) #LM# 21.01.2016
*/

/*
Note: Although the implementation of print streams is based
on C++ classes, it uses here a C-like API style.
This is for compatibility with the old Atari800 C sources,
so that the streams can be directly used by the P: device
in devices.c.
*/

#ifndef __PRINTSTREAMS_H__
#define __PRINTSTREAMS_H__

#include "atari.h"

#ifdef __cplusplus
extern "C" {
#endif

/* declare a handle type for a print stream */
typedef void * PrintStream;

/* Create a print stream */
PrintStream CreateFilePrintStream (char * Command, BOOL ConvertToText);
PrintStream CreateTCPPrintStream (char * HostAndPort, BOOL ConvertToText);

/* Release a print stream */
void PrintStreamRelease (PrintStream * StreamPtr);

/* Open, close and write a print stream */
BOOL PrintStreamOpen (PrintStream Stream);
void PrintStreamClose (PrintStream Stream);
BOOL PrintStreamWriteByte (PrintStream Stream, UBYTE Value);

/* Executes the print command in case of Stream is a File stream */
void PrintStreamProcessSpoolFile (PrintStream Stream);

/* Removes the spool file, if any */
void PrintStreamDeleteSpoolFile (PrintStream Stream);

#ifdef __cplusplus
}
#endif

#endif  /* __PRINTSTREAMS_H__ */
