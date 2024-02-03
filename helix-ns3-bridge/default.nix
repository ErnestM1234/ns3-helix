let
  fenix = import (builtins.fetchGit {
    url = "https://github.com/nix-community/fenix";
    ref = "refs/heads/main";
    rev = "22e61fab6d93cfce2b9659bb7734762ad6a7cf11";
  }) { };

  fenixRustToolchain = fenix.fromToolchainFile {
    file = ./rust-toolchain.toml;
    # Update this whenever a new Noise revision is used!
    # Set to lib.fakeSha256 to acquire new value.
    sha256 = "sha256-uEVewvqWJ8YQCP7Mo9ILXWyXe3gCCdEZZPC14UKRK88=";
  };

  fenixRustPlatform = makeRustPlatform:
    makeRustPlatform {
      cargo = fenixRustToolchain;
      rustc = fenixRustToolchain;
    };

  cbindgen = { rustPlatform, cmake }:
    rustPlatform.buildRustPackage rec {
      pname = "cbindgen";
      version = "0.26.0";

      cargoLock.lockFile = "${src}/Cargo.lock";

      src = builtins.fetchGit {
        url = "https://github.com/mozilla/cbindgen";
        ref = "refs/heads/master";
        rev = "703b53c06f9fe2dbc0193d67626558cfa84a0f62";
      };

      # TODO: fixme!
      doCheck = false;
    };

in

{ stdenv
, lib
, callPackage
, makeRustPlatform
, nightlyRustPlatform ? (fenixRustPlatform makeRustPlatform)
, fixDarwinDylibNames
}:

nightlyRustPlatform.buildRustPackage rec {
  pname = "helix-ns3-bridge";
  version = "0.0.1";

  cargoLock.lockFile = ./Cargo.lock;

  src = lib.cleanSource ./.;

  nativeBuildInputs = lib.optionals stdenv.isDarwin [
    fixDarwinDylibNames
  ];

  postInstall = ''
    # By default, this will simply generate the libhelix-ns3-bridge.so / .dylib
    # file. We also want to generate the necessary C headers. We instantiate
    # cbindgen with the same toolchain as we use to build the library itself, to
    # avoid any incompatibilities:
    mkdir -p $out/include
    ${
      callPackage
        cbindgen
        { rustPlatform = nightlyRustPlatform; }
    }/bin/cbindgen \
      --config cbindgen.toml \
      --crate helix-ns3-bridge \
      --output $out/include/helix-ns3-bridge.h

    # Ship the pkg-config file
    mkdir -p $out/lib/pkgconfig
    cp ${./helix-ns3-bridge.pc} $out/lib/pkgconfig/helix-ns3-bridge.pc
    substituteInPlace $out/lib/pkgconfig/helix-ns3-bridge.pc \
      --replace "@exec_path@" "$out/bin" \
      --replace "@libdir@" "$out/lib" \
      --replace "@includedir@" "$out/include"
  '';
}
