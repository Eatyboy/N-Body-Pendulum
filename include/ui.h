#ifndef UI_H
#define UI_H

// Make sure this file is in the same folder as raylib.h
#include "raylib.h"

#define TEXT_L 86
#define TEXT_M 60
#define TEXT_S 40

void drawCenteredText(const char *text, Rectangle parent, float fontSize, 
						Font font, Color color, float spacing);
Color lightenColor(Color color, float amount);

#define SLIDER_RADIUS 8
#define SLIDER_THICK 4

typedef struct Slider {
	float value; // between 0 and 1
	Vector2 pos;
	float length;
	bool isDragging;
	Vector2 dragStartPos;
	float dragStartValue;
} Slider;

Slider newSlider(float initValue, int posX, int posY, float length);
void drawSlider(Slider slider);
Vector2 getSliderPos(Slider slider);
void updateSlider(Slider *slider);

#define BTN_DOWN_SCALE 0.80f
#define BTN_HOVER_LIGHTEN 0.20f
#define BTN_BORDER_THICK 3

typedef struct Button {
	Rectangle rect;
	float round;
	Color bgColor;
	Color borderColor;
	const char* text;
	void (*fn)(void *state);
	bool isPressed;
	bool isHovered;
} Button;

Button newButton(int posX, int posY, int width, int height, float round, 
				 Color bgColor, Color borderColor, const char *text,
				 void (*fn)(void *state));
void drawButton(Button btn, Font font);
void handleButton(Button *button, void *state);

#endif // !UI_H
