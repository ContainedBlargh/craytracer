$CC="gcc"
$CFLAGS="-std=c2x -Wall -O3 -Wno-unknown-pragmas"
$LIBS="-lSDL2 -lSDL2main"
$TARGET="craytracer.exe"

cbundl .\src\main.c .\bundle.c
Invoke-Expression "$CC $CFLAGS bundle.c -o $TARGET $LIBS"
