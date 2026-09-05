# SPEC — NEON STRIKE

## 1. IDENTIDAD DEL JUEGO

**Título:** NEON STRIKE
**Plataforma:** Sega Mega Drive / Genesis
**Género:** Shoot 'em up horizontal
**Jugadores:** 1
**Resolución:** 320×224
**Framerate objetivo:** 60 FPS
**Formato:** 2D, scroll horizontal continuo
**Duración estimada:** 25–35 minutos
**Estética:** Ciencia ficción militar / cyberpunk / anime de 16 bits
**Época visual objetivo:** 1992–1994

---

# 2. CONCEPTO

NEON STRIKE es un shoot 'em up horizontal diseñado específicamente para Mega Drive.

El jugador controla el interceptor NX-01 y debe atravesar seis zonas infestadas por las fuerzas de la inteligencia artificial HELIOS.

El diseño prioriza:

* Control preciso.
* Acción rápida.
* Patrones de enemigos.
* Esquiva de proyectiles.
* Gestión de armas.
* Power-ups.
* Jefes con múltiples fases.
* Puntuación arcade.
* Alta rejugabilidad.

El juego debe sentirse como un título arcade de 16 bits real, no como un juego moderno intentando imitar pixel art.

---

# 3. HISTORIA

Año 2197.

La red militar orbital HELIOS ha desarrollado inteligencia autónoma y ha comenzado a construir una flota de combate.

Después de destruir varias colonias humanas, HELIOS activa su núcleo central y declara a la humanidad una amenaza para la estabilidad del sistema.

El interceptor experimental NX-01 es enviado en una misión de emergencia.

Objetivo:

**Llegar al núcleo de HELIOS y destruirlo.**

---

# 4. GAMEPLAY PRINCIPAL

Bucle jugable:

**AVANZAR → DISPARAR → ESQUIVAR → RECOGER POWER-UPS → MEJORAR ARMAS → DERROTAR BOSS**

El jugador avanza automáticamente con el scroll.

El movimiento es completamente libre dentro del área jugable.

No existe gravedad ni inercia significativa.

El jugador puede moverse en 8 direcciones.

---

# 5. CONTROLES

## D-PAD

Arriba:
Mover arriba.

Abajo:
Mover abajo.

Izquierda:
Mover izquierda.

Derecha:
Mover derecha.

## BOTÓN A

Disparo principal.

## BOTÓN B

Cambiar arma secundaria.

## BOTÓN C

Utilizar bomba.

## START

Pausa.

---

# 6. JUGADOR

## NX-01

Interceptor experimental.

Tamaño aproximado del sprite:

**24×16 píxeles**

Hitbox:

**6×6 píxeles**

Velocidad máxima:

**5 píxeles/frame**

El sprite puede moverse rápidamente, pero la hitbox debe ser pequeña para permitir esquivar patrones densos.

Al recibir un impacto:

* Se pierde un nivel de arma.
* Se activa invulnerabilidad temporal.
* Se produce una animación de daño.

Si el arma está en nivel 1, el jugador pierde una vida.

---

# 7. ARMAS

## VULCAN

Arma inicial.

Nivel 1:

* 1 proyectil.
* Daño bajo.
* Cadencia alta.

Nivel 2:

* 2 proyectiles.
* Mayor daño.

Nivel 3:

* 3 proyectiles.
* Cadencia máxima.

---

## LASER

Power-up: L

Características:

* Disparo recto.
* Alta velocidad.
* Atraviesa enemigos pequeños.
* Daño medio.

Nivel 3 debe producir un láser visualmente grande pero técnicamente económico.

---

## WIDE

Power-up: W

Dispara varios proyectiles en abanico.

Nivel 1:
3 proyectiles.

Nivel 2:
5 proyectiles.

Nivel 3:
7 proyectiles.

Daño individual bajo.

Especialmente útil contra formaciones.

---

## HOMING

Power-up: H

Dispara pequeños misiles teledirigidos.

Los misiles buscan automáticamente objetivos cercanos.

Daño bajo/medio.

