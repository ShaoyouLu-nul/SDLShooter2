#include "sceneMain.h"
#include "game.h"


SceneMain::SceneMain():game(Game::getInstance())
{
    
}

SceneMain::~SceneMain()
{
    clean();
}

void SceneMain::init()
{
    std::random_device rd;
    gen = std::mt19937(rd());
    dis = std::uniform_real_distribution(0.0f,1.0f);
    
    player.texture = IMG_LoadTexture(game.getRenderer(),"assets/image/SpaceShip.png");
    SDL_QueryTexture(player.texture,NULL,NULL,&player.width,&player.height);
    player.width /= _ZOOM_OUT_;
    player.height /= _ZOOM_OUT_;
    player.position.x = game.getWindowWidth()/2 - player.width/2;
    player.position.y = game.getWindowHeight() - player.height;

    projectilePlayerTemplate.texture = IMG_LoadTexture(game.getRenderer(),"assets/image/laser-1.png");
    SDL_QueryTexture(projectilePlayerTemplate.texture,NULL,NULL,&projectilePlayerTemplate.width,&projectilePlayerTemplate.height);
    projectilePlayerTemplate.width /= _ZOOM_OUT_;
    projectilePlayerTemplate.height /= _ZOOM_OUT_;

    enemyTemplate.texture = IMG_LoadTexture(game.getRenderer(),"assets/image/insect-2.png");
    SDL_QueryTexture(enemyTemplate.texture,NULL,NULL,&enemyTemplate.width,&enemyTemplate.height);
    enemyTemplate.width /= _ZOOM_OUT_;
    enemyTemplate.height /= _ZOOM_OUT_;


    projectileEnemyTemplate.texture = IMG_LoadTexture(game.getRenderer(),"assets/image/bullet-1.png");
    SDL_QueryTexture(projectileEnemyTemplate.texture,NULL,NULL,&projectileEnemyTemplate.width,&projectileEnemyTemplate.height);
    projectileEnemyTemplate.width /= _ZOOM_OUT_;
    projectileEnemyTemplate.height /= _ZOOM_OUT_;

    explosionTemplate.texture = IMG_LoadTexture(game.getRenderer(),"assets/effect/explosion.png");
    SDL_QueryTexture(explosionTemplate.texture,NULL,NULL,&explosionTemplate.width,&explosionTemplate.height);
    // calculate explosion frames.
    explosionTemplate.totalFrame = explosionTemplate.width / explosionTemplate.height;
    explosionTemplate.width = explosionTemplate.height;

    itemLifeTemplate.texture = IMG_LoadTexture(game.getRenderer(),"assets/image/bonus_life.png");
    SDL_QueryTexture(itemLifeTemplate.texture,NULL,NULL,&itemLifeTemplate.width,&itemLifeTemplate.height);
    itemLifeTemplate.width /= _ZOOM_OUT_;
    itemLifeTemplate.height /= _ZOOM_OUT_;

}

void SceneMain::handleEvent(SDL_Event * event)
{

}

void SceneMain::update(float deltaTime)
{
    keyboardControl(deltaTime);
    updatePlayerProjectiles(deltaTime);
    spawnEnemy();
    updateEnemies(deltaTime);
    updateEnemyProjectiles(deltaTime);
    updatePlayer(deltaTime);
    updateExplosion(deltaTime);
    updateItems(deltaTime);
}

void SceneMain::render()
{
    // render player's projectile;
    renderPlayerProjectiles();
    renderEnemies();
    renderEnemyProjectiles();
    renderExplosions();
    renderItems();
    
    if(!isDead){
        SDL_Rect shipRect = {static_cast<int>(player.position.x), static_cast<int>(player.position.y), player.width, player.height};
        SDL_RenderCopy(game.getRenderer(), player.texture, NULL, &shipRect);
    }
}

void SceneMain::clean() 
{
    if(player.texture != nullptr)
        SDL_DestroyTexture(player.texture);

    for(auto projectile : projectilesPlayer){
        if(projectile != nullptr){
            delete projectile;
        }
    }
    projectilesPlayer.clear();
    if(projectilePlayerTemplate.texture != nullptr){
        SDL_DestroyTexture(projectilePlayerTemplate.texture);
    }

    for(auto enemy: enemies){
        if(enemy != nullptr){
            delete enemy;
        }
    }
    enemies.clear();
    SDL_DestroyTexture(enemyTemplate.texture);

    for(auto projectile : projectilesEnemies){
        if(projectile != nullptr){
            delete projectile;
        }
    }
    projectilesEnemies.clear();
    SDL_DestroyTexture(projectileEnemyTemplate.texture);

    for(auto explosion:explosions)
    {
        delete explosion;
    }
    explosions.clear();
    SDL_DestroyTexture(explosionTemplate.texture);

    for(auto item : items)
    {
        delete item;
    }
    items.clear();
    SDL_DestroyTexture(itemLifeTemplate.texture);
    
}

