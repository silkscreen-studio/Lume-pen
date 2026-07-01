// Minimal vibecoded library I'll use as a basis to create my own library once I have the hardware in hand. This just serves as a basis to test the AW933105 IC.

#include "AW9310X_min.h"

// ── Expected chip ID (upper 16 bits of register 0xFF10) ───────
#define AW_CHIP_ID      0xA961

// ── Key registers ─────────────────────────────────────────────
#define REG_CHIP_ID     0xFF10  // read to verify IC presence
#define REG_HOSTCTRL2   0xFF0C  // write 0 → software reset
#define REG_SCANCTRL0   0x0000  // channel enable bits
#define REG_PROXINTEN   0x009C  // proximity interrupt enable
#define REG_HAAPEREN    0xFFF4  // hardware enable (must be set)
#define REG_HOSTIRQEN   0xF084  // host IRQ enable
#define REG_CMD         0xF008  // 1=active, 2=sleep, 3=deep-sleep
#define REG_STAT0       0x0090  // touch status: bit N = channel N touched

// ── Current touch state ───────────────────────────────────────
static uint8_t s_mask = 0;  // bits 0–4, one per channel

// ═════════════════════════════════════════════════════════════
//  I2C helpers
//  AW9310X protocol: 16-bit address + 32-bit value, big-endian.
// ═════════════════════════════════════════════════════════════

static bool aw_write(uint16_t reg, uint32_t val)
{
    Wire.beginTransmission(AW_ADDR);
    Wire.write((uint8_t)(reg >> 8));
    Wire.write((uint8_t)(reg & 0xFF));
    Wire.write((uint8_t)(val >> 24));
    Wire.write((uint8_t)(val >> 16));
    Wire.write((uint8_t)(val >>  8));
    Wire.write((uint8_t)(val      ));
    return Wire.endTransmission() == 0;
}

static bool aw_read(uint16_t reg, uint32_t &val)
{
    Wire.beginTransmission(AW_ADDR);
    Wire.write((uint8_t)(reg >> 8));
    Wire.write((uint8_t)(reg & 0xFF));
    if (Wire.endTransmission(true) != 0) return false;
    if (Wire.requestFrom(AW_ADDR, (uint8_t)4) < 4) return false;
    val  = (uint32_t)Wire.read() << 24;
    val |= (uint32_t)Wire.read() << 16;
    val |= (uint32_t)Wire.read() <<  8;
    val |= (uint32_t)Wire.read();
    return true;
}

// ═════════════════════════════════════════════════════════════
//  Default register table (PROGMEM — saves ~780 bytes of SRAM)
//  Taken verbatim from Awinic MCU driver v2.2.0, aw9310x_reg.h.
//  Only the last few entries are ours (channel enable, CMD).
// ═════════════════════════════════════════════════════════════

struct RegEntry { uint16_t addr; uint32_t data; };

