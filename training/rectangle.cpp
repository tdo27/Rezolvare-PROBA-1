#include "rectangle.h"

Rectangle::Rectangle() {
    this->width = 0;
    this->height = 0;
    this->pos = glm::vec2(0.0f, 0.0f);
    this->color = SDL_Color{0, 0, 0, 255};
}

Rectangle::Rectangle(float width, float height, glm::vec2 pos, SDL_Color color) {
    this->width = width;
    this->height = height;
    this->pos = pos;
    this->color = color;
}

void DrawRectangle(SDL_Renderer *renderer, Rectangle rectangle) {
    SDL_FRect tmpRect;
    tmpRect.x = rectangle.pos.x - rectangle.width / 2;
    tmpRect.y = rectangle.pos.y - rectangle.height / 2;
    tmpRect.w = rectangle.width;
    tmpRect.h = rectangle.height;
    SDL_SetRenderDrawColor(renderer, rectangle.color.r, rectangle.color.g, rectangle.color.b, rectangle.color.a);
    SDL_RenderRect(renderer, &tmpRect);
}

void FillRectangle(SDL_Renderer *renderer, Rectangle rectangle) {
    SDL_FRect tmpRect;
    tmpRect.x = rectangle.pos.x - rectangle.width / 2;
    tmpRect.y = rectangle.pos.y - rectangle.height / 2;
    tmpRect.w = rectangle.width;
    tmpRect.h = rectangle.height;
    SDL_SetRenderDrawColor(renderer, rectangle.color.r, rectangle.color.g, rectangle.color.b, rectangle.color.a);
    SDL_RenderFillRect(renderer, &tmpRect);
}

void FillBorderedRectangle(SDL_Renderer *renderer, Rectangle rectangle) {
    SDL_FRect tmpRect;
    tmpRect.x = rectangle.pos.x - rectangle.width / 2;
    tmpRect.y = rectangle.pos.y - rectangle.height / 2;
    tmpRect.w = rectangle.width;
    tmpRect.h = rectangle.height;
    SDL_SetRenderDrawColor(renderer, rectangle.color.r, rectangle.color.g, rectangle.color.b, rectangle.color.a);
    SDL_RenderFillRect(renderer, &tmpRect);
    SDL_SetRenderDrawColor(renderer, rectangle.color.r / 2, rectangle.color.g / 2, rectangle.color.b / 2, rectangle.color.a / 2);
    SDL_RenderRect(renderer, &tmpRect);
}

void Rectangle::draw(SDL_Renderer *renderer) {
    FillBorderedRectangle(renderer, *this);
}
