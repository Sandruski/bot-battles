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

static const F32 PIXELS_PER_METER = 50.0f;

inline F32 PIXELS_TO_METERS(F32 pixels)
{
    return pixels / PIXELS_PER_METER;
}

inline F32 METERS_TO_PIXELS(F32 meters)
{
    return meters * PIXELS_PER_METER;
}

static const I32 SDL_ERROR = -1;
static const I32 SDL_KEY_PRESSED = 1;

static const I32 DEFAULT_INPUT_SIZE = 128;

#endif
