#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <ctype.h>

#include "readline.h"

/**** MODEL ****/

typedef enum {BOOLEAN, FIXNUM} object_type;

typedef struct object {
    object_type type;
    union {
		struct {
			char value;
		} boolean;
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

object *false;
object *true;

char is_boolean(object *obj) {
	return obj->type == BOOLEAN;
}

char is_false(object *obj) {
	return obj == false;
}

char is_true(object *obj) {
	return !is_false(obj);
}

void init(void) {
	false = alloc_object();
	false->type = BOOLEAN;
	false->data.boolean.value = 0;

	true = alloc_object();
	true->type = BOOLEAN;
	true->data.boolean.value = 1;
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
	printf("\nGot: %c\n", c);

	if (c == '#') { // boolean
		c = mygetc();
		switch (c) {
			case 't':
				return true;
			case 'f':
				return false;
			default:
				fprintf(stderr, "Unknown boolean literal\n");
		}
	} else if (isdigit(c) || (c == '-' && (isdigit(mypeek())))) {
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
		case BOOLEAN:
			printf("#%c", is_false(obj) ? 'f' : 't');
			break;
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

	init();

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

