ifeq ($(GDK),)
    $(error GDK environment variable is not set. Install SGDK and export GDK=/path/to/sgdk)
endif

include $(GDK)/makefile.gen

# Host-native unit tests for hardware-free logic (no m68k, no emulator).
# Only files with zero SGDK/VDP/SPR/PSG calls belong here.
.PHONY: test
test:
	cc -std=c99 -Wall -Wextra -Iinc tests/test_game_state.c src/game_state_logic.c -o tests/test_game_state
	./tests/test_game_state
	cc -std=c99 -Wall -Wextra -Iinc tests/test_player_logic.c src/player_logic.c -o tests/test_player_logic
	./tests/test_player_logic
	cc -std=c99 -Wall -Wextra -Iinc tests/test_collision_logic.c src/collision_logic.c -o tests/test_collision_logic
	./tests/test_collision_logic
	cc -std=c99 -Wall -Wextra -Iinc tests/test_enemy_logic.c src/enemy_logic.c -o tests/test_enemy_logic
	./tests/test_enemy_logic
