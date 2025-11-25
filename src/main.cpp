#include <flecs.h>

struct Position
{
    float x, y;
};

struct Velocity
{
    float x, y;
};


void print_position(const flecs::entity& e, const Position& p)
{
    printf("Entity %u: Position(%f, %f)\n", (uint32_t)e.id(), p.x, p.y);
}

int main(int argc, char *argv[])
{
    flecs::world ecs;

    ecs.system<Position, const Velocity>()
        .each([](Position &p, const Velocity &v)
              {
      p.x += v.x;
      p.y += v.y; });

    auto e = ecs.entity()
                 .insert([](Position &p, Velocity &v)
                         {
      p = {10, 20};
      v = {1, 2}; });

    const flecs::system printSys = ecs.system<const Position>().each(print_position);

    while (ecs.progress())
    {
    }
}