// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef LOADERGLTF_HPP
#define LOADERGLTF_HPP

#include "assets/Loader.hpp"

namespace ouzel
{
    namespace assets
    {
        class LoaderGLTF final: public Loader
        {
        public:
            static const uint32_t TYPE = Loader::SKINNED_MESH;

            explicit LoaderGLTF(Cache& initCache);
            bool loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps = true) override;
        };
    } // namespace assets
} // namespace ouzel

#endif // LOADERGLTF_HPP
