#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>

const int KOLONKY = 10; // Ширина поля
const int RYADKY = 20; // Висота поля
const int ROZMIR_KLITYNKY = 30; // Розмір кубика

struct Tochka {
    int x, y;
};

// Шаблони фігур
const Tochka FIGURY[7][4] = {
    { {0, 1}, {1, 1}, {2, 1}, {3, 1} },
    { {1, 0}, {2, 0}, {1, 1}, {2, 1} },
    { {1, 0}, {0, 1}, {1, 1}, {2, 1} },
    { {1, 0}, {2, 0}, {0, 1}, {1, 1} },
    { {0, 0}, {1, 0}, {1, 1}, {2, 1} },
    { {0, 0}, {0, 1}, {1, 1}, {2, 1} },
    { {2, 0}, {0, 1}, {1, 1}, {2, 1} }
};

// Кольори фігур
const sf::Color KOLIORY[8] = {
    sf::Color(35, 35, 35),
    sf::Color(0, 188, 212),
    sf::Color(255, 235, 59),
    sf::Color(156, 39, 176),
    sf::Color(76, 175, 80),
    sf::Color(244, 67, 54),
    sf::Color(33, 150, 243),
    sf::Color(255, 152, 0)
};

int sitka[RYADKY][KOLONKY] = {0}; // Поле гри
Tochka potochna[4]; // Активна фігура
Tochka nastupna[4]; // Наступна фігура
int typPotochnoi = 0; // Тип поточної
int typNastupnoi = 0; // Тип наступної
int rahunok = 0; // Очки
int linii = 0; // Лінії
int riven = 0; // Рівень
float taimer = 0.0f; // Таймер
float zatrymka = 1.0f; // Затримка

bool pokazatyMenu = true; // Меню
bool naPauzi = false; // Пауза
bool kinetsHry = false; // Програш
int vybranyjPunkt = 0; // Вибір меню

// Звуки гри
sf::Sound zvukRuhu;
sf::Sound zvukPovorotu;
sf::Sound zvukOchyshchennya;
sf::Sound zvukRivnya;
sf::Sound zvukProhrashu;

// Музика гри
sf::Music muzykaMenu;
sf::Music muzykaHry;
bool yeMuzykaMenu = false;
bool yeMuzykaHry = false;
std::string spisokTrekiv[3] = {
    "sounds/theme1.mp3",
    "sounds/theme2.mp3",
    "sounds/theme3.mp3"
};
int potochnyjTrek = 0;

// Вивід логів
void zapysaty_loh(const std::string& povidomlennya) {
    std::cout << "[LOG] " << povidomlennya << std::endl;
}

// Перевірка колізії
bool perevirka_zitknen(Tochka detal[4]) {
    for (int i = 0; i < 4; i++) {
        if (detal[i].x < 0 || detal[i].x >= KOLONKY || detal[i].y >= RYADKY) return true;
        if (detal[i].y >= 0 && sitka[detal[i].y][detal[i].x] != 0) return true;
    }
    return false;
}

// Нова фігура
void stvoryty_figuru() {
    typPotochnoi = typNastupnoi;
    typNastupnoi = rand() % 7;
    for (int i = 0; i < 4; i++) {
        potochna[i].x = FIGURY[typPotochnoi][i].x + 3;
        potochna[i].y = FIGURY[typPotochnoi][i].y;
        nastupna[i].x = FIGURY[typNastupnoi][i].x;
        nastupna[i].y = FIGURY[typNastupnoi][i].y;
    }
    if (perevirka_zitknen(potochna)) {
        kinetsHry = true; // Програш
        zapysaty_loh("Game over. Final score: " + std::to_string(rahunok) + ", Level: " + std::to_string(riven) + ", Lines: " + std::to_string(linii));
        if (yeMuzykaHry) muzykaHry.stop();
        zvukProhrashu.play();
    }
}

