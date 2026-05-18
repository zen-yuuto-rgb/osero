#pragma warning(push)
#pragma warning(disable: 4828 4010)
#include "DxLib.h"
#pragma warning(pop)

#define BOARD_SIZE 8
#define CELL_SIZE 80
#define STONE_SIZE 30
#define WINDOW_WIDTH  (BOARD_SIZE * CELL_SIZE)
#define WINDOW_HEIGHT (BOARD_SIZE * CELL_SIZE)
#define EMPTY 0
#define BLACK 1
#define WHITE 2

int board[BOARD_SIZE][BOARD_SIZE];

void InitBoard(void);
void drawBoard(void);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    ChangeWindowMode(TRUE);
    SetGraphMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32);
    SetMainWindowText(L"DXLib Project");

    if (DxLib_Init() == -1)
    {
        return -1;
    }

    SetDrawScreen(DX_SCREEN_BACK);

    InitBoard();

    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
    {
        ClearDrawScreen();

        drawBoard();

        ScreenFlip();
    }

    DxLib_End();
    return 0;
}

void InitBoard(void)
{
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            board[y][x] = EMPTY;
        }
    }
}

void drawBoard(void)
{
    DrawBox(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GetColor(0, 150, 0), TRUE);

    for (int y = 0; y <= BOARD_SIZE; y++)
    {
        DrawLine(0, y * CELL_SIZE, WINDOW_WIDTH, y * CELL_SIZE, GetColor(0, 0, 0));
    }

    for (int x = 0; x <= BOARD_SIZE; x++)
    {
        DrawLine(x * CELL_SIZE, 0, x * CELL_SIZE, WINDOW_HEIGHT, GetColor(0, 0, 0));
    }
}
