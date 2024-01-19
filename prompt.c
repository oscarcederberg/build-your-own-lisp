#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char *
readline (char * prompt) {
    fputs (prompt, stdout);
    fgets (buffer, 2048, stdin);
    char *copy = malloc (strlen (buffer) + 1);
    strcpy (copy, buffer);
    copy[strlen (cpy) - 1] = '\0';
    return copy;
}

void
add_history (char * unused) {}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

static inline void freep(void *p) {
        free(*(void**) p);
}

#define cleanup_free cleanup (freep)

int main (int argc, char ** argv) {
    puts ("Lispy Version 0.0.1");
    puts ("Press CTRL+C to Exit\n");

    while (1) {
        char *input __attribute__((cleanup_free)) =
            readline ("lispy> ");

        add_history (input);
    }

    return 0;
}
