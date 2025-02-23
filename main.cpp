#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <windows.h>  // Para buscar archivos en Windows
#include <cmath>
#include <ctime>      // Para time() y srand()
#include <cstdlib>    // Para rand()
#include <sstream>    // Para stringstream

using namespace std;
using namespace sf;

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
                // Asignar recursos aleatorios a las demás casillas
                tablero[i][j].plata = rand() % 51 + 50;  // Entre 50 y 100
                tablero[i][j].oro = rand() % 11 + 10;    // Entre 10 y 20
                tablero[i][j].diamante = rand() % 6 + 5; // Entre 5 y 10
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
        mensaje = "¡Ataque exitoso! La casilla ahora es tuya.";

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

// Función para obtener los fotogramas del video desde una carpeta
vector<string> getFramesFromFolder(const string &folder) {
    vector<string> frameFiles;
    WIN32_FIND_DATA fileData;
    HANDLE hFind;

    string searchPath = folder + "\\*.png";  
    hFind = FindFirstFile(searchPath.c_str(), &fileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            frameFiles.push_back(folder + "\\" + fileData.cFileName);
        } while (FindNextFile(hFind, &fileData));
        FindClose(hFind);
    }

    return frameFiles;
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
    string textos[] = {"Ver mis recursos", "Atacar", "Farmear minerales", "Ver jerarquía", "Salir"};
    const int numOpciones = 5;
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
    float tamanoCasilla = 180;
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
            puntosCasilla[i][j].setPosition(inicioX + j * (tamanoCasilla + espaciado) + 10, inicioY + i * (tamanoCasilla + espaciado) + 10);
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
    recursosText.setPosition(50, 150);  // Posición ajustada

    // Área para entrada de coordenadas
    Text coordenadasText;
    coordenadasText.setFont(font);
    coordenadasText.setCharacterSize(24);  // Reducir el tamaño de la fuente
    coordenadasText.setFillColor(Color(255, 255, 255, 200));  // Color blanco semitransparente
    coordenadasText.setPosition(50, 250);  // Posición ajustada
    string coordenadasInput = "";
    bool ingresandoCoordenadas = false;

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
                        if (i == 0) { // Ver mis recursos
                            recursos_str = mostrar_mis_recursos(cabeza_mis_recursos);
                        } else if (i == 1) { // Atacar
                            ingresandoCoordenadas = true;
                            coordenadasInput = "";
                            mensaje = "Ingresa las coordenadas (x y): ";
                        } else if (i == 2) { // Farmear
                            farmear(cabeza_mis_recursos, mensaje);
                        } else if (i == 4) { // Salir
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

        // Actualizar textos
        mensajeText.setString(mensaje);
        recursosText.setString(recursos_str);
        coordenadasText.setString("Coordenadas: " + coordenadasInput);

        // Actualizar los puntos de cada casilla
        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                int puntos = calcular_puntos_casilla(i, j);
                puntosCasilla[i][j].setString("Puntos: " + to_string(puntos));
            }
        }

        // Actualizar animaciones del menú y el tablero
        for (int i = 0; i < numOpciones; i++) {
            float desplazamiento = sin(tiempo * 3 + i) * 10;
            botones[i].setPosition(menuX, menuY + i * separacionBotones + desplazamiento);
            botones[i].setOutlineColor(botones[i].getGlobalBounds().contains(mousePos.x, mousePos.y) ? Color::Yellow : Color::Magenta);
        }

        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                float desplazamiento = sin(tiempo * 2 + (i + j) * 0.7) * 8;
                tableroGrafico[i][j].setPosition(inicioX + j * (tamanoCasilla + espaciado), inicioY + i * (tamanoCasilla + espaciado) + desplazamiento);
            }
        }

        if (frameClock.getElapsedTime().asSeconds() >= frameRate) {
            currentFrame = (currentFrame + 1) % frames.size();
            videoSprite.setTexture(frames[currentFrame]);
            frameClock.restart();
        }

        // Dibujar todo
        window.clear();
        window.draw(videoSprite);
        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                window.draw(tableroGrafico[i][j]);
                window.draw(puntosCasilla[i][j]);  // Dibujar los puntos de la casilla
            }
        }
        for (int i = 0; i < numOpciones; i++) {
            window.draw(botones[i]);
            window.draw(opciones[i]);
        }
        window.draw(mensajeText);
        window.draw(recursosText);
        if (ingresandoCoordenadas) window.draw(coordenadasText);
        window.display();
    }

    return 0;
}
