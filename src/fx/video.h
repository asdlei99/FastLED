#pragma once

#include <stdint.h>

#include "namespace.h"
#include "ref.h"


FASTLED_NAMESPACE_BEGIN

// Forward declare dependencies.
FASTLED_SMART_REF(VideoImpl);
FASTLED_SMART_REF(ByteStream);
FASTLED_SMART_REF(FileHandle);
struct CRGB;

class Video {
public:
    // frameHistoryCount is the number of frames to keep in the buffer after draw. This
    // allows for time based effects like syncing video speed to audio triggers.
    Video();  // Please use FileSytem to construct a Video.
    ~Video();
    Video(const Video&);
    // Api
    void begin(FileHandleRef h, size_t pixelsPerFrame, float fps = 30.0f, size_t frameHistoryCount = 0);
    void beginStream(ByteStreamRef s, size_t pixelsPerFrame, float fps = 30.0f, size_t frameHistoryCount = 0);
    bool draw(uint32_t now, CRGB* leds, uint8_t* alpha = nullptr);
    void end();
    bool rewind();
    // make compatible with if statements
    operator bool() const { return mImpl.get(); }
private:
    VideoImplRef mImpl;
};

FASTLED_NAMESPACE_END

