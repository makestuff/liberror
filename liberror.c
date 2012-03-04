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
	#define strerror_r(err, buf, size) strerror_s(buf, size, err)
#else
	#undef _GNU_SOURCE
	#define _XOPEN_SOURCE 600
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "liberror.h"

// Code inspired by http://linux.die.net/man/3/snprintf
//
DLLEXPORT(void) errRender(const char **error, const char *format, ...) {
	if ( error ) {
		// Guess we need no more than 512 bytes
		int returnCode, size = 512;
		char *bufPtr, *newBufPtr;
		va_list argList;
		bufPtr = (char*)malloc(size);
		if ( bufPtr == NULL ) {
			// Insufficient memory
			*error = NULL;
			return;
		}
		for ( ; ; ) {
			// Try to print in the allocated space
			va_start(argList, format);
			returnCode = vsnprintf(bufPtr, size, format, argList);
			va_end(argList);
			
			// If that worked, return the string
			if ( returnCode != -1 && returnCode < size ) {
				*error = bufPtr;
				return;
			}
			
			// Else try again with more space
			if ( returnCode == -1 ) {
				// vsnprintf() in glibc 2.0 and MSVC not C99-compliant: returns -1 if buf too small
				size *= 2;
			} else {
				// vsnprintf() in glibc 2.1 is C99-compliant: returns the exact no. of bytes needed
				size = returnCode + 1;
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
	if ( error && prefix ) {
		char *newError, *p;
		const int pLen = strlen(prefix); // ": " and null terminator
		const int len = pLen + strlen(*error) + 3; // ": " and null terminator
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
	}
}

// Render the last libC error in the same way that errRender() does
//
DLLEXPORT(void) errRenderStd(const char **error) {
	if ( error ) {
		// Guess we need no more than 512 bytes
		int returnCode, size = 512;
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
			returnCode = strerror_r(errSave, bufPtr, size);
			if ( returnCode == 0 ) {
				// Yay, it fits! (WIN32 comes through here even if the message was truncated...doh)
				*error = bufPtr;
				return;
			} else if ( returnCode == -1 && errno == ERANGE ) {
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
