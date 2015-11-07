#ifndef READLINE_H_
#define READLINE_H_

/**** readline ****/
/* 
 * Call readline(), then use mygetc and myungetc which
 * behave like C's regular getc/ungetc funtions which
 * are not implemented with cc65.
 *
 */
void readline();
char mypeek();
char mygetc();
void myungetc();

#endif
