#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <chrono>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define MIN_PLAYERS_NUMBER 2
#define MAX_PLAYERS_NUMBER 3

#define PLAYER_1_COLOR {255, 65, 65, 255}
#define PLAYER_2_COLOR {65, 214, 255, 255}
#define PLAYER_3_COLOR {40, 195, 4, 255}
#define MAIN_COLOR {250, 193, 0, 255}

#define BARRIER_WIDTH 1400
#define BARRIER_HEIGHT 750

#define TRACKER_VELOCITY 15
#define TRACKER_ACCELERATION 1
#define TRACKER_WIDTH 30
#define TRACKER_HEIGHT 30

#define ROCKET_VELOCITY 5
#define ROCKET_ACCELERATION 1
#define ROCKET_WIDTH 50
#define ROCKET_HEIGHT 50

#define SPACESHIP_VELOCITY 5
#define SPACESHIP_WIDTH 50
#define SPACESHIP_HEIGHT 50

#define CLOCK_NOW std::chrono::system_clock::now
typedef std::chrono::duration<double> ElapsedTime;
typedef std::chrono::system_clock::time_point Moment;

struct Time
{
    Moment begin;
    Moment end;

    double calculateDuration() const
    {
        ElapsedTime duration = end - begin;
        return duration.count();
    }
};

struct Size
{
    int w;
    int h;

    Size operator+(const Size &_size) const
    {
        Size res = {w + _size.w, h + _size.h};
        return res;
    }
    Size operator-(const Size &_size) const
    {
        Size res = {w - _size.w, h - _size.h};
        return res;
    }
    Size operator*(const int &_num) const
    {
        Size res = {w * _num, h * _num};
        return res;
    }
    Size operator/(const int &_num) const
    {
        if (_num == 0)
        {
            std::cout << "Divide by zero error." << std::endl;
            exit(0);
        }
        Size res = {w / _num, h / _num};
        return res;
    }
};

struct Coordinate
{
    int x;
    int y;

    Coordinate operator+(const Size &_size) const
    {
        Coordinate res = {x + _size.w, y + _size.h};
        return res;
    }
    Coordinate operator-(const Size &_size) const
    {
        Coordinate res = {x - _size.w, y - _size.h};
        return res;
    }
    Coordinate operator+(const Coordinate &_other) const
    {
        Coordinate res = {x + _other.x, y + _other.y};
        return res;
    }
    Coordinate operator-(const Coordinate &_other) const
    {
        Coordinate res = {x - _other.x, y - _other.y};
        return res;
    }
    Coordinate operator*(const int &_num) const
    {
        Coordinate res = {x * _num, y * _num};
        return res;
    }
    Coordinate operator/(const int &_num) const
    {
        if (_num == 0)
        {
            std::cout << "Divide by zero error." << std::endl;
            exit(0);
        }
        Coordinate res = {x / _num, y / _num};
        return res;
    }
    bool operator==(const Coordinate &_other) const
    {
        return (x == _other.x && y ==_other.y);
    }
    double calculateNorm() const
    {
        return sqrt(pow(x, 2) + pow(y, 2));
    }
    void calculateAngle(double &_angle) const
    {
        if (!(x == 0 && y == 0))
        {
            double norm = this->calculateNorm();
            _angle = acos(x / norm);
            if (y < 0)
            {
                _angle = -_angle;
            }
        }
    }
    double calculateAngle() const
    {
        if (!(x == 0 && y == 0))
        {
            double norm = this->calculateNorm();
            double angle = acos(x / norm);
            if (y < 0)
            {
                return -angle;
            }
            else
            {
                return angle;
            }
        }
        else
        {
            return 0.0;
        }
    }
};

struct MovementControl
{
    unsigned char up;
    unsigned char down;
    unsigned char left;
    unsigned char right;
};

struct ShootingControl
{
    unsigned char p1;
    unsigned char p2;
    unsigned char p3;
};

enum Side
{
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    TOTAL_SIDES
};

enum ObjectType
{
    BARRIER,
    ROCKET,
    TRACKER,
    SPACESHIP,
    TOTAL_TYPES,
    NONE_TYPE,
};

enum Player
{
    PLAYER_1,
    PLAYER_2,
    PLAYER_3,
    TOTAL_PLAYERS,
    NONE_PLAYER
};

enum Sound
{
    EXPLOSION_SOUND,
    TRACKED_SOUND,
    BUTTON_SOUND,
    TOTAL_SOUNDS,
    NONE_SOUND
};

enum Music
{
    CHASING_MUSIC,
    TOTAL_MUSICS
};

