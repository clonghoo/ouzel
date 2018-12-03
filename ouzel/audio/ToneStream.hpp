// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_TONESTREAM_HPP
#define OUZEL_AUDIO_TONESTREAM_HPP

#include <cstdint>
#include "Stream.hpp"

namespace ouzel
{
    namespace audio
    {
        class ToneStream final: public Stream
        {
        public:
            void reset() override;

            inline uint32_t getOffset() const { return offset; }
            inline void setOffset(uint32_t newOffset) { offset = newOffset; }

        private:
            uint32_t offset = 0;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_TONESTREAM_HPP