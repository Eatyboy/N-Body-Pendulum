#include <math.h>
#ifndef RAYLIB_H
#include "include/raylib.h"
#include "include/raymath.h"
#endif
#include "include/ui.h"

#define GRAVITY (200.0f) // this just worked best
#define MIN_RADIUS 4
#define MAX_RADIUS 40
#define MAX_SPEED 16.0f
#define MIN_SPEED 0.0625f
#define MIN_MASS 1
#define MAX_MASS 1000
#define MIN_LENGTH 10
#define MAX_LENGTH 250
#define ONE_SIXTH 0.16666667f

#define SLIDER_LEN 150
#define ROW_WIDTH 600
#define ROW_HEIGHT 100
#define TABLE_THICK 2
#define TABLE_DIV_OFFSET 150
#define TABLE_FONT_SZ 24
#define TABLE_SLIDER_OFFSET 400

#define RK4 1

typedef enum State {
	STOP,
	RUN
} State;

typedef struct Body {
	float mass; // in kilograms
	float length; // in meters
	float theta; // angle in radians
	float omega; // radians per second
} Body;

typedef struct StartBtnState {
	State *simState;
	Button *button;
} StartBtnState;

typedef struct TableRow {
	int posX;
	int posY;
	Slider massSlider;
	Slider lengthSlider;
	Slider thetaSlider;
} TableRow;

void render(Body body0, Body body1, Vector2 origin);
void solve(Body *body0, Body *body1, float dt);

Vector2 getPos(Body body);
float getEnergy(Body body0, Body body1);

// Functions for RK4
float omegadot0(Body body0, Body body1);
float thetadot0(Body body0, Body body1);
float omegadot1(Body body0, Body body1);
float thetadot1(Body body0, Body body1);

void startSim(void *state);
TableRow newTableRow(int posX, int posY);
void drawTableRow(Body body, TableRow tableRow, int id);

int main(void) {
	const Vector2 screenSize = {1920, 1080};
	const char *screenName = "N-Body Pendulum";
	const int targetFPS = 60;

	SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);
	//SetConfigFlags(FLAG_FULLSCREEN_MODE);
	SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(screenSize.x, screenSize.y, screenName);
	SetTargetFPS(targetFPS);

	Font font = GetFontDefault();

	float speedup = 1;

	Vector2 origin = (Vector2){screenSize.x / 2, screenSize.y / 2};

	Body body0 = (Body){10, 100, 0.4f * PI, 0};
	Body body1 = (Body){5, 100, 0.8f * PI, 0};

	float initialEnergy = getEnergy(body0, body1);

	Button startBtn = newButton(0.5f * (GetScreenWidth() - 160), 50, 160, 50, 
							 0.5f, GREEN, DARKGREEN, "Start", &startSim);
	TableRow row0 = newTableRow(1200, 50);
	TableRow row1 = newTableRow(1200, 50 + ROW_HEIGHT);

	State simState = STOP;

	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		KeyboardKey key = GetKeyPressed();

		handleButton(&startBtn, &(StartBtnState){&simState, &startBtn});

		if (simState == STOP) {
			updateSlider(&row0.massSlider);
			updateSlider(&row0.lengthSlider);
			updateSlider(&row0.thetaSlider);
			updateSlider(&row1.massSlider);
			updateSlider(&row1.lengthSlider);
			updateSlider(&row1.thetaSlider);
		}

		if (key == KEY_SPACE) {
			if (simState == STOP) {
				simState = RUN;
				startBtn.bgColor = RED;
				startBtn.borderColor = DARKBROWN;
				startBtn.text = "Stop";
			} else {
				simState = STOP;
				startBtn.bgColor = GREEN;
				startBtn.borderColor = DARKGREEN;
				startBtn.text = "Start";
			}
		}

		if (key == KEY_RIGHT && speedup < MAX_SPEED) {
			speedup *= 2.0f;
		} else if (key == KEY_LEFT && speedup > MIN_SPEED) {
			speedup *= 0.5f;
		}

		// Numerically integrate to solve the system according to the
		// differential equation given by the Euler-Lagrange equation
		if (simState == RUN) {
			if (speedup > 1.0f - EPSILON) {
				int steps = (int)(speedup + EPSILON);
				for (int i = 0; i < steps; ++i) {
					solve(&body0, &body1, dt);
				}
			} else {
				solve(&body0, &body1, dt * speedup);
			}
		} else if (simState == STOP) {
			initialEnergy = getEnergy(body0, body1);

			body0.mass = Lerp(MIN_MASS, MAX_MASS, row0.massSlider.value);
			body0.length = Lerp(MIN_LENGTH, MAX_LENGTH, row0.lengthSlider.value);
			body0.theta = 2.0f * PI * row0.thetaSlider.value;
			body0.omega = 0.0f;

			body1.mass = Lerp(MIN_MASS, MAX_MASS, row1.massSlider.value);
			body1.length = Lerp(MIN_LENGTH, MAX_LENGTH, row1.lengthSlider.value);
			body1.theta = 2.0f * PI * row1.thetaSlider.value;
			body1.omega = 0.0f;
		}
		float energy = getEnergy(body0, body1);

		BeginDrawing(); {
			ClearBackground(BLACK);

			// Draw the system
			render(body0, body1, origin);

			// Speedup text
			const char *speedupText = (speedup >= 1 - EPSILON)
				? TextFormat("Speed: %dX", (int)speedup)
				: TextFormat("Speed: 1/%dX", (int)(1.0f / speedup));
			float textWidth = MeasureText(speedupText, 36);
			DrawText(speedupText, 0.5f * (GetScreenWidth() - textWidth), 120, 36, WHITE);

			// Energy text
			DrawText(TextFormat("Initial energy: %d", (int)initialEnergy), 20, 40, 24, WHITE);
			DrawText(TextFormat("Current energy: %d", (int)energy), 20, 80, 24, WHITE);
			float percentDiff = 100.0f * (energy - initialEnergy) / initialEnergy;
			DrawText(TextFormat("Energy change: %f%%", percentDiff), 20, 120, 24, WHITE);

			// UI
			drawButton(startBtn, font);
			if (simState == STOP) {
				drawTableRow(body0, row0, 0);
				drawTableRow(body1, row1, 1);
			}
		} EndDrawing();
	}

	CloseWindow();

	return 0;
}

