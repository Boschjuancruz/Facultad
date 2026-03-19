#ifndef ARCHIVOS_H
#define ARCHIVOS_H
#include <stdbool.h>
#include "TP1.h"

// Guardar y cargar metadatos
int guardarMetadatos(char *archivo, Metadatos *meta);
int cargarMetadatos(char *archivo, Metadatos *meta);

// Chequear si una PK está referenciada en algún archivo con FK
bool es_PK_referenciada(const char* archivo_pk, const char* pk_a_borrar);

#endif
