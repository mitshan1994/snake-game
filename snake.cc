/* 使用SFML来实现一下贪吃蛇(或者部分) */
/* Mit Shan 2017-05-02 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>  // for use of std::pair
#include <string>
#include <iostream>
#include <cstdlib>  // exit
#include <random>
#include <cmath>

#define WIN_WIDTH  640  // 游戏窗口大小
#define WIN_HEIGHT 640
#define GRID_LENGTH 20  // 每个格点的边长
#define HORIZONTAL_BOUND (WIN_WIDTH - GRID_LENGTH) // snake所能到达的边界
#define VERTICAL_BOUND  (WIN_HEIGHT - GRID_LENGTH)
#define SLOW_SPEED (1.0 / 5)
#define FAST_SPEED (1.0 / 10)

bool float_equal(float a, float b)
{
    const float EPSILON = 1e-5;
    if (fabs(a - b) < EPSILON)
        return true;
    return false;
}

/* 实现细节:
       snake_中存储snake的头,身体,每一个部分是一个元素.
       每个位置都表示一个格点左上角的坐标.
       如果snake咬到自己或者撞了墙,设置died_为true,游戏结束.
 */
class SnakeGame {
public:
    SnakeGame();
    void Run();

private:
    void ProcessEvents();
    void Update();
    void Render();

    // 负责画一个格点(头或者身体)
    void DrawOneCell(const std::pair<float, float> &pos, bool is_head);
    void HandleInput(sf::Keyboard::Key key);
    // 将果实(star)放到一个新的位置,并且不和snake身体重合
    void ChangeStar();
    // 给定(x, y),如果是snake的一部分,返回true
    bool InBody(float x, float y);
    // 检查是否game over
    void CheckDied();

private:
    enum Direction { LEFT, UP, RIGHT, DOWN };
        
private:
    std::vector<std::pair<float, float>> snake_;
    Direction next_direction_;
    Direction previous_direction_;
    bool died_;
    sf::Time SECONDS_PER_FRAME;

    sf::Sprite head_;
    sf::Sprite body_;
    sf::Sprite star_;
    sf::Texture head_texture_;
    sf::Texture body_texture_;
    sf::Texture star_texture_;
    sf::RenderWindow window_;
};

SnakeGame::SnakeGame()
    : next_direction_(RIGHT),
      previous_direction_(RIGHT),
      died_(false),
      SECONDS_PER_FRAME(sf::seconds(SLOW_SPEED)),
      window_(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "Snake Game")
{
    snake_.push_back({100, 300});
    snake_.push_back({80, 300});
    snake_.push_back({60, 300});

    std::string head_file_path = "pic/head.png";
    std::string body_file_path = "pic/body.png";
    std::string star_file_path = "pic/star.png";
    if (!head_texture_.loadFromFile(head_file_path))
        std::cout << "Load head failed.\n";
    head_.setTexture(head_texture_);
    if (!body_texture_.loadFromFile(body_file_path))
        std::cout << "Load body failed.\n";
    body_.setTexture(body_texture_);
    if (!star_texture_.loadFromFile(star_file_path))
        std::cout << "Load star failed.\n";
    star_.setTexture(star_texture_);
    ChangeStar();  // 初始随机化位置
}

void SnakeGame::DrawOneCell(const std::pair<float, float> &pos, bool is_head)
{
    sf::Sprite *sprite;
    if (is_head)
        sprite = &head_;
    else
        sprite = &body_;
    sprite->setPosition(pos.first + 1, pos.second + 1);
    window_.draw(*sprite);
}

void SnakeGame::Render()
{
    if (died_) {
        // 如果已经die了
        if (snake_.size() < 10)
            std::cout << "You lost.\n";
        else if (snake_.size() < 20)
            std::cout << "Great!\n";
        else
            std::cout << "Amazing!\n";
        std::cout << "Final length: " << snake_.size() << std::endl;
        window_.close();
        return;
    }
    window_.clear();
    // DrawOneCell(std::pair<float, float>(200.0, 200.0), true);
    // DrawOneCell(std::pair<float, float>(240.0, 200.0), false);
    auto it = snake_.begin();
    if (it != snake_.end())
        DrawOneCell(*it, true);
    while (++it != snake_.end())
        DrawOneCell(*it, false);
    window_.draw(star_);
    window_.display();
}

