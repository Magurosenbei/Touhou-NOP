#pragma once

template <typename T> class Singleton
{
public:
    static T& Instance()
    {
        static T theSingleInstance;  // assumes T has a protected default constructor
        return theSingleInstance;
    }
};
