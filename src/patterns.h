#pragma once
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "fire.h"
#include "bounce.h"

#define FADE_MIN 10
#define FADE_MAX 40

uint8_t auto_pattern_number = 0;
uint8_t current_pattern_number = 0;
uint8_t manual_pattern_number = 1;

// ClassicFireEffect(int size, int cooling = 80, int sparking = 50, int sparks = 3, int sparkHeight = 4, bool breversed = true, bool bmirrored = true)
ClassicFireEffect fire_right(22, 80, 50, 3, 11, true, false, false);
ClassicFireEffect fire_left(22, 50, 50, 3, 11, true, false, true);

// BouncingBallEffect(size_t cLength, size_t ballCount = 3, byte fade = 0, bool bMirrored = false)
BouncingBallEffect bouncer(EFFECT_LEDS, 4, 1, true);

void no_pattern(uint16_t frame, bool randomize);
void auto_pattern(uint16_t frame, bool randomize);
void solid_fill(uint16_t frame, bool randomize);
void liquid(uint16_t frame, bool randomize);
void inoise8_mover(uint16_t frame, bool randomize);
void rainbow(uint16_t frame, bool randomize);
void confetti(uint16_t frame, bool randomize);
void sinelon(uint16_t frame, bool randomize);
void bpm(uint16_t frame, bool randomize);
void juggle(uint16_t frame, bool randomize);
void pride(uint16_t frame, bool randomize);
void classic_fire(uint16_t frame, bool randomize);
void bouncing_balls(uint16_t frame, bool randomize);
void breath(uint16_t frame, bool randomize);
void pacifica(uint16_t frame, bool randomize);
void twinkles(uint16_t frame, bool randomize);
void ripple(uint16_t frame, bool randomize);
void stripes(uint16_t frame, bool randomize);

void add_glitter(fract8 change_of_glitter);

struct pattern_definition
{
    String name;
    bool auto_cycle;
    void (*pattern)(uint16_t frame, bool randomize);
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
    {"liquid", true, liquid},
    {"bounce", true, bouncing_balls},
    {"ripple", true, ripple},
    {"noise", true, inoise8_mover},
    {"stripes", true, stripes},
    {"fill", false, solid_fill},
    {"none", false, no_pattern},
} PROGMEM;

TBlendType get_current_blending()
{
    return LINEARBLEND;
}

CRGBPalette16 get_current_palette()
{
    return gCurrentPalette;
}

CRGB color_at_pixel(uint8_t pixel_number)
{
    uint8_t color_index = map(pixel_number, 0, EFFECT_LEDS - 1, 0, 255);
    return ColorFromPalette(get_current_palette(), color_index, 255, get_current_blending());
}

///////////////////////////////////////////////////////////////////////////////
//
// No pattern
//
///////////////////////////////////////////////////////////////////////////////
void no_pattern(uint16_t frame, bool randomize)
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
void auto_pattern(uint16_t frame, bool randomize)
{
    static uint8_t prev_pattern_number = 0;
    gPatterns[auto_pattern_number].pattern(frame, prev_pattern_number != auto_pattern_number || randomize);
    prev_pattern_number = auto_pattern_number;
}

