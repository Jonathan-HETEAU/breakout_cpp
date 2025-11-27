#pragma once

#include <flecs.h>
#include "components.hpp"

void DrawSystem(const flecs::entity &entity, const Position &position, const Shape &shape);
void MoveSystem(flecs::iter &it, size_t, Position &p, const Velocity &v);
void InputSystem(const InputBinding &ib, Input &i);
void ActionSystem(flecs::iter &it, size_t, const Input &i, Velocity &v);
void BallCollisionSystem(flecs::iter &it, size_t, const Ball &b, Position &position, Velocity &v, const Shape &shape);