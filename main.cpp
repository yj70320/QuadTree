/****************************************************************************
 Yihsiang Liow
 Copyright
****************************************************************************/
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <sstream>

#include "Includes.h"
#include "Constants.h"
#include "compgeom.h"
#include "Surface.h"
#include "Event.h"
#include "QuadTree.h"
#include "Point.h"

bool check_collisions(std::vector< Point* > &points, int i, int j)
{    
    int delta_x = (points[i]->x() - points[j]->x());
    int delta_y = (points[i]->y() - points[j]->y());
    int radius = points[i]->radius();
  
    int distance = sqrt(pow(delta_x, 2) + pow(delta_y, 2));
    if(i != j && distance <= radius * 2) return true; 
    else return false;
}

void points_collisions(Point* &x, Point* &y)
{    
    x->flip_direction();
    x->flip_color();
    x->move();
    
    y->flip_direction(); 
    y->flip_color();   
    y->move();               
}

void draw_points(std::vector< Point* > &points)
{
    for (int i = 0; i < QTY; ++i)
    {
        points[i]->draw();
    }
}

void draw_boundaries(QuadTree* n)
{
    n->draw_boundaries();
    if (n->is_leaf()) return;
    else
    {
        for (int i = 0; i < 4; ++i)
        {
            draw_boundaries(n->children(i));
        }
    }
}

void calculate_parent_collisions(QuadTree* p)
{
    QuadTree* q = p->parent();
    for(int i = 0; i < q->num_points(); ++i)
    {
        for(int j = 0; j < p->num_points(); ++j)
        {
            int delta_x = abs(q->points(i)->x() - p->points(j)->x());
            int delta_y = abs(q->points(i)->y() - p->points(j)->y());
            int distance = sqrt(pow(delta_x, 2) + pow(delta_y, 2));
            int radius = q->points(i)->radius();
                
            if(distance <= radius * 2)
            {
                points_collisions(q->points(i), p->points(j));   
            }
        }
    }
}

void calculate_quadtree_collisions(QuadTree* n)
{
    for(int i = 0; i < n->num_points(); ++i)
    {
        for(int j = 0; j < n->num_points(); ++j)
        {   
            if(i != j)
            {
                int delta_x = abs(n->points(i)->x() - n->points(j)->x());
                int delta_y = abs(n->points(i)->y() - n->points(j)->y());
                int distance = sqrt(pow(delta_x, 2) + pow(delta_y, 2));
                int radius = n->points(i)->radius();

                if(distance <= radius * 2)
                {
                    points_collisions(n->points(i), n->points(j));
                }
            }
        }
    }

    if(!n->is_leaf())
    {
        for(int i = 0; i < 4; ++i)
        {
            calculate_quadtree_collisions(n->children(i));
        }
    }
}

void handle_collisions(std::vector< Point * > & points, bool use_quadtrees,
                       QuadTree* root)
{
    if(use_quadtrees)
    {
        calculate_quadtree_collisions(root);   
    }
    else
    {
        for(int i = 0; i < QTY; ++i)
        {
            for(int j = 0; j < QTY; ++j)
            {
                if(check_collisions(points, i, j))
                {
                    points_collisions(points[i], points[j]);
                }
            }
        }
    }
}

//==========================================================================//  
//                                                                          //
//    main                                                                  //
//                                                                          //
//==========================================================================//

int main(int argc, char* argv[])
{
    std::cout << "\nQuadTree Code running...\n" << std::endl;
    Surface surface(W, H);
    Event event;
    bool use_quadtrees = true, skip_this_round = false;
    int fps_i = 0 , fps_a = 0, sum = 0;
    int fps[60] = {0};

    std::vector< Point * > points;

    // initilize all Point
    for(int i = 0; i < QTY; ++i)
    {
        Point * p = new Point(surface);
        points.push_back(p);
    }

    while(1)
    {
        int start = getTicks(); // frame starts
        
        if (event.poll()) 
        { 
            if (event.type() == QUIT)  break;                       
        }

        KeyPressed keypressed = get_keypressed();
        if ( (use_quadtrees == false) && keypressed[LEFTARROW])
        {
            use_quadtrees = true;
            std::cout << "\n+++++ Changing: using Quadtree+++++\n";
            skip_this_round = true;
        }
        if (use_quadtrees && keypressed[RIGHTARROW])
        {
            use_quadtrees = false;
            std::cout << "\n----- Changing: NOT using Quadtree---\n";	
            skip_this_round = true;		
        } 
        // 左箭头使用四叉树
        // 右箭头取消四叉树
              
        // points move
        for(int i = 0; i < points.size(); ++i)
        {
            points[i]->move();
        }

        QuadTree* root = new QuadTree(0, 0, W - 1, H - 1, points, surface, NULL);
        root->create_quadtree(); 

        handle_collisions(points, use_quadtrees, root);
        
        // frame starts
        surface.lock();
        surface.fill(BLACK);
        draw_points(points);
        if(use_quadtrees) draw_boundaries(root);
        surface.unlock();
        surface.flip();

        int end = getTicks(); // frame ends, log the time
        int dt = 1000/60 - end + start;
        // 每60帧输出一次
        if (fps_i >= RATE)
        {
            for (int i = 0; i < RATE; i++) sum = sum + fps[i];
            fps_a = sum / RATE;
            sum = 0;
            fps_i = 0;
            if (skip_this_round == false)
            {
                std::cout << "FPS = " << fps_a;
                if (use_quadtrees) std::cout << ", using Quadtree";
                std::cout << std::endl;
            }     
            skip_this_round = false;       
        }        
        fps[fps_i] = 1000/(end - start);             
        if (dt > 0) delay(dt); 
        fps_i++;        
    }

    return 0;
}


