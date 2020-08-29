
#include "e172vp/renderer.h"

int main() {
    e172vp::Renderer renderer;
    while (renderer.isAlive()) {
        renderer.update();
    }
    return 0;
}
