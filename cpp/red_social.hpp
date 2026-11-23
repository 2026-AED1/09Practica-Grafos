// Práctica 09 · Grafos: una red social.
//
// La red es un grafo NO dirigido y SIN pesos: cada persona es un vértice y
// cada amistad una arista. Se guarda como LISTA DE ADYACENCIA (la
// representación por defecto del tema 6), con los nombres traducidos a
// números UNA sola vez:
//
//   nombres_[i]   nombre de la persona i                (número -> nombre)
//   indice_[n]    número de la persona con nombre n     (nombre -> número)
//   ady_[i]       números de los amigos de i, en el orden en que se añadieron
//   m_            número de amistades (aristas)
//
// Invariante (grafo simple y no dirigido):
//   1. v aparece en ady_[u]  <=>  u aparece en ady_[v]
//   2. nadie aparece en su propia lista (sin lazos)
//   3. nadie aparece dos veces en la misma lista (sin multiaristas)
//   4. m_ == (suma de las longitudes de las listas) / 2
//
// Fuera de la clase todo son nombres; dentro, todo son enteros 0..n-1.
#pragma once

#include <algorithm>
#include <queue>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

class RedSocial {
public:
    // ---------------------------------------------------------------- dado
    RedSocial() = default;

    RedSocial(const std::vector<std::string>& personas,
              const std::vector<std::pair<std::string, std::string>>& amistades) {
        for (const auto& p : personas) anadirPersona(p);
        for (const auto& [a, b] : amistades) anadirAmistad(a, b);
    }

    // Añade una persona sin amigos. O(1) esperado.
    void anadirPersona(const std::string& nombre) {
        if (indice_.count(nombre)) throw std::invalid_argument("persona repetida: " + nombre);
        indice_[nombre] = static_cast<int>(nombres_.size());
        nombres_.push_back(nombre);
        ady_.emplace_back();
    }

    int numPersonas() const { return static_cast<int>(nombres_.size()); }
    int numAmistades() const { return m_; }
    const std::vector<std::string>& personas() const { return nombres_; }

    // Amigos de `nombre`, en el orden en que se añadieron. O(grado).
    std::vector<std::string> amigos(const std::string& nombre) const {
        std::vector<std::string> salida;
        for (int v : ady_[id(nombre)]) salida.push_back(nombres_[v]);
        return salida;
    }

    int grado(const std::string& nombre) const {
        return static_cast<int>(ady_[id(nombre)].size());
    }

    // ¿Hay amistad entre a y b? Recorre la lista de a: O(grado(a)).
    bool sonAmigos(const std::string& a, const std::string& b) const {
        const auto& lista = ady_[id(a)];
        return std::find(lista.begin(), lista.end(), id(b)) != lista.end();
    }

    // ---------------------------------------------------------- Ejercicio 1
    // Añade la amistad {a, b}.
    //   - a o b desconocidos     -> std::invalid_argument (lo hace id())
    //   - a == b                 -> std::invalid_argument (no hay lazos)
    //   - la amistad ya existía  -> no cambia nada y devuelve false
    //   - si no                  -> la añade EN LAS DOS LISTAS, m_++, devuelve true
    // Coste: O(grado(a)) por la comprobación de duplicado.
    bool anadirAmistad(const std::string& a, const std::string& b) {
        // TODO: implementar
        throw std::runtime_error("TODO Ejercicio 1: anadirAmistad");
    }

    // Amigos de a que también lo son de b, en el orden de la lista de a.
    // Pista: mete los amigos de b en un std::unordered_set y recorre los de a.
    // Coste: O(grado(a) + grado(b)) esperado. Nada de dos bucles anidados.
    std::vector<std::string> amigosEnComun(const std::string& a, const std::string& b) const {
        // TODO: implementar
        throw std::runtime_error("TODO Ejercicio 1: amigosEnComun");
    }

    // ---------------------------------------------------------- Ejercicio 2
    // Vector `grupo` de longitud n: grupo[i] = número del grupo de la persona i.
    // Un grupo es una componente conexa. Los grupos se numeran 0, 1, 2... en
    // el orden en que aparece su primera persona (la de menor número): recorre
    // las personas 0..n-1 y lanza un recorrido desde cada una que siga sin grupo.
    //
    // OBLIGATORIO: con una std::stack explícita, sin recursión. Hay un test
    // con una cadena de 300 000 personas. Coste: O(n + m).
    std::vector<int> etiquetasDeGrupo() const {
        // TODO: implementar
        throw std::runtime_error("TODO Ejercicio 2: etiquetasDeGrupo");
    }

