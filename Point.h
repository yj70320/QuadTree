#ifndef Point_H
#define Point_H

class Point
{
public:
    Point(Surface &surface)
        :surface_(surface)
    {
        x_ = rand() % W;
        y_ = rand() % H;

        radius_ = RADIUS_FOR_COLLISION;
        
        color_1 = true;       

        // 3 - 1, 5 - 2, 7 - 3 越多越乱
        dx = rand() % 7 - 3;
        dy = rand() % 7 - 3;
        
    }

    bool color() { return color_1; }
    void flip_color() { color_1 = (color_1 ? false : true); }
    void flip_direction() { dx = -dx; dy = -dy; }
    int x() { return x_; }
    int y() { return y_; }
    int radius() { return radius_; }

    void draw()
    {
        if (color_1) surface_.put_circle(x_, y_, RADIUS, R1, G1, B1);
        else surface_.put_circle(x_, y_, RADIUS, R2, G2, B2);
    }

    void move()
    {
        x_ += dx;
        if (x_ < 0)
        {
            x_ = 0;
            dx = -dx;
        }
        else if (x_ + radius_ * 2 > W - 1)
        {
            x_ = W - 1 - radius_ * 2;
            dx = -dx;
        }

        y_ += dy;
        if (y_ < 0)
        {
            y_ = 0;
            dy = -dy;
        }
        else if (y_ + radius_ * 2 > H - 1)
        {
            y_ = H - 1 - radius_ * 2;
            dy = -dy;
        }
    }

private:
    int x_;
    int y_;
    int radius_;
    double dx;
    double dy;
    int r1_, g1_, b1_;
    int r2_, g2_, b2_;
    bool color_1;
    Surface &surface_;
};

#endif
