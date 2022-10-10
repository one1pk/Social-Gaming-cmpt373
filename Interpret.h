#pragma once

#include <nlohmann/json.hpp>

template <typname T>
class Interpreter{
    public:

        Interpreter(string path);

        //maps json data to game object
        virtual T interpret(T obj ) = 0;

    private:
        T data;
}