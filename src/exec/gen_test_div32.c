#include <stdio.h>
#include <stdint.h>

int main()
{
    int64_t dividend;
    int32_t divisor, quotient;

    int32_t values[] = { 0, 1, -1, 2, 3, 10, 15, 16, 17, 
        255, 1023, 1024, 4096, 1234567, -12345678, 123456789, -1234567890,
        INT16_MIN+1, INT16_MIN, INT16_MAX-1, INT16_MAX, UINT16_MAX-1, UINT16_MAX, 
        INT32_MAX-1, INT32_MAX, INT32_MIN+1, INT32_MIN };

    int count = sizeof(values) / sizeof(values[0]);
    int id = 1;

    printf("org 100h\n");

    for (int i = 0; i < count; i++)
    {
        dividend = values[i];
        for (int j = 0; j < count; j++)
        {
            divisor = values[j];
            if (divisor == 0)
                continue;
            printf("section .data\n");
            printf("test%d\tdb 'Failed #%d dividend=%d, divisor=%d, quotient=", id, id, dividend, divisor);
            fflush(stdout);
            quotient = dividend / divisor;
            printf("%d', 13, 10, '$'\n", quotient);
            printf("section .text\n");
            printf("\tmov dx, 0%04xH\n", (unsigned int)dividend >> 16);
            printf("\tmov ax, 0%04xH\n", (unsigned int)dividend & 0xFFFF);
            printf("\tmov cx, 0%04xH\n", (unsigned int)divisor >> 16);
            printf("\tmov bx, 0%04xH\n", (unsigned int)divisor & 0xFFFF);
            printf("\tcall DIV32\n");
            printf("\tcmp dx, 0%04xH\n", (unsigned int)quotient >> 16);
            printf("\tjne failed%d\n", id);
            printf("\tcmp ax, 0%04xH\n", (unsigned int)quotient & 0xFFFF);
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
