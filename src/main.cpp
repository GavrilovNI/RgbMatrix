#include <Arduino.h>
#include <vector>
#include <cmath>
#include <algorithm>

#include "led/adafruit/AdafruitLedSnakeMatrix.h"
#include "led/adafruit/AdafruitLedStripContainer.h"
#include "led/LedMatrixSet.h"
#include "utils/ContainerConventer.h"
#include "masks/BrightnessLedPixel.h"

#include "snake/Snake.h"
#include "snake/SnakeMap.h"
#include "snake/SnakeMapUpdater.h"
#include "snake/AutoSnake.h"

#include "animations/matrix/Rainbow45.h"
#include "animations/matrix/SnakeAnim.h"

#include "masks/matrix/Circle.h"
#include "masks/matrix/Square.h"
#include "masks/matrix/BrightnessGradient.h"

#include "colorer/matrix/Solid.h"
#include "colorer/matrix/snake/SnakeMapDrawer.h"
#include "colorer/matrix/snake/SnakeDrawer.h"

AdafruitLedStripContainer<AdafruitLedSnakeMatrix> matrixes;

std::shared_ptr<LedMatrixSet> matrixSet;

std::shared_ptr<SnakeMap> snakeMap;
std::shared_ptr<SnakeMapUpdater> snakeMapUpdater = std::make_shared<SnakeMapUpdater>();


std::vector<std::shared_ptr<Snake>> snakes;
std::vector<std::shared_ptr<AutoSnake>> autoSnakes;

std::vector<std::shared_ptr<Colorer<LedMatrix>>> colorers;

void setup()
{
  matrixes.push_back(new AdafruitLedSnakeMatrix(Vector2<uint16_t>(32, 8), 16, NEO_GRB + NEO_KHZ800));

  //matrixes.push_back(new AdafruitLedSnakeMatrix(Vector2<uint16_t>(7, 13), 16, NEO_RGB + NEO_KHZ800));
  //matrixes.push_back(new AdafruitLedSnakeMatrix(Vector2<uint16_t>(7, 13), 5, NEO_RGB + NEO_KHZ800));
  //matrixes.push_back(new AdafruitLedSnakeMatrix(Vector2<uint16_t>(7, 13), 19, NEO_RGB + NEO_KHZ800));
  //matrixes.push_back(new AdafruitLedSnakeMatrix(Vector2<uint16_t>(4, 13), 21, NEO_RGB + NEO_KHZ800));
  
  matrixSet = std::make_shared<LedMatrixSet>(container_cast(matrixes.strips), LedMatrixSet::Right);

  std::shared_ptr<Snake> snake1 = std::make_shared<Snake>(Vector2<int32_t>(3, 3), Direction::Right, 3);
  std::shared_ptr<Snake> snake2 = std::make_shared<Snake>(Vector2<int32_t>(7, 3), Direction::Left, 3);

  snakes.push_back(snake1);
  snakes.push_back(snake2);

  snakeMap = std::make_shared<SnakeMap>(Vector2<int32_t>(matrixSet->getSize()));
  for(auto snake : snakes)
  {
    snakeMap->addSnake(snake);
    autoSnakes.push_back(std::make_shared<AutoSnake>(snake, snakeMap));
  }

  snakeMapUpdater->spawnFood(snakeMap);


  colorers.push_back(std::make_shared<SnakeMapDrawer>(snakeMap, ColorRGB(255, 0, 0), ColorRGB(255, 255, 0)));
  
  colorers.push_back(std::make_shared<SnakeDrawer>(snake1, ColorRGB(255, 0, 255), ColorRGB(0, 255, 0)));
  colorers.push_back(std::make_shared<SnakeDrawer>(snake2, ColorRGB(255, 0, 255), ColorRGB(0, 0, 255)));

  Serial.begin(9600);

  matrixes.begin();
  matrixes.clear();
  matrixes.show();
  matrixes.setBrightness(30);

  Serial.println("Begin");
}

unsigned long lastMillis; 
float snakeTime;
int snakeStepTime = 200;

bool moveSnake(std::shared_ptr<Snake> snake)
{
  if(snake->isDead())
    return false;
  
  snake->move(snakeMap, snakeMapUpdater);
  return true;
}

void loop()
{
  matrixes.clear();
  auto mask = matrixSet;

  int snakeMoves = (int)floor(snakeTime / snakeStepTime);
  snakeTime -= snakeMoves * snakeStepTime;

  for(int i = 0; i < snakeMoves; i++)
  {
    std::vector<std::shared_ptr<Snake>> snakesToMove = snakes;
    int deadSnakes = 0;

    for(auto autoSnake : autoSnakes)
    {
      autoSnake->decide();
      auto snake = autoSnake->getSnake();

      bool moved = moveSnake(snake);
      if(moved == false)
        deadSnakes++;

      snakesToMove.erase(std::remove(snakesToMove.begin(), snakesToMove.end(), snake), snakesToMove.end());
    }

    for(auto snake : snakesToMove)
    {
      bool moved = moveSnake(snake);
      if(moved == false)
        deadSnakes++;
    }
    
    if(deadSnakes == snakes.size())
    {
      delay(1000);
      ESP.restart();
    }
  }

  for(auto colorer : colorers)
    colorer->apply(mask);

  matrixes.show();

  unsigned long currMillis = millis();
  unsigned long delta = currMillis - lastMillis;
  lastMillis = currMillis;

  snakeTime += delta;

  delay(10);
}