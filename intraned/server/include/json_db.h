#pragma once

#ifdef _WIN32
#ifdef JSONDB_EXPORTS
#define JSONDB_API __declspec(dllexport)
#else
#define JSONDB_API __declspec(dllimport)
#endif
#else
#define JSONDB_API
#endif

extern "C"
{
  JSONDB_API bool jsondb_check_login_password(const char *request_body, const char *expected_password);
  JSONDB_API const char *jsondb_make_token_response(const char *token);
  JSONDB_API const char *jsondb_make_error_response(const char *message);
  JSONDB_API const char *jsondb_read_metadata(const char *metadata_path);
  JSONDB_API bool jsondb_append_metadata(const char *metadata_path, const char *titulo, const char *autor, const char *tema, const char *filename);
  JSONDB_API void jsondb_free_string(const char *ptr);
}

// * Es el “mostrador” de la librería que se encarga de JSON y metadata.

// * Qué cumple:

// * Define funciones para:
// * validar login leyendo JSON (jsondb_check_login_password)
// * armar respuestas JSON de token/error
// * leer metadata.json
// * agregar un registro nuevo en metadata.json
// * liberar memoria de strings (jsondb_free_string)
// * Expone API con extern "C" para que el enlace dinámico sea más simple/estable.
// * Usa macro JSONDB_API para exportar/importar símbolos en Windows (__declspec).