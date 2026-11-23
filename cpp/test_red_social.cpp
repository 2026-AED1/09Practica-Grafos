// Tests de la práctica 09.
//
//   g++ -std=c++17 -O2 -Wall -o test_red_social test_red_social.cpp && ./test_red_social
//
// Un test que lanza una excepción (por ejemplo, el "TODO" de una función sin
// hacer) cuenta como fallido, pero no para a los demás.
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "red_social.hpp"

using Nombres = std::vector<std::string>;
using Sugerencias = std::vector<std::pair<std::string, int>>;

static int comprobaciones = 0, fallos = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        ++comprobaciones;                                                        \
        if (!(cond)) {                                                           \
            ++fallos;                                                            \
            std::cout << "    FALLA (linea " << __LINE__ << "): " #cond "\n";    \
        }                                                                        \
    } while (0)

template <class F>
bool lanzaInvalidArgument(F f) {
    try { f(); } catch (const std::invalid_argument&) { return true; } catch (...) { return false; }
    return false;
}

static void prueba(const std::string& nombre, void (*f)()) {
    int antes = fallos;
    try {
        f();
    } catch (const std::exception& e) {
        ++fallos;
        std::cout << "    EXCEPCION: " << e.what() << "\n";
    }
    std::cout << (fallos == antes ? "  ok     " : "  FALLA  ") << nombre << "\n";
}

static RedSocial cadenaLarga(int n) {
    RedSocial r;
    for (int i = 0; i < n; i++) r.anadirPersona("p" + std::to_string(i));
    for (int i = 0; i + 1 < n; i++) r.anadirAmistad("p" + std::to_string(i), "p" + std::to_string(i + 1));
    return r;
}

// ------------------------------------------------------------ Ejercicio 1
static void tamanoYGrados() {
    RedSocial r = redDeEjemplo();
    CHECK(r.numPersonas() == 11);
    CHECK(r.numAmistades() == 11);
    CHECK(r.grado("Carla") == 4);
    CHECK(r.grado("Lucas") == 0);
    int suma = 0;
    for (const auto& p : r.personas()) suma += r.grado(p);
    CHECK(suma == 2 * r.numAmistades());
}

static void lasDosDirecciones() {
    RedSocial r = redDeEjemplo();
    CHECK(r.amigos("Ana") == (Nombres{"Bruno", "Carla"}));
    CHECK(r.amigos("Carla") == (Nombres{"Ana", "Bruno", "David", "Elena"}));
    CHECK(r.sonAmigos("Gloria", "Fede"));
    CHECK(r.sonAmigos("Fede", "Gloria"));
    CHECK(!r.sonAmigos("Ana", "Gloria"));
}

static void duplicadaNoCuenta() {
    RedSocial r = redDeEjemplo();
    CHECK(!r.anadirAmistad("Carla", "Ana"));
    CHECK(!r.anadirAmistad("Ana", "Carla"));
    CHECK(r.numAmistades() == 11);
    CHECK(r.amigos("Ana") == (Nombres{"Bruno", "Carla"}));
}

static void nuevaAmistad() {
    RedSocial r = redDeEjemplo();
    CHECK(r.anadirAmistad("Lucas", "Ana"));
    CHECK(r.numAmistades() == 12);
    CHECK(r.amigos("Lucas") == (Nombres{"Ana"}));
    CHECK(r.amigos("Ana") == (Nombres{"Bruno", "Carla", "Lucas"}));
}

static void errores() {
    RedSocial r = redDeEjemplo();
    CHECK(lanzaInvalidArgument([&] { r.anadirAmistad("Ana", "Ana"); }));
    CHECK(lanzaInvalidArgument([&] { r.anadirAmistad("Ana", "Zoe"); }));
    CHECK(lanzaInvalidArgument([&] { r.anadirPersona("Ana"); }));
    CHECK(r.numAmistades() == 11);
}

