Nombre del proyecto:PAINT EN VISUAL STUDIO(OPENGL Y C++)

Estudiante:Sebastian Arias

Código estudiantil:6014464

Asignatura:Introducción a la Computación Gráfica

Entorno de desarrollo:Visual Studio

Lenguaje:C++

Tecnología gráfica:OpenGL 3.3


Descripción del proyecto

PAINT es una aplicación de dibujo desarrollada en C++ mediante OpenGL 3.3 y Visual Studio. Cuenta con una cuadrícula interactiva y diferentes herramientas que permiten crear, colorear, transformar y eliminar figuras geométricas.

El proyecto implementa algoritmos de rasterización, transformaciones geométricas y procesamiento gráfico mediante shaders, integrando una interfaz gráfica desarrollada con Dear ImGui.


 Funcionalidades


 Herramientas de dibujo

Línea de Bresenham: dibuja líneas entre dos puntos de la cuadrícula.

Círculo de Bresenham: construye circunferencias mediante el algoritmo de Bresenham.

Forma libre: permite realizar trazos continuos mediante el movimiento del mouse.

Triángulo: construye figuras triangulares.

Cuadrado: crea figuras cuadradas.

Rectángulo: construye figuras rectangulares.


Herramientas de edición

Selector de color: permite seleccionar el color de los dibujos.

Color de relleno: establece el color aplicado al interior de las figuras.

Relleno de figuras: colorea las áreas interiores de las figuras.

Borrador por celda: elimina celdas específicas del dibujo.

Borrador de figura: elimina figuras completas, incluyendo sus rellenos.

Limpiar grilla: elimina los elementos dibujados y restablece el área de trabajo.


Transformaciones geométricas

Rotación: modifica la orientación de las figuras.

Traslación: cambia la posición de las figuras dentro de la cuadrícula.


 Navegación por la cuadrícula

Zoom: permite acercar o alejar la vista mediante la rueda del mouse.

Desplazamiento: permite mover la cuadrícula para explorar diferentes zonas del área de dibujo.


Tecnologías utilizadas

C++: lenguaje de programación principal.

Visual Studio: entorno de desarrollo.

OpenGL 3.3: biblioteca gráfica para la representación de elementos.

GLFW: creación de ventanas y administración de eventos.

GLAD: carga de funciones de OpenGL.

Dear ImGui: desarrollo de la interfaz gráfica.

GLSL: lenguaje utilizado para desarrollar los shaders.


 Arquitectura del proyecto

El proyecto PAINT está organizado mediante diferentes componentes que separan la lógica principal de la aplicación, la administración de recursos gráficos, el procesamiento de shaders y la interfaz gráfica.

1. Archivo principal – Main.cpp

Es el núcleo de la aplicación. Contiene la lógica general de PAINT y coordina las herramientas de dibujo, las operaciones geométricas, la interfaz gráfica y el proceso de renderizado.

Entre sus principales responsabilidades se encuentran:

Inicialización de GLFW, GLAD y OpenGL.

Creación y administración de la ventana.

Implementación de los algoritmos de Bresenham para líneas y círculos.

Creación de triángulos, cuadrados y rectángulos.

Dibujo de forma libre.

Administración de colores y rellenos.

Operaciones de rotación y traslación.

Herramientas de borrado.

Administración de eventos del mouse.

Integración de la interfaz Dear ImGui.

2. Módulo de shaders

Se encuentra dentro de la carpeta Shaders y está compuesto por:

default.vert: shader de vértices encargado del procesamiento de la información geométrica.

default.frag: shader de fragmentos encargado del procesamiento de los colores y la representación de los elementos gráficos.

Estos archivos trabajan conjuntamente para generar la representación visual mediante OpenGL.

3. Módulo de administración de shaders

Está compuesto por:

shaderclass.h

shaderClass.cpp

Se encarga de cargar, compilar y administrar los programas de shaders utilizados en la aplicación.

4. Módulo de buffers gráficos

Está compuesto por los siguientes archivos:

VAO (Vertex Array Object)

VAO.h

VAO.cpp

Administra la configuración de los atributos de vértices.

VBO (Vertex Buffer Object)

VBO.h

VBO.cpp

Administra los buffers que almacenan información de vértices.

EBO (Element Buffer Object)

EBO.h

EBO.cpp

Administra los índices utilizados para definir cómo se conectan los vértices durante la representación de las primitivas gráficas.

5. Módulo de interfaz gráfica – Dear ImGui

Está integrado mediante archivos como:

imgui.cpp

imgui_draw.cpp

imgui_impl_glfw.cpp

imgui_impl_opengl3.cpp

imgui_tables.cpp

imgui_widgets.cpp

También utiliza archivos de encabezado y dependencias internas de Dear ImGui.

Este módulo proporciona los componentes visuales de la aplicación, incluyendo el panel de herramientas, los selectores de color y los controles de dibujo.

6. Módulo de inicialización gráfica – GLAD

Está compuesto por glad.c y sus archivos de encabezado correspondientes.

Se encarga de cargar las funciones de OpenGL necesarias para ejecutar las operaciones gráficas.


Funcionamiento general de la arquitectura

El funcionamiento de PAINT se desarrolla mediante las siguientes etapas:

Inicialización: se configuran GLFW, GLAD y OpenGL, y se crea la ventana principal.

Carga gráfica: se cargan los shaders y se preparan los buffers gráficos.

Interfaz: Dear ImGui proporciona los controles y herramientas de dibujo.

Procesamiento: se ejecutan los algoritmos de dibujo, relleno y transformación geométrica.

Actualización: se modifican los datos de las figuras, sus posiciones y sus colores.

Renderizado: OpenGL procesa la información mediante la GPU y los shaders para representar los elementos en pantalla.


Algoritmos implementados

Algoritmo de Bresenham para líneas

Calcula las celdas que deben activarse para representar una línea entre dos puntos de la cuadrícula, utilizando operaciones con números enteros.

Algoritmo de Bresenham para círculos

Calcula los puntos de una circunferencia aprovechando la simetría de sus coordenadas.

Relleno de figuras

Permite colorear las regiones interiores de las figuras y conservar la relación entre cada relleno y su figura correspondiente.


Objetivo académico

Desarrollar una aplicación de dibujo que permita aplicar los fundamentos de la Introducción a la Computación Gráfica mediante algoritmos de rasterización, transformaciones geométricas, buffers, shaders y herramientas de representación gráfica.


Autor

Sebastian Arias

Código estudiantil: 6014464

Asignatura: Introducción a la Computación Gráfica