enum ButtonType
{
    START_BUTTON,
    RESTART_BUTTON,
    RESUME_BUTTON,
    GUIDE_BUTTON,
    EXIT_BUTTON,
    TWO_PLAYER_BUTTON,
    THREE_PLAYER_BUTTON,
    TOTAL_BUTTONS
};

enum Status
{
    BEGIN_STATUS,
    RUNNING_STATUS,
    PAUSE_STATUS,
    END_STATUS,
    GUIDE_STATUS,
    EXIT_STATUS,
    NONE_STATUS
};
class GameObject
{
public:
    GameObject(ObjectType _type= ObjectType::NONE_TYPE, Size _size = {0, 0}, Coordinate _position = {0, 0}, double _angle = 0.0, int _state = 0)
    {
        type = _type;
        position = _position;
        size = _size;
        angle = _angle;
        state = _state;
    }
    virtual ~GameObject() = default;

    ObjectType getType() const;
    Coordinate getPosition() const;
    Size getSize() const;
    double getAngle() const;
    int getState() const;

    void setType(const ObjectType&_type);
    void setPosition(const Coordinate &_position);
    void setSize(const Size &_size);

    virtual Player getDriver() const;

protected:
    ObjectType type;
    Coordinate position;
    Size size;
    double angle;
    int state;
};

class Barrier : public GameObject
{
public:
    static constexpr int STATE_DISABLE = -1;
    static constexpr int STATE_ENABLE_PLAYER_1 = 0;
    static constexpr int STATE_ENABLE_PLAYER_2 = 1;
    static constexpr int STATE_ENABLE_PLAYER_3 = 2;

    Barrier() : GameObject(ObjectType::BARRIER, {BARRIER_WIDTH, BARRIER_HEIGHT}, {(SCREEN_WIDTH - BARRIER_WIDTH) / 2, (SCREEN_HEIGHT - BARRIER_HEIGHT) / 2 - 50}) {}
    virtual ~Barrier() = default;

    void updateState(const Player &_driver);
};

class Button
{
public:
    static constexpr int SPRITE_DISABLE = -1;
    static constexpr int SPRITE_DEFAULT = 0;
    static constexpr int SPRITE_MOUSEOVER = 1;

    Button()
    {
        position = {0, 0};
        size = {0, 0};
        sprite = SPRITE_DEFAULT;
        clicked = false;
    }
    ~Button() = default;

    Coordinate getPosition() const;
    Size getSize() const;
    int getSprite() const;
    bool isClicked() const;

    void setPosition(const Coordinate &_position);
    void setSize(const Size &_size);
    void setSprite(const int &_sprite);

    void handleEvent(SDL_Event *_p_event);

private:
    Coordinate position;
    Size size;
    int sprite;
    bool clicked;
};

class Mixer
{
public:
    static constexpr int MUSIC_DEFAULT_VOLUME = 50;

    Mixer()
    {
        music = std::vector<Mix_Music *>(Music::TOTAL_MUSICS, nullptr);
        sound = std::vector<Mix_Chunk *>(Sound::TOTAL_SOUNDS, nullptr);
    }
    ~Mixer();

    bool init();
    bool load();
    void free();

    void playMusic(const Music &_track) const;
    void stopMustic() const;
    void pauseMusic() const;
    void resumeMusic() const;
    void playSound(const Sound &_chunk) const;

protected:
    std::vector<Mix_Music *> music;
    std::vector<Mix_Chunk *> sound;
};

class Texture
{
public:
    Texture();
    ~Texture();

    void free();

    Size getSize() const;

    bool loadFromFile(SDL_Renderer *_p_renderer, const std::string &_path);
    bool loadFromRenderedText(SDL_Renderer *_p_renderer, TTF_Font *_p_font, const std::string &_textureText, const SDL_Color &_textColor);
    void setColor(const Uint8 &_red, const Uint8 &_green, const Uint8 &_blue) const;
    void setBlendMode(const SDL_BlendMode &_blending) const;
    void setAlpha(const Uint8 &_alpha) const;
    void draw(SDL_Renderer *_p_renderer, const Coordinate &_position, double _angle = 0.0, SDL_Rect *_p_clip = nullptr) const;

private:
    SDL_Texture *p_texture;
    Size size;
};

class Painter
{
public:
    static constexpr int TOTAL_DEFAULT_PLAYGROUND_OBJECTS = 3;
    static constexpr int TITLE_TEXT_SIZE = 200;
    static constexpr int WINNER_TEXT_SIZE = 150;

