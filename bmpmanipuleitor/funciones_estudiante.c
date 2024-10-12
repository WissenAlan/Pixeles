#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "estructuras.h"
#include "constantes.h"
#include "funciones_estudiante.h"
#include <math.h>
#include <stdbool.h>

int solucion(int argc, char* argv[])
{
    /*
        Aquí deben hacer el código que solucione lo solicitado.
        Todas las funciones utilizadas deben estar declaradas en este archivo, y en su respectivo .h
    */
    t_metadata meta;
    if(lecturaCabecera(&meta,argv[argc-1]) != 10) //si no le llega ARCHIVO_NO_ENCONTRADO al leer la cabecera procede al generador
        generador(&meta, argv,argc);
    return TODO_OK;
}

int lecturaCabecera(t_metadata*meta, char*nomArch)
{
    FILE*pf = abrirArchivo(nomArch); //Abre el archivo unlam.bmp
    if(!pf){
        printf("Imagen bmp no encontrada\n");
        return ARCHIVO_NO_ENCONTRADO;
    }
  ////Empieza a leer el encabezado del archivo original////
    fseek(pf, 28, SEEK_SET);
    fread(&meta->profundidad, sizeof(unsigned short), 1, pf);

    if(meta->profundidad != 24)
    {
        printf("No es de 24 bits %d", meta->profundidad);
        fclose(pf);
        return NO_ES_24BITS;
    }

    fseek(pf, 2, SEEK_SET);
    fread(&meta->tamArchivo, sizeof(unsigned int), 1, pf);

    fseek(pf, 10, SEEK_SET);
    fread(&meta->comienzoImagen, sizeof(unsigned int), 1, pf);

    fseek(pf, 14, SEEK_SET);
    fread(&meta->tamEncabezado, sizeof(unsigned int), 1, pf);

    fseek(pf, 18, SEEK_SET);
    fread(&meta->ancho, sizeof(unsigned int), 1, pf);

    fseek(pf, 22, SEEK_SET);
    fread(&meta->alto, sizeof(unsigned int), 1, pf);

    fclose(pf);
    return TODO_OK;
}

int generador(t_metadata*meta, char *argv[], int argc)
{
    FILE*bmp;
    bmp = abrirArchivo(argv[argc-1]);
    if(!bmp){
        return ARCHIVO_NO_ENCONTRADO;
    }
    int i = 1; //i=1 asi sealtea el nombre del ejecutable
    while(i < argc-1)//-1 asi no lee el bmp del final
    {
        i += pintarPixeles(bmp,meta,argv,i); //i+= por funcion concatenar
        i++;
    }

    fclose(bmp);
    return TODO_OK;
}


