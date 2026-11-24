"""Tests de la práctica 09. Ejecutar desde esta carpeta:

    python -m unittest -v test_red_social

Cada clase de test corresponde a un ejercicio; puedes lanzar solo una:

    python -m unittest -v test_red_social.TestEjercicio1
    python -m unittest -v test_red_social.TestEjercicio2
    python -m unittest -v test_red_social.TestEjercicio3
    python -m unittest -v test_red_social.TestEjercicioExtra

Al ejecutarlos, cada ejercicio sale con su cabecera y cada test indica a qué
ejercicio pertenece, por ejemplo:

    ======== Ejercicio 1: construir la red ========
    [Ej. 1] tamano_y_grados ... ok
"""

import sys
import time
import unittest

from red_social import RedSocial, red_de_ejemplo


def cadena_larga(n):
    """p0 - p1 - p2 - ... - p(n-1): un grafo en fila de n personas."""
    personas = [f"p{i}" for i in range(n)]
    amistades = [(f"p{i}", f"p{i + 1}") for i in range(n - 1)]
    return RedSocial(personas, amistades)


class _TestDeEjercicio(unittest.TestCase):
    """Base común: escribe la cabecera del ejercicio y pone delante de cada
    test su etiqueta, tanto en la lista de resultados como en los fallos."""

    ETIQUETA = ""   # por ejemplo, "Ej. 1"
    TITULO = ""     # por ejemplo, "Ejercicio 1: construir la red"

    @classmethod
    def setUpClass(cls):
        # unittest escribe sus resultados en stderr: la cabecera va al mismo
        # sitio para que salga en orden.
        sys.stderr.write(f"\n======== {cls.TITULO} ========\n")
        sys.stderr.flush()

    def __str__(self):
        nombre = self._testMethodName[len("test_"):]   # quita el "test_" del principio
        return f"[{self.ETIQUETA}] {nombre}"

    def shortDescription(self):
        return None   # que no añada una segunda línea con el docstring


class TestEjercicio1(_TestDeEjercicio):
    ETIQUETA = "Ej. 1"
    TITULO = "Ejercicio 1: construir la red"

    def test_tamano_y_grados(self):
        r = red_de_ejemplo()
        self.assertEqual(r.num_personas(), 11)
        self.assertEqual(r.num_amistades(), 11)
        self.assertEqual(r.grado("Carla"), 4)
        self.assertEqual(r.grado("Lucas"), 0)
        suma = sum(r.grado(p) for p in r.personas())
        self.assertEqual(suma, 2 * r.num_amistades())

    def test_las_dos_direcciones(self):
        r = red_de_ejemplo()
        self.assertEqual(r.amigos("Ana"), ["Bruno", "Carla"])
        self.assertEqual(r.amigos("Carla"), ["Ana", "Bruno", "David", "Elena"])
        self.assertTrue(r.son_amigos("Gloria", "Fede"))
        self.assertTrue(r.son_amigos("Fede", "Gloria"))
        self.assertFalse(r.son_amigos("Ana", "Gloria"))

    def test_duplicada_no_cuenta(self):
        r = red_de_ejemplo()
        self.assertFalse(r.anadir_amistad("Carla", "Ana"))   # ya existía, al revés
        self.assertFalse(r.anadir_amistad("Ana", "Carla"))
        self.assertEqual(r.num_amistades(), 11)
        self.assertEqual(r.amigos("Ana"), ["Bruno", "Carla"])

    def test_nueva_amistad(self):
        r = red_de_ejemplo()
        self.assertTrue(r.anadir_amistad("Lucas", "Ana"))
        self.assertEqual(r.num_amistades(), 12)
        self.assertEqual(r.amigos("Lucas"), ["Ana"])
        self.assertEqual(r.amigos("Ana"), ["Bruno", "Carla", "Lucas"])

    def test_errores(self):
        r = red_de_ejemplo()
        with self.assertRaises(ValueError):
            r.anadir_amistad("Ana", "Ana")
        with self.assertRaises(ValueError):
            r.anadir_amistad("Ana", "Zoe")
        with self.assertRaises(ValueError):
            r.anadir_persona("Ana")
        self.assertEqual(r.num_amistades(), 11)

    def test_amigos_en_comun(self):
        r = red_de_ejemplo()
        self.assertEqual(r.amigos_en_comun("Ana", "David"), ["Bruno", "Carla"])
        self.assertEqual(r.amigos_en_comun("David", "Ana"), ["Bruno", "Carla"])
        self.assertEqual(r.amigos_en_comun("Bruno", "Elena"), ["Carla"])
        self.assertEqual(r.amigos_en_comun("Ana", "Gloria"), [])
        self.assertEqual(r.amigos_en_comun("Ana", "Lucas"), [])

    def test_invariante(self):
        r = red_de_ejemplo()
        for p in r.personas():
            self.assertNotIn(p, r.amigos(p))
            self.assertEqual(len(r.amigos(p)), len(set(r.amigos(p))))
            for q in r.amigos(p):
                self.assertIn(p, r.amigos(q))


