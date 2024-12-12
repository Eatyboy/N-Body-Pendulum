#include <math.h>
#include <stdio.h>
#include "include/raylib.h"
#include "include/raymath.h"

#define GRAVITY (-9.8f)
#define BODY_COUNT 4
#define RADIUS 16

typedef struct Body {
	float mass;
	float length;
	float theta;
} Body;

void render(Body bodies[], Vector2 origin);
void solve(Body bodies[], float dt);
Vector2 getPos(Body body);

int main(void) {
	const Vector2 screenSize = {1280, 720};
	const char *screenName = "N-Body Pendulum";
	const int targetFPS = 60;

	InitWindow(screenSize.x, screenSize.y, screenName);
	SetTargetFPS(targetFPS);

	Vector2 origin = (Vector2){screenSize.x / 2, screenSize.y / 4};

	Body bodies[BODY_COUNT];
	bodies[0] = (Body){ 1.0f, 100, PI/5 };
	bodies[1] = (Body){ 2.0f, 150, PI/5 };
	bodies[2] = (Body){ 3.0f, 50, PI/5 };
	bodies[3] = (Body){ 4.0f, 100, PI/5 };

	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		solve(bodies, dt);
		render(bodies, origin);
	}

	CloseWindow();

	return 0;
}

void render(Body bodies[], Vector2 origin) {
	BeginDrawing();

	ClearBackground(BLACK);

	{
		// Draw arms
		Vector2 prevPos = origin;
		for (int i = 0; i < BODY_COUNT; ++i) {
			Vector2 newPos = Vector2Add(prevPos, getPos(bodies[i]));
			DrawLineV(prevPos, newPos, WHITE);
			prevPos = newPos;
		}

		// Draw bodies
		prevPos = origin;
		DrawCircleV(origin, RADIUS / 2.0f, RED);
		for (int i = 0; i < BODY_COUNT; ++i) {
			Vector2 newPos = Vector2Add(prevPos, getPos(bodies[i]));
			DrawCircleV(newPos, RADIUS, BLUE);
			prevPos = newPos;
		}
	}

	EndDrawing();
}

void solve(Body bodies[], float dt) {
	float omega = PI / 4;
	for (int i = 0; i < BODY_COUNT; ++i) {
		bodies[i].theta += omega * cosf(omega * GetTime()) * dt * bodies[i].mass;
	}

	return;
}

Vector2 getPos(Body body) {
	return Vector2Scale((Vector2){
		sinf(body.theta), 
		cosf(body.theta)
	}, body.length);
}
