#pragma once

#include <nlohmann/json.hpp>

template <typname T, I>
class Interpret{
    public:

        Interpret(string path);

        //maps json data to game object
        virtual T& interpret(T& obj ) = 0;

    private:
        I data;
}