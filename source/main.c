#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "include/functions/adi.c"

// macros
#define KeyList_s 64 // key list size

#define loaded true
#define unloaded false

#define success true
#define fail false

// global vars
char Question[4] = {0};

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
            if (KeyList.Addresses[i] != 0)
                TotalKeys++;
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
                const Key_t* existingKey = (Key_t*)KeyList.Addresses[i];

                if (existingKey->KeyId == GeneratedKeyId) {
                    IdAlreadyExists = true;
                    break;
                }
            }
        }
    } while (IdAlreadyExists);

    Key_t* Key = (Key_t*)malloc(sizeof(Key_t));
    if (!Key) return NULL;

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

static int DeleteKey(const unsigned long KeyId) {
    bool found = false;
    input("are you sure? ", &Question, 's');

    if (strcmp(Question, "y") == 0) {
        for (int i = 0; i < KeyList_s; i++) {
            if (KeyList.Addresses[i] != 0) {
                Key_t* Key = (Key_t*)KeyList.Addresses[i];

                if (KeyId == 69 || Key->KeyId == KeyId) {
                    const unsigned long SavedId = Key->KeyId;

                    if (KeyId != 69 || Key->KeyId != 0) {
                        memset(Key, 0, sizeof(Key_t));
                        free(Key);

                        KeyList.Addresses[i] = 0;
                        KeyList.Count--;

                        printf("[^] deleted key '%lu' ; element '%d'\n", SavedId, i);
                    }

                    if (KeyId != 69) {
                        found = true;
                        break;
                    }
                }
            }
        }
    }

    if (KeyId != 69 && !found) {
        printf("[!] key '%lu' not found\n", KeyId);

        return fail;
    }
    
    return success;
}

static int CancelKey(unsigned long KeyId) {
    for (int i = 0; i < KeyList_s; i++) {
        if (KeyList.Addresses[i] != 0) {
            Key_t* Key = (Key_t*)KeyList.Addresses[i];

            if (Key->KeyId == KeyId) {
                input("are you sure? ", &Question, 's');

                if (strcmp(Question, "y") == 0) {
                    memset(Key->KeyItems, 0, sizeof(Key->KeyItems));
                    Key->KeyBalance = (unsigned long)0;

                    printf("[+] canceled key '%lu'\n", Key->KeyId);

                    return success;
                }
            }
        }
    }

    printf("[!] key '%lu' not found\n", KeyId);

    return fail;
}

static int AddInfo(unsigned long KeyId, const char* info) {
    for (int i = 0; i < KeyList_s; i++) {
        if (KeyList.Addresses[i] != 0) {
            Key_t* Key = (Key_t*)KeyList.Addresses[i];

            if (Key->KeyId == KeyId) {
                const size_t CurrentLength = strlen(Key->KeyItems);
                const size_t NewLength = CurrentLength + strlen(info) + 2;

                if (NewLength > sizeof(Key->KeyItems)) {
                    printf("[!] failed to add item to key '%lu': exceeded maximum length\n", KeyId);
                    return fail;
                }

                if (CurrentLength > 0)
                    strcat(Key->KeyItems, "\n");

                strcat(Key->KeyItems, info);

                printf("[+] added item to key '%lu'\n", KeyId);

                return success;
            }
        }
    }

    printf("[!] key '%lu' not found\n", KeyId);
    return fail;
}

static int GetItems(unsigned long KeyId) {
    for (int i = 0; i < KeyList_s; i++) {
        if (KeyList.Addresses[i] != 0) {
            Key_t* Key = (Key_t*)KeyList.Addresses[i];

            if (Key->KeyId == KeyId) {
                if (strlen(Key->KeyItems) == 0) {
                    printf("[!] no item available for key '%lu'\n", KeyId);
                } else {
                    printf("[-] getting items in the key '%lu':\n\n%s\n", KeyId, Key->KeyItems);
                }
                return success;
            }
        }
    }

    printf("[!] key '%lu' not found\n", KeyId);

    return fail;
}

static int GetInfo(const unsigned long KeyId) {
    if (!KeyId) {
        char UserInput[12] = {0};

        printf("[+] keys running: %lu\n", KeyList.Count);

        if (KeyList.Count != (unsigned long)0) {
            input("list all keys? ", &UserInput, 's');
            if (strcmp(UserInput, "y") == 0) {
                for (int i = 0; i < KeyList_s; i++) {
                    if (KeyList.Addresses[i] != 0) {
                        printf("[%p]   : %lu\n", (void*)KeyList.Addresses[i], ((Key_t*)(KeyList.Addresses[i]))->KeyId);
                    }
                }

                return success;
            }
        }
    } else
        return fail; // for now

    return success;
}

int main(void) {
    char UserInput[128] = {0};

    unsigned long KeyId = 0;

    // extremely confusing interface
    while (1) {
        input(": ", &UserInput, 's');

        if (strcmp(UserInput, "create") == 0) {
            if (!CreateKey()) {
                puts("[!!] catastrophic error"); exit(EXIT_FAILURE);
            }
        } else if (strcmp(UserInput, "del") == 0 || strcmp(UserInput, "delete") == 0) {
            input("", &KeyId, 'u');

            DeleteKey(KeyId);
        } else if (strcmp(UserInput, "cancel") == 0) {
            input("", &KeyId, 'u');

            CancelKey(KeyId);
        } else if (strcmp(UserInput, "add") == 0) {
            input("", &KeyId, 'u');
            fgets(UserInput, sizeof(UserInput), stdin);

            AddInfo(KeyId, UserInput);
        } else if (strcmp(UserInput, "get") == 0) {
            input("", &KeyId, 'u');

            GetItems(KeyId);
        } else if (strcmp(UserInput, "info") == 0) {
            input("", UserInput, 's');

            if (strcmp(UserInput, "key") == 0) {
                input("", &KeyId, 'u');

                GetInfo(KeyId);
            } else if (strcmp(UserInput, "keys") == 0)
                GetInfo((unsigned long)NULL);
        } else if (strcmp(UserInput, "clear") == 0)
            system("clear");
        else if (strcmp(UserInput, "exit") == 0 || strcmp(UserInput, "bye") == 0)
            exit(EXIT_SUCCESS);
    }

    return 0; // unreachable
}
