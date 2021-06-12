#ifndef TETROMINO_H
#define TETROMINO_H

#include <vector>

enum Pixel : uint8_t {
    Space,
    Pipe,
    Bar,
    Cyan,  
    Blue,
    Grey,
    Yellow,
    Green,
    Purple,
    Red
};

using Block = std::vector<std::vector<uint8_t>>;

// index, rotation
const Block blocks[7][4] = {
    {
        {
            { Space,  Space,  Space,  Space},
            { Space,  Space,  Space,  Space},
            {  Cyan,   Cyan,   Cyan,   Cyan},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,   Cyan,  Space,  Space},
            { Space,   Cyan,  Space,  Space},
            { Space,   Cyan,  Space,  Space},
            { Space,   Cyan,  Space,  Space}
        },
        {
            { Space,  Space,  Space,  Space},
            {  Cyan,   Cyan,   Cyan,   Cyan},
            { Space,  Space,  Space,  Space},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,  Space,   Cyan,  Space},
            { Space,  Space,   Cyan,  Space},
            { Space,  Space,   Cyan,  Space},
            { Space,  Space,   Cyan,  Space}
        }
    },
    {
        {
            { Space,  Space,  Space,  Space},
            { Space,   Blue,  Space,  Space},
            {  Blue,   Blue,   Blue,  Space},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,   Blue,  Space,  Space},
            { Space,   Blue,   Blue,  Space},
            { Space,   Blue,  Space,  Space},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,  Space,  Space,  Space},
            { Space,   Blue,   Blue,   Blue},
            { Space,  Space,   Blue,  Space},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,  Space,  Space,  Space},
            { Space,  Space,   Blue,  Space},
            { Space,   Blue,   Blue,  Space},
            { Space,  Space,   Blue,  Space}
        },
    },
    {
        {
            { Space,  Space,  Space,  Space},
            { Space,   Grey,   Grey,  Space},
            { Space,   Grey,   Grey,  Space},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,  Space,  Space,  Space},
            { Space,   Grey,   Grey,  Space},
            { Space,   Grey,   Grey,  Space},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,  Space,  Space,  Space},
            { Space,   Grey,   Grey,  Space},
            { Space,   Grey,   Grey,  Space},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,  Space,  Space,  Space},
            { Space,   Grey,   Grey,  Space},
            { Space,   Grey,   Grey,  Space},
            { Space,  Space,  Space,  Space}
        }
    },
    {
        {
            { Space,  Space,  Space,  Space},
            { Space,  Space,  Space,  Space},
            {Yellow, Yellow, Yellow,  Space},
            { Space, Yellow,  Space,  Space}
        },
        {
            { Space, Yellow,  Space,  Space},
            {Yellow, Yellow,  Space,  Space},
            { Space, Yellow,  Space,  Space},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,  Space, Yellow,  Space},
            { Space, Yellow, Yellow, Yellow},
            { Space,  Space,  Space,  Space},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,  Space,  Space,  Space},
            { Space,  Space, Yellow,  Space},
            { Space,  Space, Yellow, Yellow},
            { Space,  Space, Yellow,  Space}
        }
    },
    {
        {
            { Space,  Space,  Space,  Space},
            { Green,  Green,  Space,  Space},
            { Space,  Green,  Green,  Space},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,  Space,  Green,  Space},
            { Space,  Green,  Green,  Space},
            { Space,  Green,  Space,  Space},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,  Space,  Space,  Space},
            { Space,  Green,  Green,  Space},
            { Space,  Space,  Green,  Green},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,  Space,  Space,  Space},
            { Space,  Space,  Green,  Space},
            { Space,  Green,  Green,  Space},
            { Space,  Green,  Space,  Space}
        }
    },
    {
        {
            { Space,  Space,  Space,  Space},
            {Purple, Purple, Purple,  Space},
            { Space,  Space, Purple,  Space},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,  Space, Purple,  Space},
            { Space,  Space, Purple,  Space},
            { Space, Purple, Purple,  Space},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,  Space,  Space,  Space},
            { Space, Purple,  Space,  Space},
            { Space, Purple, Purple, Purple},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,  Space,  Space,  Space},
            { Space, Purple, Purple,  Space},
            { Space, Purple,  Space,  Space},
            { Space, Purple,  Space,  Space}
        }
    },
    {
        {
            { Space,  Space,  Space,  Space},
            { Space,  Space,    Red,  Space},
            {   Red,    Red,    Red,  Space},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,    Red,  Space,  Space},
            { Space,    Red,  Space,  Space},
            { Space,    Red,    Red,  Space},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,  Space,  Space,  Space},
            { Space,    Red,    Red,    Red},
            { Space,    Red,  Space,  Space},
            { Space,  Space,  Space,  Space}
        },
        {
            { Space,  Space,  Space,  Space},
            { Space,    Red,    Red,  Space},
            { Space,  Space,    Red,  Space},
            { Space,  Space,    Red,  Space}
        }
    }
};

#endif
