/* Alphar: 2020.04.19 */

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h> // Using SDL
#include "SDL2_gfxPrimitives.h" // Using SDL2_gfx
//#include "SDL_image.h"  // Using SDL2 image extension library 
#include <time.h>
#include <cmath>
//#define G 6.67
#define K 1000
//Screen dimension constants
const int WIDTH = 1200;
const int HEIGHT = 900;

/*typedef struct {
	float r;
	float g;
	float b;
}color;

color colors[8] = { {255,0,0},//red
					{255,165,0},//orange
					{255,255,0},//yellow
					{0,255,0}, //green
					{0,255,255},//sky blue
					{0,0,255},//blue
					{255,0,255},//purple
					{255,255,255}};//pink
					*/
typedef struct {
	double x[255];
	double y[255];
	int size;
}trajectory;


typedef struct {
	double m;
	double r;
	double x;
	double y;
	double velx;
	double vely;
	bool exists;
	bool move;
	bool inCluster;
	int cNum;
	trajectory tale;
	//color tcol[2];
	trajectory trag;
	double opaq;
} planet;

typedef struct {
	int n;
	double M;
	planet planets[100];
	//double m[100];
	//double r[100];
	double x[100];
	double y[100];
	double centerX;
	double centerY;
	double velx;
	double vely;
	double angvel;
}cluster;

typedef struct {
	double m;
	double r;
	double x;
	double y;
	//int rings;
	float ring[5];
	float opaq[5];
}blackHole;


enum MouseState
{
	NONE = 0,
	LB_PR = 1,
	LB_RE = 2,
};

enum backColor {
	WHITE = 255,
	BLACK = 0,
};

int initSDL(); // Starts up SDL and creates window
void closeSDL(); // Frees media and shuts down SDL
void mouseHandleEvent(SDL_Event* e, MouseState* mouseState, int* x, int* y);

void setBlackHole(blackHole *h);
void resetPlanet(planet* planet1);
//void resetCluster(cluster* c);
//void initCluster(planet *p1, planet *p2, cluster *c);
//void addToCluster(planet *p, cluster *c);

void drawPlanet(planet p, backColor c);
void drawBlackHole(blackHole h, backColor c);
void drawBlackHoleRings(blackHole *h, backColor c);
//void drawCluster(cluster cluster1, backColor c);

void movePlanet(planet* p, int pNum, blackHole holes[], int hSize, planet planets[], int pSize, cluster *c);
//void collide (planet* p1, planet *p2);
void drawPull(planet planet1, int pullx, int pully, backColor c);
void drawTrajectory(planet p, int pNum, blackHole holes[], int hSize, planet planets[], int pSize, cluster c, int pullx, int pully);
void makeTrajectory(planet* p, int pNum, blackHole holes[], int hSize, planet planets[], int pSize, cluster c);
void drawTale(planet p);
void taleUpdate(planet* p);
void drawFinalTrajectory(planet p);


SDL_Window* window = NULL; // The window we'll be rendering to
SDL_Renderer* renderer = NULL; // The window renderer


int initSDL()
{
	// Initialize SDL	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		// Error Handling		
		printf("SDL_Init failed: %s\n", SDL_GetError());
		return 1;
	}

	// Create window	
	// SDL_WINDOWPOS_UNDEFINED: Used to indicate that you don't care what the window position is.
	window = SDL_CreateWindow("VertiGo!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
		SDL_Quit();
		return 2;
	}

	/* Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image failed: %s\n", IMG_GetError());
		return 3;
	}*/

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL)
	{
		SDL_DestroyWindow(window);
		printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
		SDL_Quit();
		return 3;
	}

	return 0;
}

void closeSDL()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	//IMG_Quit();
	SDL_Quit();
}

