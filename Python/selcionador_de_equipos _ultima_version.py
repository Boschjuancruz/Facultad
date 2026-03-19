import sys

# --- (Las definiciones de jugadores y listas se mantienen igual que arriba) ---
santi = {"nombre": "Santi", "posicion": "DEL", "velocidad": 8.5, "resitencia": 7, "tiro al arco": 8.875, "defensa": 8.5, "arquero": 6.28, "pases": 8.16}
Matu = {"nombre": "Matu", "posicion": "MED", "velocidad": 6.25, "resitencia": 6.6, "tiro al arco": 6.625, "defensa": 6.85, "arquero": 6, "pases": 7.16}
Martin = {"nombre": "Martin", "posicion": "DEF", "velocidad": 7.57, "resitencia": 7.85, "tiro al arco": 7.33, "defensa": 7.83, "arquero": 7.16, "pases": 7.6}
Cuki = {"nombre": "Cuki", "posicion": "POL", "velocidad": 6.83, "resitencia": 7, "tiro al arco": 8.42, "defensa": 8.4, "arquero": 7.125, "pases": 7.83}
Pedro = {"nombre": "pedro", "posicion": "DEL", "velocidad": 7.5, "resitencia": 7.14, "tiro al arco": 7.5, "defensa": 7.2, "arquero": 7, "pases": 7.14}
Juani = {"nombre": "juani", "posicion": "MED", "velocidad": 7.16, "resitencia": 7.125, "tiro al arco": 7.57, "defensa": 7, "arquero": 5.375, "pases": 7.75}
Goyo = {"nombre": "goyo", "posicion": "MED", "velocidad": 7.875, "resitencia": 8.5, "tiro al arco": 6.71, "defensa": 7.71, "arquero": 6.375, "pases": 6.875}
Marce = {"nombre": "Marce", "posicion": "DEF", "velocidad": 7, "resitencia": 7, "tiro al arco": 7.6, "defensa": 9, "arquero": 5.75, "pases": 7.71}
Lucas = {"nombre": "Lucas", "posicion": "MED", "velocidad": 6, "resitencia": 7.85, "tiro al arco": 6.57, "defensa": 6.83, "arquero": 5.625, "pases": 6.625}
Alfonso = {"nombre": "alfonso", "posicion": "MED", "velocidad": 8.2, "resitencia": 8, "tiro al arco": 6.5, "defensa": 6.875, "arquero": 8.57, "pases": 6.875}
Nahu = {"nombre": "Nahu", "posicion": "POL", "velocidad": 8.15, "resitencia": 8, "tiro al arco": 7.85, "defensa": 8.125, "arquero": 5.85, "pases": 8}
Osuna = {"nombre": "osuna", "posicion": "ARQ", "velocidad": 4.12, "resitencia": 4.5, "tiro al arco": 3.87, "defensa": 6, "arquero": 8.5, "pases": 5.5}
Tomi_R = {"nombre": "Tomi_R", "posicion": "DEF", "velocidad": 5.5, "resitencia": 5.625, "tiro al arco": 6.28, "defensa": 8, "arquero": 6.37, "pases": 7.42}
martincito = {"nombre": "martincito", "posicion": "DEL", "velocidad": 7, "resitencia": 6.625, "tiro al arco": 7.5, "defensa": 5.57, "arquero": 3.875, "pases": 7.25}
Jero = {"nombre": "Jero", "posicion": "DEF", "velocidad": 7, "resitencia": 7.71, "tiro al arco": 7.5, "defensa": 8.28, "arquero": 6.57, "pases": 7.57}
esteban = {"nombre": "esteban", "posicion": "MED", "velocidad": 5.625, "resitencia": 6.5, "tiro al arco": 6.25, "defensa": 7.5, "arquero": 6.125, "pases": 7.875}
pedro_N = {"nombre": "pedro_N", "posicion": "MED", "velocidad": 7, "resitencia": 6, "defensa": 5, "tiro al arco": 6, "arquero": 6, "pases": 6}

LISTA_MAESTRA = [santi, Matu, Cuki, Martin, Juani, Marce, esteban, Nahu, Osuna, Tomi_R, martincito, pedro_N, Jero, Alfonso, Goyo, Lucas, Pedro]

def calcular_fuerza_jugador(jugador):
    return sum(valor for hab, valor in jugador.items() if hab not in ["nombre", "posicion"])

def calcular_suma_total_equipo(equipo):
    if not equipo: return 0
    habilidades = [k for k in equipo[0].keys() if k not in ["nombre", "posicion"]]
    return sum(sum(j.get(hab, 0) for j in equipo) for hab in habilidades)

# --- BALANCEO INTELIGENTE ---

