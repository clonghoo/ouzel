// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef TTFONT_HPP
#define TTFONT_HPP

#include "stb_truetype.h"
#include "gui/Font.hpp"

namespace ouzel
{
    class TTFont final: public Font
    {
    public:
        TTFont();
        TTFont(const std::vector<uint8_t>& newData, bool newMipmaps = true);

        void getVertices(const std::string& text,
                         Color color,
                         float fontSize,
                         const Vector2& anchor,
                         std::vector<uint16_t>& indices,
                         std::vector<graphics::Vertex>& vertices,
                         std::shared_ptr<graphics::Texture>& texture) override;

        float getStringWidth(const std::string& text);

    private:
        int16_t getKerningPair(uint32_t, uint32_t);

        stbtt_fontinfo font;
        std::vector<unsigned char> data;
        bool loaded = false;
        bool mipmaps = true;
    };
}

#endif // TTFONT_HPP
