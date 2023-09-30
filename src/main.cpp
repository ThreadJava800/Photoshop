#include "includes.h"
#include "ui/window/window.h"

void runMainCycle() {
    sf::RenderWindow window(sf::VideoMode(), "Photoshop", sf::Style::Fullscreen);
    window.setPosition(sf::Vector2i(0, 0));

    Window mainWindow = Window(              MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT), MPoint(window.getSize().x - 2 * MAIN_WIN_BRD_SHIFT, window.getSize().y - 2 * MAIN_WIN_BRD_SHIFT));
    RenderTarget renderTarget = RenderTarget(MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT), MPoint(window.getSize().x - 2 * MAIN_WIN_BRD_SHIFT, window.getSize().y - 2 * MAIN_WIN_BRD_SHIFT), &window);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // close if escape clicked
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                break;
            // case sf::Event::MouseButtonPressed:
            //     if (event.mouseButton.button == sf::Mouse::Left)
            //         bossOfGym.registerClick();
            //     break;
            }
        }

        window.clear();
        mainWindow.render(&renderTarget);
        window.display();
    }
}

int main() {
    runMainCycle();

    return 0;
}