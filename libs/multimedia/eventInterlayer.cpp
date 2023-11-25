#include "eventInterlayer.h"

void runEventCycle(RenderTarget& renderTarget, EventManager& eventBoy, Widget& drawWidget) {
    auto timerStart = std::chrono::system_clock::now(); 

    while (renderTarget.getRenderWindow()->isOpen())
    {
        auto timerEnd = std::chrono::system_clock::now();
        auto passed   = std::chrono::duration_cast<std::chrono::seconds>(timerEnd - timerStart);

        if (passed.count() >= 1) {
            // renderTarget.getRenderTexture()->clear();

            eventBoy.onClock(passed.count());

            drawWidget.render(&renderTarget);
            renderTarget.displayAll();

            timerStart = timerEnd;
        }

        // event switch
        sf::Event event;
        while (renderTarget.getRenderWindow()->pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                renderTarget.getRenderWindow()->close();
                break;
            case sf::Event::KeyPressed:
                plugin::KeyboardContext keyboard_context;
                keyboard_context.key = (plugin::Key) event.key.code;

                switch (keyboard_context.key)
                {
                case plugin::Key::LShift:
                case plugin::Key::RShift:
                    keyboard_context.shift = true;
                    break;
                case plugin::Key::LAlt:
                case plugin::Key::RAlt:
                    keyboard_context.alt = true;
                    break;
                case plugin::Key::LControl:
                case plugin::Key::RControl:
                    keyboard_context.ctrl = true;
                    break;
                case plugin::Key::Escape:
                    renderTarget.getRenderWindow()->close();
                    break;
                default:
                    break;
                }

                eventBoy.onKeyboardPress(keyboard_context);

                drawWidget.render(&renderTarget);
                renderTarget.displayAll();
                break;
            case sf::Event::KeyReleased:
                eventBoy.onKeyboardRelease({false, false, false, (plugin::Key)(event.key.code)});
                break;

            case sf::Event::MouseButtonPressed: {
                // renderTarget.getRenderTexture()->clear();

                sf::Vector2i mouse_pos = sf::Mouse::getPosition();
                eventBoy.onMousePress({{(double)(mouse_pos.x), (double)(mouse_pos.y)}, (plugin::MouseButton)(event.key.code)});

                drawWidget.render(&renderTarget);
                renderTarget.displayAll();

                break;
            }
            case sf::Event::MouseButtonReleased: {
                // renderTarget.getRenderTexture()->clear();

                sf::Vector2i mouse_pos = sf::Mouse::getPosition();
                eventBoy.onMouseRelease({{(double)(mouse_pos.x), (double)(mouse_pos.y)}, (plugin::MouseButton)(event.mouseButton.button)});

                drawWidget.render(&renderTarget);
                renderTarget.displayAll();

                break;
            }
            case sf::Event::MouseMoved: {
                // renderTarget.getRenderTexture()->clear();

                sf::Vector2i mouse_pos = sf::Mouse::getPosition();
                
                eventBoy.onMouseMove({{(double)(mouse_pos.x), (double)(mouse_pos.y)}, (plugin::MouseButton)(event.mouseButton.button)});

                drawWidget.render(&renderTarget);
                renderTarget.displayAll();

                break;
            }
            }
        }
    }
}