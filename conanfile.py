from conans import ConanFile, CMake, tools


class ChronopticsToF(ConanFile):
    scm = {
        "type": "git",
        "url": "git@github.com:twantonie/match_idle.git",
        "revision": "auto",
    }

    name = "tof_internal"
    license = "Private"
    url = "https://www.chronoptics.com"
    description = "The Chronoptics time of flight library"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "compile_device": [True, False]}
    default_options = {"shared": False, "compile_device": False}

    requires = ["centurion/6.3.0"]

    generators = "cmake_find_package", "cmake_paths", "virtualenv"

    def set_version(self):
        git = tools.Git(folder=self.recipe_folder)
        self.version = git.get_commit()[0:8]

    def build_requirements(self):
        self.build_requires("catch2/2.13.8", force_host_context=True)

    def configure(self):
        self.options["catch2"].with_main = True

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="")
        cmake.build()
