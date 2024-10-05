#pragma once

#include <stdint.h>
#include "namespace.h"
#include "crgb.h"
#include "fx/fx.h"
#include "ptr.h"
#include "fixed_vector.h"
#include <string.h>

#ifndef FASTLED_FX_ENGINE_MAX_FX
#define FASTLED_FX_ENGINE_MAX_FX 64
#endif

FASTLED_NAMESPACE_BEGIN

class FxEngine {
public:
    FxEngine(uint16_t numLeds);
    ~FxEngine();

    void addFx(Fx* effect);
    //void startTransition(uint32_t now, uint32_t duration);
    void draw(uint32_t now, CRGB* outputBuffer);
    bool nextFx(uint32_t now, uint32_t duration) {
        uint16_t next_index = (mCurrentIndex + 1) % mEffects.size();
        return setNextFx(next_index, now, duration);
    }

    bool setNextFx(uint16_t index, uint32_t now, uint32_t duration) {
        if (index >= mEffects.size() && index != mCurrentIndex) {
            return false;
        }
        if (mIsTransitioning) {
            // If already transitioning, complete the current transition immediately
            mEffects[mCurrentIndex]->pause();
            mCurrentIndex = mNextIndex;
            mIsTransitioning = false;
        }
        mNextIndex = index;
        mEffects[mNextIndex]->resume();
        mIsTransitioning = true;
        mTransition.start(now, duration);
        return true;
    }

private:
    uint16_t mNumLeds;
    FixedVector<Fx*, FASTLED_FX_ENGINE_MAX_FX> mEffects;
    scoped_array<CRGB> mLayer1;
    scoped_array<CRGB> mLayer2;
    bool mIsTransitioning;
    bool mWasTransitioning;
    uint16_t mCurrentIndex;
    uint16_t mNextIndex;
    Transition mTransition;
    void startTransition(uint32_t now, uint32_t duration);

};

inline FxEngine::FxEngine(uint16_t numLeds) 
    : mNumLeds(numLeds), mIsTransitioning(false), mWasTransitioning(false), mCurrentIndex(0), mNextIndex(0) {
    mLayer1.reset(new CRGB[numLeds]);
    mLayer2.reset(new CRGB[numLeds]);
}

inline FxEngine::~FxEngine() {}

inline void FxEngine::addFx(Fx* effect) {
    mEffects.push_back(effect);
}

inline void FxEngine::draw(uint32_t now, CRGB* finalBuffer) {
    if (!mEffects.empty()) {
        Fx::DrawContext context = {now, mLayer1.get()};
        mEffects[mCurrentIndex]->draw(context);
        
        if (mIsTransitioning && mEffects.size() > 1) {
            context = {now, mLayer2.get()};
            mEffects[mNextIndex]->draw(context);
            
            uint8_t progress = mTransition.getProgress(now);
            uint8_t inverse_progress = 255 - progress;

            for (uint16_t i = 0; i < mNumLeds; i++) {
                finalBuffer[i] = mLayer1[i].nscale8(inverse_progress) + mLayer2[i].nscale8(progress);
            }

            if (progress == 255) {
                // Transition complete, update current index
                mEffects[mCurrentIndex]->pause();
                mCurrentIndex = mNextIndex;
                mIsTransitioning = false;
            }
        } else {
            memcpy(finalBuffer, mLayer1.get(), sizeof(CRGB) * mNumLeds);
        }
    }
}

FASTLED_NAMESPACE_END

