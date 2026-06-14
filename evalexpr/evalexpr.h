/**
 * @author Amin Tahmasebi
 * @class EvalExpr
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in evalexpr.c (file-level convention).
 *
 * Shunting-yard arithmetic-expression evaluator: tokenize → RPN →
 * evaluate. Supports `+ - * / ^ ( )` plus parenthesized
 * sub-expressions.
 */

#ifndef EVALEXPR_H_
#define EVALEXPR_H_

#include <stdio.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define EVALEXPR_LOGGING_ENABLE */

#ifdef EVALEXPR_LOGGING_ENABLE
    #define EVALEXPR_LOG(fmt, ...) \
        do { fprintf(stderr, "[EVALEXPR LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define EVALEXPR_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Status codes / limits                                              */
/* ------------------------------------------------------------------ */

#define EVAL_EXPR_SUCCESS              0
#define EVAL_EXPR_ERROR_TOKENIZE      -1
#define EVAL_EXPR_ERROR_SHUNTING      -2
#define EVAL_EXPR_ERROR_EVAL_RPN      -3
#define MAX_TOKENS                   128


typedef enum {
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_PAREN_LEFT,
    TOKEN_PAREN_RIGHT
} TokenType;


typedef struct {
    TokenType type;
    double    value;     /* TOKEN_NUMBER payload   */
    char      op;        /* TOKEN_OPERATOR payload */
} Token;


/* ------------------------------------------------------------------ */
/* Evaluation                                                         */
/* ------------------------------------------------------------------ */

double        eval_expr                       (const char* expr);
double        eval_expr_strict                (const char* expr, int* error);
double        eval_expr_with_vars             (const char* expr, const char** names, const double* values, size_t count, int* error);


/* ------------------------------------------------------------------ */
/* Compilation / introspection                                        */
/* ------------------------------------------------------------------ */

char*         eval_expr_to_rpn_string         (const char* expr);
char**        eval_expr_tokenize              (const char* expr, int* numTokens);
void          eval_expr_free_tokens           (char** tokens, int count);
int           eval_expr_is_valid              (const char* expr);
void          eval_expr_print_debug           (const char* expr);


/* ------------------------------------------------------------------ */
/* Diagnostics                                                        */
/* ------------------------------------------------------------------ */

const char*   eval_expr_error_message         (int error);


#ifdef __cplusplus
}
#endif

#endif 
