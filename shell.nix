{
    pkgs ? import <nixpkgs> {}
} : 
let
  inherit (pkgs) python310;
in
pkgs.mkShell {

    buildInputs = [
        #(pkgs.python310.withPackages (ps: with ps; [ 
        #   pip 
        #]))
        # cmake adds /usr/lib to runpath for dynamic libraries. then nix-paths are not checked. Why does cmake do this?
        # pkgs.buildPackages.cmake

        # pkgs.buildPackages.cgal_5
        # pkgs.python310Packages.boost
        # pkgs.buildPackages.gmp
        # pkgs.buildPackages.mpfr
        pkgs.buildPackages.ninja
        # pkgs.qt5.full

        # pkgs.python310Packages.pybind11

        # pkgs.python310Packages.loguru
        # pkgs.python310Packages.matplotlib
        # pkgs.python310Packages.mplcursor
        # pkgs.python310Packages.cgshop2023-pyutils
    ];

    shellHook = ''
        export PATH="./bin:$PATH"
        export PATH="./out/build:$PATH"
        export CC=$(which gcc)
        export CXX=$(which g++)
    '';

}
