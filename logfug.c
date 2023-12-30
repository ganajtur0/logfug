#include <stdio.h>
#include <stdbool.h>

#define ALLOWED "abcd*~+()"
#define ALLOWED_LEN 9

#define IS_VARIABLE(x) ( 'a' <= x && 'd' >= x )

typedef
enum {
    NONE,
    NOT_ALLOWED,
    PAREN_MISMATCH,
} PARSE_ERROR;

bool
mult_intended ( char slice[3] ) {
    return (
                ( IS_VARIABLE(slice[0]) && IS_VARIABLE(slice[1]) )  ||
                ( IS_VARIABLE(slice[0]) && slice[1] == '~' )        ||
                ( IS_VARIABLE(slice[0]) && slice[1] == '(' )        ||
                ( IS_VARIABLE(slice[1]) && slice[0] == ')' )        ||
                ( slice[0] == ')' && slice[1] == '(' )
           );
}

void
parserror(PARSE_ERROR err){
    switch (err) {
        case NONE:
            fprintf(stderr, "No error occured during parsing.\n");
            break;
        case NOT_ALLOWED:
            fprintf(stderr, "Unrecognized character in expression.\n");
            break;
        case PAREN_MISMATCH:
            fprintf(stderr, "Mismatched parenthesis in expression.\n");
            break;
        default:
            break;
    }
}

int
precedence(char op){
    switch (op) {
        case '~':
            return 2;
        case '*':
            return 1;
        case '+':
            return 0;
        default:
            return 0;
    }
}

typedef
struct {
    char items[1024];
    int count;
} char_stack;

typedef
struct {
    bool items[1024];
    int count;
} bool_stack;

void
char_stack_push(char_stack *stack, char c){
    stack->items[(stack->count)++] = c;
}

char
char_stack_pop(char_stack *stack){
    return stack->items[--(stack->count)];
}

char
char_stack_peek(char_stack *stack, bool *success){
    if (!stack->count){
        *success = 0;
        return 0;
    }
    *success = 1;
    return stack->items[(stack->count)-1];
}

void
char_stack_print(char_stack *stack){
    for (int i = 0; i<stack->count; i++)
        putc(stack->items[i],stdout);
    putc('\n', stdout);
}

void
bool_stack_push(bool_stack *stack, bool c){
    stack->items[(stack->count)++] = c;
}

bool
bool_stack_pop(bool_stack *stack){
    return stack->items[--(stack->count)];
}

bool
bool_stack_peek(bool_stack *stack, bool *success){
    if (!stack->count){
        *success = 0;
        return 0;
    }
    *success = 1;
    return stack->items[(stack->count)-1];
}

bool
char_allowed(char c){
    for (char i = 0; i<ALLOWED_LEN; i++)
        if (ALLOWED[i])
            return true;
    return false;
}

char
_next_token(char *s){
    s++;
    while ( *s == ' ' )
        s++;
    return *s;
}

char
next_token(char **s, bool *m){

    char *tmp = *s;
    (*s)++;

    while ( (**s) == ' ')
        (*s)++;

    if ( *((*s)+1) != '\0' ){

        char to_test[3];
        to_test[0] = **s;
        to_test[1] = _next_token(*s);
        to_test[2] = '\0';

        if (mult_intended(to_test)) {
            if (!(*m)){
                *m = true;
                char tmp2 = **s;
                *s = tmp;
                return tmp2;
            }
            else{
                *m = false;
                return '*';
            }
        }
    }

    return **s;
}

PARSE_ERROR
parse_RPN(char **expression, bool *m, char_stack *RPN){

    char_stack operators;
    operators.count = 0;

    char token, top;
    bool r;
    while ( (token=next_token(expression, m)) != '\0' ){

        if (!char_allowed(token))
            return NOT_ALLOWED;

        if (IS_VARIABLE(token))
            char_stack_push(RPN, token);

        else if (token == '(')
            char_stack_push(&operators, token);

        else if (token == ')'){
            top = char_stack_peek(&operators, &r);
            while (r && top != '('){
                char_stack_push(RPN, char_stack_pop(&operators));
                top = char_stack_peek(&operators, &r);
            }
            if (top!='(')
                return PAREN_MISMATCH;
            char_stack_pop(&operators);
        }
        
        else {
            top = char_stack_peek(&operators, &r);
            while (r && top != '(' && precedence(top) > precedence(token)){
                char_stack_push(RPN, char_stack_pop(&operators));
                top = char_stack_peek(&operators, &r);
            }
            char_stack_push(&operators, token);
        }

    }

    while (operators.count){
        top = char_stack_pop(&operators);
        if (top == '(')
            return PAREN_MISMATCH;
        char_stack_push(RPN, top); 
    }

    return NONE;

}

bool
solve_RPN(char_stack *RPN, bool nibble[4]){

    bool_stack stack; stack.count = 0;

    for (int i = 0; i<RPN->count; i++){
        char c = RPN->items[i];
        if (IS_VARIABLE(c))
            bool_stack_push(&stack, nibble[c-'a']);
        else if (c == '~')
            bool_stack_push(&stack, !(bool_stack_pop(&stack)) );
        else {
            bool a = bool_stack_pop(&stack);
            bool b = bool_stack_pop(&stack);
            switch (c) {
                case '+':
                    bool_stack_push(&stack, a || b);
                    break;
                case '*':
                    bool_stack_push(&stack, a && b);
                    break;
                default:
                break;
            }
        }
    } 
    return stack.items[0];
}

void
truthtable(char_stack *RPN, bool r){
    unsigned i, bit, tmp;
    bool test[4];
    printf(r ? "--\tDCBA\t-\n" : "--\tABCD\t-\n");
    for (i = 0; i<16; i++){
        printf("%d\t", i);
        for (bit=0; bit<4; bit++){
            tmp = ((i >> (3-bit) ) & 1);
            if (r)
                test[3-bit] = tmp;
            else
                test[bit] = tmp;
            printf("%d", tmp );
        }
        putchar('\t');
        printf(solve_RPN(RPN, test) ? "1\n" : "0\n");
    }
}

int main(int argc, char *argv[]){

    char *expression = (void*)(0);

    bool reverse = false;

    for (int args = 1; args<argc; args++){
        if (argv[args][0] == '-')
            if (argv[args][1] == 'r')
                reverse = true;
            else{
                printf("Unknown flag: %s\n", argv[1]);
                return 0;
            }
        else {
            expression = argv[args];
            break;
        }
    }

    if (expression == (void*)(0)){
        printf("USAGE: %s [-r] <expression>\n", argv[0]);
        return 0;
    }

    int rc;

    expression-=1; bool m=false;
    char_stack RPN; RPN.count = 0;

    if ( (rc=parse_RPN(&expression, &m, &RPN)) != NONE){
        fprintf(stderr, "\n Invalid expression!\n\t");
        parserror(rc);
        return 1;
    }

    truthtable(&RPN, reverse);

    return 0;
}
