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
#include <gtest/gtest.h>
#include <makestuff/liberror.h>

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

TEST(Error, testNullRender) {
	fillBuf(16, 'A');
	errRender(NULL, buf);
}

TEST(Error, testRender) {
	int i;
	const char *error;
	for ( i = 500; i < 516; i++ ) {
		error = NULL;
		fillBuf(i, 'A');
		errRender(&error, buf);
		ASSERT_STREQ(buf, error);
		errFree(error);
	}
}

TEST(Error, testNullStdRender) {
	fillBuf(16, 'A');
	errRenderStd(NULL);
}

TEST(Error, testStdRender) {
	const char *error = NULL, *expected;
	FILE *f;
	f = fopen("nonExistentFile.txt", "r");
	ASSERT_FALSE(f);
	errRenderStd(&error);
	f = fopen("nonExistentFile.txt", "r");
	ASSERT_FALSE(f);
	expected = strerror(errno);
	ASSERT_STREQ(expected, error);
	errFree(error);
}

TEST(Error, testPrefixSomething) {
	#define MSG "Foo Bar"
	#define PFX "myPrefix()"
	const char *error = NULL;
	const char *const expected = PFX ": " MSG;
	errRender(&error, MSG);
	ASSERT_STREQ(MSG, error);
	errPrefix(&error, PFX);
	ASSERT_STREQ(expected, error);
	errFree(error);
}

TEST(Error, testPrefixNothing) {
	const char *error = NULL;
	const char *const expected = "myPrefix(): Foo Bar";
	errPrefix(&error, expected);
	ASSERT_STREQ(expected, error);
	errFree(error);
}

TEST(Error, testPrefixNull) {
	const char *error = NULL;
	errPrefix(NULL, "Foo");
	errPrefix(&error, NULL);
	ASSERT_FALSE(error);
}
