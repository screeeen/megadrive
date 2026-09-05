# NEON STRIKE — ENGINEERING & VALIDATION SPECIFICATION

## 1. ESPECIFICACIÓN TÉCNICA EJECUTABLE

El proyecto debe construirse sobre el **toolchain Mega Drive/Genesis ya existente en el repositorio**.

IMPORTANTE:

* NO cambiar el toolchain existente.
* NO reemplazar el sistema de build funcional.
* NO crear un segundo sistema de compilación.
* NO modificar innecesariamente la configuración que ya permite ejecutar el Hello World.
* El Hello World existente debe considerarse la prueba base de que el entorno funciona.
* Mantener compatibilidad con el mismo emulador actualmente configurado.
* El agente debe inspeccionar primero el proyecto existente y reutilizar su configuración.

El proyecto final debe estar implementado principalmente en C, utilizando ASM 68000 únicamente cuando sea necesario para rendimiento o acceso de bajo nivel al hardware.

La arquitectura debe separar claramente:

```text
src/
    main/
    game/
    player/
    enemies/
    weapons/
    projectiles/
    bosses/
    stages/
    collision/
    rendering/
    audio/
    input/
    hud/
    effects/
    system/

assets/
    graphics/
    maps/
    audio/
    data/

tests/
tools/

build/
```

La estructura exacta puede adaptarse al toolchain existente, pero debe mantenerse una separación clara entre:

* lógica del juego
* datos de fases
* renderizado
* audio
* input
* colisiones
* assets
* tests
* herramientas

### Requisito

El agente debe evitar hardcodear el contenido de las fases dentro de la lógica de gameplay.

Las fases deben definirse mediante datos estructurados siempre que sea posible.

Ejemplo conceptual:

```c
typedef struct {
    uint16_t frame;
    uint8_t type;
    int16_t x;
    int16_t y;
    uint8_t formation;
} SpawnEvent;
```

La lógica interpreta estos datos.

No debe existir una enorme función:

```c
stage1()
{
    ...
    spawnEnemy(...)
    ...
    spawnEnemy(...)
    ...
}
```

que contenga toda la fase codificada manualmente.

---

## 2. BUILD CONTRACT

El agente debe respetar el sistema de build existente.

Antes de modificar código debe descubrir:

1. comando de compilación
2. comando de limpieza
3. ubicación de la ROM generada
4. comando utilizado para ejecutar el juego en el emulador
5. herramientas existentes para conversión/compilación de assets
6. cualquier script de validación ya existente

El agente debe documentar estos comandos en:

```text
README.md
```

### El build final debe:

* compilar desde un checkout limpio
* generar la ROM correctamente
* incluir todos los assets necesarios
* no depender de archivos locales fuera del repositorio
* no requerir intervención manual
* no producir errores
* no producir warnings críticos
* generar siempre el mismo resultado funcional a partir de los mismos fuentes

El resultado final debe ser una ROM ejecutable.

El agente debe verificar que:

```text
source → build → ROM → emulator
```

funciona de principio a fin.

### Regla

Si el build falla:

1. diagnosticar
2. corregir
3. volver a compilar
4. volver a ejecutar las validaciones

El agente NO debe declarar la tarea terminada si el build no funciona.

---

## 3. TESTS AUTOMATIZADOS

El proyecto debe disponer de validación automatizada.

Crear, adaptar o ampliar:

```text
tests/
```

y los comandos existentes del proyecto para permitir ejecutar pruebas.

Como mínimo deben validarse estos sistemas:

### Player

* movimiento horizontal
* movimiento vertical
* límites de pantalla
* velocidad máxima
* hitbox
* pérdida de vida
* invulnerabilidad después del impacto
* pérdida de nivel de arma

### Weapons

* Vulcan
* Laser
* Wide
* Homing
* Flame
* cambio de arma
* niveles 1/2/3
* límites de proyectiles

### Power-ups

