{pkgs ? import <nixpkgs> {}}:
let
  inherit (pkgs) stdenv cunit clang-tools check;
in
stdenv.mkDerivation {
  pname = "eggsh";
  version = "0.0.1";

  src = ./.;

  nativeBuildInputs = [
    cunit
    check
    clang-tools
  ];

  buildPhase = "make";

  installPhase = ''
    mkdir -p $out/bin
    cp eggsh $out/bin
  '';
}

