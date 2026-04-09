#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 600

typedef struct {
    Color bg;
    Color btn;
    Color text;
    Color circout;
} Theme;

typedef struct {
    char display[256];
    double previous_value;
    char current_operator;
    bool new_number;


    bool isAnalog;
    int currentTheme;
    Texture2D background;
    bool bgLoaded;

    //File browser
    bool showFileBrowser;
    FilePathList files;
} AppState;

//Themes
Theme themes[3] = {
    { RAYWHITE, WHITE, BLACK, DARKGRAY },   
    { BLACK, DARKGRAY, RAYWHITE, LIGHTGRAY }, 
    { (Color){ 30, 30, 45, 255 }, (Color){ 50, 50, 70, 255 }, SKYBLUE, WHITE } 
};

//Button function
bool GuiButton(Rectangle rect, const char* text, Color baseColor, Color textColor) 
{
    Vector2 mousePoint = GetMousePosition();
    bool hovering = CheckCollisionPointRec(mousePoint, rect);
    bool clicked = false;

    if (hovering) {
        DrawRectangleRounded(rect, 0.2f, 10, Fade(baseColor, 0.7f));
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) clicked = true;
    } else {
        DrawRectangleRounded(rect, 0.2f, 10, baseColor);
    }
    DrawRectangleRoundedLines(rect, 0.2f, 10, 2, GRAY);
    
    int fontSize = 20;
    DrawText(text, rect.x + (rect.width/2 - MeasureText(text, fontSize)/2), rect.y + (rect.height/2 - fontSize/2), fontSize, textColor);
    return clicked;
}

void HandleInput(AppState *state, const char *input) 
{
    if (isdigit(input[0])) 
    {
        if (state->new_number || strcmp(state->display, "0") == 0) 
        {
            strcpy(state->display, input);
            state->new_number = false;

        } else if (strlen(state->display) < 15) 
        {
            strcat(state->display, input);
        }
    } else if (input[0] == 'C') 
    {
        strcpy(state->display, "0");
        state->previous_value = 0;
        state->current_operator = '\0';
        state->new_number = true;

    } else if (input[0] == '=') 
    {

        if (state->current_operator != '\0') 
        {
            double cur = atof(state->display);
            double res = 0; 
            switch(state->current_operator)
            {
                case '+':
                    res = state->previous_value + cur;
                    sprintf(state->display, "%g", res);
                    break;
                case '-':
                    res = state->previous_value - cur;
                    sprintf(state->display, "%g", res);
                    break;

                case '*':
                    res = state->previous_value * cur;
                    sprintf(state->display, "%g", res);
                    break;

                case '/':
                    if(cur == 0)
                    {
                        sprintf(state->display, "error");
                        state->current_operator = '\0';
                        state->new_number = true;
                        break;
                    }
                    res = state->previous_value / cur;
                    sprintf(state->display, "%g", res);
                    break;

                default:
                    sprintf(state->display, "No input");
                    state->current_operator = '\0';
                    state->new_number = true;
                    break;
            }
            

            state->current_operator = '\0';
            state->new_number = true;
        }
    } else 
    {
        state->previous_value = atof(state->display);
        state->current_operator = input[0];
        state->new_number = true;
    }
}

void DrawClock(AppState *state, int x, int y, float radius, Theme t) 
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    if (state->isAnalog) 
    {
        DrawCircle(x, y, radius, Fade(t.btn, 0.8f));
        DrawCircleLines(x, y, radius, t.circout);
        
        float h_angle = (tm_info->tm_hour % 12 + tm_info->tm_min / 60.0f) * 30.0f - 90;
        float m_angle = (tm_info->tm_min) * 6.0f - 90;
        float s_angle = (tm_info->tm_sec) * 6.0f - 90;

        DrawLineEx((Vector2){x,y}, (Vector2){x + cosf(h_angle*DEG2RAD)*radius*0.5f, y + sinf(h_angle*DEG2RAD)*radius*0.5f}, 4, t.text);
        DrawLineEx((Vector2){x,y}, (Vector2){x + cosf(m_angle*DEG2RAD)*radius*0.7f, y + sinf(m_angle*DEG2RAD)*radius*0.7f}, 2, t.text);
        DrawLineEx((Vector2){x,y}, (Vector2){x + cosf(s_angle*DEG2RAD)*radius*0.8f, y + sinf(s_angle*DEG2RAD)*radius*0.8f}, 1, RED);
    } else 
    {
        const char* timeStr = TextFormat("%02d:%02d:%02d", tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
        DrawText(timeStr, x - MeasureText(timeStr, 20)/2, y - 10, 20, t.text);
    }
}

