#include <stdio.h>
#include <string.h>
#include "archivos.h"
#include <dirent.h>
#include <stdbool.h>

int guardarMetadatos(char *archivo, Metadatos *meta) {
    char nombreMeta[200];
    snprintf(nombreMeta, sizeof(nombreMeta), "%s.meta", archivo);
    FILE *f = fopen(nombreMeta, "w");
    if (!f) return -1;
    fprintf(f, "%d %d %d %d %d %d %s\n",
            meta->pk_pos, meta->pk_len, meta->lrecl,
            meta->fk_pos, meta->fk_len, meta->tiene_fk,
            meta->fk_archivo);
    fclose(f);
    return 0;
}

int cargarMetadatos(char *archivo, Metadatos *meta) {
    char nombreMeta[200];
    snprintf(nombreMeta, sizeof(nombreMeta), "%s.meta", archivo);
    FILE *f = fopen(nombreMeta, "r");
    if (!f) return -1;
    fscanf(f, "%d %d %d %d %d %d %99s",
           &meta->pk_pos, &meta->pk_len, &meta->lrecl,
           &meta->fk_pos, &meta->fk_len, &meta->tiene_fk,
           meta->fk_archivo);
    fclose(f);
    return 0;
}

bool es_PK_referenciada(const char* archivo_pk, const char* pk_a_borrar) {
    DIR* dir = opendir(".");
    if (!dir) return false;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".meta")) {
            char base[200];
            snprintf(base, sizeof(base), "%.*s",
                     (int)(strlen(entry->d_name) - 5), entry->d_name);

            Metadatos meta;
            if (cargarMetadatos(base, &meta) == 0) {
                if (meta.tiene_fk && strcmp(meta.fk_archivo, archivo_pk) == 0) {
                    // Abrir el archivo de datos correspondiente
                    char nombreDat[200];
                    snprintf(nombreDat, sizeof(nombreDat), "%s", base);
                    FILE* f = fopen(nombreDat, "r");
                    if (!f) continue;

                    char linea[MAX_REG];
                    while (fgets(linea, sizeof(linea), f)) {
                        if (linea[0] == BORRADO) continue;

                        char valor_fk[100];
                        strncpy(valor_fk, linea + meta.fk_pos, meta.fk_len);
                        valor_fk[meta.fk_len] = '\0';

                        if (strcmp(valor_fk, pk_a_borrar) == 0) {
                            fclose(f);
                            closedir(dir);
                            return true;
                        }
                    }

                    fclose(f);
                }
            }
        }
    }

    closedir(dir);
    return false;
}
