{
	description = "mdsplus scientific database";
	inputs.nixpkgs.url = "github:nixos/nixpkgs/21.05";

	outputs = { self, nixpkgs }:
		with import nixpkgs {
			system = "x86_64-linux";
		};
	
	let
		mdsplus = stdenv.mkDerivation rec {
			name = "mdsplus";
			src = self;
			# https://github.com/MDSplus/mdsplus/blob/alpha/README.INSTALL
			buildInputs = [ 
				gperf
				jdk8
				jre8
				libxml2
				readline
				motif
				perl
				xorg.libXt
			];
			patchPhase = ''
				patchShebangs --build .
			'';
			preConfigure = ''
				./bootstrap
			'';
			nativeBuildInputs = [
				which
				automake
				autoconf
				autoconf-archive
				python
				git
				flex
				bison
				gfortran
				rsync
			];
		};

		mdsplus_wrapped = runCommand "mdsplus" { 
			buildInputs = [ mdsplus ]; 
			nativeBuildInputs = [ makeWrapper ];
			} ''
				for b in $(ls ${mdsplus}/bin); do
					makeWrapper ${mdsplus}/bin/$b $out/bin/$b --set MDSPLUS_DIR "${mdsplus}"
				done
			'';

	in {
		packages.x86_64-linux = { mdsplus = mdsplus_wrapped; };
		defaultPackage.x86_64-linux = self.packages.x86_64-linux.mdsplus;
	};
}
