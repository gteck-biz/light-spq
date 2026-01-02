#include "library.h"
#include "SupervisorZone.h"

namespace gteck {

    void LigthSpq::fire() {
    }

    template<typename T>
    void LigthSpq::push(T data) {

    }

    LigthSpq::LigthSpq(int8_t thread_size): sz(new SupervisorZone(thread_size)) {}

    LigthSpq::~LigthSpq() {
        delete sz;
    }
}
