# ZC

ZC is a dialect of the C programming language made to "parody" Zig. 
ZC uses the clang compiler (and only it), because it uses clang blocks for lambda functions, and requires the `libblocksruntime` library to build

## Usage

To start new project with `zc`:

```sh
zc init your_project_name
```

To build file from `code/` directory:

```sh
zc build file_without_ext
```

And to run:

```sh
zc run file_without_ext
```

You also can build without debugging info with `release` instead of _build_

### Test system

ZC has testing system, similliar to Zig's. To make testing file:

```c
$test(your_test_name){
    // do something
    return noerropt; // no errors
}
```

And to build and run file with tests:

```sh
zc test file
```

### Examples

More examples on `./code/passed`
