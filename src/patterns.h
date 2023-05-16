#pragma once
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "fire.h"
#include "bounce.h"

uint8_t auto_pattern_number = 0;
uint8_t current_pattern_number = 0;
uint8_t manual_pattern_number = 1;

uint8_t frame_hue = 0; // rotating "base color" used by many of the patterns

// ClassicFireEffect(int size, int cooling = 80, int sparking = 50, int sparks = 3, int sparkHeight = 4, bool breversed = true, bool bmirrored = true)
ClassicFireEffect fire_right(22, 80, 50, 3, 11, true, false, false);
ClassicFireEffect fire_left(22, 50, 50, 3, 11, true, false, true);

// BouncingBallEffect(size_t cLength, size_t ballCount = 3, byte fade = 0, bool bMirrored = false)
BouncingBallEffect bouncer(EFFECT_LEDS, 4, 1, true);

void no_pattern();
void auto_pattern();
void solid_fill();
void palette_fill();
void inoise8_mover();
void rainbow();
void confetti();
void sinelon();
void bpm();
void juggle();
void pride();
void classic_fire();
void bouncing_balls();
void breath();
void pacifica();
void twinkles();
void ripple();
void candy_cane();

void add_glitter(fract8 change_of_glitter);

struct pattern_definition
{
    String name;
    bool auto_cycle;
    void (*pattern)();
};

pattern_definition gPatterns[] = {
    {"cycle", false, auto_pattern},
    //{"rainbow", false, rainbow},
    {"rainbow", false, pride},
    {"ocean", true, pacifica},
    {"fire", true, classic_fire},
    {"confetti", true, confetti},
    {"sinelon", true, sinelon},
    {"bpm", true, bpm},
    {"juggle", true, juggle},
    {"breath", true, breath},
    {"twinkles", true, twinkles},
    {"color shift", true, palette_fill},
    {"balls", true, bouncing_balls},
    {"ripple", true, ripple},
    {"noise", true, inoise8_mover},
    {"stripes", true, candy_cane},
    {"fill", false, solid_fill},
    {"none", false, no_pattern},
} PROGMEM;

TBlendType get_current_blending()
{
    return LINEARBLEND;
}

