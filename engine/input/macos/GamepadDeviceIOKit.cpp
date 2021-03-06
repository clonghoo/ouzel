// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <vector>
#include "GamepadDeviceIOKit.hpp"
#include "InputSystemMacOS.hpp"
#include "../GamepadConfig.hpp"
#include "../../utils/Utils.hpp"

namespace ouzel::input::macos
{
    namespace
    {
        constexpr float thumbDeadzone = 0.2F;

        void deviceInput(void* ctx, IOReturn, void*, IOHIDValueRef value)
        {
            auto gamepadDevice = static_cast<GamepadDeviceIOKit*>(ctx);
            gamepadDevice->handleInput(value);
        }
    }

    GamepadDeviceIOKit::GamepadDeviceIOKit(InputSystem& initInputSystem,
                                           DeviceId initId,
                                           IOHIDDeviceRef initDevice):
        GamepadDevice(initInputSystem, initId),
        device(initDevice)
    {
        IOReturn ret;
        if ((ret = IOHIDDeviceOpen(device, kIOHIDOptionsTypeNone)) != kIOReturnSuccess)
            throw std::system_error(ret, getErrorCategory(), "Failed to open HID device");

        const auto productName = static_cast<CFStringRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductKey)));
        if (productName)
        {
            if (const char* deviceName = CFStringGetCStringPtr(productName, kCFStringEncodingUTF8))
                name = deviceName;
            else
            {
                const auto stringLength = CFStringGetLength(productName);
                std::vector<char> temp(static_cast<std::size_t>(CFStringGetMaximumSizeForEncoding(stringLength, kCFStringEncodingUTF8)) + 1);
                if (CFStringGetCString(productName, temp.data(), static_cast<CFIndex>(temp.size()), kCFStringEncodingUTF8))
                    name = temp.data();
            }
        }

        const auto vendor = static_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVendorIDKey)));
        if (!vendor)
            throw std::runtime_error("Failed to get vendor ID");

        std::int32_t vendorId;
        if (!CFNumberGetValue(vendor, kCFNumberSInt32Type, &vendorId))
            throw std::runtime_error("Failed to get vendor ID");

        const auto product = static_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductIDKey)));
        if (!product)
            throw std::runtime_error("Failed to get product ID");

        std::int32_t productId;
        if (!CFNumberGetValue(product, kCFNumberSInt32Type, &productId))
            throw std::runtime_error("Failed to get product ID");

        const auto& gamepadConfig = getGamepadConfig(vendorId, productId);

        static const std::unordered_map<std::uint32_t, std::size_t> axisUsageMap = {
            {kHIDUsage_GD_X, 0},
            {kHIDUsage_GD_Y, 1},
            {kHIDUsage_GD_Z, 2},
            {kHIDUsage_GD_Rx, 3},
            {kHIDUsage_GD_Ry, 4},
            {kHIDUsage_GD_Rz, 5}
        };

        const auto elementArray = IOHIDDeviceCopyMatchingElements(device, nullptr, kIOHIDOptionsTypeNone);
        const auto count = CFArrayGetCount(elementArray);

        for (CFIndex i = 0; i < count; ++i)
        {
            auto getElement = [](CFArrayRef elementArray, CFIndex i) noexcept {
                auto arrayValue = CFArrayGetValueAtIndex(elementArray, i);
                return bitCast<IOHIDElementRef>(arrayValue);
            };

            const auto element = getElement(elementArray, i);
            const auto type = IOHIDElementGetType(element);
            const auto usagePage = IOHIDElementGetUsagePage(element);
            const auto usage = IOHIDElementGetUsage(element);

            if (usage == kHIDUsage_GD_Hatswitch)
                hatElement = element;

            if (usage >= kHIDUsage_Button_1 && usage < kHIDUsage_Button_1 + 24)
            {
                Button button;
                button.button = gamepadConfig.buttonMap[usage - kHIDUsage_Button_1];
                buttons.insert(std::make_pair(element, button));
            }

            if ((type == kIOHIDElementTypeInput_Misc || type == kIOHIDElementTypeInput_Axis) &&
                usagePage == kHIDPage_GenericDesktop)
            {
                auto usageIterator = axisUsageMap.find(usage);

                if (usageIterator != axisUsageMap.end())
                {
                    const std::size_t index = usageIterator->second;

                    Axis axis;
                    axis.axis = gamepadConfig.axisMap[index];
                    axis.min = IOHIDElementGetLogicalMin(element);
                    axis.max = IOHIDElementGetLogicalMax(element);
                    axis.range = axis.max - axis.min;

                    switch (gamepadConfig.axisMap[index])
                    {
                        case Gamepad::Axis::none:
                            break;
                        case Gamepad::Axis::leftThumbX:
                            axis.negativeButton = Gamepad::Button::leftThumbLeft;
                            axis.positiveButton = Gamepad::Button::leftThumbRight;
                            break;
                        case Gamepad::Axis::leftThumbY:
                            axis.negativeButton = Gamepad::Button::leftThumbUp;
                            axis.positiveButton = Gamepad::Button::leftThumbDown;
                            break;
                        case Gamepad::Axis::rightThumbX:
                            axis.negativeButton = Gamepad::Button::rightThumbLeft;
                            axis.positiveButton = Gamepad::Button::rightThumbRight;
                            break;
                        case Gamepad::Axis::rightThumbY:
                            axis.negativeButton = Gamepad::Button::rightThumbUp;
                            axis.positiveButton = Gamepad::Button::rightThumbDown;
                            break;
                        case Gamepad::Axis::leftTrigger:
                            axis.negativeButton = Gamepad::Button::leftTrigger;
                            axis.positiveButton = Gamepad::Button::leftTrigger;
                            hasLeftTrigger = true;
                            break;
                        case Gamepad::Axis::rightTrigger:
                            axis.negativeButton = Gamepad::Button::rightTrigger;
                            axis.positiveButton = Gamepad::Button::rightTrigger;
                            hasRightTrigger = true;
                            break;
                    }

                    axes.insert(std::make_pair(element, axis));
                }
            }
        }

        CFRelease(elementArray);

        IOHIDDeviceRegisterInputValueCallback(device, deviceInput, this);
    }

    void GamepadDeviceIOKit::handleInput(IOHIDValueRef value)
    {
        const auto element = IOHIDValueGetElement(value);
        const auto newValue = IOHIDValueGetIntegerValue(value);

        if (element == hatElement)
        {
            const auto oldHatValue = static_cast<std::uint32_t>(hatValue);
            const std::uint32_t oldBitmask = (oldHatValue >= 8) ? 0 : (1 << (oldHatValue / 2)) | // first bit
                (1 << (oldHatValue / 2 + oldHatValue % 2)) % 4; // second bit

            const std::uint32_t newBitmask = (newValue >= 8) ? 0 : (1 << (newValue / 2)) | // first bit
            (1 << (newValue / 2 + newValue % 2)) % 4; // second bit

            if ((oldBitmask & 0x01) != (newBitmask & 0x01))
                handleButtonValueChange(Gamepad::Button::dPadUp,
                                        (newBitmask & 0x01) > 0,
                                        (newBitmask & 0x01) > 0 ? 1.0F : 0.0F);
            if ((oldBitmask & 0x02) != (newBitmask & 0x02))
                handleButtonValueChange(Gamepad::Button::dPadRight,
                                        (newBitmask & 0x02) > 0,
                                        (newBitmask & 0x02) > 0 ? 1.0F : 0.0F);
            if ((oldBitmask & 0x04) != (newBitmask & 0x04))
                handleButtonValueChange(Gamepad::Button::dPadDown,
                                        (newBitmask & 0x04) > 0,
                                        (newBitmask & 0x04) > 0 ? 1.0F : 0.0F);
            if ((oldBitmask & 0x08) != (newBitmask & 0x08))
                handleButtonValueChange(Gamepad::Button::dPadLeft,
                                        (newBitmask & 0x08) > 0,
                                        (newBitmask & 0x08) > 0 ? 1.0F : 0.0F);

            hatValue = newValue;
        }

        auto buttonIterator = buttons.find(element);

        if (buttonIterator != buttons.end())
        {
            Button& button = buttonIterator->second;

            if ((button.button != Gamepad::Button::leftTrigger || !hasLeftTrigger) &&
                (button.button != Gamepad::Button::rightTrigger || !hasRightTrigger))
                handleButtonValueChange(button.button, newValue > 0, (newValue > 0) ? 1.0F : 0.0F);

            button.value = newValue;
        }

        auto axisIterator = axes.find(element);

        if (axisIterator != axes.end())
        {
            Axis& axis = axisIterator->second;

            handleAxisChange(axis.value,
                                newValue,
                                axis.min, axis.range,
                                axis.negativeButton, axis.positiveButton);

            axis.value = newValue;
        }
    }

    void GamepadDeviceIOKit::handleAxisChange(CFIndex oldValue, CFIndex newValue,
                                                CFIndex min, CFIndex range,
                                                Gamepad::Button negativeButton,
                                                Gamepad::Button positiveButton)
    {
        if (negativeButton == positiveButton)
        {
            const auto floatValue = static_cast<float>(newValue - min) / range;

            handleButtonValueChange(negativeButton,
                                    floatValue > 0.0F,
                                    floatValue);
        }
        else
        {
            const auto floatValue = 2.0F * static_cast<float>(newValue - min) / range - 1.0F;

            if (floatValue > 0.0F)
                handleButtonValueChange(positiveButton,
                                        floatValue > thumbDeadzone,
                                        floatValue);
            else if (floatValue < 0.0F)
                handleButtonValueChange(negativeButton,
                                        -floatValue > thumbDeadzone,
                                        -floatValue);
            else // thumbstick is 0
            {
                if (oldValue > newValue)
                    handleButtonValueChange(positiveButton, false, 0.0F);
                else
                    handleButtonValueChange(negativeButton, false, 0.0F);
            }
        }
    }
}
