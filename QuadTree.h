#ifndef QUADTREE_H
#define QUADTREE_H

#include <iostream>
#include <vector>
#include "Surface.h"
#include "Point.h"
#include "../Constants.h"

class QuadTree
{
    public:  
    QuadTree(int x_min, int y_min, int x_max, int y_max,
    std::vector< Point* > & points, Surface &surface, QuadTree* parent = nullptr)
    :x_min_(x_min), y_min_(y_min), x_max_(x_max), y_max_(y_max), parent_(parent),
    surface_(surface), points_(points)    {    }

    std::vector< Point * > points() { return points_; }
    QuadTree *& parent() { return parent_; }

    int x_min() const { return x_min_; }
    int y_min() const { return y_min_; }
    int x_max() const { return x_max_; }
    int y_max() const { return y_max_; }

    int num_points() const { return points_.size(); }
    int num_children() const { return children_.size(); }

    const QuadTree * const parent() const { return parent_; }
    
    bool is_root() const { return (parent_ == NULL); }
    bool is_leaf() const { return (children_.size() > 0 ? false : true);}    
    bool has_children() const { return !this->is_leaf(); }

    const bool parent_contains_objects() const
    {
        if(!this->is_root() && parent_->contains_objects()) return true;
        return false;
    }

    const bool contains_objects() const
    {
        if(points_[0] != NULL && points_[0] != 0) return true;
        return false;
    }

    void create_quadtree()
    {
        _create(this);
    }

    void draw_boundaries()
    {
        surface_.put_line(x_min_, y_min_, x_max_, y_min_, R_LINE, G_LINE, B_LINE);
        surface_.put_line(x_min_, y_max_, x_max_, y_max_, R_LINE, G_LINE, B_LINE);
        surface_.put_line(x_min_, y_min_, x_min_, y_max_, R_LINE, G_LINE, B_LINE);
        surface_.put_line(x_max_, y_min_, x_max_, y_max_, R_LINE, G_LINE, B_LINE);
    }

    QuadTree * root()
    {
        QuadTree * p = this;
        while(p->parent() != NULL)
        {
            p = p->parent();
        }
        return p;
    }

    int depth()
    {
        if(this->parent() == NULL) return 0;
        else return 1 + this->parent()->depth();
    }
   
    const Point* points(int index) const
    {
        if(points_.size() <= index) return NULL;
        else return points_[index];
    }

    Point* &points(int index)
    {
        if(points_.size() <= index) points_.resize(index + 1);
        return points_[index];
    }

    const QuadTree* children(int index) const
    {
        if(index < 0 || index >= children_.size()) throw std::out_of_range("Index out of range");
        return children_[index];
    }

    QuadTree*& children(int index)
    {
        if(index < 0) throw std::out_of_range("Index out of range");
        if(index >= children_.size()) children_.resize(index + 1, NULL);
        return children_[index];
    }

    void insert_point(Point * c)
    {
        points_.push_back(c);
    }

    void remove_point(const int index)
    {
        points_.erase(points_.begin()+index);
    }

    void clear_points()
    {
        points_.clear();
    }

    void pop_back_point()
    {
        points_.pop_back();
    }
    
    void insert_child(const int index, const int xmin, const int ymin,
                      const int xmax, const int ymax)
    {
        QuadTree * p = new QuadTree(xmin, ymin, xmax, ymax,
                                points_, surface_, this);
        if(this->children(index) != 0 || this->children(index) != NULL
            || index < 0 || index > 3)
        {
            std::cout << "Invalid insertion of child" << std::endl;
        }
        else if(index > children_.size() - 1)
        {
            for(int i = children_.size() - 1; i < index; ++i)
            {
                this->children(i) = NULL;
            }
            this->children(index) = p;
        }
        else
        {
            this->children(index) = p;
        }
    }

    void remove_child(const int index) 
    {
        remove_subsequent(this->children(index));
        this->children(index) = NULL;
    }

    void remove_subsequent(const QuadTree * p)
    {
        if(p->is_leaf())
        {
            delete p; return;
        }
        else
        {
            for(int i = 0; i < p->num_children(); ++i)
            {
                remove_subsequent(p->children(i));
            }
            delete p;
        }
    }    
    
    void delete_quadtree(QuadTree * p)
    {
        if(p->is_leaf())
        {
            delete p; 
            return;
        }
        
        for(int i = 0; i < p->num_children(); ++i)
        {
            delete_quadtree(p->children(i));
        }
        delete p;        
    }
    
    ~QuadTree()
    {
        delete_quadtree(this);
    }

private:
    int x_min_, y_min_, x_max_, y_max_;
    Surface & surface_;
    QuadTree * parent_;
    std::vector< Point * > points_; 
    std::vector< QuadTree * > children_; 
    
    void _create(QuadTree* n)
    {
        if (n->num_points() > THRESHOLD && n->depth() < MAX_DEPTH)
        {
            n->_create_children();
            n->_populate_quadtree(n);

            for (int i = 0; i < 4; ++i)
            {
                _create(n->children(i));
            }
        }
        else return;
    }

    void _create_children()
    {
        children_.push_back(new QuadTree(x_min_, y_min_, (x_min_ + x_max_) / 2,
                                       (y_min_ + y_max_) / 2, points_,
                                       surface_, this));
        children_.push_back(new QuadTree((x_min_ + x_max_) / 2, y_min_, x_max_,
                                       (y_min_ + y_max_) / 2, points_,
                                       surface_, this));
        
        children_.push_back(new QuadTree(x_min_, (y_min_ + y_max_) / 2,
                                       (x_min_ + x_max_) / 2, y_max_, points_,
                                       surface_, this));
        children_.push_back(new QuadTree((x_min_ + x_max_) / 2, (y_min_ + y_max_) / 2,
                                       x_max_, y_max_, points_, surface_, this));

        children_[0]->clear_points();
        children_[1]->clear_points();
        children_[2]->clear_points();
        children_[3]->clear_points();
    }

    void _populate_quadtree(QuadTree* n)
    {
        std::vector< Point* > temp;
        for (int i = n->num_points() - 1; i > -1; --i)
        {
            Point* x = n->points(i);

            bool inserted = false;
            for (int j = 0; j < 4; ++j)
            {
                if (x->x() > n->children(j)->x_min() &&
                    x->x() < n->children(j)->x_max() &&
                    x->y() > n->children(j)->y_min() &&
                    x->y() < n->children(j)->y_max())
                {
                    n->children(j)->insert_point(x);
                    inserted = true;
                    break;
                }
            }

            if (!inserted)
            {
                temp.push_back(x);
            }
            else
            {
                n->pop_back_point();
            }
        }

        for (int i = 0; i < temp.size(); ++i)
        {
            n->insert_point(temp[i]);
        }
    }


};

#endif
