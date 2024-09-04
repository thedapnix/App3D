#include "LevelSetup.h"

using namespace DirectX;

void SetupLevel1(D3D11Engine* engine)
{
    //FIRST ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f,  0.0f, -5.0f }, { 2.0f, 1.0f, 2.0f });
    //engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f, 18.0f, -5.0f }, { 2.0f, 1.0f, 2.0f });

    //FRONT WALL
    //Door (elongated metal crate with a small metal sphere as a makeshift doorknob, sue me)
    //engine->CreateDrawable("Meshes/door_metal.obj", { 0.0f, 9.0f, 15.0f }, { 5.0f, 8.0f, 1.0f });
    //engine->CreateDrawable("Meshes/sphere_metal.obj", { -2.75f, 9.25f, 13.75f }, { 0.25f, 0.25f, 0.25f });
    //Walls surrounding the door (left, then right)
    engine->CreateDrawable("Meshes/wall_brick.obj", { -12.5f, 9.0f, 15.0f }, { 7.5f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 12.5f,  9.0f, 15.0f }, { 7.5f, 8.0f, 1.0f });

    //LEFT AND RIGHT WALLS
    engine->CreateDrawable("Meshes/wall_brick.obj", { -20.0f, 9.0f, -5.0f }, { 20.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 20.0f, 9.0f, -5.0f }, { 20.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //BACK WALL
    engine->CreateDrawable("Meshes/wall_brick.obj", { 0.0f, 9.0f, -25.0f }, { 20.0f, 8.0f, 1.0f });

    //Clutter on the left side of the door
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f, 3.0f, 10.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_blue.obj", { -8.0f, 6.0f, 10.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f,  0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f, 2.0f,  6.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -5.0f, 2.0f, 10.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -4.0f, 2.0f,  6.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -6.0f, 2.0f,  3.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -14.0f, 3.0f, 10.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -14.0f, 2.0f,  6.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -10.0f, 2.0f,  3.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -10.0f, 4.0f,  3.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -12.0f, 2.0f,  0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //Clutter on the right side of the door
    engine->CreateDrawable("Meshes/crate_wood.obj", { 7.0f, 3.0f, 11.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f, 2.0f,  7.0f }, { 1.0f, 1.0f, 1.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f, 4.0f,  7.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f, 2.0f,  3.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 6.0f, 2.0f,  5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 12.0f, 2.0f,  6.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f, 2.0f,  7.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 12.0f, 2.0f,  3.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f, 3.0f, 11.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f, 6.0f, 11.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //Light(s)
    engine->CreateLightSpot({ 0.0f, 17.0f, -5.0f }, 0.75f, 0.0f, 0.5f); //Ceiling in the center of the room pointing straight down onto the floor

    //HALLWAY AFTER FIRST ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f,  0.0f, 25.0f });
    //engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f, 18.0f, 25.0f });

    //Walls (left, then right)
    engine->CreateDrawable("Meshes/wall_brick.obj", { -10.0f, 9.0f, 26.0f }, { 11.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 10.0f, 9.0f, 26.0f }, { 11.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //SECOND ROOM
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f, 0.0f, 55.0f }, { 2.0f, 1.0f, 2.0f });
    engine->CreateDrawable("Meshes/ground_stone.obj", { -40.0f, 0.0f, 55.0f }, { 2.0f, 1.0f, 2.0f });

    engine->CreateDrawable("Meshes/wall_brick.obj", { 16.0f, 9.0f, 37.0f }, { 5.0f, 8.0f, 1.0f });
}

void SetupLevel2(D3D11Engine* engine)
{
    //DRAWABLES IN LEVEL2: 6 + 12
    //Ground and floor
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f + 60.0f, -10.0f,   7.0f }, { 1.8f, 1.0f, 1.8f });
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f + 60.0f,   8.0f,   7.0f }, { 1.8f, 1.0f, 1.8f });

    //Walls (left, then right)
    engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f + 60.0f, -1.0f,  19.0f }, { 10.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 15.0f + 60.0f, -1.0f,  19.0f }, { 10.0f, 8.0f, 1.0f });

    //Door (elongated metal crate with a small metal sphere as a makeshift doorknob)
    engine->CreateDrawable("Meshes/door_metal.obj", { 0.0f + 60.0f, -1.0f, 19.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { -2.75f + 60.0f, -1.25f, 17.75f }, { 0.25f, 0.25f, 0.25f });

    //LEFT SIDE
    //In order: Big box, box on top of it, box in front of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 60.0f, -7.0f, 15.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 60.0f, -4.0f, 15.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f,  0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 60.0f, -8.0f, 11.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //In order: First box in front of first three, box on top of it, box in front of it (SECOND ONE MOVED TO RIGHT SIDE, THIRD ONE EVEN FURTHER FORWARD)
    engine->CreateDrawable("Meshes/crate_wood.obj", { -10.0f + 60.0f, -8.0f,  8.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 14.0f + 60.0f, -8.0f,  10.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -12.0f + 60.0f, -8.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //RIGHT SIDE
    //In order: Big box, medium box in front of it, box on top of it, box further in front
    engine->CreateDrawable("Meshes/crate_wood.obj", { 7.0f + 60.0f, -7.0f, 16.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 60.0f, -7.5f, 12.0f }, { 1.5f, 1.5f, 1.5f });
    engine->CreateDrawable("Meshes/crate_red.obj", { 9.0f + 60.0f, -5.0f, 12.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 60.0f, -8.0f, 8.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //In order: Big box, box on top of it (MOVED TO THE LEFT SIDE ON LEVEL 2, FOR THE SAKE OF SPICING THINGS UP)
    engine->CreateDrawable("Meshes/crate_wood.obj", { -15.0f + 60.0f, -7.0f, 16.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -15.0f + 60.0f, -4.0f, 16.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //LIGHTS
    engine->CreateLightSpot({ 0.0f + 60.0f, 5.0f, 5.0f }, 0.75f, 0.0f, 0.35f); //fov previously 0.25, rotX previously -0.25, rotY previously 0.125
}

void SetupLevel3(D3D11Engine* engine)
{
    //DRAWABLES IN LEVEL3: 6 + 12
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f + 120.0f, -10.0f,   7.0f }, { 1.8f, 1.0f, 1.8f });
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f + 120.0f,   8.0f,   7.0f }, { 1.8f, 1.0f, 1.8f });

    //Walls (left, then right)
    engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f + 120.0f, -1.0f,  19.0f }, { 10.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 15.0f + 120.0f, -1.0f,  19.0f }, { 10.0f, 8.0f, 1.0f });

    //Door (elongated metal crate with a small metal sphere as a makeshift doorknob)
    engine->CreateDrawable("Meshes/door_metal.obj", { 0.0f + 120.0f, -1.0f, 19.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { -2.75f + 120.0f, -1.25f, 17.75f }, { 0.25f, 0.25f, 0.25f });

    //LEFT SIDE
    //In order: Big box, box on top of it, box in front of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 120.0f, -7.0f, 15.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 120.0f, -4.0f, 15.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f,  0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 120.0f, -8.0f, 11.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //In order: First box in front of first three, box on top of it, box in front of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { -10.0f + 120.0f, -8.0f,  8.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -10.0f + 120.0f, -6.0f,  8.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -10.0f + 120.0f, -8.0f, 4.0f }, { 3.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f }); //Longboy

    //RIGHT SIDE
    //In order: Big box, medium box in front of it, box on top of it, box further in front
    engine->CreateDrawable("Meshes/crate_wood.obj", { 7.0f + 120.0f, -8.0f, 16.0f }, { 1.0f, 1.0f, 1.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 120.0f, -7.5f, 12.0f }, { 1.5f, 1.5f, 1.5f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 120.0f, -5.0f, 12.0f }, { 1.0f, 1.0f, 1.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 120.0f, -8.0f, 8.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //In order: Big box, box on top of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f + 120.0f, -7.0f, 16.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f + 120.0f, -4.0f, 16.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //LIGHTS
    engine->CreateLightSpot({ 0.0f + 120.0f, 5.0f, 5.0f }, 0.75f, 0.0f, 0.35f); //fov previously 0.25, rotX previously -0.25, rotY previously 0.125
}

void SetupLevel4(D3D11Engine* engine)
{
    //DRAWABLES IN LEVEL4: 6 + 21
    //Ground and floor
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f + 180.0f, -10.0f,   7.0f }, { 1.8f, 1.0f, 1.8f });
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f + 180.0f,   8.0f,   7.0f }, { 1.8f, 1.0f, 1.8f });

    //Walls (left, then right)
    engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f + 180.0f, -1.0f,  19.0f }, { 10.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 15.0f + 180.0f, -1.0f,  19.0f }, { 10.0f, 8.0f, 1.0f });

    //Door (elongated metal crate with a small metal sphere as a makeshift doorknob)
    engine->CreateDrawable("Meshes/door_metal.obj", { 0.0f + 180.0f, -1.0f, 19.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { -2.75f + 180.0f, -1.25f, 17.75f }, { 0.25f, 0.25f, 0.25f });

    //LEFT SIDE
    //In order: Big box, box on top of it, box in front of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 180.0f, -7.0f, 15.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_blue.obj", { -8.0f + 180.0f, -4.0f, 15.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f,  0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 180.0f, -8.0f, 11.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //Specific for level 4: more distractors
    engine->CreateDrawable("Meshes/crate_wood.obj", { -5.0f + 180.0f, -8.0f,  15.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -4.0f + 180.0f, -8.0f,  11.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -6.0f + 180.0f, -8.0f,  8.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -14.0f + 180.0f, -7.0f, 15.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -14.0f + 180.0f, -8.0f, 11.0f });


    //In order: First box in front of first three, box on top of it, box in front of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { -10.0f + 180.0f, -8.0f,  8.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -10.0f + 180.0f, -6.0f,  8.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -12.0f + 180.0f, -8.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //RIGHT SIDE
    //In order: Big box, medium box in front of it, box on top of it, box further in front
    engine->CreateDrawable("Meshes/crate_wood.obj", { 7.0f + 180.0f, -7.0f, 16.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 180.0f, -8.0f, 12.0f }, { 1.0f, 1.0f, 1.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f + 180.0f, -6.0f, 12.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 180.0f, -8.0f, 8.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //Specific for level 4: more distractors
    engine->CreateDrawable("Meshes/crate_wood.obj", { 6.0f + 180.0f, -8.0f, 10.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 12.0f + 180.0f, -8.0f, 11.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f + 180.0f, -8.0f, 12.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 12.0f + 180.0f, -8.0f, 8.0f });

    //In order: Big box, box on top of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f + 180.0f, -7.0f, 16.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f + 180.0f, -4.0f, 16.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //LIGHTS
    engine->CreateLightSpot({ 0.0f + 180.0f, 5.0f, 5.0f }, 0.75f, 0.0f, 0.35f); //fov previously 0.25, rotX previously -0.25, rotY previously 0.125
}

void SetupLevel5(D3D11Engine* engine)
{
    //DRAWABLES IN LEVEL5: 6 + 12
    //Ground and floor
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f + 240.0f, -10.0f,   7.0f }, { 1.8f, 1.0f, 1.8f });
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f + 240.0f,   8.0f,   7.0f }, { 1.8f, 1.0f, 1.8f });

    //Walls (left, then right)
    engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f + 240.0f, -1.0f,  19.0f }, { 10.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 15.0f + 240.0f, -1.0f,  19.0f }, { 10.0f, 8.0f, 1.0f });

    //Door (elongated metal crate with a small metal sphere as a makeshift doorknob)
    engine->CreateDrawable("Meshes/door_metal.obj", { 0.0f + 240.0f, -1.0f, 19.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { -2.75f + 240.0f, -1.25f, 17.75f }, { 0.25f, 0.25f, 0.25f });

    //LEFT SIDE
    //In order: Big box, box on top of it, box in front of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 240.0f, -8.0f, 15.0f }, { 1.0f, 1.0f, 1.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 240.0f, -5.5f, 15.0f }, { 1.5f, 1.5f, 1.5f }, { 0.0f,  0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 240.0f, -8.0f, 11.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //In order: First box in front of first three, box on top of it, box in front of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { -10.0f + 240.0f, -8.0f,  8.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -13.0f + 240.0f, -8.0f,  8.0f }); //
    engine->CreateDrawable("Meshes/crate_wood.obj", { -12.0f + 240.0f, -8.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //RIGHT SIDE
    //In order: Big box, medium box in front of it, box on top of it, box further in front
    engine->CreateDrawable("Meshes/crate_wood.obj", { 7.0f + 240.0f, -8.0f, 16.0f }, { 1.0f, 1.0f, 1.0f }); //smaller
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 240.0f, -7.5f, 12.0f }, { 1.5f, 1.5f, 1.5f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 240.0f, -5.0f, 12.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 240.0f, -8.0f, 8.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //In order: Big box, box on top of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f + 240.0f, -8.0f, 14.0f }, { 1.0f, 1.0f, 1.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f + 240.0f, -6.0f, 14.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //LIGHTS
    engine->CreateLightSpot({ 0.0f + 240.0f, 5.0f, 5.0f }, 0.75f, 0.0f, 0.35f); //fov previously 0.25, rotX previously -0.25, rotY previously 0.125
}

void SetupLevel6(D3D11Engine* engine)
{
    //DRAWABLES IN LEVEL5: 6 + 12
    //Ground and floor
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f + 300.0f, -10.0f,   7.0f }, { 1.8f, 1.0f, 1.8f });
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f + 300.0f,   8.0f,   7.0f }, { 1.8f, 1.0f, 1.8f });

    //Walls (left, then right)
    engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f + 300.0f, -1.0f,  19.0f }, { 10.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 15.0f + 300.0f, -1.0f,  19.0f }, { 10.0f, 8.0f, 1.0f });

    //Door (elongated metal crate with a small metal sphere as a makeshift doorknob)
    engine->CreateDrawable("Meshes/door_metal.obj", { 0.0f + 300.0f, -1.0f, 19.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { -2.75f + 300.0f, -1.25f, 17.75f }, { 0.25f, 0.25f, 0.25f });

    //LEFT SIDE
    //In order: Big box, box on top of it, box in front of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 300.0f, -8.0f, 15.0f }, { 1.0f, 1.0f, 1.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -11.0f + 300.0f, -8.0f, 15.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f,  0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 300.0f, -8.0f, 11.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //In order: First box in front of first three, box on top of it, box in front of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { -10.0f + 300.0f, -8.0f,  8.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -14.0f + 300.0f, -7.5f,  8.0f }, {1.5f, 1.5, 1.5f});
    engine->CreateDrawable("Meshes/crate_wood.obj", { -12.0f + 300.0f, -8.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //RIGHT SIDE
    //In order: Big box, medium box in front of it, box on top of it, box further in front
    engine->CreateDrawable("Meshes/sphere_wood.obj", { 7.0f + 300.0f, -7.0f, 16.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 300.0f, -7.5f, 12.0f }, { 1.5f, 1.5f, 1.5f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 300.0f, -5.0f, 12.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 300.0f, -8.0f, 8.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //In order: Big box, box on top of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f + 300.0f, -7.0f, 16.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f + 300.0f, -4.0f, 16.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //LIGHTS
    engine->CreateLightSpot({ 0.0f + 300.0f, 5.0f, 5.0f }, 0.75f, 0.0f, 0.35f); //fov previously 0.25, rotX previously -0.25, rotY previously 0.125
}

void SetupLevel7(D3D11Engine* engine)
{
    //Ground and floor
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f + 360.0f, -10.0f,   7.0f }, { 1.8f, 1.0f, 1.8f });
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f + 360.0f,   8.0f,   7.0f }, { 1.8f, 1.0f, 1.8f });

    //Walls (left, then right)
    engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f + 360.0f, -1.0f,  19.0f }, { 10.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 15.0f + 360.0f, -1.0f,  19.0f }, { 10.0f, 8.0f, 1.0f });

    //Door (elongated metal crate with a small metal sphere as a makeshift doorknob)
    engine->CreateDrawable("Meshes/door_metal.obj", { 0.0f + 360.0f, -1.0f, 19.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { -2.75f + 360.0f, -1.25f, 17.75f }, { 0.25f, 0.25f, 0.25f });

    //LEFT SIDE
    //In order: Big box, box on top of it, box in front of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 360.0f, -7.0f, 15.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 360.0f, -4.0f, 15.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f,  0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 360.0f, -8.0f, 11.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //In order: First box in front of first three, box on top of it, box in front of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { -10.0f + 360.0f, -8.0f,  8.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -10.0f + 360.0f, -6.0f,  8.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -12.0f + 360.0f, -8.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //RIGHT SIDE
    //In order: Big box, medium box in front of it, box on top of it, box further in front
    engine->CreateDrawable("Meshes/crate_wood.obj", { 7.0f + 360.0f, -7.5f, 16.0f }, { 1.5f, 1.5f, 1.5f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 360.0f, -7.0f, 12.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 360.0f, -4.0f, 12.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 360.0f, -8.0f, 8.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //In order: Big box, box on top of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { 16.0f + 360.0f, -8.0f, 16.0f }, { 1.0f, 1.0f, 1.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 16.0f + 360.0f, -6.0f, 16.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //New:
    engine->CreateDrawable("Meshes/crate_wood.obj", { -13.0f + 360.0f, -8.0f, 8.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -14.0f + 360.0f, -8.0f, 11.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 14.0f + 360.0f, -8.0f, 11.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f + 360.0f, -8.0f, 8.0f });

    //LIGHTS
    engine->CreateLightSpot({ 0.0f + 360.0f, 5.0f, 5.0f }, 0.75f, 0.0f, 0.35f); //fov previously 0.25, rotX previously -0.25, rotY previously 0.125
}

void SetupLevel8(D3D11Engine* engine)
{
    //Ground and floor
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f + 420.0f, -10.0f,   7.0f }, { 1.8f, 1.0f, 1.8f });
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f + 420.0f,   8.0f,   7.0f }, { 1.8f, 1.0f, 1.8f });

    //Walls (left, then right)
    engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f + 420.0f, -1.0f,  19.0f }, { 10.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 15.0f + 420.0f, -1.0f,  19.0f }, { 10.0f, 8.0f, 1.0f });

    //Door (elongated metal crate with a small metal sphere as a makeshift doorknob)
    engine->CreateDrawable("Meshes/door_metal.obj", { 0.0f + 420.0f, -1.0f, 19.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { -2.75f + 420.0f, -1.25f, 17.75f }, { 0.25f, 0.25f, 0.25f });

    //LEFT SIDE
    //In order: Big box, box on top of it, box in front of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 420.0f, -7.0f, 15.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 420.0f, -4.0f, 15.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f,  0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 420.0f, -8.0f, 11.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //new bigass create left side
    engine->CreateDrawable("Meshes/crate_wood.obj", { -15.0f + 420.0f, -6.0f, 12.0f }, { 3.0f, 3.0f, 3.0f });

    //In order: First box in front of first three, box on top of it, box in front of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { 12.0f + 420.0f, -8.0f,  11.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 6.0f + 420.0f, -8.0f,  11.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -12.0f + 420.0f, -8.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //RIGHT SIDE
    //In order: Big box, medium box in front of it, box on top of it, box further in front
    engine->CreateDrawable("Meshes/crate_wood.obj", { 7.0f + 420.0f, -7.0f, 16.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_glow_blue.obj", { 9.0f + 420.0f, -7.5f, 12.0f }, { 1.5f, 1.5f, 1.5f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 420.0f, -5.0f, 12.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 420.0f, -8.0f, 8.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //In order: Big box, box on top of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { 13.0f + 420.0f, -7.0f, 16.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 13.0f + 420.0f, -4.0f, 16.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //LIGHTS
    engine->CreateLightSpot({ 0.0f + 420.0f, 5.0f, 5.0f }, 0.75f, 0.0f, 0.35f); //fov previously 0.25, rotX previously -0.25, rotY previously 0.125
}

void SetupLevel9(D3D11Engine* engine)
{
    //Ground and floor
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f + 480.0f, -10.0f,   7.0f }, { 1.8f, 1.0f, 1.8f });
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f + 480.0f,   8.0f,   7.0f }, { 1.8f, 1.0f, 1.8f });

    //Walls (left, then right)
    engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f + 480.0f, -1.0f,  19.0f }, { 10.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 15.0f + 480.0f, -1.0f,  19.0f }, { 10.0f, 8.0f, 1.0f });

    //Door (elongated metal crate with a small metal sphere as a makeshift doorknob)
    engine->CreateDrawable("Meshes/door_metal.obj", { 0.0f + 480.0f, -1.0f, 19.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { -2.75f + 480.0f, -1.25f, 17.75f }, { 0.25f, 0.25f, 0.25f });

    //LEFT SIDE
    //In order: Big box, box on top of it, box in front of it
    engine->CreateDrawable("Meshes/crate_wood_skewed.obj", { -8.0f + 480.0f, -7.0f, 15.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 480.0f, -4.0f, 15.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f,  0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f + 480.0f, -8.0f, 11.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //In order: First box in front of first three, box on top of it, box in front of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { -10.0f + 480.0f, -8.0f,  8.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -10.0f + 480.0f, -6.0f,  8.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -12.0f + 480.0f, -8.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //RIGHT SIDE
    //In order: Big box, medium box in front of it, box on top of it, box further in front
    engine->CreateDrawable("Meshes/crate_wood.obj", { 7.0f + 480.0f, -7.0f, 16.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 480.0f, -8.0f, 12.0f }, { 1.0f, 1.0f, 1.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 13.0f + 480.0f, -8.0f, 12.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 9.0f + 480.0f, -8.0f, 8.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //In order: Big box, box on top of it
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f + 480.0f, -7.0f, 16.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f + 480.0f, -3.5f, 16.0f }, { 1.5f, 1.5f, 1.5f }, { 0.0f, 0.45f, 0.0f });

    //New:
    engine->CreateDrawable("Meshes/crate_wood.obj", { -13.0f + 480.0f, -8.0f, 8.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -14.0f + 480.0f, -8.0f, 11.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 15.0f + 480.0f, -7.5f, 8.0f }, {1.5f, 1.5f, 1.5f});

    //LIGHTS
    engine->CreateLightSpot({ 0.0f + 480.0f, 5.0f, 5.0f }, 0.75f, 0.0f, 0.35f); //fov previously 0.25, rotX previously -0.25, rotY previously 0.125
}