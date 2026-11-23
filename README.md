# Práctica 09: Grafos

## Una red social: grupos, grados de separación y «personas que quizá conozcas»

AED I — Bloque IV · Python / C++ · 110 minutos

Una red social es el ejemplo de grafo por excelencia: cada persona es un
**vértice** y cada amistad una **arista** no dirigida. En esta práctica vas a
construir una red pequeña y a contestar sobre ella las tres preguntas que
cualquier red social se hace a diario:

1. ¿Qué **grupos** de personas hay, sin ninguna amistad que los una?
   (componentes conexas, con una **pila**)
2. ¿Cuántos **grados de separación** hay entre dos personas, y por quién
   habría que pasar para que se conozcan? (BFS, con una **cola**)
3. ¿A quién le sugiero como **nuevo amigo**? (amigos de amigos, contados con
   un **diccionario**; es el **ejercicio extra**)

Todo con la representación por defecto del tema 6, la **lista de
adyacencia**, y en tiempo **O(n + m)**.

---

## Cómo usar esta guía

Igual que en las prácticas anteriores: cada ejercicio tiene un **contrato**
(qué tiene que hacer la función, sin ambigüedades), unas **pistas** y un
bloque **Antes de ejecutar** con preguntas para hacer a mano. Contesta primero
en papel y solo después ejecuta la demo para comparar.

Elige **un** lenguaje. Los dos tienen las mismas funciones, los mismos tests y
la misma red de ejemplo. Las partes que tienes que escribir están marcadas con
`TODO`.

**Python** (desde la carpeta `python/`):

```bash
python -m unittest -v test_red_social                    # todos los tests
python -m unittest -v test_red_social.TestEjercicio1     # solo un ejercicio
python demo_red_social.py                                # la demo
```

**C++** (desde la carpeta `cpp/`):

```bash
g++ -std=c++17 -O2 -Wall -o test_red_social test_red_social.cpp && ./test_red_social
g++ -std=c++17 -Wall -o demo demo_red_social.cpp && ./demo
```

**Ficheros:**

| Fichero                                       | Contenido                                       | ¿Hay que completarlo?          |
| --------------------------------------------- | ----------------------------------------------- | ------------------------------ |
| `TEORIA.md`                                   | El repaso: representaciones, DFS, BFS y costes  | No, para leer                  |
| `python/red_social.py` · `cpp/red_social.hpp` | La clase `RedSocial` y la red de ejemplo        | **Sí** (ejercicios 1 a 3 y extra) |
| `python/test_red_social.py` · `cpp/test_red_social.cpp` | Los tests, por ejercicio              | No                             |
| `python/demo_red_social.py` · `cpp/demo_red_social.cpp` | Imprime lo que calcula cada ejercicio | No                             |
| `python/extra_medir_cola.py`                  | Comentario extra: `deque` frente a `pop(0)`     | No, para ejecutar              |

**Plan de la sesión:**

| Fase         | Tiempo | Objetivo                                                        |
| ------------ | ------ | --------------------------------------------------------------- |
| Repaso       | 20 min | Lista de adyacencia, pila frente a cola, O(n + m)               |
| Ejercicio 1  | 25 min | Construir la red: nombres → números y amistades sin duplicados  |
| Ejercicio 2  | 25 min | Los grupos, con una pila explícita                              |
| Ejercicio 3  | 30 min | Grados de separación y la cadena de presentaciones (BFS)        |
| Cierre       | 10 min | Justificar costes y cuestionario en Blackboard                  |

Si terminas antes, o como trabajo para casa, tienes el **ejercicio extra**:
personas que quizá conozcas.

---

## 0. Repaso (20 min)

El repaso completo está en [`TEORIA.md`](TEORIA.md). Lo imprescindible:

- **Lista de adyacencia:** para cada vértice, un vector con sus vecinos. En un
  grafo **no dirigido**, cada arista {u, v} aparece **dos veces**: v en la
  lista de u y u en la de v. Memoria O(n + m); `vecinos(u)` en O(grado(u)).
- **Vértices con nombre:** se traducen a números 0..n − 1 **una sola vez**,
  con un diccionario nombre → número (el tema 5). Dentro de la clase solo hay
  enteros.
