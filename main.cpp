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

using namespace std;
using namespace sf;

// Función para obtener los archivos de imagen (frames) desde una carpeta
vector<string> getFramesFromFolder(const string &folder) {
    vector<string> frameFiles;
    WIN32_FIND_DATA fileData;
    HANDLE hFind;

    // Buscar archivos PNG en la carpeta
    string searchPath = folder + "\\*.png";  
    hFind = FindFirstFile(searchPath.c_str(), &fileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // Agregar el archivo a la lista
            frameFiles.push_back(folder + "\\" + fileData.cFileName);
        } while (FindNextFile(hFind, &fileData));
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

    NodoLogro(string nombre) : nombre(nombre), desbloqueado(false), izq(nullptr), der(nullptr) {}
};

// Tamaño del tablero
const int FILAS = 4;
const int COLUMNAS = 4;
Casilla tablero[FILAS][COLUMNAS];

// Prototipo de la función calcular_puntos_casilla
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
                    tablero[i][j].diamante = puntos / 100;  // 1 diamante = 100 puntos
                } else if (i == 2) {
                    // Fila 2: entre 5000 y 10000 puntos
                    int puntos = rand() % 5001 + 5000;  // Entre 5000 y 10000
                    tablero[i][j].plata = 0;
                    tablero[i][j].oro = 0;
                    tablero[i][j].diamante = puntos / 100;  // 1 diamante = 100 puntos
                } else if (i == 3) {
                    // Fila 3: entre 1000 y 1500 puntos
                    int puntos = rand() % 501 + 1000;  // Entre 1000 y 1500
                    tablero[i][j].plata = 0;
                    tablero[i][j].oro = 0;
                    tablero[i][j].diamante = puntos / 100;  // 1 diamante = 100 puntos
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
    Recurso *q = cabeza_mis_recursos;
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

    actualizarLogros(raiz->izq, casillasConquistadas, puntosTotales);
    actualizarLogros(raiz->der, casillasConquistadas, puntosTotales);
}

// Mostrar el árbol de logros en la interfaz gráfica (en lista vertical)
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
    mostrarArbolLogros(raiz->izq, window, font, x + 20, y);  // Indentar los hijos
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
        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                int indiceActual = i * COLUMNAS + j;
                for (int x = 0; x < FILAS; x++) {
                    for (int y = 0; y < COLUMNAS; y++) {
                        int indiceVecino = x * COLUMNAS + y;
                        if (indiceActual != indiceVecino) {
                            int puntosVecino = calcular_puntos_casilla(x, y);
                            int diferencia = puntosJugador - puntosVecino;
                            matrizAdyacencia[indiceActual][indiceVecino] = diferencia;
                        }
                    }
                }
            }
        }
    }

    string mostrarCaminosEstrategicos(RectangleShape tableroGrafico[FILAS][COLUMNAS]) {
        stringstream ss;
        ss << "Aristas desde la casilla [0, 0]:\n";
        int indiceJugador = 0;  // Casilla [0, 0]

        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                int indiceVecino = i * COLUMNAS + j;
                if (indiceJugador != indiceVecino) {
                    int diferencia = matrizAdyacencia[indiceJugador][indiceVecino];
                    ss << " -> Casilla [" << i << ", " << j << "] (Diferencia: " << diferencia << ") ";
                    if (tableroGrafico[i][j].getFillColor() == Color::Red) {
                        ss << "(Casilla ya conquistada)\n";
                    } else if (diferencia > 0) {
                        ss << "(Puedes conquistar)\n";
                    } else {
                        ss << "(No puedes conquistar)\n";
                    }
                }
            }
        }

        return ss.str();
    }
};

// Función para mostrar la matriz de adyacencia en una nueva ventana
void mostrarMatrizAdyacencia(Grafo& grafo, RectangleShape tableroGrafico[FILAS][COLUMNAS], Font& font) {
    RenderWindow matrizWindow(VideoMode(800, 600), "Matriz de Adyacencia");
    Text texto;
    texto.setFont(font);
    texto.setCharacterSize(20);
    texto.setFillColor(Color::White);

    while (matrizWindow.isOpen()) {
        Event event;
        while (matrizWindow.pollEvent(event)) {
            if (event.type == Event::Closed)
                matrizWindow.close();
        }

        matrizWindow.clear();
        stringstream ss;
        ss << grafo.mostrarCaminosEstrategicos(tableroGrafico);
        texto.setString(ss.str());
        matrizWindow.draw(texto);
        matrizWindow.display();
    }
}