* aparición
* recogida
* cambio de arma
* aumento de nivel
* speed
* bomb
* 1UP

### Bomb

* consumo
* límite máximo
* daño
* neutralización de proyectiles
* invulnerabilidad

### Enemies

* spawn
* movimiento
* disparo
* daño
* destrucción
* puntuación

### Projectiles

* creación
* movimiento
* colisión
* destrucción
* límites máximos

### Collision

Verificar:

```text
player ↔ enemy
player ↔ projectile
playerProjectile ↔ enemy
playerProjectile ↔ boss
```

Las colisiones deben ser deterministas.

### Game state

Validar:

```text
TITLE
GAME
STAGE
BOSS
STAGE_CLEAR
NEXT_STAGE
ENDING
PAUSE
PLAYER_HIT
PLAYER_DEAD
GAME_OVER
CONTINUE
```

Las transiciones deben ser válidas y no producir estados imposibles.

### Checkpoints

Verificar:

* activación
* respawn
* reinicio de enemigos
* reinicio de proyectiles
* estado del jugador
* continuidad correcta de la fase

### Score

Verificar:

* puntuación de enemigos
* combo
* bonus
* 1UP por score
* overflow/limitaciones numéricas

### Bosses

Verificar:

* aparición
* HP
* fases
* transición entre fases
* vulnerabilidad
* patrones
* muerte
* transición a siguiente estado

---

## 4. PERFORMANCE TESTS

El objetivo técnico es mantener **60 FPS sostenidos**.

El agente debe crear mecanismos de diagnóstico en modo debug.

Como mínimo deben poder observarse:

```text
FPS
FRAME TIME
ENEMIES
PROJECTILES
SPRITES
EFFECTS
MEMORY
STAGE
PLAYER STATE
BOSS STATE
```

La información puede aparecer mediante un HUD de debug que no exista en la build final.

### Límites de diseño

```text
Resolution:        320×224
Target FPS:        60
Player hitbox:     6×6
Max enemies:       ~12
Max enemy bullets: ~80
Max visible sprites: ~64
Max gameplay objects: ~100
```

Estos valores son objetivos de seguridad, no una excusa para saturar el hardware.

### Stress tests

El agente debe probar especialmente:

1. boss activo
2. máximo número de proyectiles
3. múltiples enemigos
4. explosiones
5. power-ups
6. scroll rápido
7. múltiples sprites simultáneos
8. transición de fase
9. combinación de varios sistemas simultáneamente

No basta con comprobar que el juego funciona durante una escena vacía.

Debe probarse el peor caso razonable.

### Criterio

No se acepta una caída perceptible de rendimiento durante:

```text
boss + bullets + enemies + explosions + scroll
```

Si existe sobrecarga:

1. medir
2. identificar el cuello de botella
3. optimizar
4. repetir el benchmark

No eliminar gameplay para ocultar un problema de rendimiento.

---

## 5. VALIDACIÓN VISUAL

El agente debe utilizar el emulador existente para validar visualmente el juego.

El código compilando correctamente NO significa que el juego esté terminado.

Debe verificarse visualmente:

* pantalla de título
* HUD
* jugador
* enemigos
* proyectiles
* power-ups
* explosiones
* scroll
* parallax
* fondos
* checkpoints
* mini-bosses
* bosses
* transición entre fases
* pantalla de Game Over
* Continue
* Ending
* Credits

### Capturas

El agente debe generar capturas de las partes importantes del juego siempre que el entorno existente lo permita.

Como mínimo:

```text
title.png
stage1.png
stage2.png
stage3.png
stage4.png
stage5.png
stage6.png
boss.png
final_boss.png
ending.png
```

Las capturas deben utilizarse para detectar:

* tiles corruptos
* sprites desaparecidos
* gráficos desplazados
* paletas incorrectas
* HUD ilegible
* sprites cortados
* errores de scroll
* glitches
* fondos incorrectos
* elementos superpuestos incorrectamente

### Assets placeholder

