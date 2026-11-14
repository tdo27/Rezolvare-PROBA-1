#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include "rectangle.h"

#include <algorithm>

#include "circle.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"

//define window dimensions
constexpr int WINDOW_WIDTH{700};
constexpr int WINDOW_HEIGHT{800};

// Vertical scrolling grid related variables
constexpr float VERTICAL_SCROLLING_SPEED{ 100 };
constexpr int VERTICAL_SCROLLING_GRID_SQUARE_SIZE{ 20 };
std::vector<Rectangle> scrollingGrid;

// Animation related variables
float DT{ 0.01f };
Uint64 startTime{ 0 };
Uint64 endTime{ 0 };

//define SDL Window related variables
SDL_Window *window{nullptr};
SDL_Renderer *renderer{nullptr};
SDL_Event currentEvent;
SDL_Color backgroundColor{255, 255, 255, 255};

bool quit{false};

float mouseX{-1.0f}, mouseY{-1.0f};

float displayScale{1.0f};

constexpr float PLAYER_SIZE{50.0f};
Rectangle player;

// Inertial movement related variables
constexpr float PLAYER_ACCELERATION{400.0f};
constexpr float PLAYER_DECELERATION{2.0f * PLAYER_ACCELERATION};
constexpr float PLAYER_MAX_SPEED{500.0f};
constexpr float MOUSE_STOP_DELAY{0.05f};   // s without motion events => mouse "stopped"
glm::vec2 mouseMotion{0.0f, 0.0f}; // last relative mouse movement direction
float timeSinceMouseMoved{1.0f};

// Player projectiles related variables
constexpr float PLAYER_PROJECTILE_RADIUS{10.0f};
constexpr float PLAYER_PROJECTILE_SPEED{500.0f};
constexpr float PLAYER_FIRE_INTERVAL{0.3f}; // s between projectiles
std::vector<Circle> playerProjectiles;
bool isFiring{false};
float fireCooldown{0.0f}; // s until the next projectile can be fired

constexpr int NUM_ENEMY_SLICES{3};
constexpr float ENEMY_MIN_SPEED{50.0f};
constexpr float ENEMY_MAX_SPEED{150.0f};
constexpr float ENEMY_SPAWN_INTERVAL{3.0f};

// Enemy projectiles related variables
constexpr float ENEMY_PROJECTILE_RADIUS{10.0f};
constexpr float ENEMY_PROJECTILE_SPEED{PLAYER_PROJECTILE_SPEED / 5.0f};
constexpr float ENEMY_FIRE_INTERVAL{PLAYER_FIRE_INTERVAL * 10.0f};
constexpr float ENEMY_FIRST_SHOT_DELAY{1.0f};
constexpr float ENEMY_SPREAD_ANGLE{15.0f};
std::vector<Circle> enemyProjectiles;

// Repulsor related variables
constexpr float REPULSOR_STRENGTH{10.0f};   // px/s added away from the player, per right click

struct Enemy {
    Rectangle rect;
    bool active{false};
    float fireTimer{0.0f};   // s until this enemy fires again
};

Enemy enemies[NUM_ENEMY_SLICES];   // one enemy per vertical slice
float enemySpawnTimer{0.0f};

// Game state related variables
enum class GameState { PLAYING, WON, LOST };
GameState gameState{GameState::PLAYING};

void resetGame();   // defined further below, used in processEvents()

glm::vec2 getWindowSize() {
    int w{0}, h{0};
    SDL_GetRenderOutputSize(renderer, &w, &h);
    return glm::vec2(w / displayScale, h / displayScale);
}

void initPlayer() {
    glm::vec2 windowSize = getWindowSize();

    player.width = PLAYER_SIZE;
    player.height = PLAYER_SIZE;
    player.pos.x = windowSize.x / 2.0f;  // vertical line din centru
    player.pos.y = windowSize.y - player.height / 2.0f;
    player.vel = glm::vec2(0.0f, 0.0f);
    player.acc = glm::vec2(0.0f, 0.0f);
    player.color = SDL_Color{0, 255, 0, 255}; // green
}