int pintarPixeles( FILE*bmp, t_metadata*meta,char* argv[],int pos)
{
    char instrucciones[14][21]={"--negativo", //estas son todas las instrucciones que puede mandar el usuario
                            "--escala-de-grises",
                            "--aumentar-contraste",
                            "--reducir-contraste",
                            "--tonalidad-azul",
                            "--tonalidad-verde",
                            "--tonalidad-roja",
                            "--rotar-izquierda",
                            "--rotar-derecha",
                            "--recortar",
                            "--comodin",
                            "--concatenar",
                            "--achicar",
                            "--monocromo"}; //no logramos hacerla funcionar.
    int i = 0;
    bool flag = false;

    while(i < sizeof(instrucciones)/sizeof(instrucciones[i]) && !flag){//recorre hasta encontrar una instruccion o hasta que se pase del vector instrucciones
        if(strcmp(argv[pos],instrucciones[i])==0)
            flag = true; //si se encontro la instruccion sale del while
        else
            i++;//si todavia no la encontro suma i
    }
    if(!flag){//si no se encontro la instruccion retorna 0 a la funcion generador
        printf("Instruccion invalida\n");
        return INSTRUCCION_INVALIDA;
    }

    FILE*fichero;
    t_pixel pixel;
    fichero = crearArchivo(argv[pos]);
    if(!fichero)
        return NO_SE_PUEDE_CREAR_ARCHIVO;

    cargarMetadata(bmp,fichero,meta);

    switch(i){
        case 0: negativo(&pixel,fichero,bmp,meta);
            break;
        case 1: escalaGrises(&pixel,fichero,bmp,meta);
            break;
        case 2: cambiarContraste(&pixel,fichero,bmp,meta,10);
            break;
        case 3: cambiarContraste(&pixel,fichero,bmp,meta,-10);
            break;
        case 4:
        case 5:
        case 6: cambiarTonalidad(&pixel,fichero,bmp,meta,i);//se manda i por parametro y luego se le resta 4 para ubicar bien el vector.
            break;
        case 7: rotar(&pixel,fichero,bmp,meta,0,0);
            break;
        case 8: rotar(&pixel,fichero,bmp,meta,meta->ancho-1,meta->alto-1); //al rotarla para el otro lado se debe empezar a copiar los datos desde el final
            break;
        case 9: recortar(&pixel,fichero,bmp,meta);
            break;
        case 10: comodin(&pixel,fichero,bmp,meta); //creamos una funcion espejo
            break;
        case 11: concatenar(&pixel,fichero,bmp,meta,argv[pos+1]); //Le mando los argumentos a main para el segundo archivo
            return CONCATENADO; //retorna 1 de valor y aumenta el i en la funcion generador para que se saltee el bmp que se decidio concatenar.
            break;
        case 12: achicar(&pixel,fichero,bmp,meta);
            break;
        case 13: monocromo(&pixel,fichero,bmp,meta); //ejecuta pero no funciona correctamente
            break;
        default: printf("Error desconocido");
    }

    printf("Imagen generada con exito\n");
    fclose(fichero);
    return TODO_OK;
}

void cargarMetadata(FILE*bmp,FILE*fichero,t_metadata*meta)
{
    fseek(bmp,0,SEEK_SET);
    t_metadata meta1;
    for(int i = 0; i < meta->comienzoImagen; i++)
    {
        fread(&meta1, 1, 1, bmp); //encontramos que funcione copiandole primero la informacion 1 bit por 1 del archivo bmp a metadata
        fwrite(&meta1, 1, 1, fichero);//luego de meta1 a fichero hasta el comienzo de imagen
    }
}

void negativo(t_pixel *pixel,FILE*fichero, FILE*bmp,t_metadata*meta)
{
    fseek(bmp,meta->comienzoImagen,SEEK_SET);
    fseek(fichero,meta->comienzoImagen,SEEK_SET);
    while(!feof(bmp))
    {
        fread(pixel,1,3,bmp);
        for(int i=0;i<3;i++)
        {
            pixel->pixel[i] = 255 - pixel->pixel[i];
        }
        fwrite(pixel,1,3,fichero);
    }
}

void escalaGrises(t_pixel *pixel,FILE*fichero, FILE*bmp,t_metadata*meta)
{
    fseek(bmp,meta->comienzoImagen,SEEK_SET);
    fseek(fichero,meta->comienzoImagen,SEEK_SET);
    while(!feof(bmp))
    {
        int acum=0;
        fread(pixel,1,3,bmp);
        for(int i=0;i<sizeof(pixel->pixel);i++)
        {
            acum += pixel->pixel[i];
            if(i == sizeof(pixel->pixel)-1)
                for(int j = 0; j<sizeof(pixel->pixel); j++)
                    pixel->pixel[j] = acum / sizeof(pixel->pixel);
        }
        fwrite(pixel,1,3,fichero);
    }
}

void cambiarContraste(t_pixel *pixel,FILE*fichero, FILE*bmp,t_metadata*meta,int valor)
{
    fseek(bmp,meta->comienzoImagen,SEEK_SET);
    fseek(fichero,meta->comienzoImagen,SEEK_SET);
    while(!feof(bmp))
    {
        fread(pixel,1,3,bmp);
            for(int j = 0; j < sizeof(pixel->pixel); j++)
            {
                if(pixel->pixel[j]<100 && pixel->pixel[j]>abs(valor))
                    pixel->pixel[j] -= valor;
                else if(pixel->pixel[j]>155 && pixel->pixel[j]<255-abs(valor))
                    pixel->pixel[j] += valor;
            }
        fwrite(pixel,1,3,fichero);
    }
}

