#include "option.h"
#include "hit.h"
#include "defs.h"

option_type(Hit);

int main(int argc, char const *argv[])
{
    printf("sizeof(option_type): %ld, sizeof(u32Option): %ld\n", sizeof(OptionType), sizeof(HitOption));
    printf("SOME: %d\n", (u32) SOME);
    return 0;
}
