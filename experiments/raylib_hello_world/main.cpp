//
// Created by Alexander Samarin on 17.07.2021.
//

#include "raylib.h"
#include "spdlog\spdlog.h"

int main()
{
	// Initialization
	//--------------------------------------------------------------------------------------
	const int ScreenWidth = 1280;
	const int ScreenHeight = 800;

	spdlog::info("Raylib hello world started.");

	InitWindow(ScreenWidth, ScreenHeight, "raylib [core] example - basic window");

	SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
	//--------------------------------------------------------------------------------------

	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		// Update
		//----------------------------------------------------------------------------------
		// TODO: Update your variables here
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(RAYWHITE);

		DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	spdlog::info("Raylib hello world closed.");

	return 0;
}
