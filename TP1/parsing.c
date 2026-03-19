/*
 * fprueba.c
 * 
 * Copyright 2025 osboxes <osboxes@osboxes>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
/*
REQUERIMIENTO:

-todo el contenido de los archivos es ASCII, no binario
-puede crear varios ejecutables, uno para cada comando o bien
 un unico ejecutable tipo shell en donde el usuario tipea los
 comandos
-<nro de registro> de base 0..N-1
-puede hacer un borrado logico de registros (o no, depende del diseÃ±o)
-registros de longitud fija de contenido no binario
-en caso de error, emitir errores por consola y no realizar la operacion solicitada
-si todo ok, no emitir nigun mensaje
-una sola FK por archivo como maximo
-una sola PK por archivo como maximo, obviamente
-no implementar, no soporta, claves candidatas
-PK,FK alfanumerica, no coinciden con la ubicacion fisica del registro
-la FK obviamente tiene el mismo largo que la PK del archivo que relaciona
-pueden mantener datos en memoria que luego se bajan a disco y viceversa
-todas las claves y datos son persistentes, es decir, si ingreso 1000 registros
 en N archivos, salgo de la aplicacion, apago la pc, la prendo y vuelvo a entrar,
 los datos estan disponibles nuevamente
-nombre de archivo sin espacios intermedios, ni comillas dobles, ni simples y 
 residen en la carpeta actual (no indicar ruta o path del archivo, solo su nombre)
-<pos> es de base 0
-lo que esta entre corchetes [..] (en los comandos), significa que es opcional, el
 resto es obligatorio

COMANDOS (minimos, Ud puede agregar mas):

$ crear <archivo> PK="<pos>,<largo>" LRECL="<largo registro>" [FK="<pos>,<archivo>"]
 -validar que <pos> y <largo> sean valores razonables dentro del registro
 -validar que el archivo de la FK exista previamente (en caso de indicar FK)
 -crea <archivo> desde cero, si existe previamente, lo destruye
$ insertar <archivo> REG="...."
 -controlar que no se duplique PK
 -controlar que no supere LRECL
 -controlar que que el valor de FK del registro exista como valor de PK
  en el archivo que referencia la FK
$ borrar <archivo> PK="<valor pk>"
 -no permitir borrar registro si hay FK que apuntan a este registro
 -hacer borrado logico, en lo posible, dependera del diseÃ±o que Ud proponga
 -controlar que exista PK
 -controlar que el registro ya no este borrado previamente
$ leer <archivo> PK="<valor pk>"
 -lee registro a traves del valor de su PK y muestra por pantalla:
  #reg # registro
   1    b    "...."
  -igual que comando mostrar pero de 1 solo registro
   (ejemplo registo 1 borrado)
  -si la PK no existe, mostrar PK no existe!
  -obviamente, muestra registros borrados logicos (si existieran) y
   no borrados
$ mostrar <archivo>
 -muestra:
   #reg #  registro
   0       ....
   1    b  ....
   2       ....
   ...
 -una linea de consola por registro, #reg indica nro de registro,
  # indica un "b" cuando el registro este borrado (borrado logico,
  si es que implemento borrado logico)
$ cambiar <archivo> PK="<valor pk anterior>" REG="..."
 -dar de baja PK anterior, FK anterior
 -no puede dar de baja PK anterior si esta es referenciada por FK's
 -dar de alta PK nueva, FK nueva
 -controlar que exista previamente la nueva PK
 -controlar que no se duplique PK
 -controlar que exista FK

$ ./fprueba crear pepe.dat PK="0,2" LRECL="10"
argv[0]=./fprueba
argv[1]=crear
argv[2]=pepe.dat
argv[3]=PK=0,2
argv[4]=LRECL=10
$ ./fprueba crear fpepe.dat PK="0,2" LRECL="10" FK="2,pepe.dat"
argv[0]=./fprueba
argv[1]=crear
argv[2]=fpepe.dat
argv[3]=PK=0,2
argv[4]=LRECL=10
argv[5]=FK=2,pepe.dat
$ ./fprueba insertar pepe.dat REG="01ana 7890"
argv[0]=./fprueba
argv[1]=insertar
argv[2]=pepe.dat
argv[3]=REG=01ana 7890
$ ./fprueba insertar fpepe.dat REG="0101fk ana"
argv[0]=./fprueba
argv[1]=insertar
argv[2]=fpepe.dat
argv[3]=REG=0101fk ana
$ ./fprueba borrar fpepe.dat PK="01"
argv[0]=./fprueba
argv[1]=borrar
argv[2]=fpepe.dat
argv[3]=PK=01
$ ./fprueba leer pepe.dat PK="01"
argv[0]=./fprueba
argv[1]=leer
argv[2]=pepe.dat
argv[3]=PK=01
$ ./fprueba mostrar pepe.dat 
argv[0]=./fprueba
argv[1]=mostrar
argv[2]=pepe.dat
$ ./fprueba cambiar pepe.dat PK="01" REG="01pepe 890"
argv[0]=./fprueba
argv[1]=cambiar
argv[2]=pepe.dat
argv[3]=PK=01
argv[4]=REG=01pepe 890

*/
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//estructura para guardar resultado de parsing
typedef struct rpar {
	char *comando;
	char *archivo;
	int pkpos;
	int pklen;
	int lrecl;
	int fkpos;
	int fklen; // igual al tamanio de pk de archivo fkarchivo
	char *fkarchivo;
	char *registro;
	char *pk;
} rpar;

