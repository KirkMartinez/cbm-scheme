#include <stdlib.h>
#include <conio.h>
#include <stdio.h>

// Types
typedef enum {false, true} bool;

// Constants
const char GETC_CONIO_EOF = '\0';

// Globals
unsigned char size_x, size_y;      // Screen size
static const int BUFFER_SIZE=1000; // duplicate below
static char line_buffer[1000];     // ungetc buffer
static short bufptr=0;             // where to insert next char
static short head=0;               // "now serving..."
static bool saw_newline = false;   // says "ok to serve"

// init
void getc_conio_init(void) {
	screensize(&size_x, &size_y);
	bufptr = 0;
	head = 0;
	saw_newline = false;
}

// private
// put it in the buffer
void getc_conio_insert(char c) {
	line_buffer[bufptr] = c;
	++bufptr;
}

// private
// returns next customer from queue
// only call this if there is one!
char getc_conio_remove(void) {
	char c;

	c = line_buffer[head];
	++head;

	if (head == bufptr) {
		// we just read the last char, reset
		bufptr = 0;
		head = 0;
		saw_newline = false;
	}
		

	return c;
}

//
void read_a_line(void) {
	char c;
	unsigned char x_pos;

	// buffer characters until we see RETURN
	do {
		c = cgetc();
		getc_conio_insert(c); 
		putchar(c); // show it

		// When we reach the right edge, newline
		x_pos = wherex();
		if (x_pos > size_x) {
			putchar('\n');
		}
	} while (c != '\n');

	saw_newline = true;
}

char getc_conio(void) {
	char c;

	if (saw_newline && (head != bufptr)) {
		c = getc_conio_remove();
	} else {
		read_a_line();
		c = getc_conio_remove();
	}

	return c;
}

// put characters back at the head of the queue
void ungetc_conio(char c) {
	if (head == 0) {
		if (bufptr == 0) {
			getc_conio_insert(c);
		} else {
			fprintf(stderr,"getc_conio cannot unget before start of line");
			exit(1);
		}
	} else {
		--head;
		line_buffer[head] = c;
	}
}

