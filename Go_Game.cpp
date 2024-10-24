// GoGame.cpp : Defines the entry point for the application.
//
#include <windows.h>
#include <windowsx.h>  // Include this header for GET_X_LPARAM and GET_Y_LPARAM
#include <vector>
#include <stack>
#include <string>
#include <sstream>

// Window dimensions
const int WINDOW_WIDTH = 1000;  // Increased width to accommodate side panel
const int WINDOW_HEIGHT = 800;

// Go board dimensions
const int BOARD_SIZE = 19;
int CELL_SIZE; // Will be calculated based on window size
int OFFSET;    // Will be calculated based on window size

// Side panel dimensions
const int SIDE_PANEL_WIDTH = 250;

// Enum for stone colors
enum Stone { EMPTY, BLACK, WHITE };

// 2D array to represent the board
Stone board[BOARD_SIZE][BOARD_SIZE];

// Current player
Stone currentPlayer = BLACK;

// Player scores
int blackScore = 0;
int whiteScore = 0;

// Flags to track consecutive passes
int consecutivePasses = 0;

// Star points coordinates (0-based indexing)
const std::vector<std::pair<int, int>> starPoints = {
    {3, 3}, {3, 9}, {3, 15},
    {9, 3}, {9, 9}, {9, 15},
    {15, 3}, {15, 9}, {15, 15}
};

// Control IDs
#define IDC_CURRENT_PLAYER_LABEL 101
#define IDC_CURRENT_PLAYER_TEXT  102
#define IDC_SCORES_LABEL         103
#define IDC_BLACK_SCORE_LABEL    104
#define IDC_BLACK_SCORE_TEXT     105
#define IDC_WHITE_SCORE_LABEL    106
#define IDC_WHITE_SCORE_TEXT     107
#define IDC_PASS_BUTTON          108
#define IDC_RESIGN_BUTTON        109

// Forward declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void DrawBoard(HDC hdc, HWND hwnd);
void DrawStones(HDC hdc);
bool PlaceStone(int x, int y);
bool IsValidMove(int x, int y);
bool RemoveCapturedStones(int x, int y, Stone player);
int CountLiberties(int x, int y, Stone player, bool visited[BOARD_SIZE][BOARD_SIZE]);
void TogglePlayer();
void UpdateDimensions(int width, int height);
void UpdateScore();
void UpdateCurrentPlayerText(HWND hwnd);
void UpdateScoreTexts(HWND hwnd);
void EndGame(HWND hwnd);
void PassTurn(HWND hwnd);
void ResignGame(HWND hwnd);
void CreateUIControls(HWND hwnd);
void ResetGame(HWND hwnd);

// Entry point
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"GoGameWindowClass";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    // Create the window.
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Go Game",                     // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Initialize the board to EMPTY
    ResetGame(hwnd);

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

// Function to reset the game board and scores
void ResetGame(HWND hwnd)
{
    for (int i = 0; i < BOARD_SIZE; ++i)
        for (int j = 0; j < BOARD_SIZE; ++j)
            board[i][j] = EMPTY;

    currentPlayer = BLACK;
    blackScore = 0;
    whiteScore = 0;
    consecutivePasses = 0;

    UpdateCurrentPlayerText(hwnd);
    UpdateScoreTexts(hwnd);
}