void SnakeGame::Update()
{
    auto &first = snake_[0];  // snake's head
    std::pair<float, float> new_head = first;
    if (next_direction_ - previous_direction_ == -2 ||
        next_direction_ - previous_direction_ == 2) {
        next_direction_ = previous_direction_;
    }
    if (next_direction_ == LEFT)
        new_head.first -= GRID_LENGTH;
    else if (next_direction_ == RIGHT)
        new_head.first += GRID_LENGTH;
    else if (next_direction_ == UP)
        new_head.second -= GRID_LENGTH;
    else
        new_head.second += GRID_LENGTH;
    snake_.insert(snake_.begin(), new_head);
    // 检查有没有挂
    CheckDied();
    if (died_) {
        return;
    }
    auto star_pos = star_.getPosition();
    if (float_equal(new_head.first, star_pos.x) &&
        float_equal(new_head.second, star_pos.y)) {
        ChangeStar();
    } else {
        snake_.pop_back();
    }
    previous_direction_ = next_direction_;
}

void SnakeGame::Run()
{
    int i = 0;
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    while (window_.isOpen()) {
        ProcessEvents();
        timeSinceLastUpdate += clock.restart();
        while (timeSinceLastUpdate >= SECONDS_PER_FRAME)
        {
            ProcessEvents();
            Update();
            timeSinceLastUpdate -= SECONDS_PER_FRAME;
        }
        Render();
    }
}

void SnakeGame::ProcessEvents()
{
    sf::Event e;
    while (window_.pollEvent(e)) {
        if (e.type == sf::Event::Closed) {
            window_.close();
            return;
        }
        if (e.type == sf::Event::KeyPressed)
            HandleInput(e.key.code);
        if (e.type == sf::Event::KeyReleased)
            if (e.key.code == sf::Keyboard::J)
                SECONDS_PER_FRAME = sf::seconds(SLOW_SPEED);
    }
}

void SnakeGame::HandleInput(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::W || key == sf::Keyboard::Up)
        next_direction_ = UP;
    else if (key == sf::Keyboard::A || key == sf::Keyboard::Left)
        next_direction_ = LEFT;
    else if (key == sf::Keyboard::S || key == sf::Keyboard::Down)
        next_direction_ = DOWN;
    else if (key == sf::Keyboard::D || key == sf::Keyboard::Right)
        next_direction_ = RIGHT;
    else if (key == sf::Keyboard::J)
        SECONDS_PER_FRAME = sf::seconds(FAST_SPEED);
}

// 使用最为简单的算法,每次生成一个32*32的随机数,
// 如果在身体内,则重新生成,直到合理.
void SnakeGame::ChangeStar()
{
    float x, y;
    static std::default_random_engine e;
    static std::uniform_int_distribution<unsigned> u(0, 31); // 31是每行格点数
    while (true) {
        // 生成随机数并检查
        x = (float)u(e) * 20;
        y = (float)u(e) * 20;
        if (!InBody(x, y))
            break;
    }
    star_.setPosition(x, y);
}

// 使用float_equal(float,float)来比较
bool SnakeGame::InBody(float x, float y)
{
    for (auto it = snake_.begin(); it != snake_.end(); ++it)
        if (float_equal(x, it->first) && float_equal(y, it->second))
            return true;
    return false;
}

void SnakeGame::CheckDied()
{
    auto &new_head = snake_[0];
    // snake跑出去了
    if (new_head.first < 0 || new_head.first > HORIZONTAL_BOUND ||
        new_head.second < 0 || new_head.second > VERTICAL_BOUND) {
        died_ = true;
        return;
    }
    // 咬到自己
    auto it_head = snake_.begin();
    auto it_body = snake_.begin();
    while (++it_body != snake_.end()) {
        if (float_equal(it_body->first, it_head->first) &&
            float_equal(it_body->second, it_head->second)) {
            died_ = true;
            snake_.pop_back();  // 为了让snake的长度正确
            return;
        }
    }
}

int main()
{
    SnakeGame game;
    game.Run();

    return 0;
}
