{ stdenv, mkShell, clang-tools, qemu }:

mkShell {
  CROSS_COMPILE = stdenv.cc.targetPrefix;
  depsBuildBuild = [ qemu ];
  hardeningDisable = [ "all" ];
}
