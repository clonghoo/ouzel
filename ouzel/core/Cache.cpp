// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Cache.hpp"
#include "Engine.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/TextureInterface.hpp"
#include "graphics/ShaderInterface.hpp"
#include "scene/ParticleDefinition.hpp"
#include "scene/SpriteFrame.hpp"
#include "files/FileSystem.hpp"
#include "audio/SoundDataWave.hpp"
#include "audio/SoundDataVorbis.hpp"
#include "gui/BMFont.hpp"
#include "gui/TTFont.hpp"

namespace ouzel
{
    Cache::Cache()
    {
    }

    void Cache::clear()
    {
        releaseTextures();
        releaseShaders();
        releaseParticleDefinitions();
        releaseBlendStates();
        releaseSpriteFrames();
        releaseFonts();
    }

    void Cache::preloadTexture(const std::string& filename, bool dynamic, bool mipmaps)
    {
        std::shared_ptr<graphics::Texture> texture = std::make_shared<graphics::Texture>();
        texture->init(filename, dynamic ? graphics::Texture::DYNAMIC : 0, mipmaps ? 0 : 1);

        textures[filename] = texture;
    }

    const std::shared_ptr<graphics::Texture>& Cache::getTexture(const std::string& filename, bool dynamic, bool mipmaps) const
    {
        auto i = textures.find(filename);

        if (i != textures.end())
        {
            return i->second;
        }
        else
        {
            std::shared_ptr<graphics::Texture> result = std::make_shared<graphics::Texture>();
            result->init(filename, dynamic ? graphics::Texture::DYNAMIC : 0, mipmaps ? 0 : 1);

            i = textures.insert(std::make_pair(filename, result)).first;

            return i->second;
        }
    }

    void Cache::setTexture(const std::string& filename, const std::shared_ptr<graphics::Texture>& texture)
    {
        textures[filename] = texture;
    }

    void Cache::releaseTextures()
    {
        for (auto i = textures.begin(); i != textures.end();)
        {
            // don't delete white pixel texture
            if (i->first == graphics::TEXTURE_WHITE_PIXEL)
            {
                ++i;
            }
            else
            {
                i = textures.erase(i);
            }
        }
    }

    const std::shared_ptr<graphics::Shader>& Cache::getShader(const std::string& shaderName) const
    {
        auto i = shaders.find(shaderName);

        if (i != shaders.end())
        {
            return i->second;
        }
        else
        {
            i = shaders.insert(std::make_pair(shaderName, nullptr)).first;

            return i->second;
        }
    }

    void Cache::setShader(const std::string& shaderName, const std::shared_ptr<graphics::Shader>& shader)
    {
        shaders[shaderName] = shader;
    }

    void Cache::releaseShaders()
    {
        for (auto i = shaders.begin(); i != shaders.end();)
        {
            // don't delete default shaders
            if (i->first == graphics::SHADER_COLOR ||
                i->first == graphics::SHADER_TEXTURE)
            {
                ++i;
            }
            else
            {
                i = shaders.erase(i);
            }
        }
    }

    const std::shared_ptr<graphics::BlendState>& Cache::getBlendState(const std::string& blendStateName) const
    {
        auto i = blendStates.find(blendStateName);

        if (i != blendStates.end())
        {
            return i->second;
        }
        else
        {
            i = blendStates.insert(std::make_pair(blendStateName, nullptr)).first;

            return i->second;
        }
    }

    void Cache::setBlendState(const std::string& blendStateName, const std::shared_ptr<graphics::BlendState>& blendState)
    {
        blendStates[blendStateName] = blendState;
    }

    void Cache::releaseBlendStates()
    {
        for (auto i = blendStates.begin(); i != blendStates.end();)
        {
            // don't delete default blend states
            if (i->first == graphics::BLEND_NO_BLEND ||
                i->first == graphics::BLEND_ADD ||
                i->first == graphics::BLEND_MULTIPLY ||
                i->first == graphics::BLEND_ALPHA)
            {
                ++i;
            }
            else
            {
                i = blendStates.erase(i);
            }
        }
    }

    void Cache::preloadSpriteFrames(const std::string& filename, bool mipmaps,
                                    uint32_t spritesX, uint32_t spritesY,
                                    const Vector2& pivot)
    {
        std::string extension = sharedEngine->getFileSystem()->getExtensionPart(filename);

        std::vector<scene::SpriteFrame> frames;

        if (extension == "json")
        {
            frames = scene::SpriteFrame::loadSpriteFrames(filename, mipmaps);
        }
        else
        {
            std::shared_ptr<graphics::Texture> texture = sharedEngine->getCache()->getTexture(filename, false, mipmaps);

            if (texture)
            {
                Size2 spriteSize = Size2(texture->getSize().v[0] / spritesX, texture->getSize().v[1] / spritesY);

                for (uint32_t x = 0; x < spritesX; ++x)
                {
                    for (uint32_t y = 0; y < spritesY; ++y)
                    {
                        Rectangle rectangle(spriteSize.v[0] * x,
                                            spriteSize.v[1] * y,
                                            spriteSize.v[0],
                                            spriteSize.v[1]);

                        scene::SpriteFrame frame = scene::SpriteFrame(texture, rectangle, false, spriteSize, Vector2(), pivot);
                        frames.push_back(frame);
                    }
                }
            }
        }

        spriteFrames[filename] = frames;
    }

