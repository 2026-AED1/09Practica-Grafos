"""Comentario extra de la práctica 09: ¿por qué deque y no list.pop(0)?

    python extra_medir_cola.py

Hace el mismo BFS sobre árboles binarios cada vez más grandes, con la cola
como collections.deque (popleft es O(1)) y como lista (pop(0) desplaza todos
los demás elementos: O(longitud de la cola)). En un árbol binario la última
fila tiene la mitad de los vértices, así que la cola llega a ser enorme.

No usa tu red_social.py: es independiente, solo mide.
"""

import time
from collections import deque


def arbol_binario(n):
    ady = [[] for _ in range(n)]
    for i in range(1, n):
        p = (i - 1) // 2
        ady[p].append(i)
        ady[i].append(p)
    return ady


def bfs_deque(ady, origen):
    dist = [-1] * len(ady)
    dist[origen] = 0
    cola = deque([origen])
    while cola:
        u = cola.popleft()
        for v in ady[u]:
            if dist[v] == -1:
                dist[v] = dist[u] + 1
                cola.append(v)
    return dist


def bfs_lista(ady, origen):
    dist = [-1] * len(ady)
    dist[origen] = 0
    cola = [origen]
    while cola:
        u = cola.pop(0)                 # <- el problema
        for v in ady[u]:
            if dist[v] == -1:
                dist[v] = dist[u] + 1
                cola.append(v)
    return dist


def medir(f, *args):
    t = time.perf_counter()
    f(*args)
    return 1000 * (time.perf_counter() - t)


print(f"{'n':>10} {'deque (ms)':>12} {'pop(0) (ms)':>12} {'pop(0)/deque':>14}")
for k in range(15, 20):          # el último tarda unos segundos con pop(0)
    n = 2**k - 1
    ady = arbol_binario(n)
    a, b = medir(bfs_deque, ady, 0), medir(bfs_lista, ady, 0)
    print(f"{n:>10} {a:>12.1f} {b:>12.1f} {b / a:>14.1f}")
print("\nAl doblar n, deque tarda el doble (O(n + m)); pop(0), unas cuatro veces más (O(n²)).")
