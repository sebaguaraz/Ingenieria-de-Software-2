#include "../include/http_bridge.h"
#include "../include/httplib.h"
#include "../include/json_db.h"

#include <fstream>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

// * Este modulo es el "puente HTTP":
// * - Acá vive toda la logica de rutas de httplib.
// * - Se compila como DLL/SO para no meter todo ese peso en main.cpp.
// * - Habla con json_db para parsear/guardar metadata sin tocar json.hpp en main.
static void asegurar_directorio(const std::string &path)
{
#ifdef _WIN32
    // Previene error al subir archivos si la carpeta no existe.
    _mkdir(path.c_str()); // si existe, no pasa nada relevante para este caso
#else
    mkdir(path.c_str(), 0777);
#endif
}

extern "C" int run_http_server()
{
    const std::string ADMIN_TOKEN = "educacion_libre_2026";
    const std::string ADMIN_PASSWORD = "admin123";
    const std::string upload_dir = "./server/uploads";
    const std::string metadata_path = upload_dir + "/metadata.json";

    asegurar_directorio(upload_dir);

    httplib::Server svr;
    svr.set_mount_point("/", "./public");
    svr.set_mount_point("/recursos", upload_dir);

    // Login admin: delega parseo JSON al modulo json_db.
    // Previene crasheos por JSON mal formado: json_db devuelve false.
    svr.Post("/api/login", [&](const httplib::Request &req, httplib::Response &res)
             {
        const bool ok = jsondb_check_login_password(req.body.c_str(), ADMIN_PASSWORD.c_str());

        if (ok) {
            const char* payload = jsondb_make_token_response(ADMIN_TOKEN.c_str());
            res.set_content(payload ? payload : "{\"token\":\"\"}", "application/json");
            // Importante: la memoria del payload se libera SIEMPRE.
            jsondb_free_string(payload);
        } else {
            res.status = 401;
            const char* payload = jsondb_make_error_response("No autorizado");
            res.set_content(payload ? payload : "{\"error\":\"No autorizado\"}", "application/json");
            jsondb_free_string(payload);
        } });

    svr.Get("/api/contenidos", [&](const httplib::Request &, httplib::Response &res)
            {
        const char* payload = jsondb_read_metadata(metadata_path.c_str());
        res.set_content(payload ? payload : "{\"contenidos\":[]}", "application/json");
        jsondb_free_string(payload); });

    // Upload protegido con token:
    // previene que cualquiera suba archivos sin autenticacion.
    svr.Post("/api/upload", [&](const httplib::Request &req, httplib::Response &res)
             {
        if (req.get_header_value("Authorization") != ADMIN_TOKEN) {
            res.status = 403;
            const char* payload = jsondb_make_error_response("Token invalido");
            res.set_content(payload ? payload : "{\"error\":\"Token invalido\"}", "application/json");
            jsondb_free_string(payload);
            return;
        }

        if (!req.form.has_file("archivo")) {
            res.status = 400;
            const char* payload = jsondb_make_error_response("Falta el archivo");
            res.set_content(payload ? payload : "{\"error\":\"Falta el archivo\"}", "application/json");
            jsondb_free_string(payload);
            return;
        }

        const auto& file_part = req.form.get_file("archivo");
        const std::string titulo = req.form.get_field("titulo");
        const std::string autor = req.form.get_field("autor");
        const std::string tema = req.form.get_field("tema");
        const std::string filename = file_part.filename;

        const std::string path = upload_dir + "/" + filename;
        std::ofstream ofs(path, std::ios::binary);

        // Previene perder metadata cuando falla la escritura fisica del archivo.
        if (!ofs.is_open()) {
            res.status = 500;
            const char* payload = jsondb_make_error_response("Error de E/S en disco");
            res.set_content(payload ? payload : "{\"error\":\"Error de E/S en disco\"}", "application/json");
            jsondb_free_string(payload);
            return;
        }

        ofs << file_part.content;
        ofs.close();

        const bool saved = jsondb_append_metadata(
            metadata_path.c_str(),
            titulo.c_str(),
            autor.c_str(),
            tema.c_str(),
            filename.c_str()
        );

        if (saved) {
            res.set_content("{\"status\":\"success\"}", "application/json");
        } else {
            res.status = 500;
            const char* payload = jsondb_make_error_response("Error en metadata");
            res.set_content(payload ? payload : "{\"error\":\"Error en metadata\"}", "application/json");
            jsondb_free_string(payload);
        } });

    std::cout << "Servidor iniciado en http://localhost:8080 (DLL bridge)" << std::endl;
    svr.listen("0.0.0.0", 8080);
    return 0;
}
