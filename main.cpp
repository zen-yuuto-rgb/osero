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
int currentTurn = BLACK;

void InitBoard(void);
void drawBoard(void);
void DrawStone(int x, int y, int color);
void changeTurn(void);
void drawTurn(void);
void drawStoneCount(void);

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

        //ターン交代確認用
        //クリックしたら白黒ターン変わる
        if (GetMouseInput() & MOUSE_INPUT_LEFT) {
            changeTurn();

            while (GetMouseInput() & MOUSE_INPUT_LEFT) {
                ProcessMessage();
            }
        }

        drawBoard();
        drawTurn();
        drawStoneCount();

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
    board[3][3] = WHITE;
    board[3][4] = BLACK;
    board[4][3] = BLACK;
    board[4][4] = WHITE;
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

    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            DrawStone(x, y, board[y][x]);
        }
    }
}
void DrawStone(int x, int y, int color)
{
    // マスの中央座標を計算
    int centerX = x * CELL_SIZE + CELL_SIZE / 2;
    int centerY = y * CELL_SIZE + CELL_SIZE / 2;

    // 黒石
    if (color == BLACK)
    {
        DrawCircle(
            centerX,
            centerY,
            STONE_SIZE,
            GetColor(0, 0, 0),
            TRUE
        );
    }

    // 白石
    else if (color == WHITE)
    {
        DrawCircle(
            centerX,
            centerY,
            STONE_SIZE,
            GetColor(255, 255, 255),
            TRUE
        );

        // 白石が見えやすいように枠線追加
        DrawCircle(
            centerX,
            centerY,
            STONE_SIZE,
            GetColor(0, 0, 0),
            FALSE
        );
    }
}

//ターン交代処理
void changeTurn(void)
{
    if (currentTurn == BLACK)
    {
        currentTurn = WHITE;
    }
    else
    {
        currentTurn = BLACK;
    }
}
//ターン表示
void drawTurn(void)
{
    if (currentTurn == BLACK) {
        DrawString(10, 10, L"BLACK Turn", GetColor(0, 0, 0));
    }
    else {
        DrawString(10, 10, L"WHITE Turn", GetColor(255, 255, 255));
    }
}

void drawStoneCount(void)
{
    int blackCount = 0;
    int whiteCount = 0;

    // 盤面全探索
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            if (board[y][x] == BLACK)
            {
                blackCount++;
            }
            else if (board[y][x] == WHITE)
            {
                whiteCount++;
            }
        }
    }

    // 表示用文字
    TCHAR text[64];

    // 黒石数
    wsprintf(text, L"BLACK : %d", blackCount);
    DrawString(10, 40, text, GetColor(0, 0, 0));

    // 白石数
    wsprintf(text, L"WHITE : %d", whiteCount);
    DrawString(10, 70, text, GetColor(255, 255, 255));
}