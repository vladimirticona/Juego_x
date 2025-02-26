#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <windows.h>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <algorithm>  // Para sort()
#include <queue>      // Para colas
#include <stack>      // Para pilas

using namespace std;
using namespace sf;

// Función para obtener los archivos de imagen (frames) desde una carpeta
vector<string> getFramesFromFolder(const string &folder) {
    vector<string> frameFiles; // Vector para almacenar los nombres de los archivos de los frames del video o animación
    WIN32_FIND_DATA fileData; // Estructura de datos de Windows para almacenar información sobre archivos encontrados
    HANDLE hFind; // Manejador para la búsqueda de archivos en el sistema de archivos de Windows

    // Buscar archivos PNG en la carpeta
    string searchPath = folder + "\\*.png";  //Se recolectaran todos los de extension .png
    hFind = FindFirstFile(searchPath.c_str(), &fileData); // Inicia la búsqueda de archivos en el directorio especificado por `searchPath`

    if (hFind != INVALID_HANDLE_VALUE) { // Verifica si se encontró al menos un archivo
        do {
            // Agregar el archivo al vector
            frameFiles.push_back(folder + "\\" + fileData.cFileName);
        } while (FindNextFile(hFind, &fileData)); // Continúa buscando más archivos en el directorio
        FindClose(hFind);
    }

    // Ordenar los archivos por nombre (opcional, pero útil si los frames tienen nombres secuenciales)
    sort(frameFiles.begin(), frameFiles.end());

    return frameFiles;
}

// Estructura para los recursos de una casilla
struct Casilla {
    int plata;
    int oro;
    int diamante;
};

// Estructura para los recursos del jugador
struct Recurso {
    string tipo;  // "plata", "oro", "diamante"
    int cantidad;
    Recurso *sig;  // Puntero al siguiente recurso
};

// Estructura para los logros
struct NodoLogro {
    string nombre;
    bool desbloqueado;
    NodoLogro *izq;
    NodoLogro *der;
    // Constructor de la estructura NodoLogro
    NodoLogro(string nombre) : nombre(nombre), desbloqueado(false), izq(nullptr), der(nullptr) {}
};

// Tamaño del tablero
const int FILAS = 4;
const int COLUMNAS = 4;
Casilla tablero[FILAS][COLUMNAS];

int calcular_puntos_casilla(int i, int j);
// Inicializar el tablero
void inicializar_tablero() {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            if (i == 0 && j == 0) {
                // Esta casilla representa al jugador (0 puntos)
                tablero[i][j].plata = 0;
                tablero[i][j].oro = 0;
                tablero[i][j].diamante = 0;
            } else {
                // Asignar recursos según la fila
                if (i == 0) {
                    // Fila 0: entre 500 y 800 puntos
                    int puntos = rand() % 301 + 500;  // Entre 500 y 800
                    tablero[i][j].plata = 0;
                    tablero[i][j].oro = 0;
                    tablero[i][j].diamante = puntos / 100;  // 1 diamante = 100 puntos
                } else if (i == 1) {
                    // Fila 1: entre 2000 y 2500 puntos
                    int puntos = rand() % 501 + 2000;  // Entre 2000 y 2500
                    tablero[i][j].plata = 0;
                    tablero[i][j].oro = 0;
                    tablero[i][j].diamante = puntos / 100;  
                } else if (i == 2) {
                    // Fila 2: entre 10000 y 20000 puntos
                    int puntos = rand() % 10001 + 10000;  // Entre 10000 y 20000
                    tablero[i][j].plata = 0;
                    tablero[i][j].oro = 0;
                    tablero[i][j].diamante = puntos / 100;  
                } else if (i == 3) {
                    // Fila 3: entre 1000 y 1500 puntos
                    int puntos = rand() % 501 + 1000;  // Entre 1000 y 1500
                    tablero[i][j].plata = 0;
                    tablero[i][j].oro = 0;
                    tablero[i][j].diamante = puntos / 100;  
                }
            }
        }
    }
}

// Calcular puntos de una casilla
int calcular_puntos_casilla(int i, int j) {
    return tablero[i][j].plata * 10 +
           tablero[i][j].oro * 50 +
           tablero[i][j].diamante * 100;
}