const char *errores[11] = {
	NULL,
	"comando desconocido",
	"numero de argumentos de comando incorrecto", // 2
	"token PK= no encontrado",
	"token coma de separacion no encontrado",
	"no hay espacio suficiente para almacenar numero", // 5 
	"se encontraron caracteres no numericos en donde deberia haber numeros",
	"token LRECL= no encontrado",
	"token FK= no encontrado", // 8
	"token REG= no encontrado",
	"PK con largo incorrecto"
};
enum errorParsing {
	ERR_NO_HAY,
	ERR_CMD_NO_ENCONTRADO,
	ERR_MAL_ARG,
	ERR_TOKEN_PK_NO_ENCONTRADO,
	ERR_TOKEN_COMA_NO_ENCONTRADO,
	ERR_SIN_ESPACIO,
	ERR_CAR_NO_NUMERICO,
	ERR_LRECL_NO_ENCONTRADO,
	ERR_FK_NO_ENCONTRADO,
	ERR_REG_NO_ENCONTRADO,
	ERR_PK_MAL_LARGO
};
#define ERR_TEXTO(indice) errores[indice]
    
// libero memoria de p
void libero(rpar *p);
// return ERR_NO_HAY si todo ok, return > 0  si error
enum errorParsing parsing(rpar *p,int narg,char **arg);
// return ERR_NO_HAY si todo ok, return > 0  si error
enum errorParsing parsing_crear(rpar *p,int narg,char **arg);
enum errorParsing parsing_insertar(rpar *p,int narg,char **arg);
enum errorParsing parsing_borrar(rpar *p,int narg,char **arg);
enum errorParsing parsing_leer(rpar *p,int narg,char **arg);
enum errorParsing parsing_mostrar(rpar *p,int narg,char **arg);
enum errorParsing parsing_cambiar(rpar *p,int narg,char **arg);

// parsing de num1,num2 de maximo largo para cada numero desde src
// return ERR_NO_HAY si todo ok, return > 0 si error
enum errorParsing parsing_num1num2(char *num1,char *num2,int largo,char *src);
// parsing de num1,alfa2 de maximo largonum1,largoalfa para cada numero desde src
// return ERR_NO_HAY si todo ok, return > 0 si error
enum errorParsing parsing_num1alfa2(char *num1,char *alfa2,int largo1,int largo2,char *src);
void muestro_par(rpar *p);

/*int main(int argc, char **argv) {
	int i;
	for(i=0;i<argc;i++) {
		printf("argv[%d]=%s\n",i,argv[i]);
	}
	rpar c;
	int rc = parsing(&c,argc,argv);
	if ( rc == ERR_NO_HAY ) {
		printf("parsing exitoso!\n");
		muestro_par(&c);
	} else {
		printf("parsing con error(%d): [%s]!\n",rc,ERR_TEXTO(rc));
	}
	libero(&c);
	return rc;
}*/

void libero(rpar *p) {
	if (p->comando) free(p->comando);
	if (p->archivo) free(p->archivo);
	if (p->fkarchivo) free(p->fkarchivo);
	if (p->registro) free(p->registro);
	memset(p,0,sizeof(rpar));
}

