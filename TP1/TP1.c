#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TP1.h"
#include "archivos.h"
#include "indice.h"

// ---------------- Funciones auxiliares ----------------

// Extrae la PK de un registro según los metadatos
void extraerPK(char *dest, const char *registro, Metadatos *meta) {
    strncpy(dest, registro + meta->pk_pos, meta->pk_len);
    dest[meta->pk_len] = '\0';
}

// Extrae la FK de un registro según los metadatos
void extraerFK(char *dest, const char *registro, Metadatos *meta) {
    strncpy(dest, registro + meta->fk_pos, meta->fk_len);
    dest[meta->fk_len] = '\0';
}

// Busca un registro directamente en el archivo (fallback)
int buscarRegistroPorPK(const char *archivo, const char *pk, Metadatos *meta, char *linea) {
    FILE *f = fopen(archivo, "r");
    if (!f) return -1;
    int nro = 0;
    char buffer[MAX_REG];
    while (fgets(buffer, sizeof(buffer), f)) {
        buffer[strcspn(buffer, "\n")] = 0;
        char pkEncontrada[MAX_REG];
        extraerPK(pkEncontrada, buffer, meta);
        if (strcmp(pkEncontrada, pk) == 0) {
            if (linea) strcpy(linea, buffer);
            fclose(f);
            return nro;
        }
        nro++;
    }
    fclose(f);
    return -1;
}

// Sobrescribe un registro en la posición indicada
int sobrescribirRegistro(const char *archivo, int nroReg, const char *nuevo, int lrecl) {
    FILE *f = fopen(archivo, "r+");
    if (!f) return -1;
    fseek(f, 0, SEEK_SET);

    char buffer[MAX_REG][MAX_REG];
    int total = 0;

    while (fgets(buffer[total], sizeof(buffer[0]), f) && total < MAX_REG) {
        buffer[total][strcspn(buffer[total], "\n")] = 0;
        total++;
    }

    if (nroReg >= total) {
        fclose(f);
        return -1;
    }

    snprintf(buffer[nroReg], lrecl + 1, "%-*s", lrecl, nuevo);

    freopen(archivo, "w", f);
    for (int i = 0; i < total; i++) {
        fprintf(f, "%s\n", buffer[i]);
    }

    fclose(f);
    return 0;
}

// ---------------- Comandos ----------------

// Crear un archivo nuevo con sus metadatos + índice vacío
void crear_archivo(char *archivo, int pkpos, int pklen, int lrecl, int fkpos, char *fkarchivo) {
    if (pkpos < 0 || pklen <= 0 || lrecl <= pklen || (fkpos >= 0 && (fkpos < 0 || fkpos + pklen > lrecl))) {
        printf("Error: posiciones o longitudes invalidas\n");
        return;
    }

    FILE *f = fopen(archivo, "w");
    if (!f) { printf("Error: no se pudo crear el archivo\n"); return; }
    fclose(f);

    Metadatos meta;
    meta.pk_pos = pkpos;
    meta.pk_len = pklen;
    meta.lrecl  = lrecl;
    meta.fk_pos = fkpos;
    meta.fk_len = pklen; // FK debe coincidir con PK del archivo referenciado
    meta.tiene_fk = 0;
    strcpy(meta.fk_archivo, "");

    if (fkarchivo && fkpos != -1) {
        // Validar existencia del archivo FK
        FILE *fk = fopen(fkarchivo, "r");
        if (!fk) {
            printf("Error: el archivo de referencia FK '%s' no existe.\n", fkarchivo);
            return;
        }
        fclose(fk);

        strcpy(meta.fk_archivo, fkarchivo);
        meta.tiene_fk = 1;
    }

    if (guardarMetadatos(archivo, &meta) != 0) {
        printf("Error: no se pudo guardar metadatos\n");
        return;
    }

    // Crear índice vacío
    EntradaIndice indice[1];
    if (guardarIndice(archivo, indice, 0) != 0) {
        printf("Error: no se pudo crear índice\n");
        return;
    }
}

// Insertar un registro en un archivo y actualizar índice
void insertar_registro(char *archivo, char *registro) {
    Metadatos meta;
    if (cargarMetadatos(archivo, &meta) != 0) {
        printf("Error: archivo no existe\n");
        return;
    }
    if ((int)strlen(registro) > meta.lrecl) {
        printf("Error: registro demasiado largo\n");
        return;
    }

    char pkNueva[MAX_REG];
    extraerPK(pkNueva, registro, &meta);
    if (buscarEnIndice(pkNueva, NULL, 0) != -1) { // Usar índice cargado si disponible
        printf("Error: PK duplicada\n");
        return;
    }

    // Validar valor de FK si existe
    if (meta.tiene_fk) {
        char fkValor[MAX_REG];
        extraerFK(fkValor, registro, &meta);
        Metadatos metaFK;
        if (cargarMetadatos(meta.fk_archivo, &metaFK) == 0) {
            if (buscarRegistroPorPK(meta.fk_archivo, fkValor, &metaFK, NULL) == -1) {
                printf("Error: valor FK no existe en archivo referenciado\n");
                return;
            }
        }
    }

    FILE *f = fopen(archivo, "a");
    if (!f) { printf("Error: no se pudo abrir archivo\n"); return; }
    fprintf(f, "%-*s\n", meta.lrecl, registro);
    fclose(f);

    // Reconstruir índice
    EntradaIndice indice[1000];
    int total;
    construirIndice(archivo, &meta, indice, &total);
    guardarIndice(archivo, indice, total);
}

