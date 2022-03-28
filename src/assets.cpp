#include "assets.hpp"

#include <SDL.h>
#include <fmt/core.h>
#include <physfs.h>

#include <boost/dll/runtime_symbol_info.hpp>
#include <cassert>
#include <cstdio>

static Sint64 SDLCALL physfsrwops_size(struct SDL_RWops *rw) {
  PHYSFS_File *handle = (PHYSFS_File *)rw->hidden.unknown.data1;
  return (Sint64)PHYSFS_fileLength(handle);
}

static Sint64 SDLCALL physfsrwops_seek(struct SDL_RWops *rw, Sint64 offset,
                                       int whence) {
  PHYSFS_File *handle = (PHYSFS_File *)rw->hidden.unknown.data1;
  PHYSFS_sint64 pos = 0;

  if (whence == RW_SEEK_SET)
    pos = (PHYSFS_sint64)offset;

  else if (whence == RW_SEEK_CUR) {
    const PHYSFS_sint64 current = PHYSFS_tell(handle);
    if (current == -1) {
      SDL_SetError("Can't find position in file: %s",
                   PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
      return -1;
    }

    if (offset == 0) {
      return (Sint64)current;
    }

    pos = current + ((PHYSFS_sint64)offset);
  }

  else if (whence == RW_SEEK_END) {
    const PHYSFS_sint64 len = PHYSFS_fileLength(handle);
    if (len == -1) {
      SDL_SetError("Can't find end of file: %s",
                   PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
      return -1;
    }

    pos = len + ((PHYSFS_sint64)offset);
  }

  else {
    SDL_SetError("Invalid 'whence' parameter.");
    return -1;
  }

  if (pos < 0) {
    SDL_SetError("Attempt to seek past start of file.");
    return -1;
  }

  if (!PHYSFS_seek(handle, (PHYSFS_uint64)pos)) {
    SDL_SetError("PhysicsFS error: %s",
                 PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return -1;
  }

  return (Sint64)pos;
}

static size_t SDLCALL physfsrwops_read(struct SDL_RWops *rw, void *ptr,
                                       size_t size, size_t maxnum) {
  PHYSFS_File *handle = (PHYSFS_File *)rw->hidden.unknown.data1;
  const PHYSFS_uint64 readlen = (PHYSFS_uint64)(maxnum * size);
  const PHYSFS_sint64 rc = PHYSFS_readBytes(handle, ptr, readlen);
  if (rc != ((PHYSFS_sint64)readlen)) {
    if (!PHYSFS_eof(handle)) /* not EOF? Must be an error. */
    {
      SDL_SetError("PhysicsFS error: %s",
                   PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));

      return 0;
    }
  }

  return (size_t)rc / size;
}

static size_t SDLCALL physfsrwops_write(struct SDL_RWops *rw, const void *ptr,
                                        size_t size, size_t num) {
  PHYSFS_File *handle = (PHYSFS_File *)rw->hidden.unknown.data1;
  const PHYSFS_uint64 writelen = (PHYSFS_uint64)(num * size);
  const PHYSFS_sint64 rc = PHYSFS_writeBytes(handle, ptr, writelen);
  if (rc != ((PHYSFS_sint64)writelen))
    SDL_SetError("PhysicsFS error: %s",
                 PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));

  return (size_t)rc;
}

static int physfsrwops_close(SDL_RWops *rw) {
  PHYSFS_File *handle = (PHYSFS_File *)rw->hidden.unknown.data1;
  if (!PHYSFS_close(handle)) {
    SDL_SetError("PhysicsFS error: %s",
                 PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return -1;
  }

  SDL_FreeRW(rw);
  return 0;
}

static SDL_RWops *create_rwops(PHYSFS_File *handle) {
  SDL_RWops *retval = NULL;

  if (handle == NULL)
    SDL_SetError("PhysicsFS error: %s",
                 PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
  else {
    retval = SDL_AllocRW();
    if (retval != NULL) {
      retval->size = physfsrwops_size;
      retval->seek = physfsrwops_seek;
      retval->read = physfsrwops_read;
      retval->write = physfsrwops_write;
      retval->close = physfsrwops_close;
      retval->hidden.unknown.data1 = handle;
    }
  }

  return retval;
}

static SDL_RWops *makeRWops(PHYSFS_File *handle) {
  SDL_RWops *retval = NULL;
  if (handle == NULL)
    SDL_SetError("NULL pointer passed to PHYSFSRWOPS_makeRWops().");
  else
    retval = create_rwops(handle);

  return retval;
}

namespace match_idle {

Assets::Assets() {
  PHYSFS_init(nullptr);

  auto path = boost::dll::program_location().parent_path();
  auto asset_path = (path / "assets.zip").string();

  PHYSFS_addToSearchPath(asset_path.c_str(), 1);
}

Assets::~Assets() { PHYSFS_deinit(); }

cen::rw_ops Assets::file(const char *location) {
  return cen::rw_ops(file_raw(location));
}

SDL_RWops *Assets::file_raw(const char *location) {
  assert(PHYSFS_exists(location));
  return makeRWops(PHYSFS_openRead(location));
}

}  // namespace match_idle
