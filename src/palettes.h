#pragma once
#define FASTLED_INTERNAL
#include <FastLED.h>

uint8_t current_palette_number = 0;
uint8_t auto_palette_number = 0;
uint8_t manual_palette_number = 1;

// Gradient palette "bhw1_04_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_04.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE(bhw1_04_gp){
    0, 229, 227, 1,
    15, 227, 101, 3,
    142, 40, 1, 80,
    198, 17, 1, 79,
    255, 0, 0, 45};

// Gradient palette "bhw1_28_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_28.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 32 bytes of program space.

DEFINE_GRADIENT_PALETTE(bhw1_28_gp){
    0, 75, 1, 221,
    30, 252, 73, 255,
    48, 169, 0, 242,
    119, 0, 149, 242,
    170, 43, 0, 242,
    206, 252, 73, 255,
    232, 78, 12, 214,
    255, 0, 149, 242};

// Gradient palette "bhw1_26_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_26.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 52 bytes of program space.

DEFINE_GRADIENT_PALETTE(bhw1_26_gp){
    0, 107, 1, 205,
    35, 255, 255, 255,
    73, 107, 1, 205,
    107, 10, 149, 210,
    130, 255, 255, 255,
    153, 10, 149, 210,
    170, 27, 175, 119,
    198, 53, 203, 56,
    207, 132, 229, 135,
    219, 255, 255, 255,
    231, 132, 229, 135,
    252, 53, 203, 56,
    255, 53, 203, 56};

// Gradient palette "bhw4_063_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw4/tn/bhw4_063.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 76 bytes of program space.

DEFINE_GRADIENT_PALETTE(bhw4_063_gp){
    0, 8, 3, 1,
    20, 50, 17, 1,
    35, 19, 13, 5,
    48, 242, 115, 9,
    61, 252, 184, 17,
    76, 252, 114, 9,
    89, 75, 24, 7,
    99, 252, 195, 14,
    117, 75, 24, 7,
    130, 210, 77, 6,
    140, 103, 33, 3,
    153, 10, 9, 9,
    168, 252, 213, 21,
    186, 18, 6, 1,
    196, 50, 17, 1,
    209, 6, 4, 2,
    224, 91, 87, 72,
    242, 17, 9, 3,
    255, 4, 1, 12};

// Gradient palette "bhw4_008_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw4/tn/bhw4_008.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 36 bytes of program space.

DEFINE_GRADIENT_PALETTE(bhw4_008_gp){
    0, 252, 252, 172,
    25, 239, 255, 61,
    53, 247, 45, 17,
    76, 197, 82, 19,
    96, 239, 255, 61,
    124, 83, 4, 1,
    153, 247, 45, 17,
    214, 23, 15, 17,
    255, 1, 1, 1};

// Gradient palette "bhw3_61_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw3/tn/bhw3_61.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

// Gradient palette "bhw3_62_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw3/tn/bhw3_62.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE(bhw3_62_gp){
    0, 255, 255, 45,
    43, 208, 93, 1,
    137, 224, 1, 242,
    181, 159, 1, 29,
    255, 63, 4, 68};

DEFINE_GRADIENT_PALETTE(tropicalPalette){
    0, 255, 230, 0,   /* at index 0, yellow(0,0,0) */
    50, 255, 200, 0,  /* at index 50, orange(0,0,0) */
    100, 0, 255, 255, /* at index 100, teal(255,0,0) */
    200, 0, 255, 100, /* at index 200, blue(255,255,255) */
    255, 255, 190, 0  /* at index 255, yellow(0,0,0) */
};

DEFINE_GRADIENT_PALETTE(raggaPalette){
    0, 255, 255, 0,   /* at index 0, yellow(0,0,0) */
    100, 100, 255, 0, /* at index 100, greenyellow(0,0,0) */
    150, 0, 255, 0,   /* at index 150, green(255,0,0) */
    200, 255, 0, 0,   /* at index 200, red(255,255,255) */
    255, 255, 255, 0  /* at index 255, yellow(0,0,0) */
};

