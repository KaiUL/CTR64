/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 94):
 * <joshuahuelsman@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Josh Huelsman
 * ----------------------------------------------------------------------------
 */
#include "z64.h"
#include <cstring>

inline u32
SwapME32(u32 Data)
{
    u32 Value = 0;
    Value |= ((Data & 0xFF) << 8);
    Value |= ((Data & 0xFF00) >> 8);
    Value |= ((Data & 0xFF0000) << 8);
    Value |= ((Data & 0xFF000000) >> 8);
    return Value;
}

static void
Z64GetHeader(z64 *Z64, FILE *File)
{
    z64_hdr *Hdr = &Z64->Hdr;
    u32 *StaleData = (u32 *)linearAlloc(sizeof(z64_hdr));
    fread(StaleData, sizeof(z64_hdr), 1, File);
    if (Z64->Flags & Z64_FLAG_MIDDLE_ENDIAN)
    {
        for (u32 i = 0; i < (sizeof(z64_hdr) / 4); ++i)
        {
            StaleData[i] = SwapME32(StaleData[i]);
        }
    }

    memcpy(Hdr, StaleData, sizeof(z64_hdr));
    linearFree(StaleData);

    Hdr->ClockRate = __builtin_bswap32(Hdr->ClockRate);
    Hdr->PC = __builtin_bswap32(Hdr->PC);
    Hdr->Release = __builtin_bswap32(Hdr->Release);
    Hdr->CRC1 = __builtin_bswap32(Hdr->CRC1);
    Hdr->CRC2 = __builtin_bswap32(Hdr->CRC2);
    Hdr->ManufacturerID = __builtin_bswap32(Hdr->ManufacturerID);
    Hdr->CartridgeID = __builtin_bswap16(Hdr->CartridgeID);
    Hdr->Country = __builtin_bswap16(Hdr->Country);

    for (int i = 0; i < 1008; ++i)
    {
        Hdr->BootCode[i] = __builtin_bswap32(Hdr->BootCode[i]);
    }
}

void
Z64Open(z64 *Z64, int Flags, const char *FileName)
{
    Z64->FileStream = fopen(FileName, "rb");
    Z64->Flags = Flags;
    Z64GetHeader(Z64, Z64->FileStream);
}

void
Z64Close(z64 *Z64)
{
    fclose(Z64->FileStream);
}

const char *
Z64GetCountryString(u16 Country)
{
    switch (Country)
    {
        case COUNTRY_GERMANY:
            return "Germany";
        case COUNTRY_USA:
            return "USA";
        case COUNTRY_JAPAN:
            return "Japan";
        case COUNTRY_EUROPE:
            return "Europe";
        case COUNTRY_AUSTRALIA:
            return "Australia";
    }

    return "Unknown";
}

const char *
Z64GetManufacturerString(u16 ManufacturerID)
{
    switch (ManufacturerID) {
        case MANUFACTURER_NINTENDO:
            return "Nintendo";
    }

    return "Unknown";
}

