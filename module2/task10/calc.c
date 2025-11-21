#include "main.h"

int main() {
    char command[10];
    void* endptr;
    int op_count = sizeof(ops) / sizeof(ops[0]);
    while (1)
    {    
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;
        char* token = strtok(command, " ");
        char func = *token;

        token = strtok(NULL, " ");
        double arg1 = strtod(token, endptr);

        token = strtok(NULL, " ");
        double arg2 = strtod(token, endptr);

        for (int i = 0; i < op_count; i++) {
            if (ops[i].chara == func) {
                double result = ops[i].func(arg1, arg2);
                printf("%.2lf\n", result);
                break;
            }
        }
    }
}