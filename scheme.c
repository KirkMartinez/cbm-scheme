#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "getc_conio.h"

/************************ MODEL ************************/

typedef enum {THE_EMPTY_LIST, BOOLEAN, SYMBOL, FIXNUM, 
              CHARACTER, STRING, PAIR} object_type;

typedef struct object {
    object_type type;

    union {
        struct {
            char value;
        } boolean;
        struct {
            char *value;
        } symbol;
        struct {
            int value;
        } fixnum;
        struct {
            char value;
        } character;
        struct {
            char *value;
        } string;
        struct {
            struct object *car;
            struct object *cdr;
        } pair;
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

// Globals
object *the_empty_list;
object *false;
object *true;
object *symbol_table;

// Forwards
object *cons(object *car, object *cdr);
object *car(object *pair);
object *cdr(object *pair);

void init(void) {
    getc_conio_init();

    the_empty_list = alloc_object();
    the_empty_list->type = THE_EMPTY_LIST;

    false = alloc_object();
    false->type = BOOLEAN;
    false->data.boolean.value = 0;

    true = alloc_object();
    true->type = BOOLEAN;
    true->data.boolean.value = 1;

    symbol_table = the_empty_list;
}

// EMPTY LIST

char is_the_empty_list(object *obj) {
    return obj == the_empty_list;
}

// BOOLEAN

char is_boolean(object *obj) {
    return obj->type == BOOLEAN;
}

char is_false(object *obj) {
    return obj == false;
}

char is_true(object *obj) {
    return !is_false(obj);
}

// SYMBOL

object *make_symbol(char *value) {
    object *obj;
    object *element;

    // TODO: re-implement using a hash table...for performance
    
    /* search for they symbol in the symbol table */
    element = symbol_table;
    while (!is_the_empty_list(element)) {
        if (strcmp(car(element)->data.symbol.value, value) == 0) {
            return car(element);
        }   
        element = cdr(element);
    };
    
    /* create the symbol and add it to the symbol table */
    obj = alloc_object();
    obj->type = SYMBOL;
    obj->data.symbol.value = malloc(strlen(value) + 1); 
    if (obj->data.symbol.value == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }   
    strcpy(obj->data.symbol.value, value);
    symbol_table = cons(obj, symbol_table);
    return obj;
}

char is_symbol(object *obj) {
    return obj->type == SYMBOL;
}


// FIXNUM

object *make_fixnum(int value) { 
    object *obj;

    obj = alloc_object();
    obj->type = FIXNUM;
    obj->data.fixnum.value = value;
    return obj;
}

char is_fixnum(object *obj) {
    return obj->type == FIXNUM;
}

// CHARACTER

object *make_character(char value) {
    object *obj;

    obj = alloc_object();
    obj->type = CHARACTER;
    obj->data.character.value = value;
    return obj;
}

char is_character(object *obj) {
    return obj->type == CHARACTER;
}

// STRING

object *make_string(char *value) {
    object *obj;

    obj = alloc_object();
    obj->type = STRING;
    obj->data.string.value = malloc(strlen(value) + 1); 
    if (obj->data.string.value == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }   
    strcpy(obj->data.string.value, value);
    return obj;
}

char is_string(object *obj) {
    return obj->type == STRING;
}

// PAIR

object *cons(object *car, object *cdr) {
    object *obj;

    obj = alloc_object();
    obj->type = PAIR;
    obj->data.pair.car = car;
    obj->data.pair.cdr = cdr;

    return obj;
}

char is_pair(object *obj) {
    return obj->type == PAIR;
}

object *car(object *pair) {
    return pair->data.pair.car;
}

void set_car(object *obj, object *value) {
    obj->data.pair.car = value;
}

object *cdr(object *pair) {
    return pair->data.pair.cdr;
}

void set_cdr(object *obj, object *value) {
    obj->data.pair.cdr = value;
}

/*********************** READ ***********************/

char peek(void) {
    char c;

    c = getc_conio();
    ungetc_conio(c);
    return c;
}

// consume spaces until a non-space char is seen
void eat_whitespace(void) {
    char c;

    // For now, stop reading on newline, not EOF
    while ((c = getc_conio()) != '\n') {
        if (isspace(c)) {
            continue;
        }
        else if (c == ';') { // comments are whitespace 
            while (((c = getc_conio()) != 0) && (c != '\n'));
            continue;
        }
        ungetc_conio(c);
        break;
    }
}

char is_delimiter(int c) {
    return isspace(c) || c == GETC_CONIO_EOF ||
        c == '('   || c == ')' ||
        c == '"'   || c == ';';
}

char is_initial(int c) {
    return isalpha(c) || c == '*' || c == '/' || c == '>' ||
             c == '<' || c == '=' || c == '?' || c == '!';
}

void eat_expected_string(char *str) {
    int c;

    while (*str != '\0') {
        c = getc_conio();
        if (c != *str) {
            fprintf(stderr, "unexpected '%c'\n", c);
            exit(1);
        }
        str++;
    }
}

void peek_expected_delimiter(void) {
    if (!is_delimiter(peek())) {
        fprintf(stderr, "no delim after char\n");
        exit(1);
    }
}

object *read_character(void) {
    int c;

    c = getc_conio();
    switch (c) {
        case EOF:
            fprintf(stderr, "early EOF\n");
            exit(1);
        case 's':
            if (peek() == 'p') {
                eat_expected_string("pace");
                peek_expected_delimiter();
                return make_character(' ');
            }
            break;
        case 'n':
            if (peek() == 'e') {
                eat_expected_string("ewline");
                peek_expected_delimiter();
                return make_character('\n');
            }
            break;
    }
    peek_expected_delimiter();
    return make_character(c);
}

object *read(void); 

object *read_pair(void) {
    char c;
    object *car_obj;
    object *cdr_obj;

    eat_whitespace();
    
    c = getc_conio();
    if (c == ')') { /* read the empty list */
        return the_empty_list;
    }   
    ungetc_conio(c);

    car_obj = read();

    eat_whitespace();
    
    c = getc_conio();    
    if (c == '.') { /* read improper list */
        c = peek();
        if (!is_delimiter(c)) {
            fprintf(stderr, "no delimn after dot\n");
            exit(1);
        }   
        cdr_obj = read();
        eat_whitespace();
        c = getc_conio();
        if (c != ')') {
            fprintf(stderr, "missing (\n");
            exit(1);
        }   

        return cons(car_obj, cdr_obj);
    }   
    else { /* read list */
        ungetc_conio(c);
        cdr_obj = read_pair();    
        return cons(car_obj, cdr_obj);
    }   
}

// the main read function
object *read(void) {
    char c;
    short sign = 1;
    short i=0;
    int num = 0;
#define BUFFER_MAX 10
    char buffer[BUFFER_MAX];

    eat_whitespace();

    c = getc_conio();

    if (c == '#') { // BOOLEAN or CHARACTER
        c = getc_conio();
        switch (c) {
            case 't':
                return true;
            case 'f':
                return false;
            case '%':
                return read_character();
            default:
                fprintf(stderr, "Unknown literal\n");
                exit(1);
        }
    } else if (isdigit(c) || (c == '-' && (isdigit(peek())))) {
        // FIXNUM
        if (c == '-') {
            sign = -1;
            ++i;
        } else {
            ungetc_conio(c);
        } 
        while (isdigit(c=getc_conio())) {
            num = (num * 10) + (c - '0');
        }
        num *= sign;
        if (is_delimiter(c)) {
            ungetc_conio(c);
            return make_fixnum(num);
        } else {
            fprintf(stderr, "no delim after number\n");
            exit(1);
        }
    } else if (is_initial(c) ||
             ((c == '+' || c == '-') &&
              is_delimiter(peek()))) { // SYMBOL
        i = 0;
        while (is_initial(c) || isdigit(c) ||
               c == '+' || c == '-') {
            /* subtract 1 to save space for '\0' terminator */
            if (i < BUFFER_MAX - 1) {
                buffer[i++] = c;
            }
            else {
                fprintf(stderr, "symbol exceeds %d chars\n", BUFFER_MAX-1);
                exit(1);
            }
            c = getc_conio();
        }
        if (is_delimiter(c)) {
            buffer[i] = '\0';
            ungetc_conio(c);
            return make_symbol(buffer);
        }
        else {
            fprintf(stderr, "no delimn after symbol"
                            "Found '%c'\n", c);
            exit(1);
        }
    } else if (c == '"') { // STRING
        i = 0;
        while ((c = getc_conio()) != '"') {
            if (c == '%') {
                c = getc_conio();
                if (c == 'n') {
                    c = '\n';
                }
            }
            if (c == 0) { // readline returns zero for EOF
                fprintf(stderr, "non-terminated string\n");
                exit(1);
            }
            /* subtract 1 to save space for '\0' terminator */
            if (i < BUFFER_MAX - 1) {
                buffer[i++] = c;
            }
            else {
                fprintf(stderr, "string exceeds %d chars\n", BUFFER_MAX);
                exit(1);
            }
        }
        buffer[i] = '\0';
        return make_string(buffer);
    } else if (c == '(') { // THE_EMPTY_LIST or PAIR
        return read_pair();
    } else { 
        fprintf(stderr, "bad input. Unexpected '%c'\n", c);
        exit(1);
    }
    fprintf(stderr, "input error\n");
    exit(1);
}

/************************ EVAL ************************/

/* until we have lists and symbols just echo */
object *eval(object *exp) {
    return exp;
}

/************************ PRINT ************************/

void writeit(object *obj);

void write_pair(object *pair) {
    object *car_obj;
    object *cdr_obj;

    car_obj = car(pair);
    cdr_obj = cdr(pair);

    writeit(car_obj);
    if (cdr_obj->type == PAIR) {
        printf(" ");
        write_pair(cdr_obj);
    }
    else if (cdr_obj->type == THE_EMPTY_LIST) {
        return;
    }
    else {
        printf(" . ");
        writeit(cdr_obj);
    }
}

void writeit(object *obj) {
    char c;
    char *str;

    switch (obj->type) {
        case THE_EMPTY_LIST:
            printf("()");
            break;
        case BOOLEAN:
            printf("#%c", is_false(obj) ? 'f' : 't');
            break;
        case SYMBOL:
            printf("%s", obj->data.symbol.value);
            break;
        case FIXNUM:
            printf("%d", obj->data.fixnum.value);
            break;
        case CHARACTER:
            c = obj->data.character.value;
            printf("#'");
            switch (c) {
                case '\n':
                    printf("newline");
                    break;
                case ' ':
                    printf("space");
                    break;
                default:
                    putchar(c);
            }
            break;
        case STRING:
            str = obj->data.string.value;
            putchar('"');
            while (*str != '\0') {
                switch (*str) {
                    case '\n':
                        printf("\\n");
                        break;
                    case '\'':
                        printf("''");
                        break;
                    case '"':
                        printf("'");
                        break;
                    default:
                        putchar(*str);
                }
                ++str;
            }
            putchar('"');
            break;
        case PAIR:
            printf("(");
            write_pair(obj);
            printf(")");
            break;
        default:
            fprintf(stderr, "cannot write unknown type\n");
            exit(1);
    }
}

/************************ REPL *************************/

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

