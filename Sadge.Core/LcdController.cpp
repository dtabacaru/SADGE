#include "LcdController.h"

#include "Constants.h"

#include <algorithm>
#include <execution>
#include <ranges>

LcdController::LcdController(InterruptReceiver& intRec, uint8_t& dataBus, uint16_t& addrBus) :
  InterruptProvider(intRec, InterruptBitMask::LCD),
  mDataBus(dataBus),
  mAddrBus(addrBus)
{
}

void LcdController::SetColorPalettes(const std::array<Pixel, 4>& bg, const std::array<Pixel, 4>& obj0, const std::array<Pixel, 4>& obj1)
{
  mBgPalette = bg;
  mObj0Palette = obj0;
  mObj1Palette = obj1;
}

void LcdController::SetFrameCallback(FrameCallback frame_callback)
{
  mCb = frame_callback;
}

bool LcdController::GetFrameReady()
{
  bool frameReady = mFrameReady;
  mFrameReady = false;
  return frameReady;
}

bool LcdController::DmaRequested() const
{
  return mDmaReq;
};

void LcdController::SetCurrentDmaAddress() const
{
  mAddrBus = mDmaStart + mDmaPtr;
};

std::array<Pixel, SCREEN_SIZE> LcdController::GetCurrentFrameBuffer()
{
  return mFrameBuf;
}

void LcdController::UpdateFrameTime(double frame_time)
{
  mFrameTime = frame_time;
}

void LcdController::Read() const
{
  if (mAddrBus >= 0x8000 && mAddrBus <= 0x9FFF)
  {
    mDataBus = mVRAM[mAddrBus - 0x8000];
    return;
  }
  else if (mAddrBus >= 0xFE00 && mAddrBus <= 0xFE9F)
  {
    mDataBus = mOam[mAddrBus - 0xFE00];
    return;
  }

  Address addr = static_cast<Address>(mAddrBus);

  switch (addr)
  {
    case LcdController::Address::CONTROL:
      mDataBus = mLCDC;
      break;
    case LcdController::Address::STATUS:
      mDataBus = mSTAT;
      break;
    case LcdController::Address::SCREEN_Y:
      mDataBus = mSCY;
      break;
    case LcdController::Address::SCREEN_X:
      mDataBus = mSCX;
      break;
    case LcdController::Address::LCD_Y:
      mDataBus = mLY;
      break;
    case LcdController::Address::LCD_Y_COMP:
      mDataBus = mLYC;
      break;
    case LcdController::Address::DMA:
      mDataBus = mDMA;
      break;
    case LcdController::Address::BG_PALETTE:
      mDataBus = mBGP;
      break;
    case LcdController::Address::OBJ_PALETTE_0:
      mDataBus = mOBP0;
      break;
    case LcdController::Address::OBJ_PALETTE_1:
      mDataBus = mOBP1;
      break;
    case LcdController::Address::WINDOW_Y:
      mDataBus = mWY;
      break;
    case LcdController::Address::WINDOW_X:
      mDataBus = mWX;
      break;
    default:
      mDataBus = DEFAULT_READ;
      break;
  }
}

void LcdController::Write()
{
  if (mAddrBus >= 0x8000 && mAddrBus <= 0x9FFF)
  {
    mVRAM[mAddrBus - 0x8000] = mDataBus;
    return;
  }
  else if (mAddrBus >= 0xFE00 && mAddrBus <= 0xFE9F)
  {
    mOam[mAddrBus - 0xFE00] = mDataBus;
    return;
  }

  Address addr = static_cast<Address>(mAddrBus);

  switch (addr)
  {
    case LcdController::Address::CONTROL:
      HandleLcdcWrite();
      break;
    case LcdController::Address::STATUS:
      HandleStatWrite();
      break;
    case LcdController::Address::SCREEN_Y:
      mSCY = mDataBus;
      break;
    case LcdController::Address::SCREEN_X:
      mSCX = mDataBus;
      break;
    case LcdController::Address::LCD_Y:
      break;
    case LcdController::Address::LCD_Y_COMP:
      mLYC = mDataBus;
      CheckLyc();
      break;
    case LcdController::Address::DMA:
      HandleDmaWrite();
      break;
    case LcdController::Address::BG_PALETTE:
      mBGP = mDataBus;
      break;
    case LcdController::Address::OBJ_PALETTE_0:
      mOBP0 = mDataBus;
      break;
    case LcdController::Address::OBJ_PALETTE_1:
      mOBP1 = mDataBus;
      break;
    case LcdController::Address::WINDOW_Y:
      mWY = mDataBus;
      break;
    case LcdController::Address::WINDOW_X:
      mWX = mDataBus;
      break;
  }
}