enum errorParsing parsing(rpar *p,int narg,char **arg) {
	char *comandos[6] = { "crear", "insertar", "borrar", "leer", "mostrar", "cambiar" };
	enum errorParsing (*fcomando[6])(rpar *,int,char **) = { parsing_crear, parsing_insertar, parsing_borrar, parsing_leer, parsing_mostrar, parsing_cambiar };
	memset(p,0,sizeof(rpar));
	int i;
	for(i=0;i<6;i++) {
		if ( strcmp(arg[1],comandos[i]) == 0 ) break;
	}
	if ( i >= 6 ) return ERR_CMD_NO_ENCONTRADO;
	p->comando = strdup(arg[1]);
	return fcomando[i](p,narg,arg);
}

/*ejemplos:
./fprueba crear pepe.dat PK="0,2" LRECL="10"
argv[0]=./fprueba
argv[1]=crear
argv[2]=pepe.dat
argv[3]=PK=0,2
argv[4]=LRECL=10
$ ./fprueba crear fpepe.dat PK="0,2" LRECL="10" FK="2,pepe.dat"
argv[0]=./fprueba
argv[1]=crear
argv[2]=fpepe.dat
argv[3]=PK=0,2
argv[4]=LRECL=10
argv[5]=FK=2,pepe.dat
*/
enum errorParsing parsing_crear(rpar *p,int narg,char **arg) {
	if ( !(narg == 5 || narg == 6) ) return ERR_MAL_ARG;
	p->archivo = strdup(arg[2]);
	char tpk[40],num1[20],num2[20],alfa[50],tlrecl[20],tfk[50];
	char *pstr = strstr(arg[3],"PK=");
	if ( pstr == NULL ) return ERR_TOKEN_PK_NO_ENCONTRADO;
	strncpy(tpk,pstr+3,40);
	int rc = parsing_num1num2(num1,num2,20,tpk);
	if ( rc != ERR_NO_HAY ) return rc;
	p->pkpos = atoi(num1);
	p->pklen = atoi(num2);
	pstr = strstr(arg[4],"LRECL=");
	if ( pstr == NULL ) return ERR_LRECL_NO_ENCONTRADO;
	strncpy(tlrecl,pstr+6,20);
	p->lrecl = atoi(tlrecl);
	if ( narg == 6 ) { // indico FK
		pstr = strstr(arg[5],"FK=");
		if ( pstr == NULL ) return ERR_FK_NO_ENCONTRADO;
		strncpy(tfk,pstr+3,50);
		rc = parsing_num1alfa2(num1,alfa,20,50,tfk);
		if ( rc != ERR_NO_HAY ) return rc;
		p->fkpos = atoi(num1);
		p->fkarchivo = strdup(alfa);
	}
	if ( p->pklen > p->lrecl ) return ERR_PK_MAL_LARGO;
	if ( (p->pkpos+p->pklen) > (p->lrecl -1) ) return ERR_PK_MAL_LARGO; 
	return ERR_NO_HAY;
}
/*ejemplos:
./fprueba insertar pepe.dat REG="01ana 7890"
argv[0]=./fprueba
argv[1]=insertar
argv[2]=pepe.dat
argv[3]=REG=01ana 7890
*/
enum errorParsing parsing_insertar(rpar *p,int narg,char **arg) {
	if ( narg != 4 ) return ERR_MAL_ARG;
	p->archivo = strdup(arg[2]);
	char *pstr = strstr(arg[3],"REG=");
	if ( pstr == NULL ) return ERR_REG_NO_ENCONTRADO;
	p->registro = strdup(pstr+4);
	return ERR_NO_HAY;
}
/*ejemplos:
$ ./fprueba borrar fpepe.dat PK="01"
argv[0]=./fprueba
argv[1]=borrar
argv[2]=fpepe.dat
argv[3]=PK=01
*/
enum errorParsing parsing_borrar(rpar *p,int narg,char **arg) {
	if ( narg != 4 ) return ERR_MAL_ARG;
	p->archivo = strdup(arg[2]);
	char *pstr = strstr(arg[3],"PK=");
	if ( pstr == NULL ) return ERR_TOKEN_PK_NO_ENCONTRADO;
	p->pk = strdup(pstr+3);
	return ERR_NO_HAY;
}
/*ejemplos:
$ ./fprueba leer pepe.dat PK="01"
argv[0]=./fprueba
argv[1]=leer
argv[2]=pepe.dat
argv[3]=PK=01
*/
enum errorParsing parsing_leer(rpar *p,int narg,char **arg) {
	return parsing_borrar(p,narg,arg);
}
/*ejemplos:
$ ./fprueba mostrar pepe.dat 
argv[0]=./fprueba
argv[1]=mostrar
argv[2]=pepe.dat
*/
enum errorParsing parsing_mostrar(rpar *p,int narg,char **arg) {
	if ( narg != 3 ) return ERR_MAL_ARG;
	p->archivo = strdup(arg[2]);
	return ERR_NO_HAY;	
}
/*ejemplos:
$ ./fprueba cambiar pepe.dat PK="01" REG="01pepe 890"
argv[0]=./fprueba
argv[1]=cambiar
argv[2]=pepe.dat
argv[3]=PK=01
argv[4]=REG=01pepe 890
*/
enum errorParsing parsing_cambiar(rpar *p,int narg,char **arg) {
	if ( narg != 5 ) return ERR_MAL_ARG;
	p->archivo = strdup(arg[2]);
	char *pstr = strstr(arg[3],"PK=");
	if ( pstr == NULL ) return ERR_TOKEN_PK_NO_ENCONTRADO;
	p->pk = strdup(pstr+3);
	pstr = strstr(arg[4],"REG=");
	if ( pstr == NULL ) return ERR_REG_NO_ENCONTRADO;
	p->registro = strdup(pstr+4);	
	return ERR_NO_HAY;
}

