#include "LcdController.h"

#include "Constants.h"

#include <algorithm>
#include <execution>

LcdController::LcdController(InterruptReceiver& interrupt_receiver) :
	InterruptProvider(interrupt_receiver, InterruptBitMask::LCD)
{
}

void LcdController::SetColorPalettes(const std::array<Pixel, 4>& bg, const std::array<Pixel, 4>& obj0, const std::array<Pixel, 4>& obj1)
{
	m_bg_palette = bg;
	m_obj0_palette = obj0;
	m_obj1_palette = obj1;
}

void LcdController::SetFrameCallback(FrameCallback frame_callback)
{
	mCb = frame_callback;
}

bool LcdController::DmaRequested() const
{
	return mDmaReq;
};

uint16_t LcdController::GetCurrentDmaAddress() const
{
	return mDmaStart + mDmaPtr;
};

std::array<Pixel, SCREEN_SIZE> LcdController::GetCurrentFrameBuffer()
{
	return mFrameBuf;
}

void LcdController::ServiceDma(uint8_t val)
{
	mOam[mDmaPtr] = val;

	mDmaPtr += 1;
	if (mDmaPtr == OAM_SIZE)
		mDmaReq = false;
};

void LcdController::UpdateFrameTime(double frame_time)
{
	mFrameTime = frame_time;
}

bool LcdController::Update()
{
	UpdateDma();

	bool frame_ready = mEnabled ? UpdateState() : UpdateDisabled();

	return frame_ready;
}

void LcdController::RenderBackground()
{
	if (mBgWinEnable)
	{
		uint16_t tile_map_offset = mBgTileMap ? TILE_MAP_OFFSET_1 : TILE_MAP_OFFSET_0;

		uint8_t x = 0;
		while (x < SCREEN_WIDTH)
		{
			uint8_t global_y = mSCY + mLY;
			uint8_t global_x = mSCX + x;

			uint16_t tile_index_address = tile_map_offset + (global_y / NUM_PIXELS_PER_TILE_COLUMN) * NUM_TILES_PER_ROW + (global_x / NUM_PIXELS_PER_TILE_ROW);
			uint8_t tile_index = mVRAM[tile_index_address];

			uint16_t byte_offset = (mBgWinTiles || (tile_index > TILE_BLOCK_0_THRESH)) ? tile_index * NUM_BYTES_PER_TILE
				                                                                         : tile_index * NUM_BYTES_PER_TILE + TILE_BLOCK_1_OFFSET;

			uint8_t tile_byte_offset = (global_y % NUM_PIXELS_PER_TILE_COLUMN) * 2;

			int remaining_pixels = NUM_PIXELS_PER_TILE_ROW - (global_x % NUM_PIXELS_PER_TILE_ROW);
			while (remaining_pixels > 0 && x < SCREEN_WIDTH)
			{
				uint8_t color_id = (((mVRAM[byte_offset + tile_byte_offset + 1] >> (remaining_pixels - 1)) & 0b1) << 1) |
					                  ((mVRAM[byte_offset + tile_byte_offset + 0] >> (remaining_pixels - 1)) & 0b1);

				mLineBuf[x] = color_id;

				x += 1;
				remaining_pixels -= 1;
			}
		}
	}
	else
	{
		for (int x = 0; x < SCREEN_WIDTH; x += 1)
		{
			mLineBuf[x] = 0;
		}
	}
}