void mouseHandleEvent(SDL_Event* e, MouseState* mouseState, int* x, int* y)
{
	//SDL_GetMouseState(x, y);
	//*mouseState = NONE;
	if (e->type == SDL_MOUSEMOTION || ((e->button.button == SDL_BUTTON_LEFT) &&
		(e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)))
	{
		SDL_GetMouseState(x,y);
		if (e->button.button == SDL_BUTTON_LEFT)
		{
			*mouseState = NONE;
			switch (e->type)
			{
			case SDL_MOUSEBUTTONDOWN:
				*mouseState = LB_PR;
				break;
			case SDL_MOUSEBUTTONUP:
				*mouseState = LB_RE;
				break;
			}
		}
	}
}

void setBlackHole(blackHole * h) 
{
	h->m = 100000;
	h->r = 25;
	//double x;
	//double y;
	//float maxopaq = 100;
	//float dopaq = maxopaq / 5;
	for (int i = 0; i < 5; i++) {
		h->ring[i] = (5 - i) * h->r;
		h->opaq[i] = 10 * i;
	}
}

void resetPlanet(planet* planet1) 
{
	planet1->m = 1;
	planet1->r = 10;
	planet1->velx = 10;
	planet1->vely = -10;
	planet1->exists = false;
	planet1->move = false;
	//planet1->inCluster = false;
	//planet1->tale.size = 200;
	//planet1->tcol[0] = colors[rand() % 8];
	//planet1->tcol[1] = colors[rand() % 8];
}

void resetCluster(cluster* c) {
	c->n = 0;
	c->M = 0;
	//planet planets[100];
	//for (int i = 0; i < c->n; i++)
	planet *p = new planet[100];
	resetPlanet(&p[0]);
	c->planets[0] = *p;
	//double x[100];
	//double y[100];
	c->centerX = 0;
	c->centerY = 0;
	//double velx;
	//double vely;
	//double angvel;
	c->velx = 0;
	c->vely = 0;
}

void initCluster(planet *p1, planet *p2, cluster *c) {
	c->n = 2;
	c->planets[0] = *p1;
	c->planets[0] = *p2;
	c->M = p1->m + p2->m;
	c->centerX = (p1->m * p1->x + p2->m * p2->x)/c->M;
	c->centerY = (p1->m * p1->y + p2->m * p2->y) / c->M;
	c->x[0] = p1->x - c->centerX;
	c->y[0] = p1->y - c->centerY;
	c->x[1] = p2->x - c->centerX;
	c->y[1] = p2->y - c->centerY;
	c->velx = (p1->m * p1->velx + p2->m * p2->velx) / c->M;
	c->vely = (p1->m * p1->vely + p2->m * p2->vely) / c->M;
	//int angvel;
	p1->inCluster = true;
	p2->inCluster = true;
}

void addToCluster(planet* p, cluster *c) {
	c->n++;
	c->planets[c->n - 1] = *p;
	c->centerX = (c->centerX * c->M + p->m * p->x) / (p->m + c->M);
	c->centerY = (c->M * c->centerY + p->m * p->y) / (p->m + c->M);
	
	c->x[c->n - 1] = p->x - c->centerX;
	c->y[c->n - 1] = p->y - c->centerY;
	
	c->velx = (c->velx * c->M + p->m * p->velx) / (p->m + c->M);
	c->vely = (c->vely * c->M + p->m * p->vely) / (p->m + c->M);
	c->M += p->m;
	//int angvel;
	p->inCluster = true;
	//p->cNum = cNum;
}

void drawBlackHole(blackHole h, backColor c)
{
	aaFilledEllipseRGBA(renderer, h.x, h.y, h.r, h.r, 255-c, 255-c, 255-c, 255);
}

void drawBlackHoleRings(blackHole* h, backColor c)
{
	for (int i = 0; i < 5; i++) {
		aaFilledEllipseRGBA(renderer, h->x, h->y, h->ring[i], h->ring[i], 255-c, 255-c, 255-c, h->opaq[i]);
		if (h->ring[i] <= h->r) {
			h->ring[i] = 5 * h->r;
			h->opaq[i] = 0;
		}
		else {
			h->ring[i]--;
			h->opaq[i]+= 0.2;
		}
	}
}

