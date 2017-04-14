// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(__OBJC__)
#include <GameController/GameController.h>
typedef GCController* GCControllerPtr;
@class ConnectDelegate;
typedef ConnectDelegate* ConnectDelegatePtr;
#else
#include <objc/objc.h>
#include <objc/NSObjCRuntime.h>
typedef id GCControllerPtr;
typedef id ConnectDelegatePtr;
#endif

#include "input/Input.h"

namespace ouzel
{
    namespace input
    {
        class InputMacOS: public Input
        {
            friend Engine;
        public:
            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;

            virtual void setCursorLocked(bool locked) override;
            virtual bool isCursorLocked() const override;

            virtual void setCursorPosition(const Vector2& position) override;

            virtual void startGamepadDiscovery() override;
            virtual void stopGamepadDiscovery() override;

            void handleGamepadDiscoveryCompleted();
            void handleGamepadConnected(GCControllerPtr controller);
            void handleGamepadDisconnected(GCControllerPtr controller);

            static KeyboardKey convertKeyCode(unsigned short keyCode);
            static uint32_t getModifiers(NSUInteger modifierFlags, NSUInteger pressedMouseButtons);

        protected:
            InputMacOS();

            ConnectDelegatePtr connectDelegate = nullptr;

            bool discovering = false;
            bool cursorVisible = true;
            bool cursorLocked = false;
        };
    } // namespace input
} // namespace ouzel
