VERT_SOURCE="shaders/shader.vert"
FRAG_SOURCE="shaders/shader.frag"
VERT_C_HEADER="include/vert-shader.h"
FRAG_C_HEADER="include/frag-shader.h"

xxd -i "$VERT_SOURCE" >"$VERT_C_HEADER"
xxd -i "$FRAG_SOURCE" >"$FRAG_C_HEADER"
