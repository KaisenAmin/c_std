#include "evalexpr.h"

#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>



typedef enum {
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_PAREN_LEFT,
    TOKEN_PAREN_RIGHT
} EvalTokenType;

typedef struct {
    EvalTokenType type;
    double        value;     /* TOKEN_NUMBER payload   */
    char          op;        /* TOKEN_OPERATOR payload */
} Token;


/* Internal operator code for unary minus (prefix negation). */
#define EVAL_OP_NEGATE '~'

/* Returns the precedence of the operator. Higher numbers mean higher precedence.
   Unary minus binds tighter than * and / but looser than ^, as in Python and
   ordinary math notation: -2^2 = -(2^2) = -4 and 2*-3 = -6. */
static int precedence(char op) {
    switch (op) {
        case '^':
            return 5;
        case EVAL_OP_NEGATE:
            return 4;
        case '*':
        case '/': 
            return 3;
        case '+':
        case '-': 
            return 2;
        default:  
            return 0;
    }
}


/* Writes the printable form of an operator. Unary minus is "-" in an infix token
   list, but "neg" in RPN output, where a bare "-" would read as subtraction. */
static void op_to_text(char op, int rpn, char *out, size_t size) {
    if (op == EVAL_OP_NEGATE) {
        snprintf(out, size, "%s", rpn ? "neg" : "-");
    }
    else {
        snprintf(out, size, "%c", op);
    }
}

/* Returns whether the operator is right-associative (exponentiation and the
   prefix negation). */
static int is_right_associative(char op) {
    return (op == '^' || op == EVAL_OP_NEGATE);
}


/* Returns whether the character is a valid operator. */
static int is_operator(char ch) {
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^');
}


/* strdup is POSIX (C23), not C17: a strict -std=c11 build on glibc does not
   declare it. */
static char *eval_strdup(const char *s) {
    size_t n = strlen(s) + 1;
    char *copy = (char *)malloc(n);
    if (copy) {
        memcpy(copy, s, n);
    }
    return copy;
}


/* The current LC_NUMERIC decimal point ("." in the C locale). */
static const char *eval_decimal_point(void) {
    const struct lconv *lc = localeconv();
    return (lc && lc->decimal_point && lc->decimal_point[0]) ? lc->decimal_point : ".";
}


/* Length of the numeric literal at s, 0 if there is none: decimal ("12",
   "3.5", ".5", "1e-3") or hexadecimal ("0x1A", "0x1.8p1"), the forms strtod
   reads, but always with '.' as the decimal point. */
static size_t eval_literal_length(const char *s) {
    size_t i = 0;
    size_t digits = 0;

    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        i = 2;
        while (isxdigit((unsigned char)s[i])) {
            i++;
            digits++;
        }
        if (s[i] == '.') {
            i++;
            while (isxdigit((unsigned char)s[i])) {
                i++;
                digits++;
            }
        }
        if (digits > 0) {
            if (s[i] == 'p' || s[i] == 'P') {
                size_t k = i + 1;
                if (s[k] == '+' || s[k] == '-') {
                    k++;
                }
                if (isdigit((unsigned char)s[k])) {
                    while (isdigit((unsigned char)s[k])) {
                        k++;
                    }
                    i = k;
                }
            }
            return i;
        }
        i = 0;   /* "0x" without a hex digit: just the 0 */
    }

    while (isdigit((unsigned char)s[i])) {
        i++;
        digits++;
    }
    if (s[i] == '.') {
        i++;
        while (isdigit((unsigned char)s[i])) {
            i++;
            digits++;
        }
    }
    if (digits == 0) {
        return 0;
    }
    if (s[i] == 'e' || s[i] == 'E') {
        size_t k = i + 1;
        if (s[k] == '+' || s[k] == '-') {
            k++;
        }
        if (isdigit((unsigned char)s[k])) {
            while (isdigit((unsigned char)s[k])) {
                k++;
            }
            i = k;
        }
    }
    return i;
}


/* Reads the numeric literal at s into *out and its end into *endp; 0 if there
   is no literal or it overflows. strtod follows LC_NUMERIC, so under a
   decimal-comma locale "3.5" read as 3 and every fraction was a tokenize
   error: the literal is handed over with the locale's decimal point (and in
   its original form for a runtime whose strtod ignores the locale). A literal
   that underflows to a subnormal or to zero (1e-320) is accepted; only ERANGE
   with an infinite result (1e400) is an error. */
