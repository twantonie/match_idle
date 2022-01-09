option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)

# Helper function to check compiler flags for language compiler.
function(check_compiler_warning WARNING)
  get_property(ENABLED_LANGUAGES GLOBAL PROPERTY ENABLED_LANGUAGES)

  if("C" IN_LIST ENABLED_LANGUAGES)
    include(CheckCCompilerFlag)
    check_c_compiler_flag("${WARNING}" C${WARNING})
  else()
    set(c${WARNING} 1)
  endif()

  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag("${WARNING}" CXX${WARNING})

  if(C${WARNING} AND CXX${WARNING})
    set(PROJECT_WARNINGS
        ${PROJECT_WARNINGS} ${WARNING}
        PARENT_SCOPE)
  elseif(CXX${WARNING})
    set(PROJECT_WARNINGS
        ${PROJECT_WARNINGS} "$<$<COMPILE_LANGUAGE:CXX>:${WARNING}>"
        PARENT_SCOPE)
  endif()
endfunction()

set(MSVC_WARNINGS
    /w14254 # 'operator': conversion from 'type1:field_bits' to
            # 'type2:field_bits', possible loss of data
    /w14263 # 'function': member function does not override any base class
            # virtual member function
    /w14287 # 'operator': unsigned/negative constant mismatch
    /we4289 # nonstandard extension used: 'variable': loop control variable
            # declared in the for-loop is used outside the for-loop scope
    /w14296 # 'operator': expression is always 'boolean_value'
    /w14311 # 'variable': pointer truncation from 'type1' to 'type2'
    /w14545 # expression before comma evaluates to a function which is missing
            # an argument list
    /w14546 # function call before comma missing argument list
    /w14547 # 'operator': operator before comma has no effect; expected operator
            # with side-effect
    /w14549 # 'operator': operator before comma has no effect; did you intend
            # 'operator'?
    /w14555 # expression has no effect; expected expression with side- effect
    /w14640 # Enable warning on thread un-safe static member initialization
    /w14826 # Conversion from 'type1' to 'type_2' is sign-extended. This may
            # cause unexpected runtime behavior.
    /w14905 # wide string literal cast to 'LPSTR'
    /w14906 # string literal cast to 'LPWSTR'
    /w14928 # illegal copy-initialization; more than one user-defined conversion
            # has been implicitly applied
    /wd4200 # Disable warning for zero-sized array in struct/union
    /wd4201 # Disable nonstandard extension used: nameless struct/union
    /wd4244 # Disable conversion from 'double' to 'uint32_t', possible loss of
            # data
    /wd4267 # Disable truncation warning int
    /wd4305 # Disable truncation warning float
    /wd4702 # Disable Unreachable code
    /wd4389 # Disable Signed unsigned mismatch
    # ~~~
    # /w14619 # pragma warning: there is no warning number 'number'
    # /w14242 # 'identifier': conversion from 'type1' to 'type1', possible loss of data
    # /w14265 # 'classname': class has virtual functions, but destructor is not virtual instances of this class may not be
    # destructed correctly
    # ~~~
)

set(CLANG_WARNINGS
    -Wall
    -Wextra # reasonable and standard
    -Wshadow # warn the user if a variable declaration shadows one from a parent
             # context
    -Wunused # warn on anything being unused
    -Wformat=2 # warn on security issues around functions that format output (ie
               # printf)
    -Wnull-dereference # warn if a null dereference is detected
    -Wno-error=null-dereference
    -Wcast-align # warn for potential performance problem casts
    # ~~~
    # -Wpedantic # warn if non-standard C++ is used
    # -Wconversion # warn on type conversions that may lose data
    # -Wsign-conversion # warn on sign conversions
    # -Wold-style-cast # warn for c-style casts
    # -Woverloaded-virtual # warn if you overload (not override) a virtual function
    # -Wdouble-promotion # warn if float is implicit promoted to double
    # ~~~
)

if(WARNINGS_AS_ERRORS)
  set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
  set(MSVC_WARNINGS ${MSVC_WARNINGS} /WX)
endif()

if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    list(APPEND MSVC_WARNINGS /W4)
  endif()

  set(PROJECT_WARNINGS ${MSVC_WARNINGS})
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" OR CMAKE_CXX_COMPILER_ID
                                                  STREQUAL "GNU")
  set(PROJECT_WARNINGS ${CLANG_WARNINGS})

  check_compiler_warning("-Wnon-virtual-dtor") # warn the user if a class with
                                               # virtual functions has
  # a non-virtual destructor. This helps catch hard to track down memory errors
  check_compiler_warning(
    "-Wmisleading-indentation") # warn if identation implies blocks where blocks
                                # do not exist
  check_compiler_warning("-Wno-error=maybe-uninitialized")
  check_compiler_warning("-Wno-error=deprecated-copy")
  # useless cast gives false positive on explicit copy constructor
  # check_compiler_warning("-Wuseless-cast") # warn if you perform a cast to the
  # same type
  check_compiler_warning("-Wduplicated-cond") # warn if if / else chain has
                                              # duplicated conditions
  check_compiler_warning(
    "-Wlogical-op") # warn about logical operations being used where bitwise
                    # were probably wanted
  # This warning doesn't respect -isystem includes on GCC7
  # check_compiler_warning("-Wduplicated-branches") # warn if if / else branches
  # have duplicated code

  # This has false positives
  check_compiler_warning("-Wno-error=missing-braces")

else()
  message(
    AUTHOR_WARNING
      "No compiler warnings set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
endif()

add_compile_options(${PROJECT_WARNINGS})
