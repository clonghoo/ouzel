// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "core/Setup.h"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "Listener.hpp"
#include "alsa/AudioDeviceALSA.hpp"
#include "core/Engine.hpp"
#include "coreaudio/AudioDeviceCA.hpp"
#include "dsound/AudioDeviceDS.hpp"
#include "empty/AudioDeviceEmpty.hpp"
#include "openal/AudioDeviceAL.hpp"
#include "opensl/AudioDeviceSL.hpp"
#include "xaudio2/AudioDeviceXA2.hpp"
#include "math/MathUtils.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace audio
    {
        std::set<Driver> Audio::getAvailableAudioDrivers()
        {
            static std::set<Driver> availableDrivers;

            if (availableDrivers.empty())
            {
                availableDrivers.insert(Driver::EMPTY);

#if OUZEL_COMPILE_OPENAL
                availableDrivers.insert(Driver::OPENAL);
#endif

#if OUZEL_COMPILE_DIRECTSOUND
                availableDrivers.insert(Driver::DIRECTSOUND);
#endif

#if OUZEL_COMPILE_XAUDIO2
                availableDrivers.insert(Driver::XAUDIO2);
#endif

#if OUZEL_COMPILE_OPENSL
                availableDrivers.insert(Driver::OPENSL);
#endif

#if OUZEL_COMPILE_COREAUDIO
                availableDrivers.insert(Driver::COREAUDIO);
#endif

#if OUZEL_COMPILE_ALSA
                availableDrivers.insert(Driver::ALSA);
#endif
            }

            return availableDrivers;
        }

        static std::unique_ptr<AudioDevice> createAudioDevice(Driver driver, Mixer& mixer, bool debugAudio, Window* window)
        {
            switch (driver)
            {
#if OUZEL_COMPILE_OPENAL
                case Driver::OPENAL:
                    engine->log(Log::Level::INFO) << "Using OpenAL audio driver";
                    return std::unique_ptr<AudioDevice>(new AudioDeviceAL(mixer));
#endif
#if OUZEL_COMPILE_DIRECTSOUND
                case Driver::DIRECTSOUND:
                    engine->log(Log::Level::INFO) << "Using DirectSound audio driver";
                    return std::unique_ptr<AudioDevice>(new AudioDeviceDS(mixer, window));
#endif
#if OUZEL_COMPILE_XAUDIO2
                case Driver::XAUDIO2:
                    engine->log(Log::Level::INFO) << "Using XAudio 2 audio driver";
                    return std::unique_ptr<AudioDevice>(new AudioDeviceXA2(mixer, debugAudio));
#endif
#if OUZEL_COMPILE_OPENSL
                case Driver::OPENSL:
                    engine->log(Log::Level::INFO) << "Using OpenSL ES audio driver";
                    return std::unique_ptr<AudioDevice>(new AudioDeviceSL(mixer));
#endif
#if OUZEL_COMPILE_COREAUDIO
                case Driver::COREAUDIO:
                    engine->log(Log::Level::INFO) << "Using CoreAudio audio driver";
                    return std::unique_ptr<AudioDevice>(new AudioDeviceCA(mixer));
#endif
#if OUZEL_COMPILE_ALSA
                case Driver::ALSA:
                    engine->log(Log::Level::INFO) << "Using ALSA audio driver";
                    return std::unique_ptr<AudioDevice>(new AudioDeviceALSA(mixer));
#endif
                default:
                    engine->log(Log::Level::INFO) << "Not using audio driver";
                    (void)debugAudio;
                    (void)window;
                    return std::unique_ptr<AudioDevice>(new AudioDeviceEmpty(mixer));
            }
        }

        Audio::Audio(Driver driver, bool debugAudio, Window* window):
            device(createAudioDevice(driver, mixer, debugAudio, window)),
            masterMix(*this)
        {
        }

        Audio::~Audio()
        {
        }

        void Audio::update()
        {
            // TODO: handle events from the audio device
        }

        void Audio::deleteObject(uintptr_t objectId)
        {
            Mixer::Command command(Mixer::Command::Type::DELETE_OBJECT);
            command.objectId = objectId;
            mixer.addCommand(command);
        }

        uintptr_t Audio::initBus()
        {
            uintptr_t busId = mixer.getObjectId();
            Mixer::Command command(Mixer::Command::Type::INIT_BUS);
            command.busId = busId;
            mixer.addCommand(command);
            return busId;
        }

        uintptr_t Audio::initProcessor(const std::function<std::unique_ptr<Processor>(void)>& createFunction)
        {
            uintptr_t processorId = mixer.getObjectId();
            Mixer::Command command(Mixer::Command::Type::INIT_PROCESSOR);
            command.processorId = processorId;
            command.createFunction = createFunction;
            mixer.addCommand(command);
            return processorId;
        }

        void Audio::updateProcessor(uintptr_t processorId, const std::function<void(Processor*)>& updateFunction)
        {
            Mixer::Command command(Mixer::Command::Type::UPDATE_PROCESSOR);
            command.processorId = processorId;
            command.updateFunction = updateFunction;
            mixer.addCommand(command);
        }
    } // namespace audio
} // namespace ouzel
