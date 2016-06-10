//
// Created by Wouter Posdijk on 09/06/16.
//

#ifndef IMOVE_COLORHOLEEFFECT_H
#define IMOVE_COLORHOLEEFFECT_H

#include <SFML/Graphics.hpp>
#include <queue>
#include "Effect.h"
#include "../LightPerson.h"
#include "../../Util/Timer.h"

struct ColorCircle {
    sf::Color color;
    float radius;
    ColorCircle(sf::Color color, float radius) : color(color), radius(radius) {}
};


class ColorHoleEffect : public Effect {
private:
    float hue = 0;
    std::deque<ColorCircle> circles;
    Timer timer;
    Vector2 location;
    void addCircle();
public:
    ColorHoleEffect(const Vector2 &loc);
    void setLocation(const Vector2 &loc);
    void update(float dt) override;
    void draw(sf::RenderTarget &target) override;

};


#endif //IMOVE_COLORHOLEEFFECT_H