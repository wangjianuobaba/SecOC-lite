#include"bitmap.h"

// 1-10
// 0-9
// 0 .. 8 9
int main() {
    bitmap b = init(10);
    printf(bit2string(b, 10));
    printf("\n");
    set(b, 0);
    set(b, 8);
    printf(bit2string(b, 10));
    return 0;
}