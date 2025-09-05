#include "scene.h"
#include "object.h"
#include <list>
#include <random>

#define _MARGIN_ 32
#define _SPWAN_ 0.0015f
#define _ATTACK_DESIRE_ 0.1f
#define _PROBABILITY_DROP_ 0.5f

class Game;

class SceneMain : public Scene
{

public:
    SceneMain();
    ~SceneMain();
    void init() override;
    void handleEvent(SDL_Event *event) override;
    void update(float deltaTime) override;
    void render() override;
    void clean() override;

    void keyboardControl(float deltaTime);
    void shootPlayer();
    void shootEnemy(Enemy *enemy);
    void enemyExplode(Enemy *enemy);
    SDL_FPoint getDirection(Enemy *enemy);

    void spawnEnemy();
    void dropItem(Enemy * enemy);


    void updatePlayerProjectiles(float deltaTime);
    void updateEnemies(float deltaTime);
    void updateEnemyProjectiles(float deltaTime);
    void updatePlayer(float deltaTime);
    void updateExplosion(float deltaTime);
    void updateItems(float deltaTime);


    void renderEnemyProjectiles();
    void renderPlayerProjectiles();
    void renderEnemies();
    void renderExplosions();
    void renderItems();

private:
    Game &game;
    Player player;
    bool isDead = false;
    ProjectilePlayer projectilePlayerTemplate;
    Enemy enemyTemplate;
    ProjectileEnemy projectileEnemyTemplate;
    Explosion explosionTemplate;
    Item itemLifeTemplate;


    std::list<ProjectilePlayer *> projectilesPlayer;
    std::list<Enemy *> enemies;
    std::list<ProjectileEnemy *> projectilesEnemies;
    std::list<Explosion*> explosions;
    std::list<Item*> items;
    

    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;
};