void drawPlanet(planet p, backColor c)
{
	aaFilledEllipseRGBA(renderer, p.x, p.y, p.r, p.r, 255-c, 255-c, 255-c, 255);
}

void drawCluster(cluster c, backColor col) {
	for (int i = 0; i < c.n; i++)
			if (c.planets[i].exists)
				drawPlanet(c.planets[i], col);
}

/*void collide(planet* p1, planet* p2) {
	float v1 = p1->velx * p1->velx + p1->vely * p1->vely;
	float v2 = p2->velx * p2->velx + p2->vely * p2->vely;
	printf("initial velocities: %lf %lf\n", v1, v2);
	float v1par, v2par, v1perp, v2perp;
	float distance = p1->r + p2->r;
	float x = p2->x - p1->x;
	float y = p2->y - p1->y;
	v1par = x * p1->velx / distance + y * p1->vely / distance;
	v1perp = y * p1->velx / distance + x * p1->vely / distance;//const
	v2par = x * p2->velx / distance + y * p2->vely / distance;
	v2perp = y * p2->velx / distance + x * p2->vely / distance;//const
	float vparnew = (p1->m * v1par + p2->m * v2par) / (p1->m + p2->m);
	float vnew1 = sqrt(vparnew * vparnew + v1perp * v1perp);
	float vnew2 = sqrt(vparnew * vparnew + v2perp * v2perp);
	printf("final velocities: %lf %lf\n", vnew1, vnew2);
	p1->velx = vnew1 * x / distance;
	p1->vely = vnew1 * y / distance;
	p2->velx = vnew2 * x / distance;
	p2->vely = vnew2 * y / distance;
}*/

/*void movePlanet(planet* p, int pNum, blackHole holes[], int hSize, planet planets[], int pSize) 
{
	double distance, sqdistance, a;
	double ax = 0, ay = 0;
	//force of black holes
	for (int i = 0; i < hSize; i++) {
		sqdistance = (p->x - holes[i].x) * (p->x - holes[i].x) + (p->y - holes[i].y) * (p->y - holes[i].y);
		distance = sqrt(sqdistance);
		if (distance < holes[i].r + p->r)
			p->exists = false;
		
		if (p->exists) {
			a = holes[i].m / (sqdistance);
			ax += a * (holes[i].x - p->x) / distance;
			ay += a * (holes[i].y - p->y) / distance;
		}
	}
	//if planet was not absorbed
	if (p->exists) {
		//force of clusters
		
		/*for (int i = 0; i <= *cSize; i++) {
			double csqdistance = (p->x - clusters[i].centerX) * (p->x - clusters[i].centerX) + (p->y - clusters[i].centerX) * (p->y - clusters[i].centerX);
			double ca = clusters[i].M / (csqdistance)*K;
			double cdistance = sqrt(csqdistance);
			
			for (int j = 0; j < clusters[i].n; j++){
				double dist = sqrt((p->x - clusters[i].planets[j].x) * (p->x - clusters[i].planets[j].x) + (p->y - clusters[i].planets[j].x) * (p->y - clusters[i].planets[j].x));
				if (dist < (p->r + clusters[i].planets[j].r))
					addToCluster(p, &clusters[i]);
			}
			if (p->inCluster == false) {
				ax += ca * (clusters[i].centerX - p->x) / cdistance;
				ay += ca * (clusters[i].centerY - p->y) / cdistance;
			}
		}
		//force of free planets
		for (int i = 0; i <= pSize; i++) {
			if (i != pNum && planets[i].exists == true && planets[i].move == true) {
				double psqdistance = (p->x - planets[i].x) * (p->x - planets[i].x) + (p->y - planets[i].y) * (p->y - planets[i].y);
				double pa = planets[i].m / (psqdistance)*K;
				double pdistance = sqrt(psqdistance);
				/*if (pdistance <= (p->r + planets[i].r)) {
					*cSize+=1;
					//initCluster(p, &planets[i], &clusters[*cSize-1]);
					//addToCluster();
				}
				if (pdistance > (p->r + planets[i].r)) {
					ax += pa * (planets[i].x - p->x) / pdistance;
					ay += pa * (planets[i].y - p->y) / pdistance;
				}
			}
		}
		p->velx += ax;
		p->vely += ay;

		p->x += p->velx / 5;
		p->y += p->vely / 5;
	}
}*/

