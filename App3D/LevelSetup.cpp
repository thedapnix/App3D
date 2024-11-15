#include "LevelSetup.h"

using namespace DirectX;

void SetupLevel1(D3D11Engine* engine)
{
    //FIRST ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f,  0.0f, -5.0f }, { 2.0f, 1.0f, 2.0f });
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f, 18.0f, -5.0f }, { 2.0f, 1.0f, 2.0f });

    //FRONT WALL
    //Door (elongated metal crate with a small metal sphere as a makeshift doorknob, sue me) and walls surrounding it (left, then right)
    engine->CreateDrawable("Meshes/door_metal.obj", { 0.0f, 9.0f, 15.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { -2.75f, 9.25f, 13.75f }, { 0.25f, 0.25f, 0.25f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -12.5f, 9.0f, 15.0f }, { 7.5f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 12.5f,  9.0f, 15.0f }, { 7.5f, 8.0f, 1.0f });

    //LEFT AND RIGHT WALLS
    engine->CreateDrawable("Meshes/wall_brick.obj", { -20.0f, 9.0f, -5.0f }, { 20.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 20.0f, 9.0f, -5.0f }, { 20.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //BACK WALL
    engine->CreateDrawable("Meshes/wall_brick.obj", { 0.0f, 9.0f, -25.0f }, { 20.0f, 8.0f, 1.0f });

    //Light(s)
    //Ceiling in the center of the room pointing straight down onto the floor (Lamp mesh, point light lighting up the mesh, and spotlight pointing down to the ground)
    engine->CreateConcaveDrawable("Meshes/lamp.obj", { 0.0f, 16.75f, -5.0f });
    engine->CreateLightPoint({ 0.0f, 17.75f, -5.0f }, 3.0f, { 1.0f, 1.0f, 0.5f });  //Doesn't light if it's *below* the lamp and lights upwards, then it just hits the roof... 
                                                                                    //This is a workaround but not good. Thoughts:
                                                                                    //Hollowed out cone so we don't have downwards/inwards facing normals
                                                                                    //Thus, we don't connect when we do nDotL in the shader (L = distance vec from light to pixel)
    engine->CreateLightSpot({ 0.0f, 16.5f, -5.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });

    /*
    Room    #1 Completed
        Drawables   -> 10
        Lights      -> 1
    */
    //HALLWAY AFTER FIRST ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f,  0.0f, 25.0f });
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f, 18.0f, 25.0f });

    //Walls (left, then right)
    engine->CreateDrawable("Meshes/wall_brick.obj", { -10.0f, 9.0f, 26.0f }, { 10.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 10.0f, 9.0f, 26.0f }, { 10.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    /*
    Hallway #1 Completed
        Drawables   -> 4    (14)
        Lights      -> 0    (1)
    */

    //SECOND ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f, 0.0f, 55.0f }, { 2.0f, 1.0f, 2.0f });     //From  {-20, -1, +35} to {+20, +1, +75}
    engine->CreateDrawable("Meshes/ground_stone.obj", { -40.0f, 0.0f, 55.0f }, { 2.0f, 1.0f, 2.0f });   //From  {-60, -1, +35} to {-20, +1, +75}
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f, 18.0f, 55.0f }, { 2.0f, 1.0f, 2.0f });
    engine->CreateDrawable("Meshes/ground_stone.obj", { -40.0f, 18.0f, 55.0f }, { 2.0f, 1.0f, 2.0f });

    //Walls (counter-clockwise as you exit the hallway)
    engine->CreateDrawable("Meshes/wall_brick.obj", { 15.0f, 9.0f, 36.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 20.0f, 9.0f, 56.0f }, { 20.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 0.0f, 9.0f, 76.0f }, { 20.0f, 8.0f, 1.0f });     //From  {-20, +1, +75} to {+20, +17 +77}
    engine->CreateDrawable("Meshes/wall_brick.obj", { -50.0f, 9.0f, 76.0f }, { 10.0f, 8.0f, 1.0f });

    //Door and walls (left, then right)
    engine->CreateDrawable("Meshes/door_metal.obj", { -60.0f, 9.0f, 55.0f }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { -58.75f, 9.25f, 52.25f }, { 0.25f, 0.25f, 0.25f }, { 0.0f, -XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -60.0f, 9.0f, 42.5f }, { 7.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -60.0f,  9.0f, 67.5f }, { 7.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Final wall connecting back to the hallway we came from
    engine->CreateDrawable("Meshes/wall_brick.obj", { -35.0f, 9.0f, 36.0f }, { 25.0f, 8.0f, 1.0f });

    //Light(s)
    engine->CreateConcaveDrawable("Meshes/lamp.obj", { 0.0f, 16.75f, 55.0f });
    engine->CreateLightPoint({ 0.0f, 17.75f, 55.0f }, 3.0f, { 1.0f, 1.0f, 0.5f });
    engine->CreateLightSpot({ 0.0f, 16.5f, 55.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
    engine->CreateConcaveDrawable("Meshes/lamp.obj", { -35.0f, 16.75f, 55.0f });
    engine->CreateLightPoint({ -35.0f, 17.75f, 55.0f }, 3.0f, { 1.0f, 1.0f, 0.5f });
    engine->CreateLightSpot({ -35.0f, 16.5f, 55.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });

    /*
    Room    #2 Completed
        Drawables   -> 15   (29)
        Lights      -> 2    (3)
    */

    //HALLWAY AFTER SECOND ROOM (TAKING A "RIGHT" INSTEAD OF CONTINUING FORWARD THROUGH THE ROOM)
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", { -30.0f,  0.0f, 85.0f });
    engine->CreateDrawable("Meshes/ground_stone.obj", { -30.0f, 18.0f, 85.0f });

    //Walls (left, then right)
    engine->CreateDrawable("Meshes/wall_brick.obj", { -40.0f, 9.0f, 86.0f }, { 10.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -20.0f, 9.0f, 86.0f }, { 10.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    /*
    Hallway #2 Completed
        Drawables   -> 4    (33)
        Lights      -> 0    (3)
    */

    //THIRD ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", { -40.0f, 0.0f, 115.0f }, { 2.0f, 1.0f, 2.0f });   //From  {-60, -1, +95} to {-20, +1, +135}
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f, 0.0f, 115.0f }, { 2.0f, 1.0f, 2.0f });   //From  {-20, -1, +95} to {+20, +1, +135}
    engine->CreateDrawable("Meshes/ground_stone.obj", { 40.0f, 0.0f, 115.0f }, { 2.0f, 1.0f, 2.0f });   //From  {+20, -1, +95} to {+60, +1, +135}
    engine->CreateDrawable("Meshes/ground_stone.obj", { 45.0f, 0.0f, 80.0f }, { 1.5f, 1.0f, 1.5f });   //From  {+30, -1, +65} to {+60, +1, +95}

    engine->CreateDrawable("Meshes/ground_stone.obj", { -40.0f, 18.0f, 115.0f }, { 2.0f, 1.0f, 2.0f });   //From  {-60, -1, +95} to {-20, +1, +135}
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f, 18.0f, 115.0f }, { 2.0f, 1.0f, 2.0f });   //From  {-20, -1, +95} to {+20, +1, +135}
    engine->CreateDrawable("Meshes/ground_stone.obj", { 40.0f, 18.0f, 115.0f }, { 2.0f, 1.0f, 2.0f });   //From  {+20, -1, +95} to {+60, +1, +135}
    engine->CreateDrawable("Meshes/ground_stone.obj", { 45.0f, 18.0f, 80.0f }, { 1.5f, 1.0f, 1.5f });   //From  {+30, -1, +65} to {+60, +1, +95}

    //Door and walls going counter-clockwise
    engine->CreateDrawable("Meshes/door_metal.obj", { 15.0f, 9.0f, 135.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { 12.25f, 9.25f, 133.75f }, { 0.25f, 0.25f, 0.25f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -10.0f,  9.0f, 135.0f }, { 20.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -45.0f, 9.0f, 135.0f }, { 15.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -60.0f, 9.0f, 115.0f }, { 20.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -50.0f, 9.0f, 96.0f }, { 10.0f, 8.0f, 1.0f }); //Now we're back at the hallway

    //Keep going counter-clockwise starting from the hallway entrance
    engine->CreateDrawable("Meshes/wall_brick.obj", { 5.0f, 9.0f, 96.0f }, { 25.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 30.0f, 9.0f, 81.0f }, { 15.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 45.0f, 9.0f, 65.0f }, { 15.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 60.0f, 9.0f, 82.5f }, { 17.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 60.0f, 9.0f, 117.5f }, { 17.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 40.0f, 9.0f, 135.0f }, { 20.0f, 8.0f, 1.0f }); //Now we're back to the door and the room is finished

    //Light(s)
    engine->CreateConcaveDrawable("Meshes/lamp.obj", { 5.0f, 16.75f, 115.0f });
    engine->CreateLightPoint({ 5.0f, 17.75f, 115.0f }, 3.0f, { 1.0f, 1.0f, 0.5f });
    engine->CreateLightSpot({ 5.0f, 16.5f, 115.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
    engine->CreateConcaveDrawable("Meshes/lamp.obj", { -30.0f, 16.75f, 115.0f });
    engine->CreateLightPoint({ -30.0f, 17.75f, 115.0f }, 3.0f, { 1.0f, 1.0f, 0.5f });
    engine->CreateLightSpot({ -30.0f, 16.5f, 115.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
    engine->CreateConcaveDrawable("Meshes/lamp.obj", { 45.0f, 16.75f, 80.0f });
    engine->CreateLightPoint({ 45.0f, 17.75f, 80.0f }, 3.0f, { 1.0f, 1.0f, 0.5f });
    engine->CreateLightSpot({ 45.0f, 16.5f, 80.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });

    /*
    Room    #3 Completed
        Drawables   -> 23   (56)
        Lights      -> 3    (6)
    */

    //HALLWAY TO "FOURTH" ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", { 15.0f, 0.0f, 145.0f }, { 1.0f, 1.0f, 1.0f });   //From  {+5, -1, +135} to {+25, +1, +155}
    engine->CreateDrawable("Meshes/ground_stone.obj", { 5.0f, 0.0f, 165.0f }, { 2.0f, 1.0f, 1.0f });   //From  {-15, -1, +155} to {+25, +1, +175}
    engine->CreateDrawable("Meshes/ground_stone.obj", { 15.0f, 18.0f, 145.0f }, { 1.0f, 1.0f, 1.0f });
    engine->CreateDrawable("Meshes/ground_stone.obj", { 5.0f, 18.0f, 165.0f }, { 2.0f, 1.0f, 1.0f });

    //Walls (Starting from the right side)
    engine->CreateDrawable("Meshes/wall_brick.obj", { 25.0f, 9.0f, 155.0f }, { 20.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 5.0f, 9.0f, 175.0f }, { 20.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -5.0f, 9.0f, 155.0f }, { 10.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { 5.0f, 9.0f, 145.0f }, { 10.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    /*
    Hallway #4 Completed
        Drawables   -> 8    (64)
        Lights      -> 0    (6)
    */

    //"FOURTH" ROOM (Leads to the end)
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", { -40.0f, 0.0f, 165.0f }, { 2.5f, 1.0f, 2.5f });   //From  {-65, -1, +140} to {-15, +1, +190}
    engine->CreateDrawable("Meshes/ground_stone.obj", { -40.0f, 18.0f, 165.0f }, { 2.5f, 1.0f, 2.5f });

    //Walls and Doors (Starting from the door on the left, going clockwise)
    engine->CreateDrawable("Meshes/door_metal.obj", { -65.0f, 9.0f, 155.0f }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { -63.75f, 9.25f, 152.25f }, { 0.25f, 0.25f, 0.25f }, { 0.0f, -XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -65.0f, 9.0f, 165.0f }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/door_metal.obj", { -65.0f, 9.0f, 175.0f }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { -63.75f, 9.25f, 172.25f }, { 0.25f, 0.25f, 0.25f }, { 0.0f, -XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -65.0f, 9.0f, 185.0f }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -40.0f, 9.0f, 190.0f }, { 25.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f, 9.0f, 182.5f }, { 7.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f, 9.0f, 147.5f }, { 7.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -40.0f, 9.0f, 140.0f }, { 25.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -65.0f, 9.0f, 145.0f }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Light(s)
    engine->CreateConcaveDrawable("Meshes/lamp.obj", { -40.0f, 16.75f, 165.0f });
    engine->CreateLightPoint({ -40.0f, 17.75f, 165.0f }, 3.0f, { 1.0f, 1.0f, 0.5f });
    engine->CreateLightSpot({ -40.0f, 16.5f, 165.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });

    /*
    Room    #4 Completed
        Drawables   -> 14   (78)
        Lights      -> 1    (7)
    */

    //OTHER HALLWAY TO OTHER "FOURTH" ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", { -67.5f, 0.0f, 55.0f }, { 0.75f, 1.0f, 1.0f });//From {-80, -1, +45} to {-60, +1, +65}
    engine->CreateDrawable("Meshes/ground_stone.obj", { -90.0f, 0.0f, 75.0f }, { 1.5f, 1.0f, 3.0f }); //From {-105, -1, +45} to {-75, +1, +105}
    engine->CreateDrawable("Meshes/ground_stone.obj", { -67.5f, 18.0f, 55.0f }, { 0.75f, 1.0f, 1.0f });
    engine->CreateDrawable("Meshes/ground_stone.obj", { -90.0f, 18.0f, 75.0f }, { 1.5f, 1.0f, 3.0f });

    //Walls and door (Starting from the door, going clockwise)
    engine->CreateDrawable("Meshes/door_metal.obj", { -90.0f, 9.0f, 105.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { -87.25f, 9.25f, 103.75f }, { 0.25f, 0.25f, 0.25f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -80.0f, 9.0f, 105.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -75.0f, 9.0f, 85.0f }, { 20.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -67.5f, 9.0f, 65.0f }, { 7.5f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -82.5f, 9.0f, 45.0f }, { 22.5f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -105.0f, 9.0f, 75.0f }, { 30.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -100.0f, 9.0f, 105.0f }, { 5.0f, 8.0f, 1.0f });

    //Light(s)
    engine->CreateConcaveDrawable("Meshes/lamp.obj", { -90.0f, 16.75f, 82.5f });
    engine->CreateLightPoint({ -90.0f, 17.75f, 82.5f }, 3.0f, { 1.0f, 1.0f, 0.5f });
    engine->CreateLightSpot({ -90.0f, 16.5f, 82.5f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });

    /*
    Hallway #5 Completed
        Drawables   -> 13   (91)
        Lights      -> 1  (8)
    */

    //Ending (Connect the two alternating paths, one is a hallway ending at -90, 0, 105, the other a room with an edge at -65, 0, 205)
    //engine->CreateDrawable("Meshes/ground_stone.obj", { -90.0f, 0.0f, 155.0f }, { 2.5f, 1.0f, 5.0f });  //From {-115, -1, +105} to {-65, +1, +205}

    /*
    Total
        Drawables   -> 91
        Lights      -> 8
    */

    /*CLUTTER*/
    //FIRST ROOM
    //Clutter on the left side of the door
    engine->CreateDrawable("Meshes/crate_wood.obj", { -9.5f, 3.0f, 10.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_blue.obj", { -9.5f, 6.0f, 10.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f,  0.45f, 0.0f }, 1, { 2, 3 }); //interactID 1 = Destroy something, interacts with {1, 2}, aka the door
    engine->CreateDrawable("Meshes/crate_wood.obj", { -8.0f, 2.0f,  6.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -5.0f, 2.0f, 10.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -4.0f, 2.0f,  6.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -6.0f, 2.0f,  3.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -15.5f, 3.0f, 10.0f }, { 2.0f, 2.0f, 2.0f });
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

    /*
    Room    #1 Completed Clutter
        Drawables   -> 21   (112)
    */

    //SECOND ROOM
    //Clutter on the right side of the door
    engine->CreateDrawable("Meshes/crate_wood.obj", { -55.0f, 3.0f, 71.5f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -53.0f, 2.5f, 67.5f }, { 1.5f, 1.5f, 1.5f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -53.0f, 5.0f, 67.5f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -56.5f, 2.0f, 66.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -49.0f, 3.0f, 71.5f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -49.0f, 6.0f, 71.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //Attempt: Clutter collections made in blender to save on manually placing all of these with code (Works well enough, things aren't placed or rotated properly though because blender is kinda dogshit when it comes to its axes)
    //Room 2
    engine->CreateDrawable("Meshes/clutter_small_1.obj", { -30.0f, 2.0f, 42.5f }, {1.0f, 1.0f, 1.0f}, { 0.0f, XM_PIDIV2, 0.0f }); //Facing +Z
    engine->CreateDrawable("Meshes/clutter_small_2.obj", { -5.0f, 2.0f, 67.5f }, { 1.0f, 1.0f, 1.0f }, {0.0f, -XM_PIDIV2, 0.0f}); //Facing -Z
    engine->CreateDrawable("Meshes/clutter_small_3.obj", { 13.0f, 2.0f, 66.0f }, { 1.0f, 1.0f, 1.0f }, {0.0f, -XM_PIDIV2, 0.0f}); //Facing -X

    //Clutter room #2 done:
       //Drawables  -> 9    (121)

    //Room 3
    engine->CreateDrawable("Meshes/clutter_medium_1.obj", { -45.0f, 2.0f, 130.0f }, { 1.0f, 1.0f, 1.0f }, {0.0f, XM_PI, 0.0f}); //Facing -Z
    engine->CreateDrawable("Meshes/clutter_medium_2.obj", {  0.0f, 2.0f, 100.0f }, { 1.0f, 1.0f, 1.0f }); //Facing +Z
    engine->CreateDrawable("Meshes/clutter_medium_3.obj", { 46.0f, 2.0f, 77.0f }, { 1.0f, 1.0f, 1.0f }, {0.0f, XM_PI, 0.0f}); //Default: Facing -Z. Flip it to face the other way and put it in the corner under the lighting
    engine->CreateDrawable("Meshes/clutter_small_1.obj", { 5.0f, 2.0f, 128.0f }, { 1.0f, 1.0f, 1.0f }, {0.0f, -XM_PIDIV2, 0.0f}); //Default facing -X? Want -Z so negative pidiv2 i think?

    //Drawables     -> 4    (125)

    //Two target boxes in room 3, both of these will have the destroy-interaction and get rid of the other box, as well as both the door in this room and the door in the previous room
    //Door in second room: 23 and 24 (-1 because of indexing stuff)
    //Door in this room: 42 and 43 (--,,--)
    //These two boxes are 126 and 127 (Apparently they are actually 126 and 127 so there's been a miscalc somewhere, oopsie? (Oh yeah, the two new lights I added also have drawable))
    engine->CreateDrawable("Meshes/crate_red.obj", { -1.75f, 6.0f, 130.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f,  0.0f, 0.0f }, 1, { 22, 23, 41, 42, 126 });
    engine->CreateDrawable("Meshes/crate_blue.obj", { 46.2f, 5.0f, 78.6f }, { 1.0f, 1.0f, 1.0f }, { 0.0f,  0.0f, 0.0f }, 1, { 22, 23, 41, 42, 125 });

    //Hallway 5-ish
    engine->CreateDrawable("Meshes/clutter_small_2.obj", { -97.0f, 2.0f, 100.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PI, 0.0f }); //Facing +X
    engine->CreateDrawable("Meshes/crate_wood.obj", { -80.0f, 3.0f, 100.0f }, {2.0f, 2.0f, 2.0f});

}

void SetupLevel2(D3D11Engine* engine)
{
    //Temp code for testing
    int w1, w2, w3, w11, w12, w13, w14, w15; //Walls
    int d1, d2; //Doors

    for (int i = 0; i < 6; i++) //6 hallways, 3 color, 3 motion (as a result of the preliminary experiment we skip the form attribute)
    {
        //Reset w and d every corridor
        w1 = w2 = w3 = w11 = w12 = w13 = w14 = w15 = 0;
        d1 = d2 = 0;

        //Floor and roof
        int g1 = engine->CreateDrawable("Meshes/ground_stone.obj",   {  0.0f + (60.0f * i),  0.0f, 25.0f + (60.0f * i) }, { 1.5f, 1.0f, 3.0f });
        int g2 = engine->CreateDrawable("Meshes/ground_stone.obj",   {  0.0f + (60.0f * i), 18.0f, 25.0f + (60.0f * i) }, { 1.5f, 1.0f, 3.0f });
        int g3 = engine->CreateDrawable("Meshes/ground_stone.obj",   { 15.0f + (60.0f * i),  0.0f, 70.0f + (60.0f * i) }, { 1.5f, 1.0f, 3.0f }, { 0.0f, XM_PIDIV2, 0.0f });
        int g4 = engine->CreateDrawable("Meshes/ground_stone.obj",   { 15.0f + (60.0f * i), 18.0f, 70.0f + (60.0f * i) }, { 1.5f, 1.0f, 3.0f }, { 0.0f, XM_PIDIV2, 0.0f });

        //Walls (Starting with back wall, then going along the side with the door until we've looped back around)
        int o1 = engine->CreateDrawable("Meshes/wall_brick.obj", { 0.0f + (60.0f * i),  9.0f, -5.0f + (60.0f * i) }, { 15.0f, 8.0f, 1.0f });
        if (i == 0) //First hallway there won't be a door behind you, you're on your own
        {
            w1 = engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f, 9.0f, 17.5f }, { 22.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
            w2 = engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f, 9.0f, 62.5f }, { 22.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
        }
        else
        {
            d1 = engine->CreateDrawable("Meshes/door_metal.obj", { -15.0f + (60.0f * i), 9.0f, 10.0f + (60.0f * i) }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
            d2 = engine->CreateDrawable("Meshes/sphere_metal.obj", { -16.25f + (60.0f * i), 9.25f, 12.75f + (60.0f * i) }, { 0.25f, 0.25f, 0.25f }, { 0.0f, XM_PIDIV2, 0.0f });
            w1 = engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f + (60.0f * i), 9.0f, 0.0f + (60.0f * i) }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
            w2 = engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f + (60.0f * i), 9.0f, 67.5f + (60.0f * i)}, {17.5f, 8.0f, 1.0f}, {0.0f, XM_PIDIV2, 0.0f});
            w3 = engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f + (60.0f * i), 9.0f, 32.5f + (60.0f * i) }, { 17.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
        }
        w11 = engine->CreateDrawable("Meshes/wall_brick.obj", { 0.0f + (60.0f * i) , 9.0f, 85.0f + (60.0f * i) }, { 15.0f, 8.0f, 1.0f });
        w12 = engine->CreateDrawable("Meshes/wall_brick.obj", { 30.0f + (60.0f * i), 9.0f, 85.0f + (60.0f * i) }, { 15.0f, 8.0f, 1.0f });
        w13 = engine->CreateDrawable("Meshes/wall_brick.obj", { 30.0f + (60.0f * i), 9.0f, 55.0f + (60.0f * i) }, { 15.0f, 8.0f, 1.0f });
        w14 = engine->CreateDrawable("Meshes/wall_brick.obj", { 15.0f + (60.0f * i), 9.0f, 40.0f + (60.0f * i) }, { 15.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
        w15 = engine->CreateDrawable("Meshes/wall_brick.obj", { 15.0f + (60.0f * i), 9.0f, 10.0f + (60.0f * i) }, { 15.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

        //Add normal maps to all the brick walls hoo boy
        engine->ApplyNormalMapToDrawable(o1, "NormalMaps/brick.jpg");
        engine->ApplyNormalMapToDrawable(w1, "NormalMaps/brick.jpg");
        engine->ApplyNormalMapToDrawable(w2, "NormalMaps/brick.jpg");
        if (w3 != 0) engine->ApplyNormalMapToDrawable(w3, "NormalMaps/brick.jpg");
        engine->ApplyNormalMapToDrawable(w11, "NormalMaps/brick.jpg");
        engine->ApplyNormalMapToDrawable(w12, "NormalMaps/brick.jpg");
        engine->ApplyNormalMapToDrawable(w13, "NormalMaps/brick.jpg");
        engine->ApplyNormalMapToDrawable(w14, "NormalMaps/brick.jpg");
        engine->ApplyNormalMapToDrawable(w15, "NormalMaps/brick.jpg");

        //And the doors
        if (d1 != 0) engine->ApplyNormalMapToDrawable(d1, "NormalMaps/metal_long.jpg");
        if (d2 != 0) engine->ApplyNormalMapToDrawable(d2, "NormalMaps/metal.jpg");

        //Normal maps to the floors and roofs (No this looks fucking AWFUL lmao, since the stone texture i made is just a grey image with a noise filter applied to it, and that messes with the normals)
        //engine->ApplyNormalMapToDrawable(g1, "NormalMaps/stone.jpg");
        //engine->ApplyNormalMapToDrawable(g2, "NormalMaps/stone.jpg");
        //engine->ApplyNormalMapToDrawable(g3, "NormalMaps/stone.jpg");
        //engine->ApplyNormalMapToDrawable(g4, "NormalMaps/stone.jpg");

        //Lights
        engine->CreateConcaveDrawable("Meshes/lamp.obj", { 0.0f + (60.0f * i), 16.75f, 25.0f + (60.0f * i) });
        engine->CreateLightPoint({ 0.0f + (60.0f * i), 17.75f, 25.0f + (60.0f * i) }, 3.0f, { 1.0f, 1.0f, 0.5f });
        engine->CreateLightSpot({ 0.0f + (60.0f * i), 16.5f, 25.0f + (60.0f * i) }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
        engine->CreateConcaveDrawable("Meshes/lamp.obj", { 17.5f + (60.0f * i), 16.75f, 70.0f + (60.0f * i) });
        engine->CreateLightPoint({ 17.5f + (60.0f * i), 17.75f, 70.0f + (60.0f * i) }, 3.0f, { 1.0f, 1.0f, 0.5f });
        engine->CreateLightSpot({ 17.5f + (60.0f * i), 16.5f, 70.0f + (60.0f * i) }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
    }

    //First hallway, 13 drawables make up the basis
    //Every hallway after that, 17 drawables make up the basis, of which #6 and #7 regard the door

    //Total drawables after basis levels:
    //13 + (17 * 5) = 98
    //Doors:
    /*
        (19,20)
        (36,37)
        (53,54)
        (70,71)
        (87,88)
    */

    //Final door at the end (99,100) (Don't destroy it though, unless you want to jump into the endless abyss)
    engine->CreateDrawable("Meshes/door_metal.obj", { -15.0f + (360.0f), 9.0f, 10.0f + (360.0f) }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { -16.25f + (360.0f), 9.25f, 12.75f + (360.0f) }, { 0.25f, 0.25f, 0.25f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f + (360.0f), 9.0f, 0.0f + (360.0f) }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f + (360.0f), 9.0f, 32.5f + (360.0f) }, { 17.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Targets start here, after 102 drawables
 
    //Red Box in the first hallway          (103)
    engine->CreateDrawable("Meshes/crate_red.obj", { 4.5f, 6.0f, 74.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1, { 19, 20 });

    //Bouncing Box in the second hallway    (104)
    engine->CreateDrawable("Meshes/crate_wood.obj", { 19.0f + 60.0f, 5.0f, 73.5f + 60.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1, { 36, 37 });     //Bounce from 5 to 10 and back

    //Blue Box in the third hallway         (105)
    engine->CreateDrawable("Meshes/crate_blue.obj", { 25.0f + 120.0f, 6.0f, 60.0f + 120.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1, { 53, 54 });

    //Sliding Box in the fourth hallway     (106)
    engine->CreateDrawable("Meshes/crate_wood.obj", { 11.0f + 180.0f, 6.0f, 22.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1, { 70, 71 });   //Slide from 22 to 30 and back (202 to 210 I think)

    //Glowing Box in the fifth hallway      (107)
    engine->CreateDrawable("Meshes/crate_glow_blue.obj", { -2.95f + 240.0f, 4.0f, 61.9f + 240.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1, { 87, 88 });

    //Swinging Box in the sixth hallway     (108)
    //engine->CreateDrawable("Meshes/crate_wood.obj", { 30.0f + 300.0f, 12.0f, 70.0f + 300.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1, { 99, 100 });   //This one is way more complicated. Swing from a rope or something?
    //engine->CreatePovDrawable("Meshes/crate_wood.obj", { 0.0f, -6.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1, { 99, 100 });
    engine->CreateOrbitDrawable("Meshes/crate_wood.obj", { 25.0f + 300.0f, 12.0f, 70.0f + 300.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1, { 99, 100 });

    //Then add clutter, can't do that in the loop since they're supposed to differ between levels (have fun (god I hate this part I'm a PROGRAMMER NOT A LEVEL DESIGNER GRRRAHHHH))
    //At the very least, we can place clutter in relation to the first hallway, and then simply do +60, +120, +180, and +240, to put them in hallways number 2, 3, 4, and 5

    /*////////////
    //////////////
    //HALLWAY #1//
    //////////////
    */////////////
    //Back-wall clutter
    engine->CreateDrawable("Meshes/clutter_small_2.obj", { 3.0f, 2.0f, 2.5f }, { 1.0f, 1.0f, 1.0f }, {0.0f, XM_PIDIV2, 0.0f});

    //Hallway (part 1, left side) clutter
    engine->CreateDrawable("Meshes/clutter_small_1.obj", { -8.0f, 2.0f, 40.0f }, {1.0f, 1.0f, 1.0f}, {0.0f, -XM_PI, 0.0f});

    //Hallway (part 1, right side)
    //Nothing in this hallway

    //Hallway (part 2, far side) clutter
    engine->CreateDrawable("Meshes/clutter_medium_3.obj", { -2.5f, 2.0f, 74.0f });

    //Hallway (part 2, near side) clutter
    engine->CreateDrawable("Meshes/clutter_small_1.obj", { 25.0f, 2.0f, 62.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative X by default, rotated to face positive Z
    
    /*////////////
    //////////////
    //HALLWAY #2//
    //////////////
    */////////////
    //Back-wall clutter
    engine->CreateDrawable("Meshes/clutter_small_1.obj", { 3.0f + 60.0f, 2.0f, 2.5f + 60.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Hallway (part 1, left side) clutter
    //engine->CreateDrawable("Meshes/clutter_small_2.obj", { -8.0f + 60.0f, 2.0f, 40.0f + 60.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -XM_PI, 0.0f });

    //Hallway (part 1, right side)
    engine->CreateDrawable("Meshes/clutter_medium_2.obj", { 4.5f + 60.0f, 2.0f, 40.0f + 60.0f }, {1.0f, 1.0f, 1.0f}, {0.0f, XM_PIDIV2, 0.0f}); //Faces negative Z by default, rotated to face negative X

    //Hallway (part 2, far side) clutter
    engine->CreateDrawable("Meshes/clutter_small_3.obj", { -8.5f + 60.0f, 2.0f, 79.0f + 60.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/clutter_medium_1.obj", { 25.0f + 60.0f, 2.0f, 80.0f + 60.0f }, {1.0f, 1.0f, 1.0f}, {0.0f, XM_PI, 0.0f}); //Faces positive Z by default, rotated to face negative Z

    //Hallway (part 2, near side) clutter
    //engine->CreateDrawable("Meshes/clutter_small_3.obj", { 25.0f + 60.0f, 2.0f, 65.0f + 60.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative X by default, rotated to face positive Z

    /*////////////
    //////////////
    //HALLWAY #3//
    //////////////
    */////////////
    //Back-wall clutter
    engine->CreateDrawable("Meshes/clutter_medium_1.obj", { 0.0f + 120.0f, 2.0f, -1.0f + 120.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f });

    //Hallway (part 1, left side) clutter
    engine->CreateDrawable("Meshes/clutter_small_2.obj", { -8.0f + 120.0f, 2.0f, 40.0f + 120.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -XM_PI, 0.0f });

    //Hallway (part 1, right side)
    //engine->CreateDrawable("Meshes/clutter_medium_1.obj", { 4.5f + 120.0f, 2.0f, 40.0f + 120.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative Z by default, rotated to face negative X

    //Hallway (part 2, far side) clutter
    //engine->CreateDrawable("Meshes/clutter_small_3.obj", { -8.5f + 120.0f, 2.0f, 79.0f + 120.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/clutter_medium_2.obj", { 0.0f + 120.0f, 2.0f, 80.0f + 120.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PI, 0.0f }); //Faces positive Z by default, rotated to face negative Z

    //Hallway (part 2, near side) clutter
    engine->CreateDrawable("Meshes/clutter_small_2.obj", { 25.0f + 120.0f, 2.0f, 64.0f + 120.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative X by default, rotated to face positive Z

    /*////////////
    //////////////
    //HALLWAY #4//
    //////////////
    */////////////
    //Back-wall clutter
    engine->CreateDrawable("Meshes/clutter_small_3.obj", { -9.0f + 180.0f, 2.0f, 4.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Hallway (part 1, left side) clutter
    engine->CreateDrawable("Meshes/clutter_medium_3.obj", { -8.0f + 180.0f, 2.0f, 72.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, {0.0f, XM_PIDIV2, 0.0f});

    //Hallway (part 1, right side)
    //engine->CreateDrawable("Meshes/clutter_medium_1.obj", { 4.5f + 180.0f, 2.0f, 40.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative Z by default, rotated to face negative X
    engine->CreateDrawable("Meshes/crate_wood.obj", { 11.0f + 180.0f, 3.0f, 22.0f + 180.0f }, { 2.0f, 2.0f, 2.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 11.0f + 180.0f, 3.0f, 26.0f + 180.0f }, { 2.0f, 2.0f, 2.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { 11.0f + 180.0f, 3.0f, 30.0f + 180.0f }, { 2.0f, 2.0f, 2.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Hallway (part 2, far side) clutter
    engine->CreateDrawable("Meshes/clutter_small_1.obj", { 22.5f + 180.0f, 2.0f, 78.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -XM_PIDIV2, 0.0f });
    //engine->CreateDrawable("Meshes/clutter_medium_2.obj", { 25.0f + 180.0f, 2.0f, 80.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PI, 0.0f }); //Faces positive Z by default, rotated to face negative Z

    //Hallway (part 2, near side) clutter
    //engine->CreateDrawable("Meshes/clutter_small_2.obj", { 25.0f + 180.0f, 2.0f, 64.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative X by default, rotated to face positive Z

    /*////////////
    //////////////
    //HALLWAY #5//
    //////////////
    */////////////
    //Back-wall clutter
    engine->CreateDrawable("Meshes/clutter_small_3.obj", { 5.0f + 240.0f, 2.0f, 4.0f + 240.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/clutter_small_1.obj", { -10.0f + 240.0f, 2.0f, 3.0f + 240.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Hallway (part 1, left side) clutter
    engine->CreateDrawable("Meshes/clutter_medium_2.obj", { -11.0f + 240.0f, 2.0f, 60.0f + 240.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Hallway (part 1, right side)
    engine->CreateDrawable("Meshes/clutter_small_2.obj", { 7.0f + 240.0f, 2.0f, 30.0f + 240.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }); //Faces negative Z by default, rotated to face negative X

    //Hallway (part 2, far side) clutter
    //engine->CreateDrawable("Meshes/clutter_medium_2.obj", { 25.0f + 180.0f, 2.0f, 80.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PI, 0.0f }); //Faces positive Z by default, rotated to face negative Z

    //Hallway (part 2, near side) clutter
    engine->CreateDrawable("Meshes/clutter_small_1.obj", { 22.5f + 240.0f, 2.0f, 62.0f + 240.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    //engine->CreateDrawable("Meshes/clutter_small_2.obj", { 25.0f + 180.0f, 2.0f, 64.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative X by default, rotated to face positive Z

    /*////////////
    //////////////
    //HALLWAY #6//
    //////////////
    */////////////
    //Back-wall clutter
    //engine->CreateDrawable("Meshes/clutter_small_3.obj", { 5.0f + 240.0f, 2.0f, 4.0f + 240.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    //engine->CreateDrawable("Meshes/clutter_small_1.obj", { -10.0f + 240.0f, 2.0f, 3.0f + 240.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Hallway (part 1, left side) clutter
    engine->CreateDrawable("Meshes/clutter_small_1.obj", { -11.0f + 300.0f, 2.0f, 81.0f + 300.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/clutter_small_3.obj", { -11.0f + 300.0f, 2.0f, 45.0f + 300.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Hallway (part 1, right side)
    engine->CreateDrawable("Meshes/clutter_medium_2.obj", { 4.5f + 300.0f, 2.0f, 10.0f + 300.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative Z by default, rotated to face negative X

    //Hallway (part 2, far side) clutter
    engine->CreateDrawable("Meshes/clutter_medium_1.obj", { 22.5f + 300.0f, 2.0f, 81.0f + 300.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PI, 0.0f });
    //engine->CreateDrawable("Meshes/clutter_medium_2.obj", { 25.0f + 180.0f, 2.0f, 80.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PI, 0.0f }); //Faces positive Z by default, rotated to face negative Z

    //Hallway (part 2, near side) clutter
    //engine->CreateDrawable("Meshes/clutter_small_2.obj", { 25.0f + 180.0f, 2.0f, 64.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative X by default, rotated to face positive Z

    //34 crate-type things
    for (int i = 103; i < 136; i++)
    {
        engine->ApplyNormalMapToDrawable(i, "NormalMaps/wood_invert.jpg");
    }
}

void SetupTestLevel(D3D11Engine* engine)
{
    for (int i = 0; i < 4; i++)
    {
        //Drawable creation
        int floorIndex = engine->CreateDrawable("Meshes/Test/stone.obj", { -5.0f + 20.0f * (float)i, 0.0f, 5.0f }, { 5.0f, 1.0f, 5.0f });
        int floorIndex2 = engine->CreateDrawable("Meshes/Test/stone.obj", { 5.0f + 20.0f * (float)i, 0.0f, 5.0f }, { 5.0f, 1.0f, 5.0f });
        int floorIndex3 = engine->CreateDrawable("Meshes/Test/stone.obj", { -5.0f + 20.0f * (float)i, 0.0f, -5.0f }, { 5.0f, 1.0f, 5.0f });
        int floorIndex4 = engine->CreateDrawable("Meshes/Test/stone.obj", { 5.0f + 20.0f * (float)i, 0.0f, -5.0f }, { 5.0f, 1.0f, 5.0f });

        int roofIndex = engine->CreateDrawable("Meshes/Test/stone.obj", { -5.0f + 20.0f * (float)i, 18.0f, 5.0f }, { 5.0f, 1.0f, 5.0f });
        int roofIndex2 = engine->CreateDrawable("Meshes/Test/stone.obj", { 5.0f + 20.0f * (float)i, 18.0f, 5.0f }, { 5.0f, 1.0f, 5.0f });
        int roofIndex3 = engine->CreateDrawable("Meshes/Test/stone.obj", { -5.0f + 20.0f * (float)i, 18.0f, -5.0f }, { 5.0f, 1.0f, 5.0f });
        int roofIndex4 = engine->CreateDrawable("Meshes/Test/stone.obj", { 5.0f + 20.0f * (float)i, 18.0f, -5.0f }, { 5.0f, 1.0f, 5.0f });

        int wallIndex = engine->CreateDrawable("Meshes/Test/wall.obj", { 0.0f + 20.0f * (float)i, 9.0f, 11.0f }, { 10.0f, 8.0f, 1.0f });
        //int wallIndex2 = engine->CreateDrawable("Meshes/Test/wall.obj", { -11.0f, 9.0f, 0.0f }, { 10.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
        //int wallIndex3 = engine->CreateDrawable("Meshes/Test/wall.obj", { 11.0f, 9.0f, 0.0f }, { 10.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

        //Normalmap application
        engine->ApplyNormalMapToDrawable(floorIndex, "NormalMaps/stone_fancy.jpg");
        engine->ApplyNormalMapToDrawable(floorIndex2, "NormalMaps/stone_fancy.jpg");
        engine->ApplyNormalMapToDrawable(floorIndex3, "NormalMaps/stone_fancy.jpg");
        engine->ApplyNormalMapToDrawable(floorIndex4, "NormalMaps/stone_fancy.jpg");

        engine->ApplyNormalMapToDrawable(roofIndex, "NormalMaps/stone_fancy.jpg");
        engine->ApplyNormalMapToDrawable(roofIndex2, "NormalMaps/stone_fancy.jpg");
        engine->ApplyNormalMapToDrawable(roofIndex3, "NormalMaps/stone_fancy.jpg");
        engine->ApplyNormalMapToDrawable(roofIndex4, "NormalMaps/stone_fancy.jpg");

        engine->ApplyNormalMapToDrawable(wallIndex, "NormalMaps/brick_fancy.jpg");
        //engine->ApplyNormalMapToDrawable(wallIndex2, "NormalMaps/brick_fancy.jpg");
        //engine->ApplyNormalMapToDrawable(wallIndex3, "NormalMaps/brick_fancy.jpg");
    }
    
    for (int i = 0; i < 2; i++)
    {
        //Drawable creation
        int crateIndex = engine->CreateDrawable("Meshes/Test/crate.obj", { 0.0f + 10.0f * (float)i, 2.0f, 5.0f});
        int crateIndex2 = engine->CreateDrawable("Meshes/Test/crate.obj", { 0.0f + 10.0f * (float)i, 4.0f, 5.0f });
        int crateIndex3 = engine->CreateDrawable("Meshes/Test/crate.obj", { 0.0f + 10.0f * (float)i, 6.0f, 5.0f });
        int crateIndex4 = engine->CreateDrawable("Meshes/Test/crate.obj", { 0.0f + 10.0f * (float)i, 8.0f, 5.0f });
        int crateIndex5 = engine->CreateDrawable("Meshes/Test/crate.obj", { 0.0f + 10.0f * (float)i, 10.0f, 5.0f });
        int crateIndex6 = engine->CreateDrawable("Meshes/Test/crate.obj", { 0.0f + 10.0f * (float)i, 12.0f, 5.0f });
        int crateIndex7 = engine->CreateDrawable("Meshes/Test/crate.obj", { 0.0f + 10.0f * (float)i, 14.0f, 5.0f });
        int crateIndex8 = engine->CreateDrawable("Meshes/Test/crate.obj", { 0.0f + 10.0f * (float)i, 16.0f, 5.0f });

        int crateIndex11 = engine->CreateDrawable("Meshes/Test/crate.obj", { 3.0f + 10.0f * (float)i, 2.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PI / 8.0f, 0.0f }); //PI is a full turn, half of pi is 180 degrees, fourth of pi is 90, 8th is 45 bla bla, so 45 degrees back and forth here
        int crateIndex12 = engine->CreateDrawable("Meshes/Test/crate.obj", { 3.0f + 10.0f * (float)i, 4.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -XM_PI / 8.0f, 0.0f });
        int crateIndex13 = engine->CreateDrawable("Meshes/Test/crate.obj", { 3.0f + 10.0f * (float)i, 6.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PI / 8.0f, 0.0f });
        int crateIndex14 = engine->CreateDrawable("Meshes/Test/crate.obj", { 3.0f + 10.0f * (float)i, 8.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -XM_PI / 8.0f, 0.0f });
        int crateIndex15 = engine->CreateDrawable("Meshes/Test/crate.obj", { 3.0f + 10.0f * (float)i, 10.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PI / 8.0f, 0.0f });
        int crateIndex16 = engine->CreateDrawable("Meshes/Test/crate.obj", { 3.0f + 10.0f * (float)i, 12.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -XM_PI / 8.0f, 0.0f });
        int crateIndex17 = engine->CreateDrawable("Meshes/Test/crate.obj", { 3.0f + 10.0f * (float)i, 14.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PI / 8.0f, 0.0f });
        int crateIndex18 = engine->CreateDrawable("Meshes/Test/crate.obj", { 3.0f + 10.0f * (float)i, 16.0f, 5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -XM_PI / 8.0f, 0.0f });

        //Normalmap application
        engine->ApplyNormalMapToDrawable(crateIndex, "NormalMaps/wood_fancy_invert.png");
        engine->ApplyNormalMapToDrawable(crateIndex2, "NormalMaps/wood_fancy_invert.png");
        engine->ApplyNormalMapToDrawable(crateIndex3, "NormalMaps/wood_fancy_invert.png");
        engine->ApplyNormalMapToDrawable(crateIndex4, "NormalMaps/wood_fancy_invert.png");
        engine->ApplyNormalMapToDrawable(crateIndex5, "NormalMaps/wood_fancy_invert.png");
        engine->ApplyNormalMapToDrawable(crateIndex6, "NormalMaps/wood_fancy_invert.png");
        engine->ApplyNormalMapToDrawable(crateIndex7, "NormalMaps/wood_fancy_invert.png");
        engine->ApplyNormalMapToDrawable(crateIndex8, "NormalMaps/wood_fancy_invert.png");

        engine->ApplyNormalMapToDrawable(crateIndex11, "NormalMaps/wood_fancy_invert.png");
        engine->ApplyNormalMapToDrawable(crateIndex12, "NormalMaps/wood_fancy_invert.png");
        engine->ApplyNormalMapToDrawable(crateIndex13, "NormalMaps/wood_fancy_invert.png");
        engine->ApplyNormalMapToDrawable(crateIndex14, "NormalMaps/wood_fancy_invert.png");
        engine->ApplyNormalMapToDrawable(crateIndex15, "NormalMaps/wood_fancy_invert.png");
        engine->ApplyNormalMapToDrawable(crateIndex16, "NormalMaps/wood_fancy_invert.png");
        engine->ApplyNormalMapToDrawable(crateIndex17, "NormalMaps/wood_fancy_invert.png");
        engine->ApplyNormalMapToDrawable(crateIndex18, "NormalMaps/wood_fancy_invert.png");
    }

    //Drawable creation
    int crateIndex21 = engine->CreateDrawable("Meshes/Test/crate.obj", { 5.0f, 2.0f, 0.0f });
    int crateIndex22 = engine->CreateDrawable("Meshes/Test/crate.obj", { 5.0f, 4.0f, 0.0f });
    int crateIndex23 = engine->CreateDrawable("Meshes/Test/crate.obj", { 5.0f, 6.0f, 0.0f });
    int crateIndex24 = engine->CreateDrawable("Meshes/Test/crate.obj", { 5.0f, 8.0f, 0.0f });
    int crateIndex25 = engine->CreateDrawable("Meshes/Test/crate.obj", { 5.0f, 10.0f, 0.0f });
    int crateIndex26 = engine->CreateDrawable("Meshes/Test/crate.obj", { 5.0f, 12.0f, 0.0f });
    int crateIndex27 = engine->CreateDrawable("Meshes/Test/crate.obj", { 5.0f, 14.0f, 0.0f });
    int crateIndex28 = engine->CreateDrawable("Meshes/Test/crate.obj", { 5.0f, 16.0f, 0.0f });

    //Normalmap application
    engine->ApplyNormalMapToDrawable(crateIndex21, "NormalMaps/wood_fancy_invert.png");
    engine->ApplyNormalMapToDrawable(crateIndex22, "NormalMaps/wood_fancy_invert.png");
    engine->ApplyNormalMapToDrawable(crateIndex23, "NormalMaps/wood_fancy_invert.png");
    engine->ApplyNormalMapToDrawable(crateIndex24, "NormalMaps/wood_fancy_invert.png");
    engine->ApplyNormalMapToDrawable(crateIndex25, "NormalMaps/wood_fancy_invert.png");
    engine->ApplyNormalMapToDrawable(crateIndex26, "NormalMaps/wood_fancy_invert.png");
    engine->ApplyNormalMapToDrawable(crateIndex27, "NormalMaps/wood_fancy_invert.png");
    engine->ApplyNormalMapToDrawable(crateIndex28, "NormalMaps/wood_fancy_invert.png");

    //Drawable creation
    int crateIndex31 = engine->CreateDrawable("Meshes/Test/crate.obj", { -5.0f, 2.0f, 0.0f });
    int crateIndex32 = engine->CreateDrawable("Meshes/Test/crate.obj", { -5.0f, 4.0f, 0.0f });
    int crateIndex33 = engine->CreateDrawable("Meshes/Test/crate.obj", { -5.0f, 6.0f, 0.0f });
    int crateIndex34 = engine->CreateDrawable("Meshes/Test/crate.obj", { -5.0f, 8.0f, 0.0f });
    int crateIndex35 = engine->CreateDrawable("Meshes/Test/crate.obj", { -5.0f, 10.0f, 0.0f });
    int crateIndex36 = engine->CreateDrawable("Meshes/Test/crate.obj", { -5.0f, 12.0f, 0.0f });
    int crateIndex37 = engine->CreateDrawable("Meshes/Test/crate.obj", { -5.0f, 14.0f, 0.0f });
    int crateIndex38 = engine->CreateDrawable("Meshes/Test/crate.obj", { -5.0f, 16.0f, 0.0f });

    //Normalmap application
    engine->ApplyNormalMapToDrawable(crateIndex31, "NormalMaps/wood_fancy_invert.png");
    engine->ApplyNormalMapToDrawable(crateIndex32, "NormalMaps/wood_fancy_invert.png");
    engine->ApplyNormalMapToDrawable(crateIndex33, "NormalMaps/wood_fancy_invert.png");
    engine->ApplyNormalMapToDrawable(crateIndex34, "NormalMaps/wood_fancy_invert.png");
    engine->ApplyNormalMapToDrawable(crateIndex35, "NormalMaps/wood_fancy_invert.png");
    engine->ApplyNormalMapToDrawable(crateIndex36, "NormalMaps/wood_fancy_invert.png");
    engine->ApplyNormalMapToDrawable(crateIndex37, "NormalMaps/wood_fancy_invert.png");
    engine->ApplyNormalMapToDrawable(crateIndex38, "NormalMaps/wood_fancy_invert.png");

    //Add a light because otherwise normal mapping obviously won't do anything
    engine->CreateConcaveDrawable("Meshes/lamp.obj", { 0.0f, 16.75f, 0.0f });
    engine->CreateLightPoint({ 0.0f, 17.75f, 0.0f }, 3.0f, { 1.0f, 1.0f, 0.5f });
    engine->CreateLightSpot({ 0.0f, 16.5f, 0.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
}

void SetupInstancedLevel(D3D11Engine* engine)
{
    //Average FPS: 2500
    //Process Memory: 122MB

    //Increasing the crates from 125 to 512 (5*5*5 to 8*8*8) has no impact on performance (How cracked is that)

    //engine->CreateInstancedDrawable("Meshes/Test/crate.obj");

     //Grid-size
    const int n = 8;

    //Total
    float width = 16.0f;
    float height = 16.0f;
    float depth = 16.0f;

    //Between each instance
    float x = -0.5f * width;
    float y = -0.5f * height;
    float z = -0.5f * depth;
    float dx = width / (n - 1);
    float dy = height / (n - 1);
    float dz = depth / (n - 1);

    /*
    m_instancedData[k * n * n + i * n + j].world = XMFLOAT4X4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        x + j * dx, y + i * dy, z + k * dz, 1.0f);
    */

    /*for (int k = 0; k < n; ++k)
    {
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                engine->CreateInstancedDrawable("Meshes/Test/crate.obj", { -10.0f + j * dx, -10.0f + i * dy, 10.0f + k * dz });
            }
        }
    }*/

    //Floor (100 crates)
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            engine->CreateInstancedDrawable("Meshes/Test/crate.obj", { -10.0f + (float)i * 2.0f, 0.0f, 0.0f + (float)j * 2.0f});
        }
    }

    //Left wall (70 crates)
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            engine->CreateInstancedDrawable("Meshes/Test/crate.obj", { -10.0f, 2.0f + (float)i * 2.0f, 0.0f + (float)j * 2.0f});
        }
    }

    //Right wall (Another 70 crates)
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            engine->CreateInstancedDrawable("Meshes/Test/crate.obj", { 8.0f, 2.0f + (float)i * 2.0f, 0.0f + (float)j * 2.0f });
        }
    }

    //Back wall (56 crates)
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            engine->CreateInstancedDrawable("Meshes/Test/crate.obj", { -8.0f + (float)j * 2.0f, 2.0f + (float)i * 2.0f, 18.0f});
        }
    }

    engine->CreateInstancedDrawable("Meshes/Test/crate.obj", { -1.0f, 4.0f, 8.0f });

    engine->ApplyNormalMapToDrawable(0, "NormalMaps/wood_fancy_invert.png");

    //296 crates (with normalmaps) and a light: 2300 fps (297, put a little boy in the middle)

    //Finale yo
    engine->SetupInstancedBuffer();

    engine->CreateLightSpot({ -1.0f, 16.0f, 8.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
    //engine->CreateLightSpot({ 0.0f, 16.0f, -4.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
}

void SetupNonInstancedLevel(D3D11Engine* engine)
{
    //Average FPS: 300
    //Process Memory: 122MB (I thought this would be higher, since I create so many separate drawables here, but apparently not? Guess the textures only being loaded in once is good by me?)

    //Increasing the crates from 125 to 512 (5*5*5 to 8*8*8) makes the fps drop from 300 average down to 80

    //Grid-size
    const int n = 8;

    //Total
    float width = 20.0f;
    float height = 20.0f;
    float depth = 20.0f;

    //Between each instance
    float x = -0.5f * width;
    float y = -0.5f * height;
    float z = -0.5f * depth;
    float dx = width / (n - 1);
    float dy = height / (n - 1);
    float dz = depth / (n - 1);

    /*
    m_instancedData[k * n * n + i * n + j].world = XMFLOAT4X4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        x + j * dx, y + i * dy, z + k * dz, 1.0f);
    */

    /*for (int k = 0; k < n; ++k)
    {
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                engine->CreateDrawable("Meshes/Test/crate.obj", {-10.0f + j * dx, -10.0f + i * dy, -10.0f + k * dz});
            }
        }
    }*/

    //Floor (100 crates)
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            int index = engine->CreateDrawable("Meshes/Test/crate.obj", { -10.0f + (float)i * 2.0f, 0.0f, 0.0f + (float)j * 2.0f });
            engine->ApplyNormalMapToDrawable(index, "NormalMaps/wood_fancy_invert.png");
        }
    }

    //Left wall (70 crates)
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            int index = engine->CreateDrawable("Meshes/Test/crate.obj", { -10.0f, 2.0f + (float)i * 2.0f, 0.0f + (float)j * 2.0f });
            engine->ApplyNormalMapToDrawable(index, "NormalMaps/wood_fancy_invert.png");
        }
    }

    //Right wall (Another 70 crates)
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            int index = engine->CreateDrawable("Meshes/Test/crate.obj", { 8.0f, 2.0f + (float)i * 2.0f, 0.0f + (float)j * 2.0f });
            engine->ApplyNormalMapToDrawable(index, "NormalMaps/wood_fancy_invert.png");
        }
    }

    //Back wall (56 crates)
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int index = engine->CreateDrawable("Meshes/Test/crate.obj", { -8.0f + (float)j * 2.0f, 2.0f + (float)i * 2.0f, 18.0f });
            engine->ApplyNormalMapToDrawable(index, "NormalMaps/wood_fancy_invert.png");
        }
    }

    engine->CreateLightSpot({ -1.0f, 16.5f, 9.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
}
