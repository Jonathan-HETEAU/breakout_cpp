#include <flecs.h>
#include <raylib.h>
#include <raymath.h>
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
                    auto normal = Vector2Normalize({-1, -1});
                    v = {normal.x , normal.y};
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

void addBrick(flecs::world &world, int x, int y)
{
    world.entity()
        .insert([x, y](Position &p, Velocity &v, Shape &s)
                {
                    p = {(float)x * Unit , (float)y * Unit};
                    v = {0,0};
                    s = {Rec , {0,0, 2 * Unit, Unit}}; })
        .add<Brick>();
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

    world.system<const Ball, Position, Velocity, const Shape>()
        .each(BallCollisionSystem);

    world.system<Paddle, Position, const Shape>()
        .each(PaddleCollisionSystem);

    world.system<const Position, const Shape>()
        .each(DrawSystem);

    world.system("HUD").run(DrawHudSystem);

    world.observer<Ball, Out>().event(flecs::OnAdd)
        .each([](flecs::entity e, Ball, Out)
            { 
                GameInfo gi = e.world().get<GameInfo>();
                gi.live --;
                e.world().set<GameInfo>(gi);
                e.destruct(); 
            });

    world.observer<Brick, Hit>().event(flecs::OnAdd)
        .each([](flecs::entity e, Brick, Hit)
            {
                GameInfo gi = e.world().get<GameInfo>();
                gi.score += 10;
                e.world().set<GameInfo>(gi); 
                e.destruct(); 
            });

    world.set<GameInfo>({ 3 , 0 });

    addBall(world);
    addPaddle(world);

    for( int x = 0 ; x < 12 ; x++){
        for( int y = 0 ; y < 4 ; y++){
            addBrick(world, x * 2, y);
        }
    }
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

    world.set<Arena>({0, 0, GAME_WIDTH, GAME_HEIGHT});

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            addBall(world);
        }

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