DEFINE_GRADIENT_PALETTE(transPalette){
    0, 50, 230, 255,    /* at index 0, Blue(0,0,0) */
    50, 255, 50, 50,    /* at index 50, Pink(0,0,0) */
    125, 255, 255, 255, /* at index 192, White(255,0,0) */
    200, 255, 50, 50,   /* at index 200, Pink(255,255,255) */
    255, 50, 190, 255   /* at index 255, Blue(0,0,0) */
};

DEFINE_GRADIENT_PALETTE(biPalette){
    0, 0, 255, 255,   /* at index 0, teal(0,0,0) */
    50, 0, 0, 255,    /* at index 0, blue(0,0,0) */
    190, 255, 0, 255, /* at index 255, purple(255,255,255) */
    255, 255, 0, 100  /* at index 0, deep purple(0,0,0) */
};

DEFINE_GRADIENT_PALETTE(RedGrayGreenPalette){
    0, 255, 0, 0,       // Red
    152, 255, 0, 0,     // Red
    153, 128, 128, 128, // Gray
    178, 128, 128, 128, // Gray
    179, 0, 128, 0,     // Green
    255, 0, 128, 0      // Green
};

// Add or remove palette names from this list to control which color
// palettes are used, and in what order.
const TProgmemRGBGradientPalettePtr gActivePaletteList[] = {
    bhw1_04_gp,
    bhw1_28_gp,
    bhw1_26_gp,
    bhw4_008_gp,
    bhw3_62_gp,
    tropicalPalette,
    raggaPalette,
    biPalette};

const TProgmemRGBPalette16 RedWhiteBlue_p FL_PROGMEM =
    {CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray,
     CRGB::Blue, CRGB::Blue, CRGB::Red, CRGB::Red,
     CRGB::Gray, CRGB::Gray, CRGB::Blue, CRGB::Blue,
     CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray};

// A mostly red palette with green accents and white trim.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedGreenWhite_p FL_PROGMEM =
    {CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
     CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray,
     CRGB::Gray, CRGB::Gray, CRGB::Green, CRGB::Green,
     CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green};

// A mostly (dark) green palette with red berries.
#define Holly_Green 0x00580c
#define Holly_Red 0xB00402
const TProgmemRGBPalette16 Holly_p FL_PROGMEM =
    {Holly_Green, Holly_Green, Holly_Green, Holly_Green,
     Holly_Green, Holly_Green, Holly_Green, Holly_Green,
     Holly_Green, Holly_Green, Holly_Green, Holly_Green,
     Holly_Green, Holly_Green, Holly_Green, Holly_Red};

// A red and white striped palette
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedWhite_p FL_PROGMEM =
    {CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
     CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray,
     CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray,
     CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red};

// A mostly blue palette with white accents.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM =
    {CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
     CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
     CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
     CRGB::Blue, CRGB::Gray, CRGB::Gray, CRGB::Gray};

// A pure "fairy light" palette with some brightness variations
#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)
const TProgmemRGBPalette16 FairyLight_p FL_PROGMEM =
    {CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight,
     HALFFAIRY, HALFFAIRY, CRGB::FairyLight, CRGB::FairyLight,
     QUARTERFAIRY, QUARTERFAIRY, CRGB::FairyLight, CRGB::FairyLight,
     CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight};

// A palette of soft snowflakes with the occasional bright one
const TProgmemRGBPalette16 Snow_p FL_PROGMEM =
    {0x304048, 0x304048, 0x304048, 0x304048,
     0x304048, 0x304048, 0x304048, 0x304048,
     0x304048, 0x304048, 0x304048, 0x304048,
     0x304048, 0x304048, 0x304048, 0xE0F0FF};