// Borrar un registro y actualizar índice
void borrar_registro(char *archivo, char *pk) {
    Metadatos meta;
    if (cargarMetadatos(archivo, &meta) != 0) {
        printf("Error: archivo no existe\n");
        return;
    }

    EntradaIndice indice[1000];
    int total;
    if (cargarIndice(archivo, indice, &total) != 0) {
        printf("Error: no se pudo cargar índice\n");
        return;
    }

    int nro = buscarEnIndice(pk, indice, total);
    if (nro == -1) {
        printf("Error: PK no encontrada\n");
        return;
    }

    char linea[MAX_REG];
    buscarRegistroPorPK(archivo, pk, &meta, linea);

    if (linea[0] == BORRADO) {
        printf("Error: registro ya borrado\n");
        return;
    }

    char nuevo[MAX_REG];
    snprintf(nuevo, sizeof(nuevo), "%c%s", BORRADO, linea + 1);
    sobrescribirRegistro(archivo, nro, nuevo, meta.lrecl);

    // Reconstruir índice
    construirIndice(archivo, &meta, indice, &total);
    guardarIndice(archivo, indice, total);
}

// Leer un registro específico usando índice
void leer_registro(char *archivo, char *pk) {
    Metadatos meta;
    if (cargarMetadatos(archivo, &meta) != 0) return;

    EntradaIndice indice[1000];
    int total;
    if (cargarIndice(archivo, indice, &total) != 0) return;

    int idx = buscarEnIndice(pk, indice, total);
    if (idx == -1) { printf("PK no existe!\n"); return; }

    FILE *f = fopen(archivo, "r");
    if (!f) return;

    fseek(f, indice[idx].pos, SEEK_SET);
    char linea[MAX_REG];
    fgets(linea, sizeof(linea), f);
    linea[strcspn(linea, "\n")] = 0;
    fclose(f);

    char borrado = (linea[0] == BORRADO) ? 'b' : ' ';
    printf("%3d %c %s\n", idx, borrado, linea);
}

// Mostrar todo el archivo (también sirve para reconstruir índice)
void mostrar_archivo(char *archivo) {
    Metadatos meta;
    if (cargarMetadatos(archivo, &meta) != 0) {
        printf("Error: archivo no existe\n");
        return;
    }
    FILE *f = fopen(archivo, "r");
    if (!f) { printf("Error: no se pudo abrir archivo\n"); return; }

    char linea[MAX_REG];
    int nro = 0;
    while (fgets(linea, sizeof(linea), f)) {
        linea[strcspn(linea, "\n")] = 0;
        char borrado = (linea[0] == BORRADO) ? 'b' : ' ';
        printf("%3d %c %s\n", nro++, borrado, linea);
    }
    fclose(f);

    // Reconstruir índice
    EntradaIndice indice[1000];
    int total;
    construirIndice(archivo, &meta, indice, &total);
    guardarIndice(archivo, indice, total);
}

// Cambiar un registro y actualizar índice
void cambiar_registro(char *archivo, char *pkvieja, char *regnuevo) {
    Metadatos meta;
    if (cargarMetadatos(archivo, &meta) != 0) {
        printf("Error: archivo no existe\n");
        return;
    }

    EntradaIndice indice[1000];
    int total;
    cargarIndice(archivo, indice, &total);

    int nro = buscarEnIndice(pkvieja, indice, total);
    if (nro == -1) {
        printf("Error: PK vieja no existe\n");
        return;
    }

    // Validar que la PK vieja no esté referenciada
    if (es_PK_referenciada(archivo, pkvieja)) {
        printf("Error: no se puede cambiar, PK referenciada en otro archivo\n");
        return;
    }

    char pkNueva[MAX_REG];
    extraerPK(pkNueva, regnuevo, &meta);
    if (strcmp(pkNueva, pkvieja) != 0 && buscarEnIndice(pkNueva, indice, total) != -1) {
        printf("Error: nueva PK duplicada\n");
        return;
    }

    // Validar valor de FK si existe
    if (meta.tiene_fk) {
        char fkValor[MAX_REG];
        extraerFK(fkValor, regnuevo, &meta);
        Metadatos metaFK;
        if (cargarMetadatos(meta.fk_archivo, &metaFK) == 0) {
            if (buscarRegistroPorPK(meta.fk_archivo, fkValor, &metaFK, NULL) == -1) {
                printf("Error: valor FK no existe en archivo referenciado\n");
                return;
            }
        }
    }

    sobrescribirRegistro(archivo, nro, regnuevo, meta.lrecl);

    // Reconstruir índice
    construirIndice(archivo, &meta, indice, &total);
    guardarIndice(archivo, indice, total);
}

// Salir del programa
void salir_programa() {
    // Nada que imprimir si todo salió bien
}