void LcdController::RenderWindow()
{
	if (mBgWinEnable && mWinEnable)
	{
		if (mLY < mWY || mWX >= (SCREEN_WIDTH + WINDOW_X_OFFSET) || mWY >= SCREEN_HEIGHT)
			return;

		uint16_t tile_map_offset = mLCDC & GetLcdcBitMask(LcdcBitMask::WIN_TILE_MAP) ? TILE_MAP_OFFSET_1 : TILE_MAP_OFFSET_0;
		
		int window_x_start = mWX - WINDOW_X_OFFSET;
		int x = window_x_start < 0 ? 0 : window_x_start;
		while (x < SCREEN_WIDTH)
		{
			int relative_x = x - window_x_start;
			int relative_y = mWLY;

			uint16_t tile_index_address = tile_map_offset + relative_y / NUM_PIXELS_PER_TILE_COLUMN * NUM_TILES_PER_ROW + relative_x / NUM_PIXELS_PER_TILE_ROW;
			uint8_t tile_index = mVRAM[tile_index_address];

			uint16_t byte_offset = mBgWinTiles || (tile_index > TILE_BLOCK_0_THRESH) ? tile_index * NUM_BYTES_PER_TILE 
				                                                                       : tile_index * NUM_BYTES_PER_TILE + TILE_BLOCK_1_OFFSET;

			uint8_t tile_byte_offset = (relative_y % NUM_PIXELS_PER_TILE_COLUMN) * NUM_BYTES_PER_X_ROW;

			int remaining_pixels = NUM_PIXELS_PER_TILE_ROW - (relative_x % NUM_PIXELS_PER_TILE_ROW);
			while (remaining_pixels > 0 && x < SCREEN_WIDTH)
			{
				uint8_t color_id = (((mVRAM[byte_offset + tile_byte_offset + 1] >> (remaining_pixels - 1)) & 0b1) << 1) |
					                  ((mVRAM[byte_offset + tile_byte_offset + 0] >> (remaining_pixels - 1)) & 0b1);

				mLineBuf[x] = color_id;

				x += 1;
				remaining_pixels -= 1;
			}
		}

		mWLY += 1;
	}
}

