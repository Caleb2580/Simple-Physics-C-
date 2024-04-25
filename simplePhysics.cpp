#define _USE_MATH_DEFINES

#include <graphics.h>
#include <math.h>
#include <chrono>
#include <iostream>
#include <sys/time.h>
#include <vector>

struct timeval tp;



uint64_t getTime() {
    uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return ms;
}

class Ball {
    public:
        double x = 0;
        double y = 0;
        int mass = 0;
        double velocity[2] = {0, 0};
        uint64_t lastTime = 0;
        double ff = 1;
        double loss = .6;
        bool init = false;
        bool coll = false;

        Ball() { };
    
        Ball(int x_, int y_) {
            x = (double)x_;
            y = (double)y_;
            lastTime = getTime();
            init = true;
        };

        Ball(int x_, int y_, int mass_) {
            x = (double)x_;
            y = (double)y_;
            mass = mass_;
            lastTime = getTime();
            init = true;
        };
        
        Ball(int x_, int y_, int mass_, int *velocity_) {
            x = (double)x_;
            y = (double)y_;
            mass = mass_;
            lastTime = getTime();
            init = true;
            velocity[0] = velocity_[0];
            velocity[1] = velocity_[1];
        };

    void move() {
        int dif = getTime() - lastTime;
        // int dif = 1;
        dif *= ff;

        lastTime = getTime();
        

        x += velocity[0] * dif;
        y += velocity[1] * dif;

        velocity[1] += (.002 * dif);
        
    };

    int getX() {
        return (int)x;
    };

    int getY() {
        return (int)y;
    };

    int getRadius() {
        return mass/2;
    }

    void collision(int w, int h, Ball *balls, int numBalls, int ind) {
        if ((x + getRadius()) > w) {
            if (velocity[0] > 0)
                velocity[0] *= -1 * loss;
            x -= (x + getRadius()) - w;
            coll = true;
        } else if ((x - getRadius()) < 0) {
            if (velocity[0] < 0)
                velocity[0] *= -1 * loss;
            x += 0 - (x - getRadius());
            coll = true;
        }
        if ((y + getRadius()) > h) {
            if (velocity[1] > 0)
                velocity[1] *= -1 * loss;
            y -= (y + getRadius()) - h;
            coll = true;
        } else if ((y - getRadius()) < 0) {
            if (velocity[1] < 0)
                velocity[1] *= -1 * loss;
            y += 0 - (y - getRadius());
            coll = true;
        }

    
        if (!coll) {
            for (int i = 0; i < numBalls; i++) {
                if (ind != i && !coll) {
                    double dx = balls[i].x - x;
                    double dy = balls[i].y - y;

                    double distance = sqrt(pow(dx, 2) + pow(dy, 2));


                    if (distance < balls[i].mass/2 + mass/2) {
                        double normalX = 0;
                        double normalY = 0;
                        if (distance != 0) {
                            normalX = dx / distance;
                            normalY = dy / distance;
                        }

                        double relativeVelocity = (
                            (balls[i].velocity[0] - velocity[0]) * normalX +
                            (balls[i].velocity[1] - velocity[1]) * normalY
                        );

                        double impulse = 2 * (relativeVelocity / (balls[i].mass + mass));

                        velocity[0] += impulse * balls[i].mass * normalX;
                        velocity[1] += impulse * balls[i].mass * normalY;
                        balls[i].velocity[0] -= impulse * mass * normalX;
                        balls[i].velocity[1] -= impulse * mass * normalY;

                        coll = true;
                        balls[i].coll = true;

                        double totalSeperationDistance = ((mass/2 + balls[i].mass/2) - distance) / 2;

                        double seperationDistanceX = totalSeperationDistance * (dx/distance);
                        double seperationDistanceY = totalSeperationDistance * (dy/distance);
                        x -= seperationDistanceX;
                        y -= seperationDistanceY;
                        balls[i].x += seperationDistanceX;
                        balls[i].y += seperationDistanceY;
                        return;
                    }
                }
            }
        }
    }

};


int main(int argc, char const *argv[])
{
    int gd = DETECT, gm;
    // initgraph(&gd, &gm, (char*)"");
    int w = 800;
    int h = 800;
    initwindow(w, h, "Graphics Window");

    // Ball ball(200, 200, 30);
    const int numBalls = 100;
    Ball balls[numBalls] = {};
    for (int i = 0; i < numBalls; i++) {
        int color[3] = {100, 100, 100};
        // For No Gravity
        // int velocity[2] = {rand() % 11 - 5, rand() % 11 - 5};
        // balls[i] = Ball(rand() % (w-1), rand() % (h-1), 20, velocity);
        
        // Gravity
        balls[i] = Ball(rand() % (w-1), rand() % (h-1), rand() % 20 + 20);
    }

    while (1) {
        cleardevice();
        for (int i = 0; i < numBalls; i++) {
            balls[i].coll = false;
        }
        for (int i = 0; i < numBalls; i++) {
            if (balls[i].init) {
                balls[i].move();
            }
        }
        for (int i = 0; i < numBalls; i++) {
            if (balls[i].init) {
                if (!balls[i].coll) {
                    balls[i].collision(w, h, balls, numBalls, i);
                }
                circle(balls[i].getX(), balls[i].getY(), balls[i].getRadius());
            }
        }
        swapbuffers();
    }


    getch();
    closegraph();
    return 0;
}