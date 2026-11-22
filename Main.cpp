#include<iostream>
#include<cstdio>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"shaderClass.h"
#include"VAO.h"
#include"EBO.h"
#include"VBO.h"
#include<cmath>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <vector>
#include <queue>


// ZOOM

float zoom = 20.0f;

const float MIN_ZOOM = 2.0f;
const float MAX_ZOOM = 200.0f;


// DESPLAZAMIENTO

float offsetX = 0.0f;
float offsetY = 0.0f;

double lastMouseX = 0.0;
double lastMouseY = 0.0;

bool dragging = false;


// BRESENHAM

bool lineStartSelected = false;
bool lineEndSelected = false;

int lineX0 = 0;
int lineY0 = 0;

int lineX1 = 0;
int lineY1 = 0;

const int MAX_LINE_CELLS = 1024;

int lineCellsX[MAX_LINE_CELLS];
int lineCellsY[MAX_LINE_CELLS];

int lineCellCount = 0;

// CIRCULO DE BRESENHAM

bool circleCenterSelected = false;
bool circleRadiusSelected = false;

int circleCenterX = 0;
int circleCenterY = 0;

int circleRadius = 0;

// CELDAS DEL CIRCULO DE BRESENHAM

const int MAX_CIRCLE_CELLS = 1024;

int circleCellsX[MAX_CIRCLE_CELLS];
int circleCellsY[MAX_CIRCLE_CELLS];

int circleCellCount = 0;

// GUARDAR VARIOS CIRCULOS

const int MAX_CIRCLES = 100;

int allCircleCenterX[MAX_CIRCLES];
int allCircleCenterY[MAX_CIRCLES];
int allCircleRadius[MAX_CIRCLES];

int circleCount = 0;

// CELDAS BORRADAS

const int MAX_ERASED_CELLS = 256;

int erasedCellsX[MAX_ERASED_CELLS];
int erasedCellsY[MAX_ERASED_CELLS];

int erasedCellCount = 0;

// CELDAS DEL CUBITO DE RELLENO

struct CeldaRelleno
{
    int x;
    int y;
};

std::vector<CeldaRelleno> celdasRellenadas;

// IDENTIFICADOR DEL RELLENO AL QUE PERTENECE CADA CELDA
std::vector<int> idsRellenoPorCelda;

// DATOS DE CADA OPERACION DE RELLENO
struct Relleno
{
    int figuraPropietaria;
    float color[4];
};

std::vector<Relleno> rellenos;

// RELLENO SELECCIONADO PARA CAMBIAR SU COLOR
int rellenoSeleccionado = -1;

// REGISTRO DE FIGURAS

enum TipoFigura
{
    FIGURA_LINEA,
    FIGURA_CIRCULO,
    FIGURA_TRIANGULO,
    FIGURA_CUADRADO,
    FIGURA_RECTANGULO

};

const int MAX_FIGURAS = 200;

TipoFigura tiposFiguras[MAX_FIGURAS];

int figuraInicio[MAX_FIGURAS];
int figuraCantidad[MAX_FIGURAS];

bool figuraBorrada[MAX_FIGURAS];

int figuraCount = 0;

// HERRAMIENTAS DE MI PAINT

enum Herramienta
{
    HERRAMIENTA_NINGUNA,
    HERRAMIENTA_LINEA,
    HERRAMIENTA_CIRCULO,
    HERRAMIENTA_BORRADOR_CELDA,
    HERRAMIENTA_BORRADOR_FIGURA,
    HERRAMIENTA_TRIANGULO,
    HERRAMIENTA_CUADRADO,
    HERRAMIENTA_RECTANGULO,
    HERRAMIENTA_ROTACION,
    HERRAMIENTA_FORMA_LIBRE,
    HERRAMIENTA_TRASLACION,
    HERRAMIENTA_COLOR,
    HERRAMIENTA_RELLENO
};

Herramienta herramientaActual = HERRAMIENTA_NINGUNA;

// VARIABLES PARA FORMAS LIBRES

bool dibujandoFormaLibre = false;

int formaLibreUltimoX = 0;
int formaLibreUltimoY = 0;

int formaLibreInicio = 0;

// ROTACION INDIVIDUAL
int figuraSeleccionada = -1;
float anguloRotacion = 45.0f;

// TRASLACION INDIVIDUAL
int desplazamientoX = 0;
int desplazamientoY = 0;

int verticesX[MAX_FIGURAS][4];
int verticesY[MAX_FIGURAS][4];
int cantidadVertices[MAX_FIGURAS];

// COLOR ACTUAL DEL DIBUJO
float colorDibujo[4] = {
    0.1f, 0.8f, 0.2f, 1.0f
};

// COLOR INDEPENDIENTE DEL RELLENO
float colorRelleno[4] = {
    1.0f, 1.0f, 0.0f, 1.0f
};

// COLOR INDIVIDUAL DE CADA FIGURA
float coloresFiguras[MAX_FIGURAS][4];

// GUARDAR EL COLOR ACTUAL EN UNA FIGURA
void guardarColorFigura(int indice)
{
    for (int i = 0; i < 4; i++)
    {
        coloresFiguras[indice][i] = colorDibujo[i];
    }
}

// VERTICES PARA LAS FIGURAS GEOMETRICAS

bool figuraInicioSeleccionado = false;

int figuraX0 = 0;
int figuraY0 = 0;
int alturaTriangulo = 5;


// CALCULAR CELDA DE LA GRILLA

void getGridCell(
    GLFWwindow* window,
    double mouseX,
    double mouseY,
    int& cellX,
    int& cellY
)
{
    int width;
    int height;

    glfwGetWindowSize(
        window,
        &width,
        &height
    );

    double screenX =
        mouseX / (double)width;

    double screenY =
        ((double)height - mouseY) /
        (double)height;

    screenX += offsetX / 800.0;
    screenY -= offsetY / 800.0;

    cellX = (int)floor(
        screenX * zoom
    );

    cellY = (int)floor(
        screenY * zoom
    );
}


// ALGORITMO DE BRESENHAM
// CON PARAMETRO DE DECISION

void calculateBresenham()
{
    int x = lineX0;
    int y = lineY0;

    int dx = abs(lineX1 - lineX0);
    int dy = abs(lineY1 - lineY0);

    int sx = (lineX0 < lineX1) ? 1 : -1;
    int sy = (lineY0 < lineY1) ? 1 : -1;

    int p = dx - dy;

    while (true)
    {
        // Guardar la celda sin borrar las anteriores
        if (lineCellCount < MAX_LINE_CELLS)
        {
            lineCellsX[lineCellCount] = x;
            lineCellsY[lineCellCount] = y;
            lineCellCount++;
        }
        else
        {
            break; // Se alcanzó la capacidad disponible
        }

        if (x == lineX1 && y == lineY1)
            break;

        int p2 = 2 * p;

        if (p2 > -dy)
        {
            p -= dy;
            x += sx;
        }

        if (p2 < dx)
        {
            p += dx;
            y += sy;
        }
    }
}
// DIBUJAR UN LADO UTILIZANDO BRESENHAM

void dibujarSegmento(
    int x0, int y0,
    int x1, int y1
)
{
    lineX0 = x0;
    lineY0 = y0;

    lineX1 = x1;
    lineY1 = y1;

    calculateBresenham();
}

// CONSTRUIR FIGURAS UTILIZANDO BRESENHAM

