// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "NativeWindowTVOS.hpp"
#include "ViewTVOS.h"
#include "graphics/RenderDevice.hpp"
#include "graphics/opengl/tvos/OpenGLView.h"
#include "graphics/metal/tvos/MetalView.h"

@interface ViewController: UIViewController
@end

@implementation ViewController
{
    ouzel::NativeWindowTVOS* window;
}

-(id)initWithWindow:(ouzel::NativeWindowTVOS*)initWindow
{
    if (self = [super init])
        window = initWindow;

    return self;
}

-(void)textFieldDidChange:(__unused id)sender
{
}

-(void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];

    window->handleResize(ouzel::Size2U(static_cast<uint32_t>(size.width),
                                       static_cast<uint32_t>(size.height)));
}
@end

namespace ouzel
{
    NativeWindowTVOS::NativeWindowTVOS(const std::function<void(const Event&)>& initCallback,
                                       const std::string& newTitle,
                                       graphics::Driver graphicsDriver,
                                       bool newHighDpi):
        NativeWindow(initCallback,
                     Size2U(),
                     true,
                     true,
                     true,
                     newTitle,
                     newHighDpi)
    {
        screen = [UIScreen mainScreen];

        window = [[UIWindow alloc] initWithFrame:[screen bounds]];

        viewController = [[[ViewController alloc] initWithWindow:this] autorelease];
        window.rootViewController = viewController;

        CGRect windowFrame = [window bounds];

        size = Size2U(static_cast<uint32_t>(windowFrame.size.width),
                      static_cast<uint32_t>(windowFrame.size.height));

        switch (graphicsDriver)
        {
            case graphics::Driver::EMPTY:
                view = [[ViewTVOS alloc] initWithFrame:windowFrame];
                break;
#if OUZEL_COMPILE_OPENGL
            case graphics::Driver::OPENGL:
                view = [[OpenGLView alloc] initWithFrame:windowFrame];
                break;
#endif
#if OUZEL_COMPILE_METAL
            case graphics::Driver::METAL:
                view = [[MetalView alloc] initWithFrame:windowFrame];
                break;
#endif
            default:
                throw std::runtime_error("Unsupported render driver");
        }

        textField = [[UITextField alloc] init];
        textField.hidden = YES;
        textField.keyboardType = UIKeyboardTypeDefault;
        [textField addTarget:viewController action:@selector(textFieldDidChange:) forControlEvents:UIControlEventEditingChanged];
        [view addSubview:textField];

        viewController.view = view;

        [window makeKeyAndVisible];

        if (highDpi)
        {
            contentScale = static_cast<float>(screen.scale);
            resolution = size * static_cast<uint32_t>(contentScale);
        }
        else
            resolution = size;
    }

    NativeWindowTVOS::~NativeWindowTVOS()
    {
        if (textField) [textField release];
        if (viewController) [viewController release];
        if (view) [view release];
        if (window) [window release];
    }

    void NativeWindowTVOS::executeCommand(const Command& command)
    {
        switch (command.type)
        {
            case Command::Type::CHANGE_SIZE:
                break;
            case Command::Type::CHANGE_FULLSCREEN:
                break;
            case Command::Type::CLOSE:
                break;
            case Command::Type::SET_TITLE:
                break;
            case Command::Type::BRING_TO_FRONT:
                break;
            case Command::Type::SHOW:
                break;
            case Command::Type::HIDE:
                break;
            case Command::Type::MINIMIZE:
                break;
            case Command::Type::MAXIMIZE:
                break;
            case Command::Type::RESTORE:
                break;
            default:
                throw std::runtime_error("Invalid command");
        }
    }

    void NativeWindowTVOS::handleResize(const Size2U& newSize)
    {
        size = newSize;
        resolution = size * contentScale;

        Event sizeChangeEvent(Event::Type::SIZE_CHANGE);
        sizeChangeEvent.size = size;
        sendEvent(sizeChangeEvent);

        Event resolutionChangeEvent(Event::Type::RESOLUTION_CHANGE);
        resolutionChangeEvent.size = resolution;
        sendEvent(resolutionChangeEvent);
    }
}