static int eval_read_number(const char *s, const char **endp, double *out) {
    size_t len = eval_literal_length(s);
    if (len == 0) {
        return 0;
    }

    const char *dp = eval_decimal_point();
    size_t dpl = strlen(dp);
    char small[64];
    size_t cap = len + dpl + 1;
    char *buf = (cap <= sizeof(small)) ? small : (char *)malloc(cap);
    if (!buf) {
        return 0;
    }

    size_t k = 0;
    for (size_t i = 0; i < len; i++) {
        if (s[i] == '.') {
            memcpy(buf + k, dp, dpl);
            k += dpl;
        }
        else {
            buf[k++] = s[i];
        }
    }
    buf[k] = '\0';

    char *end = NULL;
    errno = 0;
    double v = strtod(buf, &end);
    int ok = (*end == '\0');
    if (!ok && strcmp(dp, ".") != 0) {
        memcpy(buf, s, len);
        buf[len] = '\0';
        errno = 0;
        v = strtod(buf, &end);
        ok = (*end == '\0');
    }
    int overflow = (errno == ERANGE && isinf(v));
    if (buf != small) {
        free(buf);
    }
    if (!ok || overflow) {
        return 0;
    }

    *out = v;
    *endp = s + len;
    return 1;
}


/* Writes v as the shortest of %.15g / %.16g / %.17g that reads back as the
   same double, always with '.' as the decimal point. Plain %g kept 6 digits,
   so 3.14159265 came out as 3.14159. */
static void eval_format_number(double v, char *out, size_t size) {
    if (!isfinite(v)) {
        snprintf(out, size, "%g", v);
        return;
    }

    const char *dp = eval_decimal_point();
    for (int prec = 15; prec <= 17; prec++) {
        snprintf(out, size, "%.*g", prec, v);
        if (strcmp(dp, ".") != 0) {
            char *hit = strstr(out, dp);
            if (hit) {
                size_t dpl = strlen(dp);
                *hit = '.';
                memmove(hit + 1, hit + dpl, strlen(hit + dpl) + 1);
            }
        }

        const char *digits = (out[0] == '-') ? out + 1 : out;
        const char *end = NULL;
        double back = 0.0;
        if (eval_read_number(digits, &end, &back) && *end == '\0' && ((out[0] == '-') ? -back : back) == v) {
            return;
        }
    }
}


/* Space-separated RPN text of the tokens, heap-allocated (NULL when out of
   memory). It used to be built in a char[1024] and silently cut short. */
static char *eval_rpn_text(const Token *rpn, int count) {
    enum { PIECE = 40 };   /* longest number text is 24 bytes, "neg" is 3 */
    char *text = (char *)malloc((size_t)(count > 0 ? count : 0) * PIECE + 1);
    if (!text) {
        return NULL;
    }

    size_t pos = 0;
    for (int i = 0; i < count; i++) {
        char piece[PIECE];
        if (rpn[i].type == TOKEN_NUMBER) {
            eval_format_number(rpn[i].value, piece, sizeof(piece));
        }
        else if (rpn[i].type == TOKEN_OPERATOR) {
            op_to_text(rpn[i].op, 1, piece, sizeof(piece));
        }
        else {
            continue;
        }
        size_t n = strlen(piece);
        if (pos > 0) {
            text[pos++] = ' ';
        }
        memcpy(text + pos, piece, n);
        pos += n;
    }
    text[pos] = '\0';
    return text;
}


/**
 * Parses the input expression string into an array of tokens.
 *
 * @param expr The expression string.
 * @param tokens The output array of tokens.
 * @return The number of tokens parsed, or -1 on error.
 */
