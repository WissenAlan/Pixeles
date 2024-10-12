#ifndef FUNCIONES_ESTUDIANTE_H_INCLUDED
#define FUNCIONES_ESTUDIANTE_H_INCLUDED
#include "estructuras.h"
#include "constantes.h"
#define NO_ES_24BITS 24
#define CONCATENADO 1
#define INSTRUCCION_INVALIDA 0


int solucion(int argc, char* argv[]);
int pintarPixeles(FILE*, t_metadata*,char* argv[],int);
int generador(t_metadata*, char*[], int);
int lecturaCabecera(t_metadata*,char*);
FILE* abrirArchivo(char*);
FILE* crearArchivo(char*);
void cargarMetadata(FILE*bmp,FILE*fichero,t_metadata*meta);
void rotar(t_pixel *pixel, FILE*fichero, FILE*bmp, t_metadata*meta,int, int);
void cambiarTonalidad(t_pixel *pixel,FILE*fichero, FILE*bmp,t_metadata*meta,int tipo);
void cambiarContraste(t_pixel *pixel,FILE*fichero, FILE*bmp,t_metadata*meta,int tipo);
void escalaGrises(t_pixel *pixel,FILE*fichero, FILE*bmp,t_metadata*meta);
void negativo(t_pixel *pixel,FILE*fichero, FILE*bmp,t_metadata*meta);
void recortar(t_pixel *pixel, FILE*fichero, FILE*bmp, t_metadata*meta);
void comodin(t_pixel *pixel, FILE*fichero, FILE*bmp, t_metadata*meta);
int concatenar(t_pixel *pixel,FILE* fichero,FILE *bmp,t_metadata *meta,char*);
void rellenar_imagen(FILE *, t_metadata*, unsigned int anchoMayor, unsigned char R, unsigned char G, unsigned char B);
void achicar(t_pixel *pixel,FILE*fichero, FILE*bmp,t_metadata*meta);
void monocromo(t_pixel *pixel,FILE*fichero, FILE*bmp,t_metadata*meta);
void dumpExadecimal(FILE*bmp, t_metadata*meta);
void DestruirMat(t_pixel** m,unsigned int filas);
t_pixel ** CrearMat(size_t TamElem,unsigned int filas,unsigned int columnas);
#endif // FUNCIONES_ESTUDIANTE_H_INCLUDED
