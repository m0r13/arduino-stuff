#include "ledstrip.h"

float normalizeHue(float hue) {
    hue = fmod(hue, 1.0);
    if (hue < 0)
        return 1 + hue;
    return hue;
}

float randomHueNear(float hue, float radius) {
    float minimumPossible = hue - radius;
    float maximumPossible = hue + radius;
    float alpha = float(random(1025)) / 1024.0;
    return normalizeHue((1-alpha) * minimumPossible + alpha * maximumPossible);
}

long hsvToRGB(float h, float s, float v) {
  /*
     modified from Alvy Ray Smith's site:
   http://www.alvyray.com/Papers/hsv2rgb.htm
   H is given on [0, 6]. S and V are given on [0, 1].
   RGB is returned as a 24-bit long #rrggbb
   */
  h = h * 6.0;
  int i;
  float m, n, f;

  // not very elegant way of dealing with out of range: return black
  if ((s<0.0) || (s>1.0) || (v<0.0) || (v>1.0)) {
    return 0L;
  }

  if ((h < 0.0) || (h > 6.0)) {
    return long( v * 255 ) + long( v * 255 ) * 256 + long( v * 255 ) * 65536;
  }
  i = floor(h);
  f = h - i;
  if ( !(i&1) ) {
    f = 1 - f; // if i is even
  }
  m = v * (1 - s);
  n = v * (1 - s * f);
  switch (i) {
  case 6:
  case 0: // RETURN_RGB(v, n, m)
    return long(v * 255 ) * 65536 + long( n * 255 ) * 256 + long( m * 255);
  case 1: // RETURN_RGB(n, v, m) 
    return long(n * 255 ) * 65536 + long( v * 255 ) * 256 + long( m * 255);
  case 2:  // RETURN_RGB(m, v, n)
    return long(m * 255 ) * 65536 + long( v * 255 ) * 256 + long( n * 255);
  case 3:  // RETURN_RGB(m, n, v)
    return long(m * 255 ) * 65536 + long( n * 255 ) * 256 + long( v * 255);
  case 4:  // RETURN_RGB(n, m, v)
    return long(n * 255 ) * 65536 + long( m * 255 ) * 256 + long( v * 255);
  case 5:  // RETURN_RGB(v, m, n)
    return long(v * 255 ) * 65536 + long( m * 255 ) * 256 + long( n * 255);
  }
}

LEDStrip::LEDStrip(int redPin, int greenPin, int bluePin)
    : redPin(redPin),
      greenPin(greenPin),
      bluePin(bluePin),
      red(-1), green(-1), blue(-1), colorInverting(false) {
    resetPinMapping();
}

void LEDStrip::resetPinMapping() {
    setPinMapping(0, 1, 2);
}

void LEDStrip::setPinMapping(int redMapping, int greenMapping, int blueMapping) {
    this->redMapping = redMapping;
    this->greenMapping = greenMapping;
    this->blueMapping = blueMapping;
}

bool LEDStrip::hasColorInverting() const {
    return colorInverting;
}

void LEDStrip::setColorInverting(bool colorInverting) {
    this->colorInverting = colorInverting;
}

void LEDStrip::setRGB(int r, int g, int b) {
    int values[4] = {r, g, b, 0};
    r = values[redMapping];
    g = values[greenMapping];
    b = values[blueMapping];
    if (colorInverting) {
        r = 255 - r;
        g = 255 - g;
        b = 255 - b;
    }

    setInternalRGB(r, g, b);
}

void LEDStrip::off() {
    setInternalRGB(0, 0, 0);
}

void LEDStrip::setInternalRGB(int r, int g, int b) {
    if (r != red)
        analogWrite(redPin, r);
    else
        red = r;

    if (g != green)
        analogWrite(greenPin, g);
    else
        green = g;

    if (b != blue)
        analogWrite(bluePin, b);
    else
        blue = b;
}

void LEDStrip::setHSV(float h, float s, float v) {
    long rgb = hsvToRGB(h, s, v);
    setRGB((rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff);
}

MultipleLEDStrips::MultipleLEDStrips(LEDStrip* ledStrip1, LEDStrip* ledStrip2)
    : LEDStrip(0, 0, 0), ledStrip1(ledStrip1), ledStrip2(ledStrip2) {
}

void MultipleLEDStrips::setRGB(int r, int g, int b) {
    ledStrip1->setRGB(r, g, b);
    ledStrip2->setRGB(r, g, b);
}

void MultipleLEDStrips::off() {
    ledStrip1->off();
    ledStrip2->off();
}
