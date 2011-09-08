#pragma once

unsigned char fontData[] = {

    0x7C, 0x44, 0x44, 0x7C, 0xAA,       // NULL
    0x00, 0x00, 0xAA,                   // ' '
    0x5F, 0xAA,                         // !
    0x06, 0x00, 0x06, 0xAA,             // "
    0x34, 0x4A, 0x54, 0x20, 0x50, 0xAA, // &
    0x06, 0xAA,                         // '
    0x7C, 0x82, 0xAA,                   // (
    0x82, 0x7C, 0xAA,                   // )
    0x10, 0xAA,                         // *
    0x10, 0x38, 0x10, 0xAA,             // +
    0x80, 0x40, 0xAA,                   // ,
    0x10, 0x10, 0x10, 0xAA,             // -
    0x40, 0xAA,                         // .
    0x40, 0x20, 0x10, 0x08, 0xAA,       // /

    0x3E, 0x41, 0x41, 0x3E, 0xAA,       // 0
    0x42, 0x7F, 0x40, 0xAA,             // 1
    0x62, 0x51, 0x49, 0x46, 0xAA,       // 2
    0x22, 0x41, 0x49, 0x36, 0xAA,       // 3
    0x18, 0x14, 0x12, 0x7F, 0xAA,       // 4
    0x4F, 0x49, 0x49, 0x31, 0xAA,       // 5
    0x3E, 0x49, 0x49, 0x30, 0xAA,       // 6
    0x03, 0x71, 0x09, 0x07, 0xAA,       // 7
    0x36, 0x49, 0x49, 0x36, 0xAA,       // 8
    0x06, 0x49, 0x49, 0x3E, 0xAA,       // 9

    0x48, 0xAA,                         // :
    0x80, 0x48, 0xAA,                   // ;
    0x10, 0x28, 0x44, 0xAA,             // <
    0x28, 0x28, 0x28, 0xAA,             // =
    0x44, 0x28, 0x10, 0xAA,             // >
    0x04, 0x52, 0x0C, 0xAA,             // ?

    0x78, 0x14, 0x12, 0x11, 0x7F, 0xAA, // A
    0x7F, 0x49, 0x49, 0x49, 0x36, 0xAA, // B
    0x3E, 0x41, 0x41, 0x41, 0x22, 0xAA, // C
    0x7F, 0x41, 0x41, 0x41, 0x3E, 0xAA, // D
    0x7F, 0x49, 0x49, 0x49, 0x41, 0xAA, // E
    0x7F, 0x09, 0x09, 0x09, 0x01, 0xAA, // F
    0x3E, 0x41, 0x41, 0x51, 0x32, 0xAA, // G
    0x7F, 0x08, 0x08, 0x08, 0x7F, 0xAA, // H
    0x41, 0x7F, 0x41, 0xAA,             // I
    0x23, 0x41, 0x41, 0x3F, 0xAA,       // J
    0x7F, 0x08, 0x14, 0x22, 0x41, 0xAA, // K
    0x7F, 0x40, 0x40, 0x40, 0x60, 0xAA, // L
    0x7F, 0x02, 0x0C, 0x02, 0x7F, 0xAA, // M
    0x7F, 0x04, 0x08, 0x10, 0x7F, 0xAA, // N
    0x3E, 0x41, 0x41, 0x41, 0x3E, 0xAA, // O
    0x7F, 0x11, 0x11, 0x11, 0x0E, 0xAA, // P
    0x3E, 0x41, 0x61, 0x41, 0xBE, 0xAA, // Q
    0x7F, 0x09, 0x19, 0x29, 0x46, 0xAA, // R
    0x26, 0x49, 0x49, 0x49, 0x32, 0xAA, // S
    0x03, 0x01, 0x7F, 0x01, 0x03, 0xAA, // T
    0x3F, 0x40, 0x40, 0x40, 0x3F, 0xAA, // U
    0x1F, 0x20, 0x40, 0x20, 0x1F, 0xAA, // V
    0x07, 0x18, 0x60, 0x18, 0x60, 0x18, 0x07, 0xAA,// W
    0x63, 0x14, 0x08, 0x14, 0x63, 0xAA, // X
    0x07, 0x48, 0x48, 0x28, 0x1F, 0xAA, // Y
    0x61, 0x51, 0x49, 0x45, 0x43, 0xAA, // Z

    0xFE, 0x82, 0xAA,                   // [
    0x08, 0x10, 0x20, 0x40, 0xAA,       // 
    0x82, 0xFE, 0xAA,                   // ]
    0x04, 0x02, 0x04, 0xAA,             // ^
    0x40, 0x40, 0x40, 0xAA,             // _

    0x20, 0x54, 0x54, 0x38, 0x40, 0xAA, // a
    0x7F, 0x48, 0x44, 0x38, 0xAA,       // b
    0x38, 0x44, 0x44, 0x28, 0xAA,       // c
    0x38, 0x44, 0x48, 0x7F, 0xAA,       // d
    0x38, 0x54, 0x54, 0x18, 0xAA,       // e
    0x04, 0x7E, 0x05, 0xAA,             // f
    0x18, 0xA4, 0xA4, 0x7C, 0xAA,       // g
    0x7F, 0x08, 0x04, 0x78, 0xAA,       // h
    0x44, 0x7D, 0x40, 0xAA,             // i
    0x84, 0x7D, 0xAA,                   // j
    0x7F, 0x10, 0x28, 0x44, 0xAA,       // k
    0x41, 0x7F, 0x40, 0xAA,             // l
    0x7C, 0x04, 0x78, 0x04, 0x78, 0xAA, // m
    0x7C, 0x08, 0x04, 0x78, 0xAA,       // n
    0x38, 0x44, 0x44, 0x38, 0xAA,       // o
    0xFC, 0x24, 0x24, 0x18, 0xAA,       // p
    0x18, 0x24, 0x24, 0xFC, 0xAA,       // q
    0x7C, 0x08, 0x04, 0xAA,             // r
    0x48, 0x54, 0x54, 0x20, 0xAA,       // s
    0x3E, 0x44, 0xAA,                   // t
    0x3C, 0x40, 0x40, 0x3C, 0x40, 0xAA, // u
    0x1C, 0x20, 0x40, 0x20, 0x1C, 0xAA, // v
    0x1C, 0x60, 0x18, 0x60, 0x1C, 0xAA, // w
    0x44, 0x28, 0x10, 0x28, 0x44, 0xAA, // x
    0x1C, 0xA0, 0xA0, 0x7C, 0xAA,       // y
    0x64, 0x54, 0x54, 0x4C, 0xAA,       // z
    
    0x7F, 0x45, 0x45, 0x45, 0x39, 0xAA, // �
    0x7F, 0x01, 0x01, 0x01, 0x03, 0xAA, // �
    0xC0, 0x7C, 0x42, 0x41, 0x7F, 0xC0, 0xAA,   // �
    0x63, 0x14, 0x08, 0x7F, 0x08, 0x14, 0x63, 0xAA, // �
    0x22, 0x41, 0x49, 0x49, 0x36, 0xAA, // �
    0x7F, 0x20, 0x10, 0x08, 0x7F, 0xAA, // �
    0x7F, 0x20, 0x11, 0x08, 0x7F, 0xAA, // �
    0x78, 0x04, 0x02, 0x01, 0x7F, 0xAA, // �
    0x7F, 0x01, 0x01, 0x01, 0x7F, 0xAA, // �
    0x1C, 0x22, 0x7F, 0x22, 0x1C, 0xAA, // �
    0x7F, 0x40, 0x40, 0x40, 0x7F, 0xC0, 0xAA, // �
    0x07, 0x08, 0x08, 0x08, 0x7F, 0xAA, // �
    0x7F, 0x40, 0x78, 0x40, 0x7F, 0xAA, // �
    0x7F, 0x40, 0x78, 0x40, 0x7F, 0xC0, 0xAA, // �
    0x01, 0x7F, 0x48, 0x48, 0x48, 0x30, 0xAA, // �
    0x7F, 0x48, 0x48, 0x30, 0x00, 0x7F, 0xAA, // �
    0x7F, 0x48, 0x48, 0x48, 0x30, 0xAA, // �
    0x22, 0x41, 0x49, 0x49, 0x3E, 0xAA, // �
    0x7F, 0x08, 0x3E, 0x41, 0x41, 0x3E, 0xAA, // �
    0x46, 0x29, 0x19, 0x09, 0x7F, 0xAA, // �

    0x3A, 0x45, 0x45, 0x39, 0xAA,       // �
    0x3F, 0x4D, 0x4A, 0x30, 0xAA,       // �
    0x7C, 0x04, 0x04, 0x0C, 0xAA,       // �
    0xC0, 0x78, 0x44, 0x7C, 0xC0, 0xAA, // �
    0x6C, 0x10, 0x7C, 0x10, 0x6C, 0xAA, // �
    0x28, 0x44, 0x54, 0x28, 0xAA,       // �
    0x7C, 0x20, 0x10, 0x7C, 0xAA,       // �
    0x7C, 0x21, 0x11, 0x7C, 0xAA,       // �
    0x7C, 0x10, 0x28, 0x44, 0xAA,       // �
    0x70, 0x08, 0x04, 0x7C, 0xAA,       // �
    0x7C, 0x08, 0x10, 0x08, 0x7C, 0xAA, // �
    0x7C, 0x10, 0x10, 0x7C, 0xAA,       // �
    0x7C, 0x04, 0x04, 0x7C, 0xAA,       // �
    0x0C, 0x04, 0x7C, 0x04, 0x0C, 0xAA, // �
    0x30, 0x48, 0xFC, 0x48, 0x30, 0xAA, // �
    0x7C, 0x40, 0x40, 0x7C, 0xC0, 0xAA, // �
    0x1C, 0x20, 0x20, 0x7C, 0xAA,       // �
    0x7C, 0x40, 0x70, 0x40, 0x7C, 0xAA, // �
    0x7C, 0x40, 0x70, 0x40, 0x7C, 0xC0, 0xAA, // �
    0x04, 0x7C, 0x50, 0x50, 0x20, 0xAA, // �
    0x7C, 0x50, 0x20, 0x00, 0x7C, 0xAA, // �
    0x7C, 0x50, 0x50, 0x20, 0xAA,       // �
    0x28, 0x44, 0x54, 0x38, 0xAA,       // �
    0x7C, 0x10, 0x38, 0x44, 0x38, 0xAA, // �
    0x48, 0x34, 0x14, 0x7C, 0xAA,       // �
};

