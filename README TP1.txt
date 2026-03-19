# Proyecto TP1 – Manejo de Archivos con Índices y Metadatos en C
 Alumnos:
 Bosch, Juan Cruz - Leg. 201104
 Palmieri, Julieta Araceli - Leg. 20110

## Descripción

Este proyecto implementa un sistema de manejo de archivos planos en C que permite:

* Crear archivos con metadatos (PK y FK).
* Insertar, borrar, leer y modificar registros.
* Mantener un índice para búsquedas rápidas.
* Soportar borrado lógico de registros.
* Validar claves foráneas (FK) y evitar duplicados en PK.

El sistema está orientado a trabajar con archivos de registros de longitud fija (LRECL) y utiliza un **índice en memoria y persistente** para mejorar la eficiencia de las búsquedas.

Además, el programa cuenta con un **modo shell interactivo**, donde el usuario puede escribir comandos tipo terminal para ejecutar las operaciones sin necesidad de menú.

---

## Estructura de Archivos

### Archivos fuente principales

* `TP1.c` – Funciones de comandos: crear, insertar, borrar, leer, mostrar y cambiar registros.
* `archivos.h / archivos.c` – Funciones auxiliares para manipulación de archivos y registros.
* `indice.h / indice.c` – Funciones para construir, guardar y cargar índices.
* `parser.h / parser.c` – Funciones para parsear la entrada del shell y convertirla en comandos ejecutables.
* `main.c` – Interfaz de usuario tipo shell interactivo.

### Archivos de datos

* `archivo.dat` – Archivo de registros plano.
* `archivo.idx` – Archivo de índice, que almacena PK y posición en bytes (`pos`).
* `archivo.meta` – Archivo de metadatos, que almacena posiciones y longitudes de PK y FK, LRECL y nombre de archivo FK si existe.

---

## Funcionalidades

### Modo Shell Interactivo

* Inicia con el prompt:

```
tp1> 
```

* Los comandos que se pueden ejecutar son:

| Comando                                                       | Descripción                                                                                       |
| ------------------------------------------------------------- | ------------------------------------------------------------------------------------------------- |
| `crear <archivo> <pkpos> <pklen> <lrecl> <fkpos> [fkarchivo]` | Crea un archivo con metadatos e índice. Si no hay FK, poner `-1` en `fkpos` y omitir `fkarchivo`. |
| `insertar <archivo> <registro>`                               | Inserta un registro.                                                                              |
| `borrar <archivo> <pk>`                                       | Borra un registro lógicamente.                                                                    |
| `leer <archivo> <pk>`                                         | Lee un registro usando el índice.                                                                 |
| `mostrar <archivo>`                                           | Muestra todo el archivo.                                                                          |
| `cambiar <archivo> <pk> <registro>`                           | Cambia un registro existente.                                                                     |
| `salir`                                                       | Finaliza el shell.                                                                                |

* El shell tokeniza automáticamente la línea de entrada y ejecuta los comandos correspondientes.
* Se validan posiciones, longitudes, duplicados de PK y existencia de FK.

---

### Crear archivo

* Crea un archivo vacío y genera sus metadatos (`.meta`) y un índice inicial (`.idx`).
* Permite definir posición y longitud de PK y FK, y longitud del registro (LRECL).
* Si no hay FK, se debe ingresar `-1` como `fkpos`.
* Valida consistencia de los valores ingresados y existencia del archivo FK si corresponde.

### Insertar registro

* Inserta un registro respetando la longitud fija (LRECL).
* Verifica duplicados de PK mediante el índice en memoria.
* Verifica existencia de FK si aplica.
* Reconstruye y guarda el índice actualizando posiciones.

### Borrar registro

* Realiza borrado lógico (marcando con `*`).
* Actualiza índice eliminando los registros borrados.

### Leer registro

* Usa directamente el índice para localizar la posición exacta en bytes y leer el registro.
* Indica si el registro está borrado (`b`) o activo.

### Mostrar archivo

* Muestra todos los registros con su estado.
* Reconstruye el índice tras la lectura.

### Cambiar registro

* Permite modificar un registro existente validando:

  * Que la nueva PK no esté duplicada ni referenciada.
  * Que la FK sea válida si aplica.
* Reconstruye índice tras la modificación.

---

## Estructuras principales

```c
#define MAX_REG 256
#define BORRADO '*'

typedef struct {
    int pk_pos;
    int pk_len;
    int lrecl;
    int fk_pos;
    int fk_len;
    int tiene_fk;
    char fk_archivo[100];
} Metadatos;

typedef struct {
    char pk[MAX_REG];
    long pos; // posición en bytes en el archivo
} EntradaIndice;
```

---

## Índice

* Se construye ignorando registros borrados.
* Cada entrada del índice guarda la PK y la posición en bytes del registro.
* Guardado en `.idx` y cargado en memoria para búsquedas rápidas.

---

## Compilación y ejecución

```bash
gcc main.c TP1.c archivos.c indice.c parser.c -o tp1
./tp1
```

* Inicia el shell interactivo con prompt `tp1>`.
* Escribe los comandos directamente siguiendo la sintaxis del shell.
* Finaliza escribiendo `salir`.

---

## Notas importantes

1. **Borrado lógico:** Registros borrados se marcan con `*` y no se incluyen en el índice.
2. **Búsqueda por índice:** Todas las lecturas se realizan usando el índice para eficiencia.
3. **Integridad referencial:** Las claves foráneas son opcionales pero se validan si existen.
4. **Registros de longitud fija:** Todas las operaciones respetan LRECL.
