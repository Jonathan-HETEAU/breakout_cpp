#pragma once
#include <raylib.h>

struct Paddle {};
struct Ball {};
struct Brick {};

struct Hit{};

struct Out {};


struct Arena{
    Rectangle bound ; 
};

struct Position
{
    float x, y;
};

struct Velocity
{
    float x, y;
};

struct InputBinding
{
    int Left;
    int Right;
};

struct Input
{
    bool Left;
    bool Right;
};

enum ShapeType
{
    Circle,
    Rec,
    Square
};

struct Shape
{
    ShapeType type;
    Rectangle bound;
};