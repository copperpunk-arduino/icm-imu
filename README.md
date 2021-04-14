# IcmImu

This is a simple demonstration of the ICM-20948 Digital Motion Processor (DMP), which is at the heart of the COPPERPUNK dual-GPS INS (https://github.com/copperpunk-arduino/icm-ins-dual-gps).

In order to run this code, you will need an ICM-20948 breakout board, available from [Sparkfun](https://www.sparkfun.com/products/15335) or [Adafruit](https://www.adafruit.com/product/4554). We're using the Sparkfun version for this example. In addition, you will want an Arduino board with an SAMD21 chip. If you want small and cheap, it's tough to beat the [Adafruit QT Py SAMD21 Dev Board](https://www.adafruit.com/product/4600) ($6!!), which is what we'll be using here.

## Hardware Setup
### The Qwiic Way
The easiest way to connect our hardware is using a [Qwiic](https://www.sparkfun.com/qwiic#products) cable. This setup requires NO soldering.

 <p align="center"><img src="https://static.wixstatic.com/media/07c139_bfa70eb1af774d1fb03d700c95c8f6c7~mv2.jpg" width="50%"><img src="https://static.wixstatic.com/media/07c139_52a143075cd3455aac636387dd520937~mv2.jpg" width="50%">
 </p> 

### The Robust Way
If you'd like something a little more permanant, simply solder the four connections required for I&#x00B2;C operation: 3.3V, GND, SDA, and SCL. The result is even more compact. 
<p align="center"><img src="https://static.wixstatic.com/media/07c139_c8d6b548f7bd47218c19d5015c0b8ada~mv2.jpg" width="50%"><img src="https://static.wixstatic.com/media/07c139_b24f4e959a5b4a288255117ae77bd8f7~mv2.jpg" width="50%"></p>

## The Code
Now we'll need the Arduino [ICM-20948 DMP library](https://github.com/ericalbers/ICM20948_DMP_Arduino) written by Eric Albers. Once you've installed that, the IcmImu sketch should be ready to run.

> You'll notice that we use several `.ino` files when our code starts to get large. The Arduino IDE will separate these into different tabs automatically. They are named alphabetically to ensure they are placed in the same order every time.

You've got a few options in terms of output. In order to print debug messages, be sure that the following line is uncommented.
```
#define DEBUG
```
This sketch also has the ability to publish a data message (on a separate serial port) that adheres to the VectorNav binary protocol, essentially spoofing a [VN-100 IMU](https://www.vectornav.com/products/vn-100) (with much less capability, of course). If you don't have any need for this message, leave the following line commented out:
```
//#define PUBLISH_VN
```

You can adjust the data rate and publishing rate with the `kImuIntervalMs` and `kPublishVnBinary1IntervalMs` variables. And that's about it. We hope you enjoy your new IMU!

# Contact Us
If you have any suggestions for improving this repository, there are a few ways to get in touch:

*   Create a new issue
*   Submit a pull request
*   Virtual meeting using our [Open Source Office Hours](https://www.copperpunk.com/service-page/open-source-office-hours)