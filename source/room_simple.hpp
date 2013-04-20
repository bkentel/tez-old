#pragma once

#include "room_base.hpp"

class room_simple : public room_base {
public:
    static unsigned const MIN_DIM = 5;
    static unsigned const MAX_DIM = 10;

    room_simple(unsigned w, unsigned h);
    virtual ~room_simple() {}
    
    template <typename T>
    static room_simple generate(T&& generator) {
        std::uniform_int_distribution<unsigned> dist(MIN_DIM, MAX_DIM);
        
        return generate_([&] {
            return dist(generator);
        });
    }
private:
    static room_simple generate_(std::function<unsigned ()> generator);
};
