#include <stdio.h>
#include <stdlib.h>
#include "assign1.c"

int main(int argc, char** argv) {
    long x = 0x0403deadbeef0201L; 

    // Byte sort
    printf("%lx  %lx\n", x, byte_sort(x));

    // Nibble sort
    printf("%lx  %lx\n", x, nibble_sort(x));

    // Name list
    struct elt *name = name_list();
    while (name) {
        printf("%c", name->val);
        name = name->link;
    }
    printf("\n");

    // Convert
    convert(67, x);
    convert(66, x);
    convert(68, x);
    convert(2, x);

    // Draw me
    draw_me();

    return 0;
}
