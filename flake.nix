{
  description = "C++ Development with Nix in 2023";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts"; # Make sure you have the right flake-parts repo
  };

  outputs = inputs@{ flake-parts, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      # This is the list of architectures that work with this project
      systems = [
        "x86_64-linux" "aarch64-linux" "aarch64-darwin" "x86_64-darwin"
      ];
      perSystem = { config, self', inputs', pkgs, system, ... }: {

        # devShells.default describes the default shell with C++, cmake, boost,
        # and gtest
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            boost
            cmake
            gtest
            clang-tools
          ];

          # Define the custom build command in the shell environment
          shellHook = ''
            echo "Welcome to the C++ development environment!"
            
            # Define the custom build command function
            function build_project() {
              echo "Building the project..."
              
              # Clean the build directory if it exists
              if [ -d "build" ]; then
                echo "Removing previous build directory..."
                rm -rf build
              fi

              # Create a new build directory
              mkdir -p build
              cd build

              # Run cmake to configure the build
              echo "Running CMake..."
              cmake ..

              # Run make to build the project
              echo "Running make..."
              make
            }

            # Optionally call build_project automatically when the shell starts
            # Comment this line out if you don't want the build process to run automatically
            # build_project
          '';
        };
      };
    };
}
