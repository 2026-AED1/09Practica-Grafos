# Práctica 09 · Repaso de teoría: grafos

Resumen de lo que hace falta del tema 6 (i), *Grafos como estructura de
datos*. Los ficheros `01_…` a `08_…` que se citan son los de la clase de
teoría.

---

## 1. Qué es un grafo

Un **grafo** G = (V, A) son **vértices** y **aristas** que unen pares de
vértices. Se escribe **n = |V|** y **m = |A|**.

| Término              | Qué es                                                    | En la red de la práctica                     |
| -------------------- | --------------------------------------------------------- | -------------------------------------------- |
| No dirigido          | La arista {u, v} va en los dos sentidos                   | La amistad es mutua                          |
| Simple               | Sin lazos (u–u) ni multiaristas (dos u–v)                 | Nadie es amigo de sí mismo, ni dos veces     |
| Grado de u           | Aristas que tocan u                                       | Número de amigos                             |
| Suma de grados       | Siempre **2m** en un no dirigido                          | 22 = 2 · 11                                  |
| Camino, longitud     | Vértices unidos por aristas; longitud = número de aristas | Cadena de presentaciones                     |
| Componente conexa    | Trozo máximo en el que todos se alcanzan entre sí         | Un grupo                                     |
| Disperso             | m del orden de n, muy lejos de n²                         | Todas las redes sociales reales              |

---

## 2. Cómo se guarda: las cuatro representaciones

La operación que importa es **`vecinos(u)`**: la usan todos los recorridos.

|                          | Lista de aristas | Matriz  | Lista de adyacencia | Mapa de adyacencia       |
| ------------------------ | ---------------- | ------- | ------------------- | ------------------------ |
| Memoria                  | O(m)             | O(n²)   | **O(n + m)**        | O(n + m), constante alta |
| `existeArista(u, v)`     | O(m)             | O(1)    | O(grado u)          | O(1) esperado            |
| `vecinos(u)`             | O(m)             | O(n)    | **O(grado u)**      | O(grado u)               |
| Recorrer todo el grafo   | O(n · m)         | O(n²)   | **O(n + m)**        | O(n + m)                 |

**Cómo elegir:** por defecto, **lista de adyacencia**. La matriz, solo si el
grafo es denso o pequeño. El mapa de adyacencia, si los vértices tienen nombre
y aparecen y desaparecen, o si hay que preguntar mucho por parejas concretas.

**La lista de adyacencia de la práctica** (`03_lista_adyacencia.cpp`):

```text
ady[0] Ana    -> 1 2              (Bruno, Carla)
ady[1] Bruno  -> 0 2 3            (Ana, Carla, David)
ady[2] Carla  -> 0 1 3 4
...
```

En un grafo no dirigido, cada arista {u, v} aparece **dos veces**: v en
`ady[u]` y u en `ady[v]`. Ese es el invariante que no se puede romper.

**Vértices con nombre:** un diccionario nombre → número traduce cada nombre
**una sola vez**, al entrar (la práctica 05 del tema 5 y la parte 2 del
`04_mapa_adyacencia.cpp`). Dentro, todo son enteros 0..n − 1 y vectores.

---

## 3. Cómo se recorre: DFS y BFS

Un grafo puede tener **ciclos**: sin marcar los vértices ya vistos, un
recorrido daría vueltas para siempre (Ana → Bruno → Carla → Ana → …).

### DFS: en profundidad, con una pila

```text
pila = [origen];  marcar origen
mientras la pila no esté vacía:
    u = sacar de la pila
    para cada vecino v de u:
        si v no está marcado:  marcar v;  meter v en la pila
```

- Se mete hasta el fondo antes de probar alternativas.
- La versión recursiva es más corta, pero anida una llamada por vértice: con
  una fila de 100 000 vértices, Python se para (límite de unas 1000 llamadas) y
  C++ revienta la pila del sistema (`06_dfs.cpp`). Con una pila explícita no
  hay límite.
- **Componentes conexas** (`08_aplicaciones_recorridos.cpp`): se lanza un
  recorrido desde cada vértice aún sin marcar; cada recorrido pinta un grupo.

### BFS: por niveles, con una cola

```text
dist[*] = -1;  dist[origen] = 0;  cola = [origen]
mientras la cola no esté vacía:
    u = sacar de la cola
    para cada vecino v de u:
        si dist[v] == -1:
            dist[v] = dist[u] + 1;  padre[v] = u;  meter v en la cola
```

- Saca los vértices **en orden de distancia**: todos los de distancia d antes
  que cualquiera de distancia d + 1.
- Por eso **dist[v] es el mínimo número de aristas** de origen a v, y
  siguiendo `padre` hacia atrás desde v sale un camino de esa longitud
  (`07_bfs.cpp`).
- Se marca **al meter**: así ningún vértice entra dos veces en la cola.
- **Con pesos no sirve:** menos aristas no es menos coste. Eso es Dijkstra
  (parte II).

### Lo que cuestan

Cada vértice entra una vez en la pila o en la cola, y cada lista de vecinos se
recorre una vez: **O(n + m)** con lista de adyacencia. Con matriz, **O(n²)**,
porque `vecinos(u)` recorre la fila entera.

---

## 4. Chuleta de Python y C++

| Para…                       | Python                                    | C++                                         |
| --------------------------- | ----------------------------------------- | ------------------------------------------- |
| Pila                        | `pila = []`, `append(x)`, `pop()`         | `std::stack<int>`: `push`, `top`, `pop`     |
| Cola                        | `from collections import deque`; `append`, `popleft()` | `std::queue<int>`: `push`, `front`, `pop` |
| Conjunto (¿está?) O(1)      | `s = set(lista)`; `x in s`                | `std::unordered_set<int> s(v.begin(), v.end())`; `s.count(x)` |
| Contador                    | `d[k] = d.get(k, 0) + 1`                  | `std::unordered_map<int,int> d; d[k]++;`    |
| Ordenar por clave compuesta | `sorted(xs, key=lambda x: (-a[x], x))`    | `std::sort` con lambda                      |
| Dar la vuelta               | `lista.reverse()`                         | `std::reverse(v.begin(), v.end())`          |

**Ojo en Python:** `lista.pop(0)` **no** es una cola eficiente: desplaza todos
los elementos y cuesta O(n). Un BFS con `pop(0)` pasa de O(n + m) a O(n²).

---

## 5. Un puente con la matriz: amigos en común y A²

Si A es la matriz de adyacencia (1 si hay arista, 0 si no), entonces

(A²)[u][v] = Σ_w A[u][w] · A[w][v]

cuenta los w que son vecinos de u **y** de v a la vez: los caminos de dos
aristas de u a v. En una red social, **los amigos en común**. La diagonal
(A²)[u][u] es el grado de u (`02_matriz_adyacencia.cpp`). El ejercicio
extra calcula justo eso, solo para la fila de u y sin construir la matriz.
