{
  description = "proof of concept: containerization on linux";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs {
        inherit system;
        overlays = [
          (final: prev: {
            alpineRootfs = final.stdenv.mkDerivation {
              pname = "alpine-minirootfs";
              version = "3.22.0";
              src = final.fetchurl {
                url = "https://dl-cdn.alpinelinux.org/alpine/v3.22/releases/x86_64/alpine-minirootfs-3.22.0-x86_64.tar.gz";
                sha256 = "sha256-GIeYhONbBxjwF6UP+FteZWgnnpcjP8QoIiKVhf6y+k0=";
              };
              phases = [ "installPhase" ];
              installPhase = ''
                mkdir -p $out
                tar -xzf $src -C $out
              '';
            };
          })
        ];
      };
    in
    {
      devShells.x86_64-linux.default = pkgs.mkShell {
        packages = with pkgs; [
          cmake
          gcc
          just
          alpineRootfs
        ];
        ALPINE_ROOTFS = "${pkgs.alpineRootfs}";
      };
    };
}

