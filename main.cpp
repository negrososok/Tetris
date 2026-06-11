#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>

const int COLS = 10; // Ширина поля
const int ROWS = 20; // Висота поля
const int CELL_SIZE = 30; // Розмір кубика

struct Point {
    int x, y;
};

// Шаблони фігур
const Point SHAPES[7][4] = {
    { {0, 1}, {1, 1}, {2, 1}, {3, 1} },
    { {1, 0}, {2, 0}, {1, 1}, {2, 1} },
    { {1, 0}, {0, 1}, {1, 1}, {2, 1} },
    { {1, 0}, {2, 0}, {0, 1}, {1, 1} },
    { {0, 0}, {1, 0}, {1, 1}, {2, 1} },
    { {0, 0}, {0, 1}, {1, 1}, {2, 1} },
    { {2, 0}, {0, 1}, {1, 1}, {2, 1} }
};

// Кольори фігур
const sf::Color COLORS[8] = {
    sf::Color(35, 35, 35),
    sf::Color(0, 188, 212),
    sf::Color(255, 235, 59),
    sf::Color(156, 39, 176),
    sf::Color(76, 175, 80),
    sf::Color(244, 67, 54),
    sf::Color(33, 150, 243),
    sf::Color(255, 152, 0)
};

int grid[ROWS][COLS] = {0}; // Поле гри
Point current[4]; // Активна фігура
Point next[4]; // Наступна фігура
int currentType = 0; // Тип поточної
int nextType = 0; // Тип наступної
int score = 0; // Очки
int lines = 0; // Лінії
int level = 0; // Рівень
float timer = 0.0f; // Таймер
float delay = 1.0f; // Затримка

bool showMenu = true; // Меню
bool isPaused = false; // Пауза
bool isGameOver = false; // Програш
int selectedMenu = 0; // Вибір меню

// Звуки гри
sf::Sound sndMove;
sf::Sound sndRot;
sf::Sound sndClear;
sf::Sound sndLevel;
sf::Sound sndOver;

// Музика гри
sf::Music menuMusic;
sf::Music music;
bool hasMenuMusic = false;
bool hasMusic = false;
std::string playlist[3] = {
    "sounds/theme1.mp3",
    "sounds/theme2.mp3",
    "sounds/theme3.mp3"
};
int currentTrack = 0;

// Вивід логів
void log(const std::string& msg) {
    std::cout << "[LOG] " << msg << std::endl;
}

// Перевірка колізії
bool check_collision(Point piece[4]) {
    for (int i = 0; i < 4; i++) {
        if (piece[i].x < 0 || piece[i].x >= COLS || piece[i].y >= ROWS) return true;
        if (piece[i].y >= 0 && grid[piece[i].y][piece[i].x] != 0) return true;
    }
    return false;
}

// Нова фігура
void spawn() {
    currentType = nextType;
    nextType = rand() % 7;
    for (int i = 0; i < 4; i++) {
        current[i].x = SHAPES[currentType][i].x + 3;
        current[i].y = SHAPES[currentType][i].y;
        next[i].x = SHAPES[nextType][i].x;
        next[i].y = SHAPES[nextType][i].y;
    }
    if (check_collision(current)) {
        isGameOver = true; // Програш
        log("Game over. Final score: " + std::to_string(score) + ", Level: " + std::to_string(level) + ", Lines: " + std::to_string(lines));
        if (hasMusic) music.stop();
        sndOver.play();
    }
}

// Старт спочатку
void reset() {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            grid[y][x] = 0;
        }
    }
    score = 0;
    lines = 0;
    level = 0;
    delay = 1.0f;
    timer = 0.0f;
    isGameOver = false;
    showMenu = false;
    isPaused = false;

    log("Game started");

    if (hasMenuMusic) menuMusic.stop();

    currentTrack = rand() % 3;
    hasMusic = music.openFromFile(playlist[currentTrack]);
    if (hasMusic) {
        music.setLoop(false);
        music.setVolume(20.0f);
        music.play();
    }

    spawn();
}