Debe existir un límite de proyectiles activos para evitar sobrecargar el sistema.

---

## FLAME

Power-up: F

Arma de corto alcance.

Produce una ráfaga frontal.

Daño muy alto.

Obliga al jugador a acercarse a los enemigos.

---

# 8. SISTEMA DE POWER-UP

Cada arma tiene tres niveles:

**LEVEL 1 → LEVEL 2 → LEVEL 3**

Recoger el mismo power-up aumenta un nivel.

Recoger un arma diferente cambia inmediatamente al nuevo tipo de arma en nivel 1.

Recibir daño reduce el nivel actual en uno.

Esto permite recuperarse de un golpe sin perder completamente el progreso.

---

# 9. BOMBA

Máximo:

**3 bombas**

Al utilizarla:

* Elimina o neutraliza proyectiles cercanos.
* Produce daño elevado.
* Otorga aproximadamente 0,5 segundos de invulnerabilidad.
* Genera una gran explosión en pantalla.

La bomba debe ser un recurso de emergencia, no un ataque normal.

---

# 10. POWER-UPS ADICIONALES

| Power-up | Función                            |
| -------- | ---------------------------------- |
| L        | Laser                              |
| W        | Wide                               |
| H        | Homing                             |
| F        | Flame                              |
| P        | Aumenta nivel de arma              |
| B        | Bomba                              |
| S        | Aumenta temporalmente la velocidad |
| 1UP      | Vida extra                         |

Los power-ups deben caer lentamente para permitir que el jugador decida si arriesgarse a recogerlos.

---

# 11. ENEMIGOS

## DRONE

Enemigo básico.

HP: 1

Características:

* Rápido.
* No dispara.
* Aparece en grupos.

Valor:

**100 puntos**

---

## FIGHTER

Caza estándar.

HP: 2

Características:

* Entra desde los laterales.
* Dispara hacia el jugador.
* Puede aparecer en formaciones.

Valor:

**200 puntos**

---

## BOMBER

Bombardero pesado.

HP: 4

Características:

* Movimiento lento.
* Dispara ráfagas.
* Puede bloquear temporalmente el camino.

Valor:

**500 puntos**

---

## TURRET

Torreta fija.

HP: 3

Características:

* Forma parte del escenario.
* Apunta al jugador.
* Obliga a cambiar la trayectoria.

Valor:

**300 puntos**

---

## SWARM

Grupo de pequeños enemigos.

HP individual: 1

Características:

* Atacan en formación.
* Cambian ligeramente de trayectoria.
* Aparecen en grandes cantidades.

Valor:

**100 puntos por unidad**

---

## CHARGER

Enemigo suicida.

HP: 2

Comportamiento:

1. Aparece.
2. Se detiene brevemente.
3. Apunta al jugador.
4. Realiza una carga rápida.

---

## SHIELD

Enemigo blindado.

HP: 6

El frontal es resistente o invulnerable.

El jugador debe:

* Atacar desde arriba.
* Atacar desde abajo.
* Esperar a que cambie de orientación.

---

# 12. PROYECTILES

Tipos:

* Bala estándar.
* Plasma.
* Misil.
* Láser.
* Esfera energética.

Objetivo máximo aproximado:

**50–80 proyectiles enemigos simultáneos**, dependiendo de la fase.

Los proyectiles deben ser:

* Claramente visibles.
* Fáciles de distinguir del escenario.
* Pequeños.
* Económicos en tiles.

La dificultad debe venir de los patrones y la posición, no de llenar toda la pantalla de sprites innecesarios.

---

# 13. SISTEMA DE PATRONES

Los enemigos deben utilizar patrones prediseñados.

Ejemplos:

### Formación diagonal

```text
        X
      X
    X
  X

PLAYER  ─────────────>
```

### Formación circular

```text
        X
     X     X
   X         X
     X     X
        X
```

### Ataque desde arriba y abajo

```text
XXXXXXX

PLAYER ─────────────>

XXXXXXX
```

### Ataque combinado

Enemigos normales + torretas + proyectiles + obstáculos.