static const RegEntry DEFAULTS[] PROGMEM = {
    { 0x0000, 0x00001F1F }, // SCANCTRL0: enable CH0–CH4 + offsets
    { 0x0004, 0x03F0000A }, // SCANCTRL1: scan rate
    { 0x0008, 0x0017A13E }, // SCANCTRL2: frequency spread
    { 0x000C, 0x05000000 }, // SCANCTRL3: FIFO/data format
    { 0x0010, 0x00063FFD }, // AFE CH0
    { 0x0014, 0x00000009 },
    { 0x0018, 0xD81C8207 },
    { 0x001C, 0xFF000000 },
    { 0x0020, 0x00000000 },
    { 0x0024, 0x00063FF7 }, // AFE CH1
    { 0x0028, 0x00000009 },
    { 0x002C, 0xD81C8207 },
    { 0x0030, 0xFF000000 },
    { 0x0034, 0x00000000 },
    { 0x0038, 0x00063FDF }, // AFE CH2
    { 0x003C, 0x00000009 },
    { 0x0040, 0xD81C8207 },
    { 0x0044, 0xFF000000 },
    { 0x0048, 0x00000000 },
    { 0x004C, 0x00063F7F }, // AFE CH3
    { 0x0050, 0x00000009 },
    { 0x0054, 0xD81C8207 },
    { 0x0058, 0xFF000000 },
    { 0x005C, 0x00000000 },
    { 0x0060, 0x00063DFF }, // AFE CH4
    { 0x0064, 0x00000009 },
    { 0x0068, 0xD81C8207 },
    { 0x006C, 0xFF000000 },
    { 0x0070, 0x00000000 },
    { 0x00A0, 0xE0400000 }, // proximity config CH0
    { 0x00A4, 0x00000000 },
    { 0x00A8, 0x000008D2 },
    { 0x00AC, 0x00000000 },
    { 0x00B0, 0x00000000 },
    { 0x00B8, 0x000186A0 }, // PROXTH0_CH0: touch threshold = 100 000
    { 0x00BC, 0x00000000 },
    { 0x00C0, 0x00000000 },
    { 0x00C4, 0x00000000 },
    { 0x00C8, 0x00000000 },
    { 0x00CC, 0x00000000 },
    { 0x00D0, 0x00000000 },
    { 0x00D4, 0x00000000 },
    { 0x00D8, 0x00000000 },
    { 0x00DC, 0xE0400000 }, // proximity config CH1
    { 0x00E0, 0x00000000 },
    { 0x00E4, 0x000008D2 },
    { 0x00E8, 0x00000000 },
    { 0x00EC, 0x00000000 },
    { 0x00F4, 0x000186A0 }, // PROXTH0_CH1
    { 0x00F8, 0x00000000 },
    { 0x00FC, 0x00000000 },
    { 0x0100, 0x00000000 },
    { 0x0104, 0x00000000 },
    { 0x0108, 0x00000000 },
    { 0x010C, 0x00000000 },
    { 0x0110, 0x00000000 },
    { 0x0114, 0x00000000 },
    { 0x0118, 0xE0400000 }, // proximity config CH2
    { 0x011C, 0x00000000 },
    { 0x0120, 0x000008D2 },
    { 0x0124, 0x00000000 },
    { 0x0128, 0x00000000 },
    { 0x0130, 0x000186A0 }, // PROXTH0_CH2
    { 0x0134, 0x00000000 },
    { 0x0138, 0x00000000 },
    { 0x013C, 0x00000000 },
    { 0x0140, 0x00000000 },
    { 0x0144, 0x00000000 },
    { 0x0148, 0x00000000 },
    { 0x014C, 0x00000000 },
    { 0x0150, 0x00000000 },
    { 0x0154, 0xE0400000 }, // proximity config CH3
    { 0x0158, 0x00000000 },
    { 0x015C, 0x000008D2 },
    { 0x0160, 0x00000000 },
    { 0x0164, 0x00000000 },
    { 0x016C, 0x000186A0 }, // PROXTH0_CH3
    { 0x0170, 0x00000000 },
    { 0x0174, 0x00000000 },
    { 0x0178, 0x00000000 },
    { 0x017C, 0x00000000 },
    { 0x0180, 0x00000000 },
    { 0x0184, 0x00000000 },
    { 0x0188, 0x00000000 },
    { 0x018C, 0x00000000 },
    { 0x0190, 0xE0400000 }, // proximity config CH4
    { 0x0194, 0x00000000 },
    { 0x0198, 0x000008D2 },
    { 0x019C, 0x00000000 },
    { 0x01A0, 0x00000000 },
    { 0x01A8, 0x000186A0 }, // PROXTH0_CH4
    { 0x01AC, 0x00000000 },
    { 0x01B0, 0x00000000 },
    { 0x01B4, 0x00000000 },
    { 0x01B8, 0x00000000 },
    { 0x01BC, 0x00000000 },
    { 0x01C0, 0x00000000 },
    { 0x01C4, 0x00000000 },
    { 0x01C8, 0x00000000 },
    { 0x0208, 0x00000005 }, // REFACFG
    { 0x020C, 0x00000005 }, // REFBCFG
    { 0xF084, 0x00000006 }, // HOSTIRQEN: approach + far-away IRQ on
    { 0xFF00, 0x00000301 }, // HOSTCTRL1
    { 0xFF0C, 0x01000000 }, // HOSTCTRL2: normal operation
    { 0xFFF4, 0x00004011 }, // HAAPEREN: hardware enable
    { 0x009C, 0x3F3F3F3F }, // PROXINTEN: all channel IRQs enabled
    { 0xF008, 0x00000001 }, // CMD = ACTIVE  ← must be last
};

static const uint8_t DEFAULTS_LEN =
    (uint8_t)(sizeof(DEFAULTS) / sizeof(DEFAULTS[0]));

// ═════════════════════════════════════════════════════════════
//  Public API
// ═════════════════════════════════════════════════════════════

bool aw_begin(void)
{
    // 1. Verify chip ID
    uint32_t id = 0;
    for (uint8_t i = 0; i < 5; i++) {
        if (aw_read(REG_CHIP_ID, id) && (uint16_t)(id >> 16) == AW_CHIP_ID)
            goto id_ok;
        delay(10);
    }
    return false;   // IC not found
    id_ok:

    // 2. Software reset — clears all internal state
    aw_write(REG_HOSTCTRL2, 0x00000000);
    delay(20);

    // 3. Load default registers
    for (uint8_t i = 0; i < DEFAULTS_LEN; i++) {
        uint16_t a = pgm_read_word(&DEFAULTS[i].addr);
        uint32_t d = pgm_read_dword(&DEFAULTS[i].data);
        if (!aw_write(a, d)) return false;
    }

    // IC is now scanning (CMD=ACTIVE was the last write above).
    return true;
}

bool aw_update(void)
{
    uint32_t stat = 0;
    if (!aw_read(REG_STAT0, stat)) return false;
    s_mask = (uint8_t)(stat & 0x1F);   // keep bits 0–4 only
    return true;
}

uint8_t aw_channel(uint8_t ch)
{
    if (ch >= AW_NUM_CH) return 0;
    return (s_mask >> ch) & 0x01;
}

uint8_t aw_mask(void)
{
    return s_mask;
}
