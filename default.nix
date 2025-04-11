{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  nativeBuildInputs = [
    pkgs.cmake
    # Other build dependencies...
  ];
  
  buildInputs = [
    # Runtime dependencies...
  ];
}