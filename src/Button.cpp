#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>

#include "Button.h"

Button::Button(SDL_Renderer*& renderer, SDL_Surface*& gSurface, SDL_Texture*& gTexture, TTF_Font*& bFont, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, const char* name) {
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	SDL_RenderFillRect(renderer, &rect);
	SDL_Color foreground{255, 255, 255, 255};
	gSurface = TTF_RenderText_Blended(bFont, name, foreground);
	gTexture = SDL_CreateTextureFromSurface(renderer, gSurface);
	rect.w = gSurface->w;
	rect.h = gSurface->h;
	rect.x = x + (w - rect.w) / 2;
	rect.y = y + (h - rect.h) / 2;
	SDL_RenderCopy(renderer, gTexture, NULL, &rect);
}

Button::~Button() {}

Button::Button() {}

void Button::update(int x, int y) {
	if (x <= rect.x + rect.w && x >= rect.x && y <= rect.y + rect.h && y >= rect.y) {
		isSelected = true;
	} else {
		isSelected = false;
	}
}