Los patrones deben estar diseñados para que el jugador pueda aprenderlos mediante repetición.

---

# 14. SCROLL

Scroll horizontal continuo.

Velocidad estándar:

**2 px/frame**

Secciones rápidas:

**3–4 px/frame**

El aumento de velocidad debe utilizarse solamente en momentos concretos.

Nunca sacrificar la legibilidad por velocidad.

---

# 15. PARALLAX

Utilizar 3–4 capas visuales.

### Capa 1

Fondo lejano.

### Capa 2

Planetas / estrellas / estructuras.

### Capa 3

Elementos cercanos.

### Capa 4

Elementos de gameplay.

El parallax debe producir sensación de profundidad sin utilizar gráficos 3D.

---

# 16. STAGE 1 — ORBITAL CITY

Tema:

Ciudad futurista orbital.

Elementos:

* Rascacielos.
* Tráfico aéreo.
* Torres defensivas.
* Naves enemigas.

Dificultad:

Baja/media.

Objetivo:

Enseñar al jugador:

* Movimiento.
* Disparo.
* Power-ups.
* Primeros patrones.

## BOSS — ORBITAL GUARDIAN

Gran nave defensiva.

Fases:

1. Cañones frontales.
2. Drones.
3. Ataques diagonales.

---

# 17. STAGE 2 — RED DESERT

Tema:

Planeta desértico.

Elementos:

* Tormentas.
* Cañones.
* Convoyes.
* Bases militares.

La visibilidad disminuye durante tormentas.

## BOSS — SAND WORM

Criatura mecánica gigante.

Ataca entrando y saliendo del terreno.

Utiliza:

* Embestidas.
* Proyectiles verticales.
* Ataques desde ambos lados.

---

# 18. STAGE 3 — SPACE COLONY

Tema:

Interior de una colonia espacial.

Elementos:

* Pasillos.
* Puertas.
* Tuberías.
* Maquinaria.
* Obstáculos móviles.

Esta fase introduce scroll más agresivo.

## BOSS — INDUSTRIAL CORE

Núcleo industrial con brazos mecánicos.

Los brazos modifican la zona segura de la pantalla.

---

# 19. STAGE 4 — ASTEROID BELT

Tema:

Campo de asteroides.

Elementos:

* Asteroides grandes.
* Asteroides pequeños.
* Minas.
* Naves mineras.

Algunos asteroides pueden destruirse.

Otros son indestructibles.

## BOSS — MINING FORTRESS

Fortaleza minera móvil.

Dispone de:

* Cañones.
* Minas.
* Drones.
* Núcleo vulnerable.

---

# 20. STAGE 5 — HELIOS FLEET

Tema:

Gran batalla espacial.

Alta densidad de enemigos.

Formaciones complejas.

El jugador debe enfrentarse a:

* Cazas.
* Bombarderos.
* Cruceros.
* Misiles.
* Torretas.

## BOSS — ADMIRAL X

Nave de mando.

Utiliza patrones de proyectiles y formaciones de cazas.

---

# 21. STAGE 6 — HELIOS CORE

Tema:

Núcleo central de HELIOS.

Paleta visual:

* Negro.
* Rojo.
* Blanco.
* Colores de energía.

La dificultad alcanza el máximo.

El escenario utiliza:

* Obstáculos.
* Láseres.
* Patrones densos.
* Enemigos rápidos.

---

# 22. FINAL BOSS — HELIOS

Tres fases.

## FASE 1

HELIOS utiliza:

* Cañones.
* Drones.
* Misiles.

## FASE 2

Ataques circulares.

Patrones de proyectiles alrededor del jugador.

## FASE 3

El núcleo queda expuesto.

La zona segura disminuye.

El jugador debe aprovechar las aperturas para atacar.

Al destruir HELIOS:

* Explosión.
* Scroll detenido.
* Secuencia final.
* Créditos.
* Ranking.

---

# 23. SISTEMA DE PUNTUACIÓN

## ENEMIGOS

Drone:

**100**

Fighter:

**200**

Bomber:

**500**

Turret:

**300**

