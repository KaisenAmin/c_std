#ifndef EVALEXPR_H_
#define EVALEXPR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

// #define EVALEXPR_LOGGING_ENABLE

#ifdef EVALEXPR_LOGGING_ENABLE 
    #define EVALEXPR_LOG(fmt, ...) \
        do { fprintf(stderr, "[EVALEXPR LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define EVALEXPR_LOG(fmt, ...) do { } while (0)
#endif

#define EVAL_EXPR_SUCCESS      0
#define EVAL_EXPR_ERROR_TOKENIZE  -1
#define EVAL_EXPR_ERROR_SHUNTING  -2
#define EVAL_EXPR_ERROR_EVAL_RPN   -3
#define MAX_TOKENS 128

typedef enum {
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_PAREN_LEFT,
    TOKEN_PAREN_RIGHT
} TokenType;

typedef struct {
    TokenType type;
    double value;  
    char op;       
} Token;


double eval_expr(const char *expr);
double eval_expr_strict(const char *expr, int *error);

char *eval_expr_to_rpn_string(const char *expr);
const char *eval_expr_error_message(int error);
char **eval_expr_tokenize(const char *expr, int *numTokens);

int eval_expr_is_valid(const char *expr);
void eval_expr_print_debug(const char *expr);

#ifdef __cplusplus
}
#endif

#endif  
