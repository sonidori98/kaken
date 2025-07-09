#![no_std]
#![no_main]

use arduino_hal::port::{mode::Output, Pin};
use embedded_hal::digital::OutputPin;
use panic_halt as _;

const PATTERN: [[bool; 7]; 10] = [
    // 0
    [true, true, true, true, true, true, false],
    // 1
    [false, true, true, false, false, false, false],
    // 2
    [true, true, false, true, true, false, true],
    // 3
    [true, true, true, true, false, false, true],
    // 4
    [false, true, true, false, false, true, true],
    // 5
    [true, false, true, true, false, true, true],
    // 6
    [true, false, true, true, true, true, true],
    // 7
    [true, true, true, false, false, false, false],
    // 8
    [true, true, true, true, true, true, true],
    // 9
    [true, true, true, true, false, true, true],
];

type SegmentPins = (
    Pin<Output, arduino_hal::port::D2>,
    Pin<Output, arduino_hal::port::D3>,
    Pin<Output, arduino_hal::port::D4>,
    Pin<Output, arduino_hal::port::D5>,
    Pin<Output, arduino_hal::port::D6>,
    Pin<Output, arduino_hal::port::D7>,
    Pin<Output, arduino_hal::port::D8>,
);

struct SeveSegmentDisplay {
    segments: SegmentPins,
}

impl SeveSegmentDisplay {
    fn new(segments: SegmentPins) -> Self {
        Self { segments }
    }

    fn display_digit(&mut self, digit: usize) {
        if digit >= PATTERN.len() {
            self.clear();
            return;
        }
        let pattern = PATTERN[digit];
        self.segments.0.set_state(pattern[0].into()).unwrap();
        self.segments.1.set_state(pattern[1].into()).unwrap();
        self.segments.2.set_state(pattern[2].into()).unwrap();
        self.segments.3.set_state(pattern[3].into()).unwrap();
        self.segments.4.set_state(pattern[4].into()).unwrap();
        self.segments.5.set_state(pattern[5].into()).unwrap();
        self.segments.6.set_state(pattern[6].into()).unwrap();
    }

    fn clear(&mut self) {
        self.segments.0.set_low();
        self.segments.1.set_low();
        self.segments.2.set_low();
        self.segments.3.set_low();
        self.segments.4.set_low();
        self.segments.5.set_low();
        self.segments.6.set_low();
    }
}

#[arduino_hal::entry]
fn main() -> ! {
    let dp = arduino_hal::Peripherals::take().unwrap();
    let pins = arduino_hal::pins!(dp);

    let segmetns: SegmentPins = (
        pins.d2.into_output(),
        pins.d3.into_output(),
        pins.d4.into_output(),
        pins.d5.into_output(),
        pins.d6.into_output(),
        pins.d7.into_output(),
        pins.d8.into_output(),
    );

    let mut display = SeveSegmentDisplay::new(segmetns);

    let point = 0;
    let mut is_countdown = true;

    loop {
        if is_countdown {
            for i in (0..=9).rev() {
                display.display_digit(i);
                arduino_hal::delay_ms(1000);
            }
            is_countdown = false;
        } else {
            display.clear();
            arduino_hal::delay_ms(500);
            display.display_digit(point as usize);
            arduino_hal::delay_ms(500);
        }
    }
}
