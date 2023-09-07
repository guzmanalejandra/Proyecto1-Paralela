#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <omp.h>

const int WIDTH = 640;
const int HEIGHT = 480;
const float DAMPING = 0.8;
const float SPEED_INCREASE = 1.1;
const float MAX_SPEED = 5.0f;
const float MIN_SPEED = 1.0f;

struct Circle {
    float x, y;
    float dx, dy;
    float radius;
    SDL_Color color;
};

bool areColliding(const Circle& a, const Circle& b) {
    float distance = std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
    return distance < (a.radius + b.radius);
}

void SDL_RenderFillCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    int x = radius - 1;
    int y = 0;
    int tx = 1;
    int ty = 1;
    int diameter = radius * 2;
    int error = tx - diameter;

    while (x >= y) {
        // Dibuja cada punto de la circunferencia
        for (int currentX = -x; currentX <= x; currentX++) {
            SDL_RenderDrawPoint(renderer, centerX + currentX, centerY - y);
            SDL_RenderDrawPoint(renderer, centerX + currentX, centerY + y);
        }
        for (int currentY = -y; currentY <= y; currentY++) {
            SDL_RenderDrawPoint(renderer, centerX + x, centerY + currentY);
            SDL_RenderDrawPoint(renderer, centerX - x, centerY + currentY);
        }

        if (error <= 0) {
            y++;
            error += ty;
            ty += 2;
        }

        if (error > 0) {
            x--;
            tx += 2;
            error += (tx - diameter);
        }
    }
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Por favor, introduzca un valor para N." << std::endl;
        return 1;
    }

    std::string arg = argv[1];
    for (char c : arg) {
        if (!std::isdigit(c)) {
            std::cout << "Por favor, introduzca un valor numérico para N." << std::endl;
            return 1;
        }
    }

    int N = std::stoi(arg);
    if (N <= 2) {
        std::cout << "Por favor, introduzca un valor mayor a 2 para N." << std::endl;
        return 1;
    }

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Screensaver", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::vector<Circle> circles;

    for (int i = 0; i < N; ++i) {
        Circle circle;
        circle.radius = std::rand() % 50 + 10;
        circle.x = std::rand() % (WIDTH - static_cast<int>(circle.radius * 2)) + circle.radius;
        circle.y = std::rand() % (HEIGHT - static_cast<int>(circle.radius * 2)) + circle.radius;
        circle.dx = (std::rand() % 200 - 100) / 100.f;
        circle.dy = (std::rand() % 200 - 100) / 100.f;
        circle.color = { static_cast<Uint8>(std::rand() % 256), static_cast<Uint8>(std::rand() % 256), static_cast<Uint8>(std::rand() % 256), 255 };
        circles.push_back(circle);
    }

    double sequentialTime, parallelTime;
    bool running = true;
    SDL_Event event;
    Uint32 startTick, frameTime, accumulatedTime = 0;
    int frameCount = 0;

    while (running) {
        startTick = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        sequentialTime = omp_get_wtime();
        for (Circle& circle : circles) {
            circle.x += circle.dx;
            circle.y += circle.dy;

            // Rebotar contra los bordes con atenuación
            if (circle.x - circle.radius < 0) {
                circle.dx = std::abs(circle.dx) * DAMPING;
                circle.x = circle.radius; // Ajustar posición
            } 
            else if (circle.x + circle.radius > WIDTH) {
                circle.dx = -std::abs(circle.dx) * DAMPING;
                circle.x = WIDTH - circle.radius; // Ajustar posición
            }

            if (circle.y - circle.radius < 0) {
                circle.dy = std::abs(circle.dy) * DAMPING;
                circle.y = circle.radius; // Ajustar posición
            }
            else if (circle.y + circle.radius > HEIGHT) {
                circle.dy = -std::abs(circle.dy) * DAMPING;
                circle.y = HEIGHT - circle.radius; // Ajustar posición
            }

            // Introducir una pequeña variación en el rebote para evitar movimientos repetitivos
            float variance = ((std::rand() % 200) - 100) / 1000.0f;
            circle.dx += variance;
            circle.dy += variance;

            // Detectar colisión con otros círculos y aumentar velocidad
            for (Circle& other : circles) {
                if (&circle != &other && areColliding(circle, other)) {
                    circle.dx *= SPEED_INCREASE;
                    circle.dy *= SPEED_INCREASE;
                }
            }

            // Limitar la velocidad entre MIN_SPEED y MAX_SPEED
            float currentSpeed = std::sqrt(circle.dx * circle.dx + circle.dy * circle.dy);
            if (currentSpeed > MAX_SPEED) {
                circle.dx = (circle.dx / currentSpeed) * MAX_SPEED;
                circle.dy = (circle.dy / currentSpeed) * MAX_SPEED;
            } else if (currentSpeed < MIN_SPEED) {
                circle.dx = (circle.dx / currentSpeed) * MIN_SPEED;
                circle.dy = (circle.dy / currentSpeed) * MIN_SPEED;
            }

            // Dibujar el círculo
            SDL_SetRenderDrawColor(renderer, circle.color.r, circle.color.g, circle.color.b, circle.color.a);
            for (int w = 0; w < circle.radius * 2; w++) {
                for (int h = 0; h < circle.radius * 2; h++) {
                    int dx = circle.radius - w;
                    int dy = circle.radius - h;
                    if ((dx*dx + dy*dy) <= (circle.radius * circle.radius)) {
                        SDL_RenderDrawPoint(renderer, circle.x + dx - circle.radius, circle.y + dy - circle.radius);
                    }
                }
            }
        }
        sequentialTime = omp_get_wtime() - sequentialTime;

        parallelTime = omp_get_wtime();
        #pragma omp parallel for
        for (int i = 0; i < circles.size(); i++) {
            Circle& circle = circles[i];

            // Movimiento
            circle.x += circle.dx;
            circle.y += circle.dy;

            // Rebote en los bordes
            if (circle.x - circle.radius < 0 || circle.x + circle.radius > WIDTH) {
                circle.dx = -circle.dx;
                circle.x += circle.dx;
            }
            if (circle.y - circle.radius < 0 || circle.y + circle.radius > HEIGHT) {
                circle.dy = -circle.dy;
                circle.y += circle.dy;
            }

            // Detección de colisión con otros círculos
            for (int j = 0; j < circles.size(); j++) {
                if (i != j) {
                    Circle& other = circles[j];
                    if (areColliding(circle, other)) {
                        // Cambia las direcciones si colisionan
                        circle.dx = -circle.dx;
                        circle.dy = -circle.dy;
                        other.dx = -other.dx;
                        other.dy = -other.dy;
                        
                        // Desplaza un poco para evitar que queden "pegados"
                        circle.x += circle.dx;
                        circle.y += circle.dy;
                        other.x += other.dx;
                        other.y += other.dy;
                    }
                }
            }

            // Dibuja el círculo
            SDL_SetRenderDrawColor(renderer, circle.color.r, circle.color.g, circle.color.b, circle.color.a);
            SDL_RenderFillCircle(renderer, circle.x, circle.y, circle.radius);
        }

        parallelTime = omp_get_wtime() - parallelTime;

        SDL_RenderPresent(renderer);

        frameTime = SDL_GetTicks() - startTick;
        accumulatedTime += frameTime;
        frameCount++;

        if (accumulatedTime >= 1000) {
            float fps = static_cast<float>(frameCount) / (accumulatedTime / 1000.0f);
            std::cout << "FPS: " << fps << std::endl;
            std::cout << "Tiempo secuencial: " << sequentialTime << " segundos" << std::endl;
            std::cout << "Tiempo paralelo: " << parallelTime << " segundos" << std::endl;
            std::cout << "Speed-up: " << sequentialTime / parallelTime << std::endl;
            accumulatedTime = 0;
            frameCount = 0;
        }

        if (frameTime < 1000 / 60) {
            SDL_Delay(1000 / 60 - frameTime);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
