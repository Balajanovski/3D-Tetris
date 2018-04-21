// 
// Created by Balajanovski on 21/04/2018.
//

#ifndef INC_3D_TETRIS_ENUMCLASSHASH_H
#define INC_3D_TETRIS_ENUMCLASSHASH_H

#include <cstdlib>

struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

#endif //INC_3D_TETRIS_ENUMCLASSHASH_H