    Painter()
    {
        p_renderer = nullptr;
        p_titleTextFont = nullptr;
        p_winnerTextFont = nullptr;

        backgroundTexture = Texture();
        menuTitleTexture = Texture();
        miniMenuTitleTexture = Texture();
        fatesText = Texture();
        startMenuTitleTexture = Texture();

        fateTexture = std::vector<Texture>(Player::TOTAL_PLAYERS, Texture());
        playgroundTexture = std::vector<Texture>(ObjectType::TOTAL_TYPES + Player::TOTAL_PLAYERS - 1, Texture());

        miniMenuTexture = std::vector<Texture>(Player::TOTAL_PLAYERS, Texture());
        guideTexture = std::vector<Texture>(Player::TOTAL_PLAYERS, Texture());

        buttonTexture = std::vector<Texture>(ButtonType::TOTAL_BUTTONS, Texture());
    }
    ~Painter();

    bool init(SDL_Window *_p_window);
    bool loadTexture();
    void free();

    void drawBackground() const;

    void drawFate(const Player &_driver, const int &_fate) const;
    void drawPlayground(const std::vector<GameObject *> &_map) const;

    void drawMiniMenu(const Player &_winner, const std::vector<Button> &_buttonMap) const;
    void drawMainMenu(const std::vector<Button> &_buttonMap) const;
    void drawGuideMenu(const std::vector<Button> &_buttonMap) const;
    void drawStartMenu(const std::vector<Button> &_buttonMap) const;

protected:
    SDL_Renderer *p_renderer;
    TTF_Font *p_titleTextFont;
    TTF_Font *p_winnerTextFont;

    Texture backgroundTexture;
    Texture menuTitleTexture;
    Texture miniMenuTitleTexture;
    Texture fatesText;
    Texture startMenuTitleTexture;

    std::vector<Texture> playgroundTexture;
    std::vector<Texture> fateTexture;

    std::vector<Texture> miniMenuTexture;
    std::vector<Texture> guideTexture;

    std::vector<Texture> buttonTexture;
};

class GameUI : public Painter, public Mixer
{
public:
    GameUI() : Painter(), Mixer()
    {
        p_window = nullptr;
        quit = false;

        button = std::vector<Button>(ButtonType::TOTAL_BUTTONS, Button());

        playerNumber = MIN_PLAYERS_NUMBER;
        movementKey =
        {
            {SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D},
            {SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT},
            {SDL_SCANCODE_Y, SDL_SCANCODE_H, SDL_SCANCODE_G, SDL_SCANCODE_J},
            {SDL_SCANCODE_P, SDL_SCANCODE_SEMICOLON, SDL_SCANCODE_L, SDL_SCANCODE_APOSTROPHE}
        };
        ShootingKey =
        {
            SDL_SCANCODE_R,
            SDL_SCANCODE_SLASH,
            SDL_SCANCODE_I,
        };
    }
    ~GameUI();

    bool init();

    bool loadTexture();
    bool loadSound();

    void free();

    const Uint8 *getKeyState() const;
    std::vector<Button> getButtonMap() const;
    std::vector<MovementControl> getMovementKey() const;
    ShootingControl getShootingKey() const;
    int getPlayerNumber() const;
    bool isQuit() const;
    void close();

    void initMainMenuButton();
    void handleMainMenuEvent(Status &_status);

    void initMiniMenuButton();
    void handleMiniMenuEvent(Status &_status);

    void initGuideMenuButton();
    void handleGuideMenuEvent(Status &_status);

    void initStartMenuButton();
    void handleStartMenuEvent(Status &_status);

    void handlePlaygroundEvent(Status &_status);

    void clear() const;
    void present() const;

private:
    SDL_Window *p_window;

    SDL_Event event;
    bool quit;

    std::vector<Button> button;

    int playerNumber;
    std::vector<MovementControl> movementKey;
    ShootingControl ShootingKey;
};

class Spaceship : public GameObject
{
public:
    static constexpr int MAX_FATE = 3;
    static constexpr double SPACESHIP_RESETING_TIME = 1.5;

    static constexpr int STATE_DISABLE = -1;
    static constexpr int STATE_ENABLE_DEFAULT = 0;
    static constexpr int STATE_ENABLE_RESETING = 1;

    Spaceship(const Player &_driver) : GameObject(ObjectType::SPACESHIP, {SPACESHIP_WIDTH, SPACESHIP_HEIGHT}, {0, 0}, -M_PI / 2)
    {
        driver = _driver;
        fate = MAX_FATE;

        key = {0, 0, 0, 0};
        velocityVector= {0, 0};
        velocityValue = SPACESHIP_VELOCITY;
        for (int i = 0; i < Side::TOTAL_SIDES; i++)
        {
            collision[i] = false;
        }

        resetingTime = {CLOCK_NOW(), CLOCK_NOW()};
    }
    virtual ~Spaceship() = default;