static int tokenize(const char *expr, Token tokens[MAX_TOKENS], const char **var_names, const double *var_values, size_t var_count) {
    int pos = 0;       // current position in expr
    int tcount = 0;    // number of tokens produced

    while (expr[pos] != '\0') {
        // Skip any whitespace.
        if (isspace((unsigned char)expr[pos])) {
            pos++;
            continue;
        }

        /* A sign at the start, after an operator or after '(' is unary. It is
           a real prefix operator now: the old trick of emitting a synthetic 0
           and a binary minus gave wrong results after * / ^ - (2*-(3) was -3,
           3--(2) was 1, 10/-(2+3) reported division by zero). */
        if ((expr[pos] == '-' || expr[pos] == '+') &&
            (tcount == 0 || tokens[tcount - 1].type == TOKEN_OPERATOR || tokens[tcount - 1].type == TOKEN_PAREN_LEFT)) {
            char sign = expr[pos];
            int look = pos + 1;
            while (isspace((unsigned char)expr[look])) {
                look++;
            }

            /* "-5" stays a single numeric token, unless the literal is the
               base of '^' (-2^2 must be -(2^2), not (-2)^2). */
            if (isdigit((unsigned char)expr[look]) || expr[look] == '.') {
                const char *endptr;
                double val = 0.0;

                if (!eval_read_number(&expr[look], &endptr, &val)) {
                    EVALEXPR_LOG("[tokenize] : Invalid unary numeric literal at position %d.", look);
                    return -1;
                }
                const char *after = endptr;
                while (isspace((unsigned char)*after)) {
                    after++;
                }
                if (*after != '^') {
                    if (tcount >= MAX_TOKENS) {
                        EVALEXPR_LOG("[tokenize] : Token buffer overflow.");
                        return -1;
                    }
                    tokens[tcount].type = TOKEN_NUMBER;
                    tokens[tcount].value = (sign == '-') ? -val : val;
                    tcount++;
                    pos = (int)(endptr - expr);
                    continue;
                }
            }

            if (sign == '-') {
                if (tcount >= MAX_TOKENS) {
                    EVALEXPR_LOG("[tokenize] : Token buffer overflow.");
                    return -1;
                }
                tokens[tcount].type = TOKEN_OPERATOR;
                tokens[tcount].op = EVAL_OP_NEGATE;
                tcount++;
            }
            pos++;   /* a unary '+' changes nothing */
            continue;
        }

        if (isdigit((unsigned char)expr[pos]) || expr[pos] == '.') {
            const char *endptr;
            double val = 0.0;

            if (!eval_read_number(&expr[pos], &endptr, &val)) {
                /* no digits ("."), or out of range ("1e400") */
                EVALEXPR_LOG("[tokenize] : Invalid numeric literal at position %d.", pos);
                return -1;
            }
            if (tcount >= MAX_TOKENS) {
                EVALEXPR_LOG("[tokenize] : Token buffer overflow.");
                return -1;
            }

            tokens[tcount].type = TOKEN_NUMBER;
            tokens[tcount].value = val;
            tcount++;
            pos = (int)(endptr - expr);

            continue;
        }

        if (var_count > 0 && (isalpha((unsigned char)expr[pos]) || expr[pos] == '_')) {
            int start = pos;

            while (isalnum((unsigned char)expr[pos]) || expr[pos] == '_') {
                pos++;
            }
            int len = pos - start;

            double val = 0.0;
            int found = 0;
            for (size_t v = 0; v < var_count; v++) {
                if (var_names[v] && (int)strlen(var_names[v]) == len && strncmp(var_names[v], &expr[start], (size_t)len) == 0) {
                    val = var_values[v];
                    found = 1;

                    break;
                }
            }

            if (!found) {
                EVALEXPR_LOG("[tokenize] : Undefined variable in expression.");
                return -1;
            }
            if (tcount >= MAX_TOKENS) {
                EVALEXPR_LOG("[tokenize] : Token buffer overflow.");
                return -1;
            }

            tokens[tcount].type = TOKEN_NUMBER;
            tokens[tcount].value = val;
            tcount++;

            continue;
        }

        // If the character is an operator.
        if (is_operator(expr[pos])) {
            if (tcount >= MAX_TOKENS) {
                EVALEXPR_LOG("[tokenize] : Token buffer overflow.");
                return -1;
            }

            tokens[tcount].type = TOKEN_OPERATOR;
            tokens[tcount].op = expr[pos];
            tcount++;
            pos++;

            continue;
        }

        // Parentheses.
        if (expr[pos] == '(') {
            if (tcount >= MAX_TOKENS) {
                EVALEXPR_LOG("[tokenize] : Token buffer overflow.");
                return -1;
            }

            tokens[tcount].type = TOKEN_PAREN_LEFT;
            tcount++;
            pos++;

            continue;
        }
        if (expr[pos] == ')') {
            if (tcount >= MAX_TOKENS) {
                EVALEXPR_LOG("[tokenize] : Token buffer overflow.");
                return -1;
            }

            tokens[tcount].type = TOKEN_PAREN_RIGHT;
            tcount++;
            pos++;

            continue;
        }

        EVALEXPR_LOG("[tokenize] : Unknown character encountered: '%c'", expr[pos]);
        return -1;
    }

    return tcount;
}


/**
 * Converts an array of tokens in infix notation into RPN (Reverse Polish Notation)
 * using the shunting-yard algorithm.
 *
 * @param tokens Array of tokens (infix notation).
 * @param numTokens Number of tokens in the array.
 * @param output Array where the RPN tokens will be stored.
 * @return The number of tokens in the RPN output, or -1 on error.
 */
/* Checks the infix structure: operands and binary operators must alternate,
   starting and ending with an operand, unary minus only where an operand is
   due, and parentheses balanced and never empty. Shunting-yard alone accepts
   "2 3 +" (and computes 5), "2 3", "(2)(3)", "()" and "", so eval_expr_is_valid
   called them valid. */