- **DFS** (pila) se mete hasta el fondo; **BFS** (cola) avanza por niveles y
  da el **mínimo número de aristas** desde el origen. Los dos marcan los
  vértices visitados para no dar vueltas en los ciclos, y los dos cuestan
  **O(n + m)** con lista de adyacencia.

**Preguntas rápidas** (contéstalas antes de abrir el código):

1. En un grafo no dirigido con m aristas, ¿cuánto suma la longitud de todas
   las listas de adyacencia?
2. ¿Por qué no usamos una matriz de adyacencia para una red social de mil
   millones de personas?
3. Si cambias la cola de un BFS por una pila, ¿qué deja de garantizar?

<details>
<summary>Mostrar respuestas</summary>

1. **2m**: cada arista aparece en dos listas. Es lo mismo que «la suma de los
   grados es 2m».
2. Ocuparía n² casillas: 10¹⁸, para guardar unos pocos cientos de amigos por
   persona. La red es **dispersa**: casi toda la matriz serían ceros.
3. Que la distancia sea la mínima. Con pila se obtiene un recorrido en
   profundidad, que encuentra *un* camino, no necesariamente el más corto.

</details>

---

## La red de ejemplo

Todos los tests y la demo usan esta red de 11 personas y 11 amistades:

```text
    Ana --------- Bruno
     |          /   |
     |        /     |
     |      /       |
    Carla --------- David
     |                |
     |                |
    Elena ---------- Fede --------- Gloria


    Hugo ---- Irene ---- Jorge                Lucas
```

Las personas se numeran en el orden en que se dan: Ana = 0, Bruno = 1, …,
Lucas = 10. Las amistades se añaden en este orden:

```text
Ana-Bruno  Ana-Carla  Bruno-Carla  Bruno-David  Carla-David  Carla-Elena
David-Fede  Elena-Fede  Fede-Gloria  Hugo-Irene  Irene-Jorge
```

**El orden importa:** cada lista de amigos guarda los amigos en el orden en
que se añadieron, y los recorridos visitan los vecinos en ese orden. Así todos
obtenemos exactamente el mismo resultado.

---

## 1. Construir la red (25 min)

La clase ya traduce los nombres a números (`_id` / `id`) y añade personas
(`anadir_persona`). Tú tienes que añadir **amistades** y responder a la
primera consulta.

### Ejercicio 1a: `anadir_amistad(a, b)` · `anadirAmistad`

**Contrato:**

| Situación                    | Qué hace                                                       |
| ---------------------------- | -------------------------------------------------------------- |
| a o b no existen             | `ValueError` / `std::invalid_argument` (ya lo hace `_id`)      |
| a == b                       | `ValueError` / `std::invalid_argument`: no hay **lazos**       |
| la amistad ya existía        | no cambia nada y devuelve `False`: no hay **multiaristas**     |
| en otro caso                 | la añade **en las dos listas**, suma 1 a m y devuelve `True`   |

Cuidado: la amistad Carla–Ana ya existe aunque se escribiera como Ana–Carla.

### Ejercicio 1b: `amigos_en_comun(a, b)` · `amigosEnComun`

Devuelve los amigos de a que también son amigos de b, **en el orden de la
lista de a**. Tiene que costar **O(grado(a) + grado(b))**: mete los amigos de
b en un `set` / `std::unordered_set` y recorre los de a. Dos bucles anidados
cuestan O(grado(a) · grado(b)).

**Antes de ejecutar:**

- Escribe la lista de amigos de Carla y la de David.
- ¿Qué devuelve `amigos_en_comun("Ana", "David")`? ¿Y `("Ana", "Gloria")`?
- Tras construir la red, ¿cuánto vale la suma de los grados de todos?

<details>
<summary>Mostrar resultado</summary>

