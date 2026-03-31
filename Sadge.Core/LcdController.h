#pragma once

#include "InterruptProvider.h"

#include <array>

struct Pixel
{
  uint8_t R{};
  uint8_t G{};
  uint8_t B{};
  uint8_t A{};
};

constexpr static auto SCREEN_WIDTH = 160;
constexpr static auto SCREEN_HEIGHT = 144;
constexpr static auto SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;

typedef void (*FrameCallback)(const std::array<Pixel, SCREEN_SIZE>& frame, double frameTime);

// TODO: https://github.com/AntonioND/giibiiadvance/blob/master/docs/TCAGBD.pdf
class LcdController : public InterruptProvider
{
public:
  enum class Address : uint16_t
  {
    CONTROL = 0xFF40,
    STATUS = 0xFF41,
    SCREEN_Y = 0xFF42,
    SCREEN_X = 0xFF43,
    LCD_Y = 0xFF44,
    LCD_Y_COMP = 0xFF45,
    DMA = 0xFF46,
    BG_PALETTE = 0xFF47,
    OBJ_PALETTE_0 = 0xFF48,
    OBJ_PALETTE_1 = 0xFF49,
    WINDOW_Y = 0xFF4A,
    WINDOW_X = 0xFF4B,
    START = CONTROL,
    END = WINDOW_X
  };

  LcdController(InterruptReceiver& intRec, uint8_t& dataBus, uint16_t& addrBus);

  void SetColorPalettes(const std::array<Pixel, 4>& bg, const std::array<Pixel, 4>& obj0, const std::array<Pixel, 4>& obj1);

  void SetFrameCallback(FrameCallback cb);

  bool DmaRequested() const;

  uint16_t GetCurrentDmaAddress() const;

  std::array<Pixel, SCREEN_SIZE> GetCurrentFrameBuffer();

  void ServiceDma(uint8_t val);

  void UpdateFrameTime(double frameTime);

  bool Update();

  void Read() const;
  void Write();

private:

  struct Object
  {
    int y;
    int x;
    uint8_t tileIdx;
    uint8_t attr;
  };

  constexpr static std::array<Pixel, 4> DEFAULT_COLOR_PALETTE{
    Pixel{0xFF, 0xFF, 0xFF, 0xFF},
    Pixel{0xFF, 0x84, 0x84, 0xFF},
    Pixel{0x94, 0x3A, 0x3A, 0xFF},
    Pixel{0x00, 0x00, 0x00, 0xFF}
  };

  constexpr static auto KILOBYTES_TO_BYTES = 1024;
  constexpr static auto VRAM_SIZE = 8 * KILOBYTES_TO_BYTES;
  constexpr static auto OAM_SIZE = 160;

  constexpr static auto NUM_TILES_PER_ROW = 32;
  constexpr static auto NUM_BYTES_PER_TILE = 16;
  constexpr static auto NUM_BYTES_PER_X_ROW = 2;
  constexpr static auto NUM_PIXELS_PER_TILE_ROW = 8;
  constexpr static auto NUM_PIXELS_PER_TILE_COLUMN = 8;
  constexpr static auto NUM_PIXELS_PER_DOUBLE_TILE_COLUMN = NUM_PIXELS_PER_TILE_COLUMN * 2;
  constexpr static auto BITS_PER_COLOR = 2;

  constexpr static auto WINDOW_X_OFFSET = 7;

  constexpr static auto TILE_MAP_OFFSET_0 = 0x1800;
  constexpr static auto TILE_MAP_OFFSET_1 = 0x1C00;
  constexpr static auto TILE_BLOCK_1_OFFSET = 0x1000;
  constexpr static auto TILE_BLOCK_0_THRESH = 127;

  constexpr static auto NUM_BYTES_PER_ATTRIBUTES = 4;
  constexpr static auto NUM_OBJECTS = OAM_SIZE / NUM_BYTES_PER_ATTRIBUTES;

  constexpr static auto OBJECT_Y_OFFSET = 16;
  constexpr static auto OBJECT_X_OFFSET = 8;
  constexpr static auto OBJECT_LIMIT = 10;
  constexpr static auto DOUBLESIZE_TILE_0_MASK = 0x01;
  constexpr static auto DOUBLESIZE_TILE_1_MASK = 0xFE;

  constexpr static auto CYCLES_PER_ROW = 456;
  constexpr static auto FRAME_CYCLES = 154 * CYCLES_PER_ROW;

  enum class ObjectAttributeBitMask : uint8_t
  {
    PALLETE_SELECT = 0b00010000,
    FLIP_X = 0b00100000,
    FLIP_Y = 0b01000000,
    PRIORITY = 0b10000000
  };

