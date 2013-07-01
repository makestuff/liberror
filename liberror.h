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
#ifndef LIBERROR_H
#define LIBERROR_H

#include <makestuff.h>

#ifdef __cplusplus
extern "C" {
#endif

	#undef CHECK_INTERNAL4
	#define CHECK_INTERNAL4(condition, value, label, prefix) \
		if ( condition ) { \
			errPrefix(error, prefix); \
			retVal = value; \
			goto label; \
		}

	#undef CHECK_INTERNAL5
	#define CHECK_INTERNAL5(condition, value, label, ...) \
		if ( condition ) { \
			errRender(error, __VA_ARGS__); \
			retVal = value; \
			goto label; \
		}

	// Render an error message into a newly-alloc'd buffer
	DLLEXPORT(void) errRender(const char **error, const char *format, ...);

	// Add a prefix to the existing error (reallocate & copy)
	DLLEXPORT(void) errPrefix(const char **error, const char *prefix);

	// Render the last libc error
	DLLEXPORT(void) errRenderStd(const char **error);

	// Deallocate an error string
	DLLEXPORT(void) errFree(const char *error);

#ifdef __cplusplus
}
#endif

#endif