void SceneMain::keyboardControl(float deltaTime)
{
    if(isDead)
        return;
    const uint8_t * keyboardState = SDL_GetKeyboardState(NULL);
    if(keyboardState[SDL_SCANCODE_W])
    {
        if(player.position.y > 0)
            player.position.y -= player.speed * deltaTime;
    }
    if(keyboardState[SDL_SCANCODE_S])
    {
        if(player.position.y < game.getWindowHeight() - player.height)
            player.position.y += player.speed * deltaTime;
    }
    if(keyboardState[SDL_SCANCODE_A])
    {
        if(player.position.x > 0)
        {
            player.position.x -= player.speed * deltaTime;
        }

    }
    if(keyboardState[SDL_SCANCODE_D])
    {
        if(player.position.x < game.getWindowWidth() - player.width)
        {
            player.position.x += player.speed * deltaTime;
        }
    }

    if(keyboardState[SDL_SCANCODE_J] || keyboardState[SDL_SCANCODE_SPACE])
    {
        Uint32 currentTime = SDL_GetTicks();
        if(currentTime - player.lastShootTime > player.coolDown)
        { 
            shootPlayer();
            player.lastShootTime = currentTime;
        }
    }
}

void SceneMain::shootPlayer()
{
    auto projectile = new ProjectilePlayer(projectilePlayerTemplate);
    projectile->position.x= player.position.x + player.width/2 - projectile->width/2;
    projectile->position.y = player.position.y;
    projectilesPlayer.push_back(projectile);
}

void SceneMain::shootEnemy(Enemy * enemy)
{
    if(dis(gen) > _ATTACK_DESIRE_)
        return;

    auto projectile = new ProjectileEnemy(projectileEnemyTemplate);
    projectile->position.x= enemy->position.x + enemy->width/2 - projectile->width/2;
    projectile->position.y = enemy->position.y + enemy->height - projectile->height;
    projectile->direction = getDirection(enemy);
    projectilesEnemies.push_back(projectile);
}

void SceneMain::enemyExplode(Enemy * enemy)
{
    Explosion * explosion = new Explosion(explosionTemplate);
    explosion->startTime = SDL_GetTicks();
    explosion->position.x = (enemy->position.x + enemy->width/2) - explosion->width/2;
    explosion->position.y = (enemy->position.y + enemy->height/2) - explosion->height/2;
    explosions.push_back(explosion);
    dropItem(enemy);

}

SDL_FPoint SceneMain::getDirection(Enemy * enemy)
{
    float x = (player.position.x + player.width /2 ) - (enemy->position.x + enemy->width/2);
    float y = (player.position.y + player.height/2 ) - (enemy->position.y + enemy->height/2);
    float length = sqrt(x*x + y*y);

    x /= length;
    y /= length;

    return SDL_FPoint{x,y};
}

void SceneMain::spawnEnemy()
{
    if(dis(gen) > _SPWAN_)
        return;
    Enemy * enemy = new Enemy(enemyTemplate);
    enemy->position.x = dis(gen) * (game.getWindowWidth() - enemy->width);
    enemy->position.y = -enemy->height;
    enemies.push_back(enemy);
}

void SceneMain::dropItem(Enemy * enemy)
{
    if(dis(gen) > _PROBABILITY_DROP_)
        return;
    Item* item = new Item(itemLifeTemplate);
    item->position.x = enemy->position.x + enemy->width/2 - item->width/2;
    item->position.y = enemy->position.y + enemy->height/2 - item->height/2;
    
    float angle = dis(gen) * 2 * M_PI;
    item->direction.x = cos(angle);
    item->direction.y = sin(angle);
    items.push_back(item);
    

}