static void amigosEnComun() {
    RedSocial r = redDeEjemplo();
    CHECK(r.amigosEnComun("Ana", "David") == (Nombres{"Bruno", "Carla"}));
    CHECK(r.amigosEnComun("David", "Ana") == (Nombres{"Bruno", "Carla"}));
    CHECK(r.amigosEnComun("Bruno", "Elena") == (Nombres{"Carla"}));
    CHECK(r.amigosEnComun("Ana", "Gloria").empty());
    CHECK(r.amigosEnComun("Ana", "Lucas").empty());
}

static void invariante() {
    RedSocial r = redDeEjemplo();
    for (const auto& p : r.personas()) {
        Nombres a = r.amigos(p);
        CHECK(std::find(a.begin(), a.end(), p) == a.end());
        Nombres ordenados = a;
        std::sort(ordenados.begin(), ordenados.end());
        CHECK(std::adjacent_find(ordenados.begin(), ordenados.end()) == ordenados.end());
        for (const auto& q : a) CHECK(r.sonAmigos(q, p));
    }
}

// ------------------------------------------------------------ Ejercicio 2
static void etiquetas() {
    CHECK(redDeEjemplo().etiquetasDeGrupo() == (std::vector<int>{0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2}));
}

static void grupos() {
    auto g = redDeEjemplo().grupos();
    CHECK(g.size() == 3);
    CHECK(g[0] == (Nombres{"Ana", "Bruno", "Carla", "David", "Elena", "Fede", "Gloria"}));
    CHECK(g[1] == (Nombres{"Hugo", "Irene", "Jorge"}));
    CHECK(g[2] == (Nombres{"Lucas"}));
}

static void mismoGrupo() {
    RedSocial r = redDeEjemplo();
    CHECK(r.mismoGrupo("Ana", "Gloria"));
    CHECK(r.mismoGrupo("Jorge", "Hugo"));
    CHECK(!r.mismoGrupo("Ana", "Hugo"));
    CHECK(r.mismoGrupo("Lucas", "Lucas"));
}

static void unPuenteUneGrupos() {
    RedSocial r = redDeEjemplo();
    r.anadirAmistad("Gloria", "Jorge");
    CHECK(r.grupos().size() == 2);
    CHECK(r.mismoGrupo("Ana", "Hugo"));
}

static void redVaciaYSinAmistades() {
    CHECK(RedSocial().grupos().empty());
    RedSocial r({"X", "Y", "Z"}, {});
    CHECK(r.grupos() == (std::vector<Nombres>{{"X"}, {"Y"}, {"Z"}}));
}

static void cadenaDe300000SinRecursion() {
    // Un DFS recursivo revienta la pila del sistema (8 MB) mucho antes.
    RedSocial r = cadenaLarga(300000);
    auto g = r.grupos();
    CHECK(g.size() == 1);
    CHECK(g[0].size() == 300000);
}

// ------------------------------------------------------------ Ejercicio 3
static void separacion() {
    RedSocial r = redDeEjemplo();
    CHECK(r.separacion("Ana", "Ana") == 0);
    CHECK(r.separacion("Ana", "Carla") == 1);
    CHECK(r.separacion("Ana", "Elena") == 2);
    CHECK(r.separacion("Ana", "Gloria") == 4);
    CHECK(r.separacion("Gloria", "Ana") == 4);
    CHECK(r.separacion("Ana", "Hugo") == -1);
    CHECK(r.separacion("Lucas", "Ana") == -1);
}

static void niveles() {
    CHECK(redDeEjemplo().niveles("Ana") ==
          (std::vector<Nombres>{{"Ana"}, {"Bruno", "Carla"}, {"David", "Elena"}, {"Fede"}, {"Gloria"}}));
}

static void cadena() {
    RedSocial r = redDeEjemplo();
    CHECK(r.cadena("Ana", "Gloria") == (Nombres{"Ana", "Bruno", "David", "Fede", "Gloria"}));
    CHECK(r.cadena("Gloria", "Ana") == (Nombres{"Gloria", "Fede", "David", "Bruno", "Ana"}));
    CHECK(r.cadena("Elena", "Bruno") == (Nombres{"Elena", "Carla", "Bruno"}));
    CHECK(r.cadena("Ana", "Ana") == (Nombres{"Ana"}));
    CHECK(r.cadena("Ana", "Lucas").empty());
}

