#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include "rectangle.h"
#include "circle.h"

//define window dimensions
constexpr int WINDOW_WIDTH{640};
constexpr int WINDOW_HEIGHT{480};
constexpr float VELOCITY_THRESHOLD{100.0f};

//define SDL Window related variables
SDL_Window *window{nullptr};
SDL_Renderer *renderer{nullptr};
SDL_Event currentEvent;
SDL_Color backgroundColor{255, 255, 255, 255};

bool quit{false};

float mouseX{-1.0f}, mouseY{-1.0f};

float displayScale{1.0f};

Rectangle rectangle;
Circle circle;
std::vector<Circle> circles;

Uint64 startTime, endTime;
float dt;

bool isCircleBlue{false};

SDL_TimerID circleTimerID{0};

void initShapes() {
    rectangle.pos = glm::vec2(150.0f, 150.0f);
    rectangle.width = 50.0f;
    rectangle.height = 50.0f;
    rectangle.color = SDL_Color{255, 0, 0, 255};

    circle.pos = glm::vec2(150.0f, 350.0f);
    circle.radius = 25.0;
    circle.color = SDL_Color{255, 255, 0, 255};

    circles.push_back(circle);
    //circles.size();
}

Uint32 circleTimerCallback(void *userdata, SDL_TimerID timerID, Uint32 interval) {

    if (isCircleBlue) {
        circle.color = SDL_Color{255, 255, 0, 255};
        isCircleBlue = false;
    } else {
        circle.color = SDL_Color{0, 0, 255, 255};
        isCircleBlue = true;
    }

    return interval;
}

bool initWindow() {
    bool success{true};

    //Try to initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL initialization failed: %s\n", SDL_GetError());
        success = false;
    } else {
        //Try to create the window and renderer
        displayScale = SDL_GetDisplayContentScale(1);

        if (!SDL_CreateWindowAndRenderer(
            "SDL Hello World Example",
            static_cast<int>(displayScale * WINDOW_WIDTH),
            static_cast<int>(displayScale * WINDOW_HEIGHT),
            0,
            &window, &renderer)) {
            SDL_Log("Failed to create window and renderer: %s\n", SDL_GetError());
            success = false;
        } else {
            //Apply global display scaling to renderer
            SDL_SetRenderScale(renderer, displayScale, displayScale);

            //Set background color
            SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b,
                                   backgroundColor.a);

            //Apply background color
            SDL_RenderClear(renderer);
        }
    }

    return success;
}

void processEvents() {
    //Check for events in queue
    while (SDL_PollEvent(&currentEvent)) {
        //User requests quit
        if (currentEvent.type == SDL_EVENT_QUIT) {
            quit = true;
        }

        //Mouse event -> pressed button
        if (currentEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            if (currentEvent.button.button == SDL_BUTTON_LEFT) {
                SDL_GetMouseState(&mouseX, &mouseY);
                mouseX /= displayScale;
                mouseY /= displayScale;
                std::cout << "Left mouse click => " << "x: " << mouseX << ", y: " << mouseY << std::endl;
            }
        }

        //Mouse event -> mouse movement
        if (currentEvent.type == SDL_EVENT_MOUSE_MOTION) {

            SDL_GetMouseState(&mouseX, &mouseY);
            mouseX /= displayScale;
            mouseY /= displayScale;

            rectangle.acc = glm::vec2(mouseX, mouseY) - rectangle.pos;

            if (const SDL_MouseButtonFlags mouseButtons = SDL_GetMouseState(nullptr, nullptr); mouseButtons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) {
                SDL_GetMouseState(&mouseX, &mouseY);
                mouseX /= displayScale;
                mouseY /= displayScale;
                std::cout << "Mouse move while left clicking => " << "x: " << mouseX << ", y: " << mouseY << std::endl;
            }
        }

        //Keyboard event
        if (currentEvent.type == SDL_EVENT_KEY_DOWN) {
            switch (currentEvent.key.key) {
                case SDLK_UP:
                    //std::cout << "UP arrow key was pressed" << std::endl;
                    //rectangle.pos.y -= 5.0f;
                    break;
                case SDLK_DOWN:
                    //std::cout << "DOWN arrow key was pressed" << std::endl;
                    //rectangle.pos.y += 5.0f;
                    break;
                case SDLK_LEFT:
                    //std::cout << "LEFT arrow key was pressed" << std::endl;
                    //rectangle.pos.x -= 5.0f;
                    break;
                case SDLK_RIGHT:
                    //std::cout << "RIGHT arrow key was pressed" << std::endl;
                    //rectangle.pos.x += 5.0f;
                    break;
                case SDLK_R:
                    std::cout << "R key was pressed" << std::endl;
                    break;
                case SDLK_S:
                    std::cout << "S key was pressed" << std::endl;
                    break;
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                default:
                    break;
            }
        }
    }
}

void processMovement() {
    const bool* keyboardState = SDL_GetKeyboardState(nullptr);

    if (keyboardState[SDL_SCANCODE_LEFT]) {
        rectangle.pos.x -= 1.0f;
    }

    if (keyboardState[SDL_SCANCODE_RIGHT]) {
        rectangle.pos.x += 1.0f;
    }

    if (keyboardState[SDL_SCANCODE_UP]) {
        rectangle.pos.y -= 1.0f;
    }

    if (keyboardState[SDL_SCANCODE_DOWN]) {
        rectangle.pos.y += 1.0f;
    }
}

void animate() {
    glm::vec2 direction = glm::normalize(glm::vec2(mouseX, mouseY) - rectangle.pos);
    rectangle.vel += rectangle.acc * dt;

    if (rectangle.vel.x < -VELOCITY_THRESHOLD) {
        rectangle.vel.x = -VELOCITY_THRESHOLD;
    }

    if (rectangle.vel.x > VELOCITY_THRESHOLD) {
        rectangle.vel.x = VELOCITY_THRESHOLD;
    }

    if (rectangle.vel.y < -VELOCITY_THRESHOLD) {
        rectangle.vel.y = -VELOCITY_THRESHOLD;
    }

    if (rectangle.vel.y > VELOCITY_THRESHOLD) {
        rectangle.vel.y = VELOCITY_THRESHOLD;
    }

    rectangle.pos += rectangle.vel * dt + rectangle.acc * dt * dt / 2.0f;
    //direction = glm::normalize(glm::vec2(mouseX, mouseY) - circle.pos);
    //circle.pos += direction * 150.0f * dt;
}

void drawFrame() {
    //Clear the background
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(renderer);

    //Draw scene
    rectangle.draw(renderer);
    circle.draw(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0,255, 255);
    SDL_RenderLine(renderer, mouseX, mouseY, rectangle.pos.x, rectangle.pos.y);

    //Update window
    SDL_RenderPresent(renderer);
}

void cleanup() {

    SDL_RemoveTimer(circleTimerID);

    //Destroy renderer
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    //Destroy window
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    //Quit SDL
    SDL_Quit();
}

int main() {
    //Initialize window
    if (!initWindow()) {
        std::cout << "Failed to initialize" << std::endl;
        return -1;
    }

    initShapes();

    SDL_zero(currentEvent);

    circleTimerID = SDL_AddTimer(2000, circleTimerCallback, nullptr);

    //Game loop
    while (!quit) {
        startTime = SDL_GetTicks();
        processEvents();
        processMovement();
        animate();
        drawFrame();
        endTime = SDL_GetTicks();
        dt = (endTime - startTime) / 1000.0f;
    }

    cleanup();
    return 0;
}
