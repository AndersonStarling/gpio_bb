GPIO custom Kernel Driver
===========================

1. Overview
 - `gpio_bb` is a compact Linux platform driver that implements a GPIO controller using register-level bit-bang operations.
 - It registers a `gpio_chip` and implements direction, get, and set callbacks accessing controller registers (OE, DATAIN, DATAOUT, SET/CLEAR).
 - The driver reads the number of GPIOs from the Device Tree property `ngpios` and expects a clock named `fck`.

2. Key files
 - `gpio_bb.c` — main driver implementation (probe/remove, direction handlers, get/set).
 - `Makefile` — kernel module build helper (use `KDIR`, and optionally `ARCH`/`CROSS_COMPILE`).
 - `script/` — helper scripts for building and merging Device Tree blobs.
 - `example/am335x-boneblack.dts` — example Device Tree node for BeagleBone-like platforms.
 - `test/` — test overlays and utilities to exercise the driver on hardware.

3. Project tree
```
project/gpio_driver/
├── gpio_bb.c               # driver source
├── Makefile                # module build wrapper (uses KDIR)
├── gpio_bb.o               # compiled object (optional)
├── gpio_bb.ko              # built kernel module (optional)
├── example/
│   └── am335x-boneblack.dts
├── script/
│   ├── build_ko.sh
│   ├── merge_dtb.sh
│   └── backup_dtb.sh
├── test/
│   ├── lib/
│   ├── overlay/
│   └── test_gpio/
└── readme.md
```

4. Quick build
1) Set your kernel build directory, for example:

```bash
export KDIR=/lib/modules/$(uname -r)/build
```

2) Build the module:

```bash
make
```

Cross-compile example:

```bash
make KDIR=/path/to/kernel ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
```

5. Usage notes
 - Add a platform node to your Device Tree with `.compatible = "gpio-bb"`, a `reg` range for IO registers, and a `ngpios` u32 property.
 - Provide a clock named `fck` (or adapt the driver if clocks are absent on your platform).
 - Once probed, the driver exposes standard GPIO interfaces through the kernel `gpio` framework.

6. License
 - GPL v2 (declared in the module source).

7. Notes
 - Check `script/` for convenience scripts before manual DTB or build steps.
 - Use the `test/` overlays and `example/` Device Tree node as a starting point for hardware validation.