static void cadenaEsUnCaminoValido() {
    RedSocial r = redDeEjemplo();
    for (const auto& a : r.personas()) {
        for (const auto& b : r.personas()) {
            Nombres c = r.cadena(a, b);
            int d = r.separacion(a, b);
            if (d == -1) { CHECK(c.empty()); continue; }
            CHECK(static_cast<int>(c.size()) == d + 1);
            CHECK(c.front() == a && c.back() == b);
            for (size_t i = 0; i + 1 < c.size(); i++) CHECK(r.sonAmigos(c[i], c[i + 1]));
        }
    }
}

static void escala() {
    RedSocial r = cadenaLarga(300000);
    auto inicio = std::chrono::steady_clock::now();
    CHECK(r.separacion("p0", "p299999") == 299999);
    CHECK(r.cadena("p0", "p299999").size() == 300000);
    double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - inicio).count();
    std::cout << "    (separacion + cadena con n = 300000: " << ms << " ms)\n";
}

// -------------------------------------------------------- Ejercicio extra
static void sugerenciasEjemplo() {
    RedSocial r = redDeEjemplo();
    CHECK(r.sugerencias("Ana") == (Sugerencias{{"David", 2}, {"Elena", 1}}));
    CHECK(r.sugerencias("Fede") == (Sugerencias{{"Carla", 2}, {"Bruno", 1}}));
    CHECK(r.sugerencias("David") == (Sugerencias{{"Ana", 2}, {"Elena", 2}, {"Gloria", 1}}));
    CHECK(r.sugerencias("Carla") == (Sugerencias{{"Fede", 2}}));
}

static void empatesPorNumero() {
    CHECK(redDeEjemplo().sugerencias("Gloria") == (Sugerencias{{"David", 1}, {"Elena", 1}}));
}

static void sinSugerencias() {
    RedSocial r = redDeEjemplo();
    CHECK(r.sugerencias("Lucas").empty());
    CHECK(r.sugerencias("Irene").empty());
    CHECK(r.sugerencias("Hugo") == (Sugerencias{{"Jorge", 1}}));
}

static void nuncaUnoMismoNiAmigos() {
    RedSocial r = redDeEjemplo();
    for (const auto& p : r.personas()) {
        for (const auto& [c, k] : r.sugerencias(p)) {
            CHECK(c != p);
            CHECK(!r.sonAmigos(p, c));
            CHECK(k == static_cast<int>(r.amigosEnComun(p, c).size()));
            CHECK(r.separacion(p, c) == 2);
        }
    }
}

int main() {
    std::cout << "Ejercicio 1\n";
    prueba("tamano y grados", tamanoYGrados);
    prueba("las dos direcciones", lasDosDirecciones);
    prueba("amistad duplicada no cuenta", duplicadaNoCuenta);
    prueba("nueva amistad", nuevaAmistad);
    prueba("errores", errores);
    prueba("amigosEnComun", amigosEnComun);
    prueba("invariante", invariante);

    std::cout << "Ejercicio 2\n";
    prueba("etiquetasDeGrupo", etiquetas);
    prueba("grupos", grupos);
    prueba("mismoGrupo", mismoGrupo);
    prueba("un puente une grupos", unPuenteUneGrupos);
    prueba("red vacia y sin amistades", redVaciaYSinAmistades);
    prueba("cadena de 300000 sin recursion", cadenaDe300000SinRecursion);

    std::cout << "Ejercicio 3\n";
    prueba("separacion", separacion);
    prueba("niveles", niveles);
    prueba("cadena", cadena);
    prueba("cadena es un camino valido", cadenaEsUnCaminoValido);
    prueba("escala", escala);

    std::cout << "Ejercicio extra\n";
    prueba("sugerencias del ejemplo", sugerenciasEjemplo);
    prueba("empates por numero", empatesPorNumero);
    prueba("sin sugerencias", sinSugerencias);
    prueba("nunca uno mismo ni amigos", nuncaUnoMismoNiAmigos);

    std::cout << "\n" << comprobaciones << " comprobaciones, " << fallos << " fallos\n";
    return fallos == 0 ? 0 : 1;
}
