#ifndef LIGHT_SPQ_LIBRARY_H
#define LIGHT_SPQ_LIBRARY_H

#include "../SupervisorZone.h"
#include <cstdint>

namespace gteck {
    class LigthSpq {
        public:
            LigthSpq(int8_t thread_size);
            ~LigthSpq();

            SupervisorZone* sz;
            void push(T data);
            void fire();
    };

}

#endif // LIGHT_SPQ_LIBRARY_H