void LcdController::RenderObjects()
{
	if (mLCDC & GetLcdcBitMask(LcdcBitMask::OBJ_ENABLE))
	{
		bool doublesize = (mLCDC & GetLcdcBitMask(LcdcBitMask::OBJ_SIZE));

		uint8_t valid_object_num = 0;
		for (uint8_t object_num = 0; object_num < NUM_OBJECTS; object_num += 1)
		{
			uint8_t object_y   = mOam[object_num * NUM_BYTES_PER_ATTRIBUTES + 0];
			uint8_t object_x   = mOam[object_num * NUM_BYTES_PER_ATTRIBUTES + 1];
			uint8_t tile_index = mOam[object_num * NUM_BYTES_PER_ATTRIBUTES + 2];
			uint8_t attributes = mOam[object_num * NUM_BYTES_PER_ATTRIBUTES + 3];

			int global_y = object_y - OBJECT_Y_OFFSET;
			int global_x = object_x - OBJECT_X_OFFSET;

			uint8_t object_height = doublesize ? NUM_PIXELS_PER_DOUBLE_TILE_COLUMN : NUM_PIXELS_PER_TILE_COLUMN;

			// Objects only unselected by y position, not x, even if off-screen
			if (global_y > mLY || global_y <= (mLY - object_height))
				continue;

			mObjects[valid_object_num] = { global_y, global_x, tile_index, attributes, object_num };

			valid_object_num += 1;

			if (valid_object_num == OBJECT_LIMIT)
				break;
		}

		if (valid_object_num == 0)
			return;

		std::stable_sort(mObjects.begin(), mObjects.begin() + valid_object_num,
		[](const Object& a, const Object& b)
		{
			return a.x < b.x;
		});

		for (uint64_t object_num = 0; object_num < valid_object_num; object_num++)
		{
			// Objects fully off-screen, don't bother rendering
			if (mObjects[object_num].x <= -8 || mObjects[object_num].x >= SCREEN_WIDTH)
				continue;

			bool flip_y = mObjects[object_num].attr & GetAttributeBitMask(ObjectAttributeBitMask::FLIP_Y);
			bool flip_x = mObjects[object_num].attr & GetAttributeBitMask(ObjectAttributeBitMask::FLIP_X);

			int byte_offset = mObjects[object_num].tileIdx * NUM_BYTES_PER_TILE;
			
			if (doublesize)
			{
				bool lower_tile = (mLY - mObjects[object_num].y) >= NUM_PIXELS_PER_TILE_COLUMN;

				int tile_index_upper = flip_y ? (mObjects[object_num].tileIdx | DOUBLESIZE_TILE_0_MASK) : (mObjects[object_num].tileIdx & DOUBLESIZE_TILE_1_MASK);
				int tile_index_lower = flip_y ? tile_index_upper - 1 : tile_index_upper + 1;

				byte_offset = lower_tile ? tile_index_lower * NUM_BYTES_PER_TILE : tile_index_upper * NUM_BYTES_PER_TILE;
			}
			
			int tile_byte_offset = ((mLY - mObjects[object_num].y) % NUM_PIXELS_PER_TILE_COLUMN) * 2;
			if (flip_y) tile_byte_offset = 14 - tile_byte_offset;		

			int x = flip_x ? mObjects[object_num].x + 7 : mObjects[object_num].x;
			int x_increment = flip_x ? -1 : 1;

			int remaining_pixels = NUM_PIXELS_PER_TILE_ROW;

			while (remaining_pixels > 0)
			{
				if (x < 0 || x >= SCREEN_WIDTH)
				{
					x += x_increment;
					remaining_pixels -= 1;
					continue;
				}

				uint8_t color_id = (((mVRAM[byte_offset + tile_byte_offset + 1] >> (remaining_pixels - 1)) & 0b1) << 1) |
					                  ((mVRAM[byte_offset + tile_byte_offset + 0] >> (remaining_pixels - 1)) & 0b1);

				uint8_t prio_mask = mObjects[object_num].attr & GetAttributeBitMask(ObjectAttributeBitMask::PRIORITY)       ? 0b1111 : 0b1100;
				uint8_t pallete   = mObjects[object_num].attr & GetAttributeBitMask(ObjectAttributeBitMask::PALLETE_SELECT) ? 0b1000 : 0b0100;

				if (color_id && (mLineBuf[x] & prio_mask) == 0)
				{
					mLineBuf[x] = color_id | pallete;
				}
				// else transparent (don't draw)

				x += x_increment;
				remaining_pixels -= 1;
			}
		}
	}
}

uint8_t LcdController::HandleRead(uint16_t addr) const
{
	if (addr >= 0x8000 && addr <= 0x9FFF)
	{
		addr -= 0x8000;
		return mVRAM[addr];
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F)
	{
		addr -= 0xFE00;
		return mOam[addr];
	}

	Address lcd_address = static_cast<Address>(addr);

	switch (lcd_address)
	{
		case LcdController::Address::CONTROL:
			return mLCDC;
		case LcdController::Address::STATUS:
			return mSTAT;
		case LcdController::Address::SCREEN_Y:
			return mSCY;
		case LcdController::Address::SCREEN_X:
			return mSCX;
		case LcdController::Address::LCD_Y:
			return mLY;
		case LcdController::Address::LCD_Y_COMP:
			return mLYC;
		case LcdController::Address::DMA:
			return mDMA;
		case LcdController::Address::BG_PALETTE:
			return mBGP;
		case LcdController::Address::OBJ_PALETTE_0:
			return mOBP0;
		case LcdController::Address::OBJ_PALETTE_1:
			return mOBP1;
		case LcdController::Address::WINDOW_Y:
			return mWY;
		case LcdController::Address::WINDOW_X:
			return mWX;
		default:
			return DEFAULT_READ;
	}
}

