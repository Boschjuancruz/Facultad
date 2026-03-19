#ifndef TP1_H
#define TP1_H

#define MAX_REG 256    // Tamaño máximo de un registro
#define BORRADO '*'    // Flag de borrado lógico

// ---------------- Estructura de metadatos ----------------
// lrecl: longitud fija del registro (incluye el flag de borrado lógico)
typedef struct {
    int pk_pos;              // posición de la PK dentro del registro
    int pk_len;              // longitud de la PK
    int lrecl;               // longitud total del registro (incluido flag de borrado)
    int fk_pos;              // posición de la FK dentro del registro
    int fk_len;              // longitud de la FK (igual a la PK del archivo referenciado)
    int tiene_fk;            // 1 si existe FK, 0 si no
    char fk_archivo[100];    // nombre del archivo referenciado por la FK
} Metadatos;

// ---------------- Prototipos de comandos ----------------
void crear_archivo(char *archivo, int pkpos, int pklen, int lrecl, int fkpos, char *fkarchivo);
void insertar_registro(char *archivo, char *registro);
void borrar_registro(char *archivo, char *pk);
void leer_registro(char *archivo, char *pk);
void mostrar_archivo(char *archivo);
void cambiar_registro(char *archivo, char *pkvieja, char *regnuevo);
void salir_programa();

#endif