void SceneMain::updatePlayerProjectiles(float deltaTime)
{
    int margin = _MARGIN_;
    for(std::list<ProjectilePlayer*>::iterator it = projectilesPlayer.begin();it != projectilesPlayer.end();)
    {
        bool hitFlag = false;
        auto projectile = *it;
        projectile->position.y -= projectile->speed * deltaTime;
        if(projectile->position.y + margin < 0)
        {
            delete projectile;
            it = projectilesPlayer.erase(it);
            // SDL_Log("Player projectile removed");
            continue;
        }
        for(auto enemy:enemies){
            SDL_Rect enemyRect = {
                static_cast<int>(enemy->position.x),
                static_cast<int>(enemy->position.y),
                enemy->width,
                enemy->height
            };          
            SDL_Rect projectileRect = {
                static_cast<int>(projectile->position.x),
                static_cast<int>(projectile->position.y),
                projectile->width,
                projectile->height
            };
            if(SDL_HasIntersection(&enemyRect,&projectileRect)){
                // damage first,we will use damage data.
                enemy->currentHealth -= projectile->damage;
                // if delete first,damage will be random
                delete projectile;
                it = projectilesPlayer.erase(it);                
                hitFlag = true;
                break;
            }
        }
        if(!hitFlag) ++it;   
    }
}

void SceneMain::updateEnemies(float deltaTime)
{

    for(std::list<Enemy*>::iterator it = enemies.begin();it != enemies.end();)
    {
       
        auto enemy = *it;   
        if(enemy->currentHealth <= 0)
        {
            enemyExplode(enemy);
            it = enemies.erase(it);
            continue;
        }
        enemy->position.y += enemy->speed * deltaTime;
        if(enemy->position.y > game.getWindowHeight()){
            delete enemy;
            it = enemies.erase(it);
            continue;
        }
        ++it;
    }
    if(!isDead)for(auto it = enemies.begin();it != enemies.end();){
        auto enemy = *it;
         Uint32 currentTime = SDL_GetTicks();
        if(currentTime - enemy->lastShootTime > enemy->coolDown){
            shootEnemy(enemy);
            enemy->lastShootTime = currentTime;
        }
        ++it;
    }
   
}

void SceneMain::updateEnemyProjectiles(float deltaTime)
{
    for(std::list<ProjectileEnemy*>::iterator it = projectilesEnemies.begin();it != projectilesEnemies.end();)
    {
        ProjectileEnemy * projectile = *it;
        projectile->position.x += projectile->direction.x * deltaTime * projectile->speed;
        projectile->position.y += projectile->direction.y * deltaTime * projectile->speed;
        if((projectile->position.x < -projectile->width) ||
            (projectile->position.x > game.getWindowWidth()) ||
            (projectile->position.y < -projectile->height)||
            (projectile->position.y > game.getWindowHeight())
        ){
            delete projectile;
            it = projectilesEnemies.erase(it);
            continue;
        }
        ++it;
    }
    if(!isDead)// collision detection
    {
        for(std::list<ProjectileEnemy*>::iterator it = projectilesEnemies.begin();it != projectilesEnemies.end();)
        {
            ProjectileEnemy * projectile = *it;
            SDL_Rect projectileRect = {
                static_cast<int>(projectile->position.x),
                static_cast<int>(projectile->position.y),
                projectile->width,
                projectile->height,
            };
            SDL_Rect playerRect = {
                static_cast<int>(player.position.x),
                static_cast<int>(player.position.y),
                player.width,
                player.height,
            };
            if (SDL_HasIntersection(&projectileRect, &playerRect))
            {
                player.currentHealth -= projectile->damage;
                delete projectile;
                it = projectilesEnemies.erase(it);
                continue;
            }
            ++it;
        }
    }
}

void SceneMain::updatePlayer(float deltaTime)
{
    if(isDead)
        return;
    if(player.currentHealth <= 0){
        isDead = true;
        Explosion* explosion = new Explosion(explosionTemplate);

        explosion->startTime = SDL_GetTicks();
        explosion->position.x = player.position.x + player.width/2 - explosion->width/2;
        explosion->position.y = player.position.y + player.height/2 - explosion->height/2;
        explosions.push_back(explosion);
    }
    for(std::list<Enemy*>::iterator it = enemies.begin();it != enemies.end();){
        Enemy * enemy = *it;
        SDL_Rect enemyRect = {
            static_cast<int>(enemy->position.x),
            static_cast<int>(enemy->position.y),
            enemy->width,
            enemy->height
        };
        SDL_Rect playerRect = {
            static_cast<int>(player.position.x),
            static_cast<int>(player.position.y),
            player.width,
            player.height
        };
        if(SDL_HasIntersection(&enemyRect,&playerRect)){
            player.currentHealth -= 1;
            enemyExplode(enemy);
            delete enemy;
            it = enemies.erase(it);
            continue;
        }
        ++it;
    }
}

