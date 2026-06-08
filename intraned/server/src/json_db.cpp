#include "../include/json_db.h"
#include "../include/json.hpp"

#include <cstring>
#include <fstream>
#include <string>

using json = nlohmann::json;

// * Este modulo envuelve json.hpp como "caja negra" exportable.
// * Se usa desde http_bridge para:
// * - parsear request/login
// * - armar respuestas JSON
// * - leer/escribir metadata.json
//
// * Nota de seguridad de memoria:
// * las funciones que devuelven const char* reservan memoria en heap;
// * quien llama DEBE liberar con jsondb_free_string().
static char *dup_cstr(const std::string &s)
{
    char *out = new char[s.size() + 1];
    std::memcpy(out, s.c_str(), s.size() + 1);
    return out;
}

extern "C" bool jsondb_check_login_password(const char *request_body, const char *expected_password)
{
    try
    {
        // Previene null pointer y JSON roto.
        if (!request_body || !expected_password)
            return false;
        auto j = json::parse(request_body);
        return j.contains("password") && j["password"] == expected_password;
    }
    catch (...)
    {
        // Si viene basura o JSON invalido, responde false y no tira abajo el server.
        return false;
    }
}

extern "C" const char *jsondb_make_token_response(const char *token)
{
    json j = {{"token", token ? token : ""}};
    return dup_cstr(j.dump());
}

extern "C" const char *jsondb_make_error_response(const char *message)
{
    json j = {{"error", message ? message : "Error"}};
    return dup_cstr(j.dump());
}

extern "C" const char *jsondb_read_metadata(const char *metadata_path)
{
    try
    {
        if (!metadata_path)
        {
            json empty = {{"contenidos", json::array()}};
            return dup_cstr(empty.dump());
        }

        std::ifstream file(metadata_path);
        if (!file.is_open())
        {
            // Si todavia no existe metadata.json, devolvemos estructura vacia.
            json empty = {{"contenidos", json::array()}};
            return dup_cstr(empty.dump());
        }

        json db;
        file >> db;
        return dup_cstr(db.dump());
    }
    catch (...)
    {
        // Si el archivo viene corrupto, no explota: cae en contenido vacio.
        json empty = {{"contenidos", json::array()}};
        return dup_cstr(empty.dump());
    }
}

extern "C" bool jsondb_append_metadata(const char *metadata_path, const char *titulo, const char *autor, const char *tema, const char *filename)
{
    // Previene guardar metadata "huerfana" sin archivo o ruta destino.
    if (!metadata_path || !filename)
        return false;

    json db;
    std::ifstream in(metadata_path);
    if (in.is_open())
    {
        try
        {
            in >> db;
        }
        catch (...)
        {
            db = {{"contenidos", json::array()}};
        }
    }
    else
    {
        db = {{"contenidos", json::array()}};
    }

    if (!db.contains("contenidos") || !db["contenidos"].is_array())
    {
        db["contenidos"] = json::array();
    }

    db["contenidos"].push_back({{"titulo", titulo ? titulo : ""},
                                {"autor", autor ? autor : ""},
                                {"tema", tema ? tema : ""},
                                {"file", filename}});

    std::ofstream out(metadata_path);
    if (!out.is_open())
        return false;

    // Se guarda lindo (indentado) para que se pueda auditar a mano facil.
    out << db.dump(4);
    return true;
}

extern "C" void jsondb_free_string(const char *ptr)
{
    delete[] ptr;
}