// Window Procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        // Create UI Controls on the side panel
        CreateUIControls(hwnd);
    }
    return 0;

    case WM_SIZE:
    {
        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        UpdateDimensions(width, height);

        // Reposition UI controls based on new window size
        // Current Player Label
        HWND hLabelCurrent = GetDlgItem(hwnd, IDC_CURRENT_PLAYER_LABEL);
        SetWindowPos(hLabelCurrent, NULL, OFFSET + BOARD_SIZE * CELL_SIZE + 20, 50, 100, 20, SWP_NOZORDER);

        // Current Player Text
        HWND hTextCurrent = GetDlgItem(hwnd, IDC_CURRENT_PLAYER_TEXT);
        SetWindowPos(hTextCurrent, NULL, OFFSET + BOARD_SIZE * CELL_SIZE + 130, 50, 100, 20, SWP_NOZORDER);

        // Pass Button
        HWND hPassButton = GetDlgItem(hwnd, IDC_PASS_BUTTON);
        SetWindowPos(hPassButton, NULL, OFFSET + BOARD_SIZE * CELL_SIZE + 20, 80, 100, 30, SWP_NOZORDER);

        // Resign Button
        HWND hResignButton = GetDlgItem(hwnd, IDC_RESIGN_BUTTON);
        SetWindowPos(hResignButton, NULL, OFFSET + BOARD_SIZE * CELL_SIZE + 130, 80, 100, 30, SWP_NOZORDER);

        // Scores Label
        HWND hLabelScores = GetDlgItem(hwnd, IDC_SCORES_LABEL);
        SetWindowPos(hLabelScores, NULL, OFFSET + BOARD_SIZE * CELL_SIZE + 20, 130, 100, 20, SWP_NOZORDER);

        // Black Score Label
        HWND hLabelBlack = GetDlgItem(hwnd, IDC_BLACK_SCORE_LABEL);
        SetWindowPos(hLabelBlack, NULL, OFFSET + BOARD_SIZE * CELL_SIZE + 20, 160, 100, 20, SWP_NOZORDER);

        // Black Score Text
        HWND hTextBlack = GetDlgItem(hwnd, IDC_BLACK_SCORE_TEXT);
        SetWindowPos(hTextBlack, NULL, OFFSET + BOARD_SIZE * CELL_SIZE + 130, 160, 100, 20, SWP_NOZORDER);

        // White Score Label
        HWND hLabelWhite = GetDlgItem(hwnd, IDC_WHITE_SCORE_LABEL);
        SetWindowPos(hLabelWhite, NULL, OFFSET + BOARD_SIZE * CELL_SIZE + 20, 190, 100, 20, SWP_NOZORDER);

        // White Score Text
        HWND hTextWhite = GetDlgItem(hwnd, IDC_WHITE_SCORE_TEXT);
        SetWindowPos(hTextWhite, NULL, OFFSET + BOARD_SIZE * CELL_SIZE + 130, 190, 100, 20, SWP_NOZORDER);

        InvalidateRect(hwnd, NULL, TRUE); // Redraw the board on resize
    }
    return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Draw the board and stones
        DrawBoard(hdc, hwnd);
        DrawStones(hdc);

        EndPaint(hwnd, &ps);
    }
    return 0;

    case WM_LBUTTONDOWN:
    {
        if (consecutivePasses >= 2) {
            // Game already ended, ignore clicks
            return 0;
        }

        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);

        // Calculate the closest intersection
        int col = (xPos - OFFSET + CELL_SIZE / 2) / CELL_SIZE;
        int row = (yPos - OFFSET + CELL_SIZE / 2) / CELL_SIZE;

        if (col >= 0 && col < BOARD_SIZE && row >= 0 && row < BOARD_SIZE)
        {
            if (IsValidMove(col, row))
            {
                if (PlaceStone(col, row))
                {
                    InvalidateRect(hwnd, NULL, TRUE);
                    consecutivePasses = 0; // Reset pass count after a valid move
                    UpdateCurrentPlayerText(hwnd);
                    UpdateScoreTexts(hwnd);
                }
            }
            else
            {
                // Invalid move: lose turn
                MessageBox(hwnd, L"Invalid move! Turn skipped.", L"Invalid Move", MB_OK | MB_ICONWARNING);
                TogglePlayer();
                InvalidateRect(hwnd, NULL, TRUE);
                UpdateCurrentPlayerText(hwnd);
            }
        }
    }
    return 0;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_PASS_BUTTON: // Pass
            PassTurn(hwnd);
            break;
        case IDC_RESIGN_BUTTON: // Resign
            ResignGame(hwnd);
            break;
        }
    }
    return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Function to create UI Controls
