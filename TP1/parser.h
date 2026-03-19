#ifndef PARSER_H
#define PARSER_H

#include "parsing.h"

// Alias de tipo
typedef rpar ComandoParseado;

// Alias de funciones
static inline int parsear(ComandoParseado *p, int narg, char **arg) {
    return parsing(p, narg, arg);
}
static inline void liberarParseo(ComandoParseado *p) {
    libero(p);
}

#endif