///////////////////////////////////////////////////////////////////////////////
//
// No pattern
//
///////////////////////////////////////////////////////////////////////////////
void no_pattern()
{
    for (uint_fast8_t i = 0; i < EFFECT_LEDS; i++)
    {
        effect_leds[i] = CRGB::Black;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// cycle / auto pattern
//
///////////////////////////////////////////////////////////////////////////////
void auto_pattern()
{
    gPatterns[auto_pattern_number].pattern();
}

///////////////////////////////////////////////////////////////////////////////
//
// solid palette fill
//
///////////////////////////////////////////////////////////////////////////////
void solid_fill()
{
    //fill_palette(effect_leds, EFFECT_LEDS, 0, 255 / EFFECT_LEDS, gCurrentPalette, 255, get_current_blending());
    for (int pixelnumber = 0; pixelnumber < EFFECT_LEDS; pixelnumber++)
    {
        effect_leds[translate[pixelnumber]] = ColorFromPalette(gCurrentPalette, pixelnumber * 255 / EFFECT_LEDS, 255, get_current_blending());
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// palette_fill
//
///////////////////////////////////////////////////////////////////////////////
void palette_fill()
{
    static uint8_t startIndex = 0;
    uint8_t colorIndex = startIndex;
    for (int pixelnumber = 0; pixelnumber < EFFECT_LEDS; pixelnumber++)
    {
        // effect_leds[translate[pixlenumber]] = ColorFromPalette(gCurrentPalette, colorIndex, BRIGHTNESS, gCurrentBlending);
        nblend(effect_leds[translate[pixelnumber]], ColorFromPalette(gCurrentPalette, colorIndex, 255, get_current_blending()), random16(15));
        colorIndex += 3;
    }

    // fill_palette(effect_leds, EFFECT_LEDS, startIndex, 8, gCurrentPalette, BRIGHTNESS, gCurrentBlending);
    EVERY_N_MILLISECONDS(10)
    {
        startIndex++;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// inoise8_mover
//
///////////////////////////////////////////////////////////////////////////////
void inoise8_mover()
{
    static uint16_t dist;
    fadeToBlackBy(effect_leds, EFFECT_LEDS, 10);
    for (int i = 0; i < 3; i++)
    {
        uint8_t locn = inoise8(millis() * 8 / 64 + i * 500, millis() * 8 / 64);                                  // Get a new pixel location from moving noise.
        uint8_t pixlen = map(locn, 50, 192, 0, EFFECT_LEDS - 1) % (EFFECT_LEDS - 1);                             // Map that to the length of the strand, and ensure we don't go over.
        effect_leds[translate[pixlen]] = ColorFromPalette(gCurrentPalette, pixlen, 255, get_current_blending()); // Use that value for both the location as well as the palette index colour for the pixel.
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// rainbow
//
///////////////////////////////////////////////////////////////////////////////
void rainbow()
{
    // FastLED's built-in rainbow generator
    fill_rainbow(effect_leds, EFFECT_LEDS, frame_hue, 7);
}

///////////////////////////////////////////////////////////////////////////////
//
// confetti
//
///////////////////////////////////////////////////////////////////////////////
void confetti()
{
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy(effect_leds, EFFECT_LEDS, 10);
    int pixelnumber = random16(EFFECT_LEDS);
    effect_leds[translate[pixelnumber]] += ColorFromPalette(gCurrentPalette, frame_hue + random16(64), 255, get_current_blending());
    // nblend(effect_leds[translate[pixelnumber]], ColorFromPalette(gCurrentPalette, frame_hue+ random16(64), 255), 64);
}

///////////////////////////////////////////////////////////////////////////////
//
// sinelon
//
///////////////////////////////////////////////////////////////////////////////
void sinelon()
{
    // a colored dot sweeping back and forth, with fading trails
    uint8_t BeatsPerMinute = 60;
    fadeToBlackBy(effect_leds, EFFECT_LEDS, 10);
    int pixelnumber = beatsin16(BeatsPerMinute, 0, 10);
    switch (random16(10))
    {
    case 0:
        pixelnumber = cubicwave8(pixelnumber);
        break;
    case 1:
        pixelnumber = quadwave8(pixelnumber);
        break;
    case 2:
        pixelnumber = triwave8(pixelnumber);
        break;
    default:
        break;
    }
    // CRGB color = ColorFromPalette(gCurrentPalette, frame_hue, 255);
    effect_leds[translate[pixelnumber]] += ColorFromPalette(gCurrentPalette, pixelnumber, 255, get_current_blending());
    effect_leds[translate[pixelnumber + 11]] += ColorFromPalette(gCurrentPalette, pixelnumber + 11, 255, get_current_blending());
    effect_leds[translate[pixelnumber + 23]] += ColorFromPalette(gCurrentPalette, pixelnumber + 23, 255, get_current_blending());
    effect_leds[translate[pixelnumber + 34]] += ColorFromPalette(gCurrentPalette, pixelnumber + 34, 255, get_current_blending());
    // nblend(effect_leds[translate[pixelnumber]], ColorFromPalette(gCurrentPalette, frame_hue, 192), 64);
}

///////////////////////////////////////////////////////////////////////////////
//
// BPM
//
///////////////////////////////////////////////////////////////////////////////
void bpm()
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 10;
    uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
    for (int pixelnumber = 0; pixelnumber < EFFECT_LEDS; pixelnumber++)
    {
        effect_leds[translate[pixelnumber]] = ColorFromPalette(gCurrentPalette, frame_hue + (pixelnumber * 2), beat - frame_hue + (pixelnumber * 10), get_current_blending());
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Juggle
//
///////////////////////////////////////////////////////////////////////////////
void juggle()
{
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy(effect_leds, EFFECT_LEDS, 20);
    uint8_t dothue = 0;
    for (int i = 0; i < 8; i++)
    {
        uint8_t pixelnumber = beatsin16(i + 7, 0, EFFECT_LEDS - 1);

        // effect_leds[translate[beatsin16(i + 7, 0, EFFECT_LEDS - 1)]] |= CHSV(dothue, 200, 255);
        effect_leds[translate[pixelnumber]] = ColorFromPalette(gCurrentPalette, dothue, 255, get_current_blending());
        // nblend(effect_leds[translate[pixelnumber]], ColorFromPalette(gCurrentPalette, dothue, 255), 64);
        dothue += 32;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Pride
//
///////////////////////////////////////////////////////////////////////////////
// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
//

void pride()
{
    static uint16_t sPseudotime = 0;
    static uint16_t sLastMillis = 0;
    static uint16_t sHue16 = 0;

    uint8_t sat8 = beatsin88(87, 220, 250);
    uint8_t brightdepth = beatsin88(341, 96, 224);
    uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
    uint8_t msmultiplier = beatsin88(147, 23, 60);

    uint16_t hue16 = sHue16; // frame_hue * 256;
    uint16_t hueinc16 = beatsin88(113, 1, 3000);

    uint16_t ms = millis();
    uint16_t deltams = ms - sLastMillis;
    sLastMillis = ms;
    sPseudotime += deltams * msmultiplier;
    sHue16 += deltams * beatsin88(400, 5, 9);
    uint16_t brightnesstheta16 = sPseudotime;

    for (uint16_t i = 0; i < EFFECT_LEDS; i++)
    {
        hue16 += hueinc16;
        uint8_t hue8 = hue16 / 256;

        brightnesstheta16 += brightnessthetainc16;
        uint16_t b16 = sin16(brightnesstheta16) + 32768;

        uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
        uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
        bri8 += (255 - brightdepth);

        CRGB newcolor = CHSV(hue8, sat8, bri8);

        uint16_t pixelnumber = i;
        pixelnumber = (EFFECT_LEDS - 1) - pixelnumber;

        nblend(effect_leds[translate[pixelnumber]], newcolor, 64);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Wrapper to execute fire.h
//
///////////////////////////////////////////////////////////////////////////////
void classic_fire()
{
    auto_palette_number = 10;

    EVERY_N_MILLISECONDS(15)
    {
        fadeToBlackBy(effect_leds, EFFECT_LEDS, 10);
        fire_right.DrawFire();
        fire_left.DrawFire();
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Wrapper to execute bounce.h
//
///////////////////////////////////////////////////////////////////////////////
void bouncing_balls()
{
    EVERY_N_MILLISECONDS(30)
    {
        fadeToBlackBy(effect_leds, EFFECT_LEDS, 10);
        bouncer.Draw();
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Breath
// https://github.com/marmilicious/FastLED_examples/blob/master/breath_effect_v2.ino
//
///////////////////////////////////////////////////////////////////////////////
void breath()
{
    static float pulseSpeed = 0.3; // Larger value gives faster pulse.
    static bool video_dim = true;

    uint8_t hueA = 15;      // Start hue at valueMin.
    uint8_t satA = 230;     // Start saturation at valueMin.
    float valueMin = 120.0; // Pulse minimum value (Should be less then valueMax).

    uint8_t hueB = 95;      // End hue at valueMax.
    uint8_t satB = 255;     // End saturation at valueMax.
    float valueMax = 255.0; // Pulse maximum value (Should be larger then valueMin).

    uint8_t hue = hueA;
    uint8_t sat = satA;
    float val = valueMin;
    uint8_t hueDelta = hueA - hueB;
    static float delta = (valueMax - valueMin) / 2.35040238;

    float dV = ((exp(sin(pulseSpeed * millis() / 2000.0 * PI)) - 0.36787944) * delta);
    val = valueMin + dV;
    hue = map(val, valueMin, valueMax, hueA, hueB); // Map hue based on current val
    sat = map(val, valueMin, valueMax, satA, satB); // Map sat based on current val

    for (int pixelnumber = 0; pixelnumber < EFFECT_LEDS; pixelnumber++)
    {
        // effect_leds[translate[pixelnumber]] = CHSV(hue, sat, val);
        effect_leds[translate[pixelnumber]] = ColorFromPalette(gCurrentPalette, hue, val, get_current_blending());
        // nblend(effect_leds[translate[pixelnumber]], ColorFromPalette(gCurrentPalette, hue, val), 64);

        // You can experiment with commenting out these dim8_video lines
        // to get a different sort of look.
        if (video_dim)
        {
            effect_leds[translate[pixelnumber]].r = dim8_video(effect_leds[translate[pixelnumber]].r);
            effect_leds[translate[pixelnumber]].g = dim8_video(effect_leds[translate[pixelnumber]].g);
            effect_leds[translate[pixelnumber]].b = dim8_video(effect_leds[translate[pixelnumber]].b);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Pacifica
//
///////////////////////////////////////////////////////////////////////////////
//
// The code for this animation is more complicated than other examples, and
// while it is "ready to run", and documented in general, it is probably not
// the best starting point for learning.  Nevertheless, it does illustrate some
// useful techniques.
//
// In this animation, there are four "layers" of waves of light.
//
// Each layer moves independently, and each is scaled separately.
//
// All four wave layers are added together on top of each other, and then
// another filter is applied that adds "whitecaps" of brightness where the
// waves line up with each other more.  Finally, another pass is taken
// over the led array to 'deepen' (dim) the blues and greens.
//
// The speed and scale and motion each layer varies slowly within independent
// hand-chosen ranges, which is why the code has a lot of low-speed 'beatsin8' functions
// with a lot of oddly specific numeric ranges.
//
// These three custom blue-green color palettes were inspired by the colors found in
// the waters off the southern coast of California, https://goo.gl/maps/QQgd97jjHesHZVxQ7
//

// Add one layer of waves into the led array
void pacifica_one_layer(CRGBPalette16 &p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
    uint16_t ci = cistart;
    uint16_t waveangle = ioff;
    uint16_t wavescale_half = (wavescale / 2) + 20;
    for (uint16_t i = 0; i < EFFECT_LEDS; i++)
    {
        waveangle += 250;
        uint16_t s16 = sin16(waveangle) + 32768;
        uint16_t cs = scale16(s16, wavescale_half) + wavescale_half;
        ci += cs;
        uint16_t sindex16 = sin16(ci) + 32768;
        uint8_t sindex8 = scale16(sindex16, 240);
        CRGB c = ColorFromPalette(p, sindex8, bri, LINEARBLEND);
        effect_leds[translate[i]] += c;
    }
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void pacifica_add_whitecaps()
{
    uint8_t basethreshold = beatsin8(9, 55, 65);
    uint8_t wave = beat8(7);

    for (uint16_t i = 0; i < EFFECT_LEDS; i++)
    {
        uint8_t threshold = scale8(sin8(wave), 20) + basethreshold;
        wave += 7;
        uint8_t l = effect_leds[translate[i]].getAverageLight();
        if (l > threshold)
        {
            uint8_t overage = l - threshold;
            uint8_t overage2 = qadd8(overage, overage);
            effect_leds[translate[i]] += CRGB(overage, overage2, qadd8(overage2, overage2));
        }
    }
}

// Deepen the blues and greens
void pacifica_deepen_colors()
{
    for (uint16_t i = 0; i < EFFECT_LEDS; i++)
    {
        effect_leds[translate[i]].blue = scale8(effect_leds[translate[i]].blue, 145);
        effect_leds[translate[i]].green = scale8(effect_leds[translate[i]].green, 200);
        effect_leds[translate[i]] |= CRGB(2, 5, 7);
    }
}

void pacifica()
{
    // Increment the four "color index start" counters, one for each wave layer.
    // Each is incremented at a different speed, and the speeds vary over time.
    static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
    static uint32_t sLastms = 0;
    uint32_t ms = GET_MILLIS();
    uint32_t deltams = ms - sLastms;
    sLastms = ms;
    uint16_t speedfactor1 = beatsin16(3, 179, 269);
    uint16_t speedfactor2 = beatsin16(4, 179, 269);
    uint32_t deltams1 = (deltams * speedfactor1) / 256;
    uint32_t deltams2 = (deltams * speedfactor2) / 256;
    uint32_t deltams21 = (deltams1 + deltams2) / 2;
    sCIStart1 += (deltams1 * beatsin88(1011, 10, 13));
    sCIStart2 -= (deltams21 * beatsin88(777, 8, 11));
    sCIStart3 -= (deltams1 * beatsin88(501, 5, 7));
    sCIStart4 -= (deltams2 * beatsin88(257, 4, 6));

    // Clear out the LED array to a dim background blue-green
    fill_solid(effect_leds, EFFECT_LEDS, CRGB(2, 6, 10));

    // Render each of four layers, with different scales and speeds, that vary over time
    pacifica_one_layer(pacifica_palette_1, sCIStart1, beatsin16(3, 11 * 256, 14 * 256), beatsin8(10, 70, 130), 0 - beat16(301));
    pacifica_one_layer(pacifica_palette_2, sCIStart2, beatsin16(4, 6 * 256, 9 * 256), beatsin8(17, 40, 80), beat16(401));
    pacifica_one_layer(pacifica_palette_3, sCIStart3, 6 * 256, beatsin8(9, 10, 38), 0 - beat16(503));
    pacifica_one_layer(pacifica_palette_3, sCIStart4, 5 * 256, beatsin8(8, 10, 28), beat16(601));

    // Add brighter 'whitecaps' where the waves lines up more
    pacifica_add_whitecaps();

    // Deepen the blues and greens a bit
    pacifica_deepen_colors();
}

///////////////////////////////////////////////////////////////////////////////
//
// Twinkles
//
///////////////////////////////////////////////////////////////////////////////
//  This function loops over each pixel, calculates the
//  adjusted 'clock' that this pixel should use, and calls
//  "CalculateOneTwinkle" on each pixel.  It then displays
//  either the twinkle color of the background color,
//  whichever is brighter.

// This function takes a pixel, and if its in the 'fading down'
// part of the cycle, it adjusts the color a little bit like the
// way that incandescent bulbs fade toward 'red' as they dim.
void cool_like_incandescent(CRGB &c, uint8_t phase)
{
    if (phase < 128)
        return;

    uint8_t cooling = (phase - 128) >> 4;
    c.g = qsub8(c.g, cooling);
    c.b = qsub8(c.b, cooling * 2);
}

// This function is like 'triwave8', which produces a
// symmetrical up-and-down triangle sawtooth waveform, except that this
// function produces a triangle wave with a faster attack and a slower decay:
//
//     / \ 
//    /     \ 
//   /         \ 
//  /             \ 
//
uint8_t attack_decay_wave8(uint8_t i)
{
    if (i < 86)
    {
        return i * 3;
    }
    else
    {
        i -= 86;
        return 255 - (i + (i / 2));
    }
}

//  This function takes a time in pseudo-milliseconds,
//  figures out brightness = f( time ), and also hue = f( time )
//  The 'low digits' of the millisecond time are used as
//  input to the brightness wave function.
//  The 'high digits' are used to select a color, so that the color
//  does not change over the course of the fade-in, fade-out
//  of one cycle of the brightness wave function.
//  The 'high digits' are also used to determine whether this pixel
//  should light at all during this cycle, based on the TWINKLE_DENSITY.
CRGB compute_one_twinkle(uint32_t ms, uint8_t salt)
{
    // Overall twinkle speed.
    // 0 (VERY slow) to 8 (VERY fast).
    // 4, 5, and 6 are recommended, default is 4.
    static uint8_t twinkle_speed = 3;

    // Overall twinkle density.
    // 0 (NONE lit) to 8 (ALL lit at once).
    // Default is 5.
    static uint8_t twinkle_density = 4;

    // If set to 1, colors will fade out slighted 'reddened',
    // similar to how incandescent bulbs change color
    // as they get dim down.
    // TODO: set this to 1 in december and 0 other times
    static bool incandescent = 1;

    uint16_t ticks = ms >> (8 - twinkle_speed);
    uint8_t fastcycle8 = ticks;
    uint16_t slowcycle16 = (ticks >> 8) + salt;
    slowcycle16 += sin8(slowcycle16);
    slowcycle16 = (slowcycle16 * 2053) + 1384;
    uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);

    uint8_t bright = 0;
    if (((slowcycle8 & 0x0E) / 2) < twinkle_density)
    {
        bright = attack_decay_wave8(fastcycle8);
    }

    uint8_t hue = slowcycle8 - salt;
    CRGB c;
    if (bright > 0)
    {
        c = ColorFromPalette(gCurrentPalette, hue, bright, NOBLEND);
        if (incandescent == 1)
        {
            cool_like_incandescent(c, fastcycle8);
        }
    }
    else
    {
        c = CRGB::Black;
    }
    return c;
}

void twinkles()
{
    // "PRNG16" is the pseudorandom number generator
    // It MUST be reset to the same starting value each time
    // this function is called, so that the sequence of 'random'
    // numbers that it generates is (paradoxically) stable.
    uint16_t PRNG16 = 11337;
    uint32_t clock32 = millis();

    CRGB bg = CRGB::Black;
    uint8_t backgroundBrightness = bg.getAverageLight();

    for (uint16_t i = 0; i < EFFECT_LEDS; i++)
    {
        PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
        uint16_t myclockoffset16 = PRNG16;         // use that number as clock offset
        PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
        // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
        uint8_t myspeedmultiplierQ5_3 = ((((PRNG16 & 0xFF) >> 4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
        uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
        uint8_t myunique8 = PRNG16 >> 8; // get 'salt' value for this pixel

        // We now have the adjusted 'clock' for this pixel, now we call
        // the function that computes what color the pixel should be based
        // on the "brightness = f( time )" idea.
        CRGB c = compute_one_twinkle(myclock30, myunique8);

        uint8_t cbright = c.getAverageLight();
        int16_t deltabright = cbright - backgroundBrightness;
        if (deltabright >= 32 || (!bg))
        {
            // If the new pixel is significantly brighter than the background color,
            // use the new color.
            effect_leds[translate[i]] = c;
        }
        else if (deltabright > 0)
        {
            // If the new pixel is just slightly brighter than the background color,
            // mix a blend of the new color and the background color
            effect_leds[translate[i]] = blend(bg, c, deltabright * 8);
        }
        else
        {
            // if the new pixel is not at all brighter than the background color,
            // just use the background color.
            effect_leds[translate[i]] = bg;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// From https://www.youtube.com/watch?v=IrMzopUe8F4
//
///////////////////////////////////////////////////////////////////////////////
int wrap(int step)
{
    if (step < 0)
        return EFFECT_LEDS + step;
    if (step > EFFECT_LEDS - 1)
        return step - EFFECT_LEDS;
    return step;
}

void do_ripple()
{
    static int color = random(256);
    static int center = random(EFFECT_LEDS);
    static int step = -1;
    static int maxSteps = random(20);
    static float fadeRate = 0.8;
    static int diff;

    // background color
    static uint32_t currentBg = random(255);
    static uint32_t nextBg = currentBg;

    if (currentBg == nextBg)
    {
        nextBg = random(255);
    }
    else if (nextBg > currentBg)
    {
        currentBg++;
    }
    else
    {
        currentBg--;
    }
    for (uint16_t l = 0; l < EFFECT_LEDS; l++)
    {
        nblend(effect_leds[translate[l]], ColorFromPalette(gCurrentPalette, currentBg, 255, get_current_blending()), 45);
    }

    if (step == -1)
    {
        center = random(EFFECT_LEDS);
        color = random(256);
        step = 0;
    }

    if (step == 0)
    {
        effect_leds[translate[center]] = ColorFromPalette(gCurrentPalette, color, 255, get_current_blending());
        step++;
    }
    else
    {
        if (step < maxSteps)
        {
            effect_leds[translate[wrap(center + step)]] = ColorFromPalette(gCurrentPalette, color, pow(fadeRate, step) * 255, get_current_blending()); // CHSV(color, 255, pow(fadeRate, step) * 255);
            effect_leds[translate[wrap(center - step)]] = ColorFromPalette(gCurrentPalette, color, pow(fadeRate, step) * 255, get_current_blending()); // CHSV(color, 255, pow(fadeRate, step) * 255);
            if (step > 3)
            {
                effect_leds[translate[wrap(center + step - 3)]] = ColorFromPalette(gCurrentPalette, color, pow(fadeRate, step - 2) * 255, get_current_blending()); // CHSV(color, 255, pow(fadeRate, step - 2) * 255);
                effect_leds[translate[wrap(center - step + 3)]] = ColorFromPalette(gCurrentPalette, color, pow(fadeRate, step - 2) * 255, get_current_blending()); // CHSV(color, 255, pow(fadeRate, step - 2) * 255);
            }
            step++;
        }
        else
        {
            step = -1;
        }
    }
}

void ripple()
{
    EVERY_N_MILLISECONDS(30)
    {
        do_ripple();
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Candy Cane
//
///////////////////////////////////////////////////////////////////////////////
// Draw alternating bands of color, 2 or 3 colors.
// When using three colors, color1 is used between the other two colors.
void candy_cane()
{
    CRGB color1 = CRGB::White; // color used between color 2 (and 3 if used)
    CRGB color2 = CRGB::Red;
    // CRGB color3 = CHSV(0,170,255);  //optional 3rd color
    CRGB color3 = CRGB(0, 255, 0); // optional 3rd color
    const uint16_t travelSpeed = 150;
    int shiftBy = 1;                 // shiftBy can be positive or negative (to change direction)
    static uint8_t numColors = 3;    // Can be either 2 or 3
    static uint8_t stripeLength = 5; // number of pixels per color
    static int offset;
    static uint8_t time_per_frame = 1000 / FRAMES_PER_SECOND;
    static uint8_t loop_count = 0;

    EVERY_N_SECONDS(5)
    {
        numColors = random16(2, 4);    // picks either 2 or 3
        stripeLength = random16(3, 6); // picks random length
    }

    if (loop_count > (travelSpeed / time_per_frame))
    {
        loop_count = 0;
        if (numColors == 2)
        {
            for (uint8_t i = 0; i < EFFECT_LEDS; i++)
            {
                if ((i + offset) % ((numColors)*stripeLength) < stripeLength)
                {
                    effect_leds[translate[i]] = color2;
                }
                else
                {
                    effect_leds[translate[i]] = color1;
                }
            }
        }

        if (numColors == 3)
        {
            for (uint8_t i = 0; i < EFFECT_LEDS; i++)
            {
                if ((i + offset) % ((numColors + 1) * stripeLength) < stripeLength)
                {
                    effect_leds[translate[i]] = color2;
                }
                else if ((i + offset + (2 * stripeLength)) % ((numColors + 1) * stripeLength) < stripeLength)
                {
                    effect_leds[translate[i]] = color3;
                }
                else
                {
                    effect_leds[translate[i]] = color1;
                }
            }
        }

        offset = offset + shiftBy;
        if (shiftBy > 0)
        { // for positive shiftBy
            if (offset >= EFFECT_LEDS)
                offset = 0;
        }
        else
        { // for negitive shiftBy
            if (offset < 0)
                offset = EFFECT_LEDS;
        }
    }

    loop_count++;
}

///////////////////////////////////////////////////////////////////////////////
//
// Glitter!!
//
///////////////////////////////////////////////////////////////////////////////
void add_glitter(fract8 change_of_glitter)
{
    if (random16(100) < change_of_glitter)
    {
        effect_leds[random16(EFFECT_LEDS)] += CRGB::White;
    }
}