void LcdController::HandleWrite(uint16_t addr, uint8_t val)
{
  if (addr >= 0x8000 && addr <= 0x9FFF)
	{
		addr -= 0x8000;
		mVRAM[addr] = val;
		return;
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F)
	{
		addr -= 0xFE00;
		mOam[addr] = val;
		return;
	}

	Address lcd_address = static_cast<Address>(addr);

	switch (lcd_address)
	{
		case LcdController::Address::CONTROL:
			HandleLcdcWrite(val);
			break;
		case LcdController::Address::STATUS:
			mSTAT = val;
			break;
		case LcdController::Address::SCREEN_Y:
			mSCY = val;
			break;
		case LcdController::Address::SCREEN_X:
			mSCX = val;
			break;
		case LcdController::Address::LCD_Y:
			break;
		case LcdController::Address::LCD_Y_COMP:
			mLYC = val;
			CheckLyc();
			break;
		case LcdController::Address::DMA:
			HandleDmaWrite(val);
			break;
		case LcdController::Address::BG_PALETTE:
			mBGP = val;
			break;
		case LcdController::Address::OBJ_PALETTE_0:
			mOBP0 = val;
			break;
		case LcdController::Address::OBJ_PALETTE_1:
			mOBP1 = val;
			break;
		case LcdController::Address::WINDOW_Y:
			mWY = val;
			break;
		case LcdController::Address::WINDOW_X:
			mWX = val;
			break;
		default:
			(void)val;
			break;
	}
}

void LcdController::HandleLcdcWrite(uint8_t val)
{
	if (!mEnabled && (val & GetLcdcBitMask(LcdcBitMask::ENABLE)))
	{
		CheckLyc();
	}

	mLCDC = val;
	mEnabled = mLCDC & GetLcdcBitMask(LcdcBitMask::ENABLE);
	mBgWinEnable = mLCDC & GetLcdcBitMask(LcdcBitMask::BG_WIN_ENABLE);
	mWinEnable = mLCDC & GetLcdcBitMask(LcdcBitMask::WIN_ENABLE);
	mBgTileMap = mLCDC & GetLcdcBitMask(LcdcBitMask::BG_TILE_MAP);
	mBgWinTiles = mLCDC& GetLcdcBitMask(LcdcBitMask::BG_WIN_TILES);

	if (!mEnabled)
	{
		SetStatMode(Modes::MODE_0_HBLANK);
		mCurrMode = Modes::MODE_0_HBLANK;
		mNextMode = Modes::MODE_3_DRAW;
		mRemainingModeCycles = 80;
		mFrameCycleCount = 0;
		mWLY = 0;
		mLY = 0;
		mDelayFrame = true;
		mDisabledCycleCount = 0;
	}
}

void LcdController::HandleDmaWrite(uint8_t val)
{
	mDMA = val;
	mDmaStart = mDMA * 0x100;
	mDmaPtr = 0;
	mEnableNext = true;
}

bool LcdController::GetStatState() const
{
	bool lyc_state    = (mSTAT & GetStatBitMask(StatBitMask::LYC_INT_SELECT)) && (mSTAT & GetStatBitMask(StatBitMask::LYC_EQ_LY));
	bool mode_0_state = (mSTAT & GetStatBitMask(StatBitMask::MODE0_INT_SELECT)) && ((mSTAT & GetStatBitMask(StatBitMask::PPU_MODE)) == 0);
	bool mode_1_state = (mSTAT & GetStatBitMask(StatBitMask::MODE1_INT_SELECT)) && ((mSTAT & GetStatBitMask(StatBitMask::PPU_MODE)) == 1);
	bool mode_2_state = (mSTAT & GetStatBitMask(StatBitMask::MODE2_INT_SELECT)) && ((mSTAT & GetStatBitMask(StatBitMask::PPU_MODE)) == 2);

	return lyc_state || mode_0_state || mode_1_state || mode_2_state;
}

void LcdController::UpdateDma()
{
	if (mEnableNext)
	{
		mDmaReq = true;
		mEnableNext = false;
	}
}