def balancear_equipos(jugadores_seleccionados):
    e1, e2 = [], []
    f_e1, f_e2 = 0, 0
    max_p = len(jugadores_seleccionados) // 2
    
    # 1. Separar por categorías
    j_con_f = [(calcular_fuerza_jugador(j), j) for j in jugadores_seleccionados]
    arqs = [p for p in j_con_f if p[1]['posicion'] == 'ARQ']
    
    # Manejo de arqueros (Igual que antes pero optimizado)
    if len(arqs) == 2:
        arqs.sort(key=lambda x: x[0], reverse=True)
        e1.append(arqs[0][1]); f_e1 += arqs[0][0]
        e2.append(arqs[1][1]); f_e2 += arqs[1][0]
        restantes = [p for p in j_con_f if p[1]['posicion'] != 'ARQ']
    else:
        # Si no hay 2 arqueros, usamos los 2 mejores con skill 'arquero'
        j_con_f.sort(key=lambda p: p[1]['arquero'], reverse=True)
        a1, a2 = j_con_f[0], j_con_f[1]
        if a1[0] >= a2[0]:
            e1.append(a1[1]); f_e1 += a1[0]
            e2.append(a2[1]); f_e2 += a2[0]
        else:
            e1.append(a2[1]); f_e1 += a2[0]
            e2.append(a1[1]); f_e1 += a1[0]
        restantes = j_con_f[2:]

    # Cubos de posiciones
    cubos = {
        "DEF": [p for p in restantes if p[1]['posicion'] == 'DEF'],
        "MED": [p for p in restantes if p[1]['posicion'] == 'MED'],
        "DEL": [p for p in restantes if p[1]['posicion'] == 'DEL'],
        "POL": [p for p in restantes if p[1]['posicion'] == 'POL'],
    }

    # Repartimos posiciones fijas primero
    for pos in ["DEF", "MED", "DEL"]:
        cubo = sorted(cubos[pos], key=lambda x: x[0], reverse=True)
        for fuerza_j, jugador in cubo:
            if (f_e1 <= f_e2 and len(e1) < max_p) or len(e2) >= max_p:
                e1.append(jugador); f_e1 += fuerza_j
            else:
                e2.append(jugador); f_e2 += fuerza_j

    # 2. REPARTO INTELIGENTE DE POLIVALENTES
    # Se fijan qué posición falta llenar para equilibrar la formación
    pols = sorted(cubos["POL"], key=lambda x: x[0], reverse=True)
    for fuerza_j, jugador in pols:
        # Analizamos qué le falta a cada equipo (conteo de posiciones)
        def contar_pos(equipo, pos): return len([j for j in equipo if j['posicion'] == pos])
        
        # Prioridad 1: Llenar defensa si no hay
        # Prioridad 2: Fuerza total
        if len(e1) < max_p and (contar_pos(e1, "DEF") < contar_pos(e2, "DEF") or f_e1 <= f_e2):
            e1.append(jugador); f_e1 += fuerza_j
        elif len(e2) < max_p:
            e2.append(jugador); f_e2 += fuerza_j
        else: # Si por alguna razón e2 está lleno
            e1.append(jugador); f_e1 += fuerza_j

    return e1, e2

# --- INTERFAZ ---

def seleccionar_modo_juego():
    print("\n" + "═"*30 + "\nMODO DE JUEGO\n" + "═"*30)
    while True:
        res = input("5) 5 vs 5\n6) 6 vs 6\nq) Salir\nElección: ").lower()
        if res in ['5', '6']: return int(res)
        if res == 'q': sys.exit()

def seleccionar_jugadores(lista, cant):
    seleccionados = []
    disponibles = lista.copy()
    while len(seleccionados) < cant:
        print(f"\nSeleccionados: {len(seleccionados)}/{cant}")
        for i, j in enumerate(disponibles):
            print(f"{i+1:2}) {j['nombre']:12} | {j['posicion']}")
        
        op = input("\nNúmero del jugador (0 para reset): ")
        if op == '0': return []
        try:
            idx = int(op)-1
            if 0 <= idx < len(disponibles):
                seleccionados.append(disponibles.pop(idx))
        except: print("Error de entrada.")
    return seleccionados

def main():
    print("⚽ BIENVENIDO AL ARMADOR DE EQUIPOS ⚽")
    while True:
        modo = seleccionar_modo_juego()
        jugadores = seleccionar_jugadores(LISTA_MAESTRA, modo * 2)
        if jugadores:
            e1, e2 = balancear_equipos(jugadores)
            
            # Imprimir resultados estéticos
            f1, f2 = calcular_suma_total_equipo(e1), calcular_suma_total_equipo(e2)
            print("\n" + "╔" + "═"*38 + "╗")
            print(f"║ {'EQUIPOS DEFINIDOS':^36} ║")
            print("╠" + "═"*38 + "╣")
            print(f"║ EQUIPO 1 ({f1:.1f} pts):")
            for j in e1: print(f"║  - {j['nombre']:12} ({j['posicion']})")
            print(f"║\n║ EQUIPO 2 ({f2:.1f} pts):")
            for j in e2: print(f"║  - {j['nombre']:12} ({j['posicion']})")
            print("╚" + "═"*38 + "╝")
            print(f"Diferencia de nivel: {abs(f1-f2):.2f}")
        
        if input("\n¿Otra vez? (s/n): ").lower() != 's': break

if __name__ == "__main__":
    main()