void movePlanet(planet* p, int pNum, blackHole holes[], int hSize, planet planets[], int pSize, cluster * c)
{
	double distance, sqdistance, a;
	double ax = 0, ay = 0;
	//force of black holes
	for (int i = 0; i < hSize; i++) {
		sqdistance = (p->x - holes[i].x) * (p->x - holes[i].x) + (p->y - holes[i].y) * (p->y - holes[i].y);
		distance = sqrt(sqdistance);
		if (distance < holes[i].r + p->r)
			p->exists = false;

		if (p->exists) {
			a = holes[i].m / (sqdistance);
			ax += a * (holes[i].x - p->x) / distance;
			ay += a * (holes[i].y - p->y) / distance;
		}
	}
	//if planet was not absorbed
	if (p->exists) {
		//force of clusters

		//for (int i = 0; i <= *cSize; i++) {
			double csqdistance = (p->x - c->centerX) * (p->x - c->centerX) + (p->y - c->centerX) * (p->y - c->centerX);
			double ca = c->M / (csqdistance)*K;
			double cdistance = sqrt(csqdistance);

			for (int j = 0; j < c->n; j++){
				double dist = sqrt((p->x - c->planets[j].x) * (p->x - c->planets[j].x) + (p->y - c->planets[j].x) * (p->y - c->planets[j].x));
				if (dist < (p->r + c->planets[j].r))
					addToCluster(p, c);
			}
			if (p->inCluster == false) {
				ax += ca * (c->centerX - p->x) / cdistance;
				ay += ca * (c->centerY - p->y) / cdistance;
			}
		//}
		//force of free planets
		for (int i = 0; i <= pSize; i++) {
			if (i != pNum && planets[i].exists == true && planets[i].move == true) {
				double psqdistance = (p->x - planets[i].x) * (p->x - planets[i].x) + (p->y - planets[i].y) * (p->y - planets[i].y);
				double pa = planets[i].m / (psqdistance)*K;
				double pdistance = sqrt(psqdistance);
				if (pdistance <= (p->r + planets[i].r)) {
					initCluster(p, &planets[i], c);
					//addToCluster();
				}
				if (pdistance > (p->r + planets[i].r)) {
					ax += pa * (planets[i].x - p->x) / pdistance;
					ay += pa * (planets[i].y - p->y) / pdistance;
				}
			}
		}
		p->velx += ax;
		p->vely += ay;

		p->x += p->velx / 5;
		p->y += p->vely / 5;
	}
}
/*
void moveCluster(cluster* c, blackHole holes[], int hSize, planet planets[], int pSize) {
	//move center of mass
	double distance, sqdistance, a;
	double ax = 0, ay = 0;

	for (int i = 0; i < hSize; i++) {
		sqdistance = (c->centerX - holes[i].x) * (c->centerX - holes[i].x) + (c->centerY - holes[i].y) * (c->centerY - holes[i].y);
		distance = sqrt(sqdistance);
		//if any planet crossed the blackHole dist - absorb planet & rebuild cluster
		/*for (int j = 0; j < c->n; j++) {
			if (distance < holes[i].r + p->r)
				p->exists = false;
		}
		//if (p->exists) {
			a = holes[i].m / (sqdistance);
			ax += a * (holes[i].x - c->centerX) / distance;
			ay += a * (holes[i].y - c->centerY) / distance;
		//}
	}

	//if (p->exists) {
	for (int i = 0; i <= pSize; i++) {
		if (planets[i].inCluster == false && planets[i].exists == true && planets[i].move == true) {
				double psqdistance = (c->centerX - planets[i].x) * (c->centerX - planets[i].x) + (c->centerY - planets[i].y) * (c->centerY - planets[i].y);
				double pa = planets[i].m / (psqdistance)*K;
				int pdistance = sqrt(psqdistance);
				/*for (int j=0;j<c->n;j++)
					if (pdistance <= (p->r + planets[i].r)) {
						initCluster(p, &planets[i]);
						addToCluster();
				}
				//if (pdistance > (p->r + planets[i].r)) {
					ax += pa * (planets[i].x - c->centerX) / pdistance;
					ay += pa * (planets[i].y - c->centerY) / pdistance;
				//}
			}
	}
	c->velx += ax;
	c->vely += ay;

	c->centerX += c->velx / 5;
	c->centerX += c->vely / 5;
	//}
	//move coordinates of each planet separately accordingly
	for (int i = 0; i < c->n; i++) {
		c->planets[i].x += c->velx / 5;
		c->planets[i].y += c->vely / 5;
	}
}
*/