///////////////////////////////////////////////////////////////////////////////
//
// solid palette fill
//
///////////////////////////////////////////////////////////////////////////////
void solid_fill(uint16_t frame, bool randomize)
{
    for (int pixel_number = 0; pixel_number < EFFECT_LEDS; pixel_number++)
    {
        effect_leds[translate[pixel_number]] = color_at_pixel(pixel_number);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// liquid
//
///////////////////////////////////////////////////////////////////////////////
void liquid(uint16_t frame, bool randomize)
{
    static uint8_t randomize_index = 0;
    static uint8_t direction = 0;

    if (randomize)
    {
        randomize_index = random16(0, 255);
        direction = random16(100);
    }

    for (int pixel_number = 0; pixel_number < EFFECT_LEDS; pixel_number++)
    {
        uint8_t color_index = map(pixel_number, 0, EFFECT_LEDS - 1, 0, 255);
        CRGB color = ColorFromPalette(get_current_palette(), color_index + randomize_index, 255, get_current_blending());
        if (direction % 2)
        {
            nblend(effect_leds[translate[pixel_number]], color, 20);
        }
        else
        {
            nblend(effect_leds[translate[abs(EFFECT_LEDS - pixel_number - 1)]], color, 20);
        }
    }

    if (frame % 8)
    {
        randomize_index += 1;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// inoise8_mover
//
///////////////////////////////////////////////////////////////////////////////
void inoise8_mover(uint16_t frame, bool randomize)
{
    static uint8_t fade_speed = 10;
    static uint8_t count = 3;

    if (randomize)
    {
        fade_speed = random16(FADE_MIN, FADE_MAX);
        count = random16(3, 5);
    }

    fadeToBlackBy(effect_leds, EFFECT_LEDS, fade_speed + 10);

    for (int i = 0; i < count; i++)
    {
        uint8_t location = inoise8(millis() * 8 / 64 + i * 500, millis() * 8 / 64);            // Get a new pixel location from moving noise.
        uint8_t pixel_number = map(location, 50, 192, 0, EFFECT_LEDS - 1) % (EFFECT_LEDS - 1); // Map that to the length of the strand, and ensure we don't go over.
        uint8_t color = map(pixel_number, 0, EFFECT_LEDS - 1, 0, 255);
        effect_leds[translate[pixel_number]] = ColorFromPalette(get_current_palette(), color, 255, get_current_blending()); // Use that value for both the location as well as the palette index colour for the pixel.
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// rainbow
// Fill all leds with FastLED's built-in rainbow generator
//
///////////////////////////////////////////////////////////////////////////////
void rainbow(uint16_t frame, bool randomize)
{
    static uint8_t delta = 7;
    static uint8_t inital_hue = 0;

    if (randomize)
    {
        delta = random16(3, 10);
        inital_hue = random16(255);
    }

    fill_rainbow(effect_leds, EFFECT_LEDS, inital_hue, delta);
}

///////////////////////////////////////////////////////////////////////////////
//
// confetti
// random colored speckles that blink in and fade smoothly
//
///////////////////////////////////////////////////////////////////////////////
void confetti(uint16_t frame, bool randomize)
{
    static uint8_t fade_speed = 10;

    if (randomize)
    {
        fade_speed = random16(FADE_MIN, FADE_MAX);
    }

    fadeToBlackBy(effect_leds, EFFECT_LEDS, fade_speed + 10);

    EVERY_N_MILLISECONDS(random16(10, 100))
    {
        uint8_t pixel_number = random16(EFFECT_LEDS);
        uint8_t brightness = random16(127, 255);
        effect_leds[translate[pixel_number]] = ColorFromPalette(get_current_palette(), random16(255), brightness, get_current_blending());
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// sinelon
// a colored dot sweeping back and forth, with fading trails
//
///////////////////////////////////////////////////////////////////////////////
void sinelon(uint16_t frame, bool randomize)
{
    static uint8_t bpm = 60;
    static uint8_t wave_pattern = 0;
    static uint8_t fade_speed = 10;
    static uint8_t loop_count = 0;
    uint8_t pixel_number = 0;

    if (randomize)
    {
        bpm = random16(40, 80);
        wave_pattern = random16(3);
        fade_speed = random16(FADE_MIN, FADE_MAX);
        loop_count = random16(0, 255);
    }

    fadeToBlackBy(effect_leds, EFFECT_LEDS, fade_speed);

    uint8_t value = map(frame, 0, FRAMES_PER_SECOND, std::numeric_limits<std::uint8_t>::min(), std::numeric_limits<std::uint8_t>::max());
    pixel_number = map(quadwave8(value), std::numeric_limits<std::uint8_t>::min(), std::numeric_limits<std::uint8_t>::max(), 0, 10);

    if (frame == 0)
    {
        loop_count++;
    }

    uint8_t color_index = loop_count + pixel_number;
    CRGB color = ColorFromPalette(get_current_palette(), color_index, 255, get_current_blending());

    effect_leds[translate[pixel_number]] = color;
    effect_leds[translate[pixel_number + 11]] = color;
    effect_leds[translate[pixel_number + 23]] = color;
    effect_leds[translate[pixel_number + 34]] = color;

    add_glitter(5);
}

///////////////////////////////////////////////////////////////////////////////
//
// BPM
// colored stripes pulsing at a defined Beats-Per-Minute (BPM)
//
///////////////////////////////////////////////////////////////////////////////
void bpm(uint16_t frame, bool randomize)
{
    static uint8_t bpm = 10;
    static uint8_t loop_count = 0;

    if (randomize)
    {
        bpm = random16(5, 25);
    }

    uint8_t beat = beatsin8(bpm, 64, 255);
    for (int pixel_number = 0; pixel_number < EFFECT_LEDS; pixel_number++)
    {
        effect_leds[translate[pixel_number]] = ColorFromPalette(get_current_palette(), loop_count + (pixel_number * 2), beat - loop_count + (pixel_number * 10), get_current_blending());
    }

    if (beat == 24)
    {
        loop_count++;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Juggle
// eight colored dots, weaving in and out of sync with each other
//
///////////////////////////////////////////////////////////////////////////////
void juggle(uint16_t frame, bool randomize)
{
    static uint8_t dot_count = 8;
    static uint8_t fade_speed = 10;

    if (randomize)
    {
        fade_speed = random16(FADE_MIN, FADE_MAX);
        dot_count = random16(8, 12);
    }

    fadeToBlackBy(effect_leds, EFFECT_LEDS, fade_speed);

    uint8_t dothue = 0;
    for (int i = 0; i < dot_count; i++)
    {
        uint8_t pixel_number = beatsin16(i + dot_count, 0, EFFECT_LEDS - 1);
        effect_leds[translate[pixel_number]] = ColorFromPalette(get_current_palette(), dothue, 255, get_current_blending());
        dothue += 32;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Pride
// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
//
///////////////////////////////////////////////////////////////////////////////
void pride(uint16_t frame, bool randomize)
{
    static uint16_t sPseudotime = 0;
    static uint16_t sLastMillis = 0;
    static uint16_t sHue16 = 0;
    static uint8_t loop_count = 0;

    uint8_t sat8 = beatsin88(87, 220, 250);
    uint8_t brightdepth = beatsin88(341, 96, 224);
    uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
    uint8_t msmultiplier = beatsin88(147, 23, 60);

    uint16_t hue16 = sHue16; // loop_count * 256;
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

        uint16_t pixel_number = i;
        pixel_number = (EFFECT_LEDS - 1) - pixel_number;

        nblend(effect_leds[translate[pixel_number]], newcolor, 64);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Wrapper to execute fire.h
//
///////////////////////////////////////////////////////////////////////////////
void classic_fire(uint16_t frame, bool randomize)
{
    gCurrentPalette = *(gPalettes[10].palette);

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
void bouncing_balls(uint16_t frame, bool randomize)
{
    EVERY_N_MILLISECONDS(30)
    {
        fadeToBlackBy(effect_leds, EFFECT_LEDS, 35);
        bouncer.Draw();
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Breath
// https://github.com/marmilicious/FastLED_examples/blob/master/breath_effect_v2.ino
//
///////////////////////////////////////////////////////////////////////////////
void breath(uint16_t frame, bool randomize)
{
    static uint8_t bpm = random16(12, 20);
    static uint8_t color_index = 0;

    if (randomize)
    {
        bpm = random16(12, 20);
        color_index = random16(255);
    }

    uint8_t brightness = beatsin8(bpm, 80);
    CRGB color = ColorFromPalette(get_current_palette(), color_index, dim8_video(brightness), get_current_blending());
    for (int pixel_number = 0; pixel_number < EFFECT_LEDS; pixel_number++)
    {
        nblend(effect_leds[pixel_number], color, 15);
    }

    if (!frame)
    {
        color_index++;
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
// the best randomizeing point for learning.  Nevertheless, it does illustrate some
// useful techniques.
//
// In this animation, there are four "layers" of waves of light.
//
// Each layer moves independently, and each is scaled separately.
//
// All four wave layers are added together on top of each other, and then
// another filter is applied that adds "whitecaps" of brightness where the
// waves line up with each other more.  Finally, another pass is taken
// over the led color_indexesay to 'deepen' (dim) the blues and greens.
//
// The fade_speed and scale and motion each layer varies slowly within independent
// hand-chosen ranges, which is why the code has a lot of low-fade_speed 'beatsin8' functions
// with a lot of oddly specific numeric ranges.
//
// These three custom blue-green color palettes were inspired by the colors found in
// the waters off the southern coast of California, https://goo.gl/maps/QQgd97jjHesHZVxQ7
//

// Add one layer of waves into the led color_indexesay
void pacifica_one_layer(CRGBPalette16 &p, uint16_t cirandomize, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
    uint16_t ci = cirandomize;
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

void pacifica(uint16_t frame, bool randomize)
{
    // Increment the four "color index randomize" counters, one for each wave layer.
    // Each is incremented at a different fade_speed, and the fade_speeds vary over time.
    static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
    static uint32_t sLastms = 0;
    uint32_t ms = GET_MILLIS();
    uint32_t deltams = ms - sLastms;
    sLastms = ms;
    uint16_t fade_speedfactor1 = beatsin16(3, 179, 269);
    uint16_t fade_speedfactor2 = beatsin16(4, 179, 269);
    uint32_t deltams1 = (deltams * fade_speedfactor1) / 256;
    uint32_t deltams2 = (deltams * fade_speedfactor2) / 256;
    uint32_t deltams21 = (deltams1 + deltams2) / 2;
    sCIStart1 += (deltams1 * beatsin88(1011, 10, 13));
    sCIStart2 -= (deltams21 * beatsin88(777, 8, 11));
    sCIStart3 -= (deltams1 * beatsin88(501, 5, 7));
    sCIStart4 -= (deltams2 * beatsin88(257, 4, 6));

    // Clear out the LED color_indexesay to a dim background blue-green
    fill_solid(effect_leds, EFFECT_LEDS, CRGB(2, 6, 10));

    // Render each of four layers, with different scales and fade_speeds, that vary over time
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
    // Overall twinkle fade_speed.
    // 0 (VERY slow) to 8 (VERY fast).
    // 4, 5, and 6 are recommended, default is 4.
    static uint8_t twinkle_fade_speed = 3;

    // Overall twinkle density.
    // 0 (NONE lit) to 8 (ALL lit at once).
    // Default is 5.
    static uint8_t twinkle_density = 4;

    // If set to 1, colors will fade out slighted 'reddened',
    // similar to how incandescent bulbs change color
    // as they get dim down.
    // TODO: set this to 1 in december and 0 other times
    static bool incandescent = 1;

    uint16_t ticks = ms >> (8 - twinkle_fade_speed);
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
        c = ColorFromPalette(get_current_palette(), hue, bright, NOBLEND);
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

void twinkles(uint16_t frame, bool randomize)
{
    // "PRNG16" is the pseudorandom number generator
    // It MUST be reset to the same randomizeing value each time
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
        // use that number as clock fade_speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
        uint8_t myfade_speedmultiplierQ5_3 = ((((PRNG16 & 0xFF) >> 4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
        uint32_t myclock30 = (uint32_t)((clock32 * myfade_speedmultiplierQ5_3) >> 3) + myclockoffset16;
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

void do_ripple(bool randomize)
{
    static int color = random(256);
    static int center = random(EFFECT_LEDS);
    static int step = -1;
    static int max_steps = random(5, 22);
    static float fade_rate = 0.8;
    static int diff;

    // background color
    static uint8_t current_background = random(255);
    static uint8_t next_background = current_background;

    if (randomize)
    {
        color = random16(256);
        center = random16(EFFECT_LEDS);
        max_steps = random(5, 22);
    }

    if (current_background == next_background)
    {
        next_background = random(255);
    }
    else if (next_background > current_background)
    {
        current_background++;
    }
    else
    {
        current_background--;
    }

    for (uint8_t pixel_number = 0; pixel_number < EFFECT_LEDS; pixel_number++)
    {
        nblend(effect_leds[translate[pixel_number]], ColorFromPalette(gCurrentPalette, current_background, 255, get_current_blending()), 45);
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
        if (step < max_steps)
        {
            if (step > 3)
            {
                uint8_t pixel_number = wrap(center + step - 3);
                uint8_t mirror_pixel_number = wrap(center - step + 3);
                uint8_t brightness = pow(fade_rate, step - 2) * 255;
                effect_leds[translate[pixel_number]] = ColorFromPalette(gCurrentPalette, color, brightness, get_current_blending());
                effect_leds[translate[mirror_pixel_number]] = ColorFromPalette(gCurrentPalette, color, brightness, get_current_blending());
            }
            else
            {
                uint8_t pixel_number = wrap(center + step);
                uint8_t mirror_pixel_number = wrap(center - step);
                uint8_t brightness = pow(fade_rate, step) * 255;
                effect_leds[translate[pixel_number]] = ColorFromPalette(gCurrentPalette, color, brightness, get_current_blending());
                effect_leds[translate[mirror_pixel_number]] = ColorFromPalette(gCurrentPalette, color, brightness, get_current_blending());
            }

            step++;
        }
        else
        {
            step = -1;
        }
    }
}

void ripple(uint16_t frame, bool randomize)
{
    EVERY_N_MILLISECONDS(30)
    {
        do_ripple(randomize);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Stripes
// Draw alternating bands of color, 2 or 3 colors.
// When using three colors, color1 is used between the other two colors.
//
///////////////////////////////////////////////////////////////////////////////
void stripes(uint16_t frame, bool randomize)
{
    static uint8_t color_index_1 = random16(0, 127);
    static uint8_t color_index_2 = random16(128, 255);
    static uint8_t stripe_length = random16(4, 8);
    static uint8_t color_indexes[EFFECT_LEDS] = {0};
    static uint8_t speed = random(70, 100);
    static uint8_t direction = 0;

    if (randomize)
    {
        stripe_length = random16(4, 7);
        color_index_1 = random16(0, 255);
        color_index_2 = random16(0, 255);
        speed = random(70, 100);
        direction = random(100);

        for (uint8_t pixel_number = 0; pixel_number < EFFECT_LEDS; pixel_number++)
        {
            if ((pixel_number % (2 * stripe_length)) < stripe_length)
            {
                color_indexes[pixel_number] = color_index_2;
            }
            else
            {
                color_indexes[pixel_number] = color_index_1;
            }
        }
    }

    EVERY_N_MILLISECONDS(speed)
    {
        uint8_t last = color_indexes[0];
        for (uint8_t pixel_number = 0; pixel_number < EFFECT_LEDS - 1; pixel_number++)
        {
            color_indexes[pixel_number] = color_indexes[pixel_number + 1];
        }
        color_indexes[EFFECT_LEDS - 1] = last;
    }

    for (uint8_t pixel_number = 0; pixel_number < EFFECT_LEDS; pixel_number++)
    {
        if (direction % 2)
        {
            nblend(effect_leds[translate[pixel_number]], ColorFromPalette(get_current_palette(), color_indexes[pixel_number], 255, get_current_blending()), 45);
        }
        else
        {
            nblend(effect_leds[translate[abs(EFFECT_LEDS - pixel_number - 1)]], ColorFromPalette(get_current_palette(), color_indexes[pixel_number], 255, get_current_blending()), 45);
        }
    }
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
