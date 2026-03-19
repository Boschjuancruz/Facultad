#ifndef INDICE_H
#define INDICE_H

#include "TP1.h"

typedef struct {
    char pk[50];   // valor de la PK
    long pos;    // número de registro en el archivo
} EntradaIndice;

// Construir índice desde el archivo .dat
int construirIndice(const char *archivo, Metadatos *meta, EntradaIndice *indice, int *total);

// Buscar PK en el índice
int buscarEnIndice(const char *pk, EntradaIndice *indice, int total);

// Guardar índice en disco
int guardarIndice(const char *archivo, EntradaIndice *indice, int total);

// Cargar índice desde disco
int cargarIndice(const char *archivo, EntradaIndice *indice, int *total);

void extraerPK(char *dest, const char *registro, Metadatos *meta);

#endif