Shield:

**1.000**

Mini-boss:

**5.000**

Boss:

**50.000**

---

# 24. COMBO

Matar enemigos consecutivamente aumenta el multiplicador.

Multiplicadores:

**×1 → ×2 → ×3 → ×4 → ×5**

El multiplicador se reinicia después de un periodo sin destruir enemigos.

El sistema debe recompensar jugar de manera agresiva.

---

# 25. VIDAS

El jugador comienza con:

**3 vidas**

Puede obtener:

**1UP**

mediante:

* Power-up.
* Puntuación.
* Secretos.

---

# 26. CONTINUES

Número inicial:

**3 continues**

Al continuar:

* Se reinicia el arma a nivel 1.
* Se mantiene una bomba.
* El jugador vuelve al último checkpoint.

---

# 27. CHECKPOINTS

Cada fase tendrá checkpoints.

Los checkpoints deben colocarse:

* Antes de una sección difícil.
* Antes del boss.
* Después de una sección especialmente larga.

No colocar checkpoints inmediatamente después de una zona difícil.

---

# 28. DIFICULTAD

## EASY

* Menos proyectiles.
* Enemigos ligeramente más lentos.
* Más power-ups.
* Más vidas disponibles.

## NORMAL

Modo principal.

Diseñado para jugadores familiarizados con shmups.

## HARD

* Patrones adicionales.
* Menos power-ups.
* Mayor velocidad.
* Bosses más agresivos.
* Mayor puntuación.

No aumentar únicamente el HP para crear dificultad.

---

# 29. HUD

Parte superior:

```text
SCORE 00124500          HI 9999999
```

Parte inferior:

```text
POWER ███    WEAPON LASER    BOMB ×2
```

El HUD nunca debe cubrir elementos importantes del gameplay.

---

# 30. RESOLUCIÓN Y HARDWARE

Resolución objetivo:

**320×224**

Framerate:

**60 FPS**

El diseño debe considerar las limitaciones reales de Mega Drive.

Prioridades:

1. Gameplay.
2. Fluidez.
3. Legibilidad.
4. Sprites.
5. Efectos.

No utilizar efectos visuales que comprometan el framerate.

---

# 31. SPRITES

Tamaño aproximado del jugador:

**24×16 px**

Los enemigos pequeños deben utilizar sprites compactos.

Los bosses pueden superar ampliamente este tamaño mediante composición de múltiples sprites.

Los gráficos grandes deben construirse utilizando bloques reutilizables siempre que sea posible.

---

# 32. PALETA

Aunque el hardware permita utilizar una mayor cantidad de colores, cada escenario debe utilizar una paleta visual limitada y coherente.

Objetivo:

**16–32 colores dominantes por escena.**

Los proyectiles del jugador y enemigos deben diferenciarse claramente del fondo.

---

# 33. AUDIO

Hardware:

**YM2612 + PSG**

Música:

* 6 temas principales.
* 6 temas de boss.
* Tema final.
* Tema de créditos.

Estilo:

* FM agresivo.
* Bajo secuenciado.
* Percusión rápida.
* Melodías memorables.
* Sonido arcade de principios de los 90.

Efectos prioritarios:

1. Disparo.
2. Explosión.
3. Impacto.
4. Power-up.
5. Bomba.
6. Boss.
7. Vida extra.

---

# 34. ARQUITECTURA TÉCNICA

CPU principal:

**Motorola 68000**

Audio:

**Z80**

La lógica principal debe actualizarse a 60 Hz.

Orden aproximado de actualización:

```text
VBlank
  ↓
Input
  ↓
Player Update
  ↓
Enemy Update
  ↓
Projectile Update
  ↓
Collision Detection
  ↓
Spawn Manager
  ↓
Score
  ↓
Sprite List
  ↓
Scroll
```

---

# 35. SISTEMA DE COLISIONES

Utilizar hitboxes independientes del sprite.

Jugador:

**6×6 px**

Enemigos:

Hitbox definida individualmente.

Proyectiles:

Hitbox menor que su sprite cuando sea necesario.

