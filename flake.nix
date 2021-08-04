{
  description = "Interpeer project shared dependency";

  # Nixpkgs / NixOS version to use.
  inputs.nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-21.05";

  outputs = { self, nixpkgs }:
    let

      # Generate a user-friendly version numer.
      version = builtins.substring 0 8 self.lastModifiedDate;

      # System types to support.
      supportedSystems = [ "x86_64-linux" "x86_64-darwin" "aarch64-linux" "aarch64-darwin" ];

      # Helper function to generate an attrset '{ x86_64-linux = f "x86_64-linux"; ... }'.
      forAllSystems = f: nixpkgs.lib.genAttrs supportedSystems (system: f system);

      # Nixpkgs instantiated for supported system types.
      nixpkgsFor = forAllSystems (system: import nixpkgs { inherit system; overlays = [ self.overlay ]; config.allowUnfree = true; });

      gtest-meson = builtins.toFile "meson.build" ''
        project('gtest')

        gtest_dep = dependency('gtest')
      '';

      liberate =
        { stdenv, meson, ninja, gtest, lib, logBackend ? "stderr" }:
        stdenv.mkDerivation {
          name = "liberate-${version}";

          src = ./.;

          postPatch = ''
            echo > ./examples/meson.build

            rm ./subprojects/gtest.wrap
            mkdir ./subprojects/gtest
            ln -s ${gtest-meson} ./subprojects/gtest/meson.build
          '';

          doCheck = true;

          buildInputs = [ gtest ];
          nativeBuildInputs = [ meson ninja ];
          mesonFlags = [ "-Dlog_backend=${logBackend}" ];

          meta = with lib; {
            description = "Interpeer project shared dependency";
            homepage    = "https://interpeer.io/";
            license     = licenses.unfree;
            platforms   = platforms.all;
          };
        };

    in

    rec {
      overlay = final: prev: {
        liberate = final.callPackage liberate {};
      };

      packages = forAllSystems (system:
        {
          inherit (nixpkgsFor.${system}) liberate;
        });

      defaultPackage = forAllSystems (system: self.packages.${system}.liberate);
      devShell = defaultPackage;

      hydraJobs.liberate = self.defaultPackage;
    };
}