void LcdController::HandleLcdcWrite()
{
  bool enabled = mLCDC & GetLcdcBitMask(LcdcBitMask::ENABLE);

  if (mEnabled && !enabled)
  {
    mDisabledCycleCount = 0;
    mWLY = 0;
    mLY = 0;
  }

  if (!mEnabled && enabled)
  {
    SetStatMode(Modes::MODE_0_HBLANK);
    mCurrMode = Modes::MODE_0_HBLANK;
    mNextMode = Modes::MODE_3_DRAW;
    mRemainingModeCycles = 80;
    mFrameCycleCount = 0;
    mDelayFrame = true;
  }

  mLCDC = mDataBus;
  
  mEnabled     = mLCDC & GetLcdcBitMask(LcdcBitMask::ENABLE);
  mBgWinEnable = mLCDC & GetLcdcBitMask(LcdcBitMask::BG_WIN_ENABLE);
  mWinEnable   = mLCDC & GetLcdcBitMask(LcdcBitMask::WIN_ENABLE);
  mBgTileMap   = mLCDC & GetLcdcBitMask(LcdcBitMask::BG_TILE_MAP);
  mBgWinTiles  = mLCDC & GetLcdcBitMask(LcdcBitMask::BG_WIN_TILES);
  mWinTileMap  = mLCDC & GetLcdcBitMask(LcdcBitMask::WIN_TILE_MAP);
  mObjEnable   = mLCDC & GetLcdcBitMask(LcdcBitMask::OBJ_ENABLE);
  mDoublesize  = mLCDC & GetLcdcBitMask(LcdcBitMask::OBJ_SIZE);
}

void LcdController::HandleStatWrite()
{
  mSTAT = mDataBus;

  mLycIntSel   = mSTAT & GetStatBitMask(StatBitMask::LYC_INT_SELECT);
  mMode0IntSel = mSTAT & GetStatBitMask(StatBitMask::MODE0_INT_SELECT);
  mMode1IntSel = mSTAT & GetStatBitMask(StatBitMask::MODE1_INT_SELECT);
  mMode2IntSel = mSTAT & GetStatBitMask(StatBitMask::MODE2_INT_SELECT);
}

void LcdController::HandleDmaWrite()
{
  mDMA = mDataBus;
  mDmaStart = mDMA * 0x100;
  mDmaPtr = 0;
  mEnableNext = true;
}

void LcdController::ServiceDma()
{
  mOam[mDmaPtr] = mDataBus;

  mDmaPtr += 1;
  if (mDmaPtr == OAM_SIZE)
    mDmaReq = false;
};

void LcdController::Update()
{
  UpdateDma();

  if (mEnabled)
  {
    UpdateLy();
    UpdateState();
  }
  else
  {
    UpdateDisabled();
  }
}

void LcdController::UpdateDma()
{
  if (mEnableNext)
  {
    mDmaReq = true;
    mEnableNext = false;
  }
}