// Función para farmear
void farmear(Recurso *&cabeza_mis_recursos, string &mensaje) {
    mensaje = "Recolectando materiales...";
    Sleep(1000);  // Espera 1 segundo (simulación)

    // Generar un recurso aleatorio
    string tipos[3] = {"plata", "oro", "diamante"};
    string tipo_aleatorio = tipos[rand() % 3];  // Elige un tipo al azar

    // Generar una cantidad aleatoria
    int cantidad_aleatoria = rand() % 5 + 1;  // Entre 1 y 5

    // Agregar el recurso a la lista del jugador
    Recurso *nuevo_recurso = new Recurso();
    nuevo_recurso->tipo = tipo_aleatorio;
    nuevo_recurso->cantidad = cantidad_aleatoria;
    nuevo_recurso->sig = cabeza_mis_recursos;
    cabeza_mis_recursos = nuevo_recurso;

    // Actualizar la casilla [0][0] del tablero
    if (tipo_aleatorio == "plata") {
        tablero[0][0].plata += cantidad_aleatoria;
    } else if (tipo_aleatorio == "oro") {
        tablero[0][0].oro += cantidad_aleatoria;
    } else if (tipo_aleatorio == "diamante") {
        tablero[0][0].diamante += cantidad_aleatoria;
    }

    mensaje = "¡Has recolectado " + to_string(cantidad_aleatoria) + " de " + tipo_aleatorio + "!";
}

// Función para atacar
void atacar(Recurso *&cabeza_mis_recursos, int x, int y, string &mensaje, RectangleShape tableroGrafico[FILAS][COLUMNAS]) {
    // Verificar si las coordenadas son válidas
    if (x < 0 || x >= FILAS || y < 0 || y >= COLUMNAS) {
        mensaje = "Coordenadas inválidas.";
        return;
    }

    // Calcular puntos del jugador
    int mis_puntos = 0;
    Recurso *q = cabeza_mis_recursos;
    while (q != NULL) {
        if (q->tipo == "plata") {
            mis_puntos += q->cantidad * 10;
        } else if (q->tipo == "oro") {
            mis_puntos += q->cantidad * 50;
        } else if (q->tipo == "diamante") {
            mis_puntos += q->cantidad * 100;
        }
        q = q->sig;
    }

    // Calcular puntos de la casilla objetivo
    int puntos_enemigo = calcular_puntos_casilla(x, y);

    // Comparar puntos
    if (mis_puntos > puntos_enemigo) {
        mensaje = "¡Ataque exitoso! La casilla ahora es tuya.\n";
        mensaje += "Materiales obtenidos:\n";
        mensaje += "Plata: " + to_string(tablero[x][y].plata) + "\n";
        mensaje += "Oro: " + to_string(tablero[x][y].oro) + "\n";
        mensaje += "Diamante: " + to_string(tablero[x][y].diamante) + "\n";

        // Sumar los recursos de la casilla objetivo a los del jugador
        if (tablero[x][y].plata > 0) {
            Recurso *nuevo_recurso = new Recurso();
            nuevo_recurso->tipo = "plata";
            nuevo_recurso->cantidad = tablero[x][y].plata;
            nuevo_recurso->sig = cabeza_mis_recursos;
            cabeza_mis_recursos = nuevo_recurso;
            tablero[0][0].plata += tablero[x][y].plata;  // Actualizar la casilla del jugador
        }
        if (tablero[x][y].oro > 0) {
            Recurso *nuevo_recurso = new Recurso();
            nuevo_recurso->tipo = "oro";
            nuevo_recurso->cantidad = tablero[x][y].oro;
            nuevo_recurso->sig = cabeza_mis_recursos;
            cabeza_mis_recursos = nuevo_recurso;
            tablero[0][0].oro += tablero[x][y].oro;  // Actualizar la casilla del jugador
        }
        if (tablero[x][y].diamante > 0) {
            Recurso *nuevo_recurso = new Recurso();
            nuevo_recurso->tipo = "diamante";
            nuevo_recurso->cantidad = tablero[x][y].diamante;
            nuevo_recurso->sig = cabeza_mis_recursos;
            cabeza_mis_recursos = nuevo_recurso;
            tablero[0][0].diamante += tablero[x][y].diamante;  // Actualizar la casilla del jugador
        }

        // Reiniciar los recursos de la casilla objetivo
        tablero[x][y].plata = 0;
        tablero[x][y].oro = 0;
        tablero[x][y].diamante = 0;

        // Pintar la casilla de rojo solo si el ataque fue exitoso
        tableroGrafico[x][y].setFillColor(Color::Red);
    } else {
        mensaje = "Ataque fallido. No tienes suficientes recursos.";
    }
}

// Función para mostrar los recursos del jugador
string mostrar_mis_recursos(Recurso *cabeza_mis_recursos) {
    Recurso *q = cabeza_mis_recursos; //Creamos un puntero para que recorra toda la lista
    int puntos_totales = 0;
    string recursos_str = "Tus recursos son:\n";

    while (q != NULL) {
        recursos_str += q->tipo + ": " + to_string(q->cantidad) + "\n";
        if (q->tipo == "plata") {
            puntos_totales += q->cantidad * 10;
        } else if (q->tipo == "oro") {
            puntos_totales += q->cantidad * 50;
        } else if (q->tipo == "diamante") {
            puntos_totales += q->cantidad * 100;
        }
        q = q->sig;
    }

    recursos_str += "Puntos totales: " + to_string(puntos_totales) + "\n";
    return recursos_str;
}

