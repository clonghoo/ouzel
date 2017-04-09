// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererMetalTVOS.h"
#include "MetalView.h"
#include "core/tvos/WindowTVOS.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        bool RendererMetalTVOS::init(Window* newWindow,
                                     const Size2& newSize,
                                     uint32_t newSampleCount,
                                     Texture::Filter newTextureFilter,
                                     uint32_t newMaxAnisotropy,
                                     PixelFormat newBackBufferFormat,
                                     bool newVerticalSync,
                                     bool newDepth)
        {
            if (!RendererMetal::init(newWindow, newSize, newSampleCount,
                                     newTextureFilter, newMaxAnisotropy,
                                     newBackBufferFormat, newVerticalSync,
                                     newDepth))
            {
                return false;
            }

            MetalView* view = (MetalView*)static_cast<WindowTVOS*>(newWindow)->getNativeView();

            metalLayer = (CAMetalLayer*)view.layer;
            metalLayer.device = device;
            colorFormat = metalLayer.pixelFormat;

            metalLayer.contentsScale = newWindow->getContentScale();

            return true;
        }
    } // namespace graphics
} // namespace ouzel
