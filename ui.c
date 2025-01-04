#include "include/ui.h"
#include "include/raylib.h"
#include "include/raymath.h"

Slider newSlider(float initValue, int posX, int posY, float length) {
	return (Slider){Clamp(initValue, 0.0f, 1.0f), (Vector2){posX, posY}, length, false};
}

void drawSlider(Slider slider) {
	Vector2 sliderEndPos = (Vector2){slider.pos.x + slider.length, slider.pos.y};
	DrawLineEx(slider.pos, sliderEndPos, SLIDER_THICK, WHITE);
	DrawCircleV(getSliderPos(slider), SLIDER_RADIUS, RAYWHITE);
}

Vector2 getSliderPos(Slider slider) {
	return (Vector2){slider.pos.x + slider.value * slider.length, slider.pos.y};
}

void updateSlider(Slider *slider) {
	Vector2 mousePos = GetMousePosition();

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		if (Vector2Distance(mousePos, getSliderPos(*slider)) <= SLIDER_RADIUS) {
			slider->isDragging = true;
			slider->dragStartPos = mousePos;
			slider->dragStartValue = slider->value;
		} else if (CheckCollisionPointLine(mousePos, slider->pos, 
			(Vector2){slider->pos.x + slider->length, slider->pos.y},
			SLIDER_THICK)) {
			float delta = (mousePos.x - getSliderPos(*slider).x) / slider->length;
			slider->value += delta;
		}
	}

	if (Vector2Distance(mousePos, getSliderPos(*slider)) <= SLIDER_RADIUS
		|| CheckCollisionPointLine(mousePos, slider->pos, 
			(Vector2){slider->pos.x + slider->length, slider->pos.y},
			SLIDER_THICK)) {
		slider->value = Clamp(slider->value + GetMouseWheelMove() * GetFrameTime(), 0.0f, 1.0f);
	}

	if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
		slider->isDragging = false;
	}

	if (slider->isDragging) {
		float delta = (mousePos.x - slider->dragStartPos.x) / slider->length;
		slider->value = Clamp(slider->dragStartValue + delta, 0.0f, 1.0f);
	}
}

Button newButton(int posX, int posY, int width, int height, float round, 
				 Color bgColor, Color borderColor, const char *text,
				 void (*fn)(void *state)) {
	return (Button){
		.rect = (Rectangle){posX, posY, width, height},
		.round = round,
		.bgColor = bgColor,
		.borderColor = borderColor,
		.text = text,
		.fn = fn,
		.isPressed = false,
		.isHovered = false
	};
}

void drawButton(Button btn, Font font) {
	Rectangle rec = btn.rect;
	bool btnDown = btn.isPressed;
	float sizeDiffMult = (1 - BTN_DOWN_SCALE) / 2;

	float btnX = rec.x; 
	float btnY = rec.y;
	float btnWidth = rec.width; 
	float btnHeight = rec.height;
	float fontSize = btnHeight / 2;
	if (btnDown) {
		btnX += rec.width * sizeDiffMult;
		btnY += rec.height * sizeDiffMult;
		btnWidth *= BTN_DOWN_SCALE;
		btnHeight *= BTN_DOWN_SCALE;
		fontSize *= BTN_DOWN_SCALE;
	}
	rec = (Rectangle){btnX, btnY, btnWidth, btnHeight};

	DrawRectangleRoundedLines(rec, btn.round, 0, BTN_BORDER_THICK, btn.borderColor);
	DrawRectangleRounded(rec, btn.round, 0, btn.isHovered 
		? lightenColor(btn.bgColor, BTN_HOVER_LIGHTEN) 
		: btn.bgColor);
	drawCenteredText(btn.text, rec, fontSize, font, WHITE, 2);
}

Color lightenColor(Color color, float amount) {
	return (Color){
		Lerp(color.r, 255, amount),
		Lerp(color.g, 255, amount),
		Lerp(color.b, 255, amount),
		255
	};
}

void drawCenteredText(const char *text, Rectangle parent, float fontSize, Font font, Color color, float spacing) {
	Vector2 textHalfDim = Vector2Scale(MeasureTextEx(font, text, fontSize, spacing), 0.5f);
	Vector2 textPos = (Vector2){
		parent.x + parent.width / 2 - textHalfDim.x,
		parent.y + parent.height / 2 - textHalfDim.y,
	};
	DrawTextEx(font, text, textPos, fontSize, spacing, color);
}

void handleButton(Button* button, void *state) {
	Vector2 mousePos = GetMousePosition();
	button->isHovered = CheckCollisionPointRec(mousePos, button->rect);
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		button->isPressed = false;
		if (CheckCollisionPointRec(mousePos, button->rect)) {
			(button->fn)(state);
		}
	}
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
		CheckCollisionPointRec(mousePos, button->rect)) {
		button->isPressed = true;
	}
}