// Función para verificar si todas las casillas están pintadas de rojo
bool todas_las_casillas_rojas(RectangleShape tableroGrafico[FILAS][COLUMNAS]) {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            if (tableroGrafico[i][j].getFillColor() != Color::Red) {
                return false;
            }
        }
    }
    return true;
}

// Construir el árbol de logros
NodoLogro* construirArbolLogros() {
    NodoLogro* raiz = new NodoLogro("Conquista tu primera casilla");
    raiz->izq = new NodoLogro("Conquista 5 casillas");
    raiz->izq->izq = new NodoLogro("Conquista 10 casillas");
    raiz->izq->der = new NodoLogro("Conquista todas las casillas");
    raiz->der = new NodoLogro("Obtener 500 puntos");
    raiz->der->izq = new NodoLogro("Obtener 1500 puntos");
    raiz->der->der = new NodoLogro("Obtener 3000 puntos");

    return raiz;
}

// Actualizar el estado de los logros
void actualizarLogros(NodoLogro* raiz, int casillasConquistadas, int puntosTotales) {
    if (raiz == nullptr) return;

    if (raiz->nombre == "Conquista tu primera casilla" && casillasConquistadas >= 1) {
        raiz->desbloqueado = true;
    }
    if (raiz->nombre == "Conquista 5 casillas" && casillasConquistadas >= 5) {
        raiz->desbloqueado = true;
    }
    if (raiz->nombre == "Conquista 10 casillas" && casillasConquistadas >= 10) {
        raiz->desbloqueado = true;
    }
    if (raiz->nombre == "Conquista todas las casillas" && casillasConquistadas == FILAS * COLUMNAS - 1) {
        raiz->desbloqueado = true;
    }
    if (raiz->nombre == "Obtener 500 puntos" && puntosTotales >= 500) {
        raiz->desbloqueado = true;
    }
    if (raiz->nombre == "Obtener 1500 puntos" && puntosTotales >= 1500) {
        raiz->desbloqueado = true;
    }
    if (raiz->nombre == "Obtener 3000 puntos" && puntosTotales >= 3000) {
        raiz->desbloqueado = true;
    }

    actualizarLogros(raiz->izq, casillasConquistadas, puntosTotales); //Recursividad para poder recorrer todo el arbol
    actualizarLogros(raiz->der, casillasConquistadas, puntosTotales);
}

// Mostrar el árbol de logros en la interfaz gráfica
void mostrarArbolLogros(NodoLogro* raiz, RenderWindow& window, Font& font, int x, int& y) {
    if (raiz == nullptr) return;

    // Dibujar el nodo actual
    Text texto;
    texto.setFont(font);
    texto.setCharacterSize(20);  // Tamaño de la fuente
    texto.setFillColor(raiz->desbloqueado ? Color::Green : Color::Red);
    texto.setString((raiz->desbloqueado ? "[X] " : "[ ] ") + raiz->nombre);
    texto.setPosition(x, y);  // Posición vertical ajustada
    window.draw(texto);

    // Incrementar la posición vertical para el siguiente logro
    y += 30;  // Espaciado vertical de 30 píxeles entre logros

    // Dibujar los hijos (recursivamente)
    mostrarArbolLogros(raiz->izq, window, font, x + 20, y);  
    mostrarArbolLogros(raiz->der, window, font, x + 20, y);
}

// Estructura para el grafo
struct Grafo {
    int matrizAdyacencia[FILAS * COLUMNAS][FILAS * COLUMNAS];

    Grafo() {
        // Inicializar la matriz de adyacencia con ceros
        for (int i = 0; i < FILAS * COLUMNAS; i++) {
            for (int j = 0; j < FILAS * COLUMNAS; j++) {
                matrizAdyacencia[i][j] = 0;
            }
        }
    }