int main() {
    srand(time(NULL));  // Inicializar la semilla de rand()

    // Inicializar el tablero
    inicializar_tablero();

    // Lista de recursos del jugador
    Recurso *cabeza_mis_recursos = NULL;

    RenderWindow window(VideoMode(1920, 1080), "Juego - Video de Fondo + Menu Mejorado");
    vector<Texture> frames;
    vector<string> frameFiles = getFramesFromFolder("frames");
    sort(frameFiles.begin(), frameFiles.end());

    for (const auto &file : frameFiles) {
        Texture texture;
        if (texture.loadFromFile(file)) {
            frames.push_back(texture);
        }
    }

    if (frames.empty()) {
        cerr << "No se encontraron fotogramas en frames/" << endl;
        return -1;
    }

    Sprite videoSprite;
    videoSprite.setScale(
        window.getSize().x / (float)frames[0].getSize().x,
        window.getSize().y / (float)frames[0].getSize().y
    );

    int currentFrame = 0;
    Clock frameClock;
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
    string textos[] = {"Ver mis recursos", "Atacar", "Farmear minerales", "Ver logros", "Ver caminos estratégicos", "Salir"};
    const int numOpciones = 6;  // Aumentamos el número de opciones
    RectangleShape botones[numOpciones];
    Text opciones[numOpciones];

    float menuX = 1300, menuY = 200;
    float tamBotonX = 500, tamBotonY = 110;
    float separacionBotones = 140;

    for (int i = 0; i < numOpciones; i++) {
        botones[i].setSize(Vector2f(tamBotonX, tamBotonY));
        botones[i].setFillColor(Color(20, 20, 20, 200));
        botones[i].setOutlineThickness(5);
        botones[i].setOutlineColor(Color::Magenta);
        botones[i].setPosition(menuX, menuY + i * separacionBotones);

        opciones[i].setFont(font);
        opciones[i].setString(textos[i]);
        opciones[i].setCharacterSize(32);
        opciones[i].setFillColor(Color::White);
        opciones[i].setPosition(menuX + 50, menuY + 35 + i * separacionBotones);
    }

    // Creación del tablero gráfico
    RectangleShape tableroGrafico[FILAS][COLUMNAS];
    Text puntosCasilla[FILAS][COLUMNAS];  // Texto para mostrar los puntos de cada casilla
    float tamanoCasilla = 150;
    float espaciado = 15;
    float tiempo = 0;

    float inicioX = (window.getSize().x - (COLUMNAS * (tamanoCasilla + espaciado))) / 3;
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
        tiempo += deltaTime;

        Vector2i mousePos = Mouse::getPosition(window);
        Event event;
        while (window.pollEvent(event)) {
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
                        } else if (i == 5) { // Salir
                            window.close();
                        }
                    }
                }
            }

            // Capturar entrada de texto para coordenadas
            if (ingresandoCoordenadas && event.type == Event::TextEntered) {
                if (event.text.unicode == '\b' && !coordenadasInput.empty()) { // Tecla Backspace
                    coordenadasInput.pop_back();
                } else if (event.text.unicode >= '0' && event.text.unicode <= '9' || event.text.unicode == ' ') {
                    coordenadasInput += static_cast<char>(event.text.unicode);
                } else if (event.text.unicode == '\r') { // Tecla Enter
                    int x, y;
                    stringstream ss(coordenadasInput);
                    ss >> x >> y;
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
            felicitaciones.setString("FELICITACIONES ACABASTE EL JUEGO\nJUEGO DESARROLLADO POR VLADIMIR TICONA Y CRISTIAN HUANCA");

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
            botones[i].setPosition(menuX, menuY + i * separacionBotones + desplazamiento);
            botones[i].setOutlineColor(botones[i].getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color::Yellow : Color::Magenta);
            window.draw(botones[i]);
            window.draw(opciones[i]);
        }

        // Dibujar mensajes, recursos y coordenadas
        mensajeText.setString(mensaje);  // Actualizar el mensaje
        window.draw(mensajeText);
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
