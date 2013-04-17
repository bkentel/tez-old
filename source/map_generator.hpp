#pragma once

#include "types.hpp"
#include <memory>

class image_targa;

class binary_split_node {
public:
    static unsigned const MIN_DIM = 5;

    binary_split_node(unsigned x, unsigned y, unsigned w, unsigned h);

    void split(unsigned depth = 0);

    void write(image_targa& img, unsigned& count);

    template <typename F>
    void for_each(F&& function) {
        bool leaf = true;
        
        if (first) {
            first->for_each(function);
            leaf = false;
        }

        if (second) {
            second->for_each(function);
            leaf = false;
        }

        if (leaf) {
            function(x + 1, y + 1, w - 1, h - 1);
        }
    }

    unsigned x, y, w, h;
    std::unique_ptr<binary_split_node> first;
    std::unique_ptr<binary_split_node> second;
};

class binary_split_generator : public binary_split_node {
public:
    binary_split_generator(unsigned w, unsigned h);
};