void registrarFiguraPoligono(
    TipoFigura tipo,
    int x0, int y0,
    int x1, int y1
)
{
    if (figuraCount >= MAX_FIGURAS)
    {
        std::printf("Limite de figuras alcanzado.\n");
        return;
    }

    // GUARDAR EL INICIO DE LA FIGURA
    int inicio = lineCellCount;

    // VERTICES
    int ax = x0;
    int ay = y0;

    int bx, by;
    int cx, cy;
    int dx, dy;


    // TRIANGULO

    if (tipo == FIGURA_TRIANGULO)
    {
        // BASE HORIZONTAL
        bx = x1;
        by = y0;

        // VERTICE SUPERIOR CENTRADO
        cx = (x0 + x1) / 2;
        cy = y0 + alturaTriangulo;

        // DIBUJAR LOS TRES LADOS
        dibujarSegmento(ax, ay, bx, by);
        dibujarSegmento(bx, by, cx, cy);
        dibujarSegmento(cx, cy, ax, ay);
    }
    // CUADRADO FORMADO POR DOS TRIANGULOS


    else if (tipo == FIGURA_CUADRADO)
    {
        int distanciaX = std::abs(x1 - x0);
        int distanciaY = std::abs(y1 - y0);

        int lado = (distanciaX > distanciaY)
            ? distanciaX
            : distanciaY;

        int signoX = (x1 >= x0) ? 1 : -1;
        int signoY = (y1 >= y0) ? 1 : -1;

        // CUATRO VERTICES DEL CUADRADO

        bx = x0 + signoX * lado;
        by = y0;

        cx = x0;
        cy = y0 + signoY * lado;

        dx = bx;
        dy = cy;

        // PRIMER TRIANGULO: A - B - D
        // SEGUNDO TRIANGULO: A - D - C
        //
        // COMPARTEN LA DIAGONAL A-D.
        // NO DIBUJAMOS ESA DIAGONAL.

        // CONTORNO EXTERIOR DEL CUADRADO

        dibujarSegmento(ax, ay, bx, by);

        dibujarSegmento(bx, by, dx, dy);

        dibujarSegmento(dx, dy, cx, cy);

        dibujarSegmento(cx, cy, ax, ay);
    }

    // =================================
    // RECTANGULO FORMADO POR DOS TRIANGULOS
    // =================================

    else if (tipo == FIGURA_RECTANGULO)
    {
        // CUATRO VERTICES DEL RECTANGULO

        bx = x1;
        by = y0;

        cx = x0;
        cy = y1;

        dx = x1;
        dy = y1;

        // DOS TRIANGULOS COMPARTEN
        // LA DIAGONAL A-D.
        // NO DIBUJAMOS ESA DIAGONAL.

        // CONTORNO EXTERIOR

        dibujarSegmento(ax, ay, bx, by);

        dibujarSegmento(bx, by, dx, dy);

        dibujarSegmento(dx, dy, cx, cy);

        dibujarSegmento(cx, cy, ax, ay);
    }


    // GUARDAR FIGURA EN EL REGISTRO


    int cantidad = lineCellCount - inicio;

    if (cantidad > 0)
    {
        tiposFiguras[figuraCount] = tipo;

        figuraInicio[figuraCount] = inicio;

        figuraCantidad[figuraCount] = cantidad;

        figuraBorrada[figuraCount] = false;

        if (tipo == FIGURA_TRIANGULO)
        {
            cantidadVertices[figuraCount] = 3;

            verticesX[figuraCount][0] = ax;
            verticesY[figuraCount][0] = ay;

            verticesX[figuraCount][1] = bx;
            verticesY[figuraCount][1] = by;

            verticesX[figuraCount][2] = cx;
            verticesY[figuraCount][2] = cy;
        }
        else
        {
            cantidadVertices[figuraCount] = 4;

            verticesX[figuraCount][0] = ax;
            verticesY[figuraCount][0] = ay;

            verticesX[figuraCount][1] = bx;
            verticesY[figuraCount][1] = by;

            verticesX[figuraCount][2] = dx;
            verticesY[figuraCount][2] = dy;

            verticesX[figuraCount][3] = cx;
            verticesY[figuraCount][3] = cy;
        }
        guardarColorFigura(figuraCount);
        figuraCount++;
    }
}
// ROTAR EL RELLENO DE UNA FIGURA
void rotarRellenoFigura(
    int indice,
    float centroX,
    float centroY,
    float coseno,
    float seno
)
{
    for (int r = 0; r < (int)rellenos.size(); r++)
    {
        if (rellenos[r].figuraPropietaria != indice)
            continue;

        for (int c = 0; c < (int)celdasRellenadas.size(); c++)
        {
            if (c >= (int)idsRellenoPorCelda.size())
                continue;

            if (idsRellenoPorCelda[c] != r)
                continue;

            float x = celdasRellenadas[c].x - centroX;
            float y = celdasRellenadas[c].y - centroY;

            float xNuevo = x * coseno - y * seno;
            float yNuevo = x * seno + y * coseno;

            celdasRellenadas[c].x =
                (int)round(xNuevo + centroX);

            celdasRellenadas[c].y =
                (int)round(yNuevo + centroY);
        }
    }
}

// ROTAR UNA FIGURA INDIVIDUAL
void rotarFigura(int indice, float grados)
{
    if (indice < 0 || indice >= figuraCount)
        return;

    if (figuraBorrada[indice])
        return;

    // LOS CIRCULOS NO NECESITAN ROTACION
    if (tiposFiguras[indice] == FIGURA_CIRCULO)
        return;

    int n = cantidadVertices[indice];

    // ROTAR DIBUJOS REALIZADOS EN MODO LIBRE
    if (n == 0)
    {
        int inicio = figuraInicio[indice];
        int cantidad = figuraCantidad[indice];

        if (cantidad <= 0)
            return;

        // CALCULAR EL CENTRO DEL DIBUJO
        float centroX = 0.0f;
        float centroY = 0.0f;

        for (int i = 0; i < cantidad; i++)
        {
            int celda = inicio + i;

            centroX += lineCellsX[celda];
            centroY += lineCellsY[celda];
        }

        centroX /= cantidad;
        centroY /= cantidad;

        // CONVERTIR ANGULO A RADIANES
        float radianes = grados * 3.14159265f / 180.0f;

        float coseno = cos(radianes);
        float seno = sin(radianes);

        // GUARDAR LAS NUEVAS POSICIONES
        std::vector<int> nuevosX;
        std::vector<int> nuevosY;

        for (int i = 0; i < cantidad; i++)
        {
            int celda = inicio + i;

            float x = lineCellsX[celda] - centroX;
            float y = lineCellsY[celda] - centroY;

            float xNuevo = x * coseno - y * seno;
            float yNuevo = x * seno + y * coseno;

            nuevosX.push_back(
                (int)round(xNuevo + centroX)
            );

            nuevosY.push_back(
                (int)round(yNuevo + centroY)
            );
        }

        // COMPROBAR ESPACIO DISPONIBLE
        if (lineCellCount + cantidad > MAX_LINE_CELLS)
        {
            std::printf("No hay espacio suficiente para rotar el dibujo.\n");
            return;
        }

        // GUARDAR LAS CELDAS ROTADAS
        int nuevoInicio = lineCellCount;

        for (int i = 0; i < cantidad; i++)
        {
            lineCellsX[lineCellCount] = nuevosX[i];
            lineCellsY[lineCellCount] = nuevosY[i];

            lineCellCount++;
        }

        // ACTUALIZAR REGISTRO DE LA FIGURA
        figuraInicio[indice] = nuevoInicio;
        figuraCantidad[indice] = cantidad;

        // ROTAR TAMBIEN EL RELLENO
        rotarRellenoFigura(
            indice,
            centroX,
            centroY,
            coseno,
            seno
        );

        std::printf("Dibujo libre rotado %.2f grados.\n", grados);

        return;
    }

    // LAS FIGURAS GEOMETRICAS CONSERVAN SU ROTACION ORIGINAL
    if (n < 2 || n > 4)
        return;

    // CALCULAR CENTRO DE LA FIGURA
    float centroX = 0.0f;
    float centroY = 0.0f;

    for (int i = 0; i < n; i++)
    {
        centroX += verticesX[indice][i];
        centroY += verticesY[indice][i];
    }

    centroX /= n;
    centroY /= n;

    // CONVERTIR ANGULO A RADIANES
    float radianes = grados * 3.14159265f / 180.0f;

    float coseno = cos(radianes);
    float seno = sin(radianes);

    int nuevosX[4];
    int nuevosY[4];

    // APLICAR MATRIZ DE ROTACION
    for (int i = 0; i < n; i++)
    {
        float x = verticesX[indice][i] - centroX;
        float y = verticesY[indice][i] - centroY;

        float xNuevo = x * coseno - y * seno;
        float yNuevo = x * seno + y * coseno;

        nuevosX[i] = (int)round(xNuevo + centroX);
        nuevosY[i] = (int)round(yNuevo + centroY);
    }

    // COMPROBAR ESPACIO DISPONIBLE
    int espacioNecesario = 0;

    int lados = (n == 2) ? 1 : n;

    for (int i = 0; i < lados; i++)
    {
        int siguiente = (i + 1) % n;

        int dx = abs(nuevosX[siguiente] - nuevosX[i]);
        int dy = abs(nuevosY[siguiente] - nuevosY[i]);

        int mayor = (dx > dy) ? dx : dy;

        espacioNecesario += mayor + 1;
    }

    if (lineCellCount + espacioNecesario > MAX_LINE_CELLS)
    {
        std::printf("No hay espacio suficiente para rotar la figura.\n");
        return;
    }

    // GUARDAR NUEVA POSICION
    int inicio = lineCellCount;

    for (int i = 0; i < lados; i++)
    {
        int siguiente = (i + 1) % n;

        dibujarSegmento(
            nuevosX[i],
            nuevosY[i],
            nuevosX[siguiente],
            nuevosY[siguiente]
        );
    }

    // ACTUALIZAR REGISTRO
    figuraInicio[indice] = inicio;
    figuraCantidad[indice] = lineCellCount - inicio;

    // ACTUALIZAR VERTICES
    for (int i = 0; i < n; i++)
    {
        verticesX[indice][i] = nuevosX[i];
        verticesY[indice][i] = nuevosY[i];
    }

    // ROTAR TAMBIEN EL RELLENO
    rotarRellenoFigura(
        indice,
        centroX,
        centroY,
        coseno,
        seno
    );

    std::printf("Figura rotada %.2f grados.\n", grados);
}
// TRASLADAR UNA FIGURA INDIVIDUAL
void trasladarFigura(int indice, int tx, int ty)
{
    if (indice < 0 || indice >= figuraCount)
        return;

    if (figuraBorrada[indice])
        return;

    // SI ES UN CIRCULO
    if (tiposFiguras[indice] == FIGURA_CIRCULO)
    {
        int c = figuraInicio[indice];

        if (c >= 0 && c < circleCount)
        {
            allCircleCenterX[c] += tx;
            allCircleCenterY[c] += ty;
        }
    }
    else
    {
        int inicio = figuraInicio[indice];
        int cantidad = figuraCantidad[indice];

        // TRASLADAR CONTORNO
        for (int i = 0; i < cantidad; i++)
        {
            int celda = inicio + i;

            if (celda >= 0 && celda < lineCellCount)
            {
                lineCellsX[celda] += tx;
                lineCellsY[celda] += ty;
            }
        }

        // TRASLADAR VERTICES
        int n = cantidadVertices[indice];

        for (int i = 0; i < n; i++)
        {
            verticesX[indice][i] += tx;
            verticesY[indice][i] += ty;
        }
    }

    // TRASLADAR RELLENO
    for (int r = 0; r < (int)rellenos.size(); r++)
    {
        if (rellenos[r].figuraPropietaria == indice)
        {
            for (int c = 0; c < (int)celdasRellenadas.size(); c++)
            {
                if (c < (int)idsRellenoPorCelda.size() &&
                    idsRellenoPorCelda[c] == r)
                {
                    celdasRellenadas[c].x += tx;
                    celdasRellenadas[c].y += ty;
                }
            }
        }
    }

    std::printf(
        "Figura trasladada: X = %d, Y = %d\n",
        tx,
        ty
    );
}

