CC=g++

CFLAGS  = -g -D _DEBUG -ggdb3 -std=c++17 -Ofast -w -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations \
-Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body \
-Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor \
-Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion \
-Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods \
-Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused \
-Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast \
-Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging \
-fno-omit-frame-pointer -pie -fPIE -Werror=vla \
-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr


SOURCES=libs/multimedia/multimedia.cpp src/ui/renderable.cpp src/ui/button/scrollbar.cpp src/ui/editbox/editbox.cpp src/ui/canvas/filters.cpp src/events/events.cpp src/ui/submenu/submenu.cpp src/ui/canvas/canvas.cpp src/ui/shapes/shapes.cpp src/ui/menu/menu.cpp src/ui/button/button.cpp src/ui/window/window.cpp src/main.cpp

EXECUTABLE=phshop

SFML_EXEC= -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
 
all: compile_debug

release: compile_release

erase:
	clear

compile_debug:
	@${CC} ${CFLAGS} $(SOURCES) -o $(EXECUTABLE) $(SFML_EXEC)

compile_release:
	@${CC} -g $(SOURCES) -O2 -o $(EXECUTABLE) $(SFML_EXEC)

run:
	@./${EXECUTABLE}

clean:
	@rm -rf ${EXECUTABLE}