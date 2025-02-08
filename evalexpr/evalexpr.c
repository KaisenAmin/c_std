#include "evalexpr.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


/* Returns the precedence of the operator. Higher numbers mean higher precedence. */
static int precedence(char op) {
    switch (op) {
        case '^': 
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

/* Returns whether the operator is right-associative. (Only exponentiation is.) */
static int is_right_associative(char op) {
    return (op == '^');
}

/* Returns whether the character is a valid operator. */
static int is_operator(char ch) {
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^');
}

/**
 * Parses the input expression string into an array of tokens.
 *
 * @param expr The expression string.
 * @param tokens The output array of tokens.
 * @return The number of tokens parsed, or -1 on error.
 */
static int tokenize(const char *expr, Token tokens[MAX_TOKENS]) {
    int pos = 0;       // current position in expr
    int tcount = 0;    // number of tokens produced

    while (expr[pos] != '\0') {
        // Skip any whitespace.
        if (isspace(expr[pos])) {
            pos++;
            continue;
        }

        // If the character is a digit or a decimal point, parse a number.
        if (isdigit(expr[pos]) || expr[pos] == '.') {
            char *endptr;
            errno = 0;
            double val = strtod(&expr[pos], &endptr);
            if (errno != 0) {
                EVALEXPR_LOG("[tokenize] : Error converting number.");
                return -1;
            }
            if (tcount >= MAX_TOKENS) {
                EVALEXPR_LOG("[tokenize] : Token buffer overflow.");
                return -1;
            }

            tokens[tcount].type = TOKEN_NUMBER;
            tokens[tcount].value = val;
            tcount++;
            pos = endptr - expr;
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
static int shunting_yard(const Token tokens[MAX_TOKENS], int numTokens, Token output[MAX_TOKENS]) {
    Token opStack[MAX_TOKENS];
    int opCount = 0;
    int outCount = 0;

    for (int i = 0; i < numTokens; i++) {
        Token token = tokens[i];

        if (token.type == TOKEN_NUMBER) {
            output[outCount++] = token;
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
    Token tokens[MAX_TOKENS];
    Token rpnTokens[MAX_TOKENS];

    int numTokens = tokenize(expr, tokens);
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
    EVALEXPR_LOG("[eval_expr_strict] : Evaluating expression: %s", expr);

    Token tokens[MAX_TOKENS];
    Token rpnTokens[MAX_TOKENS];

    int numTokens = tokenize(expr, tokens);
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
 * Converts an arithmetic expression from infix notation to a string in Reverse Polish Notation (RPN).
 *
 * The returned string is dynamically allocated. The caller must free it.
 *
 * @param expr A null‐terminated C string containing the infix expression.
 * @return A newly allocated string representing the RPN form, or NULL on error.
 */
char *eval_expr_to_rpn_string(const char *expr) {
    EVALEXPR_LOG("[eval_expr_to_rpn_string] : Converting expression to RPN: %s", expr);

    Token tokens[MAX_TOKENS];
    Token rpnTokens[MAX_TOKENS];
    char buffer[1024] = {0};
    size_t pos = 0;

    int numTokens = tokenize(expr, tokens);
    if (numTokens < 0) {
        EVALEXPR_LOG("[eval_expr_to_rpn_string] : Tokenization failed for expression: %s", expr);
        return NULL;
    }

    int rpnCount = shunting_yard(tokens, numTokens, rpnTokens);
    if (rpnCount < 0) {
        EVALEXPR_LOG("[eval_expr_to_rpn_string] : Shunting-yard conversion failed for expression: %s", expr);
        return NULL;
    }

    // Build a string representing the RPN expression.
    for (int i = 0; i < rpnCount; i++) {
        if (rpnTokens[i].type == TOKEN_NUMBER) {
            int n = snprintf(buffer + pos, sizeof(buffer) - pos, "%g ", rpnTokens[i].value);
            if (n < 0 || (size_t)n >= sizeof(buffer) - pos) {
                EVALEXPR_LOG("[eval_expr_to_rpn_string] : Buffer overflow while building RPN string for expression: %s", expr);
                break;
            }
            pos += n;
        } 
        else if (rpnTokens[i].type == TOKEN_OPERATOR) {
            int n = snprintf(buffer + pos, sizeof(buffer) - pos, "%c ", rpnTokens[i].op);
            if (n < 0 || (size_t)n >= sizeof(buffer) - pos) {
                EVALEXPR_LOG("[eval_expr_to_rpn_string] : Buffer overflow while building RPN string for expression: %s", expr);
                break;
            }
            pos += n;
        }
    }

    // Remove trailing space, if any.
    if (pos > 0 && buffer[pos - 1] == ' ') {
        buffer[pos - 1] = '\0';
    } 
    else {
        buffer[pos] = '\0';
    }

    EVALEXPR_LOG("[eval_expr_to_rpn_string] : Successfully converted expression to RPN: %s", buffer);
    return strdup(buffer);
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
    Token tokens[MAX_TOKENS];
    Token rpnTokens[MAX_TOKENS];

    int numTokens = tokenize(expr, tokens);
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
 * For a number token, the string will be the number (using %g format).
 * For an operator token, the string will contain the operator character.
 * For parentheses, the string will be "(" or ")".
 *
 * @param expr A null‐terminated C string containing the expression.
 * @param numTokens Output pointer where the number of tokens will be stored.
 * @return A dynamically allocated array of strings representing tokens,
 *         or NULL on error.
 */
char **eval_expr_tokenize(const char *expr, int *numTokens) {
    EVALEXPR_LOG("[eval_expr_tokenize] : Tokenizing expression: %s", expr);
    Token tokens[MAX_TOKENS];
    int count = tokenize(expr, tokens);

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

    char **tokenStrs = (char **)malloc(count * sizeof(char *));
    if (!tokenStrs) {
        EVALEXPR_LOG("[eval_expr_tokenize] : Memory allocation for token array failed.");
        return NULL;
    }

    for (int i = 0; i < count; i++) {
        char buffer[64] = {0};
        switch (tokens[i].type) {
            case TOKEN_NUMBER:
                snprintf(buffer, sizeof(buffer), "%g", tokens[i].value);
                break;
            case TOKEN_OPERATOR:
                snprintf(buffer, sizeof(buffer), "%c", tokens[i].op);
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
        tokenStrs[i] = strdup(buffer);
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
 * Prints detailed debug information for an arithmetic expression.
 *
 * This function tokenizes the given expression, prints the tokens,
 * converts the expression to Reverse Polish Notation (RPN), prints the RPN form,
 * and finally evaluates the expression and prints the final result.
 *
 * @param expr A null-terminated C string containing the arithmetic expression.
 */
void eval_expr_print_debug(const char *expr) {
    EVALEXPR_LOG("[eval_expr_print_debug] : Debugging expression: %s", expr);

    Token tokens[MAX_TOKENS];
    int numTokens = tokenize(expr, tokens);
    if (numTokens < 0) {
        printf("Debug: Failed to tokenize the expression.\n");
        return;
    }

    printf("Debug: Tokens:\n");
    for (int i = 0; i < numTokens; i++) {
        if (tokens[i].type == TOKEN_NUMBER) {
            printf("  Token %d: [NUMBER] %g\n", i, tokens[i].value);
        } 
        else if (tokens[i].type == TOKEN_OPERATOR) {
            printf("  Token %d: [OPERATOR] %c\n", i, tokens[i].op);
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
    char buffer[1024] = {0};
    size_t pos = 0;
    for (int i = 0; i < rpnCount; i++) {
        if (rpnTokens[i].type == TOKEN_NUMBER) {
            int n = snprintf(buffer + pos, sizeof(buffer) - pos, "%g ", rpnTokens[i].value);
            if (n < 0 || (size_t)n >= sizeof(buffer) - pos)
                break;
            pos += n;
        } 
        else if (rpnTokens[i].type == TOKEN_OPERATOR) {
            int n = snprintf(buffer + pos, sizeof(buffer) - pos, "%c ", rpnTokens[i].op);
            if (n < 0 || (size_t)n >= sizeof(buffer) - pos)
                break;
            pos += n;
        }
    }
    
    // Remove trailing space.
    if (pos > 0 && buffer[pos - 1] == ' ') {
        buffer[pos - 1] = '\0';
    }
    printf("Debug: RPN: %s\n", buffer);

    double result = eval_expr(expr);
    if (result != result) {  
        printf("Debug: Evaluation failed.\n");
    } 
    else {
        printf("Debug: Result: %g\n", result);
    }

    EVALEXPR_LOG("[eval_expr_print_debug] : Debugging complete for expression: %s", expr);
}