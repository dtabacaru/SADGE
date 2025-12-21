#include "LcdController.h"

#include <algorithm>
#include <execution>

LcdController::LcdController(InterruptReceiver& interrupt_receiver) : InterruptProvider(interrupt_receiver, InterruptBitMask::LCD)
{

}

LcdController::~LcdController()
{
	
}

void LcdController::RenderBackground()
{
	if (m_lcdc & GetLcdcBitMask(LcdcBitMask::BG_WIN_ENABLE))
	{
		uint16_t tile_map_offset = (m_lcdc & GetLcdcBitMask(LcdcBitMask::BG_TILE_MAP)) ? TILE_MAP_OFFSET_1 : TILE_MAP_OFFSET_0;

		uint8_t x = 0;
		while (x < SCREEN_WIDTH)
		{
			uint8_t global_y = m_scy + m_ly;
			uint8_t global_x = m_scx + x;

			uint16_t tile_index_address = tile_map_offset + (global_y / NUM_PIXELS_PER_TILE_COLUMN) * NUM_TILES_PER_ROW + (global_x / NUM_PIXELS_PER_TILE_ROW);
			uint8_t tile_index = m_vram[tile_index_address];

			bool tile_map_0_flag = m_lcdc & GetLcdcBitMask(LcdcBitMask::BG_WIN_TILES);
			uint16_t byte_offset = (tile_map_0_flag || (tile_index > TILE_BLOCK_0_THRESH)) ? tile_index * NUM_BYTES_PER_TILE
				                                                                             : tile_index * NUM_BYTES_PER_TILE + TILE_BLOCK_1_OFFSET;

			uint8_t tile_byte_offset = (global_y % NUM_PIXELS_PER_TILE_COLUMN) * 2;

			int remaining_pixels = NUM_PIXELS_PER_TILE_ROW - (global_x % NUM_PIXELS_PER_TILE_ROW);
			while (remaining_pixels > 0 && x < SCREEN_WIDTH)
			{
				uint8_t color_id = (((m_vram[byte_offset + tile_byte_offset + 1] >> (remaining_pixels - 1)) & 0b1) << 1) |
					                  ((m_vram[byte_offset + tile_byte_offset + 0] >> (remaining_pixels - 1)) & 0b1);

				m_line[x] = color_id;

				x += 1;
				remaining_pixels -= 1;
			}
		}
	}
	else
	{
		for (int x = 0; x < SCREEN_WIDTH; x += 1)
		{
			m_line[x] = 0;
		}
	}
}

void LcdController::RenderWindow()
{
	if (((m_lcdc & GetLcdcBitMask(LcdcBitMask::BG_WIN_ENABLE)) && (m_lcdc & GetLcdcBitMask(LcdcBitMask::WIN_ENABLE))))
	{
		if (m_ly < m_wy || m_wx >= (SCREEN_WIDTH + WINDOW_X_OFFSET) || m_wy >= SCREEN_HEIGHT)
			return;

		uint16_t tile_map_offset = m_lcdc & GetLcdcBitMask(LcdcBitMask::WIN_TILE_MAP) ? TILE_MAP_OFFSET_1 : TILE_MAP_OFFSET_0;
		
		int window_x_start = m_wx - WINDOW_X_OFFSET;
		int x = window_x_start < 0 ? 0 : window_x_start;
		while (x < SCREEN_WIDTH)
		{
			int relative_x = x - window_x_start;
			int relative_y = m_wly;

			uint16_t tile_index_address = tile_map_offset + relative_y / NUM_PIXELS_PER_TILE_COLUMN * NUM_TILES_PER_ROW + relative_x / NUM_PIXELS_PER_TILE_ROW;
			uint8_t tile_index = m_vram[tile_index_address];

			bool tile_map_0_flag = m_lcdc & GetLcdcBitMask(LcdcBitMask::BG_WIN_TILES);
			uint16_t byte_offset = tile_map_0_flag || (tile_index > TILE_BLOCK_0_THRESH) ? tile_index * NUM_BYTES_PER_TILE 
				                                                                           : tile_index * NUM_BYTES_PER_TILE + TILE_BLOCK_1_OFFSET;

			uint8_t tile_byte_offset = (relative_y % NUM_PIXELS_PER_TILE_COLUMN) * NUM_BYTES_PER_X_ROW;

			int remaining_pixels = NUM_PIXELS_PER_TILE_ROW - (relative_x % NUM_PIXELS_PER_TILE_ROW);
			while (remaining_pixels > 0 && x < SCREEN_WIDTH)
			{
				uint8_t color_id = (((m_vram[byte_offset + tile_byte_offset + 1] >> (remaining_pixels - 1)) & 0b1) << 1) |
					                  ((m_vram[byte_offset + tile_byte_offset + 0] >> (remaining_pixels - 1)) & 0b1);

				m_line[x] = color_id;

				x += 1;
				remaining_pixels -= 1;
			}
		}

		m_wly += 1;
	}
}

