#include <math.h>
#include <stdio.h>
#include "include/raylib.h"
#include "include/raymath.h"

#define GRAVITY (200.0f) // this just worked best
#define RADIUS 32
#define MAX_SPEED 16.0f
#define MIN_SPEED 0.0625f
#define ONE_SIXTH 0.16666667f

#define RK4 1 // 1: use Runge-Kutta 4, 0: use forward Euler

typedef struct Body {
	float mass;
	float length;
	float theta;
	float omega;
} Body;

void render(Body body, Vector2 origin);
void solve(Body *body, float dt);
Vector2 getPos(Body body);
float func(float theta, float length); // f in dx/dt = f(x, t) in numerical integration
float getEnergy(Body body);
void drawCenteredText(const char *text, int parentX, int parentWidth, int posY, int fontSize, Color color);

int main(void) {
	const Vector2 screenSize = {1280, 720};
	const char *screenName = "N-Body Pendulum";
	const int targetFPS = 60;

	InitWindow(screenSize.x, screenSize.y, screenName);
	SetTargetFPS(targetFPS);

	float speedup = 1;

	Vector2 origin = (Vector2){screenSize.x / 2, screenSize.y / 4 + 100};

	Body pendulum = (Body){10, 300, 0.4f * PI, 0};

	/*
	FILE *pendulumData;
	fopen_s(&pendulumData, "output.txt", "w");
	*/
	float initialEnergy = getEnergy(pendulum);

	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		KeyboardKey key = GetKeyPressed();

		if (key == KEY_RIGHT && speedup < MAX_SPEED) {
			speedup *= 2.0f;
		} else if (key == KEY_LEFT && speedup > MIN_SPEED) {
			speedup *= 0.5f;
		}

		// Numerically integrate to solve the system according to the
		// differential equation given by the Euler-Lagrange equation
		if (speedup > 1.0f - EPSILON) {
			int steps = (int)(speedup + EPSILON);
			for (int i = 0; i < steps; ++i) {
				solve(&pendulum, dt);
			}
		} else {
			solve(&pendulum, dt * speedup);
		}
		float energy = getEnergy(pendulum);

		BeginDrawing();

		ClearBackground(BLACK);

		// Draw the system
		render(pendulum, origin);

		// Speedup text
		const char *speedupText = (speedup >= 1 - EPSILON)
			? TextFormat("Speed: %dX", (int)speedup)
			: TextFormat("Speed: 1/%dX", (int)(1.0f / speedup));
		drawCenteredText(speedupText, 0, GetScreenWidth(), 50, 36, WHITE);

		//Energy text;
		DrawText(TextFormat("Initial energy: %f", initialEnergy), 20, 20, 24, WHITE);
		DrawText(TextFormat("Current energy: %f", energy), 20, 60, 24, WHITE);
		DrawText(TextFormat("Energy difference: %f", energy - initialEnergy), 20, 100, 24, WHITE);

		EndDrawing();
	}

	//fclose(pendulumData);

	CloseWindow();

	return 0;
}

void render(Body body, Vector2 origin) {
	Vector2 pos = Vector2Add(origin, getPos(body));
	DrawLineV(origin, pos, WHITE);
	DrawCircleV(origin, 0.3f * RADIUS, RED);
	DrawCircleV(pos, RADIUS, BLUE);
}

void solve(Body *body, float dt) {
	#if RK4

	float omegaDot1 = func(body->theta, body->length);
	float thetaDot1 = body->omega;
	float omegaDot2 = func(body->theta + 0.5f * dt * thetaDot1, body->length);
	float thetaDot2 = body->omega + 0.5f * dt * omegaDot1;
	float omegaDot3 = func(body->theta + 0.5f * dt * thetaDot2, body->length);
	float thetaDot3 = body->omega + 0.5f * dt * omegaDot2;
	float omegaDot4 = func(body->theta + dt * thetaDot3, body->length);
	float thetaDot4 = body->omega + dt * omegaDot3;

	body->omega += ONE_SIXTH * dt * (omegaDot1 + 2*omegaDot2 + 2*omegaDot3 + omegaDot4);
	body->theta += ONE_SIXTH * dt * (thetaDot1 + 2*thetaDot2 + 2*thetaDot3 + thetaDot4);

	#else
	
	body->theta += dt * body->omega;
	body->omega += dt * func(body->theta, body->length);

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
	return - (GRAVITY / length) * sinf(theta);
}

float getEnergy(Body body) {
	// E = 0.5mv^2 + mgh
	float kinetic = 0.5f * body.mass * body.length * body.length * body.omega * body.omega;
	float potential = body.mass * GRAVITY * body.length * (1 - cosf(body.theta));
	return kinetic + potential;
}

void drawCenteredText(const char *text, int parentX, int parentWidth, int posY, int fontSize, Color color) {
	float textWidth = MeasureText(text, 36);
	int posX = parentX + 0.5f * (parentWidth - textWidth);
	DrawText(text, posX, posY, fontSize, color);
}