```text
=== Ejercicio 1: la red ===
  n = 11  m = 11
  Ana     -> Bruno Carla   (grado 2)
  Bruno   -> Ana Carla David   (grado 3)
  Carla   -> Ana Bruno David Elena   (grado 4)
  David   -> Bruno Carla Fede   (grado 3)
  Elena   -> Carla Fede   (grado 2)
  Fede    -> David Elena Gloria   (grado 3)
  Gloria  -> Fede   (grado 1)
  Hugo    -> Irene   (grado 1)
  Irene   -> Hugo Jorge   (grado 2)
  Jorge   -> Irene   (grado 1)
  Lucas   -> -   (grado 0)
  suma de grados = 22 = 2 * m
  anadir_amistad(Carla, Ana) = False  (ya existía)
  amigos_en_comun(Ana, David) = ['Bruno', 'Carla']
```

`amigos_en_comun("Ana", "Gloria")` es la lista vacía: no tienen amigos en
común (están a distancia 4).

</details>

**El error típico** es añadir v a la lista de u y olvidarse de añadir u a la
de v. El programa no falla, pero Bruno sería amigo de Ana y Ana no lo sería de
Bruno, y el test `test_invariante` lo detecta. El segundo error típico es
sumar 1 a m aunque la amistad ya existiera.

---

## 2. Los grupos (25 min)

Un **grupo** es una componente conexa: un conjunto de personas unidas entre
sí por alguna cadena de amistades y sin ninguna amistad con nadie de fuera.

### Ejercicio 2: `etiquetas_de_grupo()` · `etiquetasDeGrupo`

Devuelve un vector `grupo` de longitud n, donde `grupo[i]` es el número del
grupo de la persona i. Con eso, `grupos()` y `mismo_grupo(a, b)` ya están
hechas.

**Contrato:**

- Los grupos se numeran 0, 1, 2… en el orden en que aparece su primera
  persona. Para conseguirlo, recorre las personas 0..n − 1 y lanza un
  recorrido desde cada una que siga sin grupo.
- **Obligatorio: con una pila explícita** (en Python, una lista con `append` y
  `pop`; en C++, `std::stack`). **Sin recursión.**

**Pista:** marca a cada persona con su grupo **al meterla** en la pila, no al
sacarla. Así nadie entra dos veces. Para pintar grupos no importa el orden en
que se visiten las personas, solo que se visiten todas.

**¿Por qué sin recursión?** Hay un test con una **fila de 100 000 personas**
(300 000 en C++): p0 – p1 – p2 – … Un DFS recursivo anida una llamada por
persona. En Python se para a unas 1000 llamadas (`RecursionError`). En C++
agota los 8 MB de la pila del sistema y el programa muere con *Segmentation
fault*: si te pasa con ese test, es esto.

**Antes de ejecutar:**

- ¿Cuántos grupos hay en la red de ejemplo? ¿Cuál es el vector de etiquetas?
- Si se añade la amistad Gloria–Jorge, ¿cuántos grupos quedan?
- ¿Cuántos grupos tiene una red de 3 personas sin ninguna amistad? ¿Y una red
  vacía?

<details>
<summary>Mostrar resultado</summary>

```text
=== Ejercicio 2: grupos ===
  etiquetas = [0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2]
  grupo 0: Ana Bruno Carla David Elena Fede Gloria
  grupo 1: Hugo Irene Jorge
  grupo 2: Lucas
  mismo_grupo(Ana, Gloria) = True   mismo_grupo(Ana, Hugo) = False
```

Con Gloria–Jorge, los dos primeros grupos se unen y quedan **2**. Tres
personas sin amistades forman **3** grupos de una persona cada uno; una red
vacía no tiene **ningún** grupo.

</details>

---

## 3. Grados de separación (30 min)

¿Cuántas amistades hay que recorrer, como mínimo, para ir de una persona a
otra? Es la distancia en número de aristas, y la calcula el **BFS**.

### Ejercicio 3a: `_bfs(origen)` · `bfs` (privada)

Desde la persona número `origen`, calcula dos vectores de longitud n:

- `dist[v]`: el **mínimo** número de amistades de origen a v (−1 si no se
  llega).
- `padre[v]`: la persona desde la que se descubrió v (−1 para el origen y para
  los no alcanzados).