static int infix_is_well_formed(const Token tokens[MAX_TOKENS], int numTokens) {
    int expectOperand = 1;
    int depth = 0;

    for (int i = 0; i < numTokens; i++) {
        const Token *t = &tokens[i];

        if (expectOperand) {
            if (t->type == TOKEN_NUMBER) {
                expectOperand = 0;
            }
            else if (t->type == TOKEN_PAREN_LEFT) {
                depth++;
            }
            else if (!(t->type == TOKEN_OPERATOR && t->op == EVAL_OP_NEGATE)) {
                return 0;   /* binary operator, ')' or "()" where an operand is due */
            }
        }
        else {
            if (t->type == TOKEN_OPERATOR && t->op != EVAL_OP_NEGATE) {
                expectOperand = 1;
            }
            else if (t->type == TOKEN_PAREN_RIGHT) {
                if (--depth < 0) {
                    return 0;
                }
            }
            else {
                return 0;   /* two operands in a row: "2 3", "(2)(3)", "2(3)" */
            }
        }
    }

    return !expectOperand && depth == 0;
}


static int shunting_yard(const Token tokens[MAX_TOKENS], int numTokens, Token output[MAX_TOKENS]) {
    Token opStack[MAX_TOKENS];
    int opCount = 0;
    int outCount = 0;

    if (!infix_is_well_formed(tokens, numTokens)) {
        EVALEXPR_LOG("[shunting_yard] : Malformed expression (operand/operator order or parentheses).");
        return -1;
    }

    for (int i = 0; i < numTokens; i++) {
        Token token = tokens[i];

        if (token.type == TOKEN_NUMBER) {
            output[outCount++] = token;
        }
        else if (token.type == TOKEN_OPERATOR && token.op == EVAL_OP_NEGATE) {
            /* A prefix operator has no left operand, so it never pops anything. */
            opStack[opCount++] = token;
        }
        else if (token.type == TOKEN_OPERATOR) {
            // While there is an operator at the top of the operator stack.
            while (opCount > 0 &&
                   opStack[opCount - 1].type == TOKEN_OPERATOR &&
                   ((!is_right_associative(token.op) &&
                     (precedence(token.op) <= precedence(opStack[opCount - 1].op))) ||
                    (is_right_associative(token.op) &&
                     (precedence(token.op) < precedence(opStack[opCount - 1].op))))) {
                output[outCount++] = opStack[--opCount];
            }
            opStack[opCount++] = token;
        }
        else if (token.type == TOKEN_PAREN_LEFT) {
            // Push left parenthesis 
            opStack[opCount++] = token;
        }
        else if (token.type == TOKEN_PAREN_RIGHT) {
            // Pop from the stack until a left parenthesis is encountered.
            int foundLeftParen = 0;
            while (opCount > 0) {
                Token top = opStack[opCount - 1];
                if (top.type == TOKEN_PAREN_LEFT) {
                    foundLeftParen = 1;
                    opCount--;  // Discard the left parenthesis.
                    break;
                }
                output[outCount++] = opStack[--opCount];
            }
            if (!foundLeftParen) {
                EVALEXPR_LOG("[shunting_yard] : Mismatched parentheses.");
                return -1;
            }
        }
    }

    // After processing all tokens, pop any remaining operators onto the output.
    while (opCount > 0) {
        if (opStack[opCount - 1].type == TOKEN_PAREN_LEFT ||
            opStack[opCount - 1].type == TOKEN_PAREN_RIGHT) {
            EVALEXPR_LOG("[shunting_yard] : Mismatched parentheses.");
            return -1;
        }
        output[outCount++] = opStack[--opCount];
    }

    return outCount;
}


/**
 * Evaluates an RPN (Reverse Polish Notation) expression.
 *
 * @param rpnTokens Array of tokens in RPN order.
 * @param numTokens Number of tokens in the array.
 * @param result Pointer where the result will be stored.
 * @return 0 on success, -1 on error.
 */
