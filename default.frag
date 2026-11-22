#version 330 core

// Color final que se mostrara en cada fragmento de la pantalla.
out vec4 FragColor;

// Posicion del fragmento que se esta procesando.
in vec2 position;

// Variables que permiten controlar el zoom y el desplazamiento.
uniform float zoom;
uniform float offsetX;
uniform float offsetY;

// Color general del dibujo.
uniform vec4 colorDibujo;


// =====================================================
// CELDAS DE LAS LINEAS
// =====================================================

// Arreglos que almacenan las coordenadas X y Y
// de las celdas que forman las lineas dibujadas.
uniform int lineCellsX[1024];
uniform int lineCellsY[1024];

// Cantidad total de celdas de las lineas.
uniform int lineCellCount;


// =====================================================
// CELDAS BORRADAS
// =====================================================

// Cantidad maxima de celdas borradas que se pueden almacenar.
#define MAX_ERASED_CELLS 256

// Arreglos que guardan las coordenadas de las celdas borradas.
uniform int erasedCellsX[MAX_ERASED_CELLS];
uniform int erasedCellsY[MAX_ERASED_CELLS];

// Cantidad actual de celdas borradas.
uniform int erasedCellCount;


// =====================================================
// INFORMACION DE LOS CIRCULOS
// =====================================================

// Cantidad maxima de circulos que se pueden registrar.
#define MAX_CIRCLES 100

// Coordenadas X y Y del centro de cada circulo.
uniform int allCircleCenterX[MAX_CIRCLES];
uniform int allCircleCenterY[MAX_CIRCLES];

// Radio de cada circulo.
uniform int allCircleRadius[MAX_CIRCLES];

// Cantidad total de circulos registrados.
uniform int circleCount;


// =====================================================
// REGISTRO DE FIGURAS
// =====================================================

// Cantidad maxima de figuras que puede manejar el programa.
#define MAX_FIGURAS 200

// Arreglo que almacena el tipo de cada figura.
uniform int tiposFiguras[MAX_FIGURAS];

// Indica la posicion inicial de cada figura en el arreglo de celdas.
uniform int figuraInicio[MAX_FIGURAS];

// Indica la cantidad de celdas que pertenecen a cada figura.
uniform int figuraCantidad[MAX_FIGURAS];

// Indica si una figura fue borrada.
uniform int figuraBorrada[MAX_FIGURAS];

// Cantidad total de figuras registradas.
uniform int figuraCount;


// =====================================================
// COLOR INDIVIDUAL DE CADA FIGURA
// =====================================================

// Textura que almacena el color correspondiente a cada figura.
uniform sampler1D coloresFigurasTexture;


// =====================================================
// CELDAS DEL CUBITO DE RELLENO
// =====================================================

// Textura de enteros que almacena las coordenadas
// de las celdas que pertenecen a los rellenos.
uniform isampler1D celdasRellenoTexture;

// Cantidad de celdas que tienen relleno.
uniform int celdasRellenoCount;


// IDENTIFICADOR DEL RELLENO DE CADA CELDA

// Textura que permite identificar a que relleno pertenece cada celda.
uniform isampler1D idsRellenoTexture;


// COLORES INDEPENDIENTES DE CADA RELLENO

// Textura que almacena los diferentes colores de los rellenos.
uniform sampler1D coloresRellenosTexture;


void main()
{
    // Convertimos la posicion del fragmento del rango -1 a 1
    // al rango de coordenadas de 0 a 1.
    vec2 screenPosition = (position + 1.0) / 2.0;

    // Aplicamos el desplazamiento horizontal y vertical
    // para mover la cuadricula.
    screenPosition.x += offsetX / 800.0;
    screenPosition.y -= offsetY / 800.0;

    // Aplicamos el zoom y utilizamos floor para obtener
    // las coordenadas enteras de la celda actual.
    vec2 gridPosition = floor(screenPosition * zoom);

    // Convertimos las coordenadas de la cuadricula a enteros.
    int currentX = int(gridPosition.x);
    int currentY = int(gridPosition.y);

    // Variables que indican si la celda pertenece a una linea o circulo.
    bool isLineCell = false;
    bool isCircleCell = false;

    // Almacena el indice de la figura encontrada.
    // El valor -1 indica que todavia no se encontro una figura.
    int figuraColorIndice = -1;

    // Almacena la posicion de la celda dentro del arreglo de rellenos.
    int fillCellIndex = -1;


// =====================================================
// BUSCAR SI LA CELDA PERTENECE AL RELLENO
// =====================================================

// Recorremos todas las celdas que tienen relleno.
for (int i = 0; i < celdasRellenoCount; i++)
{
    // Recuperamos las coordenadas de una celda desde la textura.
    ivec2 celda = texelFetch(
        celdasRellenoTexture,
        i,
        0
    ).rg;

    // Comprobamos si las coordenadas de la celda actual
    // coinciden con las coordenadas almacenadas.
    if (currentX == celda.x &&
        currentY == celda.y)
    {
        // Guardamos la posicion donde encontramos la coincidencia.
        fillCellIndex = i;

        // Terminamos la busqueda.
        break;
    }
}

    // =================================================
    // COMPROBAR FIGURAS QUE NO HAN SIDO BORRADAS
    // =================================================

    // Recorremos el registro de figuras.
    for (int f = 0; f < MAX_FIGURAS; f++)
    {
        // Si llegamos a la cantidad total de figuras,
        // terminamos el recorrido.
        if (f >= figuraCount)
            break;

        // Si la figura fue borrada, pasamos a la siguiente.
        if (figuraBorrada[f] == 1)
            continue;

        // =================================================
        // FIGURA TIPO 0: LINEA
        // =================================================

        // Si el tipo de figura no es 1, comprobamos sus celdas.
        if (tiposFiguras[f] != 1)
        {
            // Obtenemos la posicion inicial de las celdas de la figura.
            int inicio = figuraInicio[f];

            // Obtenemos la cantidad de celdas que forman la figura.
            int cantidad = figuraCantidad[f];

            // Recorremos las celdas de la figura.
            for (int i = 0; i < 1024; i++)
            {
                // Si superamos la cantidad de celdas de la figura,
                // terminamos este recorrido.
                if (i >= cantidad)
                    break;

                // Calculamos el indice de la celda dentro del arreglo general.
                int indice = inicio + i;

                // Comprobamos que el indice no supere
                // la cantidad total de celdas almacenadas.
                if (indice >= lineCellCount)
                    break;

                // Comparamos las coordenadas actuales con las coordenadas
                // de las celdas que forman la figura.
                if (currentX == lineCellsX[indice] &&
                    currentY == lineCellsY[indice])
                {
                   // Indicamos que encontramos una celda de una figura.
                   isLineCell = true;

                   // Guardamos el indice de la figura para obtener su color.
                   figuraColorIndice = f;

                   // Terminamos la busqueda de esta figura.
                   break;
                }
            }
        }

        // =================================================
        // FIGURA TIPO 1: CIRCULO
        // =================================================

        // Si el tipo de figura es 1, comprobamos si pertenece a un circulo.
        else if (tiposFiguras[f] == 1)
        {
            // Obtenemos el indice del circulo registrado.
            int c = figuraInicio[f];

            // Verificamos que el circulo exista.
            if (c < circleCount)
            {
                // Recuperamos las coordenadas del centro y el radio.
                int centerX = allCircleCenterX[c];
                int centerY = allCircleCenterY[c];
                int radius = allCircleRadius[c];

                // Variables iniciales utilizadas por Bresenham.
                int x = 0;
                int y = radius;
                int p = 3 - 2 * radius;

                // Repetimos el algoritmo mientras x no supere a y.
                while (x <= y)
                {
                    // Comprobamos los ocho puntos simetricos del circulo.
                    // Esto permite aprovechar la simetria de la circunferencia.
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
                       // Indicamos que encontramos una celda del circulo.
                       isCircleCell = true;

                       // Guardamos el indice de la figura para consultar su color.
                       figuraColorIndice = f;

                       // Terminamos la busqueda.
                       break;
                    }

                    // FACTOR DE DECISION DE BRESENHAM

                    // Si el parametro de decision es negativo,
                    // actualizamos el valor de p sin disminuir y.
                    if (p < 0)
                    {
                        p = p + 4 * x + 6;
                    }
                    else
                    {
                        // Si el parametro es positivo o igual a cero,
                        // actualizamos p y disminuimos y.
                        p = p + 4 * (x - y) + 10;
                        y--;
                    }

                    // Incrementamos x para continuar el recorrido.
                    x++;
                }
            }
        }
    }


    // COMPROBAR SI LA CELDA FUE BORRADA
   

    // Inicialmente indicamos que la celda no ha sido borrada.
    bool isErasedCell = false;

    // Recorremos el arreglo de celdas borradas.
    for (int i = 0; i < MAX_ERASED_CELLS; i++)
    {
        // Si ya superamos la cantidad de celdas borradas,
        // terminamos el recorrido.
        if (i >= erasedCellCount)
            break;

        // Comparamos las coordenadas actuales con las coordenadas
        // de las celdas que fueron borradas.
        if (currentX == erasedCellsX[i] &&
            currentY == erasedCellsY[i])
        {
            // Confirmamos que la celda fue borrada.
            isErasedCell = true;

            // Terminamos la busqueda.
            break;
        }
    }


    // COLORES
   

    // PRIMERA CONDICION: COMPROBAR SI LA CELDA FUE BORRADA.
    if (isErasedCell)
    {
        // Calculamos un patron de cuadros alternados
        // utilizando la suma de las coordenadas.
        float checker = mod(
            gridPosition.x + gridPosition.y,
            2.0
        );

        // Si el resultado es cero, utilizamos el primer tono de gris.
        if (checker == 0.0)
            FragColor = vec4(0.85, 0.85, 0.85, 1.0);
        // En caso contrario, utilizamos el segundo tono de gris.
        else
            FragColor = vec4(0.65, 0.65, 0.65, 1.0);
    }

    // SEGUNDA CONDICION: COMPROBAR SI PERTENECE A UNA FIGURA.
    else if (figuraColorIndice >= 0)
    {
        // Consultamos la textura de colores para obtener
        // el color individual de la figura encontrada.
    FragColor = texelFetch(
        coloresFigurasTexture,
        figuraColorIndice,
        0
        );
    }

    // TERCERA CONDICION: COMPROBAR SI PERTENECE A UN RELLENO.
else if (fillCellIndex >= 0)
{
    // Obtenemos el identificador del relleno al que pertenece la celda.
    int idRelleno = texelFetch(
        idsRellenoTexture,
        fillCellIndex,
        0
    ).r;

    // Consultamos la textura de colores de relleno
    // para obtener el color correspondiente.
    FragColor = texelFetch(
        coloresRellenosTexture,
        idRelleno,
        0
    );
}

    // CUARTA CONDICION: SI NO PERTENECE A NINGUNO,
    // SE MUESTRA EL FONDO DE LA CUADRICULA.
    else
    {
        // Calculamos el patron de cuadros alternados.
        float checker = mod(
            gridPosition.x + gridPosition.y,
            2.0
        );

        // Alternamos entre dos tonos de gris para formar la cuadricula.
        if (checker == 0.0)
            FragColor = vec4(0.85, 0.85, 0.85, 1.0);
        else
            FragColor = vec4(0.65, 0.65, 0.65, 1.0);
    }
}