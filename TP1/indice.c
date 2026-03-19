#include <stdio.h>
#include <string.h>
#include "indice.h"

// Construir el índice recorriendo el archivo
int construirIndice(const char *archivo, Metadatos *meta, EntradaIndice *indice, int *total) {
    FILE *f = fopen(archivo, "r");
    if (!f) return -1;
    char buffer[MAX_REG];
    *total = 0;

    while (1) {
        long pos = ftell(f);  // posición actual en bytes
        if (!fgets(buffer, sizeof(buffer), f)) break;
        buffer[strcspn(buffer, "\n")] = 0;
        if (buffer[0] == '*') continue; // registro borrado
        extraerPK(indice[*total].pk, buffer, meta);
        indice[*total].pos = pos;       // guardo la posición
        (*total)++;
    }
    fclose(f);
    return 0;
}


// Devuelve el índice en memoria (no nroReg)
int buscarEnIndice(const char *pk, EntradaIndice *indice, int total) {
    for (int i = 0; i < total; i++) {
        if (strcmp(indice[i].pk, pk) == 0)
            return i; // índice en memoria
    }
    return -1;
}


// Guardar índice con posiciones en bytes
int guardarIndice(const char *archivo, EntradaIndice *indice, int total) {
    char nombreIdx[200];
    snprintf(nombreIdx, sizeof(nombreIdx), "%s.idx", archivo);
    FILE *f = fopen(nombreIdx, "w");
    if (!f) return -1;
    for (int i = 0; i < total; i++) {
        fprintf(f, "%s %ld\n", indice[i].pk, indice[i].pos);
    }
    fclose(f);
    return 0;
}

// Cargar índice con posiciones en bytes
int cargarIndice(const char *archivo, EntradaIndice *indice, int *total) {
    char nombreIdx[200];
    snprintf(nombreIdx, sizeof(nombreIdx), "%s.idx", archivo);
    FILE *f = fopen(nombreIdx, "r");
    if (!f) return -1;
    *total = 0;
    while (fscanf(f, "%s %ld", indice[*total].pk, &indice[*total].pos) == 2) {
        (*total)++;
    }
    fclose(f);
    return 0;
}

