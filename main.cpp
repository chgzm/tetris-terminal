#include <thread>
#include <chrono>
#include <mutex>
#include <termios.h>
#include <unistd.h>
#include <bitset>
#include <atomic>
#include "Tetromino.h"

constexpr static const int SCREEN_HEIGHT = 20;
constexpr static const int SCREEN_WIDTH  = 10; 
constexpr static const int LOOP_INTERVAL_MSEC = 25; 
constexpr static const int DOWN_INTERVAL_MSEC = 500;
constexpr static const int SPEEDUP_COUNT = 20; 

static uint8_t screen[SCREEN_HEIGHT][SCREEN_WIDTH];
static std::bitset<4> bKey;
static std::mutex mtx;
static std::atomic_bool gameOver = false;

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
    for (int i = 0; i < SCREEN_HEIGHT + 1; i++) {
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

static void drawScreen(int blockType, int curRot, int curRow, int curCol) {
    printf("\e[%dA", SCREEN_HEIGHT + 1); // back to the top

    const Block& block = blocks[blockType][curRot];
    for (int r = 0; r < SCREEN_HEIGHT + 1; r++) {
        for (int c = 0; c < SCREEN_WIDTH + 2; ++c) {
            if (r == SCREEN_HEIGHT) {
                if (c != SCREEN_WIDTH - 1) {
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
        printf("\n");
    }
}

static void processInput() {
    while (!gameOver) {
        switch(getchar()) {
        // vim key binding
        case 'h': { mtx.lock(); bKey.set(1); mtx.unlock(); break; }
        case 'l': { mtx.lock(); bKey.set(0); mtx.unlock(); break; }
        case 'j': { mtx.lock(); bKey.set(2); mtx.unlock(); break; }
        case 'k': { mtx.lock(); bKey.set(3); mtx.unlock(); break; }
        // direction key
        case '\033': {
            getchar();
            switch(getchar()) {
            case 'A': { mtx.lock(); bKey.set(3); mtx.unlock(); break; }
            case 'B': { mtx.lock(); bKey.set(2); mtx.unlock(); break; }
            case 'C': { mtx.lock(); bKey.set(0); mtx.unlock(); break; }
            case 'D': { mtx.lock(); bKey.set(1); mtx.unlock(); break; }
            default:  {                                        break; }
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
    int curRot = 0;
    int curRow = 0;
    int curCol = SCREEN_WIDTH / 2;
    int loopCount = 0;
    int fallCount = 0;
    int loopsTillFall = (DOWN_INTERVAL_MSEC / LOOP_INTERVAL_MSEC);
    int score = 0;
    while (!gameOver) {
        std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_INTERVAL_MSEC));

        // check input
        mtx.lock(); 
        curCol += (bKey[0] && isDeployable(blockType, curRot, curRow, curCol + 1)) ? 1 : 0;
        curCol -= (bKey[1] && isDeployable(blockType, curRot, curRow, curCol - 1)) ? 1 : 0;        
        curRow += (bKey[2] && isDeployable(blockType, curRot, curRow + 1, curCol)) ? 1 : 0;
        curRot += (bKey[3] && isDeployable(blockType, (curRot + 1) % 4, curRow, curCol)) ? 1 : 0;
        curRot %= 4;

        bKey.reset();
        mtx.unlock();

        ++loopCount;
        if (loopCount < loopsTillFall) {
            drawScreen(blockType, curRot, curRow, curCol);
            continue;
        }

        // block falls
        loopCount = 0;
        ++fallCount;
        if (fallCount % SPEEDUP_COUNT == 0) {
            loopsTillFall = std::max(10, loopsTillFall - 1);
        }

        if (isDeployable(blockType, curRot, curRow + 1, curCol)) {
            curRow++;
            drawScreen(blockType, curRot, curRow, curCol);
            continue;
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

        // check line complete
        int clines = 0;
        for (int i = curRow; (i < SCREEN_HEIGHT && i < curRow + 4); ++i) {
            bool complete = true; 
            for (int j = 0; j < SCREEN_WIDTH; ++j) {
                if (screen[i][j] == Pixel::Space) {
                    complete = false;
                    break;
                }
            }

            if (complete) {
                ++clines;
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

        if (clines != 0) {
            score += (1 << (clines - 1)) * 100;
        }

        // next block
        curCol = SCREEN_WIDTH / 2;
        curRow = 0;
        curRot = 0;
        blockType = rand() % 7;

        if (isDeployable(blockType, curRot, curRow, curCol) == false) {
            break; // Game over
        }

        drawScreen(blockType, curRot, curRow, curCol);
    }

    printf("Game Over. Score: %d\n", score);
    gameOver = true;
    th.join();

    // reset terminal
    struct termios termOrig;
    tcsetattr(STDIN_FILENO, TCSANOW, &termOrig);

    return 0;
}