    // Lista de grupos; cada grupo, la lista de sus nombres por número. O(n + m).
    std::vector<std::vector<std::string>> grupos() const {
        std::vector<int> grupo = etiquetasDeGrupo();
        int k = grupo.empty() ? 0 : *std::max_element(grupo.begin(), grupo.end()) + 1;
        std::vector<std::vector<std::string>> salida(k);
        for (int i = 0; i < numPersonas(); i++) salida[grupo[i]].push_back(nombres_[i]);
        return salida;
    }

    // ¿Hay alguna cadena de amistades entre a y b? O(n + m).
    bool mismoGrupo(const std::string& a, const std::string& b) const {
        int u = id(a), v = id(b);
        std::vector<int> grupo = etiquetasDeGrupo();
        return grupo[u] == grupo[v];
    }

    // ---------------------------------------------------------- Ejercicio 3
    // Grados de separación entre a y b: amistades del camino más corto, -1 si no hay.
    int separacion(const std::string& a, const std::string& b) const {
        int u = id(a), v = id(b);
        std::vector<int> dist, padre;
        bfs(u, dist, padre);
        return dist[v];
    }

    // Personas agrupadas por distancia a `nombre`: {{nombre}, {amigos}, {amigos de amigos}, ...}.
    std::vector<std::vector<std::string>> niveles(const std::string& nombre) const {
        std::vector<int> dist, padre;
        bfs(id(nombre), dist, padre);
        std::vector<std::vector<std::string>> salida(*std::max_element(dist.begin(), dist.end()) + 1);
        for (int i = 0; i < numPersonas(); i++)
            if (dist[i] != -1) salida[dist[i]].push_back(nombres_[i]);
        return salida;
    }

    // Cadena de presentaciones más corta de a a b: {a, ..., b}.
    // {a} si a == b; {} si no están conectados. Usa bfs y sigue `padre` hacia
    // atrás desde b; luego dale la vuelta (std::reverse). Coste: O(n + m).
    std::vector<std::string> cadena(const std::string& a, const std::string& b) const {
        // TODO: implementar
        throw std::runtime_error("TODO Ejercicio 3: cadena");
    }

    // ------------------------------------------------------ Ejercicio extra
    // "Personas que quizá conozcas".
    // Candidatos: amigos de amigos de `nombre` que no son ella misma ni ya amigos
    // suyos. Para cada uno, cuántos amigos tiene en común con `nombre`.
    // Devuelve pares (candidato, comunes) ordenados por comunes de mayor a menor
    // y, a igualdad, por número de persona (de menor a mayor).
    //
    // Pistas: un std::unordered_set con los amigos de `nombre` y un
    // std::unordered_map<int,int> contador (como las frecuencias de la
    // práctica 08). No hace falta un BFS completo.
    // Coste: O(suma de los grados de sus amigos + S log S), S = candidatos.
    std::vector<std::pair<std::string, int>> sugerencias(const std::string& nombre) const {
        // TODO: implementar
        throw std::runtime_error("TODO Ejercicio extra: sugerencias");
    }

private:
    std::vector<std::string> nombres_;
    std::unordered_map<std::string, int> indice_;
    std::vector<std::vector<int>> ady_;
    int m_ = 0;

    // Nombre -> número. std::invalid_argument si la persona no existe.
    int id(const std::string& nombre) const {
        auto it = indice_.find(nombre);
        if (it == indice_.end()) throw std::invalid_argument("persona desconocida: " + nombre);
        return it->second;
    }

    // ---------------------------------------------------------- Ejercicio 3
    // BFS desde el número `origen`. Rellena dist y padre (longitud n):
    //   dist[v]  = mínimo número de amistades de origen a v (-1 si no se llega)
    //   padre[v] = persona desde la que se descubrió v (-1 para el origen y los no alcanzados)
    // Reglas: vecinos en el orden de ady_[u], std::queue, y marcar al METER
    // en la cola. Coste: O(n + m).
    void bfs(int origen, std::vector<int>& dist, std::vector<int>& padre) const {
        // TODO: implementar
        throw std::runtime_error("TODO Ejercicio 3: bfs");
    }
};

// ------------------------------------------------------------ la red de ejemplo
inline RedSocial redDeEjemplo() {
    return RedSocial(
        {"Ana", "Bruno", "Carla", "David", "Elena", "Fede",
         "Gloria", "Hugo", "Irene", "Jorge", "Lucas"},
        {{"Ana", "Bruno"}, {"Ana", "Carla"}, {"Bruno", "Carla"}, {"Bruno", "David"},
         {"Carla", "David"}, {"Carla", "Elena"}, {"David", "Fede"}, {"Elena", "Fede"},
         {"Fede", "Gloria"}, {"Hugo", "Irene"}, {"Irene", "Jorge"}});
}
