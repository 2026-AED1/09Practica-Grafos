// Demo de la práctica 09: imprime lo que calcula cada ejercicio sobre la red de ejemplo.
//
//   g++ -std=c++17 -Wall -o demo demo_red_social.cpp && ./demo
//
// Los apartados de los ejercicios que aún no estén hechos aparecen como "pendiente".
#include <iomanip>
#include <iostream>
#include <string>

#include "red_social.hpp"

static std::string unir(const std::vector<std::string>& v, const std::string& sep = " ") {
    std::string s;
    for (size_t i = 0; i < v.size(); i++) s += (i ? sep : "") + v[i];
    return s;
}

template <class F>
static void apartado(const std::string& titulo, F f) {
    std::cout << "\n=== " << titulo << " ===\n";
    try {
        f();
    } catch (const std::runtime_error& e) {
        std::cout << "  (pendiente: " << e.what() << ")\n";
    }
}

int main() {
    std::cout << std::boolalpha;
    apartado("Ejercicio 1: la red", [&] {
        RedSocial r = redDeEjemplo();
        std::cout << "  n = " << r.numPersonas() << "  m = " << r.numAmistades() << "\n";
        int suma = 0;
        for (const auto& p : r.personas()) {
            auto a = r.amigos(p);
            std::cout << "  " << std::left << std::setw(7) << p << " -> "
                      << (a.empty() ? "-" : unir(a)) << "   (grado " << r.grado(p) << ")\n";
            suma += r.grado(p);
        }
        std::cout << "  suma de grados = " << suma << " = 2 * m\n";
        std::cout << "  anadirAmistad(Carla, Ana) = " << r.anadirAmistad("Carla", "Ana") << "  (ya existia)\n";
        std::cout << "  amigosEnComun(Ana, David) = " << unir(r.amigosEnComun("Ana", "David")) << "\n";
    });

    apartado("Ejercicio 2: grupos", [&] {
        RedSocial r = redDeEjemplo();
        std::cout << "  etiquetas =";
        for (int g : r.etiquetasDeGrupo()) std::cout << " " << g;
        std::cout << "\n";
        auto gs = r.grupos();
        for (size_t i = 0; i < gs.size(); i++) std::cout << "  grupo " << i << ": " << unir(gs[i]) << "\n";
        std::cout << "  mismoGrupo(Ana, Gloria) = " << r.mismoGrupo("Ana", "Gloria")
                  << "   mismoGrupo(Ana, Hugo) = " << r.mismoGrupo("Ana", "Hugo") << "\n";
    });

    apartado("Ejercicio 3: grados de separacion", [&] {
        RedSocial r = redDeEjemplo();
        auto ns = r.niveles("Ana");
        for (size_t d = 0; d < ns.size(); d++) std::cout << "  nivel " << d << ": " << unir(ns[d]) << "\n";
        for (auto [a, b] : std::vector<std::pair<std::string, std::string>>{
                 {"Ana", "Gloria"}, {"Gloria", "Ana"}, {"Ana", "Hugo"}}) {
            auto c = r.cadena(a, b);
            std::cout << "  separacion(" << a << ", " << b << ") = " << std::setw(2) << std::right
                      << r.separacion(a, b) << "   cadena: " << (c.empty() ? "(no hay)" : unir(c, " -> ")) << "\n";
        }
    });

    apartado("Ejercicio extra: personas que quiza conozcas", [&] {
        RedSocial r = redDeEjemplo();
        for (std::string p : {"Ana", "Fede", "Gloria", "Lucas"}) {
            std::cout << "  quiza conozcas, " << p << ": ";
            auto s = r.sugerencias(p);
            if (s.empty()) std::cout << "-";
            for (size_t i = 0; i < s.size(); i++)
                std::cout << (i ? ", " : "") << s[i].first << " (" << s[i].second << " en comun)";
            std::cout << "\n";
        }
    });
}
