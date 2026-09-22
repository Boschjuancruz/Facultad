# Armador y Balanceador de Equipos de Fútbol

Un script interactivo en Python diseñado para organizar y nivelar partidos de fútbol (5 vs 5 o 6 vs 6). El programa utiliza un algoritmo inteligente que distribuye a los jugadores en dos equipos, buscando la menor diferencia de nivel posible basándose en sus atributos individuales y roles tácticos.

##  Características Principales

* **Modos de Juego:** Soporta selección dinámica para partidos de 5 o 6 jugadores por lado.
* **Algoritmo de Balanceo Inteligente:** Calcula la fuerza neta de cada jugador mediante la sumatoria de sus estadísticas individuales (velocidad, resistencia, tiro, defensa, atajada y pase).
* **Gestión Táctica de Posiciones:** Asegura que los equipos estén equilibrados estructuralmente:
  * `ARQ` (Arquero): Distribución equitativa de los especialistas bajo los tres palos.
  * `DEF` (Defensor), `MED` (Mediocampista), `DEL` (Delantero): Reparto parejo y ordenado por nivel de fuerza.
  * `POL` (Polivalente): Sistema de comodines que analiza las debilidades de la formación en tiempo real (ej. falta de defensores) y asigna al jugador polivalente para cubrir ese hueco.
* **Interfaz de Usuario (CLI):** Menú interactivo por consola que permite seleccionar jugadores de una base de datos predefinida y muestra los resultados en una tabla ASCII ordenada con estadísticas de puntaje.

##  Cómo utilizarlo

1. Asegurate de tener **Python 3** instalado en tu sistema.
2. Ejecutá el script desde la terminal o consola:
   ```bash
   python armador_equipos.py