void cambiarTonalidad(t_pixel *pixel,FILE*fichero, FILE*bmp,t_metadata*meta,int tipo)
{
    fseek(bmp,meta->comienzoImagen,SEEK_SET);
    fseek(fichero,meta->comienzoImagen,SEEK_SET);
    tipo -=4;
    while(!feof(bmp))
    {
        fread(pixel,1,3,bmp);
        for(int i=0;i<3;i++)
        {
            if(pixel->pixel[tipo]<205 && tipo==i) pixel->pixel[tipo]+= 50;
            else if(pixel->pixel[i]>10 && i!=tipo){
                pixel->pixel[i]-= 10;
            }
        }
        fwrite(pixel,1,3,fichero);
    }
}

void rotar(t_pixel *pixel, FILE*fichero, FILE*bmp, t_metadata*meta,int fila, int colum)
{
    t_pixel **pixelMatrizOriginal=CrearMat(sizeof(t_pixel),meta->alto,meta->ancho);
    t_pixel **pixelMatrizCambio=CrearMat(sizeof(t_pixel),meta->ancho,meta->alto);

    fseek(fichero, 18, SEEK_SET);
    fwrite(&meta->alto, sizeof(unsigned int), 1, fichero);
    fseek(fichero, 22, SEEK_SET);
    fwrite(&meta->ancho, sizeof(unsigned int), 1, fichero);
    fseek(bmp, meta->comienzoImagen, SEEK_SET);
    fseek(fichero, meta->comienzoImagen, SEEK_SET);
    while(!feof(bmp))
    {
        for(int i=0; i<meta->alto; i++)
            for(int j=0; j<meta->ancho; j++)
            {
                fread(&pixelMatrizOriginal[i][j],1,3,bmp);
                for(int x=0;x<3;x++)
                    pixelMatrizCambio[j][meta->alto-i-1].pixel[x] = pixelMatrizOriginal[i][j].pixel[x];
            }
    }
        for(int i=0; i<meta->ancho; i++)
            for(int j=0; j<meta->alto; j++)
                fwrite(&pixelMatrizCambio[abs(fila-i)][abs(colum-j)],1,3,fichero);

        DestruirMat(pixelMatrizOriginal,meta->alto);
        DestruirMat(pixelMatrizCambio,meta->ancho);
}

void recortar(t_pixel *pixel, FILE*fichero, FILE*bmp, t_metadata*meta)
{
    unsigned int ancho=meta->ancho/2;
    unsigned int alto=meta->alto/2;
    fseek(fichero,18,SEEK_SET);
    fwrite(&ancho,sizeof(unsigned int),1,fichero);
    fseek(fichero,22,SEEK_SET);
    fwrite(&alto,sizeof(unsigned int),1,fichero);
    fseek(fichero,meta->comienzoImagen,SEEK_SET);
    fseek(bmp,meta->comienzoImagen,SEEK_SET);

    t_pixel**matPixel=CrearMat(sizeof(t_pixel),meta->alto,meta->ancho);

    while (!feof(bmp))
    {
        for(int i=0; i<meta->alto; i++)
        {
            for(int j=0; j<meta->ancho; j++)
            {
                fread(&matPixel[i][j],1,3,bmp);
            }
        }
    }
    for(int i=0; i<(meta->alto)/2; i++)
    {
        for(int j=0; j<(meta->ancho)/2; j++)
        {
            fwrite(&matPixel[i][j],1,3,fichero);
        }
    }
    DestruirMat(matPixel,meta->alto);
}

void comodin(t_pixel *pixel, FILE*fichero, FILE*bmp, t_metadata*meta)
{
    fseek(fichero,meta->comienzoImagen,SEEK_SET);
    fseek(bmp,meta->comienzoImagen,SEEK_SET);

    t_pixel**matPixel=CrearMat(sizeof(t_pixel),meta->alto,meta->ancho);

    t_pixel basura;
    while (!feof(bmp))
    {
        for(int i=0; i<meta->alto; i++)
        {
            for(int j=0; j<meta->ancho; j++)
            {
                if(j<meta->ancho/2){
                    fread(&matPixel[i][j],1,3,bmp);
                    matPixel[i][meta->ancho-1-j] = matPixel[i][j];
                }else{
                    fread(&basura,1,3,bmp);
                }
                fwrite(&matPixel[i][j],1,3,fichero);
            }
        }
    }
    DestruirMat(matPixel,meta->alto);
}

int concatenar(t_pixel *pixel,FILE* fichero,FILE *bmp,t_metadata *meta, char*nomArch)
{
    unsigned int AnchoMayor;
    t_metadata*meta2 = malloc(sizeof(meta) + 1);
    FILE* fichero2;
    fichero2 = abrirArchivo(nomArch);
    if(!fichero2){
        printf("Imagen bmp no encontrada\n");
        return ARCHIVO_NO_ENCONTRADO;
    }
    lecturaCabecera(meta2,nomArch); //Me baja a meta2 la cabecera del segundo archivo
    free(nomArch);
    unsigned int alturaNueva = meta->alto + meta2->alto;
    fseek(fichero, 22, SEEK_SET);
    fwrite(&alturaNueva, sizeof(unsigned int), 1, fichero); // Guardo la nueva altura
    if(meta->ancho > meta2->ancho)
        AnchoMayor = meta->ancho;
    else
        AnchoMayor = meta2->ancho;

    fseek(fichero, 18, SEEK_SET);
    fwrite(&AnchoMayor, sizeof(unsigned int), 1, fichero);

    fseek(bmp, meta->comienzoImagen, SEEK_SET);
    fseek(fichero, meta->comienzoImagen, SEEK_SET);
    fseek(fichero2, meta2->comienzoImagen, SEEK_SET);

    ////Me escribe la imagen 2 hasta su alto y ancho//

    for(int i = 0; i < meta2->alto; i++)
    {
        for(int j = 0; j < meta2->ancho;j++)
        {
            fread(pixel, 1, 3, fichero2);
            fwrite(pixel, 1, 3, fichero);
        }
        if(meta2->ancho<AnchoMayor)
            rellenar_imagen(fichero,meta2,AnchoMayor,255, 105, 180);
    }

     for (int i = 0; i < meta->alto; i++)
    {
        for (int j = 0; j < meta->ancho; j++)
        {
            fread(pixel, 1, 3, bmp);
            fwrite(pixel, 1, 3, fichero);
        }

        if(meta->ancho<AnchoMayor) //Si es la mas chica que la pinte//
            rellenar_imagen(fichero,meta,AnchoMayor,255, 105, 180);
    }
    free(meta2);
    fclose(fichero2);
    return TODO_OK;
}

void rellenar_imagen(FILE *fichero, t_metadata *meta, unsigned int anchoMayor, unsigned char R, unsigned char G, unsigned char B)
{
    unsigned char pixel[3] = {R, G, B};
    // Rellenar la imagen con el color deseado hasta completar el ancho mayor
    for (int j = meta->ancho; j < anchoMayor; j++)
        fwrite(pixel, 1, 3, fichero);
}

void achicar(t_pixel *pixel,FILE*fichero, FILE*bmp,t_metadata*meta)
{
    unsigned int ancho = meta->ancho/2;
    unsigned int alto = meta->alto/2;
    fseek(fichero,18,SEEK_SET);
    fwrite(&ancho,sizeof(unsigned int),1,fichero);
    fseek(fichero,22,SEEK_SET);
    fwrite(&alto,sizeof(unsigned int),1,fichero);
    fseek(fichero,meta->comienzoImagen,SEEK_SET);
    fseek(bmp,meta->comienzoImagen,SEEK_SET);

    t_pixel **matPixel=CrearMat(sizeof(t_pixel),meta->alto,meta->ancho);

    while(!feof(bmp))
        for(int i=0; i<meta->alto; i++)
            for(int j=0; j<meta->ancho; j++)
                fread(&matPixel[i][j],1,3,bmp);
    for(int i=0; i<meta->alto; i+=2){
        for(int j=0; j<meta->ancho; j+=2)
        {
            t_pixel pixelesAgrupados;
            int acum;
            for (int k = 0; k < sizeof(pixel->pixel); k++)
            {
                acum = 0;
                acum = (matPixel[i][j].pixel[k] + matPixel[i+1][j+1].pixel[k] + matPixel[i+1][j].pixel[k] + matPixel[i][j+1].pixel[k])/4;
                pixelesAgrupados.pixel[k] = acum;

            }
            pixelesAgrupados.profundidad = (matPixel[i][j].profundidad + matPixel[i+1][j+1].profundidad + matPixel[i+1][j].profundidad + matPixel[i][j+1].profundidad)/4;
            fwrite(&pixelesAgrupados,1,3,fichero);
        }
    }
    DestruirMat(matPixel,meta->alto);
}

void monocromo(t_pixel *pixel,FILE*fichero, FILE*bmp,t_metadata*meta)
{
    unsigned short profundidad = 1;

    fseek(fichero,28,SEEK_SET);
    fwrite(&profundidad,sizeof(unsigned short),1,fichero);

    fseek(fichero,meta->comienzoImagen,SEEK_SET);
    fseek(bmp,meta->comienzoImagen,SEEK_SET);

    t_pixel**matPixel=CrearMat(sizeof(t_pixel),meta->alto,meta->ancho);

    while(!feof(bmp)){
        for(int i=0; i<meta->alto; i++){
            for(int j=0; j<meta->ancho; j++){
                fread(&matPixel[i][j],1,3,bmp);
            }
        }
    }
    for (int i = 0; i < meta->alto/2; i++)
    {
        for (int j = 0; j < meta->ancho/2; j++)
        {
            int acum = 0;
            for (int k = 0; k < sizeof(pixel->pixel); k++)
            {
                acum += matPixel[i][j].pixel[k];
            }
            int color = acum/sizeof(pixel->pixel);
            if(color > 127){
                color = 255;
            }
            else{
                color = 0;
            }
            fwrite(&color,1,1,fichero);
        }
    }
    DestruirMat(matPixel,meta->alto);
}

t_pixel ** CrearMat(size_t TamElem,unsigned int filas,unsigned int columnas)
{
     t_pixel **mat = malloc(sizeof(t_pixel)*filas);
     if(!mat)return NULL;

     t_pixel **ult = mat + filas -1;
     t_pixel **i;

     for(i = mat; i<= ult;i++)
     {
         *i = malloc(TamElem*columnas);
         if(!i)
         {
             DestruirMat(mat, i-mat);
             return NULL;
         }
     }
     return mat;
}

void DestruirMat(t_pixel** m,unsigned int filas)
{
    t_pixel **ult= m + filas - 1;
    t_pixel **i;
    for(i=m;i<=ult;i++)
    {
        free(*i);
    }
    free(m);
}

FILE* abrirArchivo(char*nomArch)
{
    FILE*imagen = fopen(nomArch,"rb");
    if(!imagen)
    {
        printf("Error al abrir archivo\n");
        fclose(imagen);
        return NULL;
    }
    return imagen;
}
FILE* crearArchivo(char*operacion)
{
    char *nomArchivo = malloc(strlen("estudiante") + strlen(operacion) + strlen(".bmp") + 10);//le asigno una buena cantidad de memoria
    operacion = operacion + 2;//le sobreescribo los 2 primeros iones
    sprintf(nomArchivo, "estudiante_%s.bmp", operacion);
    FILE*pf = fopen(nomArchivo,"w+b");
    free(nomArchivo);//libero espacio en memoria
    if(!pf)
    {
        printf("Error al crear archivo");
        fclose(pf);
        return NULL;
    }
    return pf;
}