class TestEjercicio2(_TestDeEjercicio):
    ETIQUETA = "Ej. 2"
    TITULO = "Ejercicio 2: los grupos"

    def test_etiquetas(self):
        r = red_de_ejemplo()
        self.assertEqual(r.etiquetas_de_grupo(), [0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2])

    def test_grupos(self):
        r = red_de_ejemplo()
        self.assertEqual(r.grupos(), [
            ["Ana", "Bruno", "Carla", "David", "Elena", "Fede", "Gloria"],
            ["Hugo", "Irene", "Jorge"],
            ["Lucas"],
        ])

    def test_mismo_grupo(self):
        r = red_de_ejemplo()
        self.assertTrue(r.mismo_grupo("Ana", "Gloria"))
        self.assertTrue(r.mismo_grupo("Jorge", "Hugo"))
        self.assertFalse(r.mismo_grupo("Ana", "Hugo"))
        self.assertTrue(r.mismo_grupo("Lucas", "Lucas"))

    def test_un_puente_une_grupos(self):
        r = red_de_ejemplo()
        r.anadir_amistad("Gloria", "Jorge")
        self.assertEqual(len(r.grupos()), 2)
        self.assertTrue(r.mismo_grupo("Ana", "Hugo"))

    def test_red_vacia_y_sin_amistades(self):
        self.assertEqual(RedSocial().grupos(), [])
        r = RedSocial(["X", "Y", "Z"])
        self.assertEqual(r.grupos(), [["X"], ["Y"], ["Z"]])

    def test_cadena_de_100000_sin_recursion(self):
        # Con recursión, Python se para a unas 1000 llamadas (RecursionError).
        r = cadena_larga(100_000)
        g = r.grupos()
        self.assertEqual(len(g), 1)
        self.assertEqual(len(g[0]), 100_000)


class TestEjercicio3(_TestDeEjercicio):
    ETIQUETA = "Ej. 3"
    TITULO = "Ejercicio 3: grados de separación"

    def test_separacion(self):
        r = red_de_ejemplo()
        self.assertEqual(r.separacion("Ana", "Ana"), 0)
        self.assertEqual(r.separacion("Ana", "Carla"), 1)
        self.assertEqual(r.separacion("Ana", "Elena"), 2)
        self.assertEqual(r.separacion("Ana", "Gloria"), 4)
        self.assertEqual(r.separacion("Gloria", "Ana"), 4)
        self.assertEqual(r.separacion("Ana", "Hugo"), -1)
        self.assertEqual(r.separacion("Lucas", "Ana"), -1)

    def test_niveles(self):
        r = red_de_ejemplo()
        self.assertEqual(r.niveles("Ana"), [
            ["Ana"], ["Bruno", "Carla"], ["David", "Elena"], ["Fede"], ["Gloria"],
        ])

    def test_cadena(self):
        r = red_de_ejemplo()
        self.assertEqual(r.cadena("Ana", "Gloria"),
                         ["Ana", "Bruno", "David", "Fede", "Gloria"])
        self.assertEqual(r.cadena("Gloria", "Ana"),
                         ["Gloria", "Fede", "David", "Bruno", "Ana"])
        self.assertEqual(r.cadena("Elena", "Bruno"), ["Elena", "Carla", "Bruno"])
        self.assertEqual(r.cadena("Ana", "Ana"), ["Ana"])
        self.assertEqual(r.cadena("Ana", "Lucas"), [])

    def test_cadena_es_un_camino_valido(self):
        r = red_de_ejemplo()
        for a in r.personas():
            for b in r.personas():
                c = r.cadena(a, b)
                d = r.separacion(a, b)
                if d == -1:
                    self.assertEqual(c, [])
                    continue
                self.assertEqual(len(c), d + 1)
                self.assertEqual((c[0], c[-1]), (a, b))
                for x, y in zip(c, c[1:]):
                    self.assertTrue(r.son_amigos(x, y))

    def test_escala(self):
        # Una fila de 100 000 personas y un árbol binario de 131 071.
        r = cadena_larga(100_000)
        self.assertEqual(r.separacion("p0", "p99999"), 99_999)
        self.assertEqual(len(r.cadena("p0", "p99999")), 100_000)
        n = 2**17 - 1
        arbol = RedSocial([f"p{i}" for i in range(n)],
                          [(f"p{(i - 1) // 2}", f"p{i}") for i in range(1, n)])
        inicio = time.perf_counter()
        self.assertEqual(arbol.separacion("p0", f"p{n - 1}"), 16)
        print(f"({1000 * (time.perf_counter() - inicio):.0f} ms el BFS en un "
              f"árbol de {n} personas)", end=" ", file=sys.stderr)


class TestEjercicioExtra(_TestDeEjercicio):
    ETIQUETA = "Extra"
    TITULO = "Ejercicio extra: personas que quizá conozcas"

    def test_sugerencias_ejemplo(self):
        r = red_de_ejemplo()
        self.assertEqual(r.sugerencias("Ana"), [("David", 2), ("Elena", 1)])
        self.assertEqual(r.sugerencias("Fede"), [("Carla", 2), ("Bruno", 1)])
        self.assertEqual(r.sugerencias("David"), [("Ana", 2), ("Elena", 2), ("Gloria", 1)])
        self.assertEqual(r.sugerencias("Carla"), [("Fede", 2)])

    def test_empates_por_numero(self):
        r = red_de_ejemplo()
        self.assertEqual(r.sugerencias("Gloria"), [("David", 1), ("Elena", 1)])

    def test_sin_sugerencias(self):
        r = red_de_ejemplo()
        self.assertEqual(r.sugerencias("Lucas"), [])
        self.assertEqual(r.sugerencias("Irene"), [])
        self.assertEqual(r.sugerencias("Hugo"), [("Jorge", 1)])

    def test_nunca_uno_mismo_ni_amigos(self):
        r = red_de_ejemplo()
        for p in r.personas():
            for candidato, comunes in r.sugerencias(p):
                self.assertNotEqual(candidato, p)
                self.assertFalse(r.son_amigos(p, candidato))
                self.assertEqual(comunes, len(r.amigos_en_comun(p, candidato)))
                self.assertEqual(r.separacion(p, candidato), 2)


if __name__ == "__main__":
    unittest.main(verbosity=2)