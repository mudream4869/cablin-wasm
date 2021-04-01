# Cablin-WASM

## Clone

```
git clone git@github.com:mudream4869/cablin-wasm.git --recursive
```

## Build

```
mkdir build
cd build
emcmake cmake -DYAML_CPP_BUILD_TESTS=OFF ..
make
```

## Test

```
cd public
python3 -m http.server
```