TODOS los assets que falten deben ser generados por el agente como **placeholder funcional**.

Esto incluye:

* sprites
* enemigos
* bosses
* backgrounds
* tiles
* efectos
* power-ups
* HUD
* iconos
* fuentes
* gráficos de título

Los placeholders deben respetar:

* resolución
* tamaño
* límites de color
* contraste
* estilo 16-bit
* restricciones del hardware

No utilizar assets externos con copyright.

No detener la implementación esperando assets finales.

El objetivo es:

```text
placeholder → gameplay completo → validación → sustitución futura por arte final
```

Los placeholders deben ser suficientemente claros para validar gameplay y colisiones.

---

## 6. GAMEPLAY STATE MACHINE

El juego debe implementar una máquina de estados explícita.

Estados mínimos:

```text
TITLE
GAME
STAGE
BOSS
STAGE_CLEAR
NEXT_STAGE
PAUSE
PLAYER_HIT
PLAYER_DEAD
GAME_OVER
CONTINUE
ENDING
CREDITS
```

Debe existir una transición clara entre estados.

Ejemplo:

```text
TITLE
  ↓
STAGE
  ↓
BOSS
  ↓
STAGE_CLEAR
  ↓
NEXT_STAGE
  ↓
...
  ↓
FINAL_BOSS
  ↓
ENDING
  ↓
CREDITS
```

En caso de muerte:

```text
GAME
 ↓
PLAYER_HIT
 ↓
PLAYER_DEAD
 ↓
CONTINUE / GAME_OVER
```

### Reglas

No debe existir lógica que dependa de estados implícitos.

No utilizar múltiples flags contradictorios para representar el estado principal.

Preferir una estructura equivalente a:

```c
GameState currentState;
```

y transiciones controladas.

Cada estado debe tener claramente:

```text
enter
update
draw
exit
```

cuando sea necesario.

---

## 7. DATOS EJECUTABLES DE LAS FASES

Las fases deben estar construidas principalmente mediante datos.

Cada fase debe poder definir:

* duración
* velocidad de scroll
* background
* parallax
* spawn de enemigos
* posiciones
* formaciones
* power-ups
* checkpoints
* mini-boss
* boss
* cambios de música
* eventos especiales

Ejemplo conceptual:

```c
typedef struct {
    uint16_t frame;
    uint8_t enemyType;
    int16_t x;
    int16_t y;
    uint8_t pattern;
} SpawnEvent;
```

Y:

```c
typedef struct {
    uint16_t frame;
    uint8_t type;
    int16_t x;
    int16_t y;
} PowerupEvent;
```

El Stage Manager procesa estos eventos.

### Ventajas requeridas

Debe ser posible modificar:

```text
enemy
position
timing
formation
powerup
difficulty
checkpoint
```

sin modificar la lógica general del motor.

El contenido de cada fase debe poder balancearse independientemente.

---

## 8. ESPECIFICACIÓN EXACTA DE BOSSES

Cada boss debe ser una entidad independiente y basada en datos/estado.

Cada boss debe especificar explícitamente:

```text
name
HP
intro duration
number of phases
phase duration
movement speed
attack cooldown
attack patterns
vulnerable points
damage rules
phase transitions
death sequence
score
```

### Cada fase del boss debe definir

```text
movement
attack pattern
projectile count
projectile speed
cooldown
vulnerable window
telegraph
duration
transition condition
```

### Reglas de diseño

Cada boss debe:

* tener una silueta reconocible
* tener 3–5 puntos vulnerables cuando corresponda
* tener 3–5 patrones principales
* tener múltiples fases
* comunicar cuándo va a atacar
* comunicar cuándo es vulnerable
* permitir aprender sus patrones
* tener una secuencia de muerte clara

### IMPORTANTE

No utilizar dificultad basada exclusivamente en:

```text
+HP
+bullet count
+enemy speed
```

La dificultad debe aumentar principalmente mediante:

