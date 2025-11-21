#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

typedef double (*mathFunc)(double, double);
typedef struct {
    const char* name;
    mathFunc func;
} ops;

int main() {
    void *addHandle, *subHandle, *mulHandle, *divHandle;
    mathFunc iadd, isub, imul, idiv;

    addHandle = dlopen("./lib/iadd.so", RTLD_LAZY);
    subHandle = dlopen("./lib/isub.so", RTLD_LAZY);
    mulHandle = dlopen("./lib/imul.so", RTLD_LAZY);
    divHandle = dlopen("./lib/idiv.so", RTLD_LAZY);
    if (!addHandle || !subHandle || !mulHandle || !divHandle) {
        fprintf(stderr, "Error while loading DL's");
        exit(1);
    }

    iadd = (mathFunc)dlsym(addHandle, "iadd");
    isub = (mathFunc)dlsym(subHandle, "isub");
    imul = (mathFunc)dlsym(mulHandle, "imul");
    idiv = (mathFunc)dlsym(divHandle, "idiv");
    if (!iadd) {
        fprintf(stderr, "Error while loading function iadd");
        dlclose(addHandle);
        exit(1);
    }
    if (!isub) {
        fprintf(stderr, "Error while loading function isub");
        dlclose(subHandle);
        exit(1);
    }
    if (!imul) {
        fprintf(stderr, "Error while loading function imul");
        dlclose(mulHandle);
        exit(1);
    }
    if (!idiv) {
        fprintf(stderr, "Error while loading function idiv");
        dlclose(divHandle);
        exit(1);
    }
    ops ops[] = {
        {"Add", iadd},
        {"Substract", isub},
        {"Multiply", imul},
        {"Divide", idiv},
    };
    printf("%s: %.0lf\n", ops[0].name, ops[0].func(1, 1));
    printf("%s: %.0lf\n", ops[1].name, ops[1].func(1, 1));
    printf("%s: %.2lf\n", ops[2].name, ops[2].func(1, 1));
    printf("%s: %.2lf\n", ops[3].name, ops[3].func(1, 1));
    return 0;
}