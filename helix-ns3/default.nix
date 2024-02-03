{ stdenv
, lib
, fetchFromGitHub
, ns-3
, cmake
, pkg-config
, helix-ns3-bridge ? (callPackage ../helix-ns3-bridge {})
, callPackage
}:

let
  gitignoreSource = (
    import (fetchFromGitHub {
      owner = "hercules-ci";
      repo = "gitignore.nix";
      rev = "43e1aa1308018f37118e34d3a9cb4f5e75dc11d5";
      sha256 = "sha256-gGPa9qWNc6eCXT/+Z5/zMkyYOuRZqeFZBDbopNZQkuY=";
    }) { inherit lib; }
  ).gitignoreSource;

in
stdenv.mkDerivation {
  name = "helix-ns3";

  src = gitignoreSource ./.;

  nativeBuildInputs = [
    cmake pkg-config
  ];

  buildInputs = [
    ns-3 helix-ns3-bridge
  ];
}
