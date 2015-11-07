#include <stdlib.h>
#include <conio.h>
#include <stdio.h>

/**** readline ****/
/* 
 * Call readline(), then use mygetc and myungetc which
 * behave like C's regular getc/ungetc funtions which
 * are not implemented with cc65.
 *
 */
static const int LINE_BUFFER_SIZE=100;
static char line_buffer[100]; // Global line buffer
static short bufptr=0; 		  // Current character to read

void readline() {
	// Loads buffer until newline detected or buffer full.
	// Zero-terminates resulting string.
	char c;
	short i=0;

	bufptr=0;

	while( ((c=cgetc()) != '\n') && i<LINE_BUFFER_SIZE) {
		cputc(c);
		line_buffer[i] = c;
		++i;
	}
	printf("\n");

	if (i<LINE_BUFFER_SIZE) {
		line_buffer[i] = 0;
	} else {
		line_buffer[LINE_BUFFER_SIZE-1] = 0;
	}
}

char mygetc() {
	if (bufptr < LINE_BUFFER_SIZE) {
		return line_buffer[bufptr++];
	} else {
		return 0;
	}
}

char mypeek() {
	// return next character, but don't advance ptr
	if (bufptr < LINE_BUFFER_SIZE) {
		return line_buffer[bufptr];
	} else {
		return 0;
	}
}

void myungetc() {
	// Not really ungetc since you can't insert arbitrary characters...
	if (bufptr > 0) {
		--bufptr;
	} 
}
