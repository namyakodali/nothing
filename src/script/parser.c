#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "script/parser.h"

static void skip_whitespaces(const char *str, size_t *cursor, size_t n)
{
    assert(str);
    assert(cursor);

    while (*cursor < n && isspace(str[*cursor])) {
        (*cursor)++;
    }
}

struct ParseResult create_expr_from_str(const char *str,
                                        size_t *cursor,
                                        size_t n)
{
    assert(str);
    assert(cursor);

    /* TODO: create_expr_from_str doesn't no support comments */

    skip_whitespaces(str, cursor, n);
    if (*cursor >= n) {
        return parse_failure("EOF");
    }

    switch (str[*cursor]) {
    case '(': {
        (*cursor)++;
        struct ParseResult car = create_expr_from_str(str, cursor, n);
        if (car.is_error) {
            return car;
        }

        skip_whitespaces(str, cursor, n);
        if (*cursor >= n) {
            return parse_failure("EOF");
        }

        if (str[*cursor] != '.') {
            return parse_failure("Expected .");
        }
        (*cursor)++;

        skip_whitespaces(str, cursor, n);
        if (*cursor >= n) {
            return parse_failure("EOF");
        }

        struct ParseResult cdr = create_expr_from_str(str, cursor, n);
        if (cdr.is_error) {
            return cdr;
        }

        skip_whitespaces(str, cursor, n);
        if (*cursor >= n) {
            return parse_failure("EOF");
        }

        if (str[*cursor] != ')') {
            return parse_failure("Expected )");
        }

        (*cursor)++;

        return parse_success(cons_as_expr(create_cons(car.expr, cdr.expr)));
    }

    case '"': {
        /* TODO(#288): create_expr_from_str does not support strings */
        return parse_failure("Strings are not supported");
    }

    default: {
        if (isdigit(str[*cursor])) {
            const char *nptr = str + *cursor;
            char *endptr = 0;
            const double x = strtod(nptr, &endptr);

            if (nptr == endptr) {
                return parse_failure("Number expected");
            }

            *cursor += (size_t) (endptr - nptr);

            return parse_success(atom_as_expr(create_atom(ATOM_NUMBER, x)));
        } else if (isalpha(str[*cursor])) {
            /* TODO(#289): create_expr_from_str does not support symbols */
            return parse_failure("Symbols are not supported");
        }
    }
    }

    return parse_failure("Unexpected sequence of characters");
}

struct ParseResult parse_success(struct Expr expr)
{
    struct ParseResult result = {
        .is_error = false,
        .expr = expr
    };

    return result;
}

struct ParseResult parse_failure(const char *error)
{
    struct ParseResult result = {
        .is_error = true,
        .error = error
    };

    return result;
}