int main() {
    srand(time(0));

    sf::RenderWindow window(sf::VideoMode(720, 660), "Tetris", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60); // Обмеження FPS

    sf::Font font; // Шрифт
    font.loadFromFile("C:\\Windows\\Fonts\\times.ttf");

    // Завантаження звуків
    sf::SoundBuffer bufMove, bufRot, bufClear, bufLevel, bufOver;
    bufMove.loadFromFile("sounds/move.mp3");
    bufRot.loadFromFile("sounds/rotate.mp3");
    bufClear.loadFromFile("sounds/clear.mp3");
    bufLevel.loadFromFile("sounds/levelup.mp3");
    bufOver.loadFromFile("sounds/gameover.mp3");

    sndMove.setBuffer(bufMove);
    sndRot.setBuffer(bufRot);
    sndClear.setBuffer(bufClear);
    sndLevel.setBuffer(bufLevel);
    sndOver.setBuffer(bufOver);

    sndRot.setVolume(30.0f);
    sndMove.setVolume(50.0f);

    hasMenuMusic = menuMusic.openFromFile("sounds/menu.mp3");
    if (hasMenuMusic) {
        menuMusic.setLoop(true);
        menuMusic.setVolume(20.0f);
    }

    nextType = rand() % 7;
    spawn();

    sf::RectangleShape block(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
    
    // Елементи інтерфейсу
    sf::RectangleShape boardBg(sf::Vector2f(COLS * CELL_SIZE, ROWS * CELL_SIZE));
    boardBg.setFillColor(sf::Color::Black);
    boardBg.setOutlineThickness(3);
    boardBg.setOutlineColor(sf::Color(80, 80, 80));
    boardBg.setPosition(40, 30);

    sf::RectangleShape previewBg(sf::Vector2f(160, 160));
    previewBg.setFillColor(sf::Color::Black);
    previewBg.setOutlineThickness(2);
    previewBg.setOutlineColor(sf::Color(80, 80, 80));
    previewBg.setPosition(440, 30);

    sf::RectangleShape btnStart(sf::Vector2f(260, 52));
    btnStart.setOutlineThickness(1);

    sf::RectangleShape btnExit(sf::Vector2f(260, 52));
    btnExit.setOutlineThickness(1);

    sf::Clock clock; // Годинник

    // Головний цикл
    while (window.isOpen()) {
        float time = clock.restart().asSeconds(); // Час кадру
        if (!showMenu && !isPaused && !isGameOver) {
            timer += time;
        }

        // Зміна треків
        if (!showMenu && !isPaused && !isGameOver) {
            if (hasMusic && music.getStatus() == sf::SoundSource::Stopped) {
                currentTrack = (currentTrack + 1) % 3;
                hasMusic = music.openFromFile(playlist[currentTrack]);
                if (hasMusic) {
                    music.setLoop(false);
                    music.setVolume(20.0f);
                    music.play();
                }
            }
        }

        // Зчитування подій
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                // Керування меню
                if (showMenu) {
                    if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
                        selectedMenu = 1 - selectedMenu;
                        sndMove.play();
                    }
                    if (event.key.code == sf::Keyboard::Enter) {
                        if (selectedMenu == 0) reset();
                        else window.close();
                    }
                    if (event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    }
                }
                // Керування грою
                else {
                    if (event.key.code == sf::Keyboard::P || event.key.code == sf::Keyboard::Escape) {
                        if (!isGameOver) {
                            isPaused = !isPaused; // Пауза
                            if (isPaused) {
                                if (hasMusic) music.pause();
                            } else {
                                if (hasMusic) music.play();
                            }
                        }
                    }
                    if (event.key.code == sf::Keyboard::R) {
                        reset(); // Перезапуск
                    }
                    if ((isPaused || isGameOver) && event.key.code == sf::Keyboard::M) {
                        showMenu = true; // Вихід в меню
                        isPaused = false;
                        isGameOver = false;
                        if (hasMusic) music.stop();
                    }

                    if (!isPaused && !isGameOver) {
                        Point temp[4];
                        for (int i = 0; i < 4; i++) {
                            temp[i] = current[i];
                        }

                        if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
                            for (int i = 0; i < 4; i++) temp[i].x--; // Вліво
                            if (!check_collision(temp)) {
                                for (int i = 0; i < 4; i++) current[i] = temp[i];
                                if (sndLevel.getStatus() != sf::SoundSource::Playing) {
                                    sndMove.play();
                                }
                            }
                        }
                        if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
                            for (int i = 0; i < 4; i++) temp[i].x++; // Вправо
                            if (!check_collision(temp)) {
                                for (int i = 0; i < 4; i++) current[i] = temp[i];
                                if (sndLevel.getStatus() != sf::SoundSource::Playing) {
                                    sndMove.play();
                                }
                            }
                        }
                        if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
                            if (currentType != 1) { // Поворот
                                Point center = temp[1];
                                for (int i = 0; i < 4; i++) {
                                    int rx = temp[i].y - center.y;
                                    int ry = temp[i].x - center.x;
                                    temp[i].x = center.x - rx;
                                    temp[i].y = center.y + ry;
                                }
                                if (check_collision(temp)) {
                                    int offsets[2] = {-1, 1};
                                    for (int offset : offsets) {
                                        Point test[4];
                                        for (int i = 0; i < 4; i++) {
                                            test[i].x = temp[i].x + offset;
                                            test[i].y = temp[i].y;
                                        }
                                        if (!check_collision(test)) {
                                            for (int i = 0; i < 4; i++) temp[i] = test[i];
                                            break;
                                        }
                                    }
                                }
                                if (!check_collision(temp)) {
                                    for (int i = 0; i < 4; i++) current[i] = temp[i];
                                    if (sndLevel.getStatus() != sf::SoundSource::Playing) {
                                        sndRot.play();
                                    }
                                }
                            }
                        }
                        if (event.key.code == sf::Keyboard::Space) {
                            while (true) { // Миттєве падіння
                                for (int i = 0; i < 4; i++) temp[i].y++;
                                if (check_collision(temp)) break;
                                for (int i = 0; i < 4; i++) current[i] = temp[i];
                            }
                            timer = delay;
                        }
                    }
                }
            }
        }

        if (showMenu) {
            if (hasMenuMusic && menuMusic.getStatus() != sf::SoundSource::Playing) {
                menuMusic.play();
            }
        }

        // Швидкість падіння
        if (!showMenu && !isPaused && !isGameOver && (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))) {
            delay = 0.05f; // Прискорення
        } else {
            if (level < 9) {
                delay = 1.0f - level * 0.09f;
            } else {
                float newDelay = 0.15f - (level - 9) * 0.01f;
                if (newDelay < 0.05f) delay = 0.05f;
                else delay = newDelay;
            }
        }

        // Крок вниз
        if (!showMenu && !isPaused && !isGameOver && timer > delay) {
            Point temp[4];
            for (int i = 0; i < 4; i++) temp[i] = current[i];
            for (int i = 0; i < 4; i++) temp[i].y++;

            if (check_collision(temp)) {
                for (int i = 0; i < 4; i++) {
                    if (current[i].y >= 0) {
                        grid[current[i].y][current[i].x] = currentType + 1; // Запис у сітку
                    }
                }

                // Перевірка ліній
                int cleared = 0;
                for (int y = ROWS - 1; y >= 0; y--) {
                    bool full = true;
                    for (int x = 0; x < COLS; x++) {
                        if (grid[y][x] == 0) {
                            full = false;
                            break;
                        }
                    }
                    if (full) {
                        cleared++;
                        for (int dy = y; dy > 0; dy--) {
                            for (int x = 0; x < COLS; x++) {
                                grid[dy][x] = grid[dy - 1][x];
                            }
                        }
                        for (int x = 0; x < COLS; x++) grid[0][x] = 0;
                        y++;
                    }
                }

                // Нарахування очок
                if (cleared > 0) {
                    lines += cleared;
                    int base = 0;
                    if (cleared == 1) base = 40;
                    else if (cleared == 2) base = 100;
                    else if (cleared == 3) base = 300;
                    else if (cleared == 4) base = 1200;

                    score += base * (level + 1);
                    log("Cleared " + std::to_string(cleared) + " lines. Score: " + std::to_string(score));

                    int newLevel = lines / 5;
                    if (newLevel > level) {
                        level = newLevel; // Новий рівень
                        log("Level up: " + std::to_string(level));
                        sndClear.stop();
                        sndMove.stop();
                        sndRot.stop();
                        sndLevel.play();
                    } else {
                        if (sndLevel.getStatus() != sf::SoundSource::Playing) {
                            sndClear.play();
                        }
                    }
                } else {
                    if (sndLevel.getStatus() != sf::SoundSource::Playing) {
                        sndMove.play();
                    }
                }

                spawn();
            } else {
                for (int i = 0; i < 4; i++) current[i] = temp[i];
            }
            timer = 0.0f;
        }

        window.clear(sf::Color::Black); // Очищення екрану

        // Малювання меню
        if (showMenu) {
            block.setFillColor(COLORS[3]);
            block.setPosition(100, 480); window.draw(block);
            block.setPosition(70, 510); window.draw(block);
            block.setPosition(100, 510); window.draw(block);
            block.setPosition(130, 510); window.draw(block);

            block.setFillColor(COLORS[7]);
            block.setPosition(580, 100); window.draw(block);
            block.setPosition(580, 130); window.draw(block);
            block.setPosition(580, 160); window.draw(block);
            block.setPosition(610, 160); window.draw(block);

            block.setFillColor(COLORS[1]);
            block.setPosition(80, 120); window.draw(block);
            block.setPosition(80, 150); window.draw(block);
            block.setPosition(80, 180); window.draw(block);
            block.setPosition(80, 210); window.draw(block);

            std::string letters = "TETRIS";
            sf::Color letterColors[6] = {
                COLORS[5], COLORS[7], COLORS[2], COLORS[4], COLORS[1], COLORS[3]
            };

            for (int i = 0; i < 6; i++) {
                sf::Text titleLetter(std::string(1, letters[i]), font, 70);
                titleLetter.setFillColor(letterColors[i]);
                titleLetter.setStyle(sf::Text::Bold);
                titleLetter.setPosition(200 + i * 55, 120);
                window.draw(titleLetter);
            }

            btnStart.setPosition(720 / 2.0f - 130, 308);
            if (selectedMenu == 0) {
                btnStart.setFillColor(sf::Color(60, 60, 60));
                btnStart.setOutlineColor(sf::Color::White);
            } else {
                btnStart.setFillColor(sf::Color(20, 20, 20));
                btnStart.setOutlineColor(sf::Color(100, 100, 100));
            }
            window.draw(btnStart);

            sf::Text startText("START GAME", font, 22);
            startText.setFillColor(selectedMenu == 0 ? sf::Color::White : sf::Color(150, 150, 150));
            if (selectedMenu == 0) startText.setStyle(sf::Text::Bold);
            sf::FloatRect rS = startText.getLocalBounds();
            startText.setOrigin(rS.left + rS.width / 2.0f, rS.top + rS.height / 2.0f);
            startText.setPosition(720 / 2.0f, 334);
            window.draw(startText);

            btnExit.setPosition(720 / 2.0f - 130, 374);
            if (selectedMenu == 1) {
                btnExit.setFillColor(sf::Color(60, 60, 60));
                btnExit.setOutlineColor(sf::Color::White);
            } else {
                btnExit.setFillColor(sf::Color(20, 20, 20));
                btnExit.setOutlineColor(sf::Color(100, 100, 100));
            }
            window.draw(btnExit);

            sf::Text exitText("EXIT", font, 22);
            exitText.setFillColor(selectedMenu == 1 ? sf::Color::White : sf::Color(150, 150, 150));
            if (selectedMenu == 1) exitText.setStyle(sf::Text::Bold);
            sf::FloatRect rE = exitText.getLocalBounds();
            exitText.setOrigin(rE.left + rE.width / 2.0f, rE.top + rE.height / 2.0f);
            exitText.setPosition(720 / 2.0f, 400);
            window.draw(exitText);

            sf::Text info("W / S or Arrow Keys to Select, Enter to Confirm", font, 13);
            info.setFillColor(sf::Color(90, 90, 90));
            sf::FloatRect rI = info.getLocalBounds();
            info.setOrigin(rI.left + rI.width / 2.0f, rI.top + rI.height / 2.0f);
            info.setPosition(720 / 2.0f, 540);
            window.draw(info);
        }
        // Малювання гри
        else {
            window.draw(boardBg);

            for (int y = 0; y < ROWS; y++) {
                for (int x = 0; x < COLS; x++) {
                    if (grid[y][x] != 0) {
                        block.setFillColor(COLORS[grid[y][x]]);
                        block.setPosition(40 + x * CELL_SIZE + 1, 30 + y * CELL_SIZE + 1);
                        window.draw(block);
                    } else {
                        sf::RectangleShape line(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                        line.setFillColor(sf::Color::Transparent);
                        line.setOutlineThickness(0.5f);
                        line.setOutlineColor(sf::Color(35, 35, 35));
                        line.setPosition(40 + x * CELL_SIZE, 30 + y * CELL_SIZE);
                        window.draw(line);
                    }
                }
            }

            if (!isGameOver) {
                for (int i = 0; i < 4; i++) {
                    if (current[i].y >= 0) {
                        block.setFillColor(COLORS[currentType + 1]);
                        block.setPosition(40 + current[i].x * CELL_SIZE + 1, 30 + current[i].y * CELL_SIZE + 1);
                        window.draw(block);
                    }
                }
            }

            window.draw(previewBg);
            sf::Text nextTxt("NEXT", font, 20);
            nextTxt.setFillColor(sf::Color(160, 160, 160));
            nextTxt.setPosition(440, 5);
            window.draw(nextTxt);

            for (int i = 0; i < 4; i++) {
                block.setFillColor(COLORS[nextType + 1]);
                float px = 440 + 80 + (next[i].x - 1.5f) * CELL_SIZE + 1;
                float py = 30 + 80 + (next[i].y - 0.5f) * CELL_SIZE + 1;
                block.setPosition(px, py);
                window.draw(block);
            }

            sf::Text scoreLbl("SCORE", font, 18);
            scoreLbl.setFillColor(sf::Color(130, 130, 130));
            scoreLbl.setPosition(440, 210);
            window.draw(scoreLbl);

            sf::Text scoreVal(std::to_string(score), font, 28);
            scoreVal.setFillColor(sf::Color::White);
            scoreVal.setPosition(440, 235);
            window.draw(scoreVal);

            sf::Text levelLbl("LEVEL", font, 18);
            levelLbl.setFillColor(sf::Color(130, 130, 130));
            levelLbl.setPosition(440, 290);
            window.draw(levelLbl);

            sf::Text levelVal(std::to_string(level), font, 28);
            levelVal.setFillColor(sf::Color::White);
            levelVal.setPosition(440, 315);
            window.draw(levelVal);

            sf::Text linesLbl("LINES", font, 18);
            linesLbl.setFillColor(sf::Color(130, 130, 130));
            linesLbl.setPosition(440, 370);
            window.draw(linesLbl);

            sf::Text linesVal(std::to_string(lines), font, 28);
            linesVal.setFillColor(sf::Color::White);
            linesVal.setPosition(440, 395);
            window.draw(linesVal);

            sf::Text ctrlLbl("CONTROLS", font, 14);
            ctrlLbl.setFillColor(sf::Color(110, 110, 110));
            ctrlLbl.setPosition(440, 470);
            window.draw(ctrlLbl);

            sf::Text ctrlTxt("A / D / Left / Right - Move\nW / Up - Rotate\nS / Down - Soft Drop\nSpace - Hard Drop\nP / Esc - Pause\nR - Restart", font, 12);
            ctrlTxt.setFillColor(sf::Color(150, 150, 150));
            ctrlTxt.setPosition(440, 495);
            window.draw(ctrlTxt);

            // Пауза
            if (isPaused) {
                sf::RectangleShape overlay(sf::Vector2f(COLS * CELL_SIZE, ROWS * CELL_SIZE));
                overlay.setFillColor(sf::Color(0, 0, 0, 200));
                overlay.setPosition(40, 30);
                window.draw(overlay);

                sf::Text pauseText("PAUSE", font, 36);
                pauseText.setFillColor(sf::Color::White);
                sf::FloatRect rP = pauseText.getLocalBounds();
                pauseText.setOrigin(rP.left + rP.width / 2.0f, rP.top + rP.height / 2.0f);
                pauseText.setPosition(40 + (COLS * CELL_SIZE) / 2.0f, 30 + (ROWS * CELL_SIZE) / 2.0f - 20);
                window.draw(pauseText);

                sf::Text menuText("Press 'M' for Menu", font, 15);
                menuText.setFillColor(sf::Color(200, 200, 200));
                sf::FloatRect rM = menuText.getLocalBounds();
                menuText.setOrigin(rM.left + rM.width / 2.0f, rM.top + rM.height / 2.0f);
                menuText.setPosition(40 + (COLS * CELL_SIZE) / 2.0f, 30 + (ROWS * CELL_SIZE) / 2.0f + 25);
                window.draw(menuText);
            }

            // Game Over
            if (isGameOver) {
                sf::RectangleShape overlay(sf::Vector2f(COLS * CELL_SIZE, ROWS * CELL_SIZE));
                overlay.setFillColor(sf::Color(20, 10, 10, 220));
                overlay.setPosition(40, 30);
                window.draw(overlay);

                sf::Text goText("GAME OVER", font, 36);
                goText.setFillColor(sf::Color(244, 67, 54));
                sf::FloatRect rG = goText.getLocalBounds();
                goText.setOrigin(rG.left + rG.width / 2.0f, rG.top + rG.height / 2.0f);
                goText.setPosition(40 + (COLS * CELL_SIZE) / 2.0f, 30 + (ROWS * CELL_SIZE) / 2.0f - 30);
                window.draw(goText);

                sf::Text rstText("Press 'R' to Restart, 'M' for Menu", font, 15);
                rstText.setFillColor(sf::Color(200, 200, 200));
                sf::FloatRect rR = rstText.getLocalBounds();
                rstText.setOrigin(rR.left + rR.width / 2.0f, rR.top + rR.height / 2.0f);
                rstText.setPosition(40 + (COLS * CELL_SIZE) / 2.0f, 30 + (ROWS * CELL_SIZE) / 2.0f + 20);
                window.draw(rstText);
            }
        }

        window.display(); // Вивід кадру
    }

    return 0;
}
