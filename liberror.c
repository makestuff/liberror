/*
 * Copyright (C) 2009-2012 Chris McClelland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef WIN32
	#ifdef __MINGW32__
		#include <stddef.h>
		#include <stdio.h>
		#include <errno.h>
		__MINGW_IMPORT int _sys_nerr;
		#define sys_nerr _sys_nerr
		__MINGW_IMPORT char* _sys_errlist[];
		#define sys_errlist _sys_errlist
		static int strerror_r(int errCode, char *buffer, size_t bufSize) {
			size_t numBytes;
			if ( errCode < sys_nerr ) {
				numBytes = snprintf(buffer, bufSize, "%s", sys_errlist[errCode]);
				return (numBytes >= bufSize) ? ERANGE : 0;
			} else {
				numBytes = snprintf(buffer, bufSize, "Unknown error %d", errCode);
				return EINVAL;
			}
		}
	#elif defined(_MSC_VER)
		#define strerror_r(err, buf, size) strerror_s(buf, size, err)
	#endif
#else
	#undef _GNU_SOURCE
	#define _XOPEN_SOURCE 600
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "liberror.h"

// Code inspired by http://linux.die.net/man/3/snprintf
//
DLLEXPORT(void) errRender(const char **error, const char *format, ...) {
	if ( error ) {
		// Guess we need no more than 512 bytes
		int status;
		size_t size = 512;
		char *bufPtr, *newBufPtr;
		va_list argList;
		assert(*error == NULL);
		bufPtr = (char*)malloc(size);
		if ( bufPtr == NULL ) {
			// Insufficient memory
			*error = NULL;
			return;
		}
		for ( ; ; ) {
			// Try to print in the allocated space
			va_start(argList, format);
			status = vsnprintf(bufPtr, size, format, argList);
			va_end(argList);
			
			// If that worked, return the string
			if ( status != -1 && (size_t)status < size ) {
				*error = bufPtr;
				return;
			}
			
			// Else try again with more space
			if ( status == -1 ) {
				// vsnprintf() in glibc 2.0 and MSVC not C99-compliant: returns -1 if buf too small
				size *= 2;
			} else {
				// vsnprintf() in glibc 2.1 is C99-compliant: returns the exact no. of bytes needed
				size = (size_t)status + 1;
			}
			newBufPtr = (char*)realloc(bufPtr, size);
			if ( newBufPtr == NULL ) {
				free(bufPtr);
				*error = NULL;
				return;
			} else {
				bufPtr = newBufPtr;
			}
		}
	}
}

// Add a prefix to the front of the error by reallocating & copying
//
DLLEXPORT(void) errPrefix(const char **error, const char *prefix) {
	if ( !prefix || !error ) {
		return;
	}
	if ( *error ) {
		// There's already something there - prefix it
		char *newError, *p;
		const size_t pLen = strlen(prefix); // ": " and null terminator
		const size_t len = pLen + strlen(*error) + 3; // ": " and null terminator
		p = newError = (char*)malloc(len);
		if ( newError == NULL ) {
			errFree(*error);
			*error = NULL;
			return;
		}
		strcpy(p, prefix);
		p += pLen;
		*p++ = ':';
		*p++ = ' ';
		strcpy(p, *error);
		errFree(*error);
		*error = newError;
	} else {
		// Nothing is already there, so just copy
		char *newError = (char*)malloc(strlen(prefix) + 1);
		strcpy(newError, prefix);
		*error = newError;
	}
}

// Render the last libC error in the same way that errRender() does
//
DLLEXPORT(void) errRenderStd(const char **error) {
	if ( error ) {
		// Guess we need no more than 512 bytes
		int status;
		size_t size = 512;
		char *bufPtr, *newBufPtr;
		const int errSave = errno;
		bufPtr = (char*)malloc(size);
		if ( bufPtr == NULL ) {
			// Insufficient memory
			*error = NULL;
			return;
		}
		for ( ; ; ) {
			// Try to print in the allocated space
			status = strerror_r(errSave, bufPtr, size);
			if ( status == 0 ) {
				// Yay, it fits! (WIN32 comes through here even if the message was truncated...doh)
				*error = bufPtr;
				return;
			} else if ( status == -1 && errno == ERANGE ) {
				// It doesn't fit...resize buffer and try again
				size *= 2;
			} else {
				// Some other problem...invalid errno perhaps?
				*error = NULL;
				return;
			}
			newBufPtr = (char*)realloc(bufPtr, size);
			if ( newBufPtr == NULL ) {
				free(bufPtr);
				*error = NULL;
				return;
			} else {
				bufPtr = newBufPtr;
			}
		}
	}
}

DLLEXPORT(void) errFree(const char *error) {
	free((void*)error);
}
