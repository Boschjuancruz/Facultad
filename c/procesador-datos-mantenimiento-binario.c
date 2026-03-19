#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Necesario para ftruncate y fileno en sistemas POSIX

/**
 * Estructura para almacenamiento persistente de estadísticas de cajeros.
 */
typedef struct {
    int id_cajero;
    int cantidad_movimientos;
} RegistroEstadistico;

int main() {
    FILE *archivo_txt, *archivo_bin;
    int id_actual, operacion;
    float monto, saldo_acumulado;
    int contador_movs;
    RegistroEstadistico stats;

    // 1. Fase de Procesamiento: Texto -> Binario
    archivo_txt = fopen("movimientos.txt", "r");
    archivo_bin = fopen("cajeros.dat", "wb");

    if (!archivo_txt || !archivo_bin) {
        perror("Error al inicializar archivos");
        return 1;
    }

    // Lectura inicial para el Corte de Control
    if (fscanf(archivo_txt, "%d %d %f", &id_actual, &operacion, &monto) == 3) {
        
        while (!feof(archivo_txt)) {
            int cajero_proceso = id_actual;
            saldo_acumulado = 0;
            contador_movs = 0;

            // Bucle de agrupación por Cajero
            while (!feof(archivo_txt) && id_actual == cajero_proceso) {
                if (operacion == 1) saldo_acumulado += monto;
                else saldo_acumulado -= monto;
                
                contador_movs++;
                fscanf(archivo_txt, "%d %d %f", &id_actual, &operacion, &monto);
            }

            // Guardado en estructura binaria
            stats.id_cajero = cajero_proceso;
            stats.cantidad_movimientos = contador_movs;
            fwrite(&stats, sizeof(RegistroEstadistico), 1, archivo_bin);

            printf("Cajero %d procesado. Saldo Final: %.2f | Movs: %d\n", 
                    cajero_proceso, saldo_acumulado, contador_movs);
        }
    }

    fclose(archivo_txt);
    fclose(archivo_bin);

    // 2. Fase de Mantenimiento: Eliminación del último registro
    // Abrimos con "rb+" para lectura y escritura sin borrar el contenido
    archivo_bin = fopen("cajeros.dat", "rb+");
    if (archivo_bin) {
        // Posicionamos el puntero al inicio del último registro
        fseek(archivo_bin, (long)-sizeof(RegistroEstadistico), SEEK_END);
        
        // ftell nos da la posición actual (el nuevo final deseado)
        off_t nuevo_final = ftell(archivo_bin);
        
        // Truncamos el archivo físicamente en el disco
        if (ftruncate(fileno(archivo_bin), nuevo_final) == 0) {
            printf("\nMantenimiento: Se elimino el ultimo registro del archivo binario.\n");
        } else {
            perror("Error al truncar el archivo");
        }
        fclose(archivo_bin);
    }

    return 0;
}