    virtual Player getDriver() const override;
    Coordinate getVelocityVector() const;
    int getFate() const;

    void setKey(const MovementControl &_key);

    bool checkCollision_inside(const GameObject *_p_other);
    bool checkCollision_outside(const GameObject *_p_other);
    void handleCollision(const ObjectType &_type, Spaceship *_p_spaceship = nullptr);
    void move(const unsigned char *_p_keyState);

    void reset();

private:
    Player driver;
    int fate;

    MovementControl key;
    Coordinate velocityVector;
    int velocityValue;
    bool collision[Side::TOTAL_SIDES];

    Time resetingTime;
};

class Tracker : public GameObject
{
public:
    static constexpr int STATE_DISABLE = -1;
    static constexpr int STATE_ENABLE_DEFAULT = 0;

    Tracker() : GameObject(ObjectType::TRACKER, {TRACKER_WIDTH, TRACKER_HEIGHT})
    {
        key = {0, 0, 0};
        velocityVector = {0, 0};
        acceleration = 0;

        p_tracking = nullptr;
        p_previousTracking = nullptr;

        for (int i = 0; i < Side::TOTAL_SIDES; i++)
        {
            collision[i] = false;
        }
    }
    virtual ~Tracker() = default;

    Spaceship *getTracking() const;
    void setTrancking(Spaceship *_p_spaceship);

    void setKey(const ShootingControl &_key);

    bool checkCollision_inside(const GameObject *_p_other);
    bool checkCollision_outside(const GameObject *_p_other);
    void handleCollision(const ObjectType &_type, Spaceship *_p_spaceship = nullptr);
    void move(const unsigned char *_p_keyState, const std::vector<Spaceship *> &_p_spaceship);

    void reset(const std::vector<Spaceship *> &_p_spaceship);

private:
    void findTarget(const std::vector<Spaceship *> &_p_spaceship);

    ShootingControl key;
    Coordinate velocityVector;
    int acceleration;

    Spaceship *p_tracking;
    Spaceship *p_previousTracking;

    bool collision[Side::TOTAL_SIDES];
};

class Rocket : public GameObject
{
public:
    static constexpr double ROCKET_LIVING_TIME = 15.0;
    static constexpr double ROCKET_RESETING_TIME = 1.0;
    static constexpr double ROCKET_EXPLOSION_TIME = 0.5;

    static constexpr int STATE_DISABLE = -1;
    static constexpr int STATE_ENABLE_DEFAULT = 0;
    static constexpr int STATE_ENABLE_SIGN = 1;
    static constexpr int STATE_ENABLE_EXPLOSION = 2;

    Rocket() : GameObject(ObjectType::ROCKET, {ROCKET_WIDTH, ROCKET_HEIGHT}, {SCREEN_WIDTH / 2 - ROCKET_WIDTH / 2, SCREEN_HEIGHT * 4})
    {
        velocityVector = {0 , ROCKET_VELOCITY};
        velocityValue = ROCKET_VELOCITY;

        p_chasing = nullptr;
        p_previousChasing = nullptr;

        livingTime = {CLOCK_NOW(), CLOCK_NOW()};
    }
    virtual ~Rocket() = default;

    Coordinate getVelocityVector() const;

    void updateState();
    bool checkCollision_outside(const GameObject *_p_other) const;
    void move(const Tracker *_p_tracker);

    void reset();

private:
    void findTarget(const Tracker *_p_tracker);

    Coordinate velocityVector;
    int velocityValue;

    Spaceship *p_chasing;
    Spaceship *p_previousChasing;

    Time livingTime;
};

class GameEngine
{
public:
    static constexpr int TOTAL_DEFAULT_OBJECTS = 3;

    GameEngine(const int &_playerNumber)
    {
        playerNumber = _playerNumber;

        explosionPosition = {-SCREEN_WIDTH, -SCREEN_HEIGHT}; //TODO: Remove this

        p_spaceship = std::vector<Spaceship *>(_playerNumber, nullptr);
        p_tracker = nullptr;
        p_rocket = nullptr;
        p_barrier = nullptr;
    }
    ~GameEngine() = default;

    void init(const std::vector<MovementControl> &_movementKey, const ShootingControl &_shootingKey);
    void free();

    Player getWinner() const;
    void updateStatus(Status &_status) const;

    void handleMovement(const GameUI *_p_gameUI);
    void handleCollision(const GameUI *_p_gameUI);
    void draw(const GameUI *_p_gameUI);

private:
    int playerNumber;

    Coordinate explosionPosition;

    std::vector<Spaceship *> p_spaceship;
    Tracker *p_tracker;
    Rocket *p_rocket;
    Barrier *p_barrier;
};