void drawPull(planet planet1, int pullx, int pully, backColor c) 
{

	thickLineRGBA(renderer, planet1.x, planet1.y, pullx, pully, 2, 255-c, 255-c, 255-c, 255);

	int r = planet1.r / 2;
	aaFilledEllipseRGBA(renderer, pullx, pully, r, r, 255 - c, 255 - c, 255 - c, 255);
}

void drawTrajectory(planet p, int pNum, blackHole holes[], int hSize, planet planets[], int pSize, cluster c, int pullx, int pully) {
	planet protot = p;
	protot.r = p.r / 4;
	//cluster protocl=c;
	protot.velx = (protot.x - pullx) / 10;
	protot.vely = -(pully - protot.y) / 10;

	int opacity = 201;
	for (int i = 0; (opacity > 0 && i < 200); i++) {
		//planet *prototypes = new planet [100];
		//prototypes = planets;
		//for (int j = 0; j < pSize; j++) {
			//prototypes[j] = planets[j];
		//}
		//cluster protoclusters[1];
		//int protos = 0;
		movePlanet(&protot, pNum, holes, hSize, planets, pSize, &c);
		aaFilledEllipseRGBA(renderer, protot.x, protot.y, protot.r, protot.r, 0, 255, 0, opacity);
		opacity--;
	}
}

void drawTale(planet p) {
	double rad = p.r;
	int red = 0;
	int opac = 125;
	//for (int i = 0; (opac >= 0 && i < p.tale.size); i++) {
	for (int i = 0; opac >= 0 && rad > 0 && i < p.tale.size; i++) {
		aaFilledEllipseRGBA(renderer, p.tale.x[i], p.tale.y[i], rad, rad, red, 0, 255, opac);
		//rad-=p.r/(p.tale.size)/5;
		rad -= 0.25;
		red += 7;
		opac--;
		//opac-=255/(p.tale.size);
	}
}

void taleUpdate(planet* p) {
	for (int i = p->tale.size; i > 0; i--) {
		p->tale.x[i] = p->tale.x[i-1];
		p->tale.y[i] = p->tale.y[i-1];
	}
	p->tale.x[0] = p->x;
	p->tale.y[0] = p->y;
}

void makeTrajectory(planet* p, int pNum, blackHole holes[], int hSize, planet planets[], int pSize, cluster c) {
	planet protot = *p;
	p->trag.size = 200;
	//cluster protocl = c;

	for (int i = 0; i < p->trag.size; i++) {
		movePlanet(&protot, pNum, holes, hSize, planets, pSize, &c);
		p->trag.x[i] = protot.x;
		p->trag.y[i] = protot.y;
	}
}

void drawFinalTrajectory(planet p) {
	float opacity = p.opaq;
	int r = p.r / 4;
	for (int i = 0; (opacity > 0 && i < p.trag.size); i++) {
		aaFilledEllipseRGBA(renderer, p.trag.x[i], p.trag.y[i], r, r, 0, 255, 0, opacity);
		opacity-=1;
	}
}