// A palette reminiscent of large 'old-school' C9-size tree lights
// in the five classic colors: red, orange, green, blue, and white.
#define C9_Red 0xB80400
#define C9_Orange 0x902C02
#define C9_Green 0x046002
#define C9_Blue 0x070758
#define C9_White 0x606820
const TProgmemRGBPalette16 RetroC9_p FL_PROGMEM =
    {C9_Red, C9_Orange, C9_Red, C9_Orange,
     C9_Orange, C9_Red, C9_Orange, C9_Red,
     C9_Green, C9_Green, C9_Green, C9_Green,
     C9_Blue, C9_Blue, C9_Blue, C9_White};

// A cold, icy pale blue palette
#define Ice_Blue1 0x0C1040
#define Ice_Blue2 0x182080
#define Ice_Blue3 0x5080C0
const TProgmemRGBPalette16 Ice_p FL_PROGMEM =
    {
        Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
        Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
        Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
        Ice_Blue2, Ice_Blue2, Ice_Blue2, Ice_Blue3};

const TProgmemRGBPalette16 PastelSunset_p FL_PROGMEM = {
    CRGB::Indigo,       // Deep night sky
    CRGB::DarkCyan,     // Calm ocean
    CRGB::Teal,         // Tropical waters
    CRGB::ForestGreen,  // Dense forest
    CRGB::GreenYellow,  // Fresh spring leaves
    CRGB::Gold,         // Sunrise or sunset
    CRGB::OrangeRed,    // Bright sunset
    CRGB::Red,          // Intense sunset
    CRGB::DeepPink,     // Blooming flowers
    CRGB::HotPink,      // Bright flowers
    CRGB::MediumPurple, // Early twilight
    CRGB::Navy,         // Deep water
    CRGB::DarkBlue,     // Night sky
    CRGB::MidnightBlue, // Just before total darkness
    CRGB::Black,        // Midnight
    CRGB::White         // Bright daylight
};

const TProgmemRGBPalette16 DramaticHorizon_p FL_PROGMEM = {
    CRGB::Crimson,        // Rich, deep red
    CRGB::Coral,          // Soft, lively orange
    CRGB::PeachPuff,      // Light, creamy orange
    CRGB::LemonChiffon,   // Bright, cheerful yellow
    CRGB::PaleGreen,      // Fresh, spring green
    CRGB::MediumSeaGreen, // Vibrant, medium green
    CRGB::LightSeaGreen,  // Muted, sea-green blue
    CRGB::CadetBlue,      // Cool, calm blue
    CRGB::RoyalBlue,      // Deep, royal blue
    CRGB::BlueViolet,     // Mysterious, medium purple
    CRGB::Orchid,         // Exotic, vibrant purple
    CRGB::Thistle,        // Gentle, pastel purple
    CRGB::LavenderBlush,  // Soft, light pink
    CRGB::MistyRose,      // Warm, pinkish beige
    CRGB::Linen,          // Natural, neutral beige
    CRGB::Ivory           // Creamy, off-white color
};

const TProgmemRGBPalette16 EmeraldDrama_p FL_PROGMEM = {
    CRGB::Black,             // Deep black
    CRGB::DarkGreen,         // Very dark green
    CRGB::Green,             // Pure green
    CRGB::LimeGreen,         // Bright, vibrant green
    CRGB::MediumSpringGreen, // Light, spring green
    CRGB::LightGreen,        // Very light green
    CRGB::YellowGreen,       // Green with a hint of yellow
    CRGB::LawnGreen,         // Bright, fresh green
    CRGB::MediumPurple,      // Muted purple
    CRGB::BlueViolet,        // Blue-leaning purple
    CRGB::Purple,            // Pure purple
    CRGB::DeepPink,          // Pinkish purple
    CRGB::Red,               // Pure red
    CRGB::DarkRed,           // Dark red
    CRGB::Maroon,            // Very dark red
    CRGB::Black              // Back to black for a smooth loop
};