static int eval_rpn(const Token rpnTokens[MAX_TOKENS], int numTokens, double *result) {
    double stack[MAX_TOKENS];
    int stackTop = 0;

    for (int i = 0; i < numTokens; i++) {
        Token token = rpnTokens[i];
        if (token.type == TOKEN_NUMBER) {
            stack[stackTop++] = token.value;
        }
        else if (token.type == TOKEN_OPERATOR && token.op == EVAL_OP_NEGATE) {
            if (stackTop < 1) {
                EVALEXPR_LOG("[eval_rpn] : Missing operand for unary minus.");
                return -1;
            }
            stack[stackTop - 1] = -stack[stackTop - 1];
        }
        else if (token.type == TOKEN_OPERATOR) {
            if (stackTop < 2) {
                EVALEXPR_LOG("[eval_rpn] : Insufficient operands for operator '%c'.", token.op);
                return -1;
            }
            double right = stack[--stackTop];
            double left = stack[--stackTop];
            double res = 0.0;

            switch (token.op) {
                case '+':
                    res = left + right;
                    break;
                case '-':
                    res = left - right;
                    break;
                case '*':
                    res = left * right;
                    break;
                case '/':
                    if (right == 0) {
                        EVALEXPR_LOG("[eval_rpn] : Division by zero encountered.");
                        return -1;
                    }
                    res = left / right;
                    break;
                case '^':
                    res = pow(left, right);
                    break;
                default:
                    EVALEXPR_LOG("[eval_rpn] : Unknown operator '%c'.", token.op);
                    return -1;
            }
            stack[stackTop++] = res;
        }
        else {
            EVALEXPR_LOG("[eval_rpn] : Invalid token type during evaluation.");
            return -1;
        }
    }

    if (stackTop != 1) {
        EVALEXPR_LOG("[eval_rpn] : The user input has too many values (stackTop = %d).", stackTop);
        return -1;
    }

    *result = stack[0];
    EVALEXPR_LOG("[eval_rpn] : Evaluation successful, result = %g", *result);
    return 0;
}


/**
 * Evaluates an arithmetic expression given as a null‐terminated string.
 *
 * The expression may include numbers (integer or floating point), the operators
 * +, -, *, /, ^, and parentheses for grouping. White spaces are allowed.
 *
 * @param expr A null‐terminated C string containing the expression.
 * @return The computed value as a double, or NAN on error.
 */
double eval_expr(const char *expr) {
    if (!expr) {
        EVALEXPR_LOG("[eval_expr] : Error: NULL expression.");
        return NAN;
    }
    Token tokens[MAX_TOKENS];
    Token rpnTokens[MAX_TOKENS];

    int numTokens = tokenize(expr, tokens, NULL, NULL, 0);
    if (numTokens < 0) {
        EVALEXPR_LOG("[eval_expr] : Tokenization failed for expression: %s", expr);
        return NAN;
    }

    int rpnCount = shunting_yard(tokens, numTokens, rpnTokens);
    if (rpnCount < 0) {
        EVALEXPR_LOG("[eval_expr] : Shunting-yard conversion failed for expression: %s", expr);
        return NAN;
    }

    double result = 0.0;
    if (eval_rpn(rpnTokens, rpnCount, &result) != 0) {
        EVALEXPR_LOG("[eval_expr] : RPN evaluation failed for expression: %s", expr);
        return NAN;
    }

    EVALEXPR_LOG("[eval_expr] : Expression evaluated successfully, result = %g", result);
    return result;
}


/**
 * Evaluates an arithmetic expression and returns an error code.
 *
 * This function works like eval_expr() but also sets an error code.
 * On success, *error is set to EVAL_EXPR_SUCCESS (0). On error, *error is
 * set to one of the defined error codes.
 *
 * @param expr A null‐terminated C string containing the expression.
 * @param error Pointer to an int where the error code will be stored.
 * @return The computed value as a double, or NAN on error.
 */
double eval_expr_strict(const char *expr, int *error) {
    if (!expr) {
        EVALEXPR_LOG("[eval_expr_strict] : Error: NULL expression.");
        if (error) *error = EVAL_EXPR_ERROR_TOKENIZE;
        return NAN;
    }
    EVALEXPR_LOG("[eval_expr_strict] : Evaluating expression: %s", expr);

    Token tokens[MAX_TOKENS];
    Token rpnTokens[MAX_TOKENS];

    int numTokens = tokenize(expr, tokens, NULL, NULL, 0);
    if (numTokens < 0) {
        EVALEXPR_LOG("[eval_expr_strict] : Tokenization failed for expression: %s", expr);
        if (error) {
            *error = EVAL_EXPR_ERROR_TOKENIZE;
        }
        return NAN;
    }

    int rpnCount = shunting_yard(tokens, numTokens, rpnTokens);
    if (rpnCount < 0) {
        EVALEXPR_LOG("[eval_expr_strict] : Shunting-yard conversion failed for expression: %s", expr);
        if (error) {
            *error = EVAL_EXPR_ERROR_SHUNTING;
        }
        return NAN;
    }

    double result = 0.0;
    if (eval_rpn(rpnTokens, rpnCount, &result) != 0) {
        EVALEXPR_LOG("[eval_expr_strict] : Evaluation of RPN failed for expression: %s", expr);
        if (error) {
            *error = EVAL_EXPR_ERROR_EVAL_RPN;
        }
        return NAN;
    }

    EVALEXPR_LOG("[eval_expr_strict] : Expression evaluated successfully: %s, result: %g", expr, result);
    if (error) {
        *error = EVAL_EXPR_SUCCESS;
    }
    return result;
}