    const std::vector<scene::SpriteFrame>& Cache::getSpriteFrames(const std::string& filename, bool mipmaps,
                                                                  uint32_t spritesX, uint32_t spritesY,
                                                                  const Vector2& pivot) const
    {
        auto i = spriteFrames.find(filename);

        if (i != spriteFrames.end())
        {
            return i->second;
        }
        else
        {
            std::string extension = sharedEngine->getFileSystem()->getExtensionPart(filename);

            std::vector<scene::SpriteFrame> frames;

            if (extension == "json")
            {
                frames = scene::SpriteFrame::loadSpriteFrames(filename, mipmaps);
            }
            else if (spritesX > 0 && spritesY > 0)
            {
                std::shared_ptr<graphics::Texture> texture = sharedEngine->getCache()->getTexture(filename, false, mipmaps);

                if (texture)
                {
                    Size2 spriteSize = Size2(texture->getSize().v[0] / spritesX, texture->getSize().v[1] / spritesY);

                    for (uint32_t x = 0; x < spritesX; ++x)
                    {
                        for (uint32_t y = 0; y < spritesY; ++y)
                        {
                            Rectangle rectangle(spriteSize.v[0] * x,
                                                spriteSize.v[1] * y,
                                                spriteSize.v[0],
                                                spriteSize.v[1]);

                            scene::SpriteFrame frame = scene::SpriteFrame(texture, rectangle, false, spriteSize, Vector2(), pivot);
                            frames.push_back(frame);
                        }
                    }
                }
            }

            i = spriteFrames.insert(std::make_pair(filename, frames)).first;

            return i->second;
        }
    }

    void Cache::setSpriteFrames(const std::string& filename, const std::vector<scene::SpriteFrame>& frames)
    {
        spriteFrames[filename] = frames;
    }

    void Cache::releaseSpriteFrames()
    {
        spriteFrames.clear();
    }

    void Cache::preloadParticleDefinition(const std::string& filename)
    {
        particleDefinitions[filename] = scene::ParticleDefinition::loadParticleDefinition(filename);
    }

    const scene::ParticleDefinition& Cache::getParticleDefinition(const std::string& filename) const
    {
        auto i = particleDefinitions.find(filename);

        if (i != particleDefinitions.end())
        {
            return i->second;
        }
        else
        {
            i = particleDefinitions.insert(std::make_pair(filename, scene::ParticleDefinition::loadParticleDefinition(filename))).first;

            return i->second;
        }
    }

    void Cache::setParticleDefinition(const std::string& filename, const scene::ParticleDefinition& particleDefinition)
    {
        particleDefinitions[filename] = particleDefinition;
    }

    void Cache::releaseParticleDefinitions()
    {
        particleDefinitions.clear();
    }

    void Cache::preloadFont(const std::string& filename, float fontSize, bool mipmaps)
    {
        auto i = fonts.find(filename);

        if (i == fonts.end())
        {
            std::string extension = sharedEngine->getFileSystem()->getExtensionPart(filename);

            if (extension == "fnt")
            {
                fonts[filename] = std::make_shared<BMFont>(filename, mipmaps);
            }
            else if (extension == "ttf")
            {
                fonts[filename] = std::make_shared<TTFont>(filename, fontSize, mipmaps);
            }
        }
    }

    const std::shared_ptr<Font>& Cache::getFont(const std::string& filename, float fontSize, bool mipmaps) const
    {
        auto i = fonts.find(filename);

        if (i != fonts.end())
        {
            return i->second;
        }
        else
        {
            std::string extension = sharedEngine->getFileSystem()->getExtensionPart(filename);

            std::shared_ptr<Font> font;

            if (extension == "fnt")
            {
                font = std::make_shared<BMFont>(filename, mipmaps);
            }
            else if (extension == "ttf")
            {
                font = std::make_shared<TTFont>(filename, fontSize, mipmaps);
            }

            i = fonts.insert(std::make_pair(filename, font)).first;

            return i->second;
        }
    }

    void Cache::setFont(const std::string& filename, const std::shared_ptr<Font>& font)
    {
        fonts[filename] = font;
    }

    void Cache::releaseFonts()
    {
        fonts.clear();
    }

    void Cache::preloadSoundData(const std::string& filename)
    {
        std::string extension = FileSystem::getExtensionPart(filename);

        if (extension == "wav")
        {
            std::shared_ptr<audio::SoundDataWave> newSoundData = std::make_shared<audio::SoundDataWave>();
            newSoundData->init(filename);
            soundData[filename] = newSoundData;
        }
        else if (extension == "ogg")
        {
            std::shared_ptr<audio::SoundDataVorbis> newSoundData = std::make_shared<audio::SoundDataVorbis>();
            newSoundData->init(filename);
            soundData[filename] = newSoundData;
        }
    }

    const std::shared_ptr<audio::SoundData>& Cache::getSoundData(const std::string& filename) const
    {
        auto i = soundData.find(filename);

        if (i != soundData.end())
        {
            return i->second;
        }
        else
        {
            std::shared_ptr<audio::SoundData> newSoundData;

            std::string extension = FileSystem::getExtensionPart(filename);

            if (extension == "wav")
            {
                newSoundData = std::make_shared<audio::SoundDataWave>();
                newSoundData->init(filename);
                soundData[filename] = newSoundData;
            }
            else if (extension == "ogg")
            {
                newSoundData = std::make_shared<audio::SoundDataVorbis>();
                newSoundData->init(filename);
                soundData[filename] = newSoundData;
            }

            i = soundData.insert(std::make_pair(filename, newSoundData)).first;

            return i->second;
        }
    }

    void Cache::setSoundData(const std::string& filename, const std::shared_ptr<audio::SoundData>& newSoundData)
    {
        soundData[filename] = newSoundData;
    }

    void Cache::releaseSoundData()
    {
        soundData.clear();
    }
}
