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
#include <UnitTest++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../liberror.h"

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

TEST(Error_testNullRender) {
	fillBuf(16, 'A');
	errRender(NULL, buf);
}

TEST(Error_testRender) {
	int i;
	const char *error;
	for ( i = 500; i < 516; i++ ) {
		error = NULL;
		fillBuf(i, 'A');
		errRender(&error, buf);
		CHECK_EQUAL(buf, error);
		errFree(error);
	}
}

TEST(Error_testNullStdRender) {
	fillBuf(16, 'A');
	errRenderStd(NULL);
}

TEST(Error_testStdRender) {
	const char *error = NULL, *expected;
	FILE *f;
	f = fopen("nonExistentFile.txt", "r");
	CHECK(!f);
	errRenderStd(&error);
	f = fopen("nonExistentFile.txt", "r");
	CHECK(!f);
	expected = strerror(errno);
	CHECK_EQUAL(expected, error);
	errFree(error);
}

TEST(Error_testPrefixSomething) {
	#define MSG "Foo Bar"
	#define PFX "myPrefix()"
	const char *error = NULL;
	const char *const expected = PFX ": " MSG;
	errRender(&error, MSG);
	CHECK_EQUAL(MSG, error);
	errPrefix(&error, PFX);
	CHECK_EQUAL(expected, error);
	errFree(error);
}

TEST(Error_testPrefixNothing) {
	const char *error = NULL;
	const char *const expected = "myPrefix(): Foo Bar";
	errPrefix(&error, expected);
	CHECK_EQUAL(expected, error);
	errFree(error);
}

TEST(Error_testPrefixNull) {
	const char *error = NULL;
	errPrefix(NULL, "Foo");
	errPrefix(&error, NULL);
	CHECK(!error);
}
