#ifndef GETC_CONIO_H_
#define GETC_CONIO_H_
/*
 * GETC_CONIO
 *
 * This library implements getc-like behavior using the conio library.
 *
 * It provides the ability to make line-buffered reads from "stdin",
 * extending the basic CBM console I/O.
 *
 * So this code will return nothing until the RETURN key is pressed.
 * At that point, it will continue to serve the entire buffer of
 * prior keypresses with each call to getc_conio().  
 * Once it is empty, it will return GETC_CONIO_EOF.
 *
 * Be sure to call getc_conio_init() first. Then, use getc_conio() and 
 * ungetc_conio() which behave like C's regular getc/ungetc funtions,
 * but adapted to work with Commodore I/O.
 *
 */
const char GETC_CONIO_EOF = '\0';

void getc_conio_init(void);
char getc_conio(void);
void ungetc_conio(char);

#endif