void initScollingRectangles() {
    int numGridRectangles = (WINDOW_HEIGHT / VERTICAL_SCROLLING_GRID_SQUARE_SIZE) / 2 + 2;

    for (int i = 0; i < numGridRectangles; i++) {
        Rectangle newGridRectangleLeft, newGridRectangleRight;

        newGridRectangleLeft.width = newGridRectangleLeft.height = VERTICAL_SCROLLING_GRID_SQUARE_SIZE;
        newGridRectangleLeft.pos.x = newGridRectangleLeft.width / 2.0f;
        newGridRectangleLeft.pos.y = WINDOW_HEIGHT - newGridRectangleLeft.height / 2.0f - 2.0f * i * newGridRectangleLeft.height;
        newGridRectangleLeft.vel = glm::vec2(0.0f, VERTICAL_SCROLLING_SPEED);
        newGridRectangleLeft.color = SDL_Color{ 235, 235, 235, 255 };

        newGridRectangleRight.width = newGridRectangleRight.height = VERTICAL_SCROLLING_GRID_SQUARE_SIZE;
        newGridRectangleRight.pos.x = WINDOW_WIDTH - newGridRectangleRight.width / 2.0f;
        newGridRectangleRight.pos.y = WINDOW_HEIGHT - newGridRectangleRight.height / 2.0f - 2.0f * i * newGridRectangleRight.height;
        newGridRectangleRight.vel = glm::vec2(0.0f, VERTICAL_SCROLLING_SPEED);
        newGridRectangleRight.color = SDL_Color{ 235, 235, 235, 255 };

        scrollingGrid.push_back(newGridRectangleLeft);
        scrollingGrid.push_back(newGridRectangleRight);
    }
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

void activateRepulsor() {
    for (Circle& projectile : enemyProjectiles) {
        glm::vec2 awayFromPlayer = projectile.pos - player.pos;
        if (glm::length(awayFromPlayer) == 0.0f) continue;   // avoid normalizing a zero vector

        projectile.vel += glm::normalize(awayFromPlayer) * REPULSOR_STRENGTH;
    }
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
                isFiring = true;
            }

            if (currentEvent.button.button == SDL_BUTTON_RIGHT) {
                activateRepulsor();
            }
        }

        //Mouse event -> released button
        if (currentEvent.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            if (currentEvent.button.button == SDL_BUTTON_LEFT) {
                isFiring = false;
            }
        }

        //Mouse event -> mouse movement
        if (currentEvent.type == SDL_EVENT_MOUSE_MOTION) {
            glm::vec2 rel(currentEvent.motion.xrel, currentEvent.motion.yrel);
            if (glm::length(rel) > 0.0f) {
                mouseMotion = rel;
                timeSinceMouseMoved = 0.0f;
            }

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
                    std::cout << "UP arrow key was pressed" << std::endl;
                    break;
                case SDLK_DOWN:
                    std::cout << "DOWN arrow key was pressed" << std::endl;
                    break;
                case SDLK_LEFT:
                    std::cout << "LEFT arrow key was pressed" << std::endl;
                    break;
                case SDLK_RIGHT:
                    std::cout << "RIGHT arrow key was pressed" << std::endl;
                    break;
                case SDLK_R:
                    // The game can only be reset after it has ended
                    if (gameState != GameState::PLAYING) {
                        resetGame();
                    }
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

void updatePlayer() {
    timeSinceMouseMoved += DT;
    bool mouseIsMoving = timeSinceMouseMoved < MOUSE_STOP_DELAY;

    if (mouseIsMoving) {
        // Accelerate in the direction the mouse moved
        player.acc = glm::normalize(mouseMotion) * PLAYER_ACCELERATION;
        player.vel += player.acc * DT;
    } else {
        // Decelerate, opposite to the current velocity, without going backwards
        player.acc = glm::vec2(0.0f, 0.0f);
        float speed = glm::length(player.vel);
        if (speed > 0.0f) {
            float newSpeed = std::max(0.0f, speed - PLAYER_DECELERATION * DT);
            player.vel = player.vel / speed * newSpeed;
        }
    }

    // Limit the speed
    if (glm::length(player.vel) > PLAYER_MAX_SPEED) {
        player.vel = glm::normalize(player.vel) * PLAYER_MAX_SPEED;
    }

    player.pos += player.vel * DT;

    // Keep the ship inside the window; stop the velocity on the axis that hits a wall
    glm::vec2 windowSize = getWindowSize();
    float halfW = player.width / 2.0f;
    float halfH = player.height / 2.0f;

    if (player.pos.x < halfW)                { player.pos.x = halfW;                player.vel.x = 0.0f; }
    if (player.pos.x > windowSize.x - halfW) { player.pos.x = windowSize.x - halfW; player.vel.x = 0.0f; }
    if (player.pos.y < halfH)                { player.pos.y = halfH;                player.vel.y = 0.0f; }
    if (player.pos.y > windowSize.y - halfH) { player.pos.y = windowSize.y - halfH; player.vel.y = 0.0f; }
}

void updatePlayerProjectiles() {
    // Fire a new projectile if the button is held and the cooldown has passed
    fireCooldown = std::max(0.0f, fireCooldown - DT);

    if (isFiring && fireCooldown <= 0.0f) {
        Circle projectile(PLAYER_PROJECTILE_RADIUS, player.pos, SDL_Color{255, 0, 0, 255});   // red
        projectile.vel = glm::vec2(0.0f, -PLAYER_PROJECTILE_SPEED);   // y grows downwards, so up is negative
        playerProjectiles.push_back(projectile);
        fireCooldown = PLAYER_FIRE_INTERVAL;
    }

    // Move all projectiles
    for (Circle& projectile : playerProjectiles) {
        projectile.pos += projectile.vel * DT;
    }

    // Remove projectiles that left the window through the top
    playerProjectiles.erase(
        std::remove_if(playerProjectiles.begin(), playerProjectiles.end(),
                       [](const Circle& p) { return p.pos.y < -p.radius; }),
        playerProjectiles.end());
}

// Random float in [min, max)
float randomFloat(float min, float max) {
    return min + SDL_randf() * (max - min);
}

void spawnEnemy(int slice) {
    glm::vec2 windowSize = getWindowSize();
    float sliceWidth = windowSize.x / NUM_ENEMY_SLICES;

    Enemy& enemy = enemies[slice];
    enemy.rect.width = PLAYER_SIZE;
    enemy.rect.height = PLAYER_SIZE;

    // Random x inside its own slice, so that the whole rectangle stays in the slice
    float minX = slice * sliceWidth + enemy.rect.width / 2.0f;
    float maxX = (slice + 1) * sliceWidth - enemy.rect.width / 2.0f;
    enemy.rect.pos.x = randomFloat(minX, maxX);

    // Start just above the window, so it enters from the top
    enemy.rect.pos.y = -enemy.rect.height / 2.0f;

    enemy.rect.vel = glm::vec2(0.0f, randomFloat(ENEMY_MIN_SPEED, ENEMY_MAX_SPEED));
    enemy.rect.color = SDL_Color{255, 165, 0, 255};   // orange
    enemy.active = true;

    enemy.fireTimer = ENEMY_FIRST_SHOT_DELAY;
}

void fireEnemyProjectiles(const Enemy& enemy) {
    glm::vec2 toPlayer = player.pos - enemy.rect.pos;
    if (glm::length(toPlayer) == 0.0f) return;   // avoid normalizing a zero vector

    glm::vec2 mainDirection = glm::normalize(toPlayer);
    glm::vec2 directions[3] = {
        mainDirection,
        glm::rotate(mainDirection, glm::radians(ENEMY_SPREAD_ANGLE)),
        glm::rotate(mainDirection, glm::radians(-ENEMY_SPREAD_ANGLE))
    };

    for (const glm::vec2& direction : directions) {
        Circle projectile(ENEMY_PROJECTILE_RADIUS, enemy.rect.pos, SDL_Color{255, 0, 255, 255});   // magenta
        projectile.vel = direction * ENEMY_PROJECTILE_SPEED;
        enemyProjectiles.push_back(projectile);
    }
}

void updateEnemyProjectiles() {
    glm::vec2 windowSize = getWindowSize();

    for (Circle& projectile : enemyProjectiles) {
        projectile.pos += projectile.vel * DT;
    }

    // Remove projectiles that left the window on any side
    enemyProjectiles.erase(
        std::remove_if(enemyProjectiles.begin(), enemyProjectiles.end(),
                       [windowSize](const Circle& p) {
                           return p.pos.x < -p.radius || p.pos.x > windowSize.x + p.radius ||
                                  p.pos.y < -p.radius || p.pos.y > windowSize.y + p.radius;
                       }),
        enemyProjectiles.end());
}

void initEnemies() {
    for (int i = 0; i < NUM_ENEMY_SLICES; i++) {
        spawnEnemy(i);
    }
    enemySpawnTimer = 0.0f;
}

void updateEnemies() {
    glm::vec2 windowSize = getWindowSize();

    // Every ENEMY_SPAWN_INTERVAL seconds, refill the slices that are clear
    enemySpawnTimer += DT;
    if (enemySpawnTimer >= ENEMY_SPAWN_INTERVAL) {
        enemySpawnTimer -= ENEMY_SPAWN_INTERVAL;
        for (int i = 0; i < NUM_ENEMY_SLICES; i++) {
            if (!enemies[i].active) {
                spawnEnemy(i);
            }
        }
    }

    for (Enemy& enemy : enemies) {
        if (!enemy.active) continue;

        enemy.rect.pos += enemy.rect.vel * DT;

        // Fire 1 s after entering the window, then every ENEMY_FIRE_INTERVAL seconds
        enemy.fireTimer -= DT;
        if (enemy.fireTimer <= 0.0f) {
            fireEnemyProjectiles(enemy);
            enemy.fireTimer += ENEMY_FIRE_INTERVAL;
        }

        // Left the window through the bottom => the slice is clear again
        if (enemy.rect.pos.y - enemy.rect.height / 2.0f > windowSize.y) {
            enemy.active = false;
        }
    }
}

// ---------- Collisions ----------

bool rectanglesOverlap(const Rectangle& a, const Rectangle& b) {
    return std::abs(a.pos.x - b.pos.x) < (a.width + b.width) / 2.0f &&
           std::abs(a.pos.y - b.pos.y) < (a.height + b.height) / 2.0f;
}

bool circleRectangleOverlap(const Circle& c, const Rectangle& r) {
    // Closest point of the rectangle to the circle's center
    glm::vec2 halfSize(r.width / 2.0f, r.height / 2.0f);
    glm::vec2 closest = glm::clamp(c.pos, r.pos - halfSize, r.pos + halfSize);
    return glm::length(c.pos - closest) < c.radius;
}

void endGame(GameState result) {
    gameState = result;

    // Everything disappears and stops
    playerProjectiles.clear();
    enemyProjectiles.clear();
    for (Enemy& enemy : enemies) {
        enemy.active = false;
    }
    player.vel = glm::vec2(0.0f, 0.0f);
    isFiring = false;

    // Red ship on loss, blue ship on victory
    if (result == GameState::LOST) {
        player.color = SDL_Color{255, 0, 0, 255};
    } else {
        player.color = SDL_Color{0, 0, 255, 255};
    }
}

void checkCollisions() {
    // Player projectiles vs enemies: both are destroyed
    bool enemyDestroyed{false};
    for (size_t i = 0; i < playerProjectiles.size(); ) {
        bool hit{false};
        for (Enemy& enemy : enemies) {
            if (enemy.active && circleRectangleOverlap(playerProjectiles[i], enemy.rect)) {
                enemy.active = false;
                hit = true;
                enemyDestroyed = true;
                break;
            }
        }
        if (hit) {
            playerProjectiles.erase(playerProjectiles.begin() + i);   // don't advance i
        } else {
            i++;
        }
    }

    // Player vs enemies => loss
    for (const Enemy& enemy : enemies) {
        if (enemy.active && rectanglesOverlap(player, enemy.rect)) {
            endGame(GameState::LOST);
            return;
        }
    }

    // Player vs enemy projectiles => loss
    for (const Circle& projectile : enemyProjectiles) {
        if (circleRectangleOverlap(projectile, player)) {
            endGame(GameState::LOST);
            return;
        }
    }

    // All enemies destroyed => victory
    if (enemyDestroyed) {
        bool anyEnemyLeft{false};
        for (const Enemy& enemy : enemies) {
            if (enemy.active) anyEnemyLeft = true;
        }
        if (!anyEnemyLeft) {
            endGame(GameState::WON);
        }
    }
}

void resetGame() {
    scrollingGrid.clear();
    initScollingRectangles();

    initPlayer();
    initEnemies();

    playerProjectiles.clear();
    enemyProjectiles.clear();
    fireCooldown = 0.0f;
    isFiring = false;
    timeSinceMouseMoved = MOUSE_STOP_DELAY;

    gameState = GameState::PLAYING;
}

void animate() {
    // When the game has ended, all movement stops (including the scrolling)
    if (gameState != GameState::PLAYING) {
        return;
    }

    updatePlayer();
    updatePlayerProjectiles();
    updateEnemies();
    updateEnemyProjectiles();
    checkCollisions();

    // Scrolling grid
    for (int i = 0; i < scrollingGrid.size(); i++) {
        Rectangle& gridRectangle = scrollingGrid.at(i);

        gridRectangle.pos += gridRectangle.vel * DT;

        if (gridRectangle.pos.y > (WINDOW_HEIGHT + gridRectangle.height / 2.0f)) {
            gridRectangle.pos.y -= gridRectangle.height * scrollingGrid.size();
        }
    }
}

void drawFrame() {
    //Clear the background
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(renderer);

    //Draw scene
    
    //Draw scrolling grid

    for (int i = 0; i < scrollingGrid.size(); i++) {
        Rectangle& gridRectangle = scrollingGrid.at(i);
        gridRectangle.draw(renderer);
    }

    for (Circle& projectile : playerProjectiles) {
        projectile.draw(renderer);
    }

    for (Circle& projectile : enemyProjectiles) {
        projectile.draw(renderer);
    }

    for (Enemy& enemy : enemies) {
        if (enemy.active) {
            enemy.rect.draw(renderer);
        }
    }

    player.draw(renderer);

    //Update window
    SDL_RenderPresent(renderer);
}

void cleanup() {
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

    SDL_zero(currentEvent);

	initScollingRectangles();

    initPlayer();
    initEnemies();

    // Hide the cursor and capture relative mouse movement
    SDL_SetWindowRelativeMouseMode(window, true);

    Uint64 lastTime = SDL_GetTicksNS();

    //Game loop
    while (!quit) {
        /*startTime = SDL_GetTicks();

        processEvents();
        animate();
        drawFrame();

        endTime = SDL_GetTicks();
        DT = (endTime - startTime) / 1000.0f;*/

        Uint64 now = SDL_GetTicksNS();
        DT = (now - lastTime) / 1'000'000'000.0f;   // nanoseconds -> seconds
        lastTime = now;

        processEvents();
        animate();
        drawFrame();
    }

    cleanup();
    return 0;
}
