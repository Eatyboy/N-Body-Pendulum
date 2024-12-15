#include <math.h>
#include <stdio.h>
#include "include/raylib.h"
#include "include/raymath.h"

#define GRAVITY (-150.0f) // this just worked best
#define RADIUS 32
#define MAX_SPEED 16.0f
#define MIN_SPEED 0.0625f
#define ONE_SIXTH 0.16666667f

#define RK4 1

typedef struct Body {
	float mass;
	float length;
	float theta;
	float thetaDot;
} Body;

void render(Body body, Vector2 origin, float speedup);
void solve(Body *body, float dt);
Vector2 getPos(Body body);
float func(float theta, float length); // f in dx/dt = f(x, t) in numerical integration

int main(void) {
	const Vector2 screenSize = {1280, 720};
	const char *screenName = "N-Body Pendulum";
	const int targetFPS = 60;

	InitWindow(screenSize.x, screenSize.y, screenName);
	SetTargetFPS(targetFPS);

	float speedup = 1;

	Vector2 origin = (Vector2){screenSize.x / 2, screenSize.y / 4};

	Body pendulum = (Body){10, 300, 0.4f * PI};

	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		KeyboardKey key = GetKeyPressed();

		if (key == KEY_RIGHT && speedup < MAX_SPEED) {
			speedup *= 2.0f;
		} else if (key == KEY_LEFT && speedup > MIN_SPEED) {
			speedup *= 0.5f;
		}

		solve(&pendulum, dt * speedup);
		render(pendulum, origin, speedup);
	}

	CloseWindow();

	return 0;
}

void render(Body body, Vector2 origin, float speedup) {
	BeginDrawing();

	ClearBackground(BLACK);

	// Pendulum
	{
		Vector2 pos = Vector2Add(origin, getPos(body));
		DrawLineV(origin, pos, WHITE);
		DrawCircleV(origin, 0.3f * RADIUS, RED);
		DrawCircleV(pos, RADIUS, BLUE);
	}

	// Speedup text
	{

		const char *text = (speedup >= 1 - EPSILON)
			? TextFormat("Speed: %dX", (int)speedup)
			: TextFormat("Speed: 1/%dX", (int)(1.0f / speedup));
		float textWidth = MeasureText(text, 36);
		DrawText(text, 0.5f * (GetScreenWidth() - textWidth), 50, 36, WHITE);
	}

	EndDrawing();
}

void solve(Body *body, float dt) {
	float omega = PI / 4;

	#if RK4

	float f1, f2, f3, f4;

	f1 = func(body->theta, body->length);
	f2 = func(body->theta + 0.5f * dt * f1, body->length);
	f3 = func(body->theta + 0.5f * dt * f2, body->length);
	f4 = func(body->theta + dt * f3, body->length);

	body->thetaDot += ONE_SIXTH * (f1 + 2*f2 + 2*f3 + f4);

	f1 = body->thetaDot;
	f2 = body->thetaDot + 0.5f * dt * f1;
	f3 = body->thetaDot + 0.5f * dt * f2;
	f4 = body->thetaDot + dt * f3;

	body->theta += ONE_SIXTH * dt * (f1 + 2*f2 + 2*f3 + f4);

	#else
	
	body->theta += dt * body->thetaDot;
	body->thetaDot += dt * func(body->theta, body->length);

	#endif

	return;
}

Vector2 getPos(Body body) {
	return Vector2Scale((Vector2){
		sinf(body.theta), 
		cosf(body.theta)
	}, body.length);
}

float func(float theta, float length) {
	return GRAVITY / length * sinf(theta);
}
