/* 
 * Copyright (C) 2010 Chris McClelland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <UnitTest++.h>
#include <stdlib.h>
#include "../error.h"

char buf[1024];

void fillBuf(int count, char value) {
	int i = 0;
	while ( i < count ) {
		buf[i++] = value;
	}
	buf[i++] = '\0';
	while ( i < 1024 ) {
		buf[i++] = '.';
	}
}

TEST(Error_testRender) {
	int i;
	const char *error;
	for ( i = 500; i < 516; i++ ) {
		fillBuf(i, 'A');
		error = renderError(buf);
		CHECK_EQUAL(buf, error);
		free((void*)error);
	}
}
