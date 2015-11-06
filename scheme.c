#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <ctype.h>

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

/**** MODEL ****/

typedef enum {FIXNUM} object_type;

typedef struct object {
    object_type type;
    union {
        struct {
            int value; // was long, but that crashed VICE
        } fixnum;
    } data;
} object;

object *alloc_object(void) {
    object *obj;

    obj = malloc(sizeof(object));
    if (obj == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    return obj;
}

object *make_fixnum(int value) { // was long
    object *obj;

    obj = alloc_object();
    obj->type = FIXNUM;
    obj->data.fixnum.value = value;
    return obj;
}

/*** READ ***/

char is_delimiter(int c) {
    return isspace(c) || c == 0||
		c == '('   || c == ')' ||
		c == '"'   || c == ';';
}

object *read() {
	char c;
	short sign = 1;
    short i=0;
    int num = 0;

	readline();

	c = mygetc();

	if (isdigit(c) || (c == '-' && (isdigit(mypeek())))) {
		/* read a fixnum */
		if (c == '-') {
			sign = -1;
			++i;
		} else {
			myungetc();
		} 
		while (isdigit(c=mygetc())) {
			num = (num * 10) + (c - '0');
		}
		num *= sign;
		if (is_delimiter(c)) {
			return make_fixnum(num);
		} else {
			fprintf(stderr, "number not followed by delimiter\n");
		}
	} else {
		fprintf(stderr, "bad input. Unexpected '%c'\n", c);
	}
	fprintf(stderr, "read illegal state\n");
}

/**** EVAL ****/

/* until we have lists and symbols just echo */
object *eval(object *exp) {
	return exp;
}

/**** PRINT ****/

void writeit(object *obj) {
	switch (obj->type) {
		case FIXNUM:
			printf("%d", obj->data.fixnum.value);
			break;
		default:
			fprintf(stderr, "cannot write unknown type\n");
	}
}

/***************************** REPL ******************************/

void scheme(void) {
	printf("Welcome to Scheme for Commodore!\n");

	while (1) {
		printf("> ");
		writeit(eval(read()));
		printf("\n");
	}   
}

int main (void)
{
	clrscr (); 
	textcolor (COLOR_WHITE);
	bordercolor (COLOR_BLACK);
	bgcolor (COLOR_BLACK);

	scheme();

	printf("\nPress a key to exit.\n");
	cgetc(); 

	return EXIT_SUCCESS;
}

