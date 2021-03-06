#pragma once

GLOBAL constexpr int APP_VERSION_MAJOR = 1;
GLOBAL constexpr int APP_VERSION_MINOR = 0;
GLOBAL constexpr int APP_VERSION_PATCH = 0;

enum AppState
{
    APP_STATE_MENU,
    APP_STATE_GAME,
    APP_STATE_LOSE,
    APP_STATE_TOTAL
};

GLOBAL struct Application
{
    AppState state;
    int frame;
    float total_time;
    float delta_time;
    bool debug;
    bool editor;

} gApplication;

INTERNAL void   InitApplication ();
INTERNAL void   QuitApplication ();
INTERNAL void UpdateApplication (float dt);
INTERNAL void RenderApplication (float dt);
