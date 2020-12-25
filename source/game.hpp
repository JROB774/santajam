#pragma once

GLOBAL constexpr int NIGHT_END = 650;
GLOBAL constexpr int EVENING_END = 450;
GLOBAL constexpr int DAY_END = 300;

GLOBAL struct GameState
{
    int turns;
} gGameState;

INTERNAL bool IsDay   ();
INTERNAL bool IsNight ();

INTERNAL void  StartGame ();
INTERNAL void UpdateGame ();
INTERNAL void RenderGame ();
