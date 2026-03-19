#include <stdio.h>
#include <stdlib.h>

/* Estructura para representar un movimiento de cajero */
typedef struct {
    int cajero_id;
    int tipo_extraccion; // 1: Depósito/Entrada, Otros: Extracción
    int monto;
} RegistroCajero;

int main() {
    FILE *archivo;
    RegistroCajero reg;
    
    // Variables de control y estadísticas
    int cajero_actual;
    int saldo_cajero;
    int movimientos_cajero;
    
    int max_movimientos = -1;
    int cajero_con_mas_movs = 0;

    archivo = fopen("movimientos.txt", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    // Leemos el primer registro para iniciar el corte de control
    // fscanf devuelve la cantidad de elementos leídos correctamente
    if (fscanf(archivo, "%d %d %d", &reg.cajero_id, &reg.tipo_extraccion, &reg.monto) != 3) {
        printf("El archivo está vacío o tiene un formato incorrecto.\n");
        fclose(archivo);
        return 0;
    }

    printf("--- Reporte de Movimientos por Cajero ---\n");

    while (!feof(archivo)) {
        cajero_actual = reg.cajero_id;
        saldo_cajero = 0;
        movimientos_cajero = 0;

        // Bucle de Corte de Control
        while (!feof(archivo) && reg.cajero_id == cajero_actual) {
            movimientos_cajero++;
            
            if (reg.tipo_extraccion == 1) {
                saldo_cajero += reg.monto;
            } else {
                saldo_cajero -= reg.monto;
            }

            // Intentamos leer el siguiente registro
            fscanf(archivo, "%d %d %d", &reg.cajero_id, &reg.tipo_extraccion, &reg.monto);
        }

        // Al terminar un cajero, mostramos resultados y actualizamos máximos
        printf("Cajero ID: %d | Saldo Final: $%d | Movimientos: %d\n", 
                cajero_actual, saldo_cajero, movimientos_cajero);

        if (movimientos_cajero > max_movimientos) {
            max_movimientos = movimientos_cajero;
            cajero_con_mas_movs = cajero_actual;
        }
    }

    printf("-----------------------------------------\n");
    printf("Resumen estadistico:\n");
    printf("El cajero con mayor actividad fue el ID %d con %d movimientos.\n", 
            cajero_con_mas_movs, max_movimientos);

    fclose(archivo);
    return 0;
}
