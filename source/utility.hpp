#pragma once

#if defined(BUILD_DEBUG)
#include <assert.h>
#define ASSERT(e) assert(e)
#else
#define ASSERT(e) ((void)0)
#endif

#define PERSISTENT static
#define GLOBAL     static
#define INTERNAL   static

typedef  uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef   int8_t  S8;
typedef  int16_t S16;
typedef  int32_t S32;
typedef  int64_t S64;

union Vec2
{
    struct { float x, y; };
    struct { float r, g; };
    struct { float s, t; };
    float raw[2];

    inline bool operator== (const Vec2& v) { return (x == v.x && y == v.y); }
    inline bool operator!= (const Vec2& v) { return !(operator==(v)); }
};
union Vec3
{
    struct { float x, y, z; };
    struct { float r, g, b; };
    struct { float s, t, p; };
    float raw[3];

    inline bool operator== (const Vec3& v) { return (x == v.x && y == v.y && z == v.z); }
    inline bool operator!= (const Vec3& v) { return !(operator==(v)); }
};
union Vec4
{
    struct { float x, y, z, w; };
    struct { float r, g, b, a; };
    struct { float s, t, p, q; };
    float raw[4];

    inline bool operator== (const Vec4& v) { return (x == v.x && y == v.y && z == v.z && w == v.w); }
    inline bool operator!= (const Vec4& v) { return !(operator==(v)); }
};

struct Rect
{
    float x, y, w, h;

    inline bool operator== (const Rect& r) { return (x == r.x && y == r.y && w == r.w && h == r.h); }
    inline bool operator!= (const Rect& r) { return !(operator==(r)); }
};

// We store colors as four floating-point values whereas SDL handles colors as
// four U8 values. This function is used to map from our color range to SDL's.
// For example the following Color values will map to SDL_Color values like so:
//
//    Vec4 { 1.0, 1.0, 1.0, 1.0 }  ==>  SDL_Color { 255, 255, 255, 255 }
//    Vec4 { 0.5, 0.2, 0.7, 1.0 }  ==>  SDL_Color { 127,  51, 178, 255 }

INTERNAL SDL_Color ColorToSDLColor (Vec4      color);
INTERNAL Vec4      SDLColorToColor (SDL_Color color);

// Alert message box.

enum AlertButtons
{
    ALERT_BUTTONS_YES_NO_CANCEL,
    ALERT_BUTTONS_YES_NO,
    ALERT_BUTTONS_OK,
    ALERT_BUTTONS_OK_CANCEL,
};
enum AlertType
{
    ALERT_TYPE_INFO,
    ALERT_TYPE_WARNING,
    ALERT_TYPE_ERROR,
};
enum AlertResult
{
    ALERT_RESULT_INVALID,
    ALERT_RESULT_CANCEL,
    ALERT_RESULT_OK,
    ALERT_RESULT_NO,
    ALERT_RESULT_YES,
};

INTERNAL AlertResult ShowAlert (std::string title, std::string msg, AlertType type, AlertButtons buttons);

// Some utility functions.

INTERNAL std::vector<std::string> TokenizeString (std::string str, char delim);

INTERNAL std::string FixPathSlashes (std::string path_name);
INTERNAL std::string FormatStringV  (const char* format, va_list args);
INTERNAL std::string FormatTime     (const char* format);

INTERNAL std::string StrUpper (std::string str);
INTERNAL std::string StrLower (std::string str);

INTERNAL float Lerp (float a, float b, float t);
INTERNAL Vec2  Lerp (Vec2  a, Vec2  b, float t);
INTERNAL Vec3  Lerp (Vec3  a, Vec3  b, float t);
INTERNAL Vec4  Lerp (Vec4  a, Vec4  b, float t);
INTERNAL Rect  Lerp (Rect  a, Rect  b, float t);

INTERNAL float DegToRad (float deg);
INTERNAL float RadToDeg (float rad);

INTERNAL Vec2 RotateVec2 (Vec2 vec, float rad);

INTERNAL float SinRange (float min, float max, float t);

INTERNAL float RoundToMultiple (float num, float multiple);

INTERNAL int Distance (int x1, int y1, int x2, int y2);

// Random functions.

INTERNAL void  RandomSeed       (int seed = -1);
INTERNAL int   RandomRange      (int min, int max); // Inclusive!
INTERNAL int   Random           ();
INTERNAL float RandomFloatRange (float min, float max); // Inclusive!
INTERNAL float RandomFloat      ();
