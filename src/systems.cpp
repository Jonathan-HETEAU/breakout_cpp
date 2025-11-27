#include <flecs.h>
#include <raylib.h>
#include <cmath>
#include <stdlib.h>
#include <algorithm>
#include "components.hpp"
#include "constante.hpp"

void DrawSystem(const flecs::entity &entity, const Position &position, const Shape &shape)
{

    switch (shape.type)
    {
    case Circle:
        DrawCircle(position.x + (shape.bound.width / 2), position.y + (shape.bound.width / 2), DemiUnit / 2, WHITE);
        break;
    case Rec:
        DrawRectangle(position.x, position.y, shape.bound.width, shape.bound.height, WHITE);
        DrawRectangle(position.x + shape.bound.width * 0.1, position.y + shape.bound.height * 0.1, shape.bound.width * 0.8, shape.bound.height * 0.8, LIGHTGRAY);
        break;
    case Square:
        break;
    default:
        break;
    }
}

void MoveSystem(flecs::iter &it, size_t, Position &p, const Velocity &v)
{
    p.x += v.x * SPEED * it.delta_time();
    p.y += v.y * SPEED * it.delta_time();
}

void InputSystem(const InputBinding &ib, Input &i)
{
    i.Left = IsKeyDown(ib.Left);
    i.Right = IsKeyDown(ib.Right);
}

void ActionSystem(flecs::iter &it, size_t, const Input &i, Velocity &v)
{
    v.x = (SPEED * it.delta_time()) * ((i.Left ? -1 : 0) + (i.Right ? 1 : 0));
}

void BallCollisionSystem(flecs::iter &it, size_t row, const Ball &b, Position &position, Velocity &v, const Shape &shape)
{
    auto paddles = it.world().query<Paddle, const Position, const Shape>();

    Rectangle ballRectangle = {position.x + shape.bound.x, position.y + shape.bound.y, shape.bound.width, shape.bound.height};

    paddles.each([&ballRectangle, &v, &position](Paddle, const Position &paddlePosition, const Shape &paddleShape)
                 {
        const Rectangle paddleRectangle = {paddlePosition.x + paddleShape.bound.x , paddlePosition.y + paddleShape.bound.y , paddleShape.bound.width , paddleShape.bound.height};
        if (CheckCollisionRecs(ballRectangle , paddleRectangle)){
            const Rectangle collision = GetCollisionRec (ballRectangle , paddleRectangle );
            if(collision.height < collision.width){
                position.y -= (2 *  collision.height);
                ballRectangle.y = position.y;
                v.y *= -1;
            }else{
                position.x -= (2 * collision.width);
                ballRectangle.x = position.x;
                v.x *= -1;
            }
        } });

    it.world().query<Brick, const Position, const Shape>().each([&ballRectangle, &v, &position](flecs::entity e, Brick, const Position &pb, const Shape &sb)
                                                                {
            const Rectangle brickRec = {pb.x + sb.bound.x , pb.y + sb.bound.y , sb.bound.width , sb.bound.height};
        if (CheckCollisionRecs(ballRectangle , brickRec)){
            const Rectangle collision = GetCollisionRec (ballRectangle , brickRec );
            if(collision.height < collision.width){
                position.y -= (2 *  collision.height);
                ballRectangle.y = position.y;
                v.y *= -1;
            }else{
                position.x -= (2 * collision.width);
                ballRectangle.x = position.x;
                v.x *= -1;
            }
            e.add<Hit>();
        } });

    Arena arena = it.world().get<Arena>();

    if (ballRectangle.x < 0 || ballRectangle.x >= arena.bound.width - ballRectangle.width)
    {
        position.x = std::min(arena.bound.width - ballRectangle.width, std::max(0.0f, ballRectangle.x));
        v.x *= -1;
    }
    if (ballRectangle.y < 0)
    {
        position.y = std::min(arena.bound.height - ballRectangle.height, std::max(0.0f, ballRectangle.y));
        v.y *= -1;
    }
    if (ballRectangle.y >= arena.bound.height - ballRectangle.height)
    {
        it.entity(row).add<Out>();
    }
}

void PaddleCollisionSystem(flecs::iter &it, size_t row, Paddle, Position &p, const Shape &s)
{
    const Arena arena = it.world().get<Arena>();
    p.x = std::min(arena.bound.width - s.bound.width, std::max(0.0f, p.x));
}
