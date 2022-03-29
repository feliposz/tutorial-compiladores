#include <stdio.h>
#include <stdint.h>

int main()
{
    int32_t op1, op2, product;

    int32_t values[] = { 0, -1, 2, 3, 10, 15, 16, 17, 
        255, 1023, 1234567, -12345678, 123456789, -1234567890,
        INT16_MIN+1, INT16_MIN, INT16_MAX-1, INT16_MAX, UINT16_MAX-1, UINT16_MAX, 
        INT32_MAX-1, INT32_MAX, INT32_MIN+1, INT32_MIN };

    int count = sizeof(values) / sizeof(values[0]);
    int id = 1;

    printf("org 100h\n");

    for (int i = 0; i < count; i++)
    {
        op1 = values[i];
        for (int j = 0; j < count; j++)
        {
            op2 = values[j];
            product = op1 * op2;
            printf("section .data\n");
            printf("test%d\tdb 'Failed #%d op1=%d, op2=%d, product=%d', 13, 10, '$'\n", id, id, op1, op2, product);
            printf("section .text\n");
            printf("\tmov dx, 0%04xH\n", (unsigned int)op1 >> 16);
            printf("\tmov ax, 0%04xH\n", (unsigned int)op1 & 0xFFFF);
            printf("\tmov cx, 0%04xH\n", (unsigned int)op2 >> 16);
            printf("\tmov bx, 0%04xH\n", (unsigned int)op2 & 0xFFFF);
            printf("\tcall MUL32\n");
            printf("\tcmp dx, 0%04xH\n", (unsigned int)product >> 16);
            printf("\tjne failed%d\n", id);
            printf("\tcmp ax, 0%04xH\n", (unsigned int)product & 0xFFFF);
            printf("\tjne failed%d\n", id);
            printf("\tjmp passed%d\n", id);
            printf("failed%d:\n", id);
            printf("\tmov dx, test%d\n", id);
            printf("\tmov ah, 09h\n");
            printf("\tint 21h\n");
            printf("passed%d:\n\n", id);
            id++;
        }
    }

    printf("\tmov ax, 4c00h\n");
    printf("\tint 21h\n");

    printf("%%include \"tinyrtl_math.inc\"\n");

    return 0;
}
