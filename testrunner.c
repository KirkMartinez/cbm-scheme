/*
 * testrunner
 *
 * This executable runs tests.
 *
 */

#include <stdio.h>
#include "getc_conio.h"

typedef enum {false, true} bool;

void main(void) {
	// For testing...
	char c;

	getc_conio_init();

	ungetc_conio('q');
	ungetc_conio('q');

	while (1) {
		c = getc_conio();
		printf("\nNext char: %c", c);
	}
}
