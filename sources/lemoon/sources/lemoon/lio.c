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
    lua_pushfstring(L, "lfile(%p,%d)",file,file->fd);
    return 1;
}


LEMOON_PRIVATE void lio_close(lua_State *L, lio *io)
{
    (void)L;
    if(io->files)
    {
        free(io->files);
    }
    
}

//push new io object into lua stack
LEMOON_PRIVATE lio* lio_new(lua_State *L,size_t len,const luaL_Reg * funcs,lua_CFunction closef)
{
    assert(len > sizeof(lio));
    
    lio * io = lua_newuserdata(L, len);
    
    memset(io,0,len);
    
    if(luaL_newmetatable(L , LEMOON_REG(LEMOON_IO)))
    {
        lua_newtable(L);
        
        luaL_setfuncs(L,funcs,0);
        
        lua_setfield(L, -2, "__index");
        
        lua_pushcfunction(L, closef);
        
        lua_setfield(L, -2, "__gc");
    }
    
    lua_setmetatable(L, -2);
    
    __io_resize(L, io, 56);
    
    return io;
}

LEMOON_PRIVATE lfile* lfile_new(lua_State *L, lio * io, size_t len, const char * tname, int fd, const luaL_Reg * funcs, lua_CFunction closef)
{
    if (((double)io->nsize / (double)io->buckets)  > 0.5)
    {
        __io_resize(L, io, io->buckets * 2);
    }
    
    lfile * file = lfile_search(io, fd);
    
    if(file)
    {
        lemoonL_error(L, "duplicate register file handle(%d)",fd);
    }
    
    file = lua_newuserdata(L, len);
    
    memset(file, 0, len);
    
    if (luaL_newmetatable(L, tname))
    {
        lua_newtable(L);
        
        luaL_setfuncs(L,funcs,0);
        
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
    
    file->io = io;
    
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

#ifdef WIN32
LEMOON_PRIVATE void lfile_close(lua_State *L, lfile * file)
{
    if (file->next)
    {
        file->next->prev = file->prev;
    }

    if (file->prev)
    {
        *file->prev = file->next;
    }

    lirp * irp;

    while (NULL != (irp = file->readQ))
    {
        lirp_remove(irp);

        irp->complete(L, file->io, irp, "file closed");

        lirp_close(L, irp);
    }

    while (NULL != (irp = file->writeQ))
    {
        lirp_remove(irp);

        irp->complete(L, file->io, irp, "file closed");

        lirp_close(L, irp);
    }
}

#else
LEMOON_PRIVATE void lfile_close(lua_State *L, lfile * file)
{
    if (file->next)
    {
        file->next->prev = file->prev;
    }

    if (file->prev)
    {
        *file->prev = file->next;
    }

    //then cancel all the pending io
    lirp * irp = file->readQ;

    while (irp)
    {
        lirp * next = irp->next;

        irp->canceled = 1;

        irp->next = NULL;

        lio_newcomplete(file->io, irp);

        irp = next;
    }

    irp = file->writeQ;

    while (irp)
    {
        lirp * next = irp->next;

        irp->canceled = 1;

        irp->next = NULL;

        lio_newcomplete(file->io, irp);

        irp = next;
    }
}
#endif // WIN32


LEMOON_PRIVATE int lio_dispatch(lua_State *L)
{
    luaL_checkstack(L, 2, NULL);
    
    int timeout = luaL_optinteger(L, 2, 0);
    
    lemoon_dispatch(L, 1, timeout);
    
    return 0;
}


#ifndef WIN32

LEMOON_PRIVATE void lio_newcomplete(lio *io, lirp * irp)
{
    irp->next = io->completeQ;
    
    irp->prev = &io->completeQ;
    
    if(io->completeQ)
    {
        io->completeQ->prev = &irp->next;
    }
    
    io->completeQ = irp;
}

LEMOON_PRIVATE void lio_dispatchcomplete(lua_State *L, lio *io)
{
    lirp  * irp = io->completeQ;
    
    int errcode;
    
    while(irp)
    {
        lirp_remove(irp);
        
        errcode = irp->complete(L, io, irp);
        
        lirp_close(L,irp);
        
        if(errcode == LEMOON_RUNTIME_ERROR)
        {
            lemoonL_error(L, "%s",lua_tostring(L, -1));
        }
        
        irp = io->completeQ;
    }
}


LEMOON_PRIVATE void lfile_process_rwQ(lua_State *L, lio *io, lirp * Q, int errcode)
{
    lirp * current = Q;

    if (errcode != 0)
    {
        while (current)
        {
            lirp * next = current->next;

            lirp_remove(current);

            lemoonL_pushsysmerror(L, errcode, "async io error");

            current->errmsg = luaL_ref(L, LUA_REGISTRYINDEX);
            lio_newcomplete(io, current);
            current = next;
        }
    }
    else
    {
        while (current)
        {
            int ret = current->proc(L, io, current);

            if (ret == LEMOON_EAGIN)
            {
                break;
            }

            lirp * next = current->next;
            
            lirp_remove(current);

            if (ret == LEMOON_RUNTIME_ERROR)
            {
                current->errmsg = luaL_ref(L, LUA_REGISTRYINDEX);
            }

            lio_newcomplete(io, current);

            current = next;
        }
    }
}

#endif //!WIN32
#ifndef WIN32
LEMOON_PRIVATE void* lirp_newrite(lua_State *L, lfile * file, size_t len,lioproc proc, liocomplete complete, int callback)
#else
LEMOON_PRIVATE void* lirp_newrite(lua_State *L, lfile * file, size_t len, liocomplete complete, int callback)
#endif //WIN32
{
    assert(len >= sizeof(lirp));
    
    lirp * irp = malloc(len);
    
    if(!irp)
    {
        lemoonL_error(L, "can't allocate irp object for lfile(%p,%d)",file,file->fd);
    }
    
    memset(irp, 0, len);
 
    irp->callback = callback;
    irp->complete = complete;
#ifndef WIN32
    irp->proc = proc;
    irp->errmsg = LUA_NOREF;
#endif //!WIN32
    irp->file = file;
    irp->next = file->writeQ;
    irp->prev = &file->writeQ;
    if(file->writeQ)
    {
        file->writeQ->prev = &irp->next;
    }
    file->writeQ = irp;
    return irp;
}
#ifndef WIN32
LEMOON_PRIVATE void* lirp_newread(lua_State *L, lfile * file, size_t len, lioproc proc,liocomplete complete, int callback)
#else
LEMOON_PRIVATE void* lirp_newread(lua_State *L, lfile * file, size_t len, liocomplete complete, int callback)
#endif //WIN32
{
    assert(len >= sizeof(lirp));
    
    lirp * irp = malloc(len);
    
    if(!irp)
    {
        lemoonL_error(L, "can't allocate irp object for lfile(%p,%d)",file,file->fd);
    }
    
    memset(irp, 0, len);

    irp->callback = callback;
    irp->complete = complete;
#ifndef WIN32
    irp->errmsg = LUA_NOREF;
    irp->proc = proc;
#endif //!WIN32
    irp->file = file;
    irp->next = file->readQ;
    irp->prev = &file->readQ;
    if(file->readQ)
    {
        file->readQ->prev = &irp->next;
    }
    file->readQ = irp;
    return irp;

}

LEMOON_PRIVATE void lirp_remove(lirp * irp)
{
    if (irp->prev)
    {
        *irp->prev = irp->next;
    }

    if (irp->next)
    {
        irp->next->prev = irp->prev;
    }
    
    irp->prev = NULL;
    irp->next = NULL;
}

LEMOON_PRIVATE void lirp_close(lua_State*L ,lirp * irp)
{
    lirp_remove(irp);

    if(irp->callback != LUA_NOREF)
    {
        luaL_unref(L, LUA_REGISTRYINDEX, irp->callback);
        irp->callback = LUA_NOREF;
    }
#ifndef WIN32
    if(irp->errmsg != LUA_NOREF)
    {
        luaL_unref(L, LUA_REGISTRYINDEX, irp->errmsg);
        irp->errmsg = LUA_NOREF;
    }
#endif //!WIN32   
    free(irp);
}