void calculateBresenhamCircle()
{
    circleCellCount = 0;

    int x = 0;
    int y = circleRadius;

    int p = 3 - 2 * circleRadius;

    while (x <= y)
    {
        int puntosX[8] = {
            circleCenterX + x,
            circleCenterX - x,
            circleCenterX + x,
            circleCenterX - x,
            circleCenterX + y,
            circleCenterX - y,
            circleCenterX + y,
            circleCenterX - y
        };

        int puntosY[8] = {
            circleCenterY + y,
            circleCenterY + y,
            circleCenterY - y,
            circleCenterY - y,
            circleCenterY + x,
            circleCenterY + x,
            circleCenterY - x,
            circleCenterY - x
        };

        for (int i = 0; i < 8; i++)
        {
            if (circleCellCount < MAX_CIRCLE_CELLS)
            {
                circleCellsX[circleCellCount] = puntosX[i];
                circleCellsY[circleCellCount] = puntosY[i];
                circleCellCount++;
            }
        }

        if (p < 0)
        {
            p = p + 4 * x + 6;
        }
        else
        {
            p = p + 4 * (x - y) + 10;
            y--;
        }

        x++;
    }
}

bool celdaPerteneceCirculo(
    int currentX,
    int currentY,
    int centerX,
    int centerY,
    int radius
)
{
    int x = 0;
    int y = radius;
    int p = 3 - 2 * radius;

    while (x <= y)
    {
        if (
            (currentX == centerX + x && currentY == centerY + y) ||
            (currentX == centerX - x && currentY == centerY + y) ||
            (currentX == centerX + x && currentY == centerY - y) ||
            (currentX == centerX - x && currentY == centerY - y) ||
            (currentX == centerX + y && currentY == centerY + x) ||
            (currentX == centerX - y && currentY == centerY + x) ||
            (currentX == centerX + y && currentY == centerY - x) ||
            (currentX == centerX - y && currentY == centerY - x)
            )
        {
            return true;
        }

        if (p < 0)
        {
            p = p + 4 * x + 6;
        }
        else
        {
            p = p + 4 * (x - y) + 10;
            y--;
        }

        x++;
    }

    return false;
}

// COMPROBAR SI UNA CELDA PERTENECE A UN CONTORNO

bool celdaEsContorno(int x, int y)
{
  
    for (int f = 0; f < figuraCount; f++)
    {
        if (figuraBorrada[f])
            continue;

        // CONTORNOS DE LINEAS Y POLIGONOS
        if (tiposFiguras[f] != FIGURA_CIRCULO)
        {
            int inicio = figuraInicio[f];
            int cantidad = figuraCantidad[f];

            for (int i = 0; i < cantidad; i++)
            {
                int indice = inicio + i;

                if (indice < 0 || indice >= lineCellCount)
                    break;

                if (lineCellsX[indice] == x &&
                    lineCellsY[indice] == y)
                {
                    return true;
                }
            }
        }
        // CONTORNO DE CIRCULOS
        else
        {
            int c = figuraInicio[f];

            if (c >= 0 && c < circleCount)
            {
                if (celdaPerteneceCirculo(
                    x,
                    y,
                    allCircleCenterX[c],
                    allCircleCenterY[c],
                    allCircleRadius[c]
                ))
                {
                    return true;
                
                }
            }
        }
    }

    // LAS CELDAS BORRADAS NO DEBEN SER BARRERAS
    for (int i = 0; i < erasedCellCount; i++)
    {
        if (erasedCellsX[i] == x &&
            erasedCellsY[i] == y)
        {
            return false;
        }
    }

    return false;
}
// IDENTIFICAR LA FIGURA A LA QUE PERTENECE UN CONTORNO
int obtenerFiguraContorno(int x, int y)
{

    for (int f = 0; f < figuraCount; f++)
    {
        if (figuraBorrada[f])
            continue;

        // LINEAS Y FIGURAS POLIGONALES
        if (tiposFiguras[f] != FIGURA_CIRCULO)
        {
            int inicio = figuraInicio[f];
            int cantidad = figuraCantidad[f];

            for (int i = 0; i < cantidad; i++)
            {
                int indice = inicio + i;

                if (indice < 0 || indice >= lineCellCount)
                    break;

                if (lineCellsX[indice] == x &&
                    lineCellsY[indice] == y)
                {
                    return f;
                }
            }
        }
        // CIRCULOS
        else
        {
            int c = figuraInicio[f];

            if (c >= 0 && c < circleCount)
            {
                int dx = x - allCircleCenterX[c];
                int dy = y - allCircleCenterY[c];

                int radio = allCircleRadius[c];

                int distancia2 = dx * dx + dy * dy;
                int radio2 = radio * radio;

                if (celdaPerteneceCirculo(
                    x,
                    y,
                    allCircleCenterX[c],
                    allCircleCenterY[c],
                    allCircleRadius[c]
                ))
                {
                    return f;
                }
            }
        }
    }

    return -1;
}

// ALGORITMO DEL CUBITO DE PINTURA

void aplicarFloodFill(
    GLFWwindow* window,
    int inicioX,
    int inicioY
)
{
    // NO RELLENAR SI EL CLIC ESTA SOBRE UN CONTORNO
    if (celdaEsContorno(inicioX, inicioY))
    {
        std::printf("El clic esta sobre un contorno.\n");
        return;
    }

    // OBTENER LOS LIMITES VISIBLES DE LA GRILLA
    int width, height;

    glfwGetWindowSize(window, &width, &height);

    int minX, minY;
    int maxX, maxY;

    getGridCell(window, 0, height, minX, minY);
    getGridCell(window, width, 0, maxX, maxY);

    if (minX > maxX) std::swap(minX, maxX);
    if (minY > maxY) std::swap(minY, maxY);

    int ancho = maxX - minX + 1;
    int alto = maxY - minY + 1;

    if (ancho <= 0 || alto <= 0)
        return;

    // LIMITE DE SEGURIDAD
    const int MAX_CELDAS = 50000;

    if ((long long)ancho * alto > MAX_CELDAS)
    {
        std::printf("Acerca el zoom para utilizar el cubito.\n");
        return;
    }

    // MARCAR CELDAS VISITADAS
    std::vector<bool> visitadas(ancho * alto, false);

    std::queue<CeldaRelleno> pendientes;

    pendientes.push({ inicioX, inicioY });

    int cantidadRellenada = 0;

    // CONTAR CUANTAS CELDAS DEL CONTORNO
// PERTENECEN A CADA FIGURA
    int contactos[MAX_FIGURAS] = { 0 };

    // RECORDAR DONDE COMIENZA ESTE RELLENO
    int inicioRelleno = (int)celdasRellenadas.size();

    while (!pendientes.empty())
    {
        CeldaRelleno actual = pendientes.front();
        pendientes.pop();

        int x = actual.x;
        int y = actual.y;

        // COMPROBAR LIMITES
        if (x < minX || x > maxX ||
            y < minY || y > maxY)
            continue;

        int indice = (y - minY) * ancho + (x - minX);

        if (visitadas[indice])
            continue;

        visitadas[indice] = true;

        // DETENERSE AL ENCONTRAR UN CONTORNO
        if (celdaEsContorno(x, y))
        {
            int figura = obtenerFiguraContorno(x, y);

            if (figura >= 0 && figura < MAX_FIGURAS)
            {
                contactos[figura]++;
            }

            continue;
        }

        // GUARDAR CELDA DEL RELLENO
        celdasRellenadas.push_back({ x, y });

        cantidadRellenada++;



        // EXPLORAR LAS CUATRO DIRECCIONES
        pendientes.push({ x + 1, y });
        pendientes.push({ x - 1, y });
        pendientes.push({ x, y + 1 });
        pendientes.push({ x, y - 1 });
    }

    // IDENTIFICAR LA FIGURA QUE MAS CONTORNO APORTO
    int figuraPropietaria = -1;
    int mayorContacto = 0;
 

    for (int f = 0; f < figuraCount; f++)
    {
        if (contactos[f] > mayorContacto)
        {
            mayorContacto = contactos[f];
            figuraPropietaria = f;
        }
    }

    // BUSCAR SI ESTA ZONA YA TENIA UN RELLENO
    if (cantidadRellenada > 0)
    {
        int idExistente = -1;

        // BUSCAR ENTRE LOS RELLENOS ANTERIORES
        for (int i = 0; i < inicioRelleno; i++)
        {
            if (celdasRellenadas[i].x == inicioX &&
                celdasRellenadas[i].y == inicioY)
            {
                idExistente = idsRellenoPorCelda[i];
                break;
            }
        }

        int idFinal;

        // SI YA EXISTIA, ACTUALIZAR SU COLOR
        if (idExistente >= 0 &&
            idExistente < (int)rellenos.size())
        {
            idFinal = idExistente;

            rellenos[idFinal].color[0] = colorRelleno[0];
            rellenos[idFinal].color[1] = colorRelleno[1];
            rellenos[idFinal].color[2] = colorRelleno[2];
            rellenos[idFinal].color[3] = colorRelleno[3];
        }
        else
        {
            // SI NO EXISTIA, CREAR UN RELLENO NUEVO
            Relleno nuevoRelleno;

            nuevoRelleno.figuraPropietaria = figuraPropietaria;

            nuevoRelleno.color[0] = colorRelleno[0];
            nuevoRelleno.color[1] = colorRelleno[1];
            nuevoRelleno.color[2] = colorRelleno[2];
            nuevoRelleno.color[3] = colorRelleno[3];

            idFinal = (int)rellenos.size();

            rellenos.push_back(nuevoRelleno);
        }

        // ASIGNAR EL ID DEL RELLENO A LAS CELDAS
        for (int i = inicioRelleno;
            i < (int)celdasRellenadas.size();
            i++)
        {
            idsRellenoPorCelda.push_back(idFinal);
        }
    }

    std::printf(
        "Cubito: %d celdas encontradas.\n",
        cantidadRellenada
    );
}

// RUEDA DEL MOUSE PARA ZOOM

void scroll_callback(
    GLFWwindow* window,
    double xoffset,
    double yoffset
)
{
    // Si ImGui necesita la rueda del raton,
    // no modificar el zoom de la grilla.
    if (ImGui::GetCurrentContext() != nullptr &&
        ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    // Zoom de la grilla
    zoom += (float)yoffset * 2.0f;

    if (zoom < MIN_ZOOM)
    {
        zoom = MIN_ZOOM;
    }

    if (zoom > MAX_ZOOM)
    {
        zoom = MAX_ZOOM;
    }
}


// BOTONES DEL MOUSE

void mouse_button_callback(
    GLFWwindow* window,
    int button,
    int action,
    int mods
)
{
    // Si Dear ImGui necesita el ratón, no ejecutar
    // las acciones de la grilla.
    if (ImGui::GetCurrentContext() != nullptr &&
        ImGui::GetIO().WantCaptureMouse)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT &&
            action == GLFW_RELEASE)
        {
            dragging = false;
        }

        return;
    }

    // CLIC IZQUIERDO: FORMA LIBRE O MOVER LA GRILLA
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        // PRESIONAR
        if (action == GLFW_PRESS)
        {
            if (herramientaActual == HERRAMIENTA_RELLENO)
            {
                double mouseX, mouseY;

                glfwGetCursorPos(
                    window,
                    &mouseX,
                    &mouseY
                );

                int cellX, cellY;

                getGridCell(
                    window,
                    mouseX,
                    mouseY,
                    cellX,
                    cellY
                );

                std::printf(
                    "Cubito: celda seleccionada X = %d, Y = %d\n",
                    cellX,
                    cellY
                );

                aplicarFloodFill(
                    window,
                    cellX,
                    cellY
                );

                return;
            }
            if (herramientaActual == HERRAMIENTA_FORMA_LIBRE)
            {
                if (figuraCount >= MAX_FIGURAS)
                {
                    std::printf("Limite de figuras alcanzado.\n");
                    return;
                }

                double mouseX, mouseY;

                glfwGetCursorPos(window, &mouseX, &mouseY);

                int cellX, cellY;

                getGridCell(
                    window,
                    mouseX,
                    mouseY,
                    cellX,
                    cellY
                );

                dibujandoFormaLibre = true;
                dragging = false;

                formaLibreInicio = lineCellCount;

                formaLibreUltimoX = cellX;
                formaLibreUltimoY = cellY;

                // GUARDAR LA PRIMERA CELDA
                if (lineCellCount < MAX_LINE_CELLS)
                {
                    lineCellsX[lineCellCount] = cellX;
                    lineCellsY[lineCellCount] = cellY;
                    lineCellCount++;
                }
            }
            else
            {
                dragging = true;

                glfwGetCursorPos(
                    window,
                    &lastMouseX,
                    &lastMouseY
                );
            }
        }

        // SOLTAR
        else if (action == GLFW_RELEASE)
        {
            dragging = false;

            if (dibujandoFormaLibre)
            {
                dibujandoFormaLibre = false;

                int cantidad = lineCellCount - formaLibreInicio;

                if (cantidad > 0 && figuraCount < MAX_FIGURAS)
                {
                    tiposFiguras[figuraCount] = FIGURA_LINEA;

                    figuraInicio[figuraCount] = formaLibreInicio;

                    figuraCantidad[figuraCount] = cantidad;

                    figuraBorrada[figuraCount] = false;

                    cantidadVertices[figuraCount] = 0;

                    guardarColorFigura(figuraCount);

                    figuraCount++;

                    std::printf("Forma libre guardada correctamente.\n");
                }
            }
        }
    }

    // SELECCIONAR FIGURA PARA ROTAR
    if (button == GLFW_MOUSE_BUTTON_RIGHT &&
        action == GLFW_PRESS &&
        (herramientaActual == HERRAMIENTA_ROTACION ||
            herramientaActual == HERRAMIENTA_TRASLACION ||
            herramientaActual == HERRAMIENTA_COLOR))
    {
        double mouseX, mouseY;

        glfwGetCursorPos(window, &mouseX, &mouseY);

        int cellX, cellY;

        getGridCell(window, mouseX, mouseY, cellX, cellY);

        figuraSeleccionada = -1;

        // BUSCAR LA FIGURA DESDE LA ULTIMA DIBUJADA
        for (int f = figuraCount - 1; f >= 0; f--)
        {
            if (figuraBorrada[f])
                continue;

            // COMPROBAR SI ES UN CIRCULO
            if (tiposFiguras[f] == FIGURA_CIRCULO)
            {
                int c = figuraInicio[f];

                if (c >= 0 && c < circleCount)
                {
                    if (celdaPerteneceCirculo(
                        cellX,
                        cellY,
                        allCircleCenterX[c],
                        allCircleCenterY[c],
                        allCircleRadius[c]))
                    {
                        figuraSeleccionada = f;
                        break;
                    }
                }

                continue;
            }

            // COMPROBAR LINEAS Y OTRAS FIGURAS
            int inicio = figuraInicio[f];
            int cantidad = figuraCantidad[f];

            for (int i = 0; i < cantidad; i++)
            {
                int indice = inicio + i;

                if (indice >= lineCellCount)
                    break;

                if (lineCellsX[indice] == cellX &&
                    lineCellsY[indice] == cellY)
                {
                    figuraSeleccionada = f;
                    break;
                }
            }

            if (figuraSeleccionada != -1)
                break;
        }

        if (figuraSeleccionada != -1)
            std::printf("Figura seleccionada: %d\n", figuraSeleccionada);
        else
            std::printf("No se encontro una figura en esa celda.\n");
    }
    int figuraAEliminar = -1;
    // BORRADOR DE FIGURA: CLIC DERECHO
    if (button == GLFW_MOUSE_BUTTON_RIGHT &&
        action == GLFW_PRESS &&
        herramientaActual == HERRAMIENTA_BORRADOR_FIGURA)
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        int cellX, cellY;
        getGridCell(window, mouseX, mouseY, cellX, cellY);

        bool figuraEncontrada = false;

        // Buscar entre todas las figuras
        for (int f = figuraCount - 1; f >= 0; f--)
        {
            if (figuraBorrada[f])
                continue;

            // COMPROBAR LINEA
            if (tiposFiguras[f] == FIGURA_LINEA ||
                tiposFiguras[f] == FIGURA_TRIANGULO ||
                tiposFiguras[f] == FIGURA_CUADRADO ||
                tiposFiguras[f] == FIGURA_RECTANGULO)
            {
                int inicio = figuraInicio[f];
                int cantidad = figuraCantidad[f];

                for (int i = 0; i < cantidad; i++)
                {
                    int indice = inicio + i;

                    if (indice >= lineCellCount)
                        break;

                    if (lineCellsX[indice] == cellX &&
                        lineCellsY[indice] == cellY)
                    {
                        figuraBorrada[f] = true;
                        figuraEncontrada = true;
                        figuraAEliminar = f;

                        break;
                    }
                }
            }

            // COMPROBAR CIRCULO
            else if (tiposFiguras[f] == FIGURA_CIRCULO)
            {
                int c = figuraInicio[f];

                if (c < circleCount &&
                    celdaPerteneceCirculo(
                        cellX,
                        cellY,
                        allCircleCenterX[c],
                        allCircleCenterY[c],
                        allCircleRadius[c]))
                {
                    figuraBorrada[f] = true;
                    figuraEncontrada = true;
                    figuraAEliminar = f;

                    

                }
            }

            if (figuraEncontrada)
                break;
        }
        if (figuraEncontrada)
        {
            // ELIMINAR LOS RELLENOS DE LA FIGURA
            std::vector<int> nuevoMapa(rellenos.size(), -1);
            std::vector<Relleno> nuevosRellenos;

            // CONSERVAR LOS RELLENOS DE OTRAS FIGURAS
            for (int i = 0; i < (int)rellenos.size(); i++)
            {
                if (rellenos[i].figuraPropietaria != figuraAEliminar)
                {
                    nuevoMapa[i] = (int)nuevosRellenos.size();
                    nuevosRellenos.push_back(rellenos[i]);
                }
            }

            // CONSERVAR SOLAMENTE LAS CELDAS DE LOS RELLENOS QUE SIGUEN
            std::vector<CeldaRelleno> nuevasCeldas;
            std::vector<int> nuevosIds;

            for (int i = 0; i < (int)celdasRellenadas.size(); i++)
            {
                int idAnterior = idsRellenoPorCelda[i];

                if (idAnterior >= 0 &&
                    idAnterior < (int)nuevoMapa.size() &&
                    nuevoMapa[idAnterior] != -1)
                {
                    nuevasCeldas.push_back(celdasRellenadas[i]);
                    nuevosIds.push_back(nuevoMapa[idAnterior]);
                }
            }

            rellenos = nuevosRellenos;
            celdasRellenadas = nuevasCeldas;
            idsRellenoPorCelda = nuevosIds;

            rellenoSeleccionado = -1;

            std::printf("Figura eliminada.\n");
        }
    }

    // BORRADOR POR CELDA: CLIC DERECHO
    if (button == GLFW_MOUSE_BUTTON_RIGHT &&
        action == GLFW_PRESS &&
        herramientaActual == HERRAMIENTA_BORRADOR_CELDA)
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        int cellX, cellY;
        getGridCell(window, mouseX, mouseY, cellX, cellY);

        // Borrar la celda de la linea, si pertenece a ella
        for (int i = 0; i < lineCellCount; i++)
        {
            if (lineCellsX[i] == cellX &&
                lineCellsY[i] == cellY)
            {
                for (int j = i; j < lineCellCount - 1; j++)
                {
                    lineCellsX[j] = lineCellsX[j + 1];
                    lineCellsY[j] = lineCellsY[j + 1];
                }

                lineCellCount--;
                break;
            }
        }

        // Registrar la celda para ocultarla tambien
        // si pertenece a un circulo
        bool yaBorrada = false;

        for (int i = 0; i < erasedCellCount; i++)
        {
            if (erasedCellsX[i] == cellX &&
                erasedCellsY[i] == cellY)
            {
                yaBorrada = true;
                break;
            }
        }

        if (!yaBorrada && erasedCellCount < MAX_ERASED_CELLS)
        {
            erasedCellsX[erasedCellCount] = cellX;
            erasedCellsY[erasedCellCount] = cellY;
            erasedCellCount++;
        }

        std::printf(
            "Celda borrada: X = %d, Y = %d\n",
            cellX, cellY
        );
    }


    // CLIC DERECHO: LINEA DE BRESENHAM
    if (button == GLFW_MOUSE_BUTTON_RIGHT &&
        action == GLFW_PRESS &&
        herramientaActual == HERRAMIENTA_LINEA)
    {
        double mouseX;
        double mouseY;

        glfwGetCursorPos(
            window,
            &mouseX,
            &mouseY
        );

        int cellX;
        int cellY;

        getGridCell(
            window,
            mouseX,
            mouseY,
            cellX,
            cellY
        );

        if (!lineStartSelected)
        {
            lineX0 = cellX;
            lineY0 = cellY;

            lineStartSelected = true;
            lineEndSelected = false;


            std::printf(
                "Punto inicial: X = %d, Y = %d\n",
                lineX0,
                lineY0
            );
        }
        else
        {
            lineX1 = cellX;
            lineY1 = cellY;

            lineEndSelected = true;

            std::printf(
                "Punto final: X = %d, Y = %d\n",
                lineX1,
                lineY1
            );

            int inicioAnterior = lineCellCount;

            calculateBresenham();

            if (figuraCount < MAX_FIGURAS)
            {
                tiposFiguras[figuraCount] = FIGURA_LINEA;

                figuraInicio[figuraCount] = inicioAnterior;

                figuraCantidad[figuraCount] =
                    lineCellCount - inicioAnterior;

                figuraBorrada[figuraCount] = false;

                // GUARDAR LOS DOS EXTREMOS DE LA LINEA
                cantidadVertices[figuraCount] = 2;

                verticesX[figuraCount][0] = lineX0;
                verticesY[figuraCount][0] = lineY0;

                verticesX[figuraCount][1] = lineX1;
                verticesY[figuraCount][1] = lineY1;

                guardarColorFigura(figuraCount);

                figuraCount++;
            }

            lineStartSelected = false;
        }
    }

    // CLIC CENTRAL: CIRCULO DE BRESENHAM
    if (button == GLFW_MOUSE_BUTTON_MIDDLE &&
        action == GLFW_PRESS &&
        herramientaActual == HERRAMIENTA_CIRCULO)
    {
        double mouseX;
        double mouseY;

        glfwGetCursorPos(
            window,
            &mouseX,
            &mouseY
        );

        int cellX;
        int cellY;

        getGridCell(
            window,
            mouseX,
            mouseY,
            cellX,
            cellY
        );

        if (!circleCenterSelected)
        {
            circleCenterX = cellX;
            circleCenterY = cellY;

            circleCenterSelected = true;
            circleRadiusSelected = false;
            circleRadius = 0;

            std::printf(
                "Centro del circulo: X = %d, Y = %d\n",
                circleCenterX,
                circleCenterY
            );
        }
        else
        {
            int dx = cellX - circleCenterX;
            int dy = cellY - circleCenterY;

            circleRadius = (int)sqrt(
                (double)(dx * dx + dy * dy)
            );

            if (circleCount < MAX_CIRCLES &&
                figuraCount < MAX_FIGURAS)
            {
                allCircleCenterX[circleCount] = circleCenterX;
                allCircleCenterY[circleCount] = circleCenterY;
                allCircleRadius[circleCount] = circleRadius;

                tiposFiguras[figuraCount] = FIGURA_CIRCULO;

                figuraInicio[figuraCount] = circleCount;

                figuraCantidad[figuraCount] = 1;

                figuraBorrada[figuraCount] = false;

                // GUARDAR LOS DOS EXTREMOS DE LA LINEA
                cantidadVertices[figuraCount] = 2;

                verticesX[figuraCount][0] = lineX0;
                verticesY[figuraCount][0] = lineY0;

                verticesX[figuraCount][1] = lineX1;
                verticesY[figuraCount][1] = lineY1;

                guardarColorFigura(figuraCount);

                figuraCount++;
                circleCount++;

                std::printf(
                    "Radio del circulo = %d\n",
                    circleRadius
                );
            }
            else
            {
                std::printf("Limite de circulos alcanzado.\n");
            }

            circleCenterSelected = false;
            circleRadiusSelected = false;
            circleRadius = 0;
        }
    }

    // FIGURAS GEOMETRICAS: SELECCION DE VERTICES

    if (button == GLFW_MOUSE_BUTTON_RIGHT &&
        action == GLFW_PRESS &&
        (herramientaActual == HERRAMIENTA_TRIANGULO ||
            herramientaActual == HERRAMIENTA_CUADRADO ||
            herramientaActual == HERRAMIENTA_RECTANGULO))
    {
        double mouseX, mouseY;

        glfwGetCursorPos(
            window,
            &mouseX,
            &mouseY
        );

        int cellX, cellY;

        getGridCell(
            window,
            mouseX,
            mouseY,
            cellX,
            cellY
        );

        // PRIMER CLIC: PRIMER VERTICE

        if (!figuraInicioSeleccionado)
        {
            figuraX0 = cellX;
            figuraY0 = cellY;

            figuraInicioSeleccionado = true;

            std::printf(
                "Primer vertice: X = %d, Y = %d\n",
                figuraX0,
                figuraY0
            );
        }

        // SEGUNDO CLIC: CONSTRUIR FIGURA

        else
        {
            TipoFigura tipo;

            if (herramientaActual == HERRAMIENTA_TRIANGULO)
            {
                tipo = FIGURA_TRIANGULO;
            }
            else if (herramientaActual == HERRAMIENTA_CUADRADO)
            {
                tipo = FIGURA_CUADRADO;
            }
            else
            {
                tipo = FIGURA_RECTANGULO;
            }

            registrarFiguraPoligono(

                tipo,
                figuraX0,
                figuraY0,
                cellX,
                cellY
            );


            std::printf("Figura creada correctamente.\n");

            figuraInicioSeleccionado = false;
        }
    }
}


