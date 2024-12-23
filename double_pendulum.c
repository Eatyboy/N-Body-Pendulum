#include <math.h>
#include "include/raylib.h"
#include "include/raymath.h"

#define GRAVITY (200.0f) // this just worked best
#define RADIUS 16
#define MAX_SPEED 16.0f
#define MIN_SPEED 0.0625f
#define ONE_SIXTH 0.16666667f

#define RK4 1

typedef struct Body {
	float mass;
	float length;
	float theta;
	float omega;
} Body;

void render(Body body0, Body body1, Vector2 origin);
void solve(Body *body0, Body *body1, float dt);

Vector2 getPos(Body body);
float getEnergy(Body body0, Body body1);

float omegadot0(Body body0, Body body1);
float thetadot0(Body body0, Body body1);
float omegadot1(Body body0, Body body1);
float thetadot1(Body body0, Body body1);

int main(void) {
	const Vector2 screenSize = {1280, 720};
	const char *screenName = "N-Body Pendulum";
	const int targetFPS = 60;

	InitWindow(screenSize.x, screenSize.y, screenName);
	SetTargetFPS(targetFPS);

	float speedup = 1;

	Vector2 origin = (Vector2){screenSize.x / 2, screenSize.y / 4};

	Body body0 = (Body){10, 200, 0.4f * PI, 0};
	Body body1 = (Body){15, 150, 0.8f * PI, 0};

	float initialEnergy = getEnergy(body0, body1);

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
				solve(&body0, &body1, dt);
			}
		} else {
			solve(&body0, &body1, dt * speedup);
		}
		float energy = getEnergy(body0, body1);

		// Draw the system
		render(body0, body1, origin);

		// Speedup text
		const char *speedupText = (speedup >= 1 - EPSILON)
			? TextFormat("Speed: %dX", (int)speedup)
			: TextFormat("Speed: 1/%dX", (int)(1.0f / speedup));
		float textWidth = MeasureText(speedupText, 36);
		DrawText(speedupText, 0.5f * (GetScreenWidth() - textWidth), 50, 36, WHITE);

		// Energy text
		DrawText(TextFormat("Initial energy: %f", initialEnergy), 20, 20, 24, WHITE);
		DrawText(TextFormat("Current energy: %f", energy), 20, 60, 24, WHITE);
		float percentDiff = 100.0f * (energy - initialEnergy) / initialEnergy;
		DrawText(TextFormat("Energy change: %f%%", percentDiff), 20, 100, 24, WHITE);
	}

	CloseWindow();

	return 0;
}

void render(Body body0, Body body1, Vector2 origin) {
	BeginDrawing();

	ClearBackground(BLACK);

	Vector2 pos0 = Vector2Add(origin, getPos(body0));
	Vector2 pos1 = Vector2Add(pos0, getPos(body1));
	DrawLineV(origin, pos0, WHITE);
	DrawLineV(pos0, pos1, WHITE);
	DrawCircleV(origin, 0.5f * RADIUS, RED);
	DrawCircleV(pos0, RADIUS, BLUE);
	DrawCircleV(pos1, RADIUS, BLUE);

	EndDrawing();
}