void LcdController::PopulateFrameLine()
{
	for (int x = 0; x < SCREEN_WIDTH; x += 1)
	{
		auto frame_index = (mLY * SCREEN_WIDTH) + x;

		Pixel pixel{};
		switch (mLineBuf[x] >> 2)
		{
			case 0:
				pixel = m_bg_palette[(mPixelBgp[x] >> ((mLineBuf[x] & 0b11) * BITS_PER_COLOR)) & 0b11];
				break;
			case 1:
				pixel = m_obj0_palette[(mOBP0 >> ((mLineBuf[x] & 0b11) * BITS_PER_COLOR)) & 0b11];
				break;
			case 2:
				pixel = m_obj1_palette[(mOBP1 >> ((mLineBuf[x] & 0b11) * BITS_PER_COLOR)) & 0b11];
				break;
			default:
				break;
		}

		mFrameBuf[frame_index] = pixel;
	}
}

void LcdController::RenderScanline()
{
	RenderBackground();
	RenderWindow();
	RenderObjects();
	PopulateFrameLine();
}

void LcdController::CheckStatInterrupt(StatBitMask mask)
{
	bool stat_state = GetStatState();

	if (!stat_state && (mSTAT & GetStatBitMask(mask)))
		TriggerInterrupt();
}

void LcdController::SetStatMode(Modes mode)
{
	mSTAT &= ~GetStatBitMask(StatBitMask::PPU_MODE);
	mSTAT |= GetMode(mode);
}

bool LcdController::ModeTransition(Modes new_mode) const
{
	return mCurrMode != new_mode;
}

void LcdController::Transition()
{
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

	CheckStatInterrupt(StatBitMask::MODE0_INT_SELECT);
	TriggerInterrupt(InterruptBitMask::VBLANK);

	mWLY = 0;
	mNextMode = Modes::MODE_2_OAM;
	mRemainingModeCycles = 4560;
}

void LcdController::Mode2()
{
	CheckStatInterrupt(StatBitMask::MODE2_INT_SELECT);

	mNextMode = Modes::MODE_3_DRAW;
	mRemainingModeCycles = 80;
}

std::array<uint8_t, SCREEN_WIDTH> mPixelBgp;

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
}

void LcdController::ResetLyc()
{
	mSTAT &= ~GetStatBitMask(StatBitMask::LYC_EQ_LY);
}

bool LcdController::UpdateDisabled()
{
	mDisabledCycleCount += 4;

	if (mDisabledCycleCount == 4560)
	{
		mFrameBuf.fill(m_bg_palette[0]);

		if (mCb)
			mCb(mFrameBuf, mFrameTime);

		return true;
	}

	return false;
}

bool LcdController::UpdateState()
{
	mFrameCycleCount += 4;
	mRemainingModeCycles -= 4;

	if (mFrameCycleCount == FRAME_CYCLES)
		mFrameCycleCount = 0;

	// Line 153 sets m_ly to 0 after 4 cycles
	//if (m_cycle_count == (FRAME_CYCLES - CYCLES_PER_ROW + 4))
	//{
	//  m_ly = 0;
	//  CheckLyc();
	//}
	//else if ((m_cycle_count % CYCLES_PER_ROW) == 0)
	if ((mFrameCycleCount % CYCLES_PER_ROW) == 0)
	{
		mLY = mFrameCycleCount / CYCLES_PER_ROW;
		CheckLyc();
	}

	if ((mCurrMode == Modes::MODE_3_DRAW) && (mRemainingModeCycles <= 160) && (mRemainingModeCycles > 0))
	{
		uint8_t pixel = (160 - mRemainingModeCycles);
		mPixelBgp[pixel] = mBGP;
		mPixelBgp[pixel + 1] = mBGP;
		mPixelBgp[pixel + 2] = mBGP;
		mPixelBgp[pixel + 3] = mBGP;
	}

	if (mRemainingModeCycles == 0)
	{
		Transition();

		return mCurrMode == Modes::MODE_1_VBLANK;
	}

	return false;
}