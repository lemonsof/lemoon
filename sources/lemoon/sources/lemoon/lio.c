#include <lemoon/lio.h>



static size_t lioB_hash(int handle, size_t buckets){
    size_t hashCode = (size_t) handle;

#if SIZE_MAX > 0xffffffff

    hashCode = (~hashCode) + (hashCode << 18); // hashCode = (hashCode << 18) - hashCode - 1;

    hashCode = hashCode ^ (hashCode >> 31);

    hashCode = hashCode * 21; // hashCode = (hashCode + (hashCode << 2)) + (hashCode << 4);

    hashCode = hashCode ^ (hashCode >> 11);

    hashCode = hashCode + (hashCode << 6);

    hashCode = hashCode ^ (hashCode >> 22);

#else

    hashCode = ~hashCode + (hashCode << 15); // hashCode = (hashCode << 15) - hashCode - 1;

    hashCode = hashCode ^ (hashCode >> 12);

    hashCode = hashCode + (hashCode << 2);

    hashCode = hashCode ^ (hashCode >> 4);

    hashCode = hashCode * 2057; // hashCode = (hashCode + (hashCode << 3)) + (hashCode << 11);

    hashCode = hashCode ^ (hashCode >> 16);

#endif

    return ((((uint32_t) hashCode) >> 3) ^ 0x7FFFFFFF) % buckets;
}

static void lioB_resize(lua_State *L, lio* io, size_t len){
    if(io->buckets < len){
        lfile ** array = malloc(sizeof(lfile*) * len);
        if(array == NULL){
            lemoonL_error(L, "memory alloc failed");
        }
        if(io->array != NULL){
            for (size_t i = 0; i < io->buckets; ++i){
                lfile * current = io->array [i];
                while (current){
                    lfile * next = current->next;

                    current->next = NULL;

                    current->prev = NULL;

                    size_t hashcode = lioB_hash(current->handle, len);

                    assert(hashcode < len);

                    current->next = array[hashcode];

                    if(array[hashcode]){
                        array[hashcode]->prev = &current->next;
                    }

                    current = next;
                }

                free(io->array);
            }

            io->array = array;

            io->buckets = len;
        }
    }
}

LEMOON_PRIVATE lio*  lioB_new(lua_State *L, size_t len){
    assert(sizeof(lio) < len);
    lio *io = malloc(len);
    memset(io,0,len);
    lioB_resize(L, io,56);
    return io;
}

//LEMOON_PRIVATE lfile* lioB_newfile(lua_State *L, int fd, size_t len)
//{
//}