int main() 
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Calculator");
    SetTargetFPS(60);

    AppState state = { 
        .display = "Waiting for input", 
        .new_number = true, 
        .isAnalog = true, 
        .currentTheme = 0, 
        .bgLoaded = false, 
        .showFileBrowser = false };

    const char *btns[4][4] = {{"7","8","9","/"}, {"4","5","6","*"}, {"1","2","3","-"}, {"C","0","=","+"}};

    while (!WindowShouldClose()) 
    {
        //Keyboard 
        if (!state.showFileBrowser) 
        {
            int key = GetKeyPressed();
            if (key >= KEY_ZERO && key <= KEY_NINE) {
                char buf[2] = {(char)('0' + (key - KEY_ZERO)), '\0'};
                HandleInput(&state, buf);
            } else if (key == KEY_EQUAL || key == KEY_ENTER) {
                HandleInput(&state, "=");
            } else if (key == KEY_KP_ADD) {
                HandleInput(&state, "+");
            } else if (key == KEY_KP_SUBTRACT) {
                HandleInput(&state, "-");
            } else if (key == KEY_KP_MULTIPLY) {
                HandleInput(&state, "*");
            } else if (key == KEY_KP_DIVIDE) {
                HandleInput(&state, "/");
            } else if (key == KEY_C) {
                HandleInput(&state, "C");
            }
        }

        BeginDrawing();
        Theme t = themes[state.currentTheme];
        ClearBackground(t.bg);

        //Draw Background Texture
        if (state.bgLoaded) 
        {
            DrawTexturePro(state.background, (Rectangle){0, 0, (float)state.background.width, (float)state.background.height},
                           (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, (Vector2){0,0}, 0, WHITE);
        }

        //Header
        DrawRectangle(0, 0, SCREEN_WIDTH, 100, Fade(t.bg, 0.6f));
        DrawClock(&state, SCREEN_WIDTH - 60, 50, 40, t);
        
        //Control buttons
        if (GuiButton((Rectangle){20, 20, 100, 30}, "Clock", t.btn, t.text)) state.isAnalog = !state.isAnalog;
        if (GuiButton((Rectangle){130, 20, 100, 30}, "Theme", t.btn, t.text)) state.currentTheme = (state.currentTheme + 1) % 3;
        
        //Bg file opener
        if (GuiButton((Rectangle){240, 20, 100, 30}, "BG", t.btn, t.text)) {
            state.files = LoadDirectoryFiles(".");
            state.showFileBrowser = true;
        }

        //Calculator
        DrawRectangleRounded((Rectangle){45, 125, 405, 465}, 0.1f, 10, Fade(t.bg, 0.8f));
        
        //Result display
        DrawRectangleRec((Rectangle){60, 140, 370, 60}, t.btn);
        DrawText(state.display, 420 - MeasureText(state.display, 40), 150, 40, t.text);

        //Calc buttons
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (GuiButton((Rectangle){60 + j * 95, 220 + i * 90, 85, 80}, btns[i][j], t.btn, t.text)) {
                    if (!state.showFileBrowser) HandleInput(&state, btns[i][j]);
                }
            }
        }

        //File browser
        if (state.showFileBrowser) {
            
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8f));
            DrawRectangleRounded((Rectangle){50, 50, 400, 500}, 0.1f, 10, t.bg);
            DrawText("Select an Image:", 70, 70, 20, t.text);
            
            int yPos = 110;
            for (unsigned int i = 0; i < state.files.count; i++) {
                
                if (IsFileExtension(state.files.paths[i], ".png") || IsFileExtension(state.files.paths[i], ".jpg")) {
                    if (GuiButton((Rectangle){70, (float)yPos, 360, 40}, GetFileName(state.files.paths[i]), t.btn, t.text)) {
                        if (state.bgLoaded) UnloadTexture(state.background);
                        state.background = LoadTexture(state.files.paths[i]);
                        state.bgLoaded = (state.background.id > 0);
                        state.showFileBrowser = false;
                        UnloadDirectoryFiles(state.files);
                    }
                    yPos += 50;
                    if (yPos > 480) break;
                }
            }

            if (GuiButton((Rectangle){70, 500, 360, 40}, "CLOSE", RED, WHITE)) {
                state.showFileBrowser = false;
                UnloadDirectoryFiles(state.files);
            }
        }

        EndDrawing();
    }

    if (state.bgLoaded) UnloadTexture(state.background);
    CloseWindow();
    return 0;
}