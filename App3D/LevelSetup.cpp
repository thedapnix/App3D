#include "LevelSetup.h"

using namespace DirectX;

//Hidden helper function
void FinishInstancedLevel(D3D11Engine* engine)
{
    //First one: Begin = 0, End = 10000        (    0), (    0 + 10000)
    //Second one: Begin = 1000, End = 10700    (10000), (10000 +   700)
    int previousBegin = 0;
    int previousEnd = 0;
    std::vector<int> vecX;
    std::vector<int> vecY;
    int vecFinal = 0;

    //Calculate buffer stuff
    size_t nDrawables = engine->GetDrawables().size();
    for (int i = 0; i < nDrawables; i++) //In this case there are 2 original drawables
    {
        vecX.push_back(previousEnd);
        vecY.push_back(previousEnd + engine->GetDrawables().at(i).GetInstanceCount());

        previousEnd = vecY[i];
    }

    //Resize the total instance buffer to be all the drawables (including instances)
    engine->ResizeInstanceBuffer(previousEnd);

    //Setup instance buffers per drawable using previous calculations
    for (int i = 0; i < nDrawables; i++)
    {
        engine->SetupInstancedBuffer(vecX[i], vecY[i], i);
    }
}

void SetupLevel1(D3D11Engine* engine)
{
    //FIRST ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { 0.0f,  0.0f, -5.0f }, { 2.0f, 1.0f, 2.0f });
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { 0.0f, 18.0f, -5.0f }, { 2.0f, 1.0f, 2.0f });

    //FRONT WALL
    //Door (elongated metal crate with a small metal sphere as a makeshift doorknob, sue me) and walls surrounding it (left, then right)
    engine->CreateDrawable("Meshes/door_metal.obj", "", { 0.0f, 9.0f, 15.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", "", { -2.75f, 9.25f, 13.75f }, { 0.25f, 0.25f, 0.25f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -12.5f, 9.0f, 15.0f }, { 7.5f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { 12.5f,  9.0f, 15.0f }, { 7.5f, 8.0f, 1.0f });

    //LEFT AND RIGHT WALLS
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -20.0f, 9.0f, -5.0f }, { 20.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { 20.0f, 9.0f, -5.0f }, { 20.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //BACK WALL
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { 0.0f, 9.0f, -25.0f }, { 20.0f, 8.0f, 1.0f });

    //Light(s)
    //Ceiling in the center of the room pointing straight down onto the floor (Lamp mesh, point light lighting up the mesh, and spotlight pointing down to the ground)
    engine->CreateConcaveDrawable("Meshes/lamp.obj", "", { 0.0f, 16.75f, -5.0f });
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
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { 0.0f,  0.0f, 25.0f });
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { 0.0f, 18.0f, 25.0f });

    //Walls (left, then right)
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -10.0f, 9.0f, 26.0f }, { 10.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { 10.0f, 9.0f, 26.0f }, { 10.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    /*
    Hallway #1 Completed
        Drawables   -> 4    (14)
        Lights      -> 0    (1)
    */

    //SECOND ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { 0.0f, 0.0f, 55.0f }, { 2.0f, 1.0f, 2.0f });     //From  {-20, -1, +35} to {+20, +1, +75}
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { -40.0f, 0.0f, 55.0f }, { 2.0f, 1.0f, 2.0f });   //From  {-60, -1, +35} to {-20, +1, +75}
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { 0.0f, 18.0f, 55.0f }, { 2.0f, 1.0f, 2.0f });
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { -40.0f, 18.0f, 55.0f }, { 2.0f, 1.0f, 2.0f });

    //Walls (counter-clockwise as you exit the hallway)
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { 15.0f, 9.0f, 36.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { 20.0f, 9.0f, 56.0f }, { 20.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { 0.0f, 9.0f, 76.0f }, { 20.0f, 8.0f, 1.0f });     //From  {-20, +1, +75} to {+20, +17 +77}
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -50.0f, 9.0f, 76.0f }, { 10.0f, 8.0f, 1.0f });

    //Door and walls (left, then right)
    engine->CreateDrawable("Meshes/door_metal.obj", "", { -60.0f, 9.0f, 55.0f }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", "", { -58.75f, 9.25f, 52.25f }, { 0.25f, 0.25f, 0.25f }, { 0.0f, -XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -60.0f, 9.0f, 42.5f }, { 7.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -60.0f,  9.0f, 67.5f }, { 7.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Final wall connecting back to the hallway we came from
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -35.0f, 9.0f, 36.0f }, { 25.0f, 8.0f, 1.0f });

    //Light(s)
    engine->CreateConcaveDrawable("Meshes/lamp.obj", "", { 0.0f, 16.75f, 55.0f });
    engine->CreateLightPoint({ 0.0f, 17.75f, 55.0f }, 3.0f, { 1.0f, 1.0f, 0.5f });
    engine->CreateLightSpot({ 0.0f, 16.5f, 55.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
    engine->CreateConcaveDrawable("Meshes/lamp.obj", "", { -35.0f, 16.75f, 55.0f });
    engine->CreateLightPoint({ -35.0f, 17.75f, 55.0f }, 3.0f, { 1.0f, 1.0f, 0.5f });
    engine->CreateLightSpot({ -35.0f, 16.5f, 55.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });

    /*
    Room    #2 Completed
        Drawables   -> 15   (29)
        Lights      -> 2    (3)
    */

    //HALLWAY AFTER SECOND ROOM (TAKING A "RIGHT" INSTEAD OF CONTINUING FORWARD THROUGH THE ROOM)
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { -30.0f,  0.0f, 85.0f });
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { -30.0f, 18.0f, 85.0f });

    //Walls (left, then right)
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -40.0f, 9.0f, 86.0f }, { 10.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -20.0f, 9.0f, 86.0f }, { 10.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    /*
    Hallway #2 Completed
        Drawables   -> 4    (33)
        Lights      -> 0    (3)
    */

    //THIRD ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { -40.0f, 0.0f, 115.0f }, { 2.0f, 1.0f, 2.0f });   //From  {-60, -1, +95} to {-20, +1, +135}
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { 0.0f, 0.0f, 115.0f }, { 2.0f, 1.0f, 2.0f });   //From  {-20, -1, +95} to {+20, +1, +135}
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { 40.0f, 0.0f, 115.0f }, { 2.0f, 1.0f, 2.0f });   //From  {+20, -1, +95} to {+60, +1, +135}
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { 45.0f, 0.0f, 80.0f }, { 1.5f, 1.0f, 1.5f });   //From  {+30, -1, +65} to {+60, +1, +95}

    engine->CreateDrawable("Meshes/ground_stone.obj", "", { -40.0f, 18.0f, 115.0f }, { 2.0f, 1.0f, 2.0f });   //From  {-60, -1, +95} to {-20, +1, +135}
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { 0.0f, 18.0f, 115.0f }, { 2.0f, 1.0f, 2.0f });   //From  {-20, -1, +95} to {+20, +1, +135}
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { 40.0f, 18.0f, 115.0f }, { 2.0f, 1.0f, 2.0f });   //From  {+20, -1, +95} to {+60, +1, +135}
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { 45.0f, 18.0f, 80.0f }, { 1.5f, 1.0f, 1.5f });   //From  {+30, -1, +65} to {+60, +1, +95}

    //Door and walls going counter-clockwise
    engine->CreateDrawable("Meshes/door_metal.obj", "", { 15.0f, 9.0f, 135.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", "", { 12.25f, 9.25f, 133.75f }, { 0.25f, 0.25f, 0.25f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -10.0f,  9.0f, 135.0f }, { 20.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -45.0f, 9.0f, 135.0f }, { 15.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -60.0f, 9.0f, 115.0f }, { 20.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -50.0f, 9.0f, 96.0f }, { 10.0f, 8.0f, 1.0f }); //Now we're back at the hallway

    //Keep going counter-clockwise starting from the hallway entrance
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { 5.0f, 9.0f, 96.0f }, { 25.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { 30.0f, 9.0f, 81.0f }, { 15.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { 45.0f, 9.0f, 65.0f }, { 15.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { 60.0f, 9.0f, 82.5f }, { 17.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { 60.0f, 9.0f, 117.5f }, { 17.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { 40.0f, 9.0f, 135.0f }, { 20.0f, 8.0f, 1.0f }); //Now we're back to the door and the room is finished

    //Light(s)
    engine->CreateConcaveDrawable("Meshes/lamp.obj", "", { 5.0f, 16.75f, 115.0f });
    engine->CreateLightPoint({ 5.0f, 17.75f, 115.0f }, 3.0f, { 1.0f, 1.0f, 0.5f });
    engine->CreateLightSpot({ 5.0f, 16.5f, 115.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
    engine->CreateConcaveDrawable("Meshes/lamp.obj", "", { -30.0f, 16.75f, 115.0f });
    engine->CreateLightPoint({ -30.0f, 17.75f, 115.0f }, 3.0f, { 1.0f, 1.0f, 0.5f });
    engine->CreateLightSpot({ -30.0f, 16.5f, 115.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
    engine->CreateConcaveDrawable("Meshes/lamp.obj", "", { 45.0f, 16.75f, 80.0f });
    engine->CreateLightPoint({ 45.0f, 17.75f, 80.0f }, 3.0f, { 1.0f, 1.0f, 0.5f });
    engine->CreateLightSpot({ 45.0f, 16.5f, 80.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });

    /*
    Room    #3 Completed
        Drawables   -> 23   (56)
        Lights      -> 3    (6)
    */

    //HALLWAY TO "FOURTH" ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { 15.0f, 0.0f, 145.0f }, { 1.0f, 1.0f, 1.0f });   //From  {+5, -1, +135} to {+25, +1, +155}
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { 5.0f, 0.0f, 165.0f }, { 2.0f, 1.0f, 1.0f });   //From  {-15, -1, +155} to {+25, +1, +175}
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { 15.0f, 18.0f, 145.0f }, { 1.0f, 1.0f, 1.0f });
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { 5.0f, 18.0f, 165.0f }, { 2.0f, 1.0f, 1.0f });

    //Walls (Starting from the right side)
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { 25.0f, 9.0f, 155.0f }, { 20.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { 5.0f, 9.0f, 175.0f }, { 20.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -5.0f, 9.0f, 155.0f }, { 10.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { 5.0f, 9.0f, 145.0f }, { 10.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    /*
    Hallway #4 Completed
        Drawables   -> 8    (64)
        Lights      -> 0    (6)
    */

    //"FOURTH" ROOM (Leads to the end)
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { -40.0f, 0.0f, 165.0f }, { 2.5f, 1.0f, 2.5f });   //From  {-65, -1, +140} to {-15, +1, +190}
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { -40.0f, 18.0f, 165.0f }, { 2.5f, 1.0f, 2.5f });

    //Walls and Doors (Starting from the door on the left, going clockwise)
    engine->CreateDrawable("Meshes/door_metal.obj", "", { -65.0f, 9.0f, 155.0f }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", "", { -63.75f, 9.25f, 152.25f }, { 0.25f, 0.25f, 0.25f }, { 0.0f, -XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -65.0f, 9.0f, 165.0f }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/door_metal.obj", "", { -65.0f, 9.0f, 175.0f }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", "", { -63.75f, 9.25f, 172.25f }, { 0.25f, 0.25f, 0.25f }, { 0.0f, -XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -65.0f, 9.0f, 185.0f }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -40.0f, 9.0f, 190.0f }, { 25.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -15.0f, 9.0f, 182.5f }, { 7.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -15.0f, 9.0f, 147.5f }, { 7.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -40.0f, 9.0f, 140.0f }, { 25.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -65.0f, 9.0f, 145.0f }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Light(s)
    engine->CreateConcaveDrawable("Meshes/lamp.obj", "", { -40.0f, 16.75f, 165.0f });
    engine->CreateLightPoint({ -40.0f, 17.75f, 165.0f }, 3.0f, { 1.0f, 1.0f, 0.5f });
    engine->CreateLightSpot({ -40.0f, 16.5f, 165.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });

    /*
    Room    #4 Completed
        Drawables   -> 14   (78)
        Lights      -> 1    (7)
    */

    //OTHER HALLWAY TO OTHER "FOURTH" ROOM
    //Floor and ceiling
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { -67.5f, 0.0f, 55.0f }, { 0.75f, 1.0f, 1.0f });//From {-80, -1, +45} to {-60, +1, +65}
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { -90.0f, 0.0f, 75.0f }, { 1.5f, 1.0f, 3.0f }); //From {-105, -1, +45} to {-75, +1, +105}
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { -67.5f, 18.0f, 55.0f }, { 0.75f, 1.0f, 1.0f });
    engine->CreateDrawable("Meshes/ground_stone.obj", "", { -90.0f, 18.0f, 75.0f }, { 1.5f, 1.0f, 3.0f });

    //Walls and door (Starting from the door, going clockwise)
    engine->CreateDrawable("Meshes/door_metal.obj", "", { -90.0f, 9.0f, 105.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", "", { -87.25f, 9.25f, 103.75f }, { 0.25f, 0.25f, 0.25f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -80.0f, 9.0f, 105.0f }, { 5.0f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -75.0f, 9.0f, 85.0f }, { 20.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -67.5f, 9.0f, 65.0f }, { 7.5f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -82.5f, 9.0f, 45.0f }, { 22.5f, 8.0f, 1.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -105.0f, 9.0f, 75.0f }, { 30.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -100.0f, 9.0f, 105.0f }, { 5.0f, 8.0f, 1.0f });

    //Light(s)
    engine->CreateConcaveDrawable("Meshes/lamp.obj", "", { -90.0f, 16.75f, 82.5f });
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
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -9.5f, 3.0f, 10.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_blue.obj", "", { -9.5f, 6.0f, 10.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f,  0.45f, 0.0f }, 1, { 2, 3 }); //interactID 1 = Destroy something, interacts with {1, 2}, aka the door
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -8.0f, 2.0f,  6.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -5.0f, 2.0f, 10.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -4.0f, 2.0f,  6.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -6.0f, 2.0f,  3.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -15.5f, 3.0f, 10.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -14.0f, 2.0f,  6.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -10.0f, 2.0f,  3.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -10.0f, 4.0f,  3.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -12.0f, 2.0f,  0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -0.45f, 0.0f });

    //Clutter on the right side of the door
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { 7.0f, 3.0f, 11.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { 9.0f, 2.0f,  7.0f }, { 1.0f, 1.0f, 1.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { 15.0f, 4.0f,  7.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { 9.0f, 2.0f,  3.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { 6.0f, 2.0f,  5.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { 12.0f, 2.0f,  6.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { 15.0f, 2.0f,  7.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { 12.0f, 2.0f,  3.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { 15.0f, 3.0f, 11.0f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { 15.0f, 6.0f, 11.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    /*
    Room    #1 Completed Clutter
        Drawables   -> 21   (112)
    */

    //SECOND ROOM
    //Clutter on the right side of the door
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -55.0f, 3.0f, 71.5f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -53.0f, 2.5f, 67.5f }, { 1.5f, 1.5f, 1.5f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -53.0f, 5.0f, 67.5f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -56.5f, 2.0f, 66.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -49.0f, 3.0f, 71.5f }, { 2.0f, 2.0f, 2.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -49.0f, 6.0f, 71.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.45f, 0.0f });

    //Attempt: Clutter collections made in blender to save on manually placing all of these with code (Works well enough, things aren't placed or rotated properly though because blender is kinda dogshit when it comes to its axes)
    //Room 2
    engine->CreateDrawable("Meshes/clutter_small_1.obj", "", { -30.0f, 2.0f, 42.5f }, {1.0f, 1.0f, 1.0f}, { 0.0f, XM_PIDIV2, 0.0f }); //Facing +Z
    engine->CreateDrawable("Meshes/clutter_small_2.obj", "", { -5.0f, 2.0f, 67.5f }, { 1.0f, 1.0f, 1.0f }, {0.0f, -XM_PIDIV2, 0.0f}); //Facing -Z
    engine->CreateDrawable("Meshes/clutter_small_3.obj", "", { 13.0f, 2.0f, 66.0f }, { 1.0f, 1.0f, 1.0f }, {0.0f, -XM_PIDIV2, 0.0f}); //Facing -X

    //Clutter room #2 done:
       //Drawables  -> 9    (121)

    //Room 3
    engine->CreateDrawable("Meshes/clutter_medium_1.obj", "", { -45.0f, 2.0f, 130.0f }, { 1.0f, 1.0f, 1.0f }, {0.0f, XM_PI, 0.0f}); //Facing -Z
    engine->CreateDrawable("Meshes/clutter_medium_2.obj", "", {  0.0f, 2.0f, 100.0f }, { 1.0f, 1.0f, 1.0f }); //Facing +Z
    engine->CreateDrawable("Meshes/clutter_medium_3.obj", "", { 46.0f, 2.0f, 77.0f }, { 1.0f, 1.0f, 1.0f }, {0.0f, XM_PI, 0.0f}); //Default: Facing -Z. Flip it to face the other way and put it in the corner under the lighting
    engine->CreateDrawable("Meshes/clutter_small_1.obj", "", { 5.0f, 2.0f, 128.0f }, { 1.0f, 1.0f, 1.0f }, {0.0f, -XM_PIDIV2, 0.0f}); //Default facing -X? Want -Z so negative pidiv2 i think?

    //Drawables     -> 4    (125)

    //Two target boxes in room 3, both of these will have the destroy-interaction and get rid of the other box, as well as both the door in this room and the door in the previous room
    //Door in second room: 23 and 24 (-1 because of indexing stuff)
    //Door in this room: 42 and 43 (--,,--)
    //These two boxes are 126 and 127 (Apparently they are actually 126 and 127 so there's been a miscalc somewhere, oopsie? (Oh yeah, the two new lights I added also have drawable))
    engine->CreateDrawable("Meshes/crate_red.obj", "", { -1.75f, 6.0f, 130.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f,  0.0f, 0.0f }, 1, { 22, 23, 41, 42, 126 });
    engine->CreateDrawable("Meshes/crate_blue.obj", "", { 46.2f, 5.0f, 78.6f }, { 1.0f, 1.0f, 1.0f }, { 0.0f,  0.0f, 0.0f }, 1, { 22, 23, 41, 42, 125 });

    //Hallway 5-ish
    engine->CreateDrawable("Meshes/clutter_small_2.obj", "", { -97.0f, 2.0f, 100.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PI, 0.0f }); //Facing +X
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { -80.0f, 3.0f, 100.0f }, {2.0f, 2.0f, 2.0f});

}

void SetupLevel2(D3D11Engine* engine)
{
    for (int i = 0; i < 6; i++) //6 hallways, 3 color, 3 motion (as a result of the preliminary experiment we skip the form attribute)
    {
        //Floor and roof
        engine->CreateDrawable("Meshes/ground_stone.obj", "", {  0.0f + (60.0f * i),  0.0f, 25.0f + (60.0f * i) }, { 1.5f, 1.0f, 3.0f });
        engine->CreateDrawable("Meshes/ground_stone.obj", "", {  0.0f + (60.0f * i), 18.0f, 25.0f + (60.0f * i) }, { 1.5f, 1.0f, 3.0f });
        engine->CreateDrawable("Meshes/ground_stone.obj", "", { 15.0f + (60.0f * i),  0.0f, 70.0f + (60.0f * i) }, { 1.5f, 1.0f, 3.0f }, { 0.0f, XM_PIDIV2, 0.0f });
        engine->CreateDrawable("Meshes/ground_stone.obj", "", { 15.0f + (60.0f * i), 18.0f, 70.0f + (60.0f * i) }, { 1.5f, 1.0f, 3.0f }, { 0.0f, XM_PIDIV2, 0.0f });

        //Walls (Starting with back wall, then going along the side with the door until we've looped back around)
        engine->CreateDrawable("Meshes/wall_brick.obj", "NormalMaps/brick.jpg", { 0.0f + (60.0f * i),  9.0f, -5.0f + (60.0f * i) }, { 15.0f, 8.0f, 1.0f });
        if (i == 0) //First hallway there won't be a door behind you, you're on your own
        {
            engine->CreateDrawable("Meshes/wall_brick.obj", "", { -15.0f, 9.0f, 17.5f }, { 22.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
            engine->CreateDrawable("Meshes/wall_brick.obj", "", { -15.0f, 9.0f, 62.5f }, { 22.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
        }
        else
        {
            engine->CreateDrawable("Meshes/door_metal.obj", "NormalMaps/metal_long.jpg", { -15.0f + (60.0f * i), 9.0f, 10.0f + (60.0f * i) }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
            engine->CreateDrawable("Meshes/sphere_metal.obj", "NormalMaps/metal.jpg", { -16.25f + (60.0f * i), 9.25f, 12.75f + (60.0f * i) }, { 0.25f, 0.25f, 0.25f }, { 0.0f, XM_PIDIV2, 0.0f });
            engine->CreateDrawable("Meshes/wall_brick.obj", "", { -15.0f + (60.0f * i), 9.0f, 0.0f + (60.0f * i) }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
            engine->CreateDrawable("Meshes/wall_brick.obj", "", { -15.0f + (60.0f * i), 9.0f, 67.5f + (60.0f * i)}, {17.5f, 8.0f, 1.0f}, {0.0f, XM_PIDIV2, 0.0f});
            engine->CreateDrawable("Meshes/wall_brick.obj", "", { -15.0f + (60.0f * i), 9.0f, 32.5f + (60.0f * i) }, { 17.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
        }
        engine->CreateDrawable("Meshes/wall_brick.obj", "", { 0.0f + (60.0f * i) , 9.0f, 85.0f + (60.0f * i) }, { 15.0f, 8.0f, 1.0f });
        engine->CreateDrawable("Meshes/wall_brick.obj", "", { 30.0f + (60.0f * i), 9.0f, 85.0f + (60.0f * i) }, { 15.0f, 8.0f, 1.0f });
        engine->CreateDrawable("Meshes/wall_brick.obj", "", { 30.0f + (60.0f * i), 9.0f, 55.0f + (60.0f * i) }, { 15.0f, 8.0f, 1.0f });
        engine->CreateDrawable("Meshes/wall_brick.obj", "", { 15.0f + (60.0f * i), 9.0f, 40.0f + (60.0f * i) }, { 15.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
        engine->CreateDrawable("Meshes/wall_brick.obj", "", { 15.0f + (60.0f * i), 9.0f, 10.0f + (60.0f * i) }, { 15.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

        //Lights
        engine->CreateConcaveDrawable("Meshes/lamp.obj", "", { 0.0f + (60.0f * i), 16.75f, 25.0f + (60.0f * i) });
        engine->CreateLightPoint({ 0.0f + (60.0f * i), 17.75f, 25.0f + (60.0f * i) }, 3.0f, { 1.0f, 1.0f, 0.5f });
        engine->CreateLightSpot({ 0.0f + (60.0f * i), 16.5f, 25.0f + (60.0f * i) }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
        engine->CreateConcaveDrawable("Meshes/lamp.obj", "", { 17.5f + (60.0f * i), 16.75f, 70.0f + (60.0f * i) });
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
    engine->CreateDrawable("Meshes/door_metal.obj", "", { -15.0f + (360.0f), 9.0f, 10.0f + (360.0f) }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/sphere_metal.obj", "", { -16.25f + (360.0f), 9.25f, 12.75f + (360.0f) }, { 0.25f, 0.25f, 0.25f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -15.0f + (360.0f), 9.0f, 0.0f + (360.0f) }, { 5.0f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/wall_brick.obj", "", { -15.0f + (360.0f), 9.0f, 32.5f + (360.0f) }, { 17.5f, 8.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Targets start here, after 102 drawables
 
    //Red Box in the first hallway          (103)
    engine->CreateDrawable("Meshes/crate_red.obj", "NormalMaps/wood_invert.jpg", { 4.5f, 6.0f, 74.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1, { 19, 20 });

    //Bouncing Box in the second hallway    (104)
    engine->CreateDrawable("Meshes/crate_wood.obj", "NormalMaps/wood_invert.jpg", { 19.0f + 60.0f, 5.0f, 73.5f + 60.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1, { 36, 37 });     //Bounce from 5 to 10 and back

    //Blue Box in the third hallway         (105)
    engine->CreateDrawable("Meshes/crate_blue.obj", "NormalMaps/wood_invert.jpg", { 25.0f + 120.0f, 6.0f, 60.0f + 120.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1, { 53, 54 });

    //Sliding Box in the fourth hallway     (106)
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { 11.0f + 180.0f, 6.0f, 22.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1, { 70, 71 });   //Slide from 22 to 30 and back (202 to 210 I think)

    //Glowing Box in the fifth hallway      (107)
    engine->CreateDrawable("Meshes/crate_glow_blue.obj", "NormalMaps/wood_invert.jpg", { -2.95f + 240.0f, 4.0f, 61.9f + 240.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1, { 87, 88 });

    //Swinging Box in the sixth hallway     (108)
    //engine->CreateDrawable("Meshes/crate_wood.obj", { 30.0f + 300.0f, 12.0f, 70.0f + 300.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1, { 99, 100 });   //This one is way more complicated. Swing from a rope or something?
    //engine->CreatePovDrawable("Meshes/crate_wood.obj", { 0.0f, -6.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1, { 99, 100 });
    engine->CreateOrbitDrawable("Meshes/crate_wood.obj", "", { 25.0f + 300.0f, 12.0f, 70.0f + 300.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1, { 99, 100 });

    //Then add clutter, can't do that in the loop since they're supposed to differ between levels (have fun (god I hate this part I'm a PROGRAMMER NOT A LEVEL DESIGNER GRRRAHHHH))
    //At the very least, we can place clutter in relation to the first hallway, and then simply do +60, +120, +180, and +240, to put them in hallways number 2, 3, 4, and 5

    /*////////////
    //////////////
    //HALLWAY #1//
    //////////////
    */////////////
    //Back-wall clutter
    engine->CreateDrawable("Meshes/clutter_small_2.obj", "NormalMaps/wood_invert.jpg", { 3.0f, 2.0f, 2.5f }, { 1.0f, 1.0f, 1.0f }, {0.0f, XM_PIDIV2, 0.0f});

    //Hallway (part 1, left side) clutter
    engine->CreateDrawable("Meshes/clutter_small_1.obj", "NormalMaps/wood_invert.jpg", { -8.0f, 2.0f, 40.0f }, {1.0f, 1.0f, 1.0f}, {0.0f, -XM_PI, 0.0f});

    //Hallway (part 1, right side)
    //Nothing in this hallway

    //Hallway (part 2, far side) clutter
    engine->CreateDrawable("Meshes/clutter_medium_3.obj", "NormalMaps/wood_invert.jpg", { -2.5f, 2.0f, 74.0f });

    //Hallway (part 2, near side) clutter
    engine->CreateDrawable("Meshes/clutter_small_1.obj", "", { 25.0f, 2.0f, 62.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative X by default, rotated to face positive Z
    
    /*////////////
    //////////////
    //HALLWAY #2//
    //////////////
    */////////////
    //Back-wall clutter
    engine->CreateDrawable("Meshes/clutter_small_1.obj", "", { 3.0f + 60.0f, 2.0f, 2.5f + 60.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Hallway (part 1, left side) clutter
    //engine->CreateDrawable("Meshes/clutter_small_2.obj", { -8.0f + 60.0f, 2.0f, 40.0f + 60.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -XM_PI, 0.0f });

    //Hallway (part 1, right side)
    engine->CreateDrawable("Meshes/clutter_medium_2.obj", "NormalMaps/wood_invert.jpg", { 4.5f + 60.0f, 2.0f, 40.0f + 60.0f }, {1.0f, 1.0f, 1.0f}, {0.0f, XM_PIDIV2, 0.0f}); //Faces negative Z by default, rotated to face negative X

    //Hallway (part 2, far side) clutter
    engine->CreateDrawable("Meshes/clutter_small_3.obj", "NormalMaps/wood_invert.jpg", { -8.5f + 60.0f, 2.0f, 79.0f + 60.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/clutter_medium_1.obj", "NormalMaps/wood_invert.jpg", { 25.0f + 60.0f, 2.0f, 80.0f + 60.0f }, {1.0f, 1.0f, 1.0f}, {0.0f, XM_PI, 0.0f}); //Faces positive Z by default, rotated to face negative Z

    //Hallway (part 2, near side) clutter
    //engine->CreateDrawable("Meshes/clutter_small_3.obj", { 25.0f + 60.0f, 2.0f, 65.0f + 60.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative X by default, rotated to face positive Z

    /*////////////
    //////////////
    //HALLWAY #3//
    //////////////
    */////////////
    //Back-wall clutter
    engine->CreateDrawable("Meshes/clutter_medium_1.obj", "", { 0.0f + 120.0f, 2.0f, -1.0f + 120.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f });

    //Hallway (part 1, left side) clutter
    engine->CreateDrawable("Meshes/clutter_small_2.obj", "", { -8.0f + 120.0f, 2.0f, 40.0f + 120.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -XM_PI, 0.0f });

    //Hallway (part 1, right side)
    //engine->CreateDrawable("Meshes/clutter_medium_1.obj", { 4.5f + 120.0f, 2.0f, 40.0f + 120.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative Z by default, rotated to face negative X

    //Hallway (part 2, far side) clutter
    //engine->CreateDrawable("Meshes/clutter_small_3.obj", { -8.5f + 120.0f, 2.0f, 79.0f + 120.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/clutter_medium_2.obj", "", { 0.0f + 120.0f, 2.0f, 80.0f + 120.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PI, 0.0f }); //Faces positive Z by default, rotated to face negative Z

    //Hallway (part 2, near side) clutter
    engine->CreateDrawable("Meshes/clutter_small_2.obj", "", { 25.0f + 120.0f, 2.0f, 64.0f + 120.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative X by default, rotated to face positive Z

    /*////////////
    //////////////
    //HALLWAY #4//
    //////////////
    */////////////
    //Back-wall clutter
    engine->CreateDrawable("Meshes/clutter_small_3.obj", "", { -9.0f + 180.0f, 2.0f, 4.0f + 180.0f }, {1.0f, 1.0f, 1.0f}, {0.0f, XM_PIDIV2, 0.0f});

    //Hallway (part 1, left side) clutter
    engine->CreateDrawable("Meshes/clutter_medium_3.obj", "", { -8.0f + 180.0f, 2.0f, 72.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, {0.0f, XM_PIDIV2, 0.0f});

    //Hallway (part 1, right side)
    //engine->CreateDrawable("Meshes/clutter_medium_1.obj", { 4.5f + 180.0f, 2.0f, 40.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative Z by default, rotated to face negative X
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { 11.0f + 180.0f, 3.0f, 22.0f + 180.0f }, { 2.0f, 2.0f, 2.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { 11.0f + 180.0f, 3.0f, 26.0f + 180.0f }, { 2.0f, 2.0f, 2.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/crate_wood.obj", "", { 11.0f + 180.0f, 3.0f, 30.0f + 180.0f }, { 2.0f, 2.0f, 2.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Hallway (part 2, far side) clutter
    engine->CreateDrawable("Meshes/clutter_small_1.obj", "", { 22.5f + 180.0f, 2.0f, 78.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -XM_PIDIV2, 0.0f });
    //engine->CreateDrawable("Meshes/clutter_medium_2.obj", { 25.0f + 180.0f, 2.0f, 80.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PI, 0.0f }); //Faces positive Z by default, rotated to face negative Z

    //Hallway (part 2, near side) clutter
    //engine->CreateDrawable("Meshes/clutter_small_2.obj", { 25.0f + 180.0f, 2.0f, 64.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative X by default, rotated to face positive Z

    /*////////////
    //////////////
    //HALLWAY #5//
    //////////////
    */////////////
    //Back-wall clutter
    engine->CreateDrawable("Meshes/clutter_small_3.obj", "", { 5.0f + 240.0f, 2.0f, 4.0f + 240.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/clutter_small_1.obj", "", { -10.0f + 240.0f, 2.0f, 3.0f + 240.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Hallway (part 1, left side) clutter
    engine->CreateDrawable("Meshes/clutter_medium_2.obj", "", { -11.0f + 240.0f, 2.0f, 60.0f + 240.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Hallway (part 1, right side)
    engine->CreateDrawable("Meshes/clutter_small_2.obj", "", { 7.0f + 240.0f, 2.0f, 30.0f + 240.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }); //Faces negative Z by default, rotated to face negative X

    //Hallway (part 2, far side) clutter
    //engine->CreateDrawable("Meshes/clutter_medium_2.obj", { 25.0f + 180.0f, 2.0f, 80.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PI, 0.0f }); //Faces positive Z by default, rotated to face negative Z

    //Hallway (part 2, near side) clutter
    engine->CreateDrawable("Meshes/clutter_small_1.obj", "", { 22.5f + 240.0f, 2.0f, 62.0f + 240.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
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
    engine->CreateDrawable("Meshes/clutter_small_1.obj", "", { -11.0f + 300.0f, 2.0f, 81.0f + 300.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });
    engine->CreateDrawable("Meshes/clutter_small_3.obj", "", { -11.0f + 300.0f, 2.0f, 45.0f + 300.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f });

    //Hallway (part 1, right side)
    engine->CreateDrawable("Meshes/clutter_medium_2.obj", "", { 4.5f + 300.0f, 2.0f, 10.0f + 300.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative Z by default, rotated to face negative X

    //Hallway (part 2, far side) clutter
    engine->CreateDrawable("Meshes/clutter_medium_1.obj", "", { 22.5f + 300.0f, 2.0f, 81.0f + 300.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PI, 0.0f });
    //engine->CreateDrawable("Meshes/clutter_medium_2.obj", { 25.0f + 180.0f, 2.0f, 80.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PI, 0.0f }); //Faces positive Z by default, rotated to face negative Z

    //Hallway (part 2, near side) clutter
    //engine->CreateDrawable("Meshes/clutter_small_2.obj", { 25.0f + 180.0f, 2.0f, 64.0f + 180.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, XM_PIDIV2, 0.0f }); //Faces negative X by default, rotated to face positive Z

    //34 crate-type things
    /*for (int i = 103; i < 136; i++)
    {
        engine->ApplyNormalMapToDrawable(i, "NormalMaps/wood_invert.jpg");
    }*/

    /*
    So, after all drawable creations, variables should be:
        total = 135
        instanceCounts[0] = 1
        instances["Meshes/ground_stone.obj"] = 1
        transforms[0] = the transform, I ain't writing it out
        drawables = 1

    But instead, we have:
        
    */

    FinishInstancedLevel(engine);
}

void SetupInstancedLevel(D3D11Engine* engine)
{
    /*
    4225 crates
    400 stone floor tiles
    30 brick walls
    100 spotlights

    Total drawables: 4655
    Average FPS: 69 (in debug mode)
    */

    //400 5x1x5 stone floor tiles (The deferred lighting gets fucked if the drawables don't line up with the walls, what's up with that?????)
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            engine->CreateInstancedDrawable("Meshes/Test/stone.obj", "", { -6.0f + (float)i * 10.0f, 0.0f, 4.0f + (float)j * 10.0f }, { 5.0f, 1.0f, 5.0f });
        }
    }
    //10 1x8x10 brick wall tiles on each side the platform
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            engine->CreateInstancedDrawable("Meshes/Test/wall.obj", "", { -10.0f + (float)i * 198.0f, 9.0f, 9.0f + (float)j * 20.0f}, {1.0f, 8.0f, 10.0f});
        }
    }
    //10 10x8x1 brick wall tiles at the back the platform
    for (int i = 0; i < 10; i++)
    {
        engine->CreateInstancedDrawable("Meshes/Test/wall.obj", "", { -1.0f + (float)i * 20.0f, 9.0f, 198.0f }, { 10.0f, 8.0f, 1.0f });
    }
    //4225 wooden crates across the floor
    for (int i = 0; i < 65; i++)
    {
        for (int j = 0; j < 65; j++)
        {
            engine->CreateInstancedDrawable("Meshes/Test/crate.obj", "", { -6.5f + (float)i * 3.0f, 2.0f, 2.0f + (float)j * 3.0f});
        }
    }

    //Lights
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            engine->CreateLightSpot({ -1.0f + (float)i * 20.0f, 16.0f, 8.0f + (float)j * 20.0f }, 0.75f, 0.0f, 0.5f, { 1.0f, 1.0f, 1.0f });
        }
    }

    FinishInstancedLevel(engine);
}

void SetupInstancedLevel2(D3D11Engine* engine)
{
    //Try adding drawables in random order, mainly to limit test my shitty instancing and mapping (this level isn't meant to look good, we just want it to run)

    engine->CreateInstancedDrawable("Meshes/ground_stone.obj", "", {0.0f, -8.0f, 0.0f});    //Here, the transform stores at 0, which aligns
    engine->CreateInstancedDrawable("Meshes/wall_brick.obj", "", {-10.0f, 0.0f, 0.0f});     //Transform stores at 1, which will be wrong, because the instanced buffers are set up PER INSTANCE
    engine->CreateInstancedDrawable("Meshes/crate_wood.obj", "", {0.0f, 2.0f, 0.0f});
    engine->CreateInstancedDrawable("Meshes/wall_brick.obj", "", { 10.0f, 0.0f, 0.0f });
    engine->CreateInstancedDrawable("Meshes/crate_wood.obj", "", { 0.0f, 4.0f, 0.0f });
    engine->CreateInstancedDrawable("Meshes/ground_stone.obj", "", { 20.0f, -8.0f, 0.0f });

    //The instanced buffers will set things up like:
    //0, 5, (Ground)
    //1, 3, (Walls)
    //2, 4  (Crates)

    //But the transforms are stored linearly, so 0, 1, 2, 3, 4, 5

    //Solution? Right now I map nDrawables to the transform (lol), but i should instead map the instance itself right, but how
    
    //I can't exactly know ahead of time that when the second transform is added, it's later going to be mapped to index 5
    //Can I do a type of "sort" where I pick out transforms just before I send them to the instanced buffer setup?

    engine->CreateLightSpot({0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f);

    FinishInstancedLevel(engine);
}