void render(Body body0, Body body1, Vector2 origin) {
	Vector2 pos0 = Vector2Add(origin, getPos(body0));
	Vector2 pos1 = Vector2Add(pos0, getPos(body1));
	DrawLineV(origin, pos0, WHITE);
	DrawLineV(pos0, pos1, WHITE);
	DrawCircleV(origin, 5, RED);
	DrawCircleV(pos0, Lerp(MIN_RADIUS, MAX_RADIUS, Normalize(body0.mass, MIN_MASS, MAX_MASS)), BLUE);
	DrawCircleV(pos1, Lerp(MIN_RADIUS, MAX_RADIUS, Normalize(body1.mass, MIN_MASS, MAX_MASS)), BLUE);
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

void startSim(void *state) {
	StartBtnState *startBtnState = (StartBtnState *)state;
	switch (*(startBtnState->simState)) {
		case STOP: 
			*(startBtnState->simState) = RUN;
			startBtnState->button->bgColor = RED;
			startBtnState->button->borderColor = DARKBROWN;
			startBtnState->button->text = "Stop";
			break;
		case RUN: 
			*(startBtnState->simState) = STOP;
			startBtnState->button->bgColor = GREEN;
			startBtnState->button->borderColor = DARKGREEN;
			startBtnState->button->text = "Start";
			break;
	}
}

TableRow newTableRow(int posX, int posY) {
	return (TableRow){
		.posX = posX,
		.posY = posY,
		.massSlider = newSlider(0.3f, posX + TABLE_SLIDER_OFFSET, 
							posY + ROW_HEIGHT / 6, SLIDER_LEN),
		.lengthSlider = newSlider(0.3f, posX + TABLE_SLIDER_OFFSET,
							posY + ROW_HEIGHT * 3 / 6,
							SLIDER_LEN),
		.thetaSlider = newSlider(0.3f, posX + TABLE_SLIDER_OFFSET,
							posY + ROW_HEIGHT * 5 / 6, 
						    SLIDER_LEN),
	};
}

void drawTextSlider(int posX, int posY, Slider slider, float value, 
					const char *var, const char *unit) {
	const char *text = TextFormat("%s: %.2f%s", var, value, unit);
	DrawText(text, posX, posY + 5, TABLE_FONT_SZ, WHITE);
	drawSlider(slider);
}

#define BODY_FONT_SZ 40
void drawTableRow(Body body, TableRow tableRow, int num) {
	DrawRectangleLinesEx((Rectangle){
			.x = tableRow.posX, 
			.y = tableRow.posY, 
			.width = ROW_WIDTH, 
			.height = ROW_HEIGHT}, 
		TABLE_THICK, WHITE);
	DrawLineEx((Vector2){tableRow.posX + TABLE_DIV_OFFSET, tableRow.posY}, 
			(Vector2){tableRow.posX + TABLE_DIV_OFFSET, tableRow.posY + ROW_HEIGHT},
			TABLE_THICK, WHITE);

	DrawText("Body", tableRow.posX + (TABLE_DIV_OFFSET - MeasureText("Body", BODY_FONT_SZ)) / 2, 
		  tableRow.posY + ROW_HEIGHT / 2 - BODY_FONT_SZ, BODY_FONT_SZ, WHITE);
	const char *numText = TextFormat("%d", num);
	DrawText(numText, tableRow.posX + (TABLE_DIV_OFFSET - MeasureText(numText, BODY_FONT_SZ)) / 2, 
		  tableRow.posY + ROW_HEIGHT / 2, BODY_FONT_SZ, WHITE);

	drawTextSlider(tableRow.posX + TABLE_DIV_OFFSET + 10, tableRow.posY, tableRow.massSlider, 
				body.mass, "mass", " kg");
	drawTextSlider(tableRow.posX + TABLE_DIV_OFFSET + 10, tableRow.posY + ROW_HEIGHT / 3, tableRow.lengthSlider, 
				body.length, "length", " m");
	drawTextSlider(tableRow.posX + TABLE_DIV_OFFSET + 10, tableRow.posY + 2 * ROW_HEIGHT / 3, tableRow.thetaSlider, 
				Wrap(body.theta * RAD2DEG, 0.0f, 360.0f), "angle", "°");
}
