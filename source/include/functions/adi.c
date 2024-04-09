// adi.c (advanced input)

#pragma once

enum DataType {
    I_INTEGER = 'd',
    I_CHAR = 'c',
    I_STRING = 's',
    I_UNSIGNED_LONG_INT = 'w',
    I_UNSIGNED_LONG = 'u',
    I_HEXADECIMAL = 'h',
    I_SIMPLE_HEXADECIMAL = 'l',
    I_ADDRESS = 'a',
    I_PID = 'p'
};

void input(char* imsg, void* data, enum DataType type) {
    printf("%s", imsg);

    switch (type) {
        case I_INTEGER:
            scanf("%d", (int*)data);
            break;
        case I_CHAR:
            scanf("%c", (char*)data);
            break;
        case I_STRING:
            scanf("%s", (char*)data);
            break;
        case I_UNSIGNED_LONG_INT:
            scanf("%lu", (unsigned long int*)data);
            break;
        case I_UNSIGNED_LONG:
            scanf("%lu", (unsigned long*)data);
            break;
        case I_HEXADECIMAL:
            scanf("%llx", (unsigned long long int*)data);
            break;
        case I_SIMPLE_HEXADECIMAL:
            scanf("%x", (int*)data);
            break;
        case I_ADDRESS:
            scanf("%u", (unsigned int*)data);
            break;
        case I_PID:
            scanf("%d", (signed int*)data);
            break;
    default:
      printf("[!] unknown data type\n");
      exit(EXIT_FAILURE);
  }
}