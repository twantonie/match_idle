#ifndef MATCH_IDLE_ASSETS_HPP
#define MATCH_IDLE_ASSETS_HPP

#include <centurion/filesystem/file.hpp>

namespace match_idle {

class File;

class Assets {
 public:
  ~Assets();

  static Assets& get() {
    static Assets assets{};
    return assets;
  }

  cen::rw_ops file(const char* location);
  SDL_RWops* file_raw(const char* location);

 private:
  Assets();
};

}  // namespace match_idle

#endif