#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<stdint.h>
#include<SDL.h>
#include<time.h>

typedef struct coors {
	int x;
	int y;
} Coors;

typedef struct Fruit {
	Coors food_pos;
} Fruit;

typedef struct Snake {
	Coors pos;
	Coors v;
	Coors tail[256];
	int tail_start;
	int tail_end;
	int tail_len;
	uint32_t accumulator;
} Snake;

void draw(Snake snake, SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	for (int i = 0; i < snake.tail_len; i++) {
		Coors tail_pos = snake.tail[(snake.tail_start + i) % 256];
		SDL_Rect r = { 15 * tail_pos.x, 15 * tail_pos.y, 15, 15 };
		SDL_RenderFillRect(renderer, &r);
	}
	SDL_Rect r = { 15 * snake.pos.x, 15 * snake.pos.y, 15, 15 };
	SDL_RenderFillRect(renderer, &r);
}
void drawFruit(Fruit fruit, SDL_Renderer* renderer) {
	SDL_Rect r = { 15 * fruit.food_pos.x, 15 * fruit.food_pos.y, 15, 15 };
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
	SDL_RenderFillRect(renderer, &r);
}
int main(int argc, char* args[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("Snake Game by Aleksander Bies", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Event event;

	bool running = true;
	Snake snake;
	snake.pos.x = 0;
	snake.pos.y = 0;
	snake.v.x = 0;
	snake.v.y = 0;
	snake.tail_start = 0;
	snake.tail_end = 0;
	snake.tail_len = 20;
	snake.accumulator = 0;

	Fruit fruit;
	fruit.food_pos.x = 0;
	fruit.food_pos.y = 0;

	uint32_t cur_time = 0, prev_time, delta_time;

	while (running) {
		prev_time = cur_time;
		cur_time = SDL_GetTicks();
		delta_time = cur_time - prev_time;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
			else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = false;
					break;
				case SDLK_UP:
					if (snake.v.y != 1) {
						snake.v.y = -1;
						snake.v.x = 0;
					}
					break;
				case SDLK_DOWN:
					if (snake.v.y != -1) {
						snake.v.y = 1;
						snake.v.x = 0;
					}
					break;
				case SDLK_LEFT:
					if (snake.v.x != 1) {
						snake.v.y = 0;
						snake.v.x = -1;
					}
					break;
				case SDLK_RIGHT:
					if (snake.v.x != -1) {
						snake.v.y = 0;
						snake.v.x = 1;
					}
					break;
				}
			}
		}
		snake.accumulator = snake.accumulator + delta_time;

		if (snake.accumulator > 100) {
			snake.accumulator = 0;
			snake.tail[snake.tail_end % 256] = snake.pos;
			snake.tail_start += 1;
			snake.tail_end += 1;
			snake.pos.x = snake.pos.x + snake.v.x;
			snake.pos.y = snake.pos.y + snake.v.y;
			if (snake.pos.x < 0 || snake.pos.y < 0 || snake.pos.x >= (600 / 15) || snake.pos.y >= (600 / 15)) {
				running = false;
			}

			if (fruit.food_pos.x == snake.pos.x && fruit.food_pos.y == snake.pos.y) {
				snake.tail_len += 1;
				snake.tail_start -= 1;
				fruit.food_pos.x = rand() % (600 / 15);
				fruit.food_pos.y = rand() % (600 / 15);
				for (int i = 0; i < snake.tail_len; i++) {
					Coors tail_pos = snake.tail[(snake.tail_start + i) % 256];
					if (fruit.food_pos.x == tail_pos.x && fruit.food_pos.y == tail_pos.y) {
						fruit.food_pos.x = rand() % (600 / 15);
						fruit.food_pos.y = rand() % (600 / 15);
					}
				}
			}
			for (int i = 0; i < snake.tail_len; i++) {
				Coors tail_pos = snake.tail[(snake.tail_start + i) % 256];
				if (tail_pos.x == snake.pos.x && tail_pos.y == snake.pos.y) {
					snake.tail_len = 0;
					snake.tail_start = 0;
					snake.tail_end = 0;
				}
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		draw(snake, renderer);
		drawFruit(fruit, renderer);
		SDL_RenderPresent(renderer);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}