void LcdController::RenderBackground()
{
  if (!mBgWinEnable)
  {
    for (int x = 0; x < SCREEN_WIDTH; x += 1)
    {
      mLineBuf[x] = 0;
    }

    return;
  }

  uint16_t tileMapOffset = mBgTileMap ? TILE_MAP_OFFSET_1 : TILE_MAP_OFFSET_0;

  uint8_t x = 0;
  while (x < SCREEN_WIDTH)
  {
    uint8_t globalY = mSCY + mLY;
    uint8_t globalX = mSCX + x;

    uint16_t tileIdxAddr = tileMapOffset + (globalY / NUM_PIXELS_PER_TILE_COLUMN) * NUM_TILES_PER_ROW + (globalX / NUM_PIXELS_PER_TILE_ROW);
    uint8_t tileIdx = mVRAM[tileIdxAddr];

    uint16_t bytePtr = (mBgWinTiles || (tileIdx > TILE_BLOCK_0_THRESH)) ? tileIdx * NUM_BYTES_PER_TILE
      : tileIdx * NUM_BYTES_PER_TILE + TILE_BLOCK_1_OFFSET;

    uint8_t tileBytePtr = (globalY % NUM_PIXELS_PER_TILE_COLUMN) * 2;

    int pixelsLeft = NUM_PIXELS_PER_TILE_ROW - (globalX % NUM_PIXELS_PER_TILE_ROW);
    while (pixelsLeft > 0 && x < SCREEN_WIDTH)
    {
      uint8_t colorId = (((mVRAM[bytePtr + tileBytePtr + 1] >> (pixelsLeft - 1)) & 0b1) << 1) |
        ((mVRAM[bytePtr + tileBytePtr + 0] >> (pixelsLeft - 1)) & 0b1);

      mLineBuf[x] = colorId;

      x += 1;
      pixelsLeft -= 1;
    }
  }
}

void LcdController::RenderWindow()
{
  if (!(mBgWinEnable && mWinEnable))
    return;

  if (mLY < mWY || mWX >= (SCREEN_WIDTH + WINDOW_X_OFFSET) || mWY >= SCREEN_HEIGHT)
    return;

  uint16_t tileMapOffset = mWinTileMap ? TILE_MAP_OFFSET_1 : TILE_MAP_OFFSET_0;

  int winXStart = mWX - WINDOW_X_OFFSET;
  int x = winXStart < 0 ? 0 : winXStart;
  while (x < SCREEN_WIDTH)
  {
    int relX = x - winXStart;
    int relY = mWLY;

    uint16_t tileIdxAddr = tileMapOffset + relY / NUM_PIXELS_PER_TILE_COLUMN * NUM_TILES_PER_ROW + relX / NUM_PIXELS_PER_TILE_ROW;
    uint8_t tileIdx = mVRAM[tileIdxAddr];

    uint16_t bytePtr = mBgWinTiles || (tileIdx > TILE_BLOCK_0_THRESH) ? tileIdx * NUM_BYTES_PER_TILE
      : tileIdx * NUM_BYTES_PER_TILE + TILE_BLOCK_1_OFFSET;

    uint8_t tileBytePtr = (relY % NUM_PIXELS_PER_TILE_COLUMN) * NUM_BYTES_PER_X_ROW;

    int pixelsLeft = NUM_PIXELS_PER_TILE_ROW - (relX % NUM_PIXELS_PER_TILE_ROW);
    while (pixelsLeft > 0 && x < SCREEN_WIDTH)
    {
      uint8_t colorId = (((mVRAM[bytePtr + tileBytePtr + 1] >> (pixelsLeft - 1)) & 0b1) << 1) |
        ((mVRAM[bytePtr + tileBytePtr + 0] >> (pixelsLeft - 1)) & 0b1);

      mLineBuf[x] = colorId;

      x += 1;
      pixelsLeft -= 1;
    }
  }

  mWLY += 1;
}