/**
 * Evaluates an arithmetic expression that may reference named variables.
 *
 * Works exactly like eval_expr_strict(), but the expression may also contain
 * identifiers (e.g. "x*x + 2*y - rate"). Each identifier
 * (`[A-Za-z_][A-Za-z0-9_]*`) is looked up in the caller-supplied table and
 * replaced by its bound value before evaluation. A unary sign in front of a
 * variable works too: "-x" evaluates to the negation of x. Any identifier not
 * present in the table is a tokenize error. This turns the evaluator into a
 * small formula engine for config-driven or user-defined arithmetic.
 *
 * The table is two parallel arrays: names[i] is bound to values[i], for i in
 * [0, count). Names are matched case-sensitively and in full. If the same name
 * appears more than once, the first match wins.
 *
 * @param expr   A null-terminated C string containing the expression.
 * @param names  Array of `count` variable names (ignored when count == 0).
 * @param values Array of `count` variable values (ignored when count == 0).
 * @param count  Number of entries in the variable table.
 * @param error  Optional out-param: set to EVAL_EXPR_SUCCESS (0) on success or
 *               one of the EVAL_EXPR_ERROR_* codes on failure. May be NULL.
 * @return The computed value as a double, or NAN on error.
 *
 * @code
 * const char* names[]  = { "x", "y" };
 * double      values[] = { 3.0, 4.0 };
 * int err;
 * double r = eval_expr_with_vars("x*x + y*y", names, values, 2, &err);
 * // err == EVAL_EXPR_SUCCESS, r == 25.0
 * @endcode
 */
double eval_expr_with_vars(const char *expr, const char **names, const double *values, size_t count, int *error) {
    if (!expr || (count > 0 && (!names || !values))) {
        EVALEXPR_LOG("[eval_expr_with_vars] : Error: NULL expression or var table.");
        if (error) {
            *error = EVAL_EXPR_ERROR_TOKENIZE;
        }
        return NAN;
    }
    EVALEXPR_LOG("[eval_expr_with_vars] : Evaluating expression: %s", expr);

    Token tokens[MAX_TOKENS];
    Token rpnTokens[MAX_TOKENS];

    int numTokens = tokenize(expr, tokens, names, values, count);
    if (numTokens < 0) {
        EVALEXPR_LOG("[eval_expr_with_vars] : Tokenization failed for expression: %s", expr);
        if (error) {
            *error = EVAL_EXPR_ERROR_TOKENIZE;
        }
        return NAN;
    }

    int rpnCount = shunting_yard(tokens, numTokens, rpnTokens);
    if (rpnCount < 0) {
        EVALEXPR_LOG("[eval_expr_with_vars] : Shunting-yard conversion failed for expression: %s", expr);
        if (error) {
            *error = EVAL_EXPR_ERROR_SHUNTING;
        }

        return NAN;
    }

    double result = 0.0;
    if (eval_rpn(rpnTokens, rpnCount, &result) != 0) {
        EVALEXPR_LOG("[eval_expr_with_vars] : RPN evaluation failed for expression: %s", expr);
        if (error) {
            *error = EVAL_EXPR_ERROR_EVAL_RPN;
        }

        return NAN;
    }

    EVALEXPR_LOG("[eval_expr_with_vars] : Expression evaluated successfully, result = %g", result);
    if (error) {
        *error = EVAL_EXPR_SUCCESS;
    }

    return result;
}


/**
 * Converts an arithmetic expression from infix notation to a string in Reverse Polish Notation (RPN).
 *
 * The returned string is dynamically allocated. The caller must free it.
 * There is no length limit, and every number is written with as many digits
 * as it takes (15 to 17 significant) to read back as the same double, with
 * '.' as the decimal point.
 *
 * @param expr A null‐terminated C string containing the infix expression.
 * @return A newly allocated string representing the RPN form, or NULL on error.
 */
char *eval_expr_to_rpn_string(const char *expr) {
    if (!expr) {
        EVALEXPR_LOG("[eval_expr_to_rpn_string] : Error: NULL expression.");
        return NULL;
    }
    EVALEXPR_LOG("[eval_expr_to_rpn_string] : Converting expression to RPN: %s", expr);

    Token tokens[MAX_TOKENS];
    Token rpnTokens[MAX_TOKENS];

    int numTokens = tokenize(expr, tokens, NULL, NULL, 0);
    if (numTokens < 0) {
        EVALEXPR_LOG("[eval_expr_to_rpn_string] : Tokenization failed for expression: %s", expr);
        return NULL;
    }

    int rpnCount = shunting_yard(tokens, numTokens, rpnTokens);
    if (rpnCount < 0) {
        EVALEXPR_LOG("[eval_expr_to_rpn_string] : Shunting-yard conversion failed for expression: %s", expr);
        return NULL;
    }

    char *text = eval_rpn_text(rpnTokens, rpnCount);
    if (!text) {
        EVALEXPR_LOG("[eval_expr_to_rpn_string] : Out of memory while building the RPN string for expression: %s", expr);
        return NULL;
    }

    EVALEXPR_LOG("[eval_expr_to_rpn_string] : Successfully converted expression to RPN: %s", text);
    return text;
}


