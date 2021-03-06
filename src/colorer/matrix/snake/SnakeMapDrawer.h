#pragma once

#include <memory>
#include "led/LedMatrix.h"
#include "colorer/Colorer.h"
#include "colors/ColorRGB.h"
#include "snake/SnakeMap.h"

class SnakeMapDrawer : public Colorer<LedMatrix>
{
private:
    std::shared_ptr<const SnakeMap> _snakeMap;
    ColorRGB _wallColor;
    ColorRGB _foodColor;

public:
    SnakeMapDrawer(std::shared_ptr<const SnakeMap> snakeMap, ColorRGB wallColor, ColorRGB foodColor) : Colorer()
    {
        _snakeMap = snakeMap;
        _wallColor = wallColor;
        _foodColor = foodColor;
    }

    virtual void apply(std::shared_ptr<LedMatrix> matrix) const override
    {
        for(int32_t y = 0; y < matrix->getSize().y; y++)
        {
            for(int32_t x = 0; x < matrix->getSize().x; x++)
            {
                Vector2<uint16_t> pos(x, y);
                ColorRGB color;
                SnakeMapTile tile = _snakeMap->getTile(Vector2<int32_t>(pos));
                switch (tile)
                {
                default:
                case SnakeMapTile::Empty:
                    color = ColorRGB(0, 0, 0);
                    break;
                case SnakeMapTile::Food:
                    color = _foodColor;
                    break;
                case SnakeMapTile::Wall:
                    color = _wallColor;
                    break;
                case SnakeMapTile::Snake:
                    continue;
                }
                matrix->getPixel(pos)->setColor(color);
            }
        }
    }
};
