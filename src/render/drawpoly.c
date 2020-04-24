#include "raylib.h"

int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight,
             "raylib [shapes] example - basic shapes drawing");

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update your variables here
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawText("some basic shapes available on raylib", 20, 20, 20, DARKGRAY);

    DrawPoly((Vector2){screenWidth / 4 * 3, 320}, 6, 80, 0, BROWN);

    // NOTE: We draw all LINES based shapes together to optimize internal
    // drawing, this way, all LINES are rendered in a single draw pass
    /*DrawLine(18, 42, screenWidth - 18, 42, BLACK);*/
    /*DrawCircleLines(screenWidth / 4, 340, 80, DARKBLUE);*/
    /*DrawTriangleLines((Vector2){screenWidth / 4 * 3, 160},*/
    /*(Vector2){screenWidth / 4 * 3 - 20, 230},*/
    /*(Vector2){screenWidth / 4 * 3 + 20, 230}, DARKBLUE);*/
    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
