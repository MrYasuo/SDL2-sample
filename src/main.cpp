#include <iostream>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "Button.h"

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;
constexpr std::string_view WINDOW_TITLE = "Guess It";

SDL_Window* gWindow;
SDL_Renderer* gRenderer;
SDL_Texture* gTexture;
SDL_Rect sourceRect;
SDL_Rect desRect;
TTF_Font* gFont;
TTF_Font* bFont;
SDL_Surface* gSurface = IMG_Load("assets/yasuo.jpg");
Button start;
Button quit;

int flags = IMG_INIT_JPG | IMG_INIT_PNG;
std::string input;
int random;
bool prompt = false;

void init();
void logSDLError(const char* msg, const char* err);
int getRandomNumber();
bool compare(std::string check, int number);
std::string returnDiff();
void menu();
bool loop();
void kill();
void game();
void playAgain();
void reDraw(std::string);
void renderText(const char* text, int x, int y, SDL_Color color, bool isCentered = false, TTF_Font*& font = gFont);

int main(int argc, char** argv) {
	init();
	menu();
	while (loop()) {
		SDL_Delay(10);
	}
	kill();
	return 0;
}

void init() {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		logSDLError("Unable to initialize SDL: %s", SDL_GetError());
	}
	// Initialize SDL_image
	if (IMG_Init(flags) != flags) {
		logSDLError("Unable to initialize SDL_image: %s", IMG_GetError());
	}
	// Initialize SDL_ttf
	if (TTF_Init() < 0) {
		logSDLError("Unable to initialize SDL_ttf: %s", TTF_GetError());
	}
	// Create SDL Window
	gWindow = SDL_CreateWindow(std::string(WINDOW_TITLE).c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
	if (gWindow == nullptr)
		logSDLError("Unable to create window: %s", SDL_GetError());
	// Create SDL Renderer
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (gRenderer == nullptr)
		logSDLError("Unable to create renderer: %s", SDL_GetError());
	// Set hint for resizing
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	// Set fixed size
	SDL_RenderSetLogicalSize(gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	// Set icon
	SDL_SetWindowIcon(gWindow, gSurface);
	// Load font
	gFont = TTF_OpenFont("OpenSans-Regular.ttf", 18);
	bFont = TTF_OpenFont("OpenSans-Bold.ttf", 28);
	if (gFont == nullptr)
		logSDLError("Unable to load font: %s", TTF_GetError());
	SDL_StartTextInput();
}

void menu() {
	// Well also can use this to load image
	// SDL_RWops* rwop;
	// rwop = SDL_RWFromFile("assets/yasuo.jpg", "rb");
	// gSurface = IMG_LoadJPG_RW(rwop);

	// Create texture from surface
	gTexture = SDL_CreateTextureFromSurface(gRenderer, gSurface);
	// Delete surface
	SDL_FreeSurface(gSurface);
	// Get the information of texture with the coordinate of sourceRect
	SDL_QueryTexture(gTexture, NULL, NULL, &sourceRect.w, &sourceRect.h);
	// resize the texture
	sourceRect.w = sourceRect.w / 10;
	sourceRect.h = sourceRect.h / 10;

	desRect.x = 0;
	desRect.y = 0;
	desRect.w = sourceRect.w;
	desRect.h = sourceRect.h;
	// Set color
	SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
	// Clear the current rendering target and apply the new color
	SDL_RenderClear(gRenderer);
	// Copy a PORTION of the texture to the current rendering target
	// Notice that if the third parameter is NULL, the entire texture will be copied to the position of the destination localtion (desRect)
	SDL_RenderCopy(gRenderer, gTexture, NULL, &desRect);
	renderText("Guess It", SCREEN_WIDTH, SCREEN_HEIGHT / 5 * 2, {255, 255, 255, 255}, true, bFont);
	// Update the screen
	start = Button(gRenderer, gSurface, gTexture, bFont, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2, 100, 100, 0, 0, 0, "Start");
	quit = Button(gRenderer, gSurface, gTexture, bFont, SCREEN_WIDTH / 4 * 3 - 100, SCREEN_HEIGHT / 2, 100, 100, 0, 0, 0, "Quit");
	SDL_RenderPresent(gRenderer);
}

bool loop() {
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		switch (e.type) {
			case SDL_QUIT:
				return false;
			case SDL_KEYDOWN: {
				if (!prompt) {
					switch (e.key.keysym.sym) {
						case SDLK_0:
						case SDLK_1:
						case SDLK_2:
						case SDLK_3:
						case SDLK_4:
						case SDLK_5:
						case SDLK_6:
						case SDLK_7:
						case SDLK_8:
						case SDLK_9: {
							if (input.length() > 3) {
								input = " ";
								reDraw("Don't type more than 3 characters");
							} else {
								reDraw(" ");
								input += e.key.keysym.sym;
								renderText(input.c_str(), (SCREEN_WIDTH - 40) / 2, SCREEN_HEIGHT / 2 + 30, {0, 0, 0, 255});
							}
							break;
						}
						case SDLK_BACKSPACE: {
							if (input.length() > 1) {
								input.pop_back();
								reDraw(" ");
							}
							break;
						}
						case SDLK_RETURN: {
							if (compare(input, random)) {
								reDraw(" ");
								renderText("You win!", SCREEN_WIDTH, SCREEN_HEIGHT / 3, {0, 255, 0, 255}, true);
								playAgain();
								return true;
							} else {
								if (input.length() > 1) {
									reDraw(returnDiff().c_str());
									return true;
								}
							}
							break;
						}
						default: {
							reDraw("Please type only numbers");
						}
					}
					break;
				} else {
					if (e.key.keysym.sym == SDLK_q)
						return false;
					else
						game();
				}
			}
			case SDL_MOUSEBUTTONDOWN: {
				if (e.button.button == SDL_BUTTON_LEFT) {
					start.update(e.motion.x, e.motion.y);
					quit.update(e.motion.x, e.motion.y);
				}
				break;
			}
		}
		if (start.isSelected) {
			SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
			game();
			start.isSelected = false;
		}
		if (quit.isSelected) {
			return false;
		}
		SDL_RenderPresent(gRenderer);
	}
	return true;
}

