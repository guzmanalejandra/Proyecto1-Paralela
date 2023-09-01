# Screensaver-Proyecto1

Este es un programa simple de protector de pantalla escrito en C++ utilizando la biblioteca SDL2. Muestra círculos rebotando en la pantalla y demuestra la detección básica de colisiones y simulación física.

## Requisitos Previos

Antes de poder compilar y ejecutar el programa, debes tener instalado lo siguiente en tu sistema:

- Biblioteca SDL2
- Compilador de C++ (por ejemplo, g++)

## Cómo Compilar

1. Instala la biblioteca SDL2 en tu sistema si aún no lo has hecho.
   - En Ubuntu, puedes usar: `sudo apt-get install libsdl2-dev`

2. Clona o descarga este repositorio en tu máquina local.

3. Abre una terminal y navega al directorio del repositorio.

4. Compila el programa utilizando el siguiente comando:
   ```bash
   g++ -o screensaver screensaver.cpp -lSDL2

## Funciones y Estructuras

### `struct Circle`

Estructura que representa un círculo en la pantalla.

- `float x, y`: Coordenadas del centro del círculo.
- `float dx, dy`: Componentes de velocidad en las direcciones X e Y.
- `float radius`: Radio del círculo.
- `SDL_Color color`: Color del círculo.

### `bool areColliding(const Circle& a, const Circle& b)`

Función que verifica si dos círculos están colisionando.

- `a`: Primer círculo.
- `b`: Segundo círculo.
- Retorna `true` si los círculos están colisionando; de lo contrario, retorna `false`.

### `int main(int argc, char* argv[])`

Función principal que ejecuta el programa.

- `argc`: Número de argumentos pasados al programa.
- `argv`: Arreglo de cadenas de caracteres que contienen los argumentos.
- El programa genera una simulación de protector de pantalla con círculos rebotando y colisionando.
- Acepta el número de círculos como argumento de línea de comandos.

## Autores
Este programa fue creado por

[Alejandra Guzman](https://github.com/guzmanalejandra).

[Jorge Caballeros](https://github.com/JorgeCab2711).