void LcdController::RenderObjects()
{
	if (m_lcdc & GetLcdcBitMask(LcdcBitMask::OBJ_ENABLE))
	{
		m_objects.clear();

		bool doublesize = (m_lcdc & GetLcdcBitMask(LcdcBitMask::OBJ_SIZE));

		for (uint8_t object_num = 0; object_num < NUM_OBJECTS; object_num += 1)
		{
			uint8_t object_y   = m_oam[object_num * NUM_BYTES_PER_ATTRIBUTES + 0];
			uint8_t object_x   = m_oam[object_num * NUM_BYTES_PER_ATTRIBUTES + 1];
			uint8_t tile_index = m_oam[object_num * NUM_BYTES_PER_ATTRIBUTES + 2];
			uint8_t attributes = m_oam[object_num * NUM_BYTES_PER_ATTRIBUTES + 3];

			int global_y = object_y - OBJECT_Y_OFFSET;
			int global_x = object_x - OBJECT_X_OFFSET;

			uint8_t object_height = doublesize ? NUM_PIXELS_PER_DOUBLE_TILE_COLUMN : NUM_PIXELS_PER_TILE_COLUMN;

			// Objects only unselected by y position, not x, even if off-screen
			if (global_y > m_ly || global_y <= (m_ly - object_height))
				continue;

			m_objects.push_back({global_y, global_x, tile_index, attributes, object_num});

			if (m_objects.size() == OBJECT_LIMIT)
				break;
		}

		if (m_objects.size() == 0)
			return;

		std::stable_sort(m_objects.begin(), m_objects.end(),
		[](const Object& a, const Object& b)
		{
			return a.global_x < b.global_x;
		});

		for (uint64_t object_num = 0; object_num < m_objects.size(); object_num++)
		{
			// Objects fully off-screen, don't bother rendering
			if (m_objects[object_num].global_x <= -8 || m_objects[object_num].global_x >= SCREEN_WIDTH)
				continue;

			bool flip_y = m_objects[object_num].attributes & GetAttributeBitMask(ObjectAttributeBitMask::FLIP_Y);
			bool flip_x = m_objects[object_num].attributes & GetAttributeBitMask(ObjectAttributeBitMask::FLIP_X);

			int byte_offset = m_objects[object_num].tile_index * NUM_BYTES_PER_TILE;
			
			if (doublesize)
			{
				bool lower_tile = (m_ly - m_objects[object_num].global_y) >= NUM_PIXELS_PER_TILE_COLUMN;

				int tile_index_upper = flip_y ? (m_objects[object_num].tile_index | DOUBLESIZE_TILE_0_MASK) : (m_objects[object_num].tile_index & DOUBLESIZE_TILE_1_MASK);
				int tile_index_lower = flip_y ? tile_index_upper - 1 : tile_index_upper + 1;

				byte_offset = lower_tile ? tile_index_lower * NUM_BYTES_PER_TILE : tile_index_upper * NUM_BYTES_PER_TILE;
			}
			
			int tile_byte_offset = ((m_ly - m_objects[object_num].global_y) % NUM_PIXELS_PER_TILE_COLUMN) * 2;
			if (flip_y) tile_byte_offset = 14 - tile_byte_offset;		

			int x = flip_x ? m_objects[object_num].global_x + 7 : m_objects[object_num].global_x;
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

				uint8_t color_id = (((m_vram[byte_offset + tile_byte_offset + 1] >> (remaining_pixels - 1)) & 0b1) << 1) |
					                  ((m_vram[byte_offset + tile_byte_offset + 0] >> (remaining_pixels - 1)) & 0b1);

				uint8_t prio_mask = m_objects[object_num].attributes & GetAttributeBitMask(ObjectAttributeBitMask::PRIORITY)       ? 0b1111 : 0b1100;
				uint8_t pallete   = m_objects[object_num].attributes & GetAttributeBitMask(ObjectAttributeBitMask::PALLETE_SELECT) ? 0b1000 : 0b0100;

				if (color_id && (m_line[x] & prio_mask) == 0)
				{
					m_line[x] = color_id | pallete;
				}
				// else transparent (don't draw)

				x += x_increment;
				remaining_pixels -= 1;
			}
		}
	}
}