// parsing de num1,num2 de maximo largo para cada numero desde src
// return 0 si todo ok, return > 0 si error
enum errorParsing parsing_num1num2(char *num1,char *num2,int largo,char *src) {
	if ( strstr(src,",") == NULL ) return ERR_TOKEN_COMA_NO_ENCONTRADO;
	int ii=0;
	// num1
	while(*src && *src != ',' && ii < largo ) {
		if (!(*src >= '0' && *src <= '9')) return ERR_CAR_NO_NUMERICO;
		num1[ii]=*src;ii++;src++;
	}
	if ( *src != ',' ) return ERR_TOKEN_COMA_NO_ENCONTRADO;
	if ( ii >= largo ) return ERR_SIN_ESPACIO;
	num1[ii]='\0';
	ii=0;src++;
	// num2
	while(*src && ii < largo ) {
		if (!(*src >= '0' && *src <= '9')) return ERR_CAR_NO_NUMERICO;
		num2[ii]=*src;ii++;src++;
	}
	if ( ii >= largo ) return ERR_SIN_ESPACIO;
	num2[ii]='\0';
	return ERR_NO_HAY;
}

// parsing de num1,alfa2 de maximo largonum1,largoalfa para cada numero desde src
// return 0 si todo ok, return > 0 si error
enum errorParsing parsing_num1alfa2(char *num1,char *alfa2,int largo1,int largo2,char *src) {
	if ( strstr(src,",") == NULL ) return ERR_TOKEN_COMA_NO_ENCONTRADO;
	int ii=0;
	// num1
	while(*src && *src != ',' && ii < largo1 ) {
		if (!(*src >= '0' && *src <= '9')) return ERR_CAR_NO_NUMERICO;
		num1[ii]=*src;ii++;src++;
	}
	if ( *src != ',' ) return ERR_TOKEN_COMA_NO_ENCONTRADO;
	if ( ii >= largo1 ) return ERR_SIN_ESPACIO;
	num1[ii]='\0';
	ii=0;src++;
	// alfa2
	while(*src && ii < largo2 ) {
		alfa2[ii]=*src;ii++;src++;
	}
	if ( ii >= largo2 ) return ERR_SIN_ESPACIO;
	alfa2[ii]='\0';
	return ERR_NO_HAY;
}

void muestro_par(rpar *p) {
	printf("Comando: [%s]\nArchivo: [%s]\nPosicion PK: [%d]\nLargo PK: [%d]\n",
		p->comando,p->archivo,p->pkpos,p->pklen);
	printf("Largo Registro: [%d]\nPosicion FK: [%d]\nLargo FK: [%d]\n",
		p->lrecl,p->fkpos,p->fklen);
	printf("FK apunta a: [%s]\nRegistro: [%s]\n",p->fkarchivo,p->registro);
	printf("Valor PK: [%s]\n",p->pk);
}
