{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.gtk3
    pkgs.gtk-layer-shell
    pkgs.pkg-config
    pkgs.cjson
    pkgs.clang
  ];

  shellHook = ''
    export PKG_CONFIG_PATH=${pkgs.gtk-layer-shell.dev}/lib/pkgconfig:${pkgs.gtk3.dev}/lib/pkgconfig:$PKG_CONFIG_PATH
    '';
}