void CreateUIControls(HWND hwnd)
{
    // Current Player Label
    CreateWindow(
        L"STATIC",  // Predefined class; Unicode assumed 
        L"Current Player:",      // Text 
        WS_VISIBLE | WS_CHILD,  // Styles 
        OFFSET + BOARD_SIZE * CELL_SIZE + 20,         // x position 
        50,         // y position 
        100,        // Label width
        20,        // Label height
        hwnd,     // Parent window
        (HMENU)IDC_CURRENT_PLAYER_LABEL,       // Control ID
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    // Current Player Text
    CreateWindow(
        L"STATIC",  // Predefined class; Unicode assumed 
        L"Black",      // Initial text 
        WS_VISIBLE | WS_CHILD,  // Styles 
        OFFSET + BOARD_SIZE * CELL_SIZE + 130,         // x position 
        50,         // y position 
        100,        // Text width
        20,        // Text height
        hwnd,     // Parent window
        (HMENU)IDC_CURRENT_PLAYER_TEXT,       // Control ID
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    // Pass Button
    CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"Pass",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        OFFSET + BOARD_SIZE * CELL_SIZE + 20,         // x position 
        80,         // y position 
        100,        // Button width
        30,        // Button height
        hwnd,     // Parent window
        (HMENU)IDC_PASS_BUTTON,       // Control ID
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    // Resign Button
    CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"Resign",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        OFFSET + BOARD_SIZE * CELL_SIZE + 130,         // x position 
        80,         // y position 
        100,        // Button width
        30,        // Button height
        hwnd,     // Parent window
        (HMENU)IDC_RESIGN_BUTTON,       // Control ID
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    // Scores Label
    CreateWindow(
        L"STATIC",  // Predefined class; Unicode assumed 
        L"Scores:",      // Text 
        WS_VISIBLE | WS_CHILD,  // Styles 
        OFFSET + BOARD_SIZE * CELL_SIZE + 20,         // x position 
        130,         // y position 
        100,        // Label width
        20,        // Label height
        hwnd,     // Parent window
        (HMENU)IDC_SCORES_LABEL,       // Control ID
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    // Black Score Label
    CreateWindow(
        L"STATIC",  // Predefined class; Unicode assumed 
        L"Black:",      // Text 
        WS_VISIBLE | WS_CHILD,  // Styles 
        OFFSET + BOARD_SIZE * CELL_SIZE + 20,         // x position 
        160,         // y position 
        100,        // Label width
        20,        // Label height
        hwnd,     // Parent window
        (HMENU)IDC_BLACK_SCORE_LABEL,       // Control ID
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    // Black Score Text
    CreateWindow(
        L"STATIC",  // Predefined class; Unicode assumed 
        L"0",      // Initial score 
        WS_VISIBLE | WS_CHILD,  // Styles 
        OFFSET + BOARD_SIZE * CELL_SIZE + 130,         // x position 
        160,         // y position 
        100,        // Text width
        20,        // Text height
        hwnd,     // Parent window
        (HMENU)IDC_BLACK_SCORE_TEXT,       // Control ID
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    // White Score Label
    CreateWindow(
        L"STATIC",  // Predefined class; Unicode assumed 
        L"White:",      // Text 
        WS_VISIBLE | WS_CHILD,  // Styles 
        OFFSET + BOARD_SIZE * CELL_SIZE + 20,         // x position 
        190,         // y position 
        100,        // Label width
        20,        // Label height
        hwnd,     // Parent window
        (HMENU)IDC_WHITE_SCORE_LABEL,       // Control ID
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    // White Score Text
    CreateWindow(
        L"STATIC",  // Predefined class; Unicode assumed 
        L"0",      // Initial score 
        WS_VISIBLE | WS_CHILD,  // Styles 
        OFFSET + BOARD_SIZE * CELL_SIZE + 130,         // x position 
        190,         // y position 
        100,        // Text width
        20,        // Text height
        hwnd,     // Parent window
        (HMENU)IDC_WHITE_SCORE_TEXT,       // Control ID
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.
}

// Function to update cell size and offset based on window size
void UpdateDimensions(int width, int height)
{
    // Calculate CELL_SIZE based on the smaller dimension
    int boardWidth = width - SIDE_PANEL_WIDTH - 40; // Allocate space for side panel with padding
    int size = (boardWidth < height - 40) ? boardWidth : (height - 40); // 40 for padding
    CELL_SIZE = size / BOARD_SIZE;
    OFFSET = 20; // Padding from the window edge
}

// Function to toggle the current player
void TogglePlayer()
{
    currentPlayer = (currentPlayer == BLACK) ? WHITE : BLACK;
}

// Function to draw the Go board
void DrawBoard(HDC hdc, HWND hwnd)
{
    // Draw background
    RECT rect;
    GetClientRect(hwnd, &rect);  // Use hwnd instead of GetDesktopWindow()
    HBRUSH brush = CreateSolidBrush(RGB(240, 217, 181)); // Wood-like color
    FillRect(hdc, &rect, brush);
    DeleteObject(brush);

    // Draw grid lines
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        // Horizontal lines
        MoveToEx(hdc, OFFSET, OFFSET + i * CELL_SIZE, NULL);
        LineTo(hdc, OFFSET + (BOARD_SIZE - 1) * CELL_SIZE, OFFSET + i * CELL_SIZE);

        // Vertical lines
        MoveToEx(hdc, OFFSET + i * CELL_SIZE, OFFSET, NULL);
        LineTo(hdc, OFFSET + i * CELL_SIZE, OFFSET + (BOARD_SIZE - 1) * CELL_SIZE);
    }

    // Draw star points (hoshi)
    for (const auto& point : starPoints)
    {
        int x = OFFSET + point.first * CELL_SIZE;
        int y = OFFSET + point.second * CELL_SIZE;
        int radius = 4; // Radius for the star point

        // Draw filled circle
        HBRUSH starBrush = CreateSolidBrush(RGB(0, 0, 0));
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, starBrush);
        HPEN starPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        HPEN oldPen = (HPEN)SelectObject(hdc, starPen);

        Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);

        // Restore old GDI objects
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);

        // Clean up
        DeleteObject(starBrush);
        DeleteObject(starPen);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

// Function to draw the stones
void DrawStones(HDC hdc)
{
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            if (board[i][j] != EMPTY)
            {
                HBRUSH brush;
                if (board[i][j] == BLACK)
                    brush = CreateSolidBrush(RGB(0, 0, 0));
                else
                    brush = CreateSolidBrush(RGB(255, 255, 255));

                // Create a black or white pen for the stone border
                HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
                HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
                HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, brush);

                int x = OFFSET + i * CELL_SIZE;
                int y = OFFSET + j * CELL_SIZE;
                int radius = CELL_SIZE / 2 - 4;

                Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);

                // Restore old GDI objects
                SelectObject(hdc, hOldPen);
                SelectObject(hdc, hOldBrush);

                // Clean up
                DeleteObject(hPen);
                DeleteObject(brush);
            }
        }
    }
}

// Function to draw the score panel (removed as we are using static controls)
// void DrawScore(HDC hdc, HWND hwnd)
// {
//     // This function is no longer needed as we use static controls
// }

// Function to update the Current Player Text
void UpdateCurrentPlayerText(HWND hwnd)
{
    HWND hTextCurrent = GetDlgItem(hwnd, IDC_CURRENT_PLAYER_TEXT);
    std::wstring playerText = (currentPlayer == BLACK) ? L"Black" : L"White";
    SetWindowText(hTextCurrent, playerText.c_str());
}

// Function to update the Score Texts
void UpdateScoreTexts(HWND hwnd)
{
    HWND hTextBlack = GetDlgItem(hwnd, IDC_BLACK_SCORE_TEXT);
    HWND hTextWhite = GetDlgItem(hwnd, IDC_WHITE_SCORE_TEXT);

    std::wstring blackScoreStr = std::to_wstring(blackScore);
    std::wstring whiteScoreStr = std::to_wstring(whiteScore);

    SetWindowText(hTextBlack, blackScoreStr.c_str());
    SetWindowText(hTextWhite, whiteScoreStr.c_str());
}

// Function to place a stone on the board
bool PlaceStone(int x, int y)
{
    if (board[x][y] != EMPTY)
        return false;

    board[x][y] = currentPlayer;

    // Check for captures
    bool captured = false;
    // Check all adjacent positions for opponent stones
    std::vector<std::pair<int, int>> directions = { {-1,0}, {1,0}, {0,-1}, {0,1} };
    for (auto& dir : directions)
    {
        int adjX = x + dir.first;
        int adjY = y + dir.second;
        if (adjX >= 0 && adjX < BOARD_SIZE && adjY >= 0 && adjY < BOARD_SIZE)
        {
            if (board[adjX][adjY] == ((currentPlayer == BLACK) ? WHITE : BLACK))
            {
                bool hasLiberty = false;
                bool visited[BOARD_SIZE][BOARD_SIZE] = { false };
                if (CountLiberties(adjX, adjY, board[adjX][adjY], visited) == 0)
                {
                    // Capture the stones
                    RemoveCapturedStones(adjX, adjY, board[adjX][adjY]);
                    captured = true;
                }
            }
        }
    }

    // Check if the placed stone has any liberties
    bool visited[BOARD_SIZE][BOARD_SIZE] = { false };
    if (CountLiberties(x, y, currentPlayer, visited) == 0)
    {
        if (!captured) // Suicide move
        {
            board[x][y] = EMPTY; // Remove the stone
            return false;
        }
    }

    // TODO: Implement the Ko rule here if necessary

    // Toggle to the next player
    TogglePlayer();

    return true;
}

// Function to check if a move is valid
bool IsValidMove(int x, int y)
{
    if (board[x][y] != EMPTY)
        return false;

    // Temporarily place the stone
    board[x][y] = currentPlayer;

    // Check for suicide
    bool suicide = false;
    bool visited[BOARD_SIZE][BOARD_SIZE] = { false };
    if (CountLiberties(x, y, currentPlayer, visited) == 0)
    {
        // Check if the move captures any opponent stones
        bool captures = false;
        std::vector<std::pair<int, int>> directions = { {-1,0}, {1,0}, {0,-1}, {0,1} };
        for (auto& dir : directions)
        {
            int adjX = x + dir.first;
            int adjY = y + dir.second;
            if (adjX >= 0 && adjX < BOARD_SIZE && adjY >= 0 && adjY < BOARD_SIZE)
            {
                if (board[adjX][adjY] == ((currentPlayer == BLACK) ? WHITE : BLACK))
                {
                    bool hasLiberty = false;
                    bool oppVisited[BOARD_SIZE][BOARD_SIZE] = { false };
                    if (CountLiberties(adjX, adjY, board[adjX][adjY], oppVisited) == 0)
                    {
                        captures = true;
                        break;
                    }
                }
            }
        }
        if (!captures)
            suicide = true;
    }

    // Remove the temporary stone
    board[x][y] = EMPTY;

    return !suicide;
}

// Function to remove captured stones
bool RemoveCapturedStones(int x, int y, Stone player)
{
    bool removed = false;
    std::stack<std::pair<int, int>> stonesToRemove;
    std::stack<std::pair<int, int>> stack;
    bool visited[BOARD_SIZE][BOARD_SIZE] = { false };

    stack.push({ x, y });
    visited[x][y] = true;

    while (!stack.empty())
    {
        auto current = stack.top();
        stack.pop();
        stonesToRemove.push(current);

        std::vector<std::pair<int, int>> directions = { {-1,0}, {1,0}, {0,-1}, {0,1} };
        for (auto& dir : directions)
        {
            int adjX = current.first + dir.first;
            int adjY = current.second + dir.second;
            if (adjX >= 0 && adjX < BOARD_SIZE && adjY >= 0 && adjY < BOARD_SIZE)
            {
                if (board[adjX][adjY] == player && !visited[adjX][adjY])
                {
                    stack.push({ adjX, adjY });
                    visited[adjX][adjY] = true;
                }
            }
        }
    }

    // Remove all stones in stonesToRemove
    while (!stonesToRemove.empty())
    {
        auto stone = stonesToRemove.top();
        stonesToRemove.pop();
        board[stone.first][stone.second] = EMPTY;
        removed = true;

        // Update score
        if (player == BLACK)
            whiteScore += 1;
        else
            blackScore += 1;
    }

    return removed;
}

// Function to count liberties of a group
int CountLiberties(int x, int y, Stone player, bool visited[BOARD_SIZE][BOARD_SIZE])
{
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
        return 0;
    if (board[x][y] != player)
        return 0;
    if (visited[x][y])
        return 0;

    visited[x][y] = true;

    int liberties = 0;
    std::vector<std::pair<int, int>> directions = { {-1,0}, {1,0}, {0,-1}, {0,1} };
    for (auto& dir : directions)
    {
        int adjX = x + dir.first;
        int adjY = y + dir.second;
        if (adjX >= 0 && adjX < BOARD_SIZE && adjY >= 0 && adjY < BOARD_SIZE)
        {
            if (board[adjX][adjY] == EMPTY)
                liberties += 1;
            else if (board[adjX][adjY] == player && !visited[adjX][adjY])
                liberties += CountLiberties(adjX, adjY, player, visited);
        }
    }
    return liberties;
}

// Function to update the scores (simplified)
void UpdateScore()
{
    // In a full implementation, this should calculate territory and subtract captured stones.
    // Here, we keep track of captured stones during the game.

    // Optionally, you can add additional scoring logic here.
}

// Function to handle passing the turn
void PassTurn(HWND hwnd)
{
    consecutivePasses += 1;
    if (consecutivePasses >= 2)
    {
        // End the game
        EndGame(hwnd);
    }
    else
    {
        TogglePlayer();
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateCurrentPlayerText(hwnd);
    }
}

// Function to handle resigning the game
void ResignGame(HWND hwnd)
{
    std::wstring message = L"Player ";
    message += (currentPlayer == BLACK) ? L"Black" : L"White";
    message += L" has resigned.\n";

    message += (currentPlayer == BLACK) ? L"White" : L"Black";
    message += L" wins!";

    MessageBox(hwnd, message.c_str(), L"Game Over", MB_OK | MB_ICONINFORMATION);
    EndGame(hwnd);
}

// Function to end the game
void EndGame(HWND hwnd)
{
    // Final scoring (simplified)
    UpdateScore();

    // Determine the winner
    std::wstring message = L"Final Scores:\n\n";
    message += L"Black: " + std::to_wstring(blackScore) + L"\n";
    message += L"White: " + std::to_wstring(whiteScore) + L"\n\n";

    if (blackScore > whiteScore)
        message += L"Black wins!";
    else if (whiteScore > blackScore)
        message += L"White wins!";
    else
        message += L"It's a tie!";

    MessageBox(hwnd, message.c_str(), L"Game Over", MB_OK | MB_ICONINFORMATION);

    // Optionally, reset the game or close the application
    // Here, we'll reset the game
    ResetGame(hwnd);
    InvalidateRect(hwnd, NULL, TRUE);
}

