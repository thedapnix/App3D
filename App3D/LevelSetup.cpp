#include "LevelSetup.h"

using namespace DirectX;

void SetupLevel1(D3D11Engine* engine)
{
    //FIRST ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f,  0.0f, -5.0f }, { 2.0f, 1.0f, 2.0f });
    engine->CreateDrawable("Meshes/ground_stone.obj", { 0.0f, 18.0f, -5.0f }, { 2.0f, 1.0f, 2.0f });

    //FRONT WALL
    //Door (elongated metal crate with a small metal sphere as a makeshift doorknob, sue me)
    engine->CreateDrawable("Meshes/door_metal.obj", { 0.0f, 9.0f, 15.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { -2.75f, 9.25f, 13.75f }, { 0.25f, 0.25f, 0.25f });
    //Walls surrounding the door (left, then right)
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
    engine->CreateLightPoint({ 0.0f, 17.75f, -5.0f }, 3.0f, { 1.0f, 1.0f, 0.0f });  //Doesn't light if it's *below* the lamp and lights upwards, then it just hits the roof... 
                                                                                    //This is a workaround but not good. Thoughts:
                                                                                    //Hollowed out cone so we don't have downwards/inwards facing normals
                                                                                    //Thus, we don't connect when we do nDotL in the shader (L = distance vec from light to pixel)
    engine->CreateLightSpot({ 0.0f, 16.5f, -5.0f }, 0.75f, 0.0f, 0.5f, {1.0f, 1.0f, 1.0f});

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
        Drawables   -> 4
        Lights      -> 0
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
    engine->CreateDrawable("Meshes/wall_brick.obj", { 0.0f, 9.0f, 76.0f },  { 20.0f, 8.0f, 1.0f });     //From  {-20, +1, +75} to {+20, +17 +77}
    engine->CreateDrawable("Meshes/wall_brick.obj", { -50.0f, 9.0f, 76.0f }, { 10.0f, 8.0f, 1.0f });

    //Door at the far end
    engine->CreateDrawable("Meshes/door_metal.obj", { -60.0f, 9.0f, 55.0f }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", { -58.75f, 9.25f, 52.25f }, { 0.25f, 0.25f, 0.25f }, { 0.0f, -XM_PIDIV2, 0.0f });
    //Walls surrounding the door (left, then right)
    engine->CreateDrawable("Meshes/wall_brick.obj", { -60.0f, 9.0f, 42.5f }, { 7.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -60.0f,  9.0f, 67.5f }, { 7.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Final wall connecting back to the hallway we came from
    engine->CreateDrawable("Meshes/wall_brick.obj", { -35.0f, 9.0f, 36.0f }, { 25.0f, 8.0f, 1.0f });

    //Light(s)
    engine->CreateConcaveDrawable("Meshes/lamp.obj", { 0.0f, 16.75f, 55.0f });
    engine->CreateLightPoint({ 0.0f, 17.75f, 55.0f }, 3.0f, { 1.0f, 1.0f, 0.0f });
    engine->CreateLightSpot({ 0.0f, 16.5f, 55.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
    engine->CreateConcaveDrawable("Meshes/lamp.obj", { -35.0f, 16.75f, 55.0f });
    engine->CreateLightPoint({ -35.0f, 17.75f, 55.0f }, 3.0f, { 1.0f, 1.0f, 0.0f });
    engine->CreateLightSpot({ -35.0f, 16.5f, 55.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });

    /*
    Room    #2 Completed
        Drawables   -> 15
        Lights      -> 2
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
        Drawables   -> 4
        Lights      -> 0
    */

    //THIRD ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", { -40.0f, 0.0f, 115.0f }, { 2.0f, 1.0f, 2.0f });   //From  {-60, -1, +95} to {-20, +1, +135}
    engine->CreateDrawable("Meshes/ground_stone.obj", {   0.0f, 0.0f, 115.0f }, { 2.0f, 1.0f, 2.0f });   //From  {-20, -1, +95} to {+20, +1, +135}
    engine->CreateDrawable("Meshes/ground_stone.obj", {  40.0f, 0.0f, 115.0f }, { 2.0f, 1.0f, 2.0f });   //From  {+20, -1, +95} to {+60, +1, +135}
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
    engine->CreateDrawable("Meshes/wall_brick.obj", { -60.0f, 9.0f, 115.0f }, { 20.0f, 8.0f, 1.0f }, {0.0f, XM_PIDIV2, 0.0f});
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
    engine->CreateLightPoint({ 5.0f, 17.75f, 115.0f }, 3.0f, { 1.0f, 1.0f, 0.0f });
    engine->CreateLightSpot({ 5.0f, 16.5f, 115.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
    engine->CreateConcaveDrawable("Meshes/lamp.obj", { -30.0f, 16.75f, 115.0f });
    engine->CreateLightPoint({ -30.0f, 17.75f, 115.0f }, 3.0f, { 1.0f, 1.0f, 0.0f });
    engine->CreateLightSpot({ -30.0f, 16.5f, 115.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
    engine->CreateConcaveDrawable("Meshes/lamp.obj", { 45.0f, 16.75f, 80.0f });
    engine->CreateLightPoint({ 45.0f, 17.75f, 80.0f }, 3.0f, { 1.0f, 1.0f, 0.0f });
    engine->CreateLightSpot({ 45.0f, 16.5f, 80.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });

    /*
    Room    #3 Completed
        Drawables   -> 23
        Lights      -> 3
    */

    //HALLWAY TO "FOURTH" ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", { 15.0f, 0.0f, 145.0f }, { 1.0f, 1.0f, 1.0f });   //From  {+5, -1, +135} to {+25, +1, +155}
    engine->CreateDrawable("Meshes/ground_stone.obj", {  5.0f, 0.0f, 165.0f }, { 2.0f, 1.0f, 1.0f });   //From  {-15, -1, +155} to {+25, +1, +175}
    engine->CreateDrawable("Meshes/ground_stone.obj", { 15.0f, 18.0f, 145.0f }, { 1.0f, 1.0f, 1.0f });  
    engine->CreateDrawable("Meshes/ground_stone.obj", { 5.0f, 18.0f, 165.0f }, { 2.0f, 1.0f, 1.0f });  
    
    //Walls (Starting from the right side)
    engine->CreateDrawable("Meshes/wall_brick.obj", { 25.0f, 9.0f, 155.0f }, { 20.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", {  5.0f, 9.0f, 175.0f }, { 20.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -5.0f, 9.0f, 155.0f }, { 10.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", {  5.0f, 9.0f, 145.0f }, { 10.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    /*
    Hallway #4 Completed
        Drawables   -> 8
        Lights      -> 0
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
    engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f, 9.0f, 182.5f }, {  7.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -15.0f, 9.0f, 147.5f }, { 7.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -40.0f, 9.0f, 140.0f }, { 25.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", { -65.0f, 9.0f, 145.0f }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Light(s)


    /*
    Room    #4 Completed
        Drawables   -> 13
        Lights      -> TBD
    */

    //OTHER HALLWAY TO OTHER "FOURTH" ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", { -67.5f, 0.0f, 55.0f }, {0.75f, 1.0f, 1.0f});//From {-80, -1, +45} to {-60, +1, +65}
    engine->CreateDrawable("Meshes/ground_stone.obj", { -90.0f, 0.0f, 75.0f }, {1.5f, 1.0f, 3.0f}); //From {-105, -1, +45} to {-75, +1, +105}
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

    /*
    Hallway #5 Completed
        Drawables   -> 12
        Lights      -> TBD
    */

    //Ending (Connect the two alternating paths, one is a hallway ending at -90, 0, 105, the other a room with an edge at -65, 0, 205)
    //engine->CreateDrawable("Meshes/ground_stone.obj", { -90.0f, 0.0f, 155.0f }, { 2.5f, 1.0f, 5.0f });  //From {-115, -1, +105} to {-65, +1, +205}

    //TALLY UP THE TOTAL AMOUNTS OF DRAWABLES, WHICH WILL GIVE US A BREAKPOINT OF WHERE THE MAP ENDS AND OBJECTS BEGIN

    /*CLUTTER*/
    //FIRST ROOM
    //Clutter on the left side of the door
    engine->CreateDrawable("Meshes/crate_wood.obj", { -9.5f, 3.0f, 10.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -9.5f, 6.0f, 10.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f,  0.45f, 0.0f }, 1, { 2, 3 }); //interactID 1 = Destroy something, interacts with {1, 2}, aka the door
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

    //SECOND ROOM
    //Clutter on the right side of the door
    engine->CreateDrawable("Meshes/crate_wood.obj", { -55.0f, 3.0f, 71.5f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -53.0f, 2.5f, 67.5f }, { 1.5f, 1.5f, 1.5f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -53.0f, 5.0f, 67.5f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -56.5f, 2.0f, 66.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -49.0f, 3.0f, 71.5f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", { -49.0f, 6.0f, 71.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });



    //"THIRD" ROOM

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