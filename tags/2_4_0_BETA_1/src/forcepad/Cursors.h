//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2003 Division of Structural Mecahnics, Lund University
//
// Written by Jonas Lindemann
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
// Comments and suggestions to jonas.lindemann@byggmek.lth.se
//

/* Matt's crsr format V1.0 */
unsigned int cross1_and[32] = {
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0x0707ffff,
    0x0707ffff,
    0x0707ffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
unsigned int cross1_xor[32] = {
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0xf8f80000,
    0x00000000,
    0xf8f80000,
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
int cross1_hotX = 0, cross1_hotY = 0;
/* Matt's crsr format V1.0 */
unsigned int cross_bc_and[32] = {
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0x0707ffff,
    0x0707ffff,
    0x0707ffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xfffc3fff,
    0xfffc3fff,
    0xfffc3fff,
    0xffe007ff,
    0xffe007ff,
    0xffe007ff,
    0xffe007ff,
    0xfffc3fff,
    0xfffc3fff,
    0xfffc3fff,
    0xff00007f,
    0xff00007f,
    0xff00007f,
    0xff00007f,
    0xff00007f,
    0xff00007f,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
unsigned int cross_bc_xor[32] = {
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0xf8f80000,
    0x00000000,
    0xf8f80000,
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0x0003c000,
    0x00024000,
    0x00024000,
    0x001e7800,
    0x00100800,
    0x00100800,
    0x001e7800,
    0x00024000,
    0x00024000,
    0x0003c000,
    0x00ffff80,
    0x00b9ca80,
    0x00b6b980,
    0x00b6ba80,
    0x0089ca80,
    0x00ffff80,
    0x00000000,
    0x00000000,
    0x00000000,
};
int cross_bc_hotX = 6, cross_bc_hotY = 6;
/* Matt's crsr format V1.0 */
unsigned int cross_bucket_and[32] = {
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xfffffbff,
    0xfffff1ff,
    0xffff00ff,
    0xfffc007f,
    0xfff8003f,
    0xfff0001f,
    0xffe0000f,
    0xf8c00007,
    0xf8c00003,
    0xf8c40007,
    0xf8ce000f,
    0xf8ff001f,
    0x0707803f,
    0x0707c07f,
    0x0707e0ff,
    0xf8fff1ff,
    0xf8fffbff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
};
unsigned int cross_bucket_xor[32] = {
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000400,
    0x00000a00,
    0x0000f500,
    0x00030a80,
    0x00041540,
    0x00082aa0,
    0x00105550,
    0x0520aaa8,
    0x05045554,
    0x050a2aa8,
    0x05111550,
    0x05008aa0,
    0xf8f84540,
    0x00002280,
    0xf8f81100,
    0x05000a00,
    0x05000400,
    0x05000000,
    0x05000000,
    0x05000000,
};
int cross_bucket_hotX = 6, cross_bucket_hotY = 25;
/* Matt's crsr format V1.0 */
unsigned int cross_circle_and[32] = {
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0x0707ffff,
    0x0707ffff,
    0x0707ffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8fc0fff,
    0xf8f807ff,
    0xfff003ff,
    0xffe001ff,
    0xffe001ff,
    0xffe001ff,
    0xffe001ff,
    0xffe001ff,
    0xffe001ff,
    0xfff003ff,
    0xfff807ff,
    0xfffc0fff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
unsigned int cross_circle_xor[32] = {
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0xf8f80000,
    0x00000000,
    0xf8f80000,
    0x05000000,
    0x05000000,
    0x05000000,
    0x05015000,
    0x0502a800,
    0x00055400,
    0x000aaa00,
    0x00155400,
    0x000aaa00,
    0x00155400,
    0x000aaa00,
    0x00155400,
    0x000aa800,
    0x00055000,
    0x0002a000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
int cross_circle_hotX = 6, cross_circle_hotY = 6;
/* Matt's crsr format V1.0 */
unsigned int cross_line_and[32] = {
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0x0707ffff,
    0x0707ffff,
    0x0707ffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8fff9ff,
    0xf8fff0ff,
    0xf8ffe1ff,
    0xffffc3ff,
    0xffff87ff,
    0xffff0fff,
    0xfffe1fff,
    0xfffc3fff,
    0xfff87fff,
    0xfff0ffff,
    0xffe1ffff,
    0xffe3ffff,
    0xffe7ffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
unsigned int cross_line_xor[32] = {
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0xf8f80000,
    0x00000000,
    0xf8f80000,
    0x05000000,
    0x05000000,
    0x05000600,
    0x05000c00,
    0x05001800,
    0x00003000,
    0x00006000,
    0x0000c000,
    0x00018000,
    0x00030000,
    0x00060000,
    0x000c0000,
    0x00180000,
    0x00100000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
int cross_line_hotX = 6, cross_line_hotY = 6;
/* Matt's crsr format V1.0 */
unsigned int cross_load_and[32] = {
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0x0707ffff,
    0x0707ffbf,
    0x0707ff1f,
    0xf8fffe0f,
    0xf8fffc1f,
    0xf8fff83f,
    0xf8fff07f,
    0xf8fc20ff,
    0xfffc01ff,
    0xfffc03ff,
    0xfffc07ff,
    0xfffc03ff,
    0xfffc03ff,
    0xfffc03ff,
    0xfffc03ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
unsigned int cross_load_xor[32] = {
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0xf8f80000,
    0x00000040,
    0xf8f800a0,
    0x05000110,
    0x05000220,
    0x05000440,
    0x05000880,
    0x0503d100,
    0x00026200,
    0x00024400,
    0x00020800,
    0x00021c00,
    0x00020400,
    0x00020400,
    0x0003fc00,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
int cross_load_hotX = 6, cross_load_hotY = 6;
/* Matt's crsr format V1.0 */
unsigned int cross_rect_and[32] = {
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0x0707ffff,
    0x0707ffff,
    0x0707ffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8e001ff,
    0xf8e001ff,
    0xffe001ff,
    0xffe001ff,
    0xffe001ff,
    0xffe001ff,
    0xffe001ff,
    0xffe001ff,
    0xffe001ff,
    0xffe001ff,
    0xffe001ff,
    0xffe001ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
unsigned int cross_rect_xor[32] = {
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0xf8f80000,
    0x00000000,
    0xf8f80000,
    0x05000000,
    0x05000000,
    0x05000000,
    0x05155400,
    0x050aaa00,
    0x00155400,
    0x000aaa00,
    0x00155400,
    0x000aaa00,
    0x00155400,
    0x000aaa00,
    0x00155400,
    0x000aaa00,
    0x00155400,
    0x000aaa00,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
int cross_rect_hotX = 6, cross_rect_hotY = 6;
/* Matt's crsr format V1.0 */
unsigned int cross_select_and[32] = {
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0x0707ffff,
    0x0707ffff,
    0x0707ffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8ffffff,
    0xf8e331ff,
    0xf8e331ff,
    0xffe7f9ff,
    0xffffffff,
    0xffffffff,
    0xffe7f9ff,
    0xffe7f9ff,
    0xffffffff,
    0xffffffff,
    0xffe7f9ff,
    0xffe331ff,
    0xffe331ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
unsigned int cross_select_xor[32] = {
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0x05000000,
    0xf8f80000,
    0x00000000,
    0xf8f80000,
    0x05000000,
    0x05000000,
    0x05000000,
    0x051cce00,
    0x05100200,
    0x00100200,
    0x00000000,
    0x00000000,
    0x00100200,
    0x00100200,
    0x00000000,
    0x00000000,
    0x00100200,
    0x00100200,
    0x001cce00,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
int cross_select_hotX = 6, cross_select_hotY = 6;
/* Matt's crsr format V1.0 */
unsigned int erase_and[32] = {
    0x0000ffff,
    0x0000ffc3,
    0x3ffcffc3,
    0x3ffcffc3,
    0x3ffce1c3,
    0x3ffce1c3,
    0x3ffc8043,
    0x3ffc8000,
    0x3ffc8000,
    0x3ffc8001,
    0x3ffce1c3,
    0x3ffce1e7,
    0x3ffcffff,
    0x3ffcf9ff,
    0x0000f1ff,
    0x0000e0ff,
    0xffffc03f,
    0xffff803f,
    0xffff000f,
    0xfffe000f,
    0xfffc001f,
    0xfff8003f,
    0xfff0007f,
    0xffe000ff,
    0xffe001ff,
    0xffe003ff,
    0xfff007ff,
    0xfff80fff,
    0xfffc1fff,
    0xfffe3fff,
    0xffffffff,
    0xffffffff,
};
unsigned int erase_xor[32] = {
    0x00000000,
    0x7ffe003c,
    0x40020024,
    0x40020024,
    0x40021e24,
    0x40021224,
    0x400273a4,
    0x400240e7,
    0x40024081,
    0x400273c2,
    0x40021224,
    0x40021e18,
    0x40020000,
    0x40020400,
    0x7ffe0a00,
    0x00001700,
    0x00002f80,
    0x00005f40,
    0x0000bea0,
    0x00017d50,
    0x0002faa0,
    0x00047540,
    0x00082a80,
    0x00101500,
    0x00100a00,
    0x00100400,
    0x00080800,
    0x00041000,
    0x00022000,
    0x0001c000,
    0x00000000,
    0x00000000,
};
int erase_hotX = 7, erase_hotY = 7;
/* Matt's crsr format V1.0 */
unsigned int rshape16_and[32] = {
    0xf00fffff,
    0xc003ffff,
    0x87e1ffff,
    0x8ff1ffff,
    0x1ff8ffff,
    0x3ffcffff,
    0x3ffcffff,
    0x3ffcffff,
    0x3ffcffff,
    0x3ffcffff,
    0x3ffcffff,
    0x1ff8ffff,
    0x8ff1ffff,
    0x87e1ffff,
    0xc003ffff,
    0xf00fffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
unsigned int rshape16_xor[32] = {
    0x00000000,
    0x07e00000,
    0x18180000,
    0x300c0000,
    0x20040000,
    0x40020000,
    0x40020000,
    0x40020000,
    0x40020000,
    0x40020000,
    0x40020000,
    0x20040000,
    0x300c0000,
    0x18180000,
    0x07e00000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
int rshape16_hotX = 8, rshape16_hotY = 8;
/* Matt's crsr format V1.0 */
unsigned int rshape32_and[32] = {
    0xffc003ff,
    0xff0000ff,
    0xfc1ff83f,
    0xf87ffe1f,
    0xf1ffff8f,
    0xe3ffffc7,
    0xc7ffffe3,
    0xcffffff3,
    0x8ffffff1,
    0x9ffffff9,
    0x1ffffff8,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x1ffffff8,
    0x9ffffff9,
    0x8ffffff1,
    0xcffffff3,
    0xc7ffffe3,
    0xe3ffffc7,
    0xf1ffff8f,
    0xf87ffe1f,
    0xfc1ff83f,
    0xff0000ff,
    0xffc003ff,
};
unsigned int rshape32_xor[32] = {
    0x00000000,
    0x001ff800,
    0x00600600,
    0x01800180,
    0x02000040,
    0x04000020,
    0x08000010,
    0x10000008,
    0x10000008,
    0x20000004,
    0x20000004,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x20000004,
    0x20000004,
    0x10000008,
    0x10000008,
    0x08000010,
    0x04000020,
    0x02000040,
    0x01800180,
    0x00600600,
    0x001ff800,
    0x00000000,
};
int rshape32_hotX = 16, rshape32_hotY = 16;
/* Matt's crsr format V1.0 */
unsigned int rshape4_and[32] = {
    0x0fffffff,
    0x0fffffff,
    0x0fffffff,
    0x0fffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
unsigned int rshape4_xor[32] = {
    0x00000000,
    0x60000000,
    0x60000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
int rshape4_hotX = 2, rshape4_hotY = 2;
/* Matt's crsr format V1.0 */
unsigned int rshape8_and[32] = {
    0x81ffffff,
    0x00ffffff,
    0x3cffffff,
    0x3cffffff,
    0x3cffffff,
    0x3cffffff,
    0x00ffffff,
    0x81ffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
unsigned int rshape8_xor[32] = {
    0x00000000,
    0x3c000000,
    0x42000000,
    0x42000000,
    0x42000000,
    0x42000000,
    0x3c000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
int rshape8_hotX = 4, rshape8_hotY = 4;
/* Matt's crsr format V1.0 */
unsigned int sshape16_and[32] = {
    0x0000ffff,
    0x0000ffff,
    0x3ffcffff,
    0x3ffcffff,
    0x3ffcffff,
    0x3ffcffff,
    0x3ffcffff,
    0x3ffcffff,
    0x3ffcffff,
    0x3ffcffff,
    0x3ffcffff,
    0x3ffcffff,
    0x3ffcffff,
    0x3ffcffff,
    0x0000ffff,
    0x0000ffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
unsigned int sshape16_xor[32] = {
    0x00000000,
    0x7ffe0000,
    0x40020000,
    0x40020000,
    0x40020000,
    0x40020000,
    0x40020000,
    0x40020000,
    0x40020000,
    0x40020000,
    0x40020000,
    0x40020000,
    0x40020000,
    0x40020000,
    0x7ffe0000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
int sshape16_hotX = 8, sshape16_hotY = 8;
/* Matt's crsr format V1.0 */
unsigned int sshape32_and[32] = {
    0x00000000,
    0x00000000,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x3ffffffc,
    0x00000000,
    0x00000000,
};
unsigned int sshape32_xor[32] = {
    0x00000000,
    0x7ffffffe,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x40000002,
    0x7ffffffe,
    0x00000000,
};
int sshape32_hotX = 16, sshape32_hotY = 16;
/* Matt's crsr format V1.0 */
unsigned int sshape4_and[32] = {
    0x0fffffff,
    0x0fffffff,
    0x0fffffff,
    0x0fffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
unsigned int sshape4_xor[32] = {
    0x00000000,
    0x60000000,
    0x60000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
int sshape4_hotX = 2, sshape4_hotY = 2;
/* Matt's crsr format V1.0 */
unsigned int sshape8_and[32] = {
    0x00ffffff,
    0x00ffffff,
    0x3cffffff,
    0x3cffffff,
    0x3cffffff,
    0x3cffffff,
    0x00ffffff,
    0x00ffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
unsigned int sshape8_xor[32] = {
    0x00000000,
    0x7e000000,
    0x42000000,
    0x42000000,
    0x42000000,
    0x42000000,
    0x7e000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
int sshape8_hotX = 4, sshape8_hotY = 4;