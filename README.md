# `barretenberg-sys`

FFI bindings for the `barretenberg` library in Rust, providing convenience for zkSNARK proving and verification.

## Introduction

`barretenberg` is an efficient elliptic curve library, optimized for zkSNARK operations. This project provides Rust FFI bindings for easy integration into Rust projects.

## Dependencies

1. **barretenberg** (preferably at commit `87aeb375d7b434e0faf47abb79f97753ab760987`)

    You must first build and install `barretenberg` by following the instructions [in the README](https://github.com/AztecProtocol/barretenberg#getting-started). Note: `barretenberg` has its own [dependencies](https://github.com/AztecProtocol/barretenberg#dependencies), such as `cmake` and `ninja`, that need to be installed.

## Build dependencies

### Clone the `barretenberg` library to your preferred folder:
```sh
git clone https://github.com/visoftsolutions/barretenberg.git
```

### Build the library:
```sh
cd cpp
# Build natively.
cmake --preset default -DCMAKE_BUILD_TYPE=RelWithAssert
cmake --build --preset default ${@/#/--target }
cd ../
```

### Copy the build library and source code into the `barretenberg-sys` project:
```sh
cp -r barretenberg <root of the barretenberg-sys project>
cp cpp/build/lib/libbarretenberg.a <root of the barretenberg-sys project>/lib
```

## Build the Rust project:
```sh
cargo build
```

## Run Tests to ensure correct bindings:
```sh
cargo test --release
```

## Usage

[Include a basic example of how to use the bindings in a Rust application]

## Contribution

Feel free to open issues or pull requests if you find any problems or have suggestions. Contributions are welcome!