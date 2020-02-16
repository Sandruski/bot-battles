#ifndef __UTILS_H__
#define __UTILS_H__

template <class T>
inline void SAFE_DELETE_POINTER(T*& p)
{
    if (p != nullptr) {
        delete p;
        p = nullptr;
    }
}

template <class T>
inline void SAFE_DELETE_ARRAY(T*& p)
{
    if (p != nullptr) {
        delete[] p;
        p = nullptr;
    }
}

inline bool COMPARE_STRINGS(const char* str1, const char* str2)
{
    if (str1 != nullptr && str2 != nullptr) {
        return strcmp(str1, str2) == 0;
    }

    return false;
}

static const I32 SDL_ERROR = -1;
static const I32 SDL_KEY_PRESSED = 1;

static const F32 EPSILON = 0.00001f;
static const F32 PI = 3.14159265f;

inline F32 DEGREES_TO_RADIANS(F32 degrees)
{
    return degrees * PI / 180.0f;
}

inline F32 RADIANS_TO_DEGREES(F32 radians)
{
    return radians * 180.0f / PI;
}

static const F32 FPS_CLIENT = 60.0f;
static const F32 FPS_SERVER = 20.0f;
#ifdef _CLIENT
static const F32 FPS = FPS_CLIENT;
#elif defined(_SERVER)
static const F32 FPS = FPS_SERVER;
#endif

#endif
