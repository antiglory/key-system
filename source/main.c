#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "include/functions/adi.c"

// sizes
#define KeyList_s 64 // key list size

// other macros
#define loaded true
#define lnloaded false

// typedefs
typedef struct {
    unsigned long KeyId;
    uintptr_t KeyAddress;
    bool KeyStatus;

    char KeyItems[1024];
    long KeyBalance;
} Key_t;

typedef struct {
    uintptr_t Addresses[KeyList_s];
    unsigned long Count;
} KeyList_t; 
KeyList_t KeyList;

// functions
static Key_t* CreateKey() {
    srand((unsigned int)time(NULL));

    unsigned long GeneratedKeyId;

    bool IdAlreadyExists;

    if (KeyList_s == 64) {
        int TotalKeys = 0;
        for (int i = 0; i < KeyList_s; i++) {
            if (KeyList.Addresses[i] != 0) {
                TotalKeys++;
            }
        }
        if (TotalKeys >= KeyList_s) {
            printf("[!] can't create more keys, the limit has already been reached (64 keys)\n");
            TotalKeys = 0;
            return NULL;
        }
    }

    do {
        IdAlreadyExists = false;
        GeneratedKeyId = rand() % 900 + 99;

        for (int i = 0; i < KeyList_s; i++) {
            if (KeyList.Addresses[i] != 0) {
                Key_t* existingKey = (Key_t*)KeyList.Addresses[i];
                if (existingKey->KeyId == GeneratedKeyId) {
                    IdAlreadyExists = true;
                    break;
                }
            }
        }
    } while (IdAlreadyExists);

    Key_t* Key = (Key_t*)malloc(sizeof(Key_t));
    if (!Key) exit(EXIT_FAILURE);

    memset(Key, 0, sizeof(Key_t));

    for (int i = 0; i < KeyList_s; i++) {
        if (KeyList.Addresses[i] == 0) {
            KeyList.Addresses[i] = (uintptr_t)Key;
            break;
        }
    }

    Key->KeyAddress = (uintptr_t)Key;
    Key->KeyId = GeneratedKeyId;
    Key->KeyStatus = loaded;

    KeyList.Count++;

    printf("[+] created key '%lu'\n", Key->KeyId);
    return Key;
}

static int DeleteKey(unsigned long KeyId) {
    for (int i = 0; i < KeyList_s; i++) {
        if (KeyList.Addresses[i] != 0) {
            Key_t* Key = (Key_t*)KeyList.Addresses[i];
            if (Key->KeyId == KeyId) {
                unsigned long SavedId = Key->KeyId;

                memset(Key, 0, sizeof(Key_t));
                free(Key);

                KeyList.Addresses[i] = 0;
                KeyList.Count--;

                printf("[^] deleted key '%lu'\n", SavedId);
                return 0;
            }
        }
    }

    printf("[!] key '%lu' not found\n", KeyId);
    return NULL;
}

static int GetInfo(unsigned long KeyId) {
    if (KeyId == NULL) {
        char UserInput[12] = {0};

        printf("[+] keys running: %lu\n", KeyList.Count);

        input("list all keys? ", &UserInput, 's');
        if (strcmp(UserInput, "y") == 0) {
            for (int i = 0; i < KeyList_s; i++) {
                if (KeyList.Addresses[i] != 0) {
                    printf("[%p]   : %lu\n", (void*)KeyList.Addresses[i], ((Key_t*)(KeyList.Addresses[i]))->KeyId);
                }
            }
        }
        return 0;
    } else {
        return 1; // por enquanto
    }
}

int main(void) {
    char UserInput[24] = {0};
    char Question[12] = {0};

    unsigned long KeyId = 0;

    // extremely confusing interface
    while (1) {
        input(": ", &UserInput, 's');

        if (strcmp(UserInput, "create") == 0) {
            CreateKey();
        } else if (strcmp(UserInput, "del") == 0 || strcmp(UserInput, "delete") == 0) {
            input("", &KeyId, 'u');

            input("are you sure? ", &Question, 's');

            if (strcmp(Question, "y") == 0) {
                DeleteKey(KeyId);
            }
        } else if (strcmp(UserInput, "info") == 0) {
            input("", UserInput, 's');

            if (strcmp(UserInput, "key") == 0) {
                input("", &KeyId, 'u');

                GetInfo(KeyId);
            } else if (strcmp(UserInput, "keys") == 0) {
                GetInfo((unsigned long)NULL);
            }
        } else if (strcmp(UserInput, "clear") == 0) {
            system("clear");
        } else if (strcmp(UserInput, "exit") == 0 || strcmp(UserInput, "bye") == 0) {
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}