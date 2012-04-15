#
# Ubuntu patched this compiler to hell
# gcc (Ubuntu/Linaro 4.4.4-14ubuntu5) 4.4.5
#
EXEC_PROGRAM(${CMAKE_CXX_COMPILER}
  ARGS --version
  OUTPUT_VARIABLE GCC_COMPILER_VERSION
)

IF(GCC_COMPILER_VERSION MATCHES ".*4\\.4\\.4\\-14ubuntu5.*")
  MESSAGE(" ")
  MESSAGE("    !!!!! WARNING Your compiler is BUGGY. !!!!! ")
  MESSAGE(" ")
  MESSAGE("    If possible, upgrade your compiler to e.g. g++ 4.5:")
  MESSAGE(" ")
  MESSAGE("       $ sudo apt-get install g++-4.5")
  MESSAGE(" ")
  MESSAGE("    And build with that compiler cmake -DCMAKE_CXX_COMPILER=g++-4.5")
  MESSAGE(" ")
  MESSAGE("    We will now disable all assertions as a work around, by")
  MESSAGE("    building using -DNDEBUG. You will need to define this")
  MESSAGE("    also for programs built using Wt")
  MESSAGE(" ")
  ADD_DEFINITIONS(-DNDEBUG)
ENDIF(GCC_COMPILER_VERSION MATCHES ".*4\\.4\\.4\\-14ubuntu5.*")

