#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<stdint.h>
#include<SDL.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include<time.h>

//typedef struct Coors to save pair of Coordinates (x,y)
typedef struct Coors {
	int x;
	int y;
} Coors;

//typedef struct Coors to save Coordinates (x,y) of fruits
typedef struct Fruit {
	Coors fruit_pos;
} Fruit;


//typedef struct Snake
typedef struct Snake {
	Coors pos; //position (x,y) of snake's head
	Coors direction; //direction of snake's moving
	Coors tail[256]; //array of coordinates of snake's tail
	int tail_start;  //index of snake's tail beginning
	int tail_end;    //index of snake's tail end
	int tail_len;    //length of snake's tail
	uint32_t acc;    //accumulator (needed in while loop)
} Snake;

void draw(Snake snake, SDL_Renderer* renderer) {
	//function drawing snak
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

	//it draws snake's tail
	for (int i = 0; i < snake.tail_len; i++) {
		Coors tail_pos = snake.tail[(snake.tail_start + i) % 256];
		SDL_Rect r = { 15 * tail_pos.x, 15 * tail_pos.y, 15, 15 };
		SDL_RenderFillRect(renderer, &r);
	}
	//if - else checks that snake's head is out of window
	if (snake.pos.x < 0) {
		snake.pos.x = 0;
	}
	else if (snake.pos.y < 0) {
		snake.pos.y = 0;
	}
	else if (snake.pos.x > (600 / 15)) {
		snake.pos.x = (600 / 15);
	}
	else if (snake.pos.y > (600 / 15)) {
		snake.pos.y = (600 / 15) - 1;
	}

	SDL_Rect r = { 15 * snake.pos.x, 15 * snake.pos.y, 15, 15 }; //snake's head
	SDL_RenderFillRect(renderer, &r);
}
void drawFruit(Fruit fruit, SDL_Renderer* renderer) {
	//function draws fruits
	SDL_Rect r = { 15 * fruit.fruit_pos.x, 15 * fruit.fruit_pos.y, 15, 15 };
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
	SDL_RenderFillRect(renderer, &r);
}
static void printText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int32_t x, int32_t y, SDL_Color color) {
	//this function draws text on screen using SDL_ttf.h library
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect rect;
	rect.y = y;
	rect.w = surface->w;
	rect.h = surface->h;
	rect.x = x; 
	SDL_RenderCopy(renderer, texture, 0, &rect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}
int main(int argc, char* args[]) {

	//char array to save and print text with score
	char buffor[2048];

	//SDL initialization: renderer, fonts, music
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	TTF_Font* title_font = TTF_OpenFont("Pacifico.ttf", 45);
	TTF_Font* score_font = TTF_OpenFont("Pacifico.ttf", 35);
	TTF_Font* gameover_font = TTF_OpenFont("Pacifico.ttf", 60);
	TTF_Font* text_font = TTF_OpenFont("JosefinSans.ttf", 30);
	SDL_Window* window = SDL_CreateWindow("Snake Game by Aleksander Bies", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 600, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	Mix_Music* bgMusic = Mix_LoadMUS("snake.mp3");
	Mix_PlayMusic(bgMusic, -1);
	SDL_Rect line = { 615,0, 5, 600 };
	SDL_Color greenyellow = { 173,255,47,255 };
	SDL_Color yellow = { 255,255,0,255 };
	SDL_Event event;

	//booleans to check game status
	bool running = true;
	bool wanttoplay = true;
	bool beforegame = true;

	//snake initialization parametres 
	Snake snake;
	snake.pos.x = 0;
	snake.pos.y = 0;
	snake.direction.x = 0;
	snake.direction.y = 0;
	snake.tail_start = 0;
	snake.tail_end = 0;
	snake.tail_len = 20;
	snake.acc = 0;

	//fruit initialization parametres
	Fruit fruit;
	fruit.fruit_pos.x = 0;
	fruit.fruit_pos.y = 0;

	//time initialization parmetres
	uint32_t cur_time = 0, prev_time, delta_time;

	//unsigned int score, it increments when snake will eat a fruit
	unsigned score = 0; 

	//main loop
	while (running || wanttoplay) {
		prev_time = cur_time;
		cur_time = SDL_GetTicks();
		delta_time = cur_time - prev_time;
		while (SDL_PollEvent(&event)) {

			//event type which let us to close the window by "x" in the upper right corner 
			if (event.type == SDL_QUIT) {
				running = false;
				wanttoplay = false;
			}
			else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					//ESC key to close the progam
					running = false;
					wanttoplay = false;
					break;
				case SDLK_SPACE:
					if (beforegame) {
						snake.direction.x = 1;
						beforegame = false;
					}
					if (!running) {
						running = true;
						memset(snake.tail, 0, sizeof(snake.tail));
						snake.pos.x = 0;
						snake.pos.y = 0;
						snake.direction.x = 0;
						snake.direction.y = 0;
						snake.tail_start = 0;
						snake.tail_end = 0;
						snake.tail_len = 20;
						snake.acc = 0;
						beforegame = true;
					}
					break;
				//control the snake
				case SDLK_UP:
					if (snake.direction.y != 1 && running && !beforegame) {
						snake.direction.y = -1;
						snake.direction.x = 0;
					}
					break;
				case SDLK_DOWN:
					if (snake.direction.y != -1 && running && !beforegame) {
						snake.direction.y = 1;
						snake.direction.x = 0;
					}
					break;
				case SDLK_LEFT:
					if (snake.direction.x != 1 && running && !beforegame) {
						snake.direction.y = 0;
						snake.direction.x = -1;
					}
					break;
				case SDLK_RIGHT:
					if (snake.direction.x != -1 && running && !beforegame) {
						snake.direction.y = 0;
						snake.direction.x = 1;
					}
					break;
				}
			}
		}
		snake.acc = snake.acc + delta_time;

		if (snake.acc > 100) {
			snake.acc = 0;
			snake.tail[snake.tail_end % 256] = snake.pos;
			snake.tail_start += 1;
			snake.tail_end += 1;
			snake.pos.x = snake.pos.x + snake.direction.x;
			snake.pos.y = snake.pos.y + snake.direction.y;
			
			//condition to check that snake's head hit the walls
			if (snake.pos.x < 0 || snake.pos.y < 0 || snake.pos.x >= (600 / 15) || snake.pos.y >= (600 / 15)) {
				running = false;
				snake.tail_len = 0;
				snake.tail_start = 0;
				snake.tail_end = 0;
			}
			//condition to check that snake ate a frut
			if (fruit.fruit_pos.x == snake.pos.x && fruit.fruit_pos.y == snake.pos.y) {
				score += 1;
				snake.tail_len += 1;
				snake.tail_start -= 1;
				fruit.fruit_pos.x = rand() % (600 / 15);
				fruit.fruit_pos.y = rand() % (600 / 15);
				//when new fruit is generated on snake's tail, fruit will be generated one more time
				for (int i = 0; i < snake.tail_len; i++) {
					Coors tail_pos = snake.tail[(snake.tail_start + i) % 256];
					if (fruit.fruit_pos.x == tail_pos.x && fruit.fruit_pos.y == tail_pos.y) {
						fruit.fruit_pos.x = rand() % (600 / 15);
						fruit.fruit_pos.y = rand() % (600 / 15);
					}
				}
			}
			//if snake's head hit snake's tail player lost his points 
			//and snake comes back to his start parametres
			for (int i = 0; i < snake.tail_len; i++) {
				Coors tail_pos = snake.tail[(snake.tail_start + i) % 256];
				if (tail_pos.x == snake.pos.x && tail_pos.y == snake.pos.y) {
					snake.tail_len = 0;
					snake.tail_start = 0;
					snake.tail_end = 0;
					score = 0;
				}
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

		//printing game title
		printText(renderer, title_font, "Snake Game", 640, 130, yellow);
		
		//rendering of game over screen
		if (!running && wanttoplay) {
			printText(renderer, gameover_font, "Game over!", 140, 200, yellow);
			printText(renderer, text_font, "Press space to play again...", 100, 310, greenyellow);
			printText(renderer, text_font, "or ESC to close.", 170, 350, greenyellow);
		}
		//rendering of start screen
		if (beforegame) {
			printText(renderer, text_font, "To control the snake press keys:", 80, 70, greenyellow);
			printText(renderer, text_font, "UP, DOWN, LEFT or RIGHT", 100, 100, greenyellow);
			printText(renderer, gameover_font, "Snake Game", 140, 200, yellow);
			printText(renderer, text_font, "Press space to start", 170, 310, greenyellow);
		}

		snprintf(buffor, sizeof(buffor), "Your score: %d", score);
		printText(renderer, score_font, buffor, 660, 200, yellow); //printing score
		
		SDL_RenderFillRect(renderer, &line); //drawing right wall

		//drawing snake and fruit
		draw(snake, renderer);
		drawFruit(fruit, renderer);
		SDL_RenderPresent(renderer);
	}
	TTF_CloseFont(title_font);
	TTF_CloseFont(score_font);
	TTF_CloseFont(gameover_font);
	TTF_CloseFont(text_font);
	TTF_Quit();
	Mix_FreeMusic(bgMusic);
	Mix_CloseAudio();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}