void SceneMain::updateExplosion(float deltaTime)
{
    for(std::list<Explosion*>::iterator it = explosions.begin();it != explosions.end();)
    {
        Uint32 currentTime = SDL_GetTicks();
        Explosion * explosion = *it;
        explosion->currentFrame = (currentTime - explosion->startTime)*explosion->FPS /1000;
        if(explosion->currentFrame >= explosion->totalFrame)
        {
            delete explosion;
            it = explosions.erase(it);
            continue;
        }
        ++it;
    }
}

void SceneMain::updateItems(float deltaTime)
{
    for(std::list<Item*>::iterator it = items.begin();it != items.end();)
    {
        Item * item = *it;
        
        item->position.x += deltaTime * item->speed * item->direction.x;
        item->position.y += deltaTime * item->speed * item->direction.y;

        if(item->bounce > 0 && (item->position.x < 0 || item->position.x > game.getWindowWidth() - item->width))
        {
            item->direction.x = -item->direction.x;
            --item->bounce;
        }
        if (item->bounce > 0 && (item->position.y < 0 || item->position.y > game.getWindowHeight() - item->height))
        {
            item->direction.y = -item->direction.y;
            --item->bounce;
        }

        if((item->position.x < -item->width) || (item->position.x > game.getWindowWidth())
    ||(item->position.y < -item->height) || (item->position.y > game.getWindowHeight()))
        {
            delete item;
            it = items.erase(it);
            continue;
        }
        
        ++it;
    }
    if(isDead)return;
    for(std::list<Item*>::iterator it = items.begin();it != items.end();)
    {
        Item * item = *it;
        SDL_Rect itemRect = {
            static_cast<int>(item->position.x),
            static_cast<int>(item->position.y),
            item->width,
            item->height
        };
        SDL_Rect playerRect = {
            static_cast<int>(player.position.x),
            static_cast<int>(player.position.y),
            player.width,
            player.height
        };
        if(SDL_HasIntersection(&itemRect,&playerRect))
        {
            player.currentHealth += item->heal;
            delete item;
            it = items.erase(it);
            continue;
        }
        ++it;
    }
}



void SceneMain::renderEnemyProjectiles()
{
    for(auto projectile : projectilesEnemies)
    {
        SDL_Rect projectileRect = {
            static_cast<int>(projectile->position.x),
            static_cast<int>(projectile->position.y),
            projectile->width,
            projectile->height
        };
        float angle = atan2(projectile->position.y,projectile->position.x) * 180 /M_PI - 90;
        SDL_RenderCopyEx(game.getRenderer(),projectile->texture,NULL,&projectileRect,angle,NULL,SDL_FLIP_NONE);
    }
}

void SceneMain::renderPlayerProjectiles()
{
    for(auto projectile : projectilesPlayer)
    {
        SDL_Rect projectileRect = {
            static_cast<int>(projectile->position.x),
            static_cast<int>(projectile->position.y),
            projectile->width,
            projectile->height
        };
        SDL_RenderCopy(game.getRenderer(),projectile->texture,NULL,&projectileRect);
    }
}

void SceneMain::renderEnemies()
{
     for(auto enemy : enemies)
    {
        SDL_Rect enemyRect = {
            static_cast<int>(enemy->position.x),
            static_cast<int>(enemy->position.y),
            enemy->width,
            enemy->height
        };
        SDL_RenderCopy(game.getRenderer(),enemy->texture,NULL,&enemyRect);
    }
}

void SceneMain::renderExplosions()
{
    for(auto explosion : explosions)
    {
        SDL_Rect srcRect = {
            explosion->currentFrame*explosion->width,
            0,
            explosion->width,
            explosion->height,
        };
        SDL_Rect dsRect = {
            static_cast<int>(explosion->position.x),
            static_cast<int>(explosion->position.y),
            explosion->width,
            explosion->height,
        };
        SDL_RenderCopy(game.getRenderer(),explosion->texture,&srcRect,&dsRect);
    }
}

void SceneMain::renderItems()
{
    for(auto item :items)
    {
        SDL_Rect itemRect = {
            static_cast<int>(item->position.x),
            static_cast<int>(item->position.y),
            item->width,
            item->height
        };
        SDL_RenderCopy(game.getRenderer(),item->texture,NULL,&itemRect);
    }
}
