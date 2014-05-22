#include <lemoon/lio.h>


static size_t __io_hash(int handle, size_t buckets){
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

static void __io_resize(lua_State *L, lio* io, size_t len){
    if(io->buckets < len){
        lfile ** array = malloc(sizeof(lfile*) * len);
        if(array == NULL){
            lemoonL_error(L, "memory alloc failed");
        }
        memset(array, 0, sizeof(lfile*) * len);
        if(io->files != NULL){
            for (size_t i = 0; i < io->buckets; ++i){
                lfile * current = io->files [i];
                while (current){
                    lfile * next = current->next;
                    
                    current->next = NULL;
                    
                    current->prev = NULL;
                    
                    size_t hashcode = __io_hash(current->fd, len);
                    
                    assert(hashcode < len);
                    
                    current->next = array[hashcode];
                    
                    if(array[hashcode]){
                        array[hashcode]->prev = &current->next;
                    }
                    
                    current = next;
                }
                
                free(io->files);
            }
            
            
        }
        io->files = array;
        io->buckets = len;
    }
}

static int __lfile_tostring(lua_State *L)
{
    lfile * file = lua_touserdata(L, 1);
    lua_pushfstring(L, "lfile(%d)",file->fd);
    return 1;
}


LEMOON_PRIVATE void lio_close(lua_State *L, lio *io)
{
    if(io->files)
    {
        //TODO: process completeQ, and close all the file
        free(io->files);
    }
    
}

//push new io object into lua stack
LEMOON_PRIVATE lio* lio_new(lua_State *L,size_t len,const luaL_Reg * funcs,lua_CFunction closef)
{
    assert(len > sizeof(lio));
    
    lio * io = lua_newuserdata(L, len);
    
    memset(io,0,sizeof(lio));
    
    if(luaL_newmetatable(L , LEMOON_REG(LEMOON_IO)))
    {
        luaL_newlib(L, funcs);
        
        lua_setfield(L, -2, "__index");
        
        lua_pushcfunction(L, closef);
        
        lua_setfield(L, -2, "__gc");
    }
    
    lua_setmetatable(L, -1);
    
    __io_resize(L, io, 56);
    
    return io;
}

LEMOON_PRIVATE lfile* lfile_new(lua_State *L, lio * io, size_t len, const char * tname, int fd, const luaL_Reg * funcs, lua_CFunction closef)
{
    if (((double)io->nsize / (double)io->buckets)  > 0.5)
    {
        __io_resize(L, io, io->buckets * 2);
    }
    
    lua_pushvalue(L, index);
    
    lfile * file = lfile_search(io, fd);
    
    if(file)
    {
        lemoonL_error(L, "duplicate register file handle(%d)",fd);
    }
    
    file = lua_newuserdata(L, len);
    
    memset(file, 0, len);
    
    if (luaL_newmetatable(L, tname))
    {
        luaL_newlib(L, funcs);
        
        lua_setfield(L, -2, "__index");
        
        lua_pushcfunction(L, closef);
        
        lua_setfield(L, -2, "__gc");
        
        lua_pushcfunction(L, __lfile_tostring);
        
        lua_setfield(L, -2, "__tostring");
    }
    
    lua_setmetatable(L, -2);
    
    size_t hashcode = __io_hash(fd, io->buckets);
    
    file->fd = fd;
    
    file->next = io->files[hashcode];
    
    file->prev = &io->files[hashcode];
    
    if(io->files[hashcode])
    {
        io->files[hashcode]->prev = &file->next;
    }

    io->files[hashcode] = file;
    
    return file;
}

LEMOON_PRIVATE lfile* lfile_search(lio * io, int fd)
{
    size_t hashcode = __io_hash(fd, io->buckets);
    //printf("search file(hashcode:%zu) :%d\n",hashcode,fd);
    
    lfile * current = io->files[hashcode];
    
    while (current)
    {
        if(current->fd == fd)
        {
            break;
        }
        
        current = current->next;
    }
    
    return current;
}