uint8_t LcdController::HandleRead(uint16_t address) const
{
	if (address >= 0x8000 && address <= 0x9FFF)
	{
		address -= 0x8000;
		return m_vram[address];
	}
	else if (address >= 0xFE00 && address <= 0xFE9F)
	{
		address -= 0xFE00;
		return m_oam[address];
	}

	Address lcd_address = static_cast<Address>(address);

	switch (lcd_address)
	{
		case LcdController::Address::CONTROL:
			return m_lcdc;
		case LcdController::Address::STATUS:
			return m_stat;
		case LcdController::Address::SCREEN_Y:
			return m_scy;
		case LcdController::Address::SCREEN_X:
			return m_scx;
		case LcdController::Address::LCD_Y:
			return m_ly;
		case LcdController::Address::LCD_Y_COMP:
			return m_lyc;
		case LcdController::Address::DMA:
			return m_dma;
		case LcdController::Address::BG_PALETTE:
			return m_bgp;
		case LcdController::Address::OBJ_PALETTE_0:
			return m_obp0;
		case LcdController::Address::OBJ_PALETTE_1:
			return m_obp1;
		case LcdController::Address::WINDOW_Y:
			return m_wy;
		case LcdController::Address::WINDOW_X:
			return m_wx;
		default:
			return DEFAULT_READ;
	}
}

void LcdController::HandleWrite(uint16_t address, uint8_t val)
{
  if (address >= 0x8000 && address <= 0x9FFF)
	{
		address -= 0x8000;
		m_vram[address] = val;
		return;
	}
	else if (address >= 0xFE00 && address <= 0xFE9F)
	{
		address -= 0xFE00;
		m_oam[address] = val;
		return;
	}

	Address lcd_address = static_cast<Address>(address);

	switch (lcd_address)
	{
		case LcdController::Address::CONTROL:
			HandleLcdcWrite(val);
			break;
		case LcdController::Address::STATUS:
			m_stat = val;
			break;
		case LcdController::Address::SCREEN_Y:
			m_scy = val;
			break;
		case LcdController::Address::SCREEN_X:
			m_scx = val;
			break;
		case LcdController::Address::LCD_Y:
			break;
		case LcdController::Address::LCD_Y_COMP:
			m_lyc = val;
			CheckLyc();
			break;
		case LcdController::Address::DMA:
			HandleDmaWrite(val);
			break;
		case LcdController::Address::BG_PALETTE:
			m_bgp = val;
			break;
		case LcdController::Address::OBJ_PALETTE_0:
			m_obp0 = val;
			break;
		case LcdController::Address::OBJ_PALETTE_1:
			m_obp1 = val;
			break;
		case LcdController::Address::WINDOW_Y:
			m_wy = val;
			break;
		case LcdController::Address::WINDOW_X:
			m_wx = val;
			break;
		default:
			(void)val;
			break;
	}
}