unsigned int fontMap[256];

void InitFontMap()
{
    for(int i = 0; i < 256; i++)
        fontMap[i] = 0;
    
    fontMap[(BYTE)' '] = 5;
    fontMap[(BYTE)'!'] = fontMap[(BYTE)' '] + 3;
    fontMap[(BYTE)'"'] = fontMap[(BYTE)'!'] + 2;
    fontMap[(BYTE)'&'] = fontMap[(BYTE)'"'] + 4;
    fontMap[(BYTE)'\''] = fontMap[(BYTE)'&'] + 6;
    fontMap[(BYTE)'('] = fontMap[(BYTE)'\''] + 2;
    fontMap[(BYTE)')'] = fontMap[(BYTE)'('] + 3;
    fontMap[(BYTE)'*'] = fontMap[(BYTE)')'] + 3;
    fontMap[(BYTE)'+'] = fontMap[(BYTE)'*'] + 2;
    fontMap[(BYTE)','] = fontMap[(BYTE)'+'] + 4;
    fontMap[(BYTE)'-'] = fontMap[(BYTE)','] + 3;
    fontMap[(BYTE)'.'] = fontMap[(BYTE)'-'] + 4;
    fontMap[(BYTE)'/'] = fontMap[(BYTE)'.'] + 2;
    
    fontMap[(BYTE)'0'] = fontMap[(BYTE)'/'] + 5;
    fontMap[(BYTE)'1'] = fontMap[(BYTE)'0'] + 5;
    fontMap[(BYTE)'2'] = fontMap[(BYTE)'1'] + 4;
    fontMap[(BYTE)'3'] = fontMap[(BYTE)'2'] + 5;
    fontMap[(BYTE)'4'] = fontMap[(BYTE)'3'] + 5;
    fontMap[(BYTE)'5'] = fontMap[(BYTE)'4'] + 5;
    fontMap[(BYTE)'6'] = fontMap[(BYTE)'5'] + 5;
    fontMap[(BYTE)'7'] = fontMap[(BYTE)'6'] + 5;
    fontMap[(BYTE)'8'] = fontMap[(BYTE)'7'] + 5;
    fontMap[(BYTE)'9'] = fontMap[(BYTE)'8'] + 5;
    fontMap[(BYTE)':'] = fontMap[(BYTE)'9'] + 5;

    fontMap[(BYTE)';'] = fontMap[(BYTE)':'] + 2;
    fontMap[(BYTE)'<'] = fontMap[(BYTE)';'] + 3;
    fontMap[(BYTE)'='] = fontMap[(BYTE)'<'] + 4;
    fontMap[(BYTE)'>'] = fontMap[(BYTE)'='] + 4;
    fontMap[(BYTE)'?'] = fontMap[(BYTE)'>'] + 4;
    fontMap[(BYTE)'A'] = fontMap[(BYTE)'?'] + 4;

    fontMap[(BYTE)'B'] = fontMap[(BYTE)'A'] + 6;
    fontMap[(BYTE)'C'] = fontMap[(BYTE)'B'] + 6;
    fontMap[(BYTE)'D'] = fontMap[(BYTE)'C'] + 6;
    fontMap[(BYTE)'E'] = fontMap[(BYTE)'D'] + 6;
    fontMap[(BYTE)'F'] = fontMap[(BYTE)'E'] + 6;
    fontMap[(BYTE)'G'] = fontMap[(BYTE)'F'] + 6;
    fontMap[(BYTE)'H'] = fontMap[(BYTE)'G'] + 6;
    fontMap[(BYTE)'I'] = fontMap[(BYTE)'H'] + 6;
    fontMap[(BYTE)'J'] = fontMap[(BYTE)'I'] + 4;
    fontMap[(BYTE)'K'] = fontMap[(BYTE)'J'] + 5;
    fontMap[(BYTE)'L'] = fontMap[(BYTE)'K'] + 6;
    fontMap[(BYTE)'M'] = fontMap[(BYTE)'L'] + 6;
    fontMap[(BYTE)'N'] = fontMap[(BYTE)'M'] + 6;
    fontMap[(BYTE)'O'] = fontMap[(BYTE)'N'] + 6;
    fontMap[(BYTE)'P'] = fontMap[(BYTE)'O'] + 6;
    fontMap[(BYTE)'Q'] = fontMap[(BYTE)'P'] + 6;
    fontMap[(BYTE)'R'] = fontMap[(BYTE)'Q'] + 6;
    fontMap[(BYTE)'S'] = fontMap[(BYTE)'R'] + 6;
    fontMap[(BYTE)'T'] = fontMap[(BYTE)'S'] + 6;
    fontMap[(BYTE)'U'] = fontMap[(BYTE)'T'] + 6;
    fontMap[(BYTE)'V'] = fontMap[(BYTE)'U'] + 6;
    fontMap[(BYTE)'W'] = fontMap[(BYTE)'V'] + 6;
    fontMap[(BYTE)'X'] = fontMap[(BYTE)'W'] + 8;
    fontMap[(BYTE)'Y'] = fontMap[(BYTE)'X'] + 6;
    fontMap[(BYTE)'Z'] = fontMap[(BYTE)'Y'] + 6;

    fontMap[(BYTE)'['] = fontMap[(BYTE)'Z'] + 6;
    fontMap[(BYTE)'\\'] = fontMap[(BYTE)'['] + 3;
    fontMap[(BYTE)']'] = fontMap[(BYTE)'\\'] + 5;
    fontMap[(BYTE)'^'] = fontMap[(BYTE)']'] + 3;
    fontMap[(BYTE)'_'] = fontMap[(BYTE)'^'] + 4;

    fontMap[(BYTE)'a'] = fontMap[(BYTE)'_'] + 4;
    fontMap[(BYTE)'b'] = fontMap[(BYTE)'a'] + 6;
    fontMap[(BYTE)'c'] = fontMap[(BYTE)'b'] + 5;
    fontMap[(BYTE)'d'] = fontMap[(BYTE)'c'] + 5;
    fontMap[(BYTE)'e'] = fontMap[(BYTE)'d'] + 5;
    fontMap[(BYTE)'f'] = fontMap[(BYTE)'e'] + 5;
    fontMap[(BYTE)'g'] = fontMap[(BYTE)'f'] + 4;
    fontMap[(BYTE)'h'] = fontMap[(BYTE)'g'] + 5;
    fontMap[(BYTE)'i'] = fontMap[(BYTE)'h'] + 5;
    fontMap[(BYTE)'j'] = fontMap[(BYTE)'i'] + 4;
    fontMap[(BYTE)'k'] = fontMap[(BYTE)'j'] + 3;
    fontMap[(BYTE)'l'] = fontMap[(BYTE)'k'] + 5;

    fontMap[(BYTE)'m'] = fontMap[(BYTE)'l'] + 4;
    fontMap[(BYTE)'n'] = fontMap[(BYTE)'m'] + 6;
    fontMap[(BYTE)'o'] = fontMap[(BYTE)'n'] + 5;
    fontMap[(BYTE)'p'] = fontMap[(BYTE)'o'] + 5;
    fontMap[(BYTE)'q'] = fontMap[(BYTE)'p'] + 5;
    fontMap[(BYTE)'r'] = fontMap[(BYTE)'q'] + 5;
    fontMap[(BYTE)'s'] = fontMap[(BYTE)'r'] + 4;
    fontMap[(BYTE)'t'] = fontMap[(BYTE)'s'] + 5;
    fontMap[(BYTE)'u'] = fontMap[(BYTE)'t'] + 3;
    fontMap[(BYTE)'v'] = fontMap[(BYTE)'u'] + 6;
    fontMap[(BYTE)'w'] = fontMap[(BYTE)'v'] + 6;
    fontMap[(BYTE)'x'] = fontMap[(BYTE)'w'] + 6;
    fontMap[(BYTE)'y'] = fontMap[(BYTE)'x'] + 6;
    fontMap[(BYTE)'z'] = fontMap[(BYTE)'y'] + 5;
    
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'z'] + 5;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 7;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 8;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 7;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 7;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 7;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 7;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 7;

    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 5;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 5;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 5;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 5;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 5;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 5;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 5;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 5;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 5;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 5;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 5;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 7;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 5;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 5;
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'�'] + 6;
    
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'A'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'B'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'E'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'K'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'M'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'H'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'O'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'P'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'C'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'T'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'Y'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'X'];

    fontMap[(BYTE)'�'] = fontMap[(BYTE)'a'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'e'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'o'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'p'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'c'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'y'];
    fontMap[(BYTE)'�'] = fontMap[(BYTE)'x'];
};