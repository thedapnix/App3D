#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#ifndef UNICODE
#define UNICODE
#endif
#define WIN32_LEAN_AND_MEAN

#include "App.h"

#pragma comment( lib, "user32" )			// link against the win32 library
#pragma comment( lib, "d3d11.lib" )			// direct3D library
#pragma comment( lib, "dxgi.lib" )			// directx graphics interface
#pragma comment( lib, "d3dcompiler.lib" )	// shader compiler

//Performance Optimization ideas:
/*
Tiled Deferred Shading
	Too much to describe in just a quick comment, but it's good light culling (Way better than my quick "is light arbitrary distance away from camera? skip")
Occlusion Culling
	Don't bother rendering things that are occluded from view, such as being entirely behind a wall
ENUM passed through buffer to shaders denoting what kind of drawable something is. 0 = dynamic, so we render all sides, 1 = floor, 2 = ceiling, 3 = right-facing wall, and 4 = left-facing wall
	So on 1 through 4 we only render the parts that have their surface normals pointing up, down, left, and right respectively (Needs to be determined before normal-mapping process)
Emission/glow
	Right now, I double the light calculations necessary to make the mesh of the lamps look better by adding a point light up there, but really what I want is just to make the lamp texture glow
Instancing
	Meshes that are repeated several times should be loaded in only once, then placed into the world "for every instance", reducing the memory cost of storing a bunch of textures
	 Also reduces the bind-calls to the rendering API, since previously those would otherwise have to be called for every mesh
*/

//Feature ideas (In order of importance):
/*
1. Normal Mapping (Sexier textures)
	Gives the illusion of higher poly count, which is peak
2. Rope swinging pendulum motion + drawable moving along the end of the rope (Most important because I need it for my thesis lol)
	Should have the basic functionality from my gun following camera rotation thingy
3. Dynamic Lights (Way more interesting scenes)
	Have a light hanging off the end of the rope instead, I render depth every frame after all, so dynamic lights shouldn't be too difficult?
4. Shooting Guns in first person (Adds meaning to the fact that I've implemented a first person gun into the game in the first place)
	Should be fairly simple, cast ray from camera in the direction of the look-vector
	Add some visual effect like a muzzle flash? Idk vfx might be way outside of my scope 
5. Skeletal Animations (Way more interesting characters)
	I keep pushing them away but I really should try to learn it, no matter how scuffed it'll be
6. ECS (Feels a million times better to work with, and boy I sure hope it doesn't tank my performance)
	Another thing I keep pushing, but this is mainly because I'm in too deep in this version of the engine, will add this from the base when I re-write for version 1.0
*/

//If I manage to get all of these things done, I should be able to use this engine to create 3D FPS games and 2D games
	//Mainly saying 3D FPS games because even if the skeletal animation is scuffed as fuck, it'll work in an FPS, TPS no fucking way

//Normal mapping now completely done
//Started doing instancing and holy shit the performance boost is INSANE. But I need to rework the Drawable-system to be able to pool several drawables into a bigger vertex buffer
//Ideas:
//Wait I think I might be fucking retarded. I only need one vertex-buffer, the only thing I need is a collection of world matrices
//That being said: Change the unordered map keeping track of instances to pair the std::string with a VECTOR of ints, thus we can store every index and access it, while also getting the count from vector.size()

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR pCmdLine,
	_In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//Since the App-class has access to *a lot* of pointers, wrapping its creation and runtime in these brackets allow its destructor to actually happen before we call 
	//_CrtDumpMemoryLeaks() and exit out of the program
	{
		App app(hInstance);
		app.Run();
	}

	_CrtDumpMemoryLeaks();

	return 0;
}