void LcdController::RenderObjects()
{
  if (!mObjEnable)
    return;

  uint8_t validObjNum = 0;
  for (uint8_t objNum = 0; objNum < NUM_OBJECTS; objNum += 1)
  {
    uint8_t objY   = mOam[objNum * NUM_BYTES_PER_ATTRIBUTES + 0];
    uint8_t objX   = mOam[objNum * NUM_BYTES_PER_ATTRIBUTES + 1];
    uint8_t tileIdx = mOam[objNum * NUM_BYTES_PER_ATTRIBUTES + 2];
    uint8_t attr = mOam[objNum * NUM_BYTES_PER_ATTRIBUTES + 3];

    int globalY = objY - OBJECT_Y_OFFSET;
    int globalX = objX - OBJECT_X_OFFSET;

    uint8_t objHeight = mDoublesize ? NUM_PIXELS_PER_DOUBLE_TILE_COLUMN : NUM_PIXELS_PER_TILE_COLUMN;

    // Objects only unselected by y position, not x, even if off-screen
    if (globalY > mLY || globalY <= (mLY - objHeight))
      continue;

    mObjects[validObjNum] = {globalY, globalX, tileIdx, attr};
    validObjNum += 1;

    if (validObjNum == OBJECT_LIMIT)
      break;
  }

  if (validObjNum == 0)
    return;

  std::stable_sort(mObjects.begin(), mObjects.begin() + validObjNum,
                   [](const Object& a, const Object& b)
  {
    return a.x < b.x;
  });

  for (uint64_t objNum = 0; objNum < validObjNum; objNum++)
  {
    // Objects fully off-screen, don't bother rendering
    if (mObjects[objNum].x <= -8 || mObjects[objNum].x >= SCREEN_WIDTH)
      continue;

    bool flipY = mObjects[objNum].attr & GetAttributeBitMask(ObjectAttributeBitMask::FLIP_Y);
    bool flipX = mObjects[objNum].attr & GetAttributeBitMask(ObjectAttributeBitMask::FLIP_X);

    int bytePtr = mObjects[objNum].tileIdx * NUM_BYTES_PER_TILE;

    if (mDoublesize)
    {
      bool lowerTile = (mLY - mObjects[objNum].y) >= NUM_PIXELS_PER_TILE_COLUMN;

      int tileIdxUpper = flipY ? (mObjects[objNum].tileIdx | DOUBLESIZE_TILE_0_MASK) : (mObjects[objNum].tileIdx & DOUBLESIZE_TILE_1_MASK);
      int tileIdxLower = flipY ? tileIdxUpper - 1 : tileIdxUpper + 1;

      bytePtr = lowerTile ? tileIdxLower * NUM_BYTES_PER_TILE : tileIdxUpper * NUM_BYTES_PER_TILE;
    }

    int tileBytePtr = ((mLY - mObjects[objNum].y) % NUM_PIXELS_PER_TILE_COLUMN) * 2;
    if (flipY) tileBytePtr = 14 - tileBytePtr;

    int x = flipX ? mObjects[objNum].x + 7 : mObjects[objNum].x;
    int xIncr = flipX ? -1 : 1;

    int pixelsLeft = NUM_PIXELS_PER_TILE_ROW;

    while (pixelsLeft > 0)
    {
      if (x < 0 || x >= SCREEN_WIDTH)
      {
        x += xIncr;
        pixelsLeft -= 1;
        continue;
      }

      uint8_t colorId = (((mVRAM[bytePtr + tileBytePtr + 1] >> (pixelsLeft - 1)) & 0b1) << 1) |
        ((mVRAM[bytePtr + tileBytePtr + 0] >> (pixelsLeft - 1)) & 0b1);

      uint8_t prioMask = mObjects[objNum].attr & GetAttributeBitMask(ObjectAttributeBitMask::PRIORITY) ? 0b1111 : 0b1100;
      uint8_t palette  = mObjects[objNum].attr & GetAttributeBitMask(ObjectAttributeBitMask::PALLETE_SELECT) ? 0b1000 : 0b0100;

      if (colorId && (mLineBuf[x] & prioMask) == 0)
      {
        mLineBuf[x] = colorId | palette;
      }
      // else transparent (don't draw)

      x += xIncr;
      pixelsLeft -= 1;
    }
  }
}

void LcdController::PopulateFrameLine()
{
  auto x = std::views::iota(static_cast<size_t>(0), mLineBuf.size());

  std::for_each(std::execution::unseq, x.begin(), x.end(),
                [this](size_t x)
  {
    auto frameIdx = (mLY * SCREEN_WIDTH) + x;

    Pixel pixel{};
    switch (mLineBuf[x] >> 2)
    {
      case 0:
        pixel = mBgPalette[(mBGP >> ((mLineBuf[x] & 0b11) * BITS_PER_COLOR)) & 0b11];
        break;
      case 1:
        pixel = mObj0Palette[(mOBP0 >> ((mLineBuf[x] & 0b11) * BITS_PER_COLOR)) & 0b11];
        break;
      case 2:
        pixel = mObj1Palette[(mOBP1 >> ((mLineBuf[x] & 0b11) * BITS_PER_COLOR)) & 0b11];
        break;
      default:
        break;
    }

    mFrameBuf[frameIdx] = pixel;
  });
}