**Reglas:** visita los vecinos en el orden de su lista. Usa una **cola de
verdad**: `collections.deque` con `popleft()` en Python (`list.pop(0)` es
O(n)) o `std::queue` en C++. Y marca al **meter** en la cola, no al sacar.

Con `_bfs` hecho, `separacion(a, b)` y `niveles(nombre)` ya funcionan.

### Ejercicio 3b: `cadena(a, b)`

La **cadena de presentaciones** más corta: `[a, …, b]`. Devuelve `[a]` si
a == b y `[]` si no están en el mismo grupo.

**Pista:** el BFS desde a deja en `padre` un árbol. Desde b, sigue `padre`
hacia atrás hasta llegar a −1 y después dale la vuelta a la lista.

**Antes de ejecutar** (hazlo con la tabla en papel: cola, dist, padre):

- Sigue el BFS desde Ana. ¿En qué orden salen las personas de la cola? ¿Qué
  personas hay a distancia 2?
- ¿Qué cadena devuelve `cadena("Ana", "Gloria")`? Hay **tres** cadenas de 4
  amistades entre ellas: ¿cuáles? ¿Por qué el BFS elige esa?
- ¿Es `cadena("Gloria", "Ana")` la de antes al revés?

<details>
<summary>Mostrar resultado</summary>

```text
=== Ejercicio 3: grados de separación ===
  nivel 0: Ana
  nivel 1: Bruno Carla
  nivel 2: David Elena
  nivel 3: Fede
  nivel 4: Gloria
  separacion(Ana, Gloria) =  4   cadena: Ana -> Bruno -> David -> Fede -> Gloria
  separacion(Gloria, Ana) =  4   cadena: Gloria -> Fede -> David -> Bruno -> Ana
  separacion(Ana, Hugo) = -1   cadena: (no hay)
```

Las tres cadenas mínimas son Ana–Bruno–David–Fede–Gloria,
Ana–Carla–David–Fede–Gloria y Ana–Carla–Elena–Fede–Gloria. El BFS elige la
primera porque David se descubre **desde Bruno**: Bruno sale de la cola antes
que Carla (Ana añadió primero a Bruno). Una vez que David tiene padre, ya no
cambia.

Aquí `cadena("Gloria", "Ana")` sí es la misma al revés, pero **no tiene por
qué** serlo: desde Gloria, el BFS descubre a Bruno y a Carla desde David y a
Ana desde Bruno, que es el primero de la lista de David. Con otro orden de
inserción podría salir otra cadena igual de corta.

</details>

**El error típico** es marcar al **sacar** de la cola: una persona puede
entrar varias veces y, si la distancia se asigna al meter, la última
asignación puede no ser la mínima. El segundo es devolver la cadena al revés,
de b a a.

---

## Cierre (10 min)

Rellena la tabla con el coste de cada operación, con lista de adyacencia
(n personas, m amistades, d = grado):

| Operación                | Coste | ¿Por qué? |
| ------------------------ | ----- | --------- |
| `anadir_amistad(a, b)`   |       |           |
| `amigos_en_comun(a, b)`  |       |           |
| `grupos()`               |       |           |
| `separacion(a, b)`       |       |           |

Y una pregunta para pensar: ¿qué cambiarías en la representación para que
`son_amigos(a, b)` costara O(1) sin gastar O(n²) de memoria?

Al terminar, contesta el **cuestionario de la práctica 09 en Blackboard**.

---

## Ejercicio extra: personas que quizá conozcas

La sugerencia de amistad clásica de cualquier red social: **amigos de mis
amigos que aún no son amigos míos**, ordenados por cuántos amigos tenemos en
común.

### `sugerencias(nombre)`

**Contrato:**

- Los candidatos son las personas que son amigas de algún amigo de `nombre`,
  sin contar a `nombre` ni a quienes ya son amigos suyos.
- Para cada candidato, se cuentan sus **amigos en común** con `nombre`.
- Se devuelven pares `(candidato, comunes)`, ordenados por `comunes` **de
  mayor a menor** y, a igualdad, **por número de persona** (de menor a mayor).

**Pistas:**

- No hace falta un BFS completo: para cada amigo f, recorre los amigos g de f.
  Cada vez que llegas a un candidato g a través de un amigo, ese amigo es uno
  más en común.
- Para saber en O(1) si g ya es amigo, usa un `set` con los amigos de
  `nombre`. Para contar, usa un diccionario candidato → comunes, como el
  contador de frecuencias de la práctica 08.
- Para ordenar: en Python, `sorted(..., key=lambda g: (-comunes[g], g))`; en
  C++, `std::sort` con una lambda que compare primero `comunes` y luego el
  número.

**Antes de ejecutar:**

- Calcula a mano `sugerencias("Ana")`. ¿Por qué David tiene 2 y Elena 1?
- ¿Y `sugerencias("Fede")`? ¿Y `sugerencias("Lucas")`?
- ¿A qué distancia de `nombre` están siempre los candidatos?
- ¿Cuánto cuesta `sugerencias(a)` con lista de adyacencia?

<details>
<summary>Mostrar resultado</summary>

```text
=== Ejercicio extra: personas que quizá conozcas ===
  quizá conozcas, Ana: David (2 en común), Elena (1 en común)
  quizá conozcas, Fede: Carla (2 en común), Bruno (1 en común)
  quizá conozcas, Gloria: David (1 en común), Elena (1 en común)
  quizá conozcas, Lucas: -
```

A David se llega desde Ana por dos amigos suyos (Bruno y Carla); a Elena, solo
por Carla. Los candidatos están siempre a **distancia exactamente 2**: no son
amigos (distancia 1) y comparten al menos un amigo.

Coste: O(Σ grado(f) + S log S), sumando sobre los amigos f de a, con S
candidatos: dos niveles de listas y ordenar los candidatos al final.

</details>

**Un detalle bonito:** el número de amigos en común de u y v es
**(A²)[u][v]**, el producto de la matriz de adyacencia por sí misma que vimos
en teoría (`02_matriz_adyacencia.cpp`): cuenta los caminos de dos aristas.
Aquí lo calculamos solo para una fila y sin construir la matriz.

---

## Comentarios extras

**1. ¿Por qué `deque` y no una lista?** En Python, `lista.pop(0)` desplaza
todos los elementos que quedan, así que un BFS con esa «cola» cuesta O(n²).
Los tests no lo detectan con la red de ejemplo, pero `extra_medir_cola.py` lo
mide en árboles cada vez más grandes. Ejecútalo: con medio millón de personas,
`pop(0)` va más de cien veces más lento que `deque`, y la diferencia crece con
n.

**2. Pila «de verdad» frente a marcar al meter.** El `dfsIterativo` de teoría
(`06_dfs.cpp`) marca al **sacar** de la pila porque tiene que dar
*exactamente* el orden del DFS recursivo. Aquí, para pintar grupos, el orden
da igual, y marcar al meter es más sencillo y garantiza que cada persona entra
en la pila una sola vez. Las dos versiones cuestan O(n + m).

**3. Por qué los nombres se traducen una vez.** Podríamos guardar la red como
un diccionario de diccionarios, nombre → {amigo: …} (el mapa de adyacencia del
tema). Sería cómodo, pero cada acceso tendría que hashear una cadena. Con la
traducción a números al entrar, los recorridos solo tocan vectores de enteros.
Si `son_amigos` tuviera que ser O(1), el cambio mínimo sería guardar, además,
un `set` de amigos por persona.

**4. Seis grados de separación.** La idea de que dos personas cualesquiera
están unidas por unos seis intermediarios viene de los experimentos de Stanley
Milgram con cartas en los años 60. En 2016, Facebook calculó el promedio sobre
toda su red y le salió 3,57. No hizo un BFS desde cada usuario, que habría sido
inviable, sino estimaciones estadísticas, pero la pregunta es la misma que la
de tu `separacion`.

**5. Lo que viene.** Si las amistades tuvieran **pesos** (por ejemplo, cuánto
se trata cada pareja), «la cadena más corta» ya no sería la de menos aristas,
y el BFS dejaría de servir: eso es **Dijkstra**, en la parte II del tema. Y en
Python, la biblioteca habitual para todo esto es `networkx`, que guarda el
grafo justo como un diccionario de diccionarios.