    void actualizarGrafo(RectangleShape tableroGrafico[FILAS][COLUMNAS], Recurso* cabeza_mis_recursos) {
        // Calcular los puntos del jugador
        int puntosJugador = 0;
        Recurso* q = cabeza_mis_recursos;
        while (q != NULL) {
            if (q->tipo == "plata") {
                puntosJugador += q->cantidad * 10;
            } else if (q->tipo == "oro") {
                puntosJugador += q->cantidad * 50;
            } else if (q->tipo == "diamante") {
                puntosJugador += q->cantidad * 100;
            }
            q = q->sig;
        }

        // Actualizar la matriz de adyacencia
        for (int i = 0; i < FILAS; i++) { //Itera sobre las filas del tablero
            for (int j = 0; j < COLUMNAS; j++) { //Itera sobre las columnas del tablero
                int indiceActual = i * COLUMNAS + j; //Calcula el indice actual de la casilla en forma lineal
                for (int x = 0; x < FILAS; x++) { //Comparar la casilla actual con todas las demas casillas del tablero
                    for (int y = 0; y < COLUMNAS; y++) {
                        int indiceVecino = x * COLUMNAS + y; //Calcula el indice del vecino en forma lineal
                        if (indiceActual != indiceVecino) {
                            int puntosVecino = calcular_puntos_casilla(x, y);
                            int diferencia = puntosJugador - puntosVecino;
                            matrizAdyacencia[indiceActual][indiceVecino] = diferencia; // Almacena la diferencia en la matriz de adyacencia
                        }
                    }
                }
            }
        }
    }

    string mostrarCaminosEstrategicos(RectangleShape tableroGrafico[FILAS][COLUMNAS]) {
        stringstream ss; //Nos servira para construir cadenas de texto de manera dinamica
        ss << "Aristas desde la casilla [0, 0]:\n";
        int indiceJugador = 0;  // Casilla [0, 0]

        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                int indiceVecino = i * COLUMNAS + j;
                if (indiceJugador != indiceVecino) { // Evitar comparar la casilla de inicio consigo misma
                    int diferencia = matrizAdyacencia[indiceJugador][indiceVecino]; // Obtener la diferencia de puntos desde la casilla [0,0] a la actual
                    ss << " -> Casilla [" << i << ", " << j << "] (Diferencia: " << diferencia << ") ";
                    if (tableroGrafico[i][j].getFillColor() == Color::Red) { // Verificar el estado de la casilla según su color en el tablero
                        ss << "(Casilla ya conquistada)\n";
                    } else if (diferencia > 0) {
                        ss << "(Puedes conquistar)\n";
                    } else {
                        ss << "(No puedes conquistar)\n";
                    }
                }
            }
        }

        return ss.str(); // Retornar el texto generado con los caminos estratégicos
    }
};

// Función para mostrar la matriz de adyacencia en una nueva ventana
void mostrarMatrizAdyacencia(Grafo& grafo, RectangleShape tableroGrafico[FILAS][COLUMNAS], Font& font) {
    RenderWindow matrizWindow(VideoMode(800, 600), "Matriz de Adyacencia"); //Tamaño de la ventana
    Text texto;
    texto.setFont(font);
    texto.setCharacterSize(20); //Tamaño del texto
    texto.setFillColor(Color::White); //Color del texto

    while (matrizWindow.isOpen()) {
        Event event;
        while (matrizWindow.pollEvent(event)) { //Manejo de eventos en la ventana
            if (event.type == Event::Closed) //Si el usuario cierra la ventana
                matrizWindow.close(); //Cerrar ventana
        }

        matrizWindow.clear();
        //Construir el texto con la matriz de adyacencia
        stringstream ss;
        ss << grafo.mostrarCaminosEstrategicos(tableroGrafico); //Obtener la informacion del grafo
        texto.setString(ss.str()); //Asigna el texto generado al objeto de texto
        matrizWindow.draw(texto); //Dibujar el texto en la ventana
        matrizWindow.display(); //Mostrar el contenido actualizado en la ventana
    }
}

// Estructura para guardar el estado de una casilla antes de un ataque
struct EstadoCasilla {
    int x, y; // Coordenadas de la casilla
    int plata, oro, diamante; // Recursos antes del ataque
    Color colorOriginal; // Color original de la casilla
    int plataJugador, oroJugador, diamanteJugador; // Recursos del jugador antes del ataque
};

// Pila para deshacer accion
stack<EstadoCasilla> pilaDeshacer;

// Cola para eventos aleatorios
queue<string> colaEventos;

