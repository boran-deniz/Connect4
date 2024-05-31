#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>

#define ROWS 6
#define COLS 7
#define CELL_SIZE 100
#define WINDOW_WIDTH (COLS * CELL_SIZE)
#define WINDOW_HEIGHT (ROWS * CELL_SIZE)
#define FONT_PATH "/Users/borandeniz/CLionProjects/Connect4/Roboto-Regular.ttf"

enum GameState {
    MENU,
    GAME,
    WIN
};

void initializeBoard(std::vector<std::vector<char>>& board) {
    board.assign(ROWS, std::vector<char>(COLS, ' '));
}

void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect destRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void renderBoard(SDL_Renderer* renderer, const std::vector<std::vector<char>>& board) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            SDL_Rect cell = { j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE };
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &cell);
            if (board[i][j] == 'X') {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderFillRect(renderer, &cell);
            } else if (board[i][j] == 'O') {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                SDL_RenderFillRect(renderer, &cell);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

bool makeMove(std::vector<std::vector<char>>& board, int col, char player) {
    if (col < 0 || col >= COLS) {
        return false;
    }
    for (int i = ROWS - 1; i >= 0; --i) {
        if (board[i][col] == ' ') {
            board[i][col] = player;
            return true;
        }
    }
    return false;
}

bool checkWin(const std::vector<std::vector<char>>& board, char player) {
    // Horizontal check
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS - 3; ++j) {
            if (board[i][j] == player && board[i][j + 1] == player && board[i][j + 2] == player && board[i][j + 3] == player) {
                return true;
            }
        }
    }
    // Vertical check
    for (int i = 0; i < ROWS - 3; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (board[i][j] == player && board[i + 1][j] == player && board[i + 2][j] == player && board[i + 3][j] == player) {
                return true;
            }
        }
    }
    // Diagonal (positive slope) check
    for (int i = 3; i < ROWS; ++i) {
        for (int j = 0; j < COLS - 3; ++j) {
            if (board[i][j] == player && board[i - 1][j + 1] == player && board[i - 2][j + 2] == player && board[i - 3][j + 3] == player) {
                return true;
            }
        }
    }
    // Diagonal (negative slope) check
    for (int i = 0; i < ROWS - 3; ++i) {
        for (int j = 0; j < COLS - 3; ++j) {
            if (board[i][j] == player && board[i + 1][j + 1] == player && board[i + 2][j + 2] == player && board[i + 3][j + 3] == player) {
                return true;
            }
        }
    }
    return false;
}

void renderMenu(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    renderText(renderer, font, "Connect Four", 100, 100, { 255, 255, 255 });
    renderText(renderer, font, "Press SPACE to Start", 100, 200, { 255, 255, 255 });
    SDL_RenderPresent(renderer);
}

void renderWinScreen(SDL_Renderer* renderer, TTF_Font* font, char winner) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    if (winner != ' ') {
        renderText(renderer, font, std::string("Player ") + winner + " wins!", 100, 100, { 255, 255, 255 });
    } else {
        renderText(renderer, font, "It's a draw!", 100, 100, { 255, 255, 255 });
    }
    renderText(renderer, font, "Press R to Restart or Q to Quit", 100, 200, { 255, 255, 255 });
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (TTF_Init() < 0) {
        std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Connect Four", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    TTF_Font* font = TTF_OpenFont(FONT_PATH, 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    std::vector<std::vector<char>> board(ROWS, std::vector<char>(COLS, ' '));
    char players[2] = { 'X', 'O' };
    int currentPlayer = 0;
    int moves = 0;
    bool gameWon = false;
    GameState state = MENU;
    char winner = ' ';
    SDL_Event e;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (state == MENU) {
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                    initializeBoard(board);
                    currentPlayer = 0;
                    moves = 0;
                    gameWon = false;
                    state = GAME;
                }
            } else if (state == GAME) {
                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    int x = e.button.x;
                    int col = x / CELL_SIZE;
                    if (makeMove(board, col, players[currentPlayer])) {
                        if (checkWin(board, players[currentPlayer])) {
                            winner = players[currentPlayer];
                            gameWon = true;
                            state = WIN;
                        } else {
                            currentPlayer = (currentPlayer + 1) % 2;
                            ++moves;
                            if (moves >= ROWS * COLS) {
                                winner = ' ';
                                state = WIN;
                            }
                        }
                    }
                }
            } else if (state == WIN) {
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_r) {
                        initializeBoard(board);
                        currentPlayer = 0;
                        moves = 0;
                        gameWon = false;
                        winner = ' ';
                        state = MENU;
                    } else if (e.key.keysym.sym == SDLK_q) {
                        quit = true;
                    }
                }
            }
        }

        if (state == MENU) {
            renderMenu(renderer, font);
        } else if (state == GAME) {
            renderBoard(renderer, board);
        } else if (state == WIN) {
            renderWinScreen(renderer, font, winner);
        }
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
