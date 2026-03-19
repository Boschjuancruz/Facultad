#ifndef PARSING_H
#define PARSING_H

// Estructura de parámetros de comandos
typedef struct rpar {
    char *comando;
    char *archivo;
    int pkpos;
    int pklen;
    int lrecl;
    int fkpos;
    int fklen;
    char *fkarchivo;
    char *registro;
    char *pk;
} rpar;

// Códigos de error
enum errorParsing {
    ERR_NO_HAY,
    ERR_CMD_NO_ENCONTRADO,
    ERR_MAL_ARG,
    ERR_TOKEN_PK_NO_ENCONTRADO,
    ERR_TOKEN_COMA_NO_ENCONTRADO,
    ERR_SIN_ESPACIO,
    ERR_CAR_NO_NUMERICO,
    ERR_LRECL_NO_ENCONTRADO,
    ERR_FK_NO_ENCONTRADO,
    ERR_REG_NO_ENCONTRADO,
    ERR_PK_MAL_LARGO
};

// Prototipos
enum errorParsing parsing(rpar *p, int narg, char **arg);
void libero(rpar *p);

#endif
