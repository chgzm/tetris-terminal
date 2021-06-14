#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <bitset>
#include <atomic>
#include "Tetromino.h"

constexpr static const int SCREEN_HEIGHT = 20;
constexpr static const int SCREEN_WIDTH  = 10; 
constexpr static const int INFO_WIDTH  = 8;
constexpr static const int LOOP_INTERVAL_MSEC = 25; 
constexpr static const int DOWN_INTERVAL_MSEC = 1000;

static uint8_t screen[SCREEN_HEIGHT][SCREEN_WIDTH];
static std::atomic_bool bKey[5];
static std::atomic_bool gameOver = false;
static int score = 0;
static int lines = 0;
static int level = 1;
static int blockCount = 0;

static void initTerm() {
    struct termios term;
    struct termios termOrig;
    tcgetattr(STDIN_FILENO, &termOrig);
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON|ECHO);
    term.c_cc[VTIME] = 0;
    term.c_cc[VMIN] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    // space for screen
    for (int i = 0; i < SCREEN_HEIGHT + 1; ++i) {
        printf("\n");
    }

    printf("\e[?25l"); // hide cursor
}

static bool isDeployable(int bType, int curRot, int curRow, int curCol) {
    const Block& t = blocks[bType][curRot];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (t[i][j] == Pixel::Space) { 
                continue;
            }

            const int r = curRow + i;
            const int c = curCol + j;
            if (r < 0 || SCREEN_HEIGHT <= r || c < 0 || SCREEN_WIDTH <= c) {
                return false;
            }

            if (screen[r][c] != Pixel::Space) {
                return false;
            }
        }
    }

    return true;
}

static void printPixel(uint8_t type) {
    switch (type) {
    case Pixel::Space:  { printf("  ");             break; }
    case Pixel::Pipe:   { printf("|");              break; }
    case Pixel::Bar:    { printf("--");             break; }
    case Pixel::Cyan:   { printf("\e[46m  \e[49m"); break; }
    case Pixel::Blue:   { printf("\e[44m  \e[49m"); break; }
    case Pixel::Grey:   { printf("\e[47m  \e[49m"); break; }
    case Pixel::Yellow: { printf("\e[43m  \e[49m"); break; }
    case Pixel::Green:  { printf("\e[42m  \e[49m"); break; }
    case Pixel::Purple: { printf("\e[45m  \e[49m"); break; }
    case Pixel::Red:    { printf("\e[41m  \e[49m"); break; }
    default:            {                           break; }
    }
}

static void drawScreen(int blockType, int nextBlockType, int curRot, int curRow, int curCol) {
    printf("\e[%dA", SCREEN_HEIGHT + 1); // back to the top

    const Block& block = blocks[blockType][curRot];
    const Block& next = blocks[nextBlockType][0];
    for (int r = 0; r < SCREEN_HEIGHT + 1; ++r) {
        // field
        for (int c = 0; c < SCREEN_WIDTH + 2; ++c) {
            if (r == SCREEN_HEIGHT) {
                if (c == 0 || c == SCREEN_WIDTH + 1) {
                    printf("+");
                } else {
                    printf("--");
                }
            } else if (c == 0 || c == SCREEN_WIDTH + 1) {
                printf("|");
            } else if (curRow <= r && r < curRow + 4 && curCol <= (c-1) && (c-1) < curCol + 4 && block[r - curRow][c - 1 - curCol] != Pixel::Space) {
                printPixel(block[r - curRow][c - 1 - curCol]);
            } else {
                printPixel(screen[r][c - 1]);
            }
        }

        // info
        if (r == 1) {
            printf(" Next:         |");
        } else if (1 < r && r < 6) {
            printf("   ");
            for (int i = 0; i < 4; ++i) {
                printPixel(next[r - 2][i]);
            }
            printf("    |"); 
        } else if (r == 7) {
            printf(" Score:        |");
        } else if (r == 8) {
            printf("      %6d   |", score);
        } else if (r == 9) {
             printf(" Lines:        |");
        } else if (r == 10) {
            printf("      %6d   |", lines);
        } else if (r == 11) {
             printf(" Level:        |");
        } else if (r == 12) {
            printf("          %2d   |", level);
        } else if (r == SCREEN_HEIGHT) {
            for (int c = 0; c < INFO_WIDTH; ++c) {
                if (c == INFO_WIDTH - 1) {
                    printf("-+");
                } else {
                    printf("--");
                }
            }
        }  
        else {
            printf("               |");
        }

        printf("\n");
    }
}