void solve(Body *body0, Body *body1, float dt) {
	#if RK4

	Body b0_1 = (Body){body0->mass, body0->length, body0->theta, body0->omega};
	Body b1_1 = (Body){body1->mass, body1->length, body1->theta, body1->omega};
	float d_omega0_1 = omegadot0(b0_1, b1_1);
	float d_theta0_1 = thetadot0(b0_1, b1_1);
	float d_omega1_1 = omegadot1(b0_1, b1_1);
	float d_theta1_1 = thetadot1(b0_1, b1_1);

	Body b0_2 = (Body){body0->mass, body0->length, 
		body0->theta + 0.5f * dt * d_theta0_1, 
		body0->omega + 0.5f * dt * d_omega0_1};
	Body b1_2 = (Body){body1->mass, body1->length, 
		body1->theta + 0.5f * dt * d_theta1_1, 
		body1->omega + 0.5f * dt * d_omega1_1};
	float d_omega0_2 = omegadot0(b0_2, b1_2);
	float d_theta0_2 = thetadot0(b0_2, b1_2);
	float d_omega1_2 = omegadot1(b0_2, b1_2);
	float d_theta1_2 = thetadot1(b0_2, b1_2);

	Body b0_3 = (Body){body0->mass, body0->length, 
		body0->theta + 0.5f * dt * d_theta0_2, 
		body0->omega + 0.5f * dt * d_omega0_2};
	Body b1_3 = (Body){body1->mass, body1->length, 
		body1->theta + 0.5f * dt * d_theta1_2, 
		body1->omega + 0.5f * dt * d_omega1_2};
	float d_omega0_3 = omegadot0(b0_3, b1_3);
	float d_theta0_3 = thetadot0(b0_3, b1_3);
	float d_omega1_3 = omegadot1(b0_3, b1_3);
	float d_theta1_3 = thetadot1(b0_3, b1_3);

	Body b0_4 = (Body){body0->mass, body0->length, 
		body0->theta + dt * d_theta0_3, 
		body0->omega + dt * d_omega0_3};
	Body b1_4 = (Body){body1->mass, body1->length, 
		body1->theta + dt * d_theta1_3, 
		body1->omega + dt * d_omega1_3};
	float d_omega0_4 = omegadot0(b0_4, b1_4);
	float d_theta0_4 = thetadot0(b0_4, b1_4);
	float d_omega1_4 = omegadot1(b0_4, b1_4);
	float d_theta1_4 = thetadot1(b0_4, b1_4);

	body0->omega += ONE_SIXTH * dt * (d_omega0_1 + 2*d_omega0_2 + 2*d_omega0_3 + d_omega0_4);
	body0->theta += ONE_SIXTH * dt * (d_theta0_1 + 2*d_theta0_2 + 2*d_theta0_3 + d_theta0_4);
	body1->omega += ONE_SIXTH * dt * (d_omega1_1 + 2*d_omega1_2 + 2*d_omega1_3 + d_omega1_4);
	body1->theta += ONE_SIXTH * dt * (d_theta1_1 + 2*d_theta1_2 + 2*d_theta1_3 + d_theta1_4);

	#else
	
	body->theta += dt * body->omega;
	body->omega += dt * func(body->theta, body->length);

	#endif

	return;
}

float omegadot0(Body body0, Body body1) {
	// This formula is so long, that this makes sense
	float m0 = body0.mass;
	float m1 = body1.mass;
	float l0 = body0.length;
	float l1 = body1.length;
	float t0 = body0.theta;
	float t1 = body1.theta;
	float w0 = body0.omega;
	float w1 = body1.omega;
	float g = GRAVITY;

	return (m1 * cosf(t0 - t1) * (l0 * w0 * w0 * sinf(t1 - t0) + g * sinf(t1))
		- (m1 * l1 * w1 * w1 * sinf(t0 - t1) + (m0 + m1) * g * sinf(t0))) 
		/ (l0 * (m0 + m1 * sinf(t0 - t1) * sinf(t0-t1)));
}

float thetadot0(Body body0, Body body1) {
	return body0.omega;
}

float omegadot1(Body body0, Body body1) {
	// This formula is so long, that this makes sense
	float m0 = body0.mass;
	float m1 = body1.mass;
	float l0 = body0.length;
	float l1 = body1.length;
	float t0 = body0.theta;
	float t1 = body1.theta;
	float w0 = body0.omega;
	float w1 = body1.omega;
	float g = GRAVITY;

	return (cosf(t0 - t1) * (m1 * l1 * w1 * w1 * sinf(t0 - t1) + (m0 + m1) * g * sinf(t0))
		- (m0 + m1) * (l0 * w0 * w0 * sinf(t1 - t0) + g * sinf(t1))) 
		/ (l1 * (m0 + m1 * sinf(t0 - t1) * sinf(t0-t1)));
}
 
float thetadot1(Body body0, Body body1) {
	return body1.omega;
}

Vector2 getPos(Body body) {
	return Vector2Scale((Vector2){
		sinf(body.theta), 
		cosf(body.theta)
	}, body.length);
}

float getEnergy(Body body0, Body body1) {
	// E = 0.5mv^2 + mgh
	// This formula is so long, that this makes sense
	float m0 = body0.mass;
	float m1 = body1.mass;
	float l0 = body0.length;
	float l1 = body1.length;
	float t0 = body0.theta;
	float t1 = body1.theta;
	float w0 = body0.omega;
	float w1 = body1.omega;
	float g = GRAVITY;

	float kinetic = 0.5f * (m0 + m1) * l0 * l0 * w0 * w0
		+ 0.5f * m1 * l1 * l1 * w1 * w1
		+ m1 * l0 * l1 * w0 * w1 * cosf(t0 - t1);
	float potential = (m0 + m1) * g * l0 * (1 - cosf(t0))
		+ m1 * g * l1 * (1 - cosf(t1));
	return kinetic + potential;
}