// Старт спочатку
void ochystyty_gru() {
    for (int y = 0; y < RYADKY; y++) {
        for (int x = 0; x < KOLONKY; x++) {
            sitka[y][x] = 0;
        }
    }
    rahunok = 0;
    linii = 0;
    riven = 0;
    zatrymka = 1.0f;
    taimer = 0.0f;
    kinetsHry = false;
    pokazatyMenu = false;
    naPauzi = false;

    zapysaty_loh("Game started");

    if (yeMuzykaMenu) muzykaMenu.stop();

    potochnyjTrek = rand() % 3;
    yeMuzykaHry = muzykaHry.openFromFile(spisokTrekiv[potochnyjTrek]);
    if (yeMuzykaHry) {
        muzykaHry.setLoop(false);
        muzykaHry.setVolume(20.0f);
        muzykaHry.play();
    }

    stvoryty_figuru();
}

int main() {
    srand(time(0));

    sf::RenderWindow vikno(sf::VideoMode(720, 660), "Tetris", sf::Style::Titlebar | sf::Style::Close);
    vikno.setFramerateLimit(60); // Обмеження FPS

    sf::Font shrift; // Шрифт
    shrift.loadFromFile("C:\\Windows\\Fonts\\arial.ttf");

    // Завантаження звуків
    sf::SoundBuffer bufRuhu, bufPovorotu, bufOchyshchennya, bufRivnya, bufProhrashu;
    bufRuhu.loadFromFile("sounds/move.mp3");
    bufPovorotu.loadFromFile("sounds/rotate.mp3");
    bufOchyshchennya.loadFromFile("sounds/clear.mp3");
    bufRivnya.loadFromFile("sounds/levelup.mp3");
    bufProhrashu.loadFromFile("sounds/gameover.mp3");

    zvukRuhu.setBuffer(bufRuhu);
    zvukPovorotu.setBuffer(bufPovorotu);
    zvukOchyshchennya.setBuffer(bufOchyshchennya);
    zvukRivnya.setBuffer(bufRivnya);
    zvukProhrashu.setBuffer(bufProhrashu);

    zvukPovorotu.setVolume(30.0f);
    zvukRuhu.setVolume(50.0f);

    yeMuzykaMenu = muzykaMenu.openFromFile("sounds/menu.mp3");
    if (yeMuzykaMenu) {
        muzykaMenu.setLoop(true);
        muzykaMenu.setVolume(20.0f);
    }

    typNastupnoi = rand() % 7;
    stvoryty_figuru();

    sf::RectangleShape kubik(sf::Vector2f(ROZMIR_KLITYNKY - 2, ROZMIR_KLITYNKY - 2));
    
    // Елементи інтерфейсу
    sf::RectangleShape tloPolya(sf::Vector2f(KOLONKY * ROZMIR_KLITYNKY, RYADKY * ROZMIR_KLITYNKY));
    tloPolya.setFillColor(sf::Color::Black);
    tloPolya.setOutlineThickness(3);
    tloPolya.setOutlineColor(sf::Color(80, 80, 80));
    tloPolya.setPosition(40, 30);

    sf::RectangleShape tloNastupnoi(sf::Vector2f(160, 160));
    tloNastupnoi.setFillColor(sf::Color::Black);
    tloNastupnoi.setOutlineThickness(2);
    tloNastupnoi.setOutlineColor(sf::Color(80, 80, 80));
    tloNastupnoi.setPosition(440, 30);

    sf::RectangleShape knopkaStart(sf::Vector2f(260, 52));
    knopkaStart.setOutlineThickness(2);

    sf::RectangleShape knopkaVihid(sf::Vector2f(260, 52));
    knopkaVihid.setOutlineThickness(2);

    sf::Clock hodynnyk; // Годинник

    // Головний цикл
    while (vikno.isOpen()) {
        float chas = hodynnyk.restart().asSeconds(); // Час кадру
        if (!pokazatyMenu && !naPauzi && !kinetsHry) {
            taimer += chas;
        }

        // Зміна треків
        if (!pokazatyMenu && !naPauzi && !kinetsHry) {
            if (yeMuzykaHry && muzykaHry.getStatus() == sf::SoundSource::Stopped) {
                potochnyjTrek = (potochnyjTrek + 1) % 3;
                yeMuzykaHry = muzykaHry.openFromFile(spisokTrekiv[potochnyjTrek]);
                if (yeMuzykaHry) {
                    muzykaHry.setLoop(false);
                    muzykaHry.setVolume(20.0f);
                    muzykaHry.play();
                }
            }
        }

        // Зчитування подій
        sf::Event podiya;
        while (vikno.pollEvent(podiya)) {
            if (podiya.type == sf::Event::Closed) {
                vikno.close();
            }

            if (podiya.type == sf::Event::KeyPressed) {
                // Керування меню
                if (pokazatyMenu) {
                    if (podiya.key.code == sf::Keyboard::Up || podiya.key.code == sf::Keyboard::W || podiya.key.code == sf::Keyboard::Down || podiya.key.code == sf::Keyboard::S) {
                        vybranyjPunkt = 1 - vybranyjPunkt;
                        zvukRuhu.play();
                    }
                    if (podiya.key.code == sf::Keyboard::Enter) {
                        if (vybranyjPunkt == 0) ochystyty_gru();
                        else vikno.close();
                    }
                    if (podiya.key.code == sf::Keyboard::Escape) {
                        vikno.close();
                    }
                }
                // Керування грою
                else {
                    if (podiya.key.code == sf::Keyboard::P || podiya.key.code == sf::Keyboard::Escape) {
                        if (!kinetsHry) {
                            naPauzi = !naPauzi; // Пауза
                            if (naPauzi) {
                                if (yeMuzykaHry) muzykaHry.pause();
                            } else {
                                if (yeMuzykaHry) muzykaHry.play();
                            }
                        }
                    }
                    if (podiya.key.code == sf::Keyboard::R) {
                        ochystyty_gru(); // Перезапуск
                    }
                    if ((naPauzi || kinetsHry) && podiya.key.code == sf::Keyboard::M) {
                        pokazatyMenu = true; // Вихід в меню
                        naPauzi = false;
                        kinetsHry = false;
                        if (yeMuzykaHry) muzykaHry.stop();
                    }

                    if (!naPauzi && !kinetsHry) {
                        Tochka temp[4];
                        for (int i = 0; i < 4; i++) {
                            temp[i] = potochna[i];
                        }

                        if (podiya.key.code == sf::Keyboard::Left || podiya.key.code == sf::Keyboard::A) {
                            for (int i = 0; i < 4; i++) temp[i].x--; // Вліво
                            if (!perevirka_zitknen(temp)) {
                                for (int i = 0; i < 4; i++) potochna[i] = temp[i];
                                if (zvukRivnya.getStatus() != sf::SoundSource::Playing) {
                                    zvukRuhu.play();
                                }
                            }
                        }
                        if (podiya.key.code == sf::Keyboard::Right || podiya.key.code == sf::Keyboard::D) {
                            for (int i = 0; i < 4; i++) temp[i].x++; // Вправо
                            if (!perevirka_zitknen(temp)) {
                                for (int i = 0; i < 4; i++) potochna[i] = temp[i];
                                if (zvukRivnya.getStatus() != sf::SoundSource::Playing) {
                                    zvukRuhu.play();
                                }
                            }
                        }
                        if (podiya.key.code == sf::Keyboard::Up || podiya.key.code == sf::Keyboard::W) {
                            if (typPotochnoi != 1) { // Поворот
                                Tochka centr = temp[1];
                                for (int i = 0; i < 4; i++) {
                                    int rx = temp[i].y - centr.y;
                                    int ry = temp[i].x - centr.x;
                                    temp[i].x = centr.x - rx;
                                    temp[i].y = centr.y + ry;
                                }
                                if (perevirka_zitknen(temp)) {
                                    int zsuvy[2] = {-1, 1};
                                    for (int zsuv : zsuvy) {
                                        Tochka proba[4];
                                        for (int i = 0; i < 4; i++) {
                                            proba[i].x = temp[i].x + zsuv;
                                            proba[i].y = temp[i].y;
                                        }
                                        if (!perevirka_zitknen(proba)) {
                                            for (int i = 0; i < 4; i++) temp[i] = proba[i];
                                            break;
                                        }
                                    }
                                }
                                if (!perevirka_zitknen(temp)) {
                                    for (int i = 0; i < 4; i++) potochna[i] = temp[i];
                                    if (zvukRivnya.getStatus() != sf::SoundSource::Playing) {
                                        zvukPovorotu.play();
                                    }
                                }
                            }
                        }
                        if (podiya.key.code == sf::Keyboard::Space) {
                            while (true) { // Миттєве падіння
                                for (int i = 0; i < 4; i++) temp[i].y++;
                                if (perevirka_zitknen(temp)) break;
                                for (int i = 0; i < 4; i++) potochna[i] = temp[i];
                            }
                            taimer = zatrymka;
                        }
                    }
                }
            }
        }

        if (pokazatyMenu) {
            if (yeMuzykaMenu && muzykaMenu.getStatus() != sf::SoundSource::Playing) {
                muzykaMenu.play();
            }
        }

        // Швидкість падіння
        if (!pokazatyMenu && !naPauzi && !kinetsHry && (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))) {
            zatrymka = 0.05f; // Прискорення
        } else {
            if (riven < 9) {
                zatrymka = 1.0f - riven * 0.09f;
            } else {
                float novaZatrymka = 0.15f - (riven - 9) * 0.01f;
                if (novaZatrymka < 0.05f) zatrymka = 0.05f;
                else zatrymka = novaZatrymka;
            }
        }

        // Крок вниз
        if (!pokazatyMenu && !naPauzi && !kinetsHry && taimer > zatrymka) {
            Tochka temp[4];
            for (int i = 0; i < 4; i++) temp[i] = potochna[i];
            for (int i = 0; i < 4; i++) temp[i].y++;

            if (perevirka_zitknen(temp)) {
                for (int i = 0; i < 4; i++) {
                    if (potochna[i].y >= 0) {
                        sitka[potochna[i].y][potochna[i].x] = typPotochnoi + 1; // Запис у сітку
                    }
                }

                // Перевірка ліній
                int ochyshcheno = 0;
                for (int y = RYADKY - 1; y >= 0; y--) {
                    bool povnyj = true;
                    for (int x = 0; x < KOLONKY; x++) {
                        if (sitka[y][x] == 0) {
                            povnyj = false;
                            break;
                        }
                    }
                    if (povnyj) {
                        ochyshcheno++;
                        for (int dy = y; dy > 0; dy--) {
                            for (int x = 0; x < KOLONKY; x++) {
                                sitka[dy][x] = sitka[dy - 1][x];
                            }
                        }
                        for (int x = 0; x < KOLONKY; x++) sitka[0][x] = 0;
                        y++;
                    }
                }

                // Нарахування очок
                if (ochyshcheno > 0) {
                    linii += ochyshcheno;
                    int baza = 0;
                    if (ochyshcheno == 1) baza = 40;
                    else if (ochyshcheno == 2) baza = 100;
                    else if (ochyshcheno == 3) baza = 300;
                    else if (ochyshcheno == 4) baza = 1200;

                    rahunok += baza * (riven + 1);
                    zapysaty_loh("Cleared " + std::to_string(ochyshcheno) + " lines. Score: " + std::to_string(rahunok));

                    int novyjRiven = linii / 5;
                    if (novyjRiven > riven) {
                        riven = novyjRiven; // Новий рівень
                        zapysaty_loh("Level up: " + std::to_string(riven));
                        zvukOchyshchennya.stop();
                        zvukRuhu.stop();
                        zvukPovorotu.stop();
                        zvukRivnya.play();
                    } else {
                        if (zvukRivnya.getStatus() != sf::SoundSource::Playing) {
                            zvukOchyshchennya.play();
                        }
                    }
                } else {
                    if (zvukRivnya.getStatus() != sf::SoundSource::Playing) {
                        zvukRuhu.play();
                    }
                }

                stvoryty_figuru();
            } else {
                for (int i = 0; i < 4; i++) potochna[i] = temp[i];
            }
            taimer = 0.0f;
        }

        vikno.clear(sf::Color::Black); // Очищення екрану

        // Малювання меню
        if (pokazatyMenu) {
            kubik.setFillColor(KOLIORY[3]);
            kubik.setPosition(100, 480); vikno.draw(kubik);
            kubik.setPosition(70, 510); vikno.draw(kubik);
            kubik.setPosition(100, 510); vikno.draw(kubik);
            kubik.setPosition(130, 510); vikno.draw(kubik);

            kubik.setFillColor(KOLIORY[7]);
            kubik.setPosition(580, 100); vikno.draw(kubik);
            kubik.setPosition(580, 130); vikno.draw(kubik);
            kubik.setPosition(580, 160); vikno.draw(kubik);
            kubik.setPosition(610, 160); vikno.draw(kubik);

            kubik.setFillColor(KOLIORY[1]);
            kubik.setPosition(80, 120); vikno.draw(kubik);
            kubik.setPosition(80, 150); vikno.draw(kubik);
            kubik.setPosition(80, 180); vikno.draw(kubik);
            kubik.setPosition(80, 210); vikno.draw(kubik);

            std::string litery = "TETRIS";
            sf::Color koloryLiter[6] = {
                KOLIORY[5], KOLIORY[7], KOLIORY[2], KOLIORY[4], KOLIORY[1], KOLIORY[3]
            };

            for (int i = 0; i < 6; i++) {
                sf::Text litera(std::string(1, litery[i]), shrift, 70);
                litera.setFillColor(koloryLiter[i]);
                litera.setStyle(sf::Text::Bold);
                litera.setPosition(200 + i * 55, 120);
                vikno.draw(litera);
            }

            knopkaStart.setPosition(720 / 2.0f - 130, 308);
            if (vybranyjPunkt == 0) {
                knopkaStart.setFillColor(sf::Color(0, 188, 212, 50));
                knopkaStart.setOutlineColor(sf::Color(0, 188, 212));
            } else {
                knopkaStart.setFillColor(sf::Color(30, 30, 30));
                knopkaStart.setOutlineColor(sf::Color(70, 70, 70));
            }
            vikno.draw(knopkaStart);

            sf::Text tekstStart("START GAME", shrift, 22);
            tekstStart.setFillColor(vybranyjPunkt == 0 ? sf::Color::White : sf::Color(100, 100, 100));
            if (vybranyjPunkt == 0) tekstStart.setStyle(sf::Text::Bold);
            sf::FloatRect rS = tekstStart.getLocalBounds();
            tekstStart.setOrigin(rS.left + rS.width / 2.0f, rS.top + rS.height / 2.0f);
            tekstStart.setPosition(720 / 2.0f, 334);
            vikno.draw(tekstStart);

            knopkaVihid.setPosition(720 / 2.0f - 130, 374);
            if (vybranyjPunkt == 1) {
                knopkaVihid.setFillColor(sf::Color(244, 67, 54, 50));
                knopkaVihid.setOutlineColor(sf::Color(244, 67, 54));
            } else {
                knopkaVihid.setFillColor(sf::Color(30, 30, 30));
                knopkaVihid.setOutlineColor(sf::Color(70, 70, 70));
            }
            vikno.draw(knopkaVihid);

            sf::Text tekstVihid("EXIT", shrift, 22);
            tekstVihid.setFillColor(vybranyjPunkt == 1 ? sf::Color::White : sf::Color(100, 100, 100));
            if (vybranyjPunkt == 1) tekstVihid.setStyle(sf::Text::Bold);
            sf::FloatRect rE = tekstVihid.getLocalBounds();
            tekstVihid.setOrigin(rE.left + rE.width / 2.0f, rE.top + rE.height / 2.0f);
            tekstVihid.setPosition(720 / 2.0f, 400);
            vikno.draw(tekstVihid);

            sf::Text pidkazka("W / S or Arrow Keys to Select, Enter to Confirm", shrift, 13);
            pidkazka.setFillColor(sf::Color(90, 90, 90));
            sf::FloatRect rI = pidkazka.getLocalBounds();
            pidkazka.setOrigin(rI.left + rI.width / 2.0f, rI.top + rI.height / 2.0f);
            pidkazka.setPosition(720 / 2.0f, 540);
            vikno.draw(pidkazka);
        }
        // Малювання гри
        else {
            vikno.draw(tloPolya);

            for (int y = 0; y < RYADKY; y++) {
                for (int x = 0; x < KOLONKY; x++) {
                    if (sitka[y][x] != 0) {
                        kubik.setFillColor(KOLIORY[sitka[y][x]]);
                        kubik.setPosition(40 + x * ROZMIR_KLITYNKY + 1, 30 + y * ROZMIR_KLITYNKY + 1);
                        vikno.draw(kubik);
                    } else {
                        sf::RectangleShape sitkaKlitka(sf::Vector2f(ROZMIR_KLITYNKY, ROZMIR_KLITYNKY));
                        sitkaKlitka.setFillColor(sf::Color::Transparent);
                        sitkaKlitka.setOutlineThickness(0.5f);
                        sitkaKlitka.setOutlineColor(sf::Color(35, 35, 35));
                        sitkaKlitka.setPosition(40 + x * ROZMIR_KLITYNKY, 30 + y * ROZMIR_KLITYNKY);
                        vikno.draw(sitkaKlitka);
                    }
                }
            }

            if (!kinetsHry) {
                for (int i = 0; i < 4; i++) {
                    if (potochna[i].y >= 0) {
                        kubik.setFillColor(KOLIORY[typPotochnoi + 1]);
                        kubik.setPosition(40 + potochna[i].x * ROZMIR_KLITYNKY + 1, 30 + potochna[i].y * ROZMIR_KLITYNKY + 1);
                        vikno.draw(kubik);
                    }
                }
            }

            vikno.draw(tloNastupnoi);
            sf::Text nastupnaTxt("NEXT", shrift, 20);
            nastupnaTxt.setFillColor(sf::Color(160, 160, 160));
            nastupnaTxt.setPosition(440, 5);
            vikno.draw(nastupnaTxt);

            for (int i = 0; i < 4; i++) {
                kubik.setFillColor(KOLIORY[typNastupnoi + 1]);
                float px = 440 + 80 + (nastupna[i].x - 1.5f) * ROZMIR_KLITYNKY + 1;
                float py = 30 + 80 + (nastupna[i].y - 0.5f) * ROZMIR_KLITYNKY + 1;
                kubik.setPosition(px, py);
                vikno.draw(kubik);
            }

            sf::Text scoreLbl("SCORE", shrift, 18);
            scoreLbl.setFillColor(sf::Color(130, 130, 130));
            scoreLbl.setPosition(440, 210);
            vikno.draw(scoreLbl);

            sf::Text scoreVal(std::to_string(rahunok), shrift, 28);
            scoreVal.setFillColor(sf::Color::White);
            scoreVal.setPosition(440, 235);
            vikno.draw(scoreVal);

            sf::Text levelLbl("LEVEL", shrift, 18);
            levelLbl.setFillColor(sf::Color(130, 130, 130));
            levelLbl.setPosition(440, 290);
            vikno.draw(levelLbl);

            sf::Text levelVal(std::to_string(riven), shrift, 28);
            levelVal.setFillColor(sf::Color::White);
            levelVal.setPosition(440, 315);
            vikno.draw(levelVal);

            sf::Text linesLbl("LINES", shrift, 18);
            linesLbl.setFillColor(sf::Color(130, 130, 130));
            linesLbl.setPosition(440, 370);
            vikno.draw(linesLbl);

            sf::Text linesVal(std::to_string(linii), shrift, 28);
            linesVal.setFillColor(sf::Color::White);
            linesVal.setPosition(440, 395);
            vikno.draw(linesVal);

            sf::Text ctrlLbl("CONTROLS", shrift, 14);
            ctrlLbl.setFillColor(sf::Color(110, 110, 110));
            ctrlLbl.setPosition(440, 470);
            vikno.draw(ctrlLbl);

            sf::Text ctrlTxt("A / D / Left / Right - Move\nW / Up - Rotate\nS / Down - Soft Drop\nSpace - Hard Drop\nP / Esc - Pause\nR - Restart", shrift, 12);
            ctrlTxt.setFillColor(sf::Color(150, 150, 150));
            ctrlTxt.setPosition(440, 495);
            vikno.draw(ctrlTxt);

            // Пауза
            if (naPauzi) {
                sf::RectangleShape overlay(sf::Vector2f(KOLONKY * ROZMIR_KLITYNKY, RYADKY * ROZMIR_KLITYNKY));
                overlay.setFillColor(sf::Color(0, 0, 0, 200));
                overlay.setPosition(40, 30);
                vikno.draw(overlay);

                sf::Text pauseText("PAUSE", shrift, 36);
                pauseText.setFillColor(sf::Color::White);
                sf::FloatRect rP = pauseText.getLocalBounds();
                pauseText.setOrigin(rP.left + rP.width / 2.0f, rP.top + rP.height / 2.0f);
                pauseText.setPosition(40 + (KOLONKY * ROZMIR_KLITYNKY) / 2.0f, 30 + (RYADKY * ROZMIR_KLITYNKY) / 2.0f - 20);
                vikno.draw(pauseText);

                sf::Text menuText("Press 'M' for Menu", shrift, 15);
                menuText.setFillColor(sf::Color(200, 200, 200));
                sf::FloatRect rM = menuText.getLocalBounds();
                menuText.setOrigin(rM.left + rM.width / 2.0f, rM.top + rM.height / 2.0f);
                menuText.setPosition(40 + (KOLONKY * ROZMIR_KLITYNKY) / 2.0f, 30 + (RYADKY * ROZMIR_KLITYNKY) / 2.0f + 25);
                vikno.draw(menuText);
            }

            // Game Over
            if (kinetsHry) {
                sf::RectangleShape overlay(sf::Vector2f(KOLONKY * ROZMIR_KLITYNKY, RYADKY * ROZMIR_KLITYNKY));
                overlay.setFillColor(sf::Color(20, 10, 10, 220));
                overlay.setPosition(40, 30);
                vikno.draw(overlay);

                sf::Text goText("GAME OVER", shrift, 36);
                goText.setFillColor(sf::Color(244, 67, 54));
                sf::FloatRect rG = goText.getLocalBounds();
                goText.setOrigin(rG.left + rG.width / 2.0f, rG.top + rG.height / 2.0f);
                goText.setPosition(40 + (KOLONKY * ROZMIR_KLITYNKY) / 2.0f, 30 + (RYADKY * ROZMIR_KLITYNKY) / 2.0f - 30);
                vikno.draw(goText);

                sf::Text rstText("Press 'R' to Restart, 'M' for Menu", shrift, 15);
                rstText.setFillColor(sf::Color(200, 200, 200));
                sf::FloatRect rR = rstText.getLocalBounds();
                rstText.setOrigin(rR.left + rR.width / 2.0f, rR.top + rR.height / 2.0f);
                rstText.setPosition(40 + (KOLONKY * ROZMIR_KLITYNKY) / 2.0f, 30 + (RYADKY * ROZMIR_KLITYNKY) / 2.0f + 20);
                vikno.draw(rstText);
            }
        }

        vikno.display(); // Вивід кадру
    }

    return 0;
}