  enum class LcdcBitMask : uint8_t
  {
    ENABLE        = 0b10000000,
    WIN_TILE_MAP  = 0b01000000,
    WIN_ENABLE    = 0b00100000,
    BG_WIN_TILES  = 0b00010000,
    BG_TILE_MAP   = 0b00001000,
    OBJ_SIZE      = 0b00000100,
    OBJ_ENABLE    = 0b00000010,
    BG_WIN_ENABLE = 0b00000001
  };

  enum class StatBitMask : uint8_t
  {
    LYC_INT_SELECT   = 0b01000000,
    MODE2_INT_SELECT = 0b00100000,
    MODE1_INT_SELECT = 0b00010000,
    MODE0_INT_SELECT = 0b00001000,
    LYC_EQ_LY        = 0b00000100,
    PPU_MODE         = 0b00000011
  };

  enum class Modes : uint8_t
  {
    MODE_2_OAM = 2,
    MODE_3_DRAW = 3,
    MODE_0_HBLANK = 0,
    MODE_1_VBLANK = 1
  };

  constexpr uint8_t GetAttributeBitMask(ObjectAttributeBitMask mask)
  {
    return static_cast<uint8_t>(mask);
  }

  constexpr uint8_t GetStatBitMask(StatBitMask mask) const
  {
    return static_cast<uint8_t>(mask);
  }

  constexpr uint8_t GetLcdcBitMask(LcdcBitMask mask) const
  {
    return static_cast<uint8_t>(mask);
  }

  constexpr uint8_t GetMode(Modes mode)
  {
    return static_cast<uint8_t>(mode);
  }

  void HandleLcdcWrite();
  void HandleStatWrite();
  void HandleDmaWrite();

  void UpdateDma();

  bool GetStatState() const;
  void CheckStatInterrupt(StatBitMask mask);
  void SetStatMode(Modes mode);

  void Transition();

  void Mode1();
  void Mode2();
  void Mode3();
  void Mode0();

  void CheckLyc();
  void SetLyc();
  void ResetLyc();

  bool UpdateDisabled();
  bool UpdateState();

  void PopulateFrameLine();
  void RenderScanline();
  void RenderBackground();
  void RenderObjects();
  void RenderWindow();

  FrameCallback mCb = nullptr;

  Modes mCurrMode = Modes::MODE_0_HBLANK;
  Modes mNextMode = Modes::MODE_3_DRAW;
  uint32_t mRemainingModeCycles = 76;

  std::array<uint8_t, SCREEN_WIDTH> mLineBuf{};
  std::array<Pixel, SCREEN_SIZE>    mFrameBuf{};
  std::array<uint8_t, OAM_SIZE>     mOam{};
  std::array<uint8_t, VRAM_SIZE>    mVRAM{};
  std::array<Object, OBJECT_LIMIT>  mObjects{};

  uint8_t mLCDC{0};
  bool    mBgWinEnable{false};
  bool    mWinEnable{false};
  bool    mBgTileMap{false};
  bool    mBgWinTiles{false};
  bool    mWinTileMap{false};
  bool    mObjEnable{false};
  bool    mDoublesize{false};

  uint8_t mSTAT{0};
  bool    mLycIntSel{false};
  bool    mMode0IntSel{false};
  bool    mMode1IntSel{false};
  bool    mMode2IntSel{false};
  bool    mLycEqLy{false};
  uint8_t mStatMode{0};

  uint8_t mSCY{0};
  uint8_t mSCX{0};
  uint8_t mLY{0};
  uint8_t mLYC{0};
  uint8_t mDMA{0};
  uint8_t mBGP{0};
  uint8_t mOBP0{0};
  uint8_t mOBP1{0};
  uint8_t mWY{0};
  uint8_t mWX{0};
  uint8_t mWLY{0};

  bool     mEnabled{false};
  double   mFrameTime{0};
  uint32_t mFrameCycleCount{0};
  uint32_t mDisabledCycleCount{0};
  bool     mDelayFrame{true};
  bool     mFrameReady{false};
  
  uint16_t mDmaStart = 0;
  uint8_t mDmaPtr = 0;
  bool mDmaReq = false;
  bool mEnableNext{false};

  uint8_t&  mDataBus;
  uint16_t& mAddrBus;

  std::array<Pixel, 4> mBgPalette   = DEFAULT_COLOR_PALETTE;
  std::array<Pixel, 4> mObj0Palette = DEFAULT_COLOR_PALETTE;
  std::array<Pixel, 4> mObj1Palette = DEFAULT_COLOR_PALETTE;
};