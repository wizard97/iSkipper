# iSkipper
<img src="https://github.com/wizard97/iSkipper/blob/master/pics/IMG_20170425_002113.jpg?raw=true" alt="iClicker" width="250" height="250">
Reverse engineering the overly priced iClicker2 and creating an open source evil version, the iSkipper


## Warning
If you use this for nefarious purposes, it's your fault if your iSkipper gets you in trouble.

## Background
Everybody hates going to lecture just because of stupid iClicker quizzes/questions.
Two Cornell University engineers set out to do away with iClicker polling by reverse engineering it
and finding any vulnerabilities to exploit. For $5 of parts, an iSkipper can be built that
can do any number of evil things:
* Submit hundreds of answers per second under fake iClicker IDs
* Capture everyones else's iClicker ID and associated answer
* After capture of other students ID's, resubmit other peoples answers under something else
* DDOS the base station, so other people can't submit
* Automatically answer questions for you so you can skip lecture


## Making Your Own iSkipper

### The Software
The software is Arduino based, with several evil examples, and an easy to use API
for creating your own evil programs. All we ask is you write your own program/exploit
, submit a pull request and put it in the example folder, so other students can enjoy it too.

### Installing
Just drag and drop the `emulator/iSkipper` folder into your Arduino library folder as you
would install any other Arduino library. Open the IDE and you should be able to lead examples.
One dependency is a Queuing library: [RingBufCPP](https://github.com/wizard97/Embedded_RingBuf_CPP).

### The Hardware
* Any Arduino with a 3.3V logic level, a SPI bus, and can source bursts of ~100mA at 3.3V
* The 900 MHz RFM69W/RFM69HW transceiver (can be had on eBay for a few $)

Or get both in one package for $25 from [Adafruit](https://learn.adafruit.com/adafruit-feather-m0-radio-with-rfm69-packet-radio/overview).
Or support Felix from LowPowerLab and buy a $13 [Moteino](https://lowpowerlab.com/shop/product/99)

## License
MIT, but please submit a pull request with your own programs

## Current Project Progress
The entire iClicker protocol is proprietary, so it had to be reverse engineered.
There is still reverse engineering needing to be done, especially for the base station.
After a whole semester of work, this is what the latest iSkipper can and can not do.
Note, there might not be Arduino examples written yet for some of these, but the API supports it. They should
be trivial to make.

| Functionality                        | Status           
| ------------------------------------ |:-------------:|
| Generate valid fake IDs              | Working       |
| Submit answers to a base stations    | Working       |
| Work across all channels             | Working       |
| Emulate a normal remote              | Working       |
| Capture other peoples answers and ID | Working       |
| DDOS the basestation                 | Somewhat      |
| Capture ACK's send from base station | Working       |
| Decode ACK from base station         | NO            |
| Emulate a base station               | Somewhat      |
| Work in large lectures  (>400)       | Working       |

Basically all that needs to be done is decoding the 7 byte ACK sent from the basestation,
then the iSkipper can emulate every part of the real iClicker.


## How we new this was possible when we started
Given that your iClicker id can be decoded by the base station, any sort of hashing
done before the remote transmits your ID must be reversible, and hence vulnerable.

## Techniques/tools Used to Reverse Engineer
* OSINT (datasheets, papers, etc..)
* Logic analyzers
* HEX dumps and disassembly
* Software Defined Radio captures
* Lots of playing around looking for patterns