// Función para deshacer la última acción
void deshacerAccion(RectangleShape tableroGrafico[FILAS][COLUMNAS], Recurso*& cabeza_mis_recursos, string &mensaje) {
    if (!pilaDeshacer.empty()) {
        EstadoCasilla estado = pilaDeshacer.top(); // Obtener el último estado
        pilaDeshacer.pop(); // Eliminar el estado de la pila

        // Restaurar los recursos de la casilla enemiga
        tablero[estado.x][estado.y].plata = estado.plata;
        tablero[estado.x][estado.y].oro = estado.oro;
        tablero[estado.x][estado.y].diamante = estado.diamante;

        // Restaurar el color original de la casilla
        tableroGrafico[estado.x][estado.y].setFillColor(estado.colorOriginal);

        // Restaurar los recursos del jugador
        tablero[0][0].plata = estado.plataJugador;
        tablero[0][0].oro = estado.oroJugador;
        tablero[0][0].diamante = estado.diamanteJugador;

        // Eliminar los recursos obtenidos del ataque de la lista del jugador
        Recurso* q = cabeza_mis_recursos;
        while (q != NULL) {
            if (q->tipo == "plata" && q->cantidad == estado.plata) {
                cabeza_mis_recursos = q->sig;
                delete q;
                break;
            } else if (q->tipo == "oro" && q->cantidad == estado.oro) {
                cabeza_mis_recursos = q->sig;
                delete q;
                break;
            } else if (q->tipo == "diamante" && q->cantidad == estado.diamante) {
                cabeza_mis_recursos = q->sig;
                delete q;
                break;
            }
            q = q->sig;
        }

        mensaje = "¡Acción deshecha! La casilla [" + to_string(estado.x) + ", " + to_string(estado.y) + "] ha sido restaurada.";
    } else {
        mensaje = "No hay acciones para deshacer.";
    }
}

// Función para generar un evento aleatorio
void generarEventoAleatorio(string &mensaje) {
    string eventos[] = {
        "Sumar 100 puntos a tu casilla",
        "Restar 200 puntos a tu casilla",
        "Sumar 300 puntos a todas las casillas enemigas",
        "Restar 200 puntos a todas las casillas enemigas"
    };

    // Si la cola está vacía, rellenarla con los eventos
    if (colaEventos.empty()) {
        for (int i = 0; i < 4; i++) {
            colaEventos.push(eventos[i]);
        }
    }

    // Obtener el evento de la cola
    string evento = colaEventos.front();
    colaEventos.pop();

    // Ejecutar el evento
    if (evento == "Sumar 100 puntos a tu casilla") {
        tablero[0][0].diamante += 1; // Sumar 100 puntos (1 diamante)
        mensaje = "¡Has ganado 100 puntos!";
    } else if (evento == "Restar 200 puntos a tu casilla") {
        tablero[0][0].diamante = max(0, tablero[0][0].diamante - 2); // Restar 200 puntos (2 diamantes)
        mensaje = "¡Has perdido 200 puntos!";
    } else if (evento == "Sumar 300 puntos a todas las casillas enemigas") {
        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                if (!(i == 0 && j == 0)) { // No afectar la casilla del jugador
                    tablero[i][j].diamante += 3; // Sumar 300 puntos (3 diamantes)
                }
            }
        }
        mensaje = "¡Todas las casillas enemigas han ganado 300 puntos!";
    } else if (evento == "Restar 200 puntos a todas las casillas enemigas") {
        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                if (!(i == 0 && j == 0)) { // No afectar la casilla del jugador
                    tablero[i][j].diamante = max(0, tablero[i][j].diamante - 2); // Restar 200 puntos (2 diamantes)
                }
            }
        }
        mensaje = "¡Todas las casillas enemigas han perdido 200 puntos!";
    }
}