void renderText(const char* text, int x, int y, SDL_Color color, bool isCentered, TTF_Font*& font) {
	gSurface = TTF_RenderText_Blended(font, text, color);
	gTexture = SDL_CreateTextureFromSurface(gRenderer, gSurface);
	SDL_FreeSurface(gSurface);
	if (isCentered) {
		desRect.x = (x - gSurface->w) / 2;
		desRect.y = (y - gSurface->h) / 2;
	} else {
		desRect.x = x;
		desRect.y = y;
	}
	desRect.w = gSurface->w;
	desRect.h = gSurface->h;
	SDL_RenderCopy(gRenderer, gTexture, NULL, &desRect);
}

void game() {
	random = getRandomNumber();
	prompt = false;
	// std::cout << random << std::endl;
	input = " ";
	reDraw(" ");
}

void playAgain() {
	renderText("Press q to quit or press other keys to continue", SCREEN_WIDTH, SCREEN_HEIGHT / 3 * 2, {255, 0, 0, 255}, true);
	prompt = true;
}

void reDraw(std::string message) {
	SDL_RenderClear(gRenderer);
	renderText(message.c_str(), SCREEN_WIDTH, SCREEN_HEIGHT / 3, {255, 0, 0, 255}, true);
	renderText("Type a number to guess:", SCREEN_WIDTH, SCREEN_HEIGHT, {0, 0, 0, 255}, true);
	gSurface = IMG_Load("assets/line.png");
	gTexture = SDL_CreateTextureFromSurface(gRenderer, gSurface);
	SDL_FreeSurface(gSurface);
	desRect.x = (SCREEN_WIDTH - 50) / 2;
	desRect.y = SCREEN_HEIGHT / 2 + 30;
	desRect.w = 50;
	desRect.h = 50;
	SDL_RenderCopy(gRenderer, gTexture, NULL, &desRect);
	renderText(input.c_str(), (SCREEN_WIDTH - 40) / 2, SCREEN_HEIGHT / 2 + 30, {0, 0, 0, 255});
	SDL_RenderPresent(gRenderer);
}

void kill() {
	SDL_StopTextInput();
	TTF_CloseFont(gFont);
	TTF_CloseFont(bFont);
	SDL_DestroyTexture(gTexture);
	gTexture = nullptr;
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;
	SDL_Quit();
}

void logSDLError(const char* msg, const char* err) {
	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, msg, err);
	exit(0);
}