void LcdController::RenderScanline()
{
  RenderBackground();
  RenderWindow();
  RenderObjects();
  PopulateFrameLine();
}

bool LcdController::GetStatState() const
{
  bool lycState = (mLycIntSel) && (mLycEqLy);
  bool m0State  = (mMode0IntSel) && (mStatMode == 0);
  bool m1State  = (mMode1IntSel) && (mStatMode == 1);
  bool m2State  = (mMode2IntSel) && (mStatMode == 2);

  return lycState || m0State || m1State || m2State;
}

void LcdController::CheckStatInterrupt(StatBitMask mask)
{
  bool statState = GetStatState();

  if (!statState && (mSTAT & GetStatBitMask(mask)))
    TriggerInterrupt();
}

void LcdController::SetStatMode(Modes mode)
{
  mSTAT &= ~GetStatBitMask(StatBitMask::PPU_MODE);
  mStatMode = GetMode(mode);
  mSTAT |= mStatMode;
}

void LcdController::Mode1()
{
  if (!mDelayFrame)
  {
    if (mCb)
      mCb(mFrameBuf, mFrameTime);
  }
  else
  {
    mDelayFrame = false;
  }

  CheckStatInterrupt(StatBitMask::MODE1_INT_SELECT);
  TriggerInterrupt(InterruptBitMask::VBLANK);

  mWLY = 0;
  mFrameReady = true;
  mNextMode = Modes::MODE_2_OAM;
  mRemainingModeCycles = 4560;
}

void LcdController::Mode2()
{
  CheckStatInterrupt(StatBitMask::MODE2_INT_SELECT);

  mNextMode = Modes::MODE_3_DRAW;
  mRemainingModeCycles = 80;
}

void LcdController::Mode3()
{
  mNextMode = Modes::MODE_0_HBLANK;
  mRemainingModeCycles = 172; // TODO: Variable
}

void LcdController::Mode0()
{
  RenderScanline();
  CheckStatInterrupt(StatBitMask::MODE0_INT_SELECT);

  mNextMode = mLY == 143 ? Modes::MODE_1_VBLANK : Modes::MODE_2_OAM;
  mRemainingModeCycles = 204; // TODO: Variable
}

void LcdController::CheckLyc()
{
  if (mLY == mLYC)
  {
    CheckStatInterrupt(StatBitMask::LYC_INT_SELECT);
    SetLyc();
  }
  else
    ResetLyc();
}

void LcdController::SetLyc()
{
  mSTAT |= GetStatBitMask(StatBitMask::LYC_EQ_LY);
  mLycEqLy = true;
}

void LcdController::ResetLyc()
{
  mSTAT &= ~GetStatBitMask(StatBitMask::LYC_EQ_LY);
  mLycEqLy = false;
}

void LcdController::UpdateDisabled()
{
  mDisabledCycleCount += 1;

  if (mDisabledCycleCount != DISABLED_FRAME_CYCLES)
    return;

  mFrameBuf.fill(mBgPalette[0]);

  if (mCb)
    mCb(mFrameBuf, mFrameTime);

  mFrameReady = true;
}

void LcdController::UpdateLy()
{
  mFrameCycleCount += 1;
  mFrameCycleCount %= FRAME_CYCLES;

  if ((mFrameCycleCount % CYCLES_PER_ROW) != 0)
    return;

  mLY = mFrameCycleCount / CYCLES_PER_ROW;
  CheckLyc();
}

void LcdController::UpdateState()
{
  mRemainingModeCycles -= 1;

  if (mRemainingModeCycles > 0)
    return;

  mCurrMode = mNextMode;

  switch (mCurrMode)
  {
    case LcdController::Modes::MODE_2_OAM:
      Mode2();
      break;
    case LcdController::Modes::MODE_3_DRAW:
      Mode3();
      break;
    case LcdController::Modes::MODE_0_HBLANK:
      Mode0();
      break;
    case LcdController::Modes::MODE_1_VBLANK:
      Mode1();
      break;
    default:
      break;
  }

  SetStatMode(mCurrMode);
}