/**
 * Checks whether an arithmetic expression is syntactically valid.
 *
 * This function tokenizes and converts the expression into RPN.
 *
 * @param expr A null‐terminated C string containing the expression.
 * @return 1 if the expression is valid, 0 otherwise.
 */
int eval_expr_is_valid(const char *expr) {
    if (!expr) {
        EVALEXPR_LOG("[eval_expr_is_valid] : Error: NULL expression.");
        return 0;
    }
    Token tokens[MAX_TOKENS];
    Token rpnTokens[MAX_TOKENS];

    int numTokens = tokenize(expr, tokens, NULL, NULL, 0);
    if (numTokens < 0) {
        EVALEXPR_LOG("[eval_expr_is_valid] : Tokenization failed for expression: %s", expr);
        return 0;
    }

    int rpnCount = shunting_yard(tokens, numTokens, rpnTokens);
    if (rpnCount < 0) {
        EVALEXPR_LOG("[eval_expr_is_valid] : Shunting-yard conversion failed for expression: %s", expr);
        return 0;
    }

    EVALEXPR_LOG("[eval_expr_is_valid] : Expression is valid: %s", expr);
    return 1;
}


/**
 * Returns a descriptive error message corresponding to an error code.
 *
 * @param error An error code (one of EVAL_EXPR_SUCCESS, EVAL_EXPR_ERROR_TOKENIZE,
 *              EVAL_EXPR_ERROR_SHUNTING, EVAL_EXPR_ERROR_EVAL_RPN).
 * @return A constant string describing the error.
 */
const char *eval_expr_error_message(int error) {
    EVALEXPR_LOG("[eval_expr_error_message] : Received error code %d", error);
    const char *msg = NULL;
    switch (error) {
        case EVAL_EXPR_SUCCESS:
            msg = "Success";
            break;
        case EVAL_EXPR_ERROR_TOKENIZE:
            msg = "Error tokenizing expression";
            break;
        case EVAL_EXPR_ERROR_SHUNTING:
            msg = "Error in shunting-yard conversion (mismatched parentheses or operator issue)";
            break;
        case EVAL_EXPR_ERROR_EVAL_RPN:
            msg = "Error evaluating RPN expression";
            break;
        default:
            msg = "Unknown error";
            break;
    }
    EVALEXPR_LOG("[eval_expr_error_message] : Returning message: %s", msg);
    return msg;
}


/**
 * Tokenizes an arithmetic expression and returns an array of strings representing each token.
 *
 * For a number token, the string will be the number with as many digits as it
 * takes (15 to 17 significant) to read back as the same double, with '.' as
 * the decimal point.
 * For an operator token, the string will contain the operator character.
 * For parentheses, the string will be "(" or ")".
 *
 * @param expr A null‐terminated C string containing the expression.
 * @param numTokens Output pointer where the number of tokens will be stored.
 * @return A dynamically allocated array of strings representing tokens,
 *         or NULL on error.
 */
char **eval_expr_tokenize(const char *expr, int *numTokens) {
    if (!expr) {
        EVALEXPR_LOG("[eval_expr_tokenize] : Error: NULL expression.");
        if (numTokens) *numTokens = 0;
        return NULL;
    }
    EVALEXPR_LOG("[eval_expr_tokenize] : Tokenizing expression: %s", expr);

    Token tokens[MAX_TOKENS];
    int count = tokenize(expr, tokens, NULL, NULL, 0);

    if (count < 0) {
        EVALEXPR_LOG("[eval_expr_tokenize] : Tokenization failed for expression: %s", expr);
        if (numTokens) {
            *numTokens = 0;
        }
        return NULL;
    }
    if (numTokens) {
        *numTokens = count;
    }
    EVALEXPR_LOG("[eval_expr_tokenize] : Tokenized %d tokens from expression: %s", count, expr);

    if (count == 0) {
        return NULL;
    }

    char **tokenStrs = (char **)malloc(count * sizeof(char *));
    if (!tokenStrs) {
        EVALEXPR_LOG("[eval_expr_tokenize] : Memory allocation for token array failed.");
        return NULL;
    }

    for (int i = 0; i < count; i++) {
        char buffer[64] = {0};
        switch (tokens[i].type) {
            case TOKEN_NUMBER:
                eval_format_number(tokens[i].value, buffer, sizeof(buffer));
                break;
            case TOKEN_OPERATOR:
                op_to_text(tokens[i].op, 0, buffer, sizeof(buffer));
                break;
            case TOKEN_PAREN_LEFT:
                strncpy(buffer, "(", sizeof(buffer) - 1);
                break;
            case TOKEN_PAREN_RIGHT:
                strncpy(buffer, ")", sizeof(buffer) - 1);
                break;
            default:
                strncpy(buffer, "?", sizeof(buffer) - 1);
                break;
        }

        tokenStrs[i] = eval_strdup(buffer);
        if (!tokenStrs[i]) {
            EVALEXPR_LOG("[eval_expr_tokenize] : Memory allocation failed for token %d", i);
            for (int j = 0; j < i; j++) {
                free(tokenStrs[j]);
            }

            free(tokenStrs);
            if (numTokens) {
                *numTokens = 0;
            }
            return NULL;
        }
    }

    EVALEXPR_LOG("[eval_expr_tokenize] : Successfully allocated token strings array for %d tokens", count);
    return tokenStrs;
}


