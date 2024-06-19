#include <assert.h>
#include <editline/readline.h>
#include <editline/history.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

typedef struct language_t {
    mpc_parser_t *number;
    mpc_parser_t *operator;
    mpc_parser_t *expr;
    mpc_parser_t *lispy;
} language_t;

long ast_eval_operator (long left, char * operator, long right) {
    long value;

    if (strcmp (operator, "+") == 0) {
        value = left + right;
    } else if (strcmp (operator, "-") == 0) {
        value = left - right;
    } else if (strcmp (operator, "*") == 0) {
        value = left * right;
    } else if (strcmp (operator, "/") == 0) {
        value = left / right;
    } else {
        assert (false);
    }

    return value;
}

long ast_eval (mpc_ast_t * ast) {
    char *operator;
    long value;

    assert (ast != NULL);

    if (strstr (ast->tag, "number")) {
        return atoi (ast->contents);
    }

    operator = ast->children[1]->contents;

    value = ast_eval (ast->children[2]);

    for (size_t i = 3; strstr (ast->children[i]->tag, "expr"); i++) {
        value = ast_eval_operator (value, operator, ast_eval (ast->children[i]));
    }

    return value;
}

bool language_parse (language_t * language, const char * input, mpc_result_t *
        result) {
    return mpc_parse ("<stdin>", input, language->lispy, result);
}

language_t * language_new (void) {
    language_t *language = calloc (1, sizeof(language_t));
    language->number = mpc_new ("number");
    language->operator = mpc_new ("operator");
    language->expr = mpc_new ("expr");
    language->lispy = mpc_new ("lispy");

    mpca_lang (MPCA_LANG_DEFAULT,
        "                                                                      \
            number      :   /-?[0-9]+/ ;                                       \
            operator    :   '+' | '-' | '*' | '/' ;                            \
            expr        :   <number> |  '(' <operator> <expr>+ ')' ;           \
            lispy       :   /^/ <operator> <expr>+ /$/ ;                       \
        ",
        language->number, language->operator, language->expr, language->lispy
    );

    return language;
}

void language_free (language_t * language) {
    assert (language != NULL);

    mpc_cleanup (
        4, language->number, language->operator, language->expr, language->lispy
    );

    free (language);
}

int main (int argc, char ** argv) {
    puts ("Lispy Version 0.0.1");
    puts ("Press CTRL+C to Exit\n");

    language_t *language = language_new ();

    while (1) {
        char *input = readline ("lispy> ");
        mpc_result_t result;
        long value;

        if (language_parse (language, input, &result)) {
            value = ast_eval (result.output);
            printf ("%li\n", value);
            mpc_ast_delete (result.output);
        } else {
            mpc_err_print (result.error);
            mpc_err_delete (result.error);
        }

        add_history (input);
        free (input);
    }

    language_free (language);
    return 0;
}