```text
pattern complexity
positioning
timing
movement
attack combinations
safe-zone reduction
```

Pero siempre debe existir una ruta razonable de supervivencia.

---

## 9. ASSETS Y PIPELINE

El agente debe asumir que inicialmente **no existen assets finales**.

Por tanto, debe generar assets placeholder funcionales.

### Pipeline

```text
SPEC
 ↓
placeholder assets
 ↓
gameplay implementation
 ↓
build
 ↓
emulator
 ↓
automated tests
 ↓
performance tests
 ↓
visual validation
```

Los assets deben estar almacenados dentro del repositorio.

No depender de:

```text
~/Desktop/
~/Downloads/
archivos temporales
assets externos
paths absolutos
```

### Placeholder requirements

Los gráficos deben permitir identificar inequívocamente:

```text
PLAYER
DRONE
FIGHTER
BOMBER
TURRET
SWARM
CHARGER
SHIELD
MINI-BOSS
BOSS
POWERUP
PROJECTILE
EXPLOSION
```

Cada elemento debe tener dimensiones conocidas y compatibles con el motor.

Los bosses pueden construirse inicialmente mediante múltiples sprites/tile blocks.

### Sustitución futura

La arquitectura debe permitir reemplazar:

```text
placeholder sprite
```

por:

```text
final sprite
```

sin modificar la lógica de gameplay.

Lo mismo debe aplicarse a:

* fondos
* tiles
* efectos
* HUD
* música
* SFX

---

## 10. DEFINITION OF DONE

El agente NO puede declarar NEON STRIKE terminado simplemente porque compila.

La implementación se considera terminada únicamente cuando se cumplen TODOS los criterios siguientes.

### BUILD

* [ ] El proyecto compila desde cero.
* [ ] La ROM se genera correctamente.
* [ ] No existen errores de compilación.
* [ ] No existen warnings críticos.
* [ ] Todos los assets están incluidos.
* [ ] No existen dependencias locales externas.
* [ ] El build es reproducible.

### BOOT

* [ ] La ROM arranca correctamente en el emulador existente.
* [ ] Aparece el título.
* [ ] El juego puede comenzar.
* [ ] No existen crashes durante el flujo normal.

### GAMEPLAY

* [ ] Player implementado.
* [ ] Movimiento 8 direcciones.
* [ ] Hitbox 6×6.
* [ ] Cinco armas implementadas.
* [ ] Tres niveles de arma.
* [ ] Power-ups implementados.
* [ ] Bomb implementada.
* [ ] Vidas implementadas.
* [ ] Continues implementados.
* [ ] Checkpoints implementados.
* [ ] Score implementado.
* [ ] Combo implementado.

### ENEMIES

* [ ] Todos los tipos de enemigos implementados.
* [ ] Cada enemigo tiene comportamiento propio.
* [ ] Los proyectiles funcionan.
* [ ] Las colisiones funcionan.
* [ ] Los límites de objetos son seguros.

### STAGES

* [ ] Stage 1 completo.
* [ ] Stage 2 completo.
* [ ] Stage 3 completo.
* [ ] Stage 4 completo.
* [ ] Stage 5 completo.
* [ ] Stage 6 completo.
* [ ] Cada stage tiene checkpoints.
* [ ] Cada stage tiene boss.
* [ ] El final boss tiene 3 fases.
* [ ] Existe ending.
* [ ] Existen credits.

### AUDIO

* [ ] Música funcional.
* [ ] Música de stages.
* [ ] Música de bosses.
* [ ] Música del final.
* [ ] Música de créditos.
* [ ] SFX principales.
* [ ] Audio sincronizado correctamente con el gameplay.

### PERFORMANCE

* [ ] 60 FPS objetivo mantenidos.
* [ ] Sin caídas graves durante bosses.
* [ ] Sin caídas graves con alta densidad de proyectiles.
* [ ] Sin caídas graves durante explosiones.
* [ ] Sin saturación de sprites.
* [ ] Sin corrupción gráfica por exceso de objetos.
* [ ] Memoria dentro de los límites del sistema.

