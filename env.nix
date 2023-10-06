{ stdenv, mkShell, clang-tools, qemu, gdb, dtc }:

mkShell {
  CROSS_COMPILE = stdenv.cc.targetPrefix;
  depsBuildBuild = [ qemu gdb dtc ];
  hardeningDisable = [ "all" ];
}
