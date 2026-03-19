#include "parser.h"
#include <ctype.h>
#include <stdio.h>
// Shell interactivo estilo TP
void menu_parsing() {
    char linea[512];
    char *argv[10];
    int argc;
    ComandoParseado cmd;

    printf("Entrando en modo shell. Escriba 'salir' para terminar.\n");

    while (1) {
        printf("tp1> ");
        if (!fgets(linea, sizeof(linea), stdin)) break;
        linea[strcspn(linea, "\n")] = 0;

        if (strcmp(linea, "salir") == 0) {
            salir_programa();
            break;
        }

        // Tokenizar como argv
        argc = 0;
        char *token = strtok(linea, " ");
        while (token && argc < 10) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }

        if (argc == 0) continue;

        int err = parsear(&cmd, argc, argv);
        if (err != ERR_NO_HAY) {
            printf("Error: %s\n", ERR_TEXTO(err));
            liberarParseo(&cmd);
            continue;
        }

        // Ejecutar comando
        if (strcmp(cmd.comando, "crear") == 0) {
            crear_archivo(cmd.archivo, cmd.pkpos, cmd.pklen, cmd.lrecl, cmd.fkpos, cmd.fkarchivo);
        } else if (strcmp(cmd.comando, "insertar") == 0) {
            insertar_registro(cmd.archivo, cmd.registro);
        } else if (strcmp(cmd.comando, "borrar") == 0) {
            borrar_registro(cmd.archivo, cmd.pk);
        } else if (strcmp(cmd.comando, "leer") == 0) {
            leer_registro(cmd.archivo, cmd.pk);
        } else if (strcmp(cmd.comando, "mostrar") == 0) {
            mostrar_archivo(cmd.archivo);
        } else if (strcmp(cmd.comando, "cambiar") == 0) {
            cambiar_registro(cmd.archivo, cmd.pk, cmd.registro);
        }

        liberarParseo(&cmd);
    }
}
