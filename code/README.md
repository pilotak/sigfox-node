# Code
In order to compile the firmware you need to install:
- GCC ARM
- Python
- mbed-cli

## Compile
`mbed deploy` will automatically install mbed OS and following libraries
- https://github.com/pilotak/wisol
- https://github.com/pilotak/SHTC3
- https://github.com/pilotak/BQ35100

```sh
mbed deploy && mbed compile -m SIGFOX_TH_V1 -t GCC_ARM --profile release
```
