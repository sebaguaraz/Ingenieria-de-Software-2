// * Punto de entrada ultra liviano.
// * Idea: que "main" casi no cambie y delegue todo a la libreria dinamica.
// * Beneficio: menos acople y compilacion mas rapida del ejecutable final.
#include "include/http_bridge.h"

int main() {
    return run_http_server();
}
