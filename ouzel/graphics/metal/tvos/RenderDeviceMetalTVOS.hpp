// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef RENDERDEVICEMETALTVOS_HPP
#define RENDERDEVICEMETALTVOS_HPP

#include "core/Setup.h"

#if OUZEL_PLATFORM_TVOS && OUZEL_COMPILE_METAL

#include "graphics/metal/RenderDeviceMetal.hpp"
#include "core/tvos/DisplayLink.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetalTVOS final: public RenderDeviceMetal
        {
            friend Renderer;
        public:
            RenderDeviceMetalTVOS();
            ~RenderDeviceMetalTVOS();

            void renderCallback();

        private:
            void init(Window* newWindow,
                      const Size2& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newDebugRenderer) override;

            DisplayLink displayLink;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // RENDERDEVICEMETALTVOS_HPP
