#include "../src/parser.h"

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("To test:\nparser_test <scene>.json\n");
        return 1;
    }
    printf("Parsing scene '%s'...\n",argv[1]);
    Scene *scene = parse_scene(argv[1]);
    printf("parsed?\n");
    scene_debug_print(scene);
    scene_free(scene);
    return 0;
}
