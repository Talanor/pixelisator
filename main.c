#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define THRESHOLD 75

void shuffle(int **array, size_t n)
{
    if (n > 1)
    {
        size_t i;

        for (i = 0; i < n - 1; i++)
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);


	  int temp[2];
	 
	  temp[0] = array[j][0];
	  temp[1] = array[j][1];
          array[j][0] = array[i][0];
          array[j][1] = array[i][1];
          array[i][0] = temp[0];
          array[i][1] = temp[1];
        }
    }
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

int main (int argc, char** argv)
{
    srand(time(NULL));

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG);

    if (argc != 2) {
    	fprintf(stderr, "Missing argument\n");
	return EXIT_FAILURE;
    }

    SDL_Surface * image = IMG_Load(argv[1]);
    image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_ARGB8888, 0);


    Uint32 *pixels = malloc(image->w * image->h * sizeof(Uint32));
    Uint32 *screen = malloc(image->w * image->h * sizeof(Uint32));;
    int **coords = malloc(image->w * image->h * sizeof(*coords));

    for (int i = 0 ; i < image->h ; ++i) {
	for (int j = 0 ; j < image->w ; ++j) {
	    pixels[i * image->w + j] = getpixel(image, j, i);
	    coords[i * image->w + j] = malloc(2 * sizeof(**coords));
	    coords[i * image->w + j][0] = j;
	    coords[i * image->w + j][1] = i;
	}
    }

    shuffle(coords, image->w * image->h);

    SDL_Window* window = NULL;

    window = SDL_CreateWindow
    (
        "Pixelizator", 0,
        0,
        1,
        1,
        SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS | SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_INPUT_GRABBED
    );
    SDL_ShowCursor(SDL_DISABLE);


    int winHeight, winWidth, imageWidth = image->w, imageHeight = image->h;
    SDL_Rect dest;
    bool resized = false;
    
    SDL_GetWindowSize(window, &winWidth, &winHeight);

    if (image->w > winWidth || image->h > winHeight) {
	resized = true;

	float widthCoeff = ((float)image->w / (float)winWidth);
	float heightCoeff = ((float)image->h / (float)winHeight);

	float maxCoeff = (widthCoeff > heightCoeff) ? widthCoeff : heightCoeff;	

	imageWidth = image->w / maxCoeff;
	imageHeight = image->h / maxCoeff;
    }
    
    if (resized == true || (image->w <= winWidth && image->h <= winHeight)) {
	dest.w = imageWidth;
	dest.h = imageHeight;
	dest.x = (int)(winWidth / 2.0 - imageWidth / 2.0);
	dest.y = (int)(winHeight / 2.0 - imageHeight / 2.0);
    }

    SDL_Renderer* renderer = NULL;
    renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture * texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, image->w, image->h);

    bool again = true, end = false, pause = false;
    int i = 0;

    while (again == true) {
	Uint32 now = SDL_GetTicks();

	if (pause == false || end == true) {
		if (i < image->w * image->h) {
			do {
	  			screen[coords[i][1] * image->w + coords[i][0]] =
					pixels[coords[i][1] * image->w + coords[i][0]];
				++i;
			} while ((i % ((image->w * image->h) / 9000) != 0 || end == true) && i < image->w * image->h);
		}

        	SDL_UpdateTexture(texture, NULL, screen, image->w * sizeof(Uint32));
	}
    	
	SDL_Event event;
    	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				again = false;
			} else if (event.key.keysym.sym == SDLK_SPACE) {
				end = true;
			} else if (event.key.keysym.sym == SDLK_p) {
				pause = !pause;
			}
		}
    	}

	if (pause == false || end == true) {
        	SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, &dest);
        	SDL_RenderPresent(renderer);
	}

	Uint32 last = SDL_GetTicks();
	if (last - now < THRESHOLD) {
		SDL_Delay(THRESHOLD - (last - now));
	}
    }

    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
