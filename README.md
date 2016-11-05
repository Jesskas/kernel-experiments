## kernel-experiments
There's nothing really notable beyond a messy "Hello world" here, for now.
Half of this is a learning process, half of this follows a goal of a minimalistic kernel for embedded devices everywhere, starting with raspberry pi's, and hopefully moving on to Android-based smartphones and other tablets in the future.

# What this works on
For now, only devices with a BCM2835 CPU; testing on a Raspberry Pi Zero and a Raspberry Pi B.

# What happens so far
```
git clone https://github.com/Jesskas/kernel-experiments.git
cd kernel-experiments
make
```
You want the `kernel.img` file that is produced. Place this file, along with `bootcode.bin` and `start.elf` from [here](https://github.com/raspberrypi/firmware/tree/master/boot "Raspberry Pi Firmware") all in the root of a SD card, and simply plug in a raspberry pi. What you should see on the screen is this:
```
Hello, world!
well...
```
(If this is not the case, sounds interesting, let me know)

# Credits:
- [dwelch67](https://github.com/dwelch67) for the [raspberry pi bare metal examples](https://github.com/dwelch67/raspberrypi) as a perfect starting point for me in bare metal development
- The success stories in the osdev (https://www.reddit.com/r/osdev/, http://wiki.osdev.org, #osdev, etc.) for the inspiration!
