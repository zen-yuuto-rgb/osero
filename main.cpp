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
int winner = 0;
int winnerFont;
bool gameEnd = false;
bool gameOver = false;
bool hintVisible  = false;
int cpuWait = 0;
bool isCPUThinking = false;

void InitBoard(void);
void drawBoard(void);
void DrawStone(int x, int y, int color);
void changeTurn(void);
void drawTurn(void);
bool CanPutStone(int x, int y, int color);
void DrawHint(void);
void drawStoneCount(void);
void putStone(void);
void reverseStone(int x, int y, int color);
bool HasValidMove(int color);
void CheckPass(void);
void CheckGameEnd(void);
void DrawWinner(void);
void ToggleHint(void);
void CPUTurn(void);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE);
	SetGraphMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32);
	SetMainWindowText(L"DXLib Project");

	if (DxLib_Init() == -1)
	{
		return -1;
	}

	winnerFont = CreateFontToHandle(
		L"Meiryo",
		48,
		3
	);

	SetDrawScreen(DX_SCREEN_BACK);

	InitBoard();

	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		ClearDrawScreen();

		ToggleHint();
		drawBoard();
		drawTurn();
		drawStoneCount();
		putStone();

		if (isCPUThinking)
		{
			cpuWait++;

			if (cpuWait > 30)
			{
				CPUTurn();
				cpuWait = 0;
				isCPUThinking = false;
			}
		}
		DrawWinner();

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

	if (hintVisible)
	{
		DrawHint();
	}

	for (int y = 0; y < BOARD_SIZE; y++)
	{
		for (int x = 0; x < BOARD_SIZE; x++)
		{
			DrawStone(x, y, board[y][x]);
		}
	}

	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);
	int boardX = mouseX / CELL_SIZE;
	int boardY = mouseY / CELL_SIZE;

	if (boardX >= 0 && boardX < BOARD_SIZE &&
		boardY >= 0 && boardY < BOARD_SIZE)
	{
		int centerX = boardX * CELL_SIZE + CELL_SIZE / 2;
		int centerY = boardY * CELL_SIZE + CELL_SIZE / 2;

		if (currentTurn == BLACK)
		{
			DrawCircle(
				centerX,
				centerY,
				STONE_SIZE,
				GetColor(0, 0, 0),
				FALSE
			);
		}
		else
		{
			DrawCircle(
				centerX,
				centerY,
				STONE_SIZE,
				GetColor(255, 255, 255),
				FALSE
			);
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

bool CanPutStone(int x, int y, int color)
{
	// 範囲外チェック
	if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
	{
		return false;
	}

	// すでに石がある
	if (board[y][x] != EMPTY)
	{
		return false;
	}

	int enemy;

	// 相手の色
	if (color == BLACK)
	{
		enemy = WHITE;
	}
	else
	{
		enemy = BLACK;
	}

	// 8方向
	int dx[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
	int dy[8] = { -1,-1,-1,  0, 0,  1, 1, 1 };

	for (int dir = 0; dir < 8; dir++)
	{
		int nx = x + dx[dir];
		int ny = y + dy[dir];

		bool foundEnemy = false;

		// 相手石が続くか確認
		while (nx >= 0 && nx < BOARD_SIZE &&
			ny >= 0 && ny < BOARD_SIZE)
		{
			// 相手石
			if (board[ny][nx] == enemy)
			{
				foundEnemy = true;
			}

			// 自分の石
			else if (board[ny][nx] == color)
			{
				if (foundEnemy)
				{
					return true;
				}
				break;
			}

			// 空マス
			else
			{
				break;
			}

			nx += dx[dir];
			ny += dy[dir];
		}
	}

	return false;
}

void DrawHint(void)
{
	for (int y = 0; y < BOARD_SIZE; y++)
	{
		for (int x = 0; x < BOARD_SIZE; x++)
		{
			// 置ける場所
			if (CanPutStone(x, y, currentTurn))
			{
				int left = x * CELL_SIZE;
				int top = y * CELL_SIZE;
				int right = left + CELL_SIZE;
				int bottom = top + CELL_SIZE;

				// マスを光らせる
				DrawBox(
					left,
					top,
					right,
					bottom,
					GetColor(180, 255, 180),
					TRUE
				);
			}
		}
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

//石を置く
void putStone(void)
{
	static int clickFlag = 0;

	if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)
	{
		if (clickFlag == 0)
		{
			int mouseX, mouseY;

			GetMousePoint(&mouseX, &mouseY);

			int boardX = mouseX / CELL_SIZE;
			int boardY = mouseY / CELL_SIZE;

			if (boardX >= 0 && boardX < BOARD_SIZE &&
				boardY >= 0 && boardY < BOARD_SIZE)
			{
				if (CanPutStone(boardX, boardY, currentTurn)) {
					board[boardY][boardX] = currentTurn;
					reverseStone(boardX, boardY, currentTurn);
					changeTurn();
					CheckPass();

					if (currentTurn == WHITE)
					{
						isCPUThinking = true;
						cpuWait = 0;
					}
					CheckGameEnd();
				}
			}

			clickFlag = 1;
		}
	}
	else
	{
		clickFlag = 0;
	}
}
//石をひっくり返す
void reverseStone(int x, int y, int color)
{
	int enemy;

	if (color == BLACK)
	{
		enemy = WHITE;
	}
	else
	{
		enemy = BLACK;
	}

	int dx[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
	int dy[] = { -1,-1,-1,  0, 0,  1, 1, 1 };

	for (int dir = 0; dir < 8; dir++)
	{
		int nx = x + dx[dir];
		int ny = y + dy[dir];

		if (nx < 0 || nx >= BOARD_SIZE ||
			ny < 0 || ny >= BOARD_SIZE)
		{
			continue;
		}

		if (board[ny][nx] != enemy)
		{
			continue;
		}

		nx += dx[dir];
		ny += dy[dir];

		while (nx >= 0 && nx < BOARD_SIZE &&
			ny >= 0 && ny < BOARD_SIZE)
		{
			if (board[ny][nx] == EMPTY)
			{
				break;
			}

			if (board[ny][nx] == color)
			{
				int rx = x + dx[dir];
				int ry = y + dy[dir];

				while (board[ry][rx] == enemy)
				{
					board[ry][rx] = color;

					rx += dx[dir];
					ry += dy[dir];
				}

				break;
			}

			nx += dx[dir];
			ny += dy[dir];
		}
	}
}

bool HasValidMove(int color)
{
	for (int y = 0; y < BOARD_SIZE; y++)
	{
		for (int x = 0; x < BOARD_SIZE; x++)
		{
			if (CanPutStone(x, y, color))
			{
				return true;
			}
		}
	}
	return false;
}

void CheckPass(void)
{
	if (!HasValidMove(currentTurn))
	{
		changeTurn();

		// パス後も置けない
		if (!HasValidMove(currentTurn))
		{
			gameOver = true;
		}
	}
}

//判定
void CheckGameEnd(void)
{
	if (HasValidMove(BLACK) || HasValidMove(WHITE))
	{
		return;
	}

	int blackCount = 0;
	int whiteCount = 0;

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

	if (blackCount > whiteCount)
	{
		winner = BLACK;
	}
	else if (whiteCount > blackCount)
	{
		winner = WHITE;
	}
	else
	{
		winner = 3; // 引き分け
	}

	gameEnd = true;
}

//勝った方を表示
void DrawWinner(void)
{
	if (!gameEnd)
	{
		return;
	}

	// 結果表示用パネル
	DrawBox(
		80,
		240,
		560,
		360,
		GetColor(255, 255, 255),
		TRUE
	);

	DrawBox(
		80,
		240,
		560,
		360,
		GetColor(0, 0, 0),
		FALSE
	);

	// 勝敗表示
	if (winner == BLACK)
	{
		DrawString(
			200,
			290,
			L"BLACK WIN!",
			GetColor(255, 0, 0)
		);
	}
	else if (winner == WHITE)
	{
		DrawString(
			200,
			290,
			L"WHITE WIN!",
			GetColor(255, 0, 0)
		);
	}
	else
	{
		DrawString(
			250,
			290,
			L"DRAW!",
			GetColor(255, 0, 0)
		);
	}
}
void ToggleHint(void)
{
	static int prevH = 0;

	int nowH = CheckHitKey(KEY_INPUT_H);

	if (nowH == 1 && prevH == 0)
	{
		hintVisible  = !hintVisible ;
	}
	prevH = nowH;
}

void CPUTurn(void)
{
	// 置ける場所を探す
	int movesX[64];
	int movesY[64];
	int moveCount = 0;

	for (int y = 0; y < BOARD_SIZE; y++)
	{
		for (int x = 0; x < BOARD_SIZE; x++)
		{
			if (CanPutStone(x, y, currentTurn))
			{
				movesX[moveCount] = x;
				movesY[moveCount] = y;
				moveCount++;
			}
		}
	}

	// 置ける場所がない
	if (moveCount == 0)
	{
		return;
	}

	int r = GetRand(moveCount - 1);

	int x = movesX[r];
	int y = movesY[r];

	board[y][x] = currentTurn;
	reverseStone(x, y, currentTurn);

	changeTurn();
	CheckPass();
}

