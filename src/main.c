#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <time.h>
#include <stdint.h>
#include <assert.h>

const int defaultScreenWidth = 800;
const int defaultScreenHeight = 600;

const Color backgroundColor = DARKGRAY;

typedef enum GameState
{
    GAME_STATE_MAIN_MENU = 0,
    GAME_STATE_RUNNING,
    GAME_STATE_PAUSED,
} GameState;

typedef enum PieceType
{
    PIECE_O,
    PIECE_I,
    PIECE_S,
    PIECE_Z,
    PIECE_L,
    PIECE_J,
    PIECE_T,
    PIECE_COUNT = PIECE_T
} PieceType;

typedef enum Orientation
{
    ORIENTATION_NORMAL = 0,
    ORIENTATION_90,
    ORIENTATION_180,
    ORIENTATION_270,
    ORIENTATION_COUNT = ORIENTATION_270,
} Orientation;

typedef struct Block
{
    Color color;
} Block;

const Color INVALID_BLOCK_COLOR = (Color){
    .r = 0xDE,
    .g = 0xAD,
    .b = 0xBE,
    .a = 0xEF,
};

typedef struct Piece
{
    Color color;
    PieceType type;
} Piece;

typedef struct GridPiece
{
    Piece data;
    Orientation orientation;
    Vector2 origin;
} GridPiece;

#define PIECE_PARTS_COUNT_1D 4
#define PIECE_PARTS_COUNT (PIECE_PARTS_COUNT_1D * PIECE_PARTS_COUNT_1D)

bool PIECE_O_PARTS[PIECE_PARTS_COUNT] = {
    0, 1, 1, 0, //
    0, 1, 1, 0, //
    0, 0, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_I_PARTS_NORMAL[PIECE_PARTS_COUNT] = {
    0, 0, 1, 0, //
    0, 0, 1, 0, //
    0, 0, 1, 0, //
    0, 0, 1, 0, //
};

bool PIECE_I_PARTS_90[PIECE_PARTS_COUNT] = {
    0, 0, 0, 0, //
    0, 0, 0, 0, //
    1, 1, 1, 1, //
    0, 0, 0, 0, //
};

bool PIECE_I_PARTS_180[PIECE_PARTS_COUNT] = {
    0, 1, 0, 0, //
    0, 1, 0, 0, //
    0, 1, 0, 0, //
    0, 1, 0, 0, //
};

bool PIECE_I_PARTS_270[PIECE_PARTS_COUNT] = {
    0, 0, 0, 0, //
    1, 1, 1, 1, //
    0, 0, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_S_PARTS_NORMAL[PIECE_PARTS_COUNT] = {
    0, 1, 1, 0, //
    1, 1, 0, 0, //
    0, 0, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_S_PARTS_90[PIECE_PARTS_COUNT] = {
    0, 1, 0, 0, //
    0, 1, 1, 0, //
    0, 0, 1, 0, //
    0, 0, 0, 0, //
};

bool PIECE_S_PARTS_180[PIECE_PARTS_COUNT] = {
    0, 0, 0, 0, //
    0, 1, 1, 0, //
    1, 1, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_S_PARTS_270[PIECE_PARTS_COUNT] = {
    1, 0, 0, 0, //
    1, 1, 0, 0, //
    0, 1, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_Z_PARTS_NORMAL[PIECE_PARTS_COUNT] = {
    1, 1, 0, 0, //
    0, 1, 1, 0, //
    0, 0, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_Z_PARTS_90[PIECE_PARTS_COUNT] = {
    0, 0, 1, 0, //
    0, 1, 1, 0, //
    0, 1, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_Z_PARTS_180[PIECE_PARTS_COUNT] = {
    0, 0, 0, 0, //
    1, 1, 0, 0, //
    0, 1, 1, 0, //
    0, 0, 0, 0, //
};

bool PIECE_Z_PARTS_270[PIECE_PARTS_COUNT] = {
    0, 1, 0, 0, //
    1, 1, 0, 0, //
    1, 0, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_L_PARTS_NORMAL[PIECE_PARTS_COUNT] = {
    0, 1, 0, 0, //
    0, 1, 0, 0, //
    0, 1, 1, 0, //
    0, 0, 0, 0, //
};

bool PIECE_L_PARTS_90[PIECE_PARTS_COUNT] = {
    0, 0, 0, 0, //
    0, 1, 1, 1, //
    0, 1, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_L_PARTS_180[PIECE_PARTS_COUNT] = {
    1, 1, 0, 0, //
    0, 1, 0, 0, //
    0, 1, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_L_PARTS_270[PIECE_PARTS_COUNT] = {
    0, 0, 1, 0, //
    1, 1, 1, 0, //
    0, 0, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_J_PARTS_NORMAL[PIECE_PARTS_COUNT] = {
    0, 1, 0, 0, //
    0, 1, 0, 0, //
    1, 1, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_J_PARTS_90[PIECE_PARTS_COUNT] = {
    1, 0, 0, 0, //
    1, 1, 1, 0, //
    0, 0, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_J_PARTS_180[PIECE_PARTS_COUNT] = {
    0, 1, 1, 0, //
    0, 1, 0, 0, //
    0, 1, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_J_PARTS_270[PIECE_PARTS_COUNT] = {
    0, 0, 0, 0, //
    1, 1, 1, 0, //
    0, 0, 1, 0, //
    0, 0, 0, 0, //
};

bool PIECE_T_PARTS_NORMAL[PIECE_PARTS_COUNT] = {
    0, 0, 0, 0, //
    1, 1, 1, 0, //
    0, 1, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_T_PARTS_90[PIECE_PARTS_COUNT] = {
    0, 1, 0, 0, //
    1, 1, 0, 0, //
    0, 1, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_T_PARTS_180[PIECE_PARTS_COUNT] = {
    0, 1, 0, 0, //
    1, 1, 1, 0, //
    0, 0, 0, 0, //
    0, 0, 0, 0, //
};

bool PIECE_T_PARTS_270[PIECE_PARTS_COUNT] = {
    0, 1, 0, 0, //
    0, 1, 1, 0, //
    0, 1, 0, 0, //
    0, 0, 0, 0, //
};

bool *piecePartsForOrientation(const Orientation orientation, const PieceType pieceType)
{
    switch (pieceType)
    {
    case PIECE_O:
    {
        return PIECE_O_PARTS;
    }
    case PIECE_I:
    {
        return (bool *[4]){PIECE_I_PARTS_NORMAL, PIECE_I_PARTS_90, PIECE_I_PARTS_180, PIECE_I_PARTS_270}[orientation];
    }
    case PIECE_S:
    {
        return (bool *[4]){PIECE_S_PARTS_NORMAL, PIECE_S_PARTS_90, PIECE_S_PARTS_180, PIECE_S_PARTS_270}[orientation];
    }
    case PIECE_Z:
    {
        return (bool *[4]){PIECE_Z_PARTS_NORMAL, PIECE_Z_PARTS_90, PIECE_Z_PARTS_180, PIECE_Z_PARTS_270}[orientation];
    }
    case PIECE_L:
    {
        return (bool *[4]){PIECE_L_PARTS_NORMAL, PIECE_L_PARTS_90, PIECE_L_PARTS_180, PIECE_L_PARTS_270}[orientation];
    }
    case PIECE_J:
    {
        return (bool *[4]){PIECE_J_PARTS_NORMAL, PIECE_J_PARTS_90, PIECE_J_PARTS_180, PIECE_J_PARTS_270}[orientation];
    }
    case PIECE_T:
    {
        return (bool *[4]){PIECE_T_PARTS_NORMAL, PIECE_T_PARTS_90, PIECE_T_PARTS_180, PIECE_T_PARTS_270}[orientation];
    }
    }

    assert(0);
}

const Color colors[] = {
    RED,
    BLUE,
    GREEN,
    ORANGE,
    YELLOW,
};

const uint8_t colorsLength = sizeof(colors) / sizeof(colors[1]);

// TODO: better name for this function?
Vector2 partCoordinatesFromIndex(size_t i, const Vector2 origin)
{
    return (Vector2){
        .x = origin.x + (i % 4),
        .y = origin.y + (i / 4),
    };
}

typedef struct GridPieceParts
{
    Vector2 coordinates[PIECE_PARTS_COUNT_1D];
} GridPieceParts;

GridPieceParts constructGridPieceParts(const GridPiece *piece)
{
    GridPieceParts gridPieceParts;

    bool *hasPart = piecePartsForOrientation(piece->orientation, piece->data.type);
    size_t index = 0;

    for (size_t i = 0; i < PIECE_PARTS_COUNT; i++)
    {
        if (!hasPart[i])
        {
            continue;
        }

        Vector2 pieceCoordinates = partCoordinatesFromIndex(i, piece->origin);
        gridPieceParts.coordinates[index++] = pieceCoordinates;
    }

    return gridPieceParts;
};

void drawGridPiece(const Vector2 gridOrigin, const Vector2 blockSize, const GridPiece *piece)
{
    GridPieceParts parts = constructGridPieceParts(piece);

    for (size_t i = 0; i < PIECE_PARTS_COUNT_1D; i++)
    {
        Vector2 pieceCoordinates = parts.coordinates[i];
        Vector2 coordinates = Vector2Add(Vector2Multiply(pieceCoordinates, blockSize), gridOrigin);

        DrawRectangleV(coordinates, blockSize, piece->data.color);
    }
}

void drawNextPiece(const Vector2 origin, size_t slot, const Vector2 blockSize, const uint16_t padding, const Piece piece)
{
    GridPieceParts parts = constructGridPieceParts(&(GridPiece){
        .data = piece,
        .orientation = ORIENTATION_NORMAL,
        .origin = (Vector2){
            .x = 0,
            .y = PIECE_PARTS_COUNT_1D * slot,
        },
    });

    for (size_t i = 0; i < PIECE_PARTS_COUNT_1D; i++)
    {
        const Vector2 coordinates = Vector2Add(
            Vector2Add(
                Vector2Multiply(parts.coordinates[i], blockSize),
                origin),
            (Vector2){
                .x = 0,
                .y = padding * slot,
            });
        DrawRectangleV(coordinates, blockSize, piece.color);
        DrawRectangleLines(coordinates.x, coordinates.y, blockSize.x, blockSize.y, BLACK);
    }
}

void drawSavedPiece(const Vector2 origin, const Vector2 blockSize, const Piece piece)
{
    GridPieceParts parts = constructGridPieceParts(&(GridPiece){
        .data = piece,
        .orientation = ORIENTATION_NORMAL,
        .origin = Vector2Zero(),
    });

    for (size_t i = 0; i < PIECE_PARTS_COUNT_1D; i++)
    {
        const Vector2 coordinates = Vector2Add(
            Vector2Multiply(parts.coordinates[i], blockSize),
            origin);
        DrawRectangleV(coordinates, blockSize, piece.color);
        DrawRectangleLines(coordinates.x, coordinates.y, blockSize.x, blockSize.y, BLACK);
    }
}

// 12<-> * ^10
#define w 10
#define h 20
Block grid[w * h] = {0};

bool isInsideGrid(const Vector2 coordinate)
{
    return !(
        coordinate.x < 0 ||
        coordinate.x >= w ||
        coordinate.y < 0 ||
        coordinate.y >= h //
    );
}

int gridIndexFromCoordinate(const Vector2 coordinate)
{
    return coordinate.y * w + coordinate.x;
}

bool isBlockInGrid(const Block grid[w * h], Vector2 coordinate)
{
    int index = coordinate.y * w + coordinate.x;
    return memcmp(&grid[index].color, &INVALID_BLOCK_COLOR, sizeof(grid[index].color));
}

typedef enum HitResult
{
    NO_HIT,
    HIT,
    GAME_OVER
} HitResult;

HitResult checkCollisions(const Block grid[w * h], const GridPiece *piece)
{
    GridPieceParts parts = constructGridPieceParts(piece);

    for (size_t i = 0; i < PIECE_PARTS_COUNT_1D; i++)
    {
        Vector2 partCoordinates = parts.coordinates[i];

        if (!isInsideGrid(partCoordinates) || isBlockInGrid(grid, partCoordinates))
        {
            return (partCoordinates.y == 0 || piece->origin.y == 0) ? GAME_OVER : HIT;
        }
    }

    return NO_HIT;
}

typedef struct LinesResult
{
    bool destroyed[PIECE_PARTS_COUNT_1D];
} LinesResult;

bool inSet(int *items, int itemLength, int item)
{
    for (size_t i = 0; i < itemLength; i++)
    {
        if (items[i] == item)
            return true;
    }

    return false;
}

LinesResult checkLines(const Block grid[w * h], const GridPiece *piece)
{
    LinesResult result = {
        .destroyed = {false, false, false, false},
    };

    bool seen[PIECE_PARTS_COUNT_1D] = {false, false, false, false};
    GridPieceParts parts = constructGridPieceParts(piece);

    for (size_t i = 0; i < PIECE_PARTS_COUNT_1D; ++i)
    {
        Vector2 partCoordinates = parts.coordinates[i];
        int index = (int)partCoordinates.y - piece->origin.y;

        if (seen[index])
        {
            continue;
        }

        seen[index] = true;
        partCoordinates.x = 0;

        for (size_t j = 0; j < w; ++j)
        {
            partCoordinates.x = j;
            if (!isBlockInGrid(grid, partCoordinates))
            {
                goto outer;
            }
        }
        result.destroyed[index] = true;
    outer:
    }

    return result;
}

void memset32(void *buf, uint32_t c, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++)
    {
        ((uint32_t *)buf)[i] = c;
    }
}

void applyGravityToBlocks(Block grid[w * h], uint8_t firstLine, uint8_t numberOfLines)
{
    uint8_t line = firstLine - numberOfLines + 1;
    size_t indexDest = gridIndexFromCoordinate((Vector2){
        .x = 0,
        .y = numberOfLines,
    });

    size_t indexSrc = 0;

    memmove(&grid[indexDest], &grid[indexSrc], (line * w) * sizeof(Block));

    //
    Block emptyBlock = {
        .color = INVALID_BLOCK_COLOR,
    };
    uint32_t emptyBlockAsInt = ((uint32_t *)&emptyBlock)[0];

    memset32(&grid[0], emptyBlockAsInt, (numberOfLines * w));
}

HitResult applyGravity(const Block grid[w * h], GridPiece *piece)
{
    piece->origin.y += 1;

    HitResult result = checkCollisions(grid, piece);

    if (result != NO_HIT)
    {
        piece->origin.y -= 1;
    }

    return result;
}

void movePieceToSides(const Block grid[w * h], GridPiece *piece)
{
    int movement = 0;
    if (IsKeyDown(KEY_A))
    {
        movement = -1;
    }

    else if (IsKeyDown(KEY_D))
    {
        movement = 1;
    }

    else
    {
        return;
    }

    piece->origin.x += movement;
    HitResult result = checkCollisions(grid, piece);

    if (result != NO_HIT)
    {
        piece->origin.x -= movement;
    }
}

// TODO: rotate sometimes blocks where it could rotate
void rotatePiece(const Block grid[w * h], GridPiece *piece)
{
    Orientation newOrientation = (piece->orientation + 1) % (ORIENTATION_COUNT + 1);
    Orientation oldOrientation = piece->orientation;
    piece->orientation = newOrientation;

    HitResult result = checkCollisions(grid, piece);

    if (result != NO_HIT)
    {
        piece->orientation = oldOrientation;
    }
}

Piece makeRandomPiece(void)
{
    PieceType type = rand() % (PIECE_COUNT + 1);
    Color color = colors[rand() % colorsLength];

    return (Piece){
        color,
        type,
    };
}

void initGrid(void)
{
    for (size_t i = 0; i < w * h; ++i)
    {
        grid[i].color = INVALID_BLOCK_COLOR;
    }
}

int score = 0;
int level = 1;
bool dead = false;

#define NEXT_PIECES_COUNT 3

Piece nextPieces[NEXT_PIECES_COUNT];
GridPiece piece;
Piece *savedPiece = NULL;
bool savedThisPiece = false;

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

double nextTimeToLevelUp(int level)
{
    // return 0.1;
    return powf(level, 0.5f) * 10.0;
}

GridPiece dequeueNextPiece()
{
    Piece piece = nextPieces[0];

    for (size_t i = 0; i < NEXT_PIECES_COUNT - 1; i++)
    {
        nextPieces[i] = nextPieces[i + 1];
    }

    nextPieces[NEXT_PIECES_COUNT - 1] = makeRandomPiece();

    return (GridPiece){
        .data = piece,
        .orientation = ORIENTATION_NORMAL,
        .origin = (Vector2){
            .x = w / 2 - 1,
            .y = 0,
        },
    };
}

void drawGrid(const Vector2 origin, const Vector2 blockSize, const Block grid[w * h])
{
    for (size_t i = 0; i < w * h; i++)
    {
        const Vector2 gridCoordinate = (Vector2){
            .x = i % w,
            .y = i / w,
        };

        const Vector2 coordinates = Vector2Add(
            Vector2Multiply(gridCoordinate, blockSize),
            origin);

        if (isBlockInGrid(grid, gridCoordinate))
        {
            DrawRectangleV(coordinates, blockSize, grid[i].color);
        }

        DrawRectangleLines(coordinates.x, coordinates.y, blockSize.x, blockSize.y, BLACK);
    }
}

void initState()
{
    score = 0;
    dead = false;
    initGrid();
    savedThisPiece = false;

    free(savedPiece);
    savedPiece = NULL;

    for (size_t i = 0; i < NEXT_PIECES_COUNT; ++i)
    {
        nextPieces[i] = makeRandomPiece();
    }

    piece = dequeueNextPiece();
}

int main(void)
{
    InitWindow(defaultScreenWidth, defaultScreenHeight, "raylib [core] example - basic window");
    SetTargetFPS(60);

    srand(time(NULL));

    double lastPhysics = GetTime();
    double lastMovement = GetTime();

    double timeToLevelUp = nextTimeToLevelUp(level);
    double lastLevelUp = GetTime();

    GameState gameState = GAME_STATE_MAIN_MENU;

    float lastHeight = 0;
    float lastWidth = 0;

    RenderTexture2D target;

    SetExitKey(KEY_NULL);

    while (!WindowShouldClose())
    {
        float height = GetScreenHeight();
        float width = GetScreenWidth();

        if (fabsf(height - lastHeight) > 0.001 || fabsf(width - lastWidth))
        {
            lastHeight = height;
            lastWidth = width;
            target = LoadRenderTexture(width, height);
        }

        if (gameState == GAME_STATE_MAIN_MENU)
        {
            BeginDrawing();
            {
                const uint16_t padding = 50;
                const uint16_t buttonWidth = width - padding * 2;
                const uint16_t buttonHeight = floorf(100.0 * height / defaultScreenHeight);
                const Rectangle startGameRectangle = {
                    .height = buttonHeight,
                    .width = buttonWidth,
                    .x = padding,
                    .y = height / 2 - buttonHeight / 2,
                };
                const uint8_t fontSize = 48;

                GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
                if (GuiButton(startGameRectangle, "Play"))
                {
                    gameState = GAME_STATE_RUNNING;
                    initState();
                }
            }
            EndDrawing();
        }
        else if (gameState == GAME_STATE_RUNNING || gameState == GAME_STATE_PAUSED)
        {
            double now = GetTime();

            if (!dead && IsKeyPressed(KEY_ESCAPE))
            {
                gameState = gameState == GAME_STATE_RUNNING ? GAME_STATE_PAUSED : GAME_STATE_RUNNING;
                lastMovement = now;
                lastPhysics = now;
                lastLevelUp = now;
            }

            bool paused = gameState == GAME_STATE_PAUSED;

            bool canLevelUp = !dead && !paused && level < 20;
            bool levelUp = (now - lastLevelUp) > timeToLevelUp;
            if (canLevelUp && levelUp)
            {
                level++;
                timeToLevelUp = nextTimeToLevelUp(level);
                lastLevelUp = now;
            }

            bool isMovementTime = (now - lastMovement) > (1000.0 / 20.0) / (1000.0);
            bool isPhysicsTime = (now - lastPhysics) > (1 / (0.5f + (level * 0.5)));

            bool canSavePiece = !paused && !dead && !savedThisPiece;
            bool canManualMove = !paused && !dead && isMovementTime;

            bool doApplyGravity = !paused && !dead && isPhysicsTime;
            bool doInstantDrop = !paused && !dead && IsKeyPressed(KEY_SPACE);
            bool doFasterDrop = !paused && !dead && IsKeyDown(KEY_S);
            bool doRotate = !paused && !dead && IsKeyPressed(KEY_W);
            bool doSave = canSavePiece && IsKeyDown(KEY_Q);

            if (!paused && IsKeyPressed(KEY_R))
            {
                initState();
            }

            if (doSave)
            {
                if (savedPiece != NULL)
                {
                    *savedPiece = piece.data;
                    piece = (GridPiece){
                        .data = *savedPiece,
                        .orientation = ORIENTATION_NORMAL,
                        .origin = (Vector2){
                            .x = w / 2 - 1,
                            .y = 0,
                        },
                    };
                }
                else
                {
                    savedPiece = malloc(sizeof(*savedPiece));
                    memcpy(savedPiece, &piece, sizeof(*savedPiece));
                    piece = dequeueNextPiece();
                }
                savedThisPiece = true;
            }

            if (doRotate)
            {
                rotatePiece(grid, &piece);
            }

            if (canManualMove)
            {
                movePieceToSides(grid, &piece);
                lastMovement = now;
            }

            HitResult result = NO_HIT;
            if (doInstantDrop)
            {
                while (result == NO_HIT)
                {
                    result = applyGravity(grid, &piece);
                    score += 20 * level;
                }
                lastPhysics = now;
            }
            else if (doFasterDrop && canManualMove)
            {
                result = applyGravity(grid, &piece);
                score += 10 * level;
                lastPhysics = now;
            }
            else if (doApplyGravity)
            {
                result = applyGravity(grid, &piece);
                lastPhysics = now;
            }

            if (result == HIT)
            {
                GridPieceParts parts = constructGridPieceParts(&piece);
                for (size_t i = 0; i < PIECE_PARTS_COUNT_1D; ++i)
                {
                    Vector2 partCoordinates = parts.coordinates[i];

                    int index = gridIndexFromCoordinate(partCoordinates);
                    grid[index] = (Block){
                        .color = piece.data.color,
                    };
                }

                LinesResult linesResult = checkLines(grid, &piece);

                uint8_t numberOfLines = 0;
                int start = -1;

                for (size_t i = 0; i < PIECE_PARTS_COUNT_1D; ++i)
                {
                    if (linesResult.destroyed[i])
                    {
                        numberOfLines++;
                        start = MAX(start, piece.origin.y + i);
                    }
                }

                if (numberOfLines > 0)
                {
                    applyGravityToBlocks(grid, start, numberOfLines);
                    uint16_t scores[] = {100, 300, 500, 800};
                    score += scores[numberOfLines - 1] * level;
                }

                piece = dequeueNextPiece();
                savedThisPiece = false;
                result = checkCollisions(grid, &piece);
            }

            if (result == GAME_OVER)
            {
                dead = true;
            }

            GridPiece ghostPiece;

            if (result == NO_HIT)
            {
                ghostPiece = piece;

                while (applyGravity(grid, &ghostPiece) == NO_HIT)
                {
                    ;
                }

                ghostPiece.data.color.a = piece.data.color.a * 0.3;
            }

            uint16_t paddingTop = (int)floorf(10.0 / defaultScreenHeight * height);
            uint16_t paddingSides = (int)floorf(15.0 / defaultScreenHeight * height);
            uint16_t paddingComponents = (int)floorf(20.0 / defaultScreenHeight * height);
            uint8_t blockSize = (int)floorf(28.0 / defaultScreenHeight * height);
            uint16_t gridWidth = blockSize * w;
            uint16_t gridStartX = paddingSides + ((width - paddingSides * 2) / 2 - (gridWidth / 2));

            const Vector2 savedPieceStart = {
                .x = gridStartX - paddingComponents - blockSize * PIECE_PARTS_COUNT_1D,
                .y = paddingTop,
            };

            const Vector2 gridStart = {
                .x = gridStartX,
                .y = paddingTop,
            };

            const Vector2 nextPiecesStart = {
                .x = gridStartX + gridWidth + paddingComponents,
                .y = paddingTop,
            };

            const Vector2 scoreLevelStart = {
                .x = nextPiecesStart.x,
                .y = nextPiecesStart.y + paddingComponents + blockSize * PIECE_PARTS_COUNT_1D * NEXT_PIECES_COUNT,
            };

            const Vector2 blockSizes = {
                .x = blockSize,
                .y = blockSize,
            };

            uint8_t fontSize = (int)floorf(28.0 / defaultScreenHeight * height);

            BeginDrawing();
            {
                if (paused)
                {
                    BeginTextureMode(target);
                }

                ClearBackground(backgroundColor);

                drawGridPiece(gridStart, blockSizes, &piece);
                if (result == NO_HIT)
                {
                    drawGridPiece(gridStart, blockSizes, &ghostPiece);
                }

                for (size_t i = 0; i < NEXT_PIECES_COUNT; i++)
                {
                    drawNextPiece(nextPiecesStart, i, blockSizes, paddingTop, nextPieces[i]);
                }

                if (savedPiece != NULL)
                {
                    drawSavedPiece(savedPieceStart, blockSizes, *savedPiece);
                }

                drawGrid(gridStart, blockSizes, grid);

                if (dead)
                {
                    const char *restartText = "You die!";
                    const Vector2 restartTextSize = MeasureTextEx(GetFontDefault(), restartText, fontSize, 10);

                    DrawText(restartText, gridStart.x + 5, gridStart.y, fontSize, RED);
                    DrawText("Press R to restart", gridStart.x + 5, gridStart.y + restartTextSize.y, fontSize, RED);
                }

                const Vector2 scoreCoordinates = (Vector2){
                    .x = scoreLevelStart.x,
                    .y = scoreLevelStart.y,
                };

                DrawText(TextFormat("Score: %08i", score), scoreCoordinates.x, scoreCoordinates.y, fontSize, BLACK);

                const Vector2 levelCoordinates = (Vector2){
                    .x = scoreLevelStart.x,
                    .y = scoreLevelStart.y + fontSize,
                };

                DrawText(TextFormat("Level: %02i", level), levelCoordinates.x, levelCoordinates.y, fontSize, BLACK);

                if (paused)
                {
                    EndTextureMode();
                    DrawTextureRec(target.texture, (Rectangle){0, 0, (float)target.texture.width, (float)-target.texture.height}, Vector2Zero(), GRAY);
                    DrawText("Paused", width / 2, height / 2, fontSize, RED);
                }
            }

            EndDrawing();
        }
    }

    CloseWindow();

    return 0;
}