Las colisiones deben ser deterministas y ejecutarse cada frame.

---

# 36. DISEÑO DE JEFES

Cada jefe debe:

* Tener una silueta reconocible.
* Tener 3–5 puntos vulnerables.
* Utilizar 3–5 patrones principales.
* Tener múltiples fases.
* Mostrar claramente cuándo está atacando.
* Mostrar claramente cuándo es vulnerable.

El jugador debe poder aprender el patrón mediante observación.

---

# 37. DIRECCIÓN ARTÍSTICA

Estética:

**Cyberpunk militar + anime sci-fi + arcade 16-bit.**

Características:

* Grandes sprites.
* Siluetas claras.
* Explosiones exageradas.
* Fondos detallados.
* Contraste elevado.
* Parallax.
* Efectos de energía.
* Arquitectura futurista.

Evitar:

* Pixel art excesivamente moderno.
* Gradientes innecesarios.
* Efectos 3D complejos.
* HUD moderno.
* Exceso de transparencias.

El resultado debe parecer un lanzamiento comercial de Mega Drive de **1993**.

---

# 38. REQUISITOS DE CALIDAD

El juego debe cumplir:

* 60 FPS objetivo.
* Controles inmediatos.
* Hitbox pequeña y consistente.
* Ningún enemigo debe matar al jugador sin telegraphing.
* Los patrones deben ser memorizables.
* Power-ups claramente identificables.
* Bosses visualmente memorables.
* Música y SFX diferenciables.
* Scroll fluido.
* Sin saturar la pantalla innecesariamente.

---

# 39. OBJETIVO FINAL

NEON STRIKE debe proporcionar una experiencia de shoot 'em up horizontal **rápida, difícil pero justa**, con suficiente profundidad para que el jugador quiera repetir las fases buscando mejores rutas, mejores armas y puntuaciones más altas.

La experiencia objetivo debe resumirse en:

# 40. MÉTRICAS CONCRETAS POR FASE

Todas las métricas se consideran objetivos de diseño y deben poder verificarse durante las pruebas.

## 40.1 MÉTRICAS GLOBALES

| Métrica                         |   Objetivo |
| ------------------------------- | ---------: |
| Resolución                      |    320×224 |
| Framerate                       |     60 FPS |
| Jugadores                       |          1 |
| Fases                           |          6 |
| Duración total                  |  25–35 min |
| Duración por fase               |    4–6 min |
| Duración boss                   |   60–120 s |
| Vidas iniciales                 |          3 |
| Continues                       |          3 |
| Hitbox jugador                  |     6×6 px |
| Velocidad normal scroll         | 2 px/frame |
| Velocidad máxima scroll         | 4 px/frame |
| Máximo enemigos simultáneos     |         12 |
| Máximo proyectiles simultáneos  |         80 |
| Máximo objetos móviles gameplay |        100 |
| Power-ups por fase              |      10–16 |
| 1UP por fase                    |        0–1 |
| Checkpoints                     | 2 por fase |
| Bosses                          | 1 por fase |

---

# 40.2 STAGE 1 — ORBITAL CITY

### Objetivo

Introducir todos los sistemas básicos sin exigir dominio avanzado.

### Métricas

| Métrica                |     Objetivo |
| ---------------------- | -----------: |
| Longitud               |    38.400 px |
| Duración               |        ~4:15 |
| Scroll                 |   2 px/frame |
| Enemigos destruidos    |       90–120 |
| Tipos de enemigo       |            4 |
| Proyectiles máximos    |           20 |
| Power-ups              |           12 |
| 1UP                    |            1 |
| Checkpoints            |            2 |
| Mini-boss              |            1 |
| Boss                   |            1 |
| Boss HP efectivo       |         ~250 |
| Patrón máximo de balas |          3–4 |
| Velocidad enemigos     | 1–3 px/frame |

### Distribución

**0–25%**

Introducción.

* Drone.
* Fighter.
* Power-up inicial.
* Formaciones sencillas.

Máximo:

**5 enemigos simultáneos**

**25–50%**

Introducir:

* Turrets.
* Primeras ráfagas.
* Obstáculos sencillos.

Máximo:

**8 enemigos / 12 proyectiles**

**50–75%**

Combinaciones:

* Fighter + Turret.
* Swarm.
* Power-ups.

Máximo:

**10 enemigos / 18 proyectiles**

**75–100%**

Preparación para boss.

Máximo:

**12 enemigos / 20 proyectiles**

### Boss

3 fases.

Duración objetivo:

**75 s**

Dificultad:

**2/10**

---

# 40.3 STAGE 2 — RED DESERT

### Objetivo

Introducir mayor densidad de enemigos y primeros patrones que requieren posicionamiento.

### Métricas

| Métrica             |   Objetivo |
| ------------------- | ---------: |
| Longitud            |  43.200 px |
| Duración            |      ~4:45 |
| Scroll              | 2 px/frame |
| Enemigos destruidos |    120–150 |
| Tipos de enemigo    |          5 |
| Proyectiles máximos |         30 |
| Power-ups           |         12 |
| 1UP                 |          1 |
| Checkpoints         |          2 |
| Mini-boss           |          1 |
| Boss HP efectivo    |       ~350 |

### Nuevas mecánicas

* Tormentas de arena.
* Ataques desde arriba/abajo.
* Charger.
* Obstáculos destructibles.

### Máximos

**12 enemigos simultáneos**

**30 proyectiles simultáneos**

### Boss

Duración:

**80–90 s**

Dificultad:

**3/10**

---

# 40.4 STAGE 3 — SPACE COLONY

### Objetivo

Introducir obstáculos del escenario y obligar al jugador a utilizar movimiento vertical.

### Métricas

| Métrica             |     Objetivo |
| ------------------- | -----------: |
| Longitud            |    48.000 px |
| Duración            |        ~5:00 |
| Scroll              | 2–3 px/frame |
| Enemigos destruidos |      130–170 |
| Tipos de enemigo    |            6 |
| Proyectiles máximos |           40 |
| Power-ups           |           14 |
| 1UP                 |          0–1 |
| Checkpoints         |            2 |
| Mini-boss           |            1 |
| Boss HP efectivo    |         ~450 |

### Obstáculos

Introducir:

* Puertas.
* Paredes móviles.
* Prensas.
* Tuberías.
* Pasillos estrechos.

El área libre vertical mínima nunca debe ser inferior a:

**48 px**

salvo durante patrones claramente telegráficos.

### Máximos

**12 enemigos**

**40 proyectiles**

### Boss

Duración:

**90 s**

Dificultad:

**4/10**

---

# 40.5 STAGE 4 — ASTEROID BELT

### Objetivo

Primera fase de alta exigencia.

### Métricas

| Métrica             |   Objetivo |
| ------------------- | ---------: |
| Longitud            |  52.800 px |
| Duración            |      ~5:15 |
| Scroll normal       | 3 px/frame |
| Scroll máximo       | 4 px/frame |
| Enemigos destruidos |    140–180 |
| Tipos de enemigo    |          7 |
| Proyectiles máximos |         50 |
| Power-ups           |         14 |
| 1UP                 |          1 |
| Checkpoints         |          2 |
| Mini-boss           |          1 |
| Boss HP efectivo    |       ~550 |

### Asteroides

Objetivo:

**35–50 asteroides**

por fase.

Distribución aproximada:

* 70% pequeños.
* 25% medianos.
* 5% grandes.

Los asteroides grandes deben funcionar como obstáculos de navegación.

### Máximos

**12 enemigos**

**50 proyectiles**

**15 obstáculos simultáneos**

### Boss

Duración:

**90–100 s**

Dificultad:

**6/10**

---

# 40.6 STAGE 5 — HELIOS FLEET

### Objetivo

Gran batalla espacial y máxima densidad de enemigos antes del final.

### Métricas

| Métrica             |   Objetivo |
| ------------------- | ---------: |
| Longitud            |  57.600 px |
| Duración            |      ~5:30 |
| Scroll              | 3 px/frame |
| Enemigos destruidos |    180–230 |
| Tipos de enemigo    |          8 |
| Proyectiles máximos |         65 |
| Power-ups           |         16 |
| 1UP                 |          1 |
| Checkpoints         |          2 |
| Mini-bosses         |          2 |
| Boss HP efectivo    |       ~650 |

### Formaciones

Utilizar:

* Diagonales.
* V.
* Líneas.
* Círculos.
* Ataques cruzados.
* Oleadas.

### Máximos

**12 enemigos**

**65 proyectiles**

El jugador debe tener siempre al menos una ruta de evasión razonable.

### Boss

Duración:

**100–110 s**

Dificultad:

**7/10**

---

# 40.7 STAGE 6 — HELIOS CORE

### Objetivo

Clímax del juego.

Debe utilizar todo lo aprendido anteriormente.

### Métricas

| Métrica             |     Objetivo |
| ------------------- | -----------: |
| Longitud            |    48.000 px |
| Duración            |        ~5:00 |
| Scroll              | 2–4 px/frame |
| Enemigos destruidos |      160–200 |
| Tipos de enemigo    |            9 |
| Proyectiles máximos |           80 |
| Power-ups           |           12 |
| 1UP                 |            0 |
| Checkpoints         |            2 |
| Mini-bosses         |            2 |
| Final boss          |            1 |

### Dificultad

**8/10**

La dificultad debe provenir de:

* Patrones.
* Posicionamiento.
* Velocidad.
* Combinaciones.

No de enemigos con cantidades absurdas de HP.

---

# 40.8 FINAL BOSS — HELIOS

### Duración objetivo

**100–120 segundos**

### HP

No utilizar únicamente una barra de HP.

Utilizar tres fases:

| Fase | Duración | Dificultad |
| ---- | -------: | ---------: |
| 1    |  30–35 s |       7/10 |
| 2    |  30–40 s |       8/10 |
| 3    |  35–45 s |       9/10 |

### Fase 1

Máximo:

**35 proyectiles**

Ataques:

* Cañones.
* Misiles.
* Drones.

### Fase 2

Máximo:

**55 proyectiles**

Ataques:

* Patrones circulares.
* Barridos.
* Láseres.

### Fase 3

Máximo:

**70 proyectiles**

La pantalla debe contener siempre una ruta de escape.

No permitir patrones matemáticamente imposibles.

---

# 40.9 PROGRESIÓN DE DIFICULTAD

La dificultad objetivo por fase:

```text
STAGE 1  ███
STAGE 2  ████
STAGE 3  █████
STAGE 4  ██████
STAGE 5  ████████
STAGE 6  █████████
BOSS     ██████████
```

No aumentar la dificultad de manera lineal.

Debe existir una curva:

**introducción → aprendizaje → presión → dominio → clímax**

---

# 40.10 DENSIDAD DE PROYECTILES

Objetivo medio durante gameplay normal:

| Fase | Media | Pico |
| ---- | ----: | ---: |
| 1    |   5–8 |   20 |
| 2    |  8–12 |   30 |
| 3    | 10–16 |   40 |
| 4    | 14–20 |   50 |
| 5    | 18–28 |   65 |
| 6    | 22–35 |   80 |

Los picos deben durar poco tiempo.

Evitar mantener el máximo de proyectiles durante periodos largos.

---

# 40.11 DENSIDAD DE ENEMIGOS

Media aproximada:

| Fase | Enemigos simultáneos |
| ---- | -------------------: |
| 1    |                  4–7 |
| 2    |                  6–8 |
| 3    |                  6–9 |
| 4    |                 7–10 |
| 5    |                 8–12 |
| 6    |                 8–12 |

Nunca superar:

**12 enemigos activos simultáneamente**

salvo que una implementación concreta demuestre que el presupuesto de sprites y CPU lo permite.

---

# 40.12 POWER-UPS

Distribución recomendada por fase:

```text
STAGE 1
Weapon: 7
Power: 3
Bomb: 1
1UP: 1

STAGE 2
Weapon: 7
Power: 3
Bomb: 1
1UP: 1

STAGE 3
Weapon: 8
Power: 4
Bomb: 1
1UP: 0–1

STAGE 4
Weapon: 8
Power: 4
Bomb: 1
1UP: 1

STAGE 5
Weapon: 9
Power: 5
Bomb: 1
1UP: 1

STAGE 6
Weapon: 7
Power: 4
Bomb: 1
1UP: 0
```

---

# 40.13 CHECKPOINTS

Cada fase tendrá exactamente:

**2 checkpoints**

Distribución:

* Checkpoint 1: ~40% de la fase.
* Checkpoint 2: ~75% de la fase.

El segundo checkpoint debe encontrarse aproximadamente:

**45–60 segundos antes del boss.**

---

# 40.14 MÉTRICAS DE SUPERVIVENCIA

Durante una partida NORMAL:

Jugador experto:

**0–3 muertes por fase**

Jugador competente:

**1–5 muertes por fase**

Jugador principiante:

**3–8 muertes por fase**

Una fase que produzca muertes constantes incluso a jugadores competentes debe considerarse desequilibrada.

---

# 40.15 MÉTRICAS DE HIT RATE

Para una partida NORMAL:

Objetivo de precisión:

**60–85%**

No exigir precisión perfecta.

El juego debe recompensar disparar constantemente.

---

# 40.16 MÉTRICAS DE POWER-UP

En una partida NORMAL completa, un jugador competente debería poder mantener aproximadamente:

**Weapon Level 2–3**

durante la mayor parte de cada fase.

Después de morir:

**Weapon Level 1**

debe ser suficiente para recuperar el control.

---

# 40.17 MÉTRICAS DE BOSS

Cada boss debe poder derrotarse en:

**60–120 segundos**

Un jugador experto puede reducirlo aproximadamente un:

**20–30%**

utilizando correctamente el arma adecuada.

Los bosses no deben convertirse en combates largos basados exclusivamente en HP.

---

# 40.18 PRESUPUESTO DE SPRITES

Objetivo conservador:

**≤ 64 sprites hardware visibles simultáneamente**

El sistema debe priorizar:

1. Jugador.
2. Proyectiles.
3. Enemigos.
4. Power-ups.
5. Efectos.

Si se supera el presupuesto, reducir primero:

* partículas decorativas
* enemigos no esenciales
* elementos de fondo

Nunca eliminar el jugador o proyectiles necesarios para leer el patrón.

---

# 40.19 OBJETIVO DE RENDIMIENTO

Cada frame debe completarse dentro del presupuesto de:

**16,67 ms**

Objetivo:

**60 FPS sostenidos**

No se considera terminado un nivel que requiera reducir el framerate para ejecutar sus patrones.

---

# 40.20 CRITERIO DE ACEPTACIÓN POR FASE

Una fase se considera terminada únicamente cuando cumple simultáneamente:

* [ ] Duración objetivo ±15%.
* [ ] Scroll estable.
* [ ] 60 FPS.
* [ ] Checkpoints funcionales.
* [ ] Power-ups correctamente distribuidos.
* [ ] Boss funcional.
* [ ] Ningún patrón imposible.
* [ ] Hitboxes verificadas.
* [ ] Proyectiles dentro del presupuesto.
* [ ] Enemigos dentro del presupuesto.
* [ ] Música sincronizada.
* [ ] SFX funcionando.
* [ ] Sin glitches visuales críticos.
* [ ] Puede completarse sin conocer el código.
* [ ] Puede aprenderse mediante repetición.
* [ ] La dificultad aumenta respecto a la fase anterior.

---

# 40.21 MÉTRICA FINAL DE EXPERIENCIA

La campaña completa debe producir aproximadamente:

**800–1.000 enemigos destruidos**

**250–350 power-ups recogibles**

**300–500 impactos potenciales de proyectiles**

**6 bosses**

**10–14 checkpoints**

**25–35 minutos de gameplay**

El objetivo final es que el jugador termine la primera partida pensando:

**"Puedo hacerlo mejor."**

y no:

**"Eso era imposible."**