/**
 * Frees the token-string array returned by eval_expr_tokenize().
 *
 * The matching deallocator for eval_expr_tokenize(): it frees each individual
 * token string and then the array itself, so callers never have to unwind the
 * allocation by hand. Safe to call with a NULL array (no-op). Pass the same
 * `count` that eval_expr_tokenize() reported through its `numTokens` out-param.
 *
 * @param tokens The array returned by eval_expr_tokenize(). NULL is allowed.
 * @param count  The number of token strings in the array.
 *
 * @code
 * int n = 0;
 * char** toks = eval_expr_tokenize("3 + 4 * 2", &n);
 * // ... use toks[0..n) ...
 * eval_expr_free_tokens(toks, n);
 * @endcode
 */
void eval_expr_free_tokens(char **tokens, int count) {
    EVALEXPR_LOG("[eval_expr_free_tokens] : Freeing %d token strings (tokens=%p)", count, (void*)tokens);
    if (!tokens) {
        return;
    }
    for (int i = 0; i < count; i++) {
        free(tokens[i]);
    }
    
    free(tokens);
}


/**
 * Prints detailed debug information for an arithmetic expression.
 *
 * This function tokenizes the given expression, prints the tokens,
 * converts the expression to Reverse Polish Notation (RPN), prints the RPN form,
 * and finally evaluates the expression and prints the final result.
 *
 * @param expr A null-terminated C string containing the arithmetic expression.
 */
void eval_expr_print_debug(const char *expr) {
    if (!expr) {
        printf("Debug: NULL expression.\n");
        return;
    }
    EVALEXPR_LOG("[eval_expr_print_debug] : Debugging expression: %s", expr);

    Token tokens[MAX_TOKENS];
    int numTokens = tokenize(expr, tokens, NULL, NULL, 0);
    if (numTokens < 0) {
        printf("Debug: Failed to tokenize the expression.\n");
        return;
    }

    printf("Debug: Tokens:\n");
    for (int i = 0; i < numTokens; i++) {
        if (tokens[i].type == TOKEN_NUMBER) {
            char number[40];
            eval_format_number(tokens[i].value, number, sizeof(number));
            printf("  Token %d: [NUMBER] %s\n", i, number);
        } 
        else if (tokens[i].type == TOKEN_OPERATOR) {
            char opText[8];
            op_to_text(tokens[i].op, 0, opText, sizeof(opText));
            printf("  Token %d: [OPERATOR] %s\n", i, opText);
        } 
        else if (tokens[i].type == TOKEN_PAREN_LEFT) {
            printf("  Token %d: [PAREN_LEFT] (\n", i);
        } 
        else if (tokens[i].type == TOKEN_PAREN_RIGHT) {
            printf("  Token %d: [PAREN_RIGHT] )\n", i);
        }
    }

    // Convert infix tokens to RPN.
    Token rpnTokens[MAX_TOKENS];
    int rpnCount = shunting_yard(tokens, numTokens, rpnTokens);
    if (rpnCount < 0) {
        printf("Debug: Failed to convert expression to RPN.\n");
        return;
    }

    // Build and print RPN string.
    char *rpnText = eval_rpn_text(rpnTokens, rpnCount);
    printf("Debug: RPN: %s\n", rpnText ? rpnText : "(out of memory)");
    free(rpnText);

    double result = eval_expr(expr);
    if (result != result) {  
        printf("Debug: Evaluation failed.\n");
    } 
    else {
        printf("Debug: Result: %g\n", result);
    }

    EVALEXPR_LOG("[eval_expr_print_debug] : Debugging complete for expression: %s", expr);
}