static void processInput() {
    while (!gameOver) {
        switch(getchar()) {
        // Forced fall
        case ' ': { bKey[4] = true; break; }
        // vim key binding
        case 'h': { bKey[1] = true; break; }
        case 'l': { bKey[0] = true; break; }
        case 'j': { bKey[2] = true; break; }
        case 'k': { bKey[3] = true; break; }
        // direction key
        case '\033': {
            getchar();
            switch(getchar()) {
            case 'A': { bKey[3] = true; break; }
            case 'B': { bKey[2] = true; break; }
            case 'C': { bKey[0] = true; break; }
            case 'D': { bKey[1] = true; break; }
            default:  {                 break; }
            }

            break;
        }
        // Quit
        case 'q': {
            gameOver = true; 
            break;
        }
        default: {
            break;
        }
        }
    }
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    // Initialize terminal condition.
    initTerm();

    // Init screen
    for (int r = 0; r < SCREEN_HEIGHT; ++r) {
        for (int c = 0; c < SCREEN_WIDTH; ++c) {
            screen[r][c] = Pixel::Space;
        }
    }

    // run input-processing thread    
    std::thread th(processInput);

    // main loop
    int blockType = rand() % 7;
    int nextBlockType = rand() % 7;
    int curRot = 0;
    int curRow = 0;
    int curCol = SCREEN_WIDTH / 2;
    int loopCount = 0;
    int loopsTillFall = (DOWN_INTERVAL_MSEC / LOOP_INTERVAL_MSEC);
    bool forcedFall = false;
    while (!gameOver) {
        std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_INTERVAL_MSEC));

        // check input
        curCol += (bKey[0] && isDeployable(blockType, curRot, curRow, curCol + 1)) ? 1 : 0;
        curCol -= (bKey[1] && isDeployable(blockType, curRot, curRow, curCol - 1)) ? 1 : 0;        
        curRow += (bKey[2] && isDeployable(blockType, curRot, curRow + 1, curCol)) ? 1 : 0;
        curRot += (bKey[3] && isDeployable(blockType, (curRot + 1) % 4, curRow, curCol)) ? 1 : 0;
        curRot %= 4;
        forcedFall = bKey[4];

        for (int i = 0; i < 5; ++i) {
            bKey[i] = false;
        }

        if (forcedFall) {
            while (isDeployable(blockType, curRot, curRow + 1, curCol)) {
                ++curRow;
            }
        } else {
            ++loopCount;
            if (loopCount < loopsTillFall) {
                drawScreen(blockType, nextBlockType, curRot, curRow, curCol);
                continue;
            }
            loopCount = 0;

            if (isDeployable(blockType, curRot, curRow + 1, curCol)) {
                ++curRow;
                drawScreen(blockType, nextBlockType, curRot, curRow, curCol);
                continue;
            }
        }

        // falling blok is fixed.
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                const Block& t = blocks[blockType][curRot];
                if (t[i][j] == Pixel::Space) {
                    continue;
                }

                const int r = curRow + i;
                const int c = curCol + j;
                screen[r][c] = t[i][j];
            }
        }
        ++blockCount;

        // Level up every 10 blocks
        if (blockCount % 10 == 0) {
            ++level;
            loopsTillFall = std::max(10, loopsTillFall - 1);
        }

        // check line complete
        int complines = 0;
        for (int i = curRow; (i < SCREEN_HEIGHT && i < curRow + 4); ++i) {
            bool complete = true; 
            for (int j = 0; j < SCREEN_WIDTH; ++j) {
                if (screen[i][j] == Pixel::Space) {
                    complete = false;
                    break;
                }
            }

            if (complete) {
                ++complines;
                for (int j = i; j > 0; --j) {
                    for (int k = 0; k < SCREEN_WIDTH; ++k) {
                        screen[j][k] = screen[j-1][k]; 
                    } 
                }

                for (int j = 0; j < SCREEN_WIDTH; ++j) {
                    screen[0][j] = Pixel::Space;
                }
            }
        }

        if (complines != 0) {
            score += (1 << (complines - 1)) * 100;
            lines += complines;
        }

        // next block
        curCol = SCREEN_WIDTH / 2;
        curRow = 0;
        curRot = 0;
        blockType = nextBlockType;
        nextBlockType = rand() % 7;

        if (isDeployable(blockType, curRot, curRow, curCol) == false) {
            break; // Game over
        }

        drawScreen(blockType, nextBlockType, curRot, curRow, curCol);
    }

    printf("Game Over!\n");
    gameOver = true;
    th.join();

    // reset terminal
    struct termios termOrig;
    tcsetattr(STDIN_FILENO, TCSANOW, &termOrig);

    return 0;
}
