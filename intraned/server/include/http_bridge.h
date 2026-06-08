#pragma once

#ifdef _WIN32
#ifdef HTTPBRIDGE_EXPORTS
#define HTTPBRIDGE_API __declspec(dllexport)
#else
#define HTTPBRIDGE_API __declspec(dllimport)
#endif
#else
#define HTTPBRIDGE_API
#endif

extern "C" HTTPBRIDGE_API int run_http_server();

// * Es el “mostrador” de la librería HTTP.

// * Qué cumple:

// * Expone una sola función pública: run_http_server().
// * Esa función arranca el servidor, define rutas y usa json_db para la parte de datos.
// * Usa macro HTTPBRIDGE_API para export/import de la función en DLL/SO.