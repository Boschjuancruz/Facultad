#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_ALUMNOS 4

typedef struct{
    int legajo;
    float nota_prom;
    int anio;
    }  t_alumno  ;
/* Tabla de alumnos */
typedef t_alumno t_tabla_alumnos[MAX_ALUMNOS];
/* Prototipos */
void cargar_alumno(t_alumno* alumno);
void mostrar_alumno(t_alumno alumno);
int main()   {
    int i;
    t_alumno alumno;
    t_tabla_alumnos tabla_alumnos;
    /* Carga de la tabla */
    for (i=0; i<MAX_ALUMNOS; i++) {
        cargar_alumno( &tabla_alumnos[i] );
    }
    /* Visualización de la tabla */
    for (i=0; i<MAX_ALUMNOS; i++) {
        mostrar_alumno( tabla_alumnos[i] );
    }
    return 0;
}
/* Nótese el pasaje de la referencia */
void cargar_alumno(t_alumno* alumno) {
    printf("Ingrese datos del alumno\n\n");
    printf("Legajo: ");
    scanf("%d", &(*alumno).legajo);
    printf("Promedio: ");
    scanf("%f", &(*alumno).nota_prom);
    printf("Año Ingreso: ");
    scanf("%d", &(*alumno).anio);
   
}
/* Nótese el pasaje por copia del registro */
void mostrar_alumno(t_alumno alumno){
    printf("Datos del alumno\n\n");
    printf("Legajo: %d\n", alumno.legajo);
    printf("Promedio: %.02f\n", alumno.nota_prom);
    printf("Año Ingreso: %d\n", alumno.anio);
    system("Pause");
}




char mostrar_alumno(t_alumno alum[]){
    for (int i=0; i<MAX_ALUMNOS; i++) {
        printf("Datos del alumno\n\n");
        printf("Legajo: %d\n", alum[i].legajo);
        printf("Promedio: %.02f\n", alum[i].nota_prom);
        printf("Año Ingreso: %d\n", alum[i].anio);
        system("Pause");
    }
}


/*musicos:
#define MAX 2
typedef struct{
    char  titulo[8];
    int anio;
    int canti;
    }  t_disco ;
    
t_disco disco[MAX];
typedef struct{
    char  nombre[15];
    int edad;
    char genero[10];
    t_disco discos[MAX];
    }  t_musicos ;
t_musicos musicos;*/