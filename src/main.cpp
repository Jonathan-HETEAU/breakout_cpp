#include <flecs.h>
#include <raylib.h>
#include <cmath>

#include "components.hpp"
#include "constante.hpp"
#include "systems.hpp"

void addBall(flecs::world &world)
{
    world.entity()
        .insert([](Position &p, Velocity &v, Shape &s)
                {
                    p = {GAME_WIDTH/2.f,GAME_HEIGHT/2.f};
                    v = {0, 1}; 
                    s = {Circle , {0,0,DemiUnit,DemiUnit} }; })
        .add<Ball>();
}

void addPaddle(flecs::world &world)
{
    world.entity()
        .insert([](Position &p, Velocity &v, Shape &s, Input &i, InputBinding &ib)
                {
                    p = {GAME_WIDTH/2.f - Unit - DemiUnit ,(float)GAME_HEIGHT - (2 * Unit)};
                    v = {0,0};
                    s = {Rec , {0,0, 3 * Unit, DemiUnit}};
                    i = {false, false};
                    ib = {KEY_LEFT,KEY_RIGHT}; })
        .add<Paddle>();
}

flecs::world initWorld()
{
    flecs::world world;

    world.system<Position, const Velocity>()
        .each(MoveSystem);

    world.system<const InputBinding, Input>()
        .each(InputSystem);

    world.system<const Input, Velocity>()
        .each(ActionSystem);

    world.system<const Position, const Shape>()
        .each(DrawSystem);

    world.system<const Ball, Position, Velocity, const Shape>()
        .each(BallCollisionSystem);

    addBall(world);
    addPaddle(world);

    return world;
}

int main(int argc, char *argv[])
{

    InitWindow(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()), "Breakout");
    ToggleFullscreen();

    float aspectRatio = std::min(GetMonitorHeight(GetCurrentMonitor()) / (float)GAME_HEIGHT, GetMonitorWidth(GetCurrentMonitor()) / (float)GAME_WIDTH);
    int SCREEN_WIDTH = (int)std::ceil(GAME_WIDTH * aspectRatio);
    int SCREEN_HEIGHT = (int)std::ceil(GAME_HEIGHT * aspectRatio);
    Rectangle srcRec = {0, 0, GAME_WIDTH, -GAME_HEIGHT};
    Rectangle targetRec = {(GetMonitorWidth(GetCurrentMonitor()) - SCREEN_WIDTH) / 2.f, (GetMonitorHeight(GetCurrentMonitor()) - SCREEN_HEIGHT) / 2.f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
    RenderTexture2D target = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);

    flecs::world world = initWorld();

    while (!WindowShouldClose())
    {
        BeginTextureMode(target);
        ClearBackground(BLACK);
        world.progress();
        EndTextureMode();

        BeginDrawing();
        ClearBackground(WHITE);
        DrawTexturePro(target.texture, srcRec, targetRec, {0, 0}, 0, WHITE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}