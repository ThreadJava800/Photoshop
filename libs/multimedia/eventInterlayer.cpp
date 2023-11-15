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

            // drawWidget.render(&renderTarget);
            // window.display();

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
                if (event.key.code == sf::Keyboard::Escape)
                    renderTarget.getRenderWindow()->close();
                if (event.key.code == sf::Keyboard::Left)
                    eventBoy.onKeyboardPress({false, false, false, plugin::Key::Left});
                if (event.key.code == sf::Keyboard::Right)
                    eventBoy.onKeyboardPress({false, false, false, plugin::Key::Right});
                if (event.key.code == sf::Keyboard::Down)
                    eventBoy.onKeyboardPress({false, false, false, plugin::Key::Down});
                if (event.key.code == sf::Keyboard::Up)
                    eventBoy.onKeyboardPress({false, false, false, plugin::Key::Up});
                break;
            case sf::Event::TextEntered:
                // renderTarget.getRenderTexture()->clear();

                eventBoy.onKeyboardPress({false, false, false, (plugin::Key)(event.key.code)});

                drawWidget.render(&renderTarget);
                renderTarget.displayAll();
                break;
            case sf::Event::KeyReleased:
                eventBoy.onKeyboardRelease({false, false, false, (plugin::Key)(event.key.code)});
                break;

            case sf::Event::MouseButtonPressed: {
                // renderTarget.getRenderTexture()->clear();

                sf::Vector2i mouse_pos = sf::Mouse::getPosition();
                eventBoy.onMousePress({{(double)(mouse_pos.x), (double)(mouse_pos.y)}, (plugin::MouseButton)(event.mouseButton.button)});

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