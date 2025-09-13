{ stdenv, clang, gtk3, pkg-config, gtk-layer-shell, cjson }:
stdenv.mkDerivation rec {
	name = "program-${version}";
	version = "1.0";

	src = ../.;

	nativeBuildInputs = [
		clang
		pkg-config
		cjson
	];
	buildInputs = [
		gtk3
		gtk-layer-shell
	];

	buildPhase = ''
make
	'';

	installPhase = ''
		mkdir -p $out/bin
		cp exec $out/bin
	'';
}
