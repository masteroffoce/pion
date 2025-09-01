{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation {
  pname = "pion";
  version = "0.1";

  src = ./.;

  buildInputs = [
    pkgs.gtk3
    pkgs.gtk-layer-shell
    pkgs.pkg-config
      pkgs.cjson
  ];

  configurePhase = ''
    export PKG_CONFIG_PATH=${pkgs.gtk-layer-shell.dev}/lib/pkgconfig:${pkgs.gtk3.dev}/lib/pkgconfig:$PKG_CONFIG_PATH
  '';

  buildPhase = ''
    make
  '';
  
  installPhase = ''
    mkdir -p $out/bin
    cp run $out/bin
  '';
}
