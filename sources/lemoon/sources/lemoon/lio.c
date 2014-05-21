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
        memset(array, 0, sizeof(lfile*) * len);
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

       
        }
        io->array = array;
        io->buckets = len;
    }
}

LEMOON_PRIVATE lio* lioB_new(lua_State *L, size_t len, const luaL_Reg * funcs, lua_CFunction closef)
{
    lio* io = lua_newuserdata(L, len);
    
    memset(io, 0, len);
    
    lioB_resize(L, io, 56);

    if(luaL_newmetatable(L,LEMOON_REG(LEMOON_IO)))
    {
        luaL_newlib(L, funcs);

        lua_setfield(L, -2, "__index");

        lua_pushcfunction(L, closef);

        lua_setfield(L, -2, "__gc");
    }

    lua_setmetatable(L,-2);

    return io;
}
LEMOON_PRIVATE void lioB_close(lua_State *L, lio* io)
{
    L;
    if(io->array) free(io->array);
}

static int __lfile_tostring(lua_State *L)
{
    lfile * file = lua_touserdata(L, 1);
    lua_pushfstring(L, "file(%d)",file->handle);
    return 1;
}


LEMOON_PRIVATE lfile* lioB_newfile(
    lua_State *L, 
    size_t len, 
    int index, 
    int fd, 
    const char* tname, 
    const luaL_Reg * funcs, 
    lua_CFunction closef)
{
    lio * io = luaL_checkudata(L, index, LEMOON_REG(LEMOON_IO));

    if (((double)io->files / (double)io->buckets)  > 0.5)
    {
        lioB_resize(L, io, io->buckets * 2);
    }

    lua_pushvalue(L, index);

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);

    lfile * file = lioB_searchfile(io, fd);

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

    size_t hashcode = lioB_hash(fd, io->buckets);
    
    file->handle = fd;

    file->io = ref;

    file->next = io->array[hashcode];

    file->prev = &io->array[hashcode];

    if(io->array[hashcode])
    {
        io->array[hashcode]->prev = &file->next;
    }

    return file;
}

LEMOON_PRIVATE void lioB_closefile(lua_State *L, lfile* file)
{
    if(file->next)
    {
        file->next->prev = file->prev;
    }

    if(file->prev)
    {
        *file->prev = file->next;
    }

    file->next = NULL;
    file->prev = NULL;

    if(file->io != LUA_NOREF)
    {
        luaL_unref(L, LUA_REGISTRYINDEX, file->io);
        file->io = LUA_NOREF;
    }
   
}

LEMOON_PRIVATE lfile* lioB_searchfile(lio* io, int fd)
{
    size_t hashcode = lioB_hash(fd, io->buckets);

    lfile * current = io->array[hashcode];

    while (current)
    {
        if(current->handle == fd)
        {
            break;
        }

        current = current->next;
    }

    return current;
}

LEMOON_PRIVATE lirp* lioB_newwrite(lua_State *L, size_t len, lfile *file, iocomplete cp)
{
    lirp * irp = malloc(len);
    if (!irp)
    {
        lemoonL_error(L, "can't create write irp for file(%d)", file->handle);
    }
    memset(irp, 0, len);

    irp->op = cp;
    irp->handle = file->counter++;
    irp->next = file->writeQ;

    if (file->writeQ)
    {
        file->writeQ->prev = &irp->next;
    }

    irp->prev = &file->writeQ;

    return irp;
}

LEMOON_PRIVATE lirp* lioB_newread(lua_State *L, size_t len, lfile *file, iocomplete cp)
{
    lirp * irp = malloc(len);
    if(!irp)
    {
        lemoonL_error(L, "can't create read irp for file(%d)",file->handle);
    }
    memset(irp, 0, len);

    irp->op = cp;
    irp->handle = file->counter++;
    irp->next = file->readQ;
    
    if(file->readQ)
    {
        file->readQ->prev = &irp->next;
    }
    
    irp->prev = &file->readQ;

    return irp;
}

LEMOON_PRIVATE void lioB_closeirp(lua_State *L, lirp * irp)
{
   
    if(irp->next )
    {
        irp->next->prev = irp->prev;
    }

    if(irp->prev)
    {
        *irp->prev = irp->next;
    }

    if(irp->callback != LUA_NOREF)
    {
        luaL_unref(L, LUA_REGISTRYINDEX, irp->callback);
        irp->callback = LUA_NOREF;
    }

    free(irp);
}


LEMOON_PRIVATE void write_complete(lua_State *L, lirp * irp, size_t bytes, const char* errmsg)
{
    if(irp->callback != LUA_NOREF)
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, irp->callback);

        luaL_checktype(L, -1, LUA_TFUNCTION);

        if (errmsg)
        {
            lua_pushstring(L, errmsg);
            lua_pushinteger(L, bytes);
        }
        else
        {
            lua_pushnil(L);
            lua_pushinteger(L, bytes);
        }

      
        if (0 != lua_pcall(L, 2, 0, 0)){
            lioB_closeirp(L, (lirp*) irp);
            lemoonL_error(L, "write_complete exception:\n\t%s", luaL_checkstring(L, -1));
        }

        lioB_closeirp(L, (lirp*) irp);
    }
    else
    {
        lioB_closeirp(L, (lirp*) irp);
    }
}

LEMOON_PRIVATE void read_complete(lua_State *L, lirprw * irp, size_t bytes, const char* errmsg)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, irp->irp.callback);

    luaL_checktype(L, -1, LUA_TFUNCTION);

    if (errmsg)
    {
        lua_pushstring(L, errmsg);
        lua_pushnil(L);
    }
    else
    {
        lua_pushnil(L);
        lua_pushlstring(L, irp->buff, bytes);
    }

  

    if (0 != lua_pcall(L, 2, 0, 0)){
        lioB_closeirp(L, (lirp*) irp);
        lemoonL_error(L, "read_complete exception:\n\t%s", luaL_checkstring(L, -1));
    }

    lioB_closeirp(L, (lirp*) irp);
}