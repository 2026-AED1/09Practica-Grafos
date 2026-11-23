"""Demo de la práctica 09: imprime lo que calcula cada ejercicio sobre la red de ejemplo.

    python demo_red_social.py

Los apartados de los ejercicios que aún no estén hechos aparecen como 'pendiente'.
Úsalo para comprobar tus predicciones de los bloques "Antes de ejecutar".
"""

from red_social import red_de_ejemplo


def apartado(titulo, funcion):
    print(f"\n=== {titulo} ===")
    try:
        funcion()
    except NotImplementedError as e:
        print(f"  (pendiente: {e})")


def ej1():
    r = red_de_ejemplo()
    print(f"  n = {r.num_personas()}  m = {r.num_amistades()}")
    for p in r.personas():
        print(f"  {p:<7} -> {' '.join(r.amigos(p)) or '-'}   (grado {r.grado(p)})")
    suma = sum(r.grado(p) for p in r.personas())
    print(f"  suma de grados = {suma} = 2 * m")
    print(f"  anadir_amistad(Carla, Ana) = {r.anadir_amistad('Carla', 'Ana')}  (ya existía)")
    print(f"  amigos_en_comun(Ana, David) = {r.amigos_en_comun('Ana', 'David')}")


def ej2():
    r = red_de_ejemplo()
    print(f"  etiquetas = {r.etiquetas_de_grupo()}")
    for i, g in enumerate(r.grupos()):
        print(f"  grupo {i}: {' '.join(g)}")
    print(f"  mismo_grupo(Ana, Gloria) = {r.mismo_grupo('Ana', 'Gloria')}   "
          f"mismo_grupo(Ana, Hugo) = {r.mismo_grupo('Ana', 'Hugo')}")


def ej3():
    r = red_de_ejemplo()
    for d, nivel in enumerate(r.niveles("Ana")):
        print(f"  nivel {d}: {' '.join(nivel)}")
    for a, b in [("Ana", "Gloria"), ("Gloria", "Ana"), ("Ana", "Hugo")]:
        c = r.cadena(a, b)
        print(f"  separacion({a}, {b}) = {r.separacion(a, b):>2}   "
              f"cadena: {' -> '.join(c) if c else '(no hay)'}")


def ej_extra():
    r = red_de_ejemplo()
    for p in ["Ana", "Fede", "Gloria", "Lucas"]:
        s = ", ".join(f"{c} ({k} en común)" for c, k in r.sugerencias(p)) or "-"
        print(f"  quizá conozcas, {p}: {s}")


apartado("Ejercicio 1: la red", ej1)
apartado("Ejercicio 2: grupos", ej2)
apartado("Ejercicio 3: grados de separación", ej3)
apartado("Ejercicio extra: personas que quizá conozcas", ej_extra)