### VISUAL

* [ ] Player visible.
* [ ] Enemigos visibles.
* [ ] Proyectiles claramente visibles.
* [ ] Power-ups reconocibles.
* [ ] HUD legible.
* [ ] Background correcto.
* [ ] Scroll correcto.
* [ ] Parallax funcional.
* [ ] Bosses reconocibles.
* [ ] Sin tiles corruptos.
* [ ] Sin sprites desapareciendo incorrectamente.
* [ ] Sin glitches visuales críticos.

### GAME FLOW

Debe ser posible jugar:

```text
TITLE
→ STAGE 1
→ BOSS
→ STAGE 2
→ BOSS
→ STAGE 3
→ BOSS
→ STAGE 4
→ BOSS
→ STAGE 5
→ BOSS
→ STAGE 6
→ FINAL BOSS
→ ENDING
→ CREDITS
```

sin modificar código.

### AUTOMATED VALIDATION

El agente debe ejecutar todas las pruebas disponibles antes de finalizar.

Si alguna falla:

```text
FAIL
 ↓
DIAGNOSE
 ↓
FIX
 ↓
BUILD
 ↓
TEST
 ↓
EMULATOR
 ↓
VALIDATE AGAIN
```

No marcar como completado mientras exista un fallo conocido.

### PERFORMANCE ACCEPTANCE

Como mínimo:

```text
Resolution:             320×224
Target FPS:             60
Player hitbox:          6×6
Max enemies:            ~12
Max enemy projectiles:  ~80
Max visible sprites:    ~64
```

El peor escenario razonable debe seguir siendo jugable.

### FINAL OUTPUT

Al finalizar, el agente debe dejar:

```text
ROM ejecutable
README.md actualizado
SPEC.md actualizado
source code
placeholder assets
stage data
tests
validation scripts
```

Y debe producir un informe final con:

```text
BUILD: PASS/FAIL
BOOT: PASS/FAIL
GAMEPLAY: PASS/FAIL
STAGES: PASS/FAIL
BOSSES: PASS/FAIL
AUDIO: PASS/FAIL
VISUAL: PASS/FAIL
PERFORMANCE: PASS/FAIL
TESTS: PASS/FAIL
DEFINITION OF DONE: PASS/FAIL
```

Si alguna categoría es `FAIL`, el agente debe indicar exactamente:

```text
qué falla
dónde falla
por qué falla
qué falta para solucionarlo
```

No debe ocultar problemas ni declarar éxito parcial como éxito completo.

---

# AGENT EXECUTION RULE

El agente debe trabajar de forma iterativa:

```text
INSPECT
 ↓
PLAN
 ↓
IMPLEMENT
 ↓
BUILD
 ↓
TEST
 ↓
RUN IN EMULATOR
 ↓
VISUAL VALIDATION
 ↓
PERFORMANCE VALIDATION
 ↓
FIX
 ↓
RETEST
```

Después de cada bloque importante de implementación debe comprobar que el juego sigue compilando y ejecutándose.

No implementar todo el juego y esperar hasta el final para descubrir errores de arquitectura.

Priorizar este orden:

```text
1. Core game loop
2. Player
3. Shooting
4. Collision
5. Enemy framework
6. Projectile framework
7. Power-ups
8. Stage data system
9. Stage 1
10. Boss framework
11. Remaining stages
12. Final boss
13. HUD / score / lives / continues
14. Audio integration
15. Effects
16. Performance optimization
17. Visual validation
18. Full campaign validation
```

## PRINCIPIO FUNDAMENTAL

El agente debe considerar que **"compila" no equivale a "funciona"**.

Y:

```text
"funciona" != "está terminado"
```

El resultado final debe ser una ROM jugable, estable, medible y validada en el emulador existente, con todos los sistemas principales implementados y con assets placeholder suficientes para demostrar el funcionamiento completo del juego.