// When using SDL, you have to use "int main(int argc, char* args[])"
// int main() and void main() are not allowed
int main(int argc, char* args[])
{
	//char imgPath2[100] = "../images/space.jpg";	
	// Start up SDL and create window
	if (initSDL())
	{
		printf("Failed to initialize SDL!\n");
		return -1;
	}
	backColor c = BLACK;
	blackHole holes[100];
	//int hSize = 2;
	int hSize = 1;
	for (int i = 0; i < hSize; i++) {
		setBlackHole(&holes[i]);
	}
	//holes[0].x = WIDTH / 3;
	holes[0].x = WIDTH / 2;
	holes[0].y = HEIGHT / 2;
	//holes[1].x = WIDTH * 2 / 3;
	//holes[1].y = HEIGHT / 2;
	planet planets[100];
	int pSize = 0;//total amount of planets+1=no. of planet that is about to be launched
	for (int i = 0; i <= pSize; i++)
		resetPlanet(&planets[i]);
	
	cluster cluster1;
	resetCluster(&cluster1);
	/*cluster clusters[10];
	int cSize = 0;
	for (int i = 0; i < cSize; i++) {
		resetCluster(&clusters[i]);
	}*/
	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	MouseState mouseState;
	int mouseX = 0;
	int mouseY = 0;
	int pullx = 0, pully = 0;

	//While application is running
	while (!quit)
	{
		mouseState = NONE;
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			mouseHandleEvent(&e, &mouseState, &mouseX, &mouseY);
		}

		pullx = mouseX;
		pully = mouseY;
		switch (mouseState) {
		case NONE:
			pullx = mouseX;
			pully = mouseY;
			break;

		case  LB_PR:
			resetPlanet(&planets[pSize]);
			planets[pSize].exists = true;
			planets[pSize].x = mouseX;
			planets[pSize].y = mouseY;
			break;

		case LB_RE:
			//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			planets[pSize].velx = (planets[pSize].x - pullx) / 10;
			planets[pSize].vely = -(pully - planets[pSize].y) / 10;
			makeTrajectory(&planets[pSize], pSize, holes, hSize, planets, pSize, cluster1);
			planets[pSize].tale.size = 1;
			planets[pSize].tale.x[0] = planets[pSize].x;
			planets[pSize].tale.y[0] = planets[pSize].y;
			planets[pSize].opaq = 201;
			planets[pSize].move = true;
			pSize++;
			break;
		}

		SDL_SetRenderDrawColor(renderer, c, c, c, 255);
		SDL_RenderClear(renderer);
		//clear everything
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		for (int i = 0; i < hSize; i++) {
			drawBlackHoleRings(&holes[i], c);
		}
		for (int i = 0; i <= pSize; i++) {
			if (planets[i].move == true && planets[i].exists) {
				movePlanet(&planets[i], i, holes, hSize, planets, pSize, &cluster1);
				drawFinalTrajectory(planets[i]);
				drawTale(planets[i]);
				if (planets[i].tale.size < 100) {
					planets[i].tale.size++;
				}
				taleUpdate(&planets[i]);
				planets[i].opaq -= 0.5;
			}

			if (planets[i].move == false && planets[i].exists) {
				drawPull(planets[i], pullx, pully, c);
				drawTrajectory(planets[i], i, holes,hSize, planets, pSize, cluster1, pullx, pully);
			}
			//draw everything
			if (planets[i].exists) {
				drawPlanet(planets[i], c);
			}
		}
		if (cluster1.n > 0)
			drawCluster(cluster1, c);

		/*for (int i = 0; i <= cSize; i++) {
			drawCluster(clusters[i], c);
		}*/

		for (int i = 0; i < hSize; i++) {
			drawBlackHole(holes[i], c);
		}
		SDL_RenderPresent(renderer);
	}

	//Free resources and close SDL
	closeSDL();

	return 0;
}