{
  description = "ww - Universal Wayland wallpaper setter";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        ww = pkgs.stdenv.mkDerivation rec {
          pname = "ww";
          version = "0.1.0";

          src = ./.;

          nativeBuildInputs = with pkgs; [
            pkg-config
            wayland-scanner
          ];

          buildInputs = with pkgs; [
            wayland
            wayland-protocols
            libwebp
            libtiff
            libjxl
            ffmpeg
            stb
          ];

          buildPhase = ''
            # Generate Wayland protocol bindings first
            bash ./generate_protocols.sh

            # Manually compile all source files
            mkdir -p build/obj

            # Compile protocol C files
            $CC -c build/protocols/wlr-layer-shell-unstable-v1-protocol.c \
              -o build/obj/wlr-layer-shell.o \
              -I${pkgs.wayland}/include

            $CC -c build/protocols/xdg-shell-protocol.c \
              -o build/obj/xdg-shell.o \
              -I${pkgs.wayland}/include

            # Compile C++ source files
            for src in src/*.cc; do
              obj="build/obj/$(basename $src .cc).o"
              echo "Compiling $src -> $obj"
              $CXX -c "$src" -o "$obj" \
                -std=c++20 \
                -O3 -march=native \
                -Iinc \
                -Ibuild/protocols \
                -I${pkgs.stb}/include/stb \
                $(pkg-config --cflags wayland-client libwebp libtiff-4 libjxl libavcodec libavformat libavutil libswscale) \
                -fno-exceptions -fno-rtti
            done

            # Link everything
            $CXX build/obj/*.o \
              -o ww \
              -pthread \
              $(pkg-config --libs wayland-client libwebp libtiff-4 libjxl libavcodec libavformat libavutil libswscale) \
              -lm
          '';

          installPhase = ''
            mkdir -p $out/bin
            mkdir -p $out/share/man/man1
            mkdir -p $out/share/bash-completion/completions
            mkdir -p $out/share/zsh/site-functions
            mkdir -p $out/share/fish/vendor_completions.d

            # Install binary
            cp ww $out/bin/

            # Install man page
            cp man/ww.1 $out/share/man/man1/

            # Install shell completions
            cp completions/ww.bash $out/share/bash-completion/completions/ww
            cp completions/_ww $out/share/zsh/site-functions/_ww
            cp completions/ww.fish $out/share/fish/vendor_completions.d/ww.fish
          '';

          meta = with pkgs.lib; {
            description = "Fast Wayland wallpaper setter with transitions and daemon mode";
            homepage = "https://github.com/jeebuscrossaint/ww";
            license = licenses.mit;
            platforms = platforms.linux;
            maintainers = [ ];
          };
        };
      in
      {
        packages.default = ww;
        packages.ww = ww;

        apps.default = {
          type = "app";
          program = "${ww}/bin/ww";
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            pkg-config
            wayland
            wayland-protocols
            wayland-scanner
            libwebp
            libtiff
            libjxl
            ffmpeg
            stb
            clang-tools
            shellcheck
          ];

          shellHook = ''
            echo "ww development environment"
            echo "Run './generate_protocols.sh' to generate protocol bindings"
          '';
        };
      }
    );
}
