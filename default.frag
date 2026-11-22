#version 330 core

out vec4 FragColor;
in vec2 position;

uniform float zoom;
uniform float offsetX;
uniform float offsetY;
uniform vec4 colorDibujo;

// CELDAS DE LAS LINEAS
uniform int lineCellsX[1024];
uniform int lineCellsY[1024];
uniform int lineCellCount;

// CELDAS BORRADAS
#define MAX_ERASED_CELLS 256

uniform int erasedCellsX[MAX_ERASED_CELLS];
uniform int erasedCellsY[MAX_ERASED_CELLS];
uniform int erasedCellCount;

// CIRCULOS
#define MAX_CIRCLES 100

uniform int allCircleCenterX[MAX_CIRCLES];
uniform int allCircleCenterY[MAX_CIRCLES];
uniform int allCircleRadius[MAX_CIRCLES];
uniform int circleCount;

// REGISTRO DE FIGURAS
#define MAX_FIGURAS 200

uniform int tiposFiguras[MAX_FIGURAS];
uniform int figuraInicio[MAX_FIGURAS];
uniform int figuraCantidad[MAX_FIGURAS];
uniform int figuraBorrada[MAX_FIGURAS];
uniform int figuraCount;

// COLOR INDIVIDUAL DE CADA FIGURA
uniform sampler1D coloresFigurasTexture;

// CELDAS DEL CUBITO DE RELLENO
uniform isampler1D celdasRellenoTexture;
uniform int celdasRellenoCount;
// IDENTIFICADOR DEL RELLENO DE CADA CELDA
uniform isampler1D idsRellenoTexture;

// COLORES INDEPENDIENTES DE CADA RELLENO
uniform sampler1D coloresRellenosTexture;

void main()
{
    vec2 screenPosition = (position + 1.0) / 2.0;

    screenPosition.x += offsetX / 800.0;
    screenPosition.y -= offsetY / 800.0;

    vec2 gridPosition = floor(screenPosition * zoom);

    int currentX = int(gridPosition.x);
    int currentY = int(gridPosition.y);

    bool isLineCell = false;
    bool isCircleCell = false;
    int figuraColorIndice = -1;

    int fillCellIndex = -1;

// BUSCAR SI LA CELDA PERTENECE AL RELLENO
for (int i = 0; i < celdasRellenoCount; i++)
{
    ivec2 celda = texelFetch(
        celdasRellenoTexture,
        i,
        0
    ).rg;

    if (currentX == celda.x &&
        currentY == celda.y)
    {
        fillCellIndex = i;
        break;
    }
}

    // COMPROBAR FIGURAS QUE NO HAN SIDO BORRADAS
    for (int f = 0; f < MAX_FIGURAS; f++)
    {
        if (f >= figuraCount)
            break;

        if (figuraBorrada[f] == 1)
            continue;

        // FIGURA TIPO 0: LINEA
        if (tiposFiguras[f] != 1)
        {
            int inicio = figuraInicio[f];
            int cantidad = figuraCantidad[f];

            for (int i = 0; i < 1024; i++)
            {
                if (i >= cantidad)
                    break;

                int indice = inicio + i;

                if (indice >= lineCellCount)
                    break;

                if (currentX == lineCellsX[indice] &&
                    currentY == lineCellsY[indice])
                {
                   isLineCell = true;
                   figuraColorIndice = f;
                   break;
                }
            }
        }

        // FIGURA TIPO 1: CIRCULO
        else if (tiposFiguras[f] == 1)
        {
            int c = figuraInicio[f];

            if (c < circleCount)
            {
                int centerX = allCircleCenterX[c];
                int centerY = allCircleCenterY[c];
                int radius = allCircleRadius[c];

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
                       isCircleCell = true;
                        figuraColorIndice = f;
                        break;
                    }

                    // FACTOR DE DECISION DE BRESENHAM
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
        }
    }

    // COMPROBAR SI LA CELDA FUE BORRADA
    bool isErasedCell = false;

    for (int i = 0; i < MAX_ERASED_CELLS; i++)
    {
        if (i >= erasedCellCount)
            break;

        if (currentX == erasedCellsX[i] &&
            currentY == erasedCellsY[i])
        {
            isErasedCell = true;
            break;
        }
    }

    // COLORES
    if (isErasedCell)
    {
        float checker = mod(
            gridPosition.x + gridPosition.y,
            2.0
        );

        if (checker == 0.0)
            FragColor = vec4(0.85, 0.85, 0.85, 1.0);
        else
            FragColor = vec4(0.65, 0.65, 0.65, 1.0);
    }
    else if (figuraColorIndice >= 0)
    {
    FragColor = texelFetch(
        coloresFigurasTexture,
        figuraColorIndice,
        0
        );
    }
else if (fillCellIndex >= 0)
{
    int idRelleno = texelFetch(
        idsRellenoTexture,
        fillCellIndex,
        0
    ).r;

    FragColor = texelFetch(
        coloresRellenosTexture,
        idRelleno,
        0
    );
}
    else
    {
        float checker = mod(
            gridPosition.x + gridPosition.y,
            2.0
        );

        if (checker == 0.0)
            FragColor = vec4(0.85, 0.85, 0.85, 1.0);
        else
            FragColor = vec4(0.65, 0.65, 0.65, 1.0);
    }
}
