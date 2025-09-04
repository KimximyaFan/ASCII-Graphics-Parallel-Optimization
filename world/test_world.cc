#include "test_world.h"

void Test_World::SetWorld()
{
    width = 10;
    height = 16;

    world_map = {
        W, W, W, W, W, W, W, W, W, W,
        W, s, z, z, W, W, z, z, z, W,
        W, z, z, z, W, W, z, z, z, W,
        W, z, z, z, z, z, z, z, z, W,
        W, z, z, z, z, z, z, z, z, W,
        W, z, z, z, z, z, z, W, W, W,
        W, z, z, z, W, z, z, z, z, W,
        W, z, z, z, W, z, z, z, z, W,
        W, W, W, W, W, z, z, z, z, W,
        W, z, z, z, z, z, z, z, z, W,
        W, z, z, z, z, z, z, z, z, W,
        W, z, z, z, z, z, z, W, W, W,
        W, z, z, z, z, z, z, z, z, W,
        W, z, z, z, z, z, z, z, z, W,
        W, z, z, z, z, z, z, z, z, W,
        W, W, W, W, W, W, W, W, W, W
    };
}