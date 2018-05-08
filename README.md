A compressor Pure Data external using [pd-lib-builder](https://github.com/pure-data/pd-lib-builder), based on the pure-data helloworld repo (https://github.com/pure-data/helloworld.git). 

## Usage ##

Clone and build this example via:

    git clone --recursive https://github.com/alexheinrich/pd-compressor
    cd pd-compressor
    make

Make sure you use the `--recursive` flag when checking out the repository so that the pd-lib-builder dependency is also checked out. If you forgot to add it, you can also manually fetch pd-lib-builder via:

    cd pd-compressor
    git submodule init
    git submodule update
