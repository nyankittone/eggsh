{pkgs ? import <nixpkgs> {}}:
let
  inherit (pkgs) stdenv clang-tools check gperf;
in
stdenv.mkDerivation {
  pname = "eggsh";
  version = "0.0.1";

  src = ./.;

  nativeBuildInputs = [
    check
    clang-tools
    gperf
  ];

  buildPhase = "make";

  installPhase = ''
    mkdir -p $out/bin
    cp eggsh $out/bin
  '';
}