const TProgmemRGBPalette16 GoldenDrama_p FL_PROGMEM = {
    CRGB::DarkGoldenrod, // Very dark gold
    CRGB::Goldenrod,     // Rich gold
    CRGB::Gold,          // Pure gold
    CRGB::Yellow,        // Pure yellow
    CRGB::LightYellow,   // Light, bright yellow
    CRGB::LemonChiffon,  // Pale, creamy yellow
    CRGB::Ivory,         // Off-white yellow
    CRGB::White,         // Pure white
    CRGB::MediumPurple,  // Muted purple
    CRGB::BlueViolet,    // Blue-leaning purple
    CRGB::Indigo,        // Deep indigo
    CRGB::DarkMagenta,   // Dark magenta
    CRGB::DeepPink,      // Bright, deep pink
    CRGB::Crimson,       // Rich, dark red
    CRGB::Maroon,        // Very dark red
    CRGB::White          // Back to white for a smooth loop
};

const TProgmemRGBPalette16 AquaHarmony_p FL_PROGMEM = {
    CRGB::Teal,             // Deep teal
    CRGB::DarkCyan,         // Dark cyan
    CRGB::Cyan,             // Pure cyan
    CRGB::LightCyan,        // Light cyan
    CRGB::Aquamarine,       // Pale cyan-green
    CRGB::MediumAquamarine, // Medium cyan-green
    CRGB::MediumTurquoise,  // Medium cyan-blue
    CRGB::Turquoise,        // Cyan-blue
    CRGB::LightSeaGreen,    // Light sea green
    CRGB::SeaGreen,         // Sea green
    CRGB::MediumSeaGreen,   // Medium sea green
    CRGB::DarkSeaGreen,     // Dark sea green
    CRGB::LightGreen,       // Light green
    CRGB::LimeGreen,        // Lime green
    CRGB::Lime,             // Bright lime
    CRGB::Yellow            // Yellow as a vibrant end point
};

CRGBPalette16 pacifica_palette_1 =
    {0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
     0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50};

CRGBPalette16 pacifica_palette_2 =
    {0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
     0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F};

CRGBPalette16 pacifica_palette_3 =
    {0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33,
     0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF};

struct palette_definition
{
    String name;
    bool auto_cycle;
    TBlendType blend_type;
    const TProgmemRGBPalette16 *palette;
    const TProgmemRGBGradientPalettePtr gradient;
};

palette_definition gPalettes[] = {
    {"cycle", false},
    {"usa", true, LINEARBLEND, &RedWhiteBlue_p},
    {"retro", true, LINEARBLEND, &RetroC9_p},
    {"party", true, LINEARBLEND, &PartyColors_p},
    {"snow", false, LINEARBLEND, &Snow_p},
    {"holly", false, LINEARBLEND, &Holly_p},
    {"ice", false, LINEARBLEND, &Ice_p},
    {"clouds", true, LINEARBLEND, &CloudColors_p},
    {"lava", true, LINEARBLEND, &LavaColors_p},
    {"forest", true, LINEARBLEND, &ForestColors_p},
    {"heat", true, LINEARBLEND, &HeatColors_p},
    {"ocean", true, LINEARBLEND, &OceanColors_p},
    {"nature", true, LINEARBLEND, &PastelSunset_p},
    {"paste", true, LINEARBLEND, &DramaticHorizon_p},
    {"emerald", true, LINEARBLEND, &EmeraldDrama_p},
    {"golden", true, LINEARBLEND, &GoldenDrama_p},
    {"aqua", true, LINEARBLEND, &AquaHarmony_p},
    {"rainbow", true, LINEARBLEND, &RainbowColors_p},
    {"red / white", true, LINEARBLEND, &RedWhite_p},
    {"blue / white", false, LINEARBLEND, &BlueWhite_p},
    {"rgw", true, LINEARBLEND, &RedGreenWhite_p},
    {"white lights", false, LINEARBLEND, &FairyLight_p},
    // {"rainbow 1", true, LINEARBLEND, &RainbowStripeColors_p},
};