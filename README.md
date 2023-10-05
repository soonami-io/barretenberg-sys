# barretenberg-sys

FFI bindings to Barretenberg

## Dependencies

1. `barretenberg` (preferably at commit `87aeb375d7b434e0faf47abb79f97753ab760987`)

    Needs to be built and installed following the instructions [in the README](https://github.com/AztecProtocol/barretenberg#getting-started). Note that barretenberg has its own [dependencies](https://github.com/AztecProtocol/barretenberg#dependencies) that will need to be installed, such as `cmake` and `ninja`.

## Usage

```rust
pub fn pedersen() {
    let input = vec![0; 64];
    barretenberg_sys::blake2s::hash_to_field(&input);

    let f_zero = [0_u8; 32];
    let mut f_one = [0_u8; 32];
    f_one[31] = 1;
    let got = barretenberg_sys::pedersen::compress_native(&f_zero, &f_one);
    assert_eq!(
        "229fb88be21cec523e9223a21324f2e305aea8bff9cdbcb3d0c6bba384666ea1",
        hex::encode(got)
    );
}
```