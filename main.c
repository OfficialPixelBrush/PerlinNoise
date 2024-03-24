#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#define maxInt (~0)
#define sizeOfIntInBits (sizeof(int)*8)
SDL_Renderer *renderer;
int status = 1;
int maxX = 512;
int maxY = 512;
float perlinNoiseScale = 32.0f;

// Utility macros
#define CHECK_ERROR(test, message) \
    do { \
        if((test)) { \
            fprintf(stderr, "%s\n", (message)); \
            exit(1); \
        } \
    } while(0)

int abs(int input) {
	if (input < 0) {
		input *= -1;
	}
	return input;
}

typedef struct {
	float r,g,b;
} color;

typedef struct {
	color c;
	char height;
} tile;

typedef struct {
	int x,y;
} int2;

typedef struct {
    float x, y;
} vector2;

/*
This pseudoranom algorithm will use the XOR-Shift algorithm
*/
int pseudoRandom(int input, int max) {
	//printf("%X -> ", input);
	int seed = 1593247432508327425 % (maxInt);
	int sampleBit = 27; // Sample nth bit
	int bitshifts = 154; // Shift n times
	int result = input ^ seed;
	for (int i = 0; i < bitshifts; i++) {
		int temp1;
		int temp2;
		temp1 = result & 1;
		temp2 = (result >> sampleBit) & 1;
		int xorResult = temp1 ^ temp2;
		//printf("%d: %X, %X\n", i, result, xorResult);
		result = result >> 1;
		xorResult = xorResult << (sizeof(int)*8-1);
		result &= ~(1 << (sizeof(int)*8-1));
		result = result | xorResult;
	}
	result = abs(result % max);
	//printf("%d: %d\n", input, result);
	//printf("%X\n", result);
	return result;
}

// Definitely a bad approach!
int getPositionInt2(int2 point) {
	int position = point.x << sizeOfIntInBits/2;
	position |= point.y;
	return position;
}

int getDistance(int2 p1, int2 p2) {
    int dx = abs(p2.x - p1.x);
    int dy = abs(p2.y - p1.y);

    /*
	// Wrap the distances around the torus
    int wrapped_dx = (dx < width - dx) ? dx : width - dx;
    int wrapped_dy = (dy < height - dy) ? dy : height - dy;

    // Calculate the wrapped distance
    int wrapped_dist = sqrt(wrapped_dx * wrapped_dx + wrapped_dy * wrapped_dy);
	*/
    int dist = sqrt(dx * dx + dy * dy);
    return dist;
}

/* Function to linearly interpolate between a0 and a1
 * Weight w should be in the range [0.0, 1.0]
 */
float interpolate(float a0, float a1, float w) {
    /* // You may want clamping by inserting:
     * if (0.0 > w) return a0;
     * if (1.0 < w) return a1;
     */
    //return (a1 - a0) * w + a0;
    /* // Use this cubic interpolation [[Smoothstep]] instead, for a smooth appearance:
     * return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
     *
     * // Use [[Smootherstep]] for an even smoother result with a second derivative equal to zero on boundaries:
     * return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
     */
	 return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
}

/* Create pseudorandom direction vector
 */
vector2 randomGradient(int ix, int iy) {
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; // rotation width
    unsigned a = ix, b = iy;
    a *= 3284157443; b ^= a << s | a >> w-s;
    b *= 1911520717; a ^= b << s | b >> w-s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
    vector2 v;
    v.x = cos(random); v.y = sin(random);
    return v;
}

// Computes the dot product of the distance and gradient vectors.
float dotGridGradient(int ix, int iy, float x, float y) {
    // Get gradient from integer coordinates
    vector2 gradient = randomGradient(ix, iy);

    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // Compute the dot-product
    return (dx*gradient.x + dy*gradient.y);
}

// Compute Perlin noise at coordinates x, y
float perlin(float x, float y) {
	x /= perlinNoiseScale;
	y /= perlinNoiseScale;
    // Determine grid cell coordinates
    int x0 = (int)floor(x);
    int x1 = x0 + 1;
    int y0 = (int)floor(y);
    int y1 = y0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;

    n0 = dotGridGradient(x0, y0, x, y);
    n1 = dotGridGradient(x1, y0, x, y);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    ix1 = interpolate(n0, n1, sx);

    value = interpolate(ix0, ix1, sy);
	value *= 0.5;
	value += 0.5;
	//printf("%f,%f: %d\n", x,y,value);
    return value; // Will return in range -1 to 1. To make it in range 0 to 1, multiply by 0.5 and add 0.5
}

// Self-written
// Returns value from -1.0 to 1.0
float perlin2d(int x, int y) {

}

int initSDL() {
    // Create an SDL window
    SDL_Window *window = SDL_CreateWindow("Perlin Noise", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, maxX,maxY, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
    CHECK_ERROR(window == NULL, SDL_GetError());
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); 
    CHECK_ERROR(renderer == NULL, SDL_GetError());
	return 0;
}


int WinMain() {
	initSDL();
	//printf("Hello, World!\n");
	int i = 0;
	int t = 0;
	int x, y;
    SDL_Event event;
    while(status) {
		for (y = 0; y < maxY; y++) {
			for (x = 0; x < maxX; x++) {
				int2 currentPoint;
				currentPoint.x = x/16;
				currentPoint.y = y/16;
				/*
				// Get Vector from -1 to 1
				float direction = (pseudoRandom(getPosition(currentPoint),2000)-1000)/1000;
				
				int color = getDistance(currentPoint, currentPoint);
				*/
				color c;
				float height = (((float)pseudoRandom(getPositionInt2(currentPoint),1000))/1000.0f);
				height *= 255;
				if (height < 128) {
					c.r = 0.1;
					c.g = 0.1;
					c.b = 1.0;
				} else {
					c.r = 0.3;
					c.g = 1.0;
					c.b = 0.1;
				}
				height /= 255.0;
				c.r *= height;
				c.g *= height;
				c.b *= height;
				SDL_SetRenderDrawColor(renderer,(int)(c.r*255),(int)(c.g*255),(int)(c.b*255),255);
				SDL_RenderDrawPoint(renderer, x, y);
				//printf("%d,%d: %d\n", x,y,pseudoRandom(getPosition(x,y), 100));
			}
		}
		SDL_Delay(16);
		SDL_RenderPresent(renderer);
		t+=5;
        // Process events
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                status = 0;
            } else if(event.type == SDL_KEYDOWN) {
                const char *key = SDL_GetKeyName(event.key.keysym.sym);
				// Exit
                if(strcmp(key, "Q") == 0) {
                    status = 0;
					SDL_DestroyRenderer(renderer);
					SDL_Quit();
					return 0;
				}
				if (strcmp(key, "W") == 0) {
					SDL_RenderPresent(renderer);
				}
			}
		}
	}
	return 0;
}