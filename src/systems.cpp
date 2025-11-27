#include <flecs.h>
#include <raylib.h>
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

    const Rectangle ballRectangle = {position.x + shape.bound.x, position.y + shape.bound.y, shape.bound.width, shape.bound.height};

    paddles.each([&ballRectangle , &v , &position](Paddle, const Position &paddlePosition, const Shape &paddleShape)
                 {
        const Rectangle paddleRectangle = {paddlePosition.x + paddleShape.bound.x , paddlePosition.y + paddleShape.bound.y , paddleShape.bound.width , paddleShape.bound.height};
        if (CheckCollisionRecs(ballRectangle , paddleRectangle)){
            const Rectangle collision = GetCollisionRec (ballRectangle , paddleRectangle );
            TraceLog(LOG_INFO,TextFormat("collision:%f,%f,%f,%f", collision.x,collision.y , collision.width  , collision.height));
            if(collision.height < collision.width){
                position.y -= (2 *  collision.height);
                v.y *= -1;
            }else{
                position.x -= (2 * collision.width);
                v.x *= -1;
            }
        } });
}