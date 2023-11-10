#include "eventInterlayer.h"

void runEventCycle(RenderTarget& renderTarget, EventManager& eventBoy, Widget& drawWidget) {
    auto timerStart = std::chrono::system_clock::now(); 

    while (renderTarget.getRenderWindow()->isOpen())
    {
        auto timerEnd = std::chrono::system_clock::now();
        auto passed   = std::chrono::duration_cast<std::chrono::seconds>(timerEnd - timerStart);

        if (passed.count() >= 1) {
            // renderTarget.getRenderTexture()->clear();

            eventBoy.onTimerTick(passed.count());

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
                    eventBoy.onKeyPressed(MKeyboard(0, LEFT_KEY));
                if (event.key.code == sf::Keyboard::Right)
                    eventBoy.onKeyPressed(MKeyboard(0, RIGHT_KEY));
                if (event.key.code == sf::Keyboard::Down)
                    eventBoy.onKeyPressed(MKeyboard(0, DOWN_KEY));
                if (event.key.code == sf::Keyboard::Up)
                    eventBoy.onKeyPressed(MKeyboard(0, UP_KEY));
                break;
            case sf::Event::TextEntered:
                // renderTarget.getRenderTexture()->clear();

                eventBoy.onKeyPressed(MKeyboard(event.text.unicode, DEFAULT_KEY));

                drawWidget.render(&renderTarget);
                renderTarget.displayAll();
                break;
            case sf::Event::KeyReleased:
                eventBoy.onKeyReleased(MKeyboard(event.text.unicode, DEFAULT_KEY));
                break;

            case sf::Event::MouseButtonPressed: {
                // renderTarget.getRenderTexture()->clear();
                if (event.mouseButton.button == sf::Mouse::Left) 
                    eventBoy.onMousePressed(MPoint(sf::Mouse::getPosition()), LEFT);
                if (event.mouseButton.button == sf::Mouse::Right)
                    eventBoy.onMousePressed(MPoint(sf::Mouse::getPosition()), RIGHT);

                drawWidget.render(&renderTarget);
                renderTarget.displayAll();

                break;
            }
            case sf::Event::MouseButtonReleased: {
                // renderTarget.getRenderTexture()->clear();
                if (event.mouseButton.button == sf::Mouse::Left)
                    eventBoy.onMouseReleased(MPoint(sf::Mouse::getPosition()), LEFT);
                if (event.mouseButton.button == sf::Mouse::Right)
                    eventBoy.onMouseReleased(MPoint(sf::Mouse::getPosition()), RIGHT);

                drawWidget.render(&renderTarget);
                renderTarget.displayAll();

                break;
            }
            case sf::Event::MouseMoved: {
                // renderTarget.getRenderTexture()->clear();
                eventBoy.onMouseMove(MPoint(sf::Mouse::getPosition()), LEFT);

                drawWidget.render(&renderTarget);
                renderTarget.displayAll();

                break;
            }
            }
        }
    }
}