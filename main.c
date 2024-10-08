#include <stdio.h>
#include "include/raylib.h"
#include "include/raymath.h"

#define GRAVITY (-9.8f)
#define BODY_COUNT 2

typedef struct Body {
	float mass;
	Vector2 position;
	Vector2 velocity;
} Body;

void solve(Body *(bodies[]), float dt);

int main(void) {
	const Vector2 screenSize = {1280, 720};
	const char *screenName = "N-Body Pendulum";
	const int targetFPS = 60;

	InitWindow(screenSize.x, screenSize.y, screenName);
	SetTargetFPS(targetFPS);

	Vector2 origin = (Vector2){screenSize.x / 2, screenSize.y / 4};

	Body bodies[BODY_COUNT];
	int lengths[BODY_COUNT];
	bodies[0] = (Body){
		1.0f,
		(Vector2){100, 100},
		(Vector2){0, 0}
	};
	bodies[1] = (Body){
		1.0f,
		(Vector2){0, 200},
		(Vector2){0, 0}
	};

	lengths[0] = Vector2Distance(origin, bodies[0].position);
	for (int i = 1; i < BODY_COUNT; ++i) {
		lengths[i] = Vector2Distance(bodies[i].position, bodies[i+1].position);
	}

	while (!WindowShouldClose()) {
		bodies[0].position = Vector2Subtract(GetMousePosition(), origin);

		BeginDrawing();

		ClearBackground(BLACK);

		DrawLineV(origin, Vector2Add(bodies[0].position, origin), WHITE);
		for (int i = 0; i < BODY_COUNT - 1; ++i) {
			DrawLineV(Vector2Add(origin, bodies[i].position),
				Vector2Add(origin, bodies[i+1].position), WHITE);
		}
		for (int i = 0; i < BODY_COUNT; ++i) {
			DrawCircleV(Vector2Add(origin, bodies[i].position), 16, BLUE);
		}

		EndDrawing();
	}

	CloseWindow();

	return 0;
}

void solve(Body *(bodies[]), float dt) {
	return;
}