int main() {
    srand(time(NULL));  // Inicializar la semilla de rand()

    // Inicializar el tablero
    inicializar_tablero();

    // Lista de recursos del jugador
    Recurso *cabeza_mis_recursos = NULL;

    RenderWindow window(VideoMode(1920, 1080), "Juego"); // Crear una ventana con resolución 1920x1080 y título "Juego"
    vector<Texture> frames; // Vector para almacenar las texturas de los frames
    // La función getFramesFromFolder("frames") debe devolver un vector de strings con las rutas de los archivos de imagen.
    vector<string> frameFiles = getFramesFromFolder("frames");
    sort(frameFiles.begin(), frameFiles.end()); // Ordenar los nombres de los archivos en orden ascendente

    // Recorrer cada archivo en el vector ordenado
    for (const auto &file : frameFiles) {
        Texture texture; // Crear una nueva textura
        if (texture.loadFromFile(file)) { // Intentar cargar la imagen en la textura
            frames.push_back(texture); // Agregar la textura cargada al vector de frames
        }
    }

    if (frames.empty()) { //Verificar si esta vacia la carpeta
        cerr << "No se encontraron fotogramas en frames/" << endl;
        return -1;
    }

    Sprite videoSprite; // Crear un objeto Sprite para representar el video en la pantalla
    // Ajustar la escala del sprite para que ocupe toda la ventana
    // Se calcula dividiendo el tamaño de la ventana entre el tamaño del primer frame
    videoSprite.setScale(
        window.getSize().x / (float)frames[0].getSize().x,
        window.getSize().y / (float)frames[0].getSize().y
    );

    // Variable para rastrear el frame actual de la animación
    int currentFrame = 0;
    // Reloj para medir el tiempo transcurrido entre los cambios de frame
    Clock frameClock;
    // Definir la tasa de refresco de la animación (FPS)
    // Se establece en 30 FPS, lo que equivale a un tiempo de cambio de frame de 1/30 segundos
    float frameRate = 1.0f / 30.0f;

    // Cargar música
    Music musica;
    if (!musica.openFromFile("C:/Users/LENOVO/Desktop/musica.ogg")) {
        cerr << "Error al cargar la música" << endl;
        return -1;
    }
    musica.setLoop(true);
    musica.play();

    // Cargar fuente
    Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/cour.ttf")) {
        cerr << "Error al cargar la fuente" << endl;
        return -1;
    }

    // Configuración del menú
    string textos[] = {"Ver mis recursos", "Atacar", "Farmear minerales", "Ver logros", "Ver caminos estratégicos", "Deshacer", "Evento Aleatorio", "Salir"};
    const int numOpciones = 8;  // Aumentamos el número de opciones
    RectangleShape botones[numOpciones]; //Para los rectangulos
    Text opciones[numOpciones]; //Para el texto de los rectangulos
    // Coordenadas de la esquina superior izquierda del menú en la ventana
    float menuX = 1300, menuY = 100; 
    // Dimensiones de los botones del menú
    float tamBotonX = 400, tamBotonY = 80; // Reducir el tamaño de los botones
    // Espaciado vertical entre los botones del menú
    float separacionBotones = 100;

    for (int i = 0; i < numOpciones; i++) {
        //Configurar el tamaño,color,etc
        botones[i].setSize(Vector2f(tamBotonX, tamBotonY));
        botones[i].setFillColor(Color(20, 20, 20, 200));
        botones[i].setOutlineThickness(5);
        botones[i].setOutlineColor(Color::Magenta);
        botones[i].setPosition(menuX, menuY + i * separacionBotones);

        opciones[i].setFont(font);
        opciones[i].setString(textos[i]);
        opciones[i].setCharacterSize(24); // Reducir el tamaño de la fuente
        opciones[i].setFillColor(Color::White);
        opciones[i].setPosition(menuX + 20, menuY + 25 + i * separacionBotones); // Ajustar la posición del texto
    }

    // Creación del tablero gráfico
    RectangleShape tableroGrafico[FILAS][COLUMNAS];
    Text puntosCasilla[FILAS][COLUMNAS];  // Texto para mostrar los puntos de cada casilla
    float tamanoCasilla = 150;
    float espaciado = 15;
    float tiempo = 0;
    // Calcular la posición inicial en el eje X para centrar el tablero horizontalmente,
    // dividiendo el espacio sobrante en 3 partes para lograr una distribución adecuada.
    float inicioX = (window.getSize().x - (COLUMNAS * (tamanoCasilla + espaciado))) / 3;
    // Calcular la posición inicial en el eje Y para centrar el tablero verticalmente,
    // dividiendo el espacio sobrante en 2 partes para alinearlo mejor en la ventana.
    float inicioY = (window.getSize().y - (FILAS * (tamanoCasilla + espaciado))) / 2;

    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            tableroGrafico[i][j].setSize(Vector2f(tamanoCasilla, tamanoCasilla));
            tableroGrafico[i][j].setFillColor(i == 0 && j == 0 ? Color::Red : Color(60, 60, 60, 220)); // Casilla [0][0] en rojo
            tableroGrafico[i][j].setOutlineThickness(4);
            tableroGrafico[i][j].setOutlineColor(Color::Cyan);
            tableroGrafico[i][j].setPosition(inicioX + j * (tamanoCasilla + espaciado), inicioY + i * (tamanoCasilla + espaciado));

            // Configurar el texto de los puntos de la casilla
            puntosCasilla[i][j].setFont(font);
            puntosCasilla[i][j].setCharacterSize(24);
            puntosCasilla[i][j].setFillColor(Color::White);
            puntosCasilla[i][j].setString(to_string(calcular_puntos_casilla(i, j)));  // Mostrar puntos de la casilla
            puntosCasilla[i][j].setPosition(
                inicioX + j * (tamanoCasilla + espaciado) + (tamanoCasilla / 2 - puntosCasilla[i][j].getLocalBounds().width / 2),
                inicioY + i * (tamanoCasilla + espaciado) + (tamanoCasilla / 2 - puntosCasilla[i][j].getLocalBounds().height / 2)
            );
        }
    }

    // Área para mostrar mensajes
    Text mensajeText;
    mensajeText.setFont(font);
    mensajeText.setCharacterSize(24);  // Reducir el tamaño de la fuente
    mensajeText.setFillColor(Color(255, 255, 255, 200));  // Color blanco semitransparente
    mensajeText.setPosition(50, 50);  // Posición ajustada

    // Área para mostrar recursos
    Text recursosText;
    recursosText.setFont(font);
    recursosText.setCharacterSize(24);  // Reducir el tamaño de la fuente
    recursosText.setFillColor(Color(255, 255, 255, 200));  // Color blanco semitransparente
    recursosText.setPosition(50, 200);  // Posición ajustada

    // Área para entrada de coordenadas (esquina superior derecha)
    Text coordenadasText;
    coordenadasText.setFont(font);
    coordenadasText.setCharacterSize(24);  // Reducir el tamaño de la fuente
    coordenadasText.setFillColor(Color(255, 255, 255, 200));  // Color blanco semitransparente
    coordenadasText.setPosition(window.getSize().x - 800, 50);  // Esquina superior derecha
    string coordenadasInput = "";
    bool ingresandoCoordenadas = false;

    // Área para mostrar logros
    bool mostrandoLogros = false;
    NodoLogro* arbolLogros = construirArbolLogros();

    // Crear el grafo
    Grafo grafo;

    string mensaje = "Bienvenido al juego!";
    string recursos_str = "";

    Clock clock;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        tiempo += deltaTime; //Para contar el tiempo
        // Obtener la posición actual del cursor del mouse dentro de la ventana
        Vector2i mousePos = Mouse::getPosition(window);
        // Declarar un objeto de tipo Event para manejar eventos de la ventana
        Event event;
        // Bucle para procesar los eventos de la ventana
        while (window.pollEvent(event)) {
            // Verificar si el evento detectado es el cierre de la ventana
            if (event.type == Event::Closed)
                window.close();

            // Capturar clics del mouse
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                for (int i = 0; i < numOpciones; i++) {
                    if (botones[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        // Limpiar la ventana antes de mostrar nuevo contenido
                        window.clear();

                        if (i == 0) { // Ver mis recursos
                            recursos_str = mostrar_mis_recursos(cabeza_mis_recursos);
                            recursosText.setString(recursos_str);  // Actualizar el texto de recursos
                        } else if (i == 1) { // Atacar
                            ingresandoCoordenadas = true;
                            coordenadasInput = "";
                            mensaje = "Ingresa las coordenadas (x y): ";
                            mensajeText.setString(mensaje);  // Actualizar el mensaje
                        } else if (i == 2) { // Farmear
                            farmear(cabeza_mis_recursos, mensaje);
                            mensajeText.setString(mensaje);  // Actualizar el mensaje
                        } else if (i == 3) { // Ver logros
                            mostrandoLogros = true;
                        } else if (i == 4) { // Ver caminos estratégicos
                            grafo.actualizarGrafo(tableroGrafico, cabeza_mis_recursos);
                            mostrarMatrizAdyacencia(grafo, tableroGrafico, font);
                        } else if (i == 5) { // Deshacer
                            deshacerAccion(tableroGrafico, cabeza_mis_recursos, mensaje);
                            mensajeText.setString(mensaje);  // Actualizar el mensaje
                        } else if (i == 6) { // Evento Aleatorio
                            generarEventoAleatorio(mensaje);
                            mensajeText.setString(mensaje);  // Actualizar el mensaje
                        } else if (i == 7) { // Salir
                            window.close();
                        }
                    }
                }
            }

            // Capturar entrada de texto para coordenadas
            if (ingresandoCoordenadas && event.type == Event::TextEntered) {
                // Si se presiona la tecla Backspace y la entrada no está vacía, eliminar el último carácter
                if (event.text.unicode == '\b' && !coordenadasInput.empty()) { // Tecla Backspace
                    coordenadasInput.pop_back();
                } else if (event.text.unicode >= '0' && event.text.unicode <= '9' || event.text.unicode == ' ') {
                    coordenadasInput += static_cast<char>(event.text.unicode);
                } else if (event.text.unicode == '\r') { // Tecla Enter, Si se presiona la tecla Enter, procesar la entrada y ejecutar la acción de ataque
                    int x, y;
                    stringstream ss(coordenadasInput);
                    ss >> x >> y; //Extraer las coordenadas ingresadas

                    // Guardar el estado actual de la casilla antes del ataque
                    EstadoCasilla estado;
                    estado.x = x;
                    estado.y = y;
                    estado.plata = tablero[x][y].plata;
                    estado.oro = tablero[x][y].oro;
                    estado.diamante = tablero[x][y].diamante;
                    estado.colorOriginal = tableroGrafico[x][y].getFillColor();
                    estado.plataJugador = tablero[0][0].plata;
                    estado.oroJugador = tablero[0][0].oro;
                    estado.diamanteJugador = tablero[0][0].diamante;
                    pilaDeshacer.push(estado); // Guardar el estado en la pila

                    atacar(cabeza_mis_recursos, x, y, mensaje, tableroGrafico); // Llamada a la función atacar
                    ingresandoCoordenadas = false;
                    mensajeText.setString(mensaje);  // Actualizar el mensaje
                }
            }
        }

        // Verificar si todas las casillas están pintadas de rojo
        if (todas_las_casillas_rojas(tableroGrafico)) {
            window.clear();

            // Crear el texto de felicitaciones
            Text felicitaciones;
            felicitaciones.setFont(font);
            felicitaciones.setCharacterSize(48);  // Tamaño de la fuente ajustado
            felicitaciones.setFillColor(Color::Green);
            felicitaciones.setString("FELICITACIONES ACABASTE EL JUEGO\nDESARROLLADO POR VLADIMIR TICONA Y CRISTIAN HUANCA");

            // Centrar el texto en la ventana
            FloatRect textRect = felicitaciones.getLocalBounds();
            felicitaciones.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
            felicitaciones.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

            // Dibujar el texto
            window.draw(felicitaciones);
            window.display();

            Sleep(5000);  // Esperar 5 segundos antes de cerrar
            window.close();
        }

        // Limpiar la ventana antes de dibujar
        window.clear();

        // Dibujar el fondo del video
        if (frameClock.getElapsedTime().asSeconds() >= frameRate) {
            currentFrame = (currentFrame + 1) % frames.size();
            videoSprite.setTexture(frames[currentFrame]);
            frameClock.restart();
        }
        window.draw(videoSprite);

        // Dibujar el tablero con animaciones
        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                // Calcular un desplazamiento vertical para generar un efecto de movimiento ondulante
                float desplazamiento = sin(tiempo * 2 + (i + j) * 0.7) * 8;
                tableroGrafico[i][j].setPosition(inicioX + j * (tamanoCasilla + espaciado), inicioY + i * (tamanoCasilla + espaciado) + desplazamiento);
                window.draw(tableroGrafico[i][j]);

                // Actualizar y dibujar los puntos de la casilla
                puntosCasilla[i][j].setString(to_string(calcular_puntos_casilla(i, j)));
                window.draw(puntosCasilla[i][j]);
            }
        }

        // Dibujar el menú con animaciones
        for (int i = 0; i < numOpciones; i++) {
            float desplazamiento = sin(tiempo * 3 + i) * 10;
            // Establecer la posición del botón aplicando el desplazamiento en el eje Y
            botones[i].setPosition(menuX, menuY + i * separacionBotones + desplazamiento);
            // Cambiar el color del contorno del botón si el mouse está sobre él
            botones[i].setOutlineColor(botones[i].getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color::Yellow : Color::Magenta);
            window.draw(botones[i]);
            window.draw(opciones[i]);
        }

        // Dibujar mensajes, recursos y coordenadas
        mensajeText.setString(mensaje);  // Actualizar el mensaje
        window.draw(mensajeText); //Dibuja lo que hemos hecho en la ventana
        window.draw(recursosText);
        if (ingresandoCoordenadas) {
            coordenadasText.setString("Ingresa las coordenadas (x y): " + coordenadasInput);
            window.draw(coordenadasText);
        }

        // Mostrar el árbol de logros si está activo
        if (mostrandoLogros) {
            // Calcular casillas conquistadas (excluyendo la casilla inicial del jugador)
            int casillasConquistadas = 0;
            for (int i = 0; i < FILAS; i++) {
                for (int j = 0; j < COLUMNAS; j++) {
                    if (tableroGrafico[i][j].getFillColor() == Color::Red && !(i == 0 && j == 0)) {
                        casillasConquistadas++;
                    }
                }
            }

            // Calcular puntos totales
            int puntosTotales = 0;
            Recurso* q = cabeza_mis_recursos;
            while (q != NULL) {
                if (q->tipo == "plata") {
                    puntosTotales += q->cantidad * 10;
                } else if (q->tipo == "oro") {
                    puntosTotales += q->cantidad * 50;
                } else if (q->tipo == "diamante") {
                    puntosTotales += q->cantidad * 100;
                }
                q = q->sig;
            }

            // Actualizar logros
            actualizarLogros(arbolLogros, casillasConquistadas, puntosTotales);

            // Dibujar el árbol de logros (en lista vertical)
            int yLogros = 750;  // Posición vertical inicial
            mostrarArbolLogros(arbolLogros, window, font, 20, yLogros);
        }

        // Mostrar todo en la ventana
        window.display();
    }

    return 0;
}
