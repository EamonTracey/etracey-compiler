# B-Minor

### Build and Test
This B-Minor compiler currently supports encoding, scanning, parsing, printing, name resolution, typechecking, and code generation. That is ... everything!
```sh
make test
./bminor --encode <FILE>
```
```sh
make test
./bminor --scan <FILE>
```
```sh
make test
./bminor --parse <FILE>
```
```sh
make test
./bminor --print <FILE>
```
```sh
make test
./bminor --resolve <FILE>
```
```sh
make test
./bminor --typecheck <FILE>
```
```sh
make test
./bminor --codegen <FILE> <FILE>
```
