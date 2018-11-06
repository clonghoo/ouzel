// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef RENDERDEVICEMETALMACOS_HPP
#define RENDERDEVICEMETALMACOS_HPP

#include "core/Setup.h"

#if OUZEL_PLATFORM_MACOS && OUZEL_COMPILE_METAL

#if defined(__OBJC__)
#import <CoreVideo/CoreVideo.h>
#import <QuartzCore/QuartzCore.h>
#import <Metal/Metal.h>
typedef CAMetalLayer* CAMetalLayerPtr;
typedef id<MTLDevice> MTLDevicePtr;
typedef MTLRenderPassDescriptor* MTLRenderPassDescriptorPtr;
typedef id<MTLSamplerState> MTLSamplerStatePtr;
typedef id<MTLCommandQueue> MTLCommandQueuePtr;
typedef id<MTLRenderPipelineState> MTLRenderPipelineStatePtr;
typedef id<MTLCommandBuffer> MTLCommandBufferPtr;
typedef id<MTLRenderCommandEncoder> MTLRenderCommandEncoderPtr;
typedef id<MTLTexture> MTLTexturePtr;
typedef id<MTLDepthStencilState> MTLDepthStencilStatePtr;
#else
#include <objc/objc.h>
#include <objc/NSObjCRuntime.h>
typedef void* CVDisplayLinkRef;
typedef id CAMetalLayerPtr;
typedef id MTLDevicePtr;
typedef id MTLRenderPassDescriptorPtr;
typedef id MTLSamplerStatePtr;
typedef id MTLCommandQueuePtr;
typedef id MTLRenderPipelineStatePtr;
typedef id MTLCommandBufferPtr;
typedef id MTLRenderCommandEncoderPtr;
typedef id MTLTexturePtr;
typedef id MTLDepthStencilStatePtr;
#endif

#include "graphics/metal/RenderDeviceMetal.hpp"
#include "events/EventHandler.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetalMacOS final: public RenderDeviceMetal
        {
            friend Renderer;
        public:
            ~RenderDeviceMetalMacOS();

            std::vector<Size2> getSupportedResolutions() const override;

            void renderCallback();

        private:
            RenderDeviceMetalMacOS();
            void init(Window* newWindow,
                      const Size2& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newDebugRenderer) override;

            bool handleWindow(const WindowEvent& event);

            CVDisplayLinkRef displayLink = nullptr;
            EventHandler eventHandler;

            std::atomic_bool running;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // RENDERDEVICEMETALMACOS_HPP
