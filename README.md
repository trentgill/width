# width

Experiment in using the Carp language to write for an embedded system.

This project uses the [W/](https://www.whimsicalraps.com/products/wslash) Eurorack module as the hardware platform,
providing:


* 1x1 Audio in/out
* 2x CV in/out (configurable)
* i2c connector
* SD card for mass storage
* 3x momentary push buttons
* 1x toggle switch (on)/off/(on)
* 10x leds for visual feedback

The intention is not to satisfy a particular feature set but instead to
consider how functional style & other lisp goodies can be used in the
context of hardware systems and audio signal processing.

Note: *This current approach is not focused on performance.* The hardware
libraries are wrappers on top of the ST provided HAL, where a future
use case would likely abandon the HAL for direct manipulation. Due to
the number of peripherals on the STM32f765 processor, rewriting this
is impractical as a lone-developer. That rewrite would be a project for
a large team of contributors if and when the use-case is proved.