// MOVIMIENTO DEL MOUSE

void cursor_position_callback(
    GLFWwindow* window,
    double xpos,
    double ypos
)
{
    // DIBUJAR FORMA LIBRE
    if (dibujandoFormaLibre)
    {
        int cellX, cellY;

        getGridCell(
            window,
            xpos,
            ypos,
            cellX,
            cellY
        );

        if (cellX != formaLibreUltimoX ||
            cellY != formaLibreUltimoY)
        {
            lineX0 = formaLibreUltimoX;
            lineY0 = formaLibreUltimoY;

            lineX1 = cellX;
            lineY1 = cellY;

            dibujarSegmento(
                formaLibreUltimoX,
                formaLibreUltimoY,
                cellX,
                cellY
            );

            formaLibreUltimoX = cellX;
            formaLibreUltimoY = cellY;
        }

        return;
    }

    // MOVER LA GRILLA
    if (dragging)
    {
        double deltaX = xpos - lastMouseX;
        double deltaY = ypos - lastMouseY;

        offsetX += (float)deltaX;
        offsetY += (float)deltaY;

        lastMouseX = xpos;
        lastMouseY = ypos;
    }
}

int main()
{
    // INICIAR GLFW

    glfwInit();


    // VERSION OPENGL 3.3

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3
    );

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );


    // CREAR VENTANA

    GLFWwindow* window = glfwCreateWindow(
        800,
        800,
        "PROYECTO OPENGL",
        NULL,
        NULL
    );


    if (window == NULL)
    {
        std::cout
            << "Failed to create GLFW window"
            << std::endl;

        glfwTerminate();

        return -1;
    }


    glfwMakeContextCurrent(window);


    // CARGAR GLAD

    gladLoadGL();

    // INICIALIZAR DEAR IMGUI

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    // CALLBACKS

    glfwSetScrollCallback(
        window,
        scroll_callback
    );

    glfwSetMouseButtonCallback(
        window,
        mouse_button_callback
    );

    glfwSetCursorPosCallback(
        window,
        cursor_position_callback
    );

    // CONECTAR IMGUI CON GLFW

    ImGui_ImplGlfw_InitForOpenGL(
        window,
        true
    );

    // CONECTAR IMGUI CON OPENGL

    ImGui_ImplOpenGL3_Init(
        "#version 330 core"
    );


    // VIEWPORT

    glViewport(
        0,
        0,
        800,
        800
    );





    // CUADRADO QUE OCUPA TODA LA PANTALLA

    GLfloat vertices[] =
    {
        -1.0f, -1.0f, 0.0f,

         1.0f, -1.0f, 0.0f,

         1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f
    };


    GLuint indices[] =
    {
        0, 1, 2,

        0, 2, 3
    };


    // SHADER

    Shader shaderProgram(
        "default.vert",
        "default.frag"
    );

    // TEXTURA PARA LOS COLORES INDIVIDUALES
    GLuint coloresFigurasTexture;

    glGenTextures(1, &coloresFigurasTexture);

    glBindTexture(GL_TEXTURE_1D, coloresFigurasTexture);

    glTexParameteri(
        GL_TEXTURE_1D,
        GL_TEXTURE_MIN_FILTER,
        GL_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_1D,
        GL_TEXTURE_MAG_FILTER,
        GL_NEAREST
    );

    glTexImage1D(
        GL_TEXTURE_1D,
        0,
        GL_RGBA32F,
        MAX_FIGURAS,
        0,
        GL_RGBA,
        GL_FLOAT,
        coloresFiguras
    );

    glBindTexture(GL_TEXTURE_1D, 0);

    glTexParameteri(
        GL_TEXTURE_1D,
        GL_TEXTURE_MAG_FILTER,
        GL_NEAREST
    );

    // TEXTURA PARA LAS CELDAS DEL RELLENO

    GLuint celdasRellenoTexture;

    glGenTextures(1, &celdasRellenoTexture);

    glBindTexture(GL_TEXTURE_1D, celdasRellenoTexture);

    glTexParameteri(
        GL_TEXTURE_1D,
        GL_TEXTURE_MIN_FILTER,
        GL_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_1D,
        GL_TEXTURE_MAG_FILTER,
        GL_NEAREST
    );

    int celdaVacia[2] = { 0, 0 };

    glTexImage1D(
        GL_TEXTURE_1D,
        0,
        GL_RG32I,
        1,
        0,
        GL_RG_INTEGER,
        GL_INT,
        celdaVacia
    );

    glBindTexture(GL_TEXTURE_1D, 0);


    glTexImage1D(
        GL_TEXTURE_1D,
        0,
        GL_RG32I,
        1,
        0,
        GL_RG_INTEGER,
        GL_INT,
        celdaVacia
    );

    glBindTexture(GL_TEXTURE_1D, 0);

    // TEXTURA PARA LOS IDENTIFICADORES DE CADA RELLENO

    GLuint idsRellenoTexture;

    glGenTextures(1, &idsRellenoTexture);

    glBindTexture(GL_TEXTURE_1D, idsRellenoTexture);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int idVacio = -1;

    glTexImage1D(
        GL_TEXTURE_1D,
        0,
        GL_R32I,
        1,
        0,
        GL_RED_INTEGER,
        GL_INT,
        &idVacio
    );

    glBindTexture(GL_TEXTURE_1D, 0);


    // TEXTURA PARA LOS COLORES INDEPENDIENTES DE CADA RELLENO

    GLuint coloresRellenosTexture;

    glGenTextures(1, &coloresRellenosTexture);

    glBindTexture(GL_TEXTURE_1D, coloresRellenosTexture);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    float colorVacio[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glTexImage1D(
        GL_TEXTURE_1D,
        0,
        GL_RGBA32F,
        1,
        0,
        GL_RGBA,
        GL_FLOAT,
        colorVacio
    );

    glBindTexture(GL_TEXTURE_1D, 0);

    // VAO

    VAO VAO1;

    VAO1.Bind();


    // VBO

    VBO VBO1(
        vertices,
        sizeof(vertices)
    );


    // EBO

    EBO EBO1(
        indices,
        sizeof(indices)
    );


    // ATRIBUTO DE POSICION

    VAO1.LinkAttrib(
        VBO1,
        0,
        3,
        GL_FLOAT,
        3 * sizeof(float),
        (void*)0
    );


    VAO1.Unbind();

    VBO1.Unbind();

    EBO1.Unbind();


    // UNIFORMS

    GLuint zoomID = glGetUniformLocation(
        shaderProgram.ID,
        "zoom"
    );

    GLuint offsetXID = glGetUniformLocation(
        shaderProgram.ID,
        "offsetX"
    );

    GLuint offsetYID = glGetUniformLocation(
        shaderProgram.ID,
        "offsetY"
    );
    GLuint colorDibujoID = glGetUniformLocation(
        shaderProgram.ID,
        "colorDibujo"
    );


    // UNIFORMS DE BRESENHAM

    GLuint lineCellsXID = glGetUniformLocation(
        shaderProgram.ID,
        "lineCellsX"
    );

    GLuint lineCellsYID = glGetUniformLocation(
        shaderProgram.ID,
        "lineCellsY"
    );

    GLuint lineCellCountID = glGetUniformLocation(
        shaderProgram.ID,
        "lineCellCount"
    );
    GLuint erasedCellsXID =
        glGetUniformLocation(shaderProgram.ID, "erasedCellsX");

    GLuint erasedCellsYID =
        glGetUniformLocation(shaderProgram.ID, "erasedCellsY");

    GLuint erasedCellCountID =
        glGetUniformLocation(shaderProgram.ID, "erasedCellCount");
    GLuint allCircleCenterXID = glGetUniformLocation(
        shaderProgram.ID,
        "allCircleCenterX"
    );

    GLuint allCircleCenterYID = glGetUniformLocation(
        shaderProgram.ID,
        "allCircleCenterY"
    );

    GLuint allCircleRadiusID = glGetUniformLocation(
        shaderProgram.ID,
        "allCircleRadius"
    );

    GLuint circleCountID = glGetUniformLocation(
        shaderProgram.ID,
        "circleCount"
    );
    GLuint tiposFigurasID = glGetUniformLocation(
        shaderProgram.ID, "tiposFiguras"
    );

    GLuint figuraInicioID = glGetUniformLocation(
        shaderProgram.ID, "figuraInicio"
    );

    GLuint figuraCantidadID = glGetUniformLocation(
        shaderProgram.ID, "figuraCantidad"
    );

    GLuint figuraBorradaID = glGetUniformLocation(
        shaderProgram.ID, "figuraBorrada"
    );

    GLuint figuraCountID = glGetUniformLocation(
        shaderProgram.ID, "figuraCount"
    );

    // UBICACION DE LA TEXTURA DE COLORES
    GLuint coloresFigurasTextureID = glGetUniformLocation(
        shaderProgram.ID,
        "coloresFigurasTexture"
    );

    GLuint celdasRellenoTextureID =
        glGetUniformLocation(
            shaderProgram.ID,
            "celdasRellenoTexture"
        );
    GLuint idsRellenoTextureID =
        glGetUniformLocation(
            shaderProgram.ID,
            "idsRellenoTexture"
        );

    GLuint coloresRellenosTextureID =
        glGetUniformLocation(
            shaderProgram.ID,
            "coloresRellenosTexture"
        );

    GLuint celdasRellenoCountID =
        glGetUniformLocation(
            shaderProgram.ID,
            "celdasRellenoCount"
        );




    int figuraBorradaGPU[MAX_FIGURAS];


    // CICLO PRINCIPAL

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(
            1.0f,
            1.0f,
            1.0f,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT
        );

        // INICIAR NUEVO FRAME DE IMGUI

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // PANEL DE HERRAMIENTAS DE MI PAINT

        ImGui::SetNextWindowSize(
            ImVec2(420, 320),
            ImGuiCond_FirstUseEver
        );

        ImGui::Begin("Mi Paint");

        ImGui::Text("Herramientas de dibujo");
        ImGui::Separator();

        ImGui::Text("Color del dibujo");

        if (ImGui::Button("Elegir color"))
        {
            ImGui::OpenPopup("Selector de color");
        }

        if (ImGui::BeginPopup("Selector de color"))
        {
            ImGui::ColorPicker4(
                "##ColorPicker",
                colorDibujo,
                ImGuiColorEditFlags_DisplayRGB |
                ImGuiColorEditFlags_DisplayHex
            );

            if (ImGui::Button("Aceptar"))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        ImGui::Separator();

        ImGui::Text("Color del relleno");

        ImGui::ColorEdit4(
            "Elegir color del relleno",
            colorRelleno
        );

        ImGui::Separator();

        if (ImGui::Button("Limpiar grilla"))
        {
            // LIMPIAR TODOS LOS RELLENOS
            celdasRellenadas.clear();
            idsRellenoPorCelda.clear();
            rellenos.clear();

            rellenoSeleccionado = -1;
            // Limpiar linea
            lineStartSelected = false;
            lineEndSelected = false;
            lineCellCount = 0;
            figuraCount = 0;
            figuraInicioSeleccionado = false;

            // Limpiar circulo
            circleCenterSelected = false;
            circleRadiusSelected = false;
            circleRadius = 0;
            circleCellCount = 0;
            circleCount = 0;
            erasedCellCount = 0;

            // Desactivar herramienta actual
            herramientaActual = HERRAMIENTA_NINGUNA;

        }

        if (ImGui::Button("Linea de Bresenham"))
        {
            herramientaActual = HERRAMIENTA_LINEA;

            lineStartSelected = false;
            lineEndSelected = false;

        }

        if (ImGui::Button("Forma libre"))
        {
            herramientaActual = HERRAMIENTA_FORMA_LIBRE;

            lineStartSelected = false;
            lineEndSelected = false;

            figuraInicioSeleccionado = false;
        }

        if (ImGui::Button("Circulo de Bresenham"))
        {
            herramientaActual = HERRAMIENTA_CIRCULO;

            circleCenterSelected = false;
            circleRadiusSelected = false;
            circleRadius = 0;
        }



        if (ImGui::Button("Borrador por celda"))
        {
            herramientaActual = HERRAMIENTA_BORRADOR_CELDA;

            lineStartSelected = false;
            lineEndSelected = false;
            circleCenterSelected = false;
            circleRadiusSelected = false;
        }

        if (ImGui::Button("Borrador de figura"))
        {
            herramientaActual = HERRAMIENTA_BORRADOR_FIGURA;

            lineStartSelected = false;
            lineEndSelected = false;
            circleCenterSelected = false;
            circleRadiusSelected = false;
        }

        // BOTONES DE FIGURAS GEOMETRICAS

        if (ImGui::Button("Triangulo"))
        {
            herramientaActual = HERRAMIENTA_TRIANGULO;

            figuraInicioSeleccionado = false;
        }
        if (herramientaActual == HERRAMIENTA_TRIANGULO)
        {
            ImGui::InputInt(
                "Altura del triangulo",
                &alturaTriangulo
            );

            if (alturaTriangulo < 1)
                alturaTriangulo = 1;
        }

        if (ImGui::Button("Cuadrado"))
        {
            herramientaActual = HERRAMIENTA_CUADRADO;

            figuraInicioSeleccionado = false;
        }

        if (ImGui::Button("Rectangulo"))
        {
            herramientaActual = HERRAMIENTA_RECTANGULO;

            figuraInicioSeleccionado = false;
        }
        if (ImGui::Button("Rotar figura"))
        {
            herramientaActual = HERRAMIENTA_ROTACION;
            figuraSeleccionada = -1;
            figuraInicioSeleccionado = false;
        }

        if (ImGui::Button("Trasladar figura"))
        {
            herramientaActual = HERRAMIENTA_TRASLACION;

            figuraSeleccionada = -1;
            figuraInicioSeleccionado = false;
        }

        if (ImGui::Button("Cambiar color"))
        {
            herramientaActual = HERRAMIENTA_COLOR;

            figuraSeleccionada = -1;
            figuraInicioSeleccionado = false;
        }

        if (ImGui::Button("Cubito de relleno"))
        {
            herramientaActual = HERRAMIENTA_RELLENO;

            figuraSeleccionada = -1;
            figuraInicioSeleccionado = false;

            lineStartSelected = false;
            lineEndSelected = false;

            circleCenterSelected = false;
            circleRadiusSelected = false;
        }

        ImGui::Separator();

        if (herramientaActual == HERRAMIENTA_LINEA)
        {
            ImGui::Text("Herramienta actual: Linea");
            ImGui::Text("Selecciona dos puntos en la grilla.");
        }
        else if (herramientaActual == HERRAMIENTA_CIRCULO)
        {
            ImGui::Text("Herramienta actual: Circulo");
            ImGui::Text("Selecciona el centro y despues el radio.");
        }
        else
        {
            ImGui::Text("Selecciona una herramienta.");
        }

        // INFORMACION DE FIGURAS

        if (herramientaActual == HERRAMIENTA_TRIANGULO)
        {
            ImGui::Text("Herramienta actual: Triangulo");
            ImGui::Text("Selecciona dos puntos con clic derecho.");
        }

        if (herramientaActual == HERRAMIENTA_CUADRADO)
        {
            ImGui::Text("Herramienta actual: Cuadrado");
            ImGui::Text("Selecciona dos puntos con clic derecho.");
        }

        if (herramientaActual == HERRAMIENTA_RECTANGULO)
        {
            ImGui::Text("Herramienta actual: Rectangulo");
            ImGui::Text("Selecciona dos puntos con clic derecho.");
        }
        if (herramientaActual == HERRAMIENTA_ROTACION)
        {
            ImGui::Text("Herramienta actual: Rotacion");

            ImGui::Text("Selecciona una figura con clic derecho.");

            ImGui::InputFloat(
                "Angulo de rotacion",
                &anguloRotacion
            );

            if (ImGui::Button("Aplicar rotacion"))
            {
                if (figuraSeleccionada != -1)
                {
                    rotarFigura(
                        figuraSeleccionada,
                        anguloRotacion
                    );
                }
                else
                {
                    std::printf("Primero selecciona una figura.\n");
                }
            }
        }

        if (herramientaActual == HERRAMIENTA_FORMA_LIBRE)
        {
            ImGui::Text("Herramienta actual: Forma libre");
            ImGui::Text("Mantén presionado el clic izquierdo y dibuja.");
        }
        // CONTROLES DE TRASLACION
        if (herramientaActual == HERRAMIENTA_TRASLACION)
        {
            ImGui::Text("Herramienta actual: Traslacion");

            ImGui::Text("Selecciona una figura con clic derecho.");

            ImGui::InputInt(
                "Desplazamiento X",
                &desplazamientoX
            );

            ImGui::InputInt(
                "Desplazamiento Y",
                &desplazamientoY
            );

            if (ImGui::Button("Aplicar traslacion"))
            {
                if (figuraSeleccionada != -1)
                {
                    trasladarFigura(
                        figuraSeleccionada,
                        desplazamientoX,
                        desplazamientoY
                    );

                    figuraSeleccionada = -1;
                }
                else
                {
                    std::printf("Primero selecciona una figura.\n");
                }
            }
        }
        // CONTROLES PARA CAMBIAR COLOR
        if (herramientaActual == HERRAMIENTA_COLOR)
        {
            ImGui::Text("Herramienta actual: Cambiar color");

            ImGui::Text("Selecciona una figura con clic derecho.");

            if (figuraSeleccionada != -1)
            {
                ImGui::Text(
                    "Figura seleccionada: %d",
                    figuraSeleccionada
                );

                if (ImGui::Button("Aplicar color"))
                {
                    guardarColorFigura(figuraSeleccionada);

                    figuraSeleccionada = -1;
                }
            }
            else
            {
                ImGui::Text("No hay ninguna figura seleccionada.");
            }
        }
        ImGui::End();


        // ACTIVAR SHADER

        shaderProgram.Activate();

        // ACTIVAR TEXTURA DE COLORES
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(
            GL_TEXTURE_1D,
            coloresFigurasTexture
        );

        // ENVIAR LOS COLORES ACTUALIZADOS
        glTexSubImage1D(
            GL_TEXTURE_1D,
            0,
            0,
            MAX_FIGURAS,
            GL_RGBA,
            GL_FLOAT,
            coloresFiguras
        );

        // INDICAR AL SHADER QUE USE LA TEXTURA 0
        glUniform1i(
            coloresFigurasTextureID,
            0
        );

        // ENVIAR CELDAS DEL RELLENO

        glActiveTexture(GL_TEXTURE2);

        glBindTexture(
            GL_TEXTURE_1D,
            celdasRellenoTexture
        );

        if (!celdasRellenadas.empty())
        {
            glTexImage1D(
                GL_TEXTURE_1D,
                0,
                GL_RG32I,
                (GLsizei)celdasRellenadas.size(),
                0,
                GL_RG_INTEGER,
                GL_INT,
                celdasRellenadas.data()
            );
        }
        else
        {
            int celdaVacia[2] = { 0, 0 };

            glTexImage1D(
                GL_TEXTURE_1D,
                0,
                GL_RG32I,
                1,
                0,
                GL_RG_INTEGER,
                GL_INT,
                celdaVacia
            );
        }

        glUniform1i(celdasRellenoTextureID, 2);

        // ENVIAR IDENTIFICADORES DE CADA CELDA RELLENADA

        glActiveTexture(GL_TEXTURE3);

        glBindTexture(
            GL_TEXTURE_1D,
            idsRellenoTexture
        );

        if (!idsRellenoPorCelda.empty())
        {
            glTexImage1D(
                GL_TEXTURE_1D,
                0,
                GL_R32I,
                (GLsizei)idsRellenoPorCelda.size(),
                0,
                GL_RED_INTEGER,
                GL_INT,
                idsRellenoPorCelda.data()
            );
        }
        else
        {
            int idVacio = -1;

            glTexImage1D(
                GL_TEXTURE_1D,
                0,
                GL_R32I,
                1,
                0,
                GL_RED_INTEGER,
                GL_INT,
                &idVacio
            );
        }

        glUniform1i(
            idsRellenoTextureID,
            3
        );


        // PREPARAR LOS COLORES DE TODOS LOS RELLENOS

        std::vector<float> coloresRellenosGPU;

        coloresRellenosGPU.reserve(rellenos.size() * 4);

        for (const Relleno& r : rellenos)
        {
            for (int c = 0; c < 4; c++)
            {
                coloresRellenosGPU.push_back(r.color[c]);
            }
        }


        // ENVIAR COLORES INDEPENDIENTES A LA GPU

        glActiveTexture(GL_TEXTURE4);

        glBindTexture(
            GL_TEXTURE_1D,
            coloresRellenosTexture
        );

        if (!coloresRellenosGPU.empty())
        {
            glTexImage1D(
                GL_TEXTURE_1D,
                0,
                GL_RGBA32F,
                (GLsizei)rellenos.size(),
                0,
                GL_RGBA,
                GL_FLOAT,
                coloresRellenosGPU.data()
            );
        }
        else
        {
            float colorVacio[4] = {
                1.0f, 1.0f, 1.0f, 1.0f
            };

            glTexImage1D(
                GL_TEXTURE_1D,
                0,
                GL_RGBA32F,
                1,
                0,
                GL_RGBA,
                GL_FLOAT,
                colorVacio
            );
        }

        glUniform1i(
            coloresRellenosTextureID,
            4
        );


        // ENVIAR CANTIDAD DE CELDAS RELLENADAS

        glUniform1i(
            celdasRellenoCountID,
            (int)celdasRellenadas.size()
        );

        glActiveTexture(GL_TEXTURE0);// ENVIAR IDENTIFICADORES DE CADA CELDA RELLENADA

        glActiveTexture(GL_TEXTURE3);

        glBindTexture(
            GL_TEXTURE_1D,
            idsRellenoTexture
        );

        if (!idsRellenoPorCelda.empty())
        {
            glTexImage1D(
                GL_TEXTURE_1D,
                0,
                GL_R32I,
                (GLsizei)idsRellenoPorCelda.size(),
                0,
                GL_RED_INTEGER,
                GL_INT,
                idsRellenoPorCelda.data()
            );
        }
        else
        {
            int idVacio = -1;

            glTexImage1D(
                GL_TEXTURE_1D,
                0,
                GL_R32I,
                1,
                0,
                GL_RED_INTEGER,
                GL_INT,
                &idVacio
            );
        }

        glUniform1i(
            idsRellenoTextureID,
            3
        );


        // ENVIAR COLORES INDEPENDIENTES A LA GPU

        glActiveTexture(GL_TEXTURE4);

        glBindTexture(
            GL_TEXTURE_1D,
            coloresRellenosTexture
        );

        if (!coloresRellenosGPU.empty())
        {
            glTexImage1D(
                GL_TEXTURE_1D,
                0,
                GL_RGBA32F,
                (GLsizei)rellenos.size(),
                0,
                GL_RGBA,
                GL_FLOAT,
                coloresRellenosGPU.data()
            );
        }
        else
        {
            float colorVacio[4] = {
                1.0f, 1.0f, 1.0f, 1.0f
            };

            glTexImage1D(
                GL_TEXTURE_1D,
                0,
                GL_RGBA32F,
                1,
                0,
                GL_RGBA,
                GL_FLOAT,
                colorVacio
            );
        }

        glUniform1i(
            coloresRellenosTextureID,
            4
        );


        // ENVIAR CANTIDAD DE CELDAS RELLENADAS

        glUniform1i(
            celdasRellenoCountID,
            (int)celdasRellenadas.size()
        );

        glActiveTexture(GL_TEXTURE0);

        glActiveTexture(GL_TEXTURE0);

        // ENVIAR ZOOM

        glUniform1f(
            zoomID,
            zoom
        );


        // ENVIAR DESPLAZAMIENTO

        glUniform1f(
            offsetXID,
            offsetX
        );

        glUniform1f(
            offsetYID,
            offsetY
        );


        // ENVIAR CELDAS DE BRESENHAM

        glUniform1iv(
            lineCellsXID,
            lineCellCount,
            lineCellsX
        );

        glUniform1iv(
            lineCellsYID,
            lineCellCount,
            lineCellsY
        );

        glUniform1i(
            lineCellCountID,
            lineCellCount
        );

        glUniform1iv(erasedCellsXID, erasedCellCount, erasedCellsX);
        glUniform1iv(erasedCellsYID, erasedCellCount, erasedCellsY);
        glUniform1i(erasedCellCountID, erasedCellCount);

        glUniform1iv(
            allCircleCenterXID,
            circleCount,
            allCircleCenterX
        );

        glUniform1iv(
            allCircleCenterYID,
            circleCount,
            allCircleCenterY
        );

        glUniform1iv(
            allCircleRadiusID,
            circleCount,
            allCircleRadius
        );

        glUniform1i(
            circleCountID,
            circleCount
        );

        // ENVIAR REGISTRO DE FIGURAS

        glUniform1iv(
            tiposFigurasID,
            figuraCount,
            (const int*)tiposFiguras
        );

        glUniform1iv(
            figuraInicioID,
            figuraCount,
            figuraInicio
        );

        glUniform1iv(
            figuraCantidadID,
            figuraCount,
            figuraCantidad
        );

        for (int i = 0; i < figuraCount; i++)
        {
            figuraBorradaGPU[i] = figuraBorrada[i] ? 1 : 0;
        }

        glUniform1iv(
            figuraBorradaID,
            figuraCount,
            figuraBorradaGPU
        );

        glUniform1i(
            figuraCountID,
            figuraCount
        );

        glUniform4fv(
            colorDibujoID,
            1,
            colorDibujo
        );

        // DIBUJAR


        VAO1.Bind();

        glDrawElements(
            GL_TRIANGLES,
            6,
            GL_UNSIGNED_INT,
            0
        );

        // DIBUJAR IMGUI

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(
            ImGui::GetDrawData()
        );

        // CAMBIAR BUFFER

        glfwSwapBuffers(window);


        // EVENTOS

        glfwPollEvents();
    }


    // LIMPIAR

    VAO1.Delete();

    VBO1.Delete();

    EBO1.Delete();

    shaderProgram.Delete();


    glfwDestroyWindow(window);

    glfwTerminate();


    return 0;
}