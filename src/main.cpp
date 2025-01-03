#include <random>
#include "raylib.h"

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

inline constexpr int SCREEN_WIDTH = 800;
inline constexpr int SCREEN_HEIGHT = 450;

inline constexpr Color BACKGROUND_COLOR = Color{8, 33, 25, 255};
inline constexpr Color FOREGROUND_COLOR = Color{158, 183, 175, 255};

inline constexpr int TITLE_FONT_SIZE = 42;
inline static int titleWidth;

inline constexpr int SCORE_FONT_SIZE = 60;

inline constexpr float INITIAL_BALL_SPEED = 8.0f;

struct Paddle
{
    Vector2 position;
    float width{10.0f};
    float height{100.0f};
    float speed{5.0f};
};

struct Ball
{
    Vector2 position;
    Vector2 direction;
    float speed{INITIAL_BALL_SPEED};
    float radius{5.0f};
};

struct GameState
{
    int leftScore{0};
    int rightScore{0};
};

inline static std::random_device rd;
inline static std::mt19937 gen(rd());
inline static std::uniform_real_distribution<float> dis(0.3f, 0.7f);

inline static Paddle leftPaddle;
inline static Paddle rightPaddle;

inline static Ball ball;

inline static GameState gameState;

static void UpdateDrawFrame(void);
static void UpdatePaddlePosition(Paddle &paddle, const Vector2 &position);
static void UpdateBallPosition(Ball &ball, const Vector2 &position);
static void ResetBallPosition(void);
static void DrawScore(void);

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PONG");

    titleWidth = MeasureText("PONG", TITLE_FONT_SIZE);

    leftPaddle = {Vector2{leftPaddle.width, SCREEN_HEIGHT / 2 - leftPaddle.height / 2}};
    rightPaddle = {Vector2{SCREEN_WIDTH - rightPaddle.width * 2, SCREEN_HEIGHT / 2 - rightPaddle.height / 2}};

    ball = {Vector2{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, Vector2{dis(gen), dis(gen)}};

#ifdef PLATFORM_WEB
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    return 0;
}

static void UpdateDrawFrame(void)
{
    if (IsKeyDown(KEY_W))
    {
        UpdatePaddlePosition(leftPaddle, {leftPaddle.position.x, leftPaddle.position.y - leftPaddle.speed});
    }
    else if (IsKeyDown(KEY_S))
    {
        UpdatePaddlePosition(leftPaddle, {leftPaddle.position.x, leftPaddle.position.y + leftPaddle.speed});
    }

    if (IsKeyDown(KEY_UP))
    {
        UpdatePaddlePosition(rightPaddle, {rightPaddle.position.x, rightPaddle.position.y - rightPaddle.speed});
    }
    else if (IsKeyDown(KEY_DOWN))
    {
        UpdatePaddlePosition(rightPaddle, {rightPaddle.position.x, rightPaddle.position.y + rightPaddle.speed});
    }

    UpdateBallPosition(ball, {ball.position.x + ball.direction.x * ball.speed, ball.position.y + ball.direction.y * ball.speed});

    BeginDrawing();
    ClearBackground(BACKGROUND_COLOR);
    DrawText("PONG", (SCREEN_WIDTH / 2) - (titleWidth / 2), (SCREEN_HEIGHT / 2) - 21, 42, FOREGROUND_COLOR);
    DrawRectangleV({leftPaddle.position.x, leftPaddle.position.y}, {leftPaddle.width, leftPaddle.height}, FOREGROUND_COLOR);
    DrawRectangleV({rightPaddle.position.x, rightPaddle.position.y}, {rightPaddle.width, rightPaddle.height}, FOREGROUND_COLOR);
    DrawCircleV(ball.position, ball.radius, FOREGROUND_COLOR);
    DrawScore();
    EndDrawing();
}

static void UpdatePaddlePosition(Paddle &paddle, const Vector2 &position)
{
    if (position.y < 0 || position.y > SCREEN_HEIGHT - paddle.height)
    {
        return;
    }
    paddle.position = position;
}

static void UpdateBallPosition(Ball &ball, const Vector2 &position)
{
    ball.position = position;
    if (ball.position.y < 0 || ball.position.y > SCREEN_HEIGHT - ball.radius)
    {
        ball.direction.y = -ball.direction.y;
    }
    if (CheckCollisionCircleRec(ball.position, ball.radius, {leftPaddle.position.x, leftPaddle.position.y, leftPaddle.width, leftPaddle.height}) ||
        CheckCollisionCircleRec(ball.position, ball.radius, {rightPaddle.position.x, rightPaddle.position.y, rightPaddle.width, rightPaddle.height}))
    {
        ball.direction.x = -ball.direction.x;
        ball.speed += 0.5f;
    }

    if (ball.position.x < 0)
    {
        gameState.rightScore++;
        ResetBallPosition();
    }
    else if (ball.position.x > SCREEN_WIDTH)
    {
        gameState.leftScore++;
        ResetBallPosition();
    }
}

static void ResetBallPosition(void)
{
    ball.position = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    ball.direction = {dis(gen), dis(gen)};
    ball.speed = INITIAL_BALL_SPEED;
}

static void DrawScore(void)
{
    auto leftScoreWidth = MeasureText(TextFormat("%i", gameState.leftScore), SCORE_FONT_SIZE);
    auto rightScoreWidth = MeasureText(TextFormat("%i", gameState.rightScore), SCORE_FONT_SIZE);
    DrawText(TextFormat("%i", gameState.leftScore), (SCREEN_WIDTH / 4) - (leftScoreWidth / 2), 10, SCORE_FONT_SIZE, FOREGROUND_COLOR);
    DrawText(TextFormat("%i", gameState.rightScore), ((SCREEN_WIDTH / 4) * 3) - (rightScoreWidth / 2), 10, SCORE_FONT_SIZE, FOREGROUND_COLOR);
}