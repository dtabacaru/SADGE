#pragma once

#include "InterruptProvider.h"
#include <iostream>
#include <array>

struct Pixel
{
  uint8_t R{};
  uint8_t G{};
  uint8_t B{};
  uint8_t A{};
};

const static std::array<Pixel, 4> DEFAULT_COLOR_PALETTE{
  Pixel{0xFF, 0xFF, 0xFF, 0xFF},
  Pixel{0xFF, 0x84, 0x84, 0xFF},
  Pixel{0x94, 0x3A, 0x3A, 0xFF},
  Pixel{0x00, 0x00, 0x00, 0xFF}
};

struct Object
{
  int global_y;
  int global_x;
  uint8_t tile_index;
  uint8_t attributes;
  uint8_t object_num;
};

constexpr static auto SCREEN_WIDTH = 160;
constexpr static auto SCREEN_HEIGHT = 144;
constexpr static auto SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;

typedef void (*FrameCallback)(const std::array<Pixel, SCREEN_SIZE>& frame, double frame_time);

// TODO: https://github.com/AntonioND/giibiiadvance/blob/master/docs/TCAGBD.pdf
class LcdController : public InterruptProvider
{
public:
  constexpr static uint8_t DEFAULT_READ = 0xFF;

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

  enum class Modes : uint8_t
  {
    MODE_2_OAM = 2,
    MODE_3_DRAW = 3,
    MODE_0_HBLANK = 0,
    MODE_1_VBLANK = 1
  };

  inline void SetColorPalettes(const std::array<Pixel, 4>& bg,const std::array<Pixel, 4>& obj0, const std::array<Pixel, 4>& obj1)
  {
    m_bg_palette = bg;
    m_obj0_palette = obj0;
    m_obj1_palette = obj1;
  }

  void SetFrameCallback(FrameCallback frame_callback)
  {
    m_frame_callback = frame_callback;
  }

  inline constexpr uint8_t GetAttributeBitMask(ObjectAttributeBitMask bit_mask)
  {
    return static_cast<uint8_t>(bit_mask);
  }

  inline constexpr uint8_t GetStatBitMask(StatBitMask bit_mask) const
  {
    return static_cast<uint8_t>(bit_mask);
  }

  inline constexpr uint8_t GetLcdcBitMask(LcdcBitMask bit_mask) const
  {
    return static_cast<uint8_t>(bit_mask);
  }

  inline constexpr uint8_t GetMode(Modes mode)
  {
    return static_cast<uint8_t>(mode);
  }

  LcdController(InterruptReceiver& interrupt_receiver);

  ~LcdController();

  inline bool DmaRequested() const
  {
    return m_dma_requested;
  };

  inline uint16_t GetCurrentDmaAddress() const
  {
    return m_dma_start_address + m_dma_byte_offset;
  };

  inline void ServiceDma(uint8_t val)
  {
    m_oam[m_dma_byte_offset] = val;

    m_dma_byte_offset += 1;
    if (m_dma_byte_offset == OAM_SIZE)
      m_dma_requested = false;
  };

  inline bool Update(double frame_time)
  {
    m_frame_time = frame_time;
    UpdateDma();

    bool frame_ready = m_lcd_enabled ? UpdateState() : UpdateDisabled();

    return frame_ready;
  }

  uint8_t HandleRead(uint16_t address) const;
  void HandleWrite(uint16_t address, uint8_t val);

  std::array<Pixel, SCREEN_SIZE>& GetCurrentFrameBuffer()
  {
    return m_frame;
  }

private:

  inline void HandleLcdcWrite(uint8_t val)
  {
    if (!m_lcd_enabled && (val & GetLcdcBitMask(LcdcBitMask::ENABLE)))
    {
      CheckLyc();
    }

    m_lcdc = val;
    m_lcd_enabled = m_lcdc & GetLcdcBitMask(LcdcBitMask::ENABLE);

    if (!m_lcd_enabled)
    {
      SetStatMode(Modes::MODE_0_HBLANK);
      m_current_mode = Modes::MODE_0_HBLANK;
      m_next_mode = Modes::MODE_3_DRAW;
      m_mode_transition_cycles = 76;
      m_cycle_count = 4;
      m_wly = 0;
      m_ly = 0;
      m_delay_frame = true;
      m_disabled_cycle_count = 0;
    }
  }

  inline void HandleDmaWrite(uint8_t val)
  {
    m_dma = val;
    m_dma_start_address = m_dma * 0x100;
    m_dma_byte_offset = 0;
    m_dma_flag = true;
  }

  inline bool GetStatState() const
  {
    bool lyc_state    = (m_stat & GetStatBitMask(StatBitMask::LYC_INT_SELECT))   && ( m_stat & GetStatBitMask(StatBitMask::LYC_EQ_LY));
    bool mode_0_state = (m_stat & GetStatBitMask(StatBitMask::MODE0_INT_SELECT)) && ((m_stat & GetStatBitMask(StatBitMask::PPU_MODE)) == 0);
    bool mode_1_state = (m_stat & GetStatBitMask(StatBitMask::MODE1_INT_SELECT)) && ((m_stat & GetStatBitMask(StatBitMask::PPU_MODE)) == 1);
    bool mode_2_state = (m_stat & GetStatBitMask(StatBitMask::MODE2_INT_SELECT)) && ((m_stat & GetStatBitMask(StatBitMask::PPU_MODE)) == 2);

    return lyc_state || mode_0_state || mode_1_state || mode_2_state;
  }

  inline void UpdateDma()
  {
    if (m_dma_flag)
    {
      m_dma_requested = true;
      m_dma_flag = false;
    }
  }

  inline void PopulateFrameBuffer()
  {
    for (int x = 0; x < SCREEN_WIDTH; x += 1)
    {
      auto frame_index = (m_ly * SCREEN_WIDTH) + x;

      Pixel pixel{};
      switch (m_line[x] >> 2)
      {
        case 0:
          pixel = m_bg_palette[(m_bgp  >> ((m_line[x] & 0b11) * BITS_PER_COLOR)) & 0b11];
          break;
        case 1:
          pixel = m_obj0_palette[(m_obp0 >> ((m_line[x] & 0b11) * BITS_PER_COLOR)) & 0b11];
          break;
        case 2:
          pixel = m_obj1_palette[(m_obp1 >> ((m_line[x] & 0b11) * BITS_PER_COLOR)) & 0b11];
          break;
        default:
          break;
      }

      m_frame[frame_index] = pixel;
    }
  }

  inline void RenderScanline()
  {
    RenderBackground();
    RenderWindow();
    RenderObjects();
    PopulateFrameBuffer();
  }

  inline void CheckStatInterrupt(StatBitMask bit_mask)
  {
    bool stat_state = GetStatState();

    if (!stat_state && (m_stat & GetStatBitMask(bit_mask)))
      TriggerInterrupt();
  }

  inline void SetStatMode(Modes mode)
  {
    m_stat &= ~GetStatBitMask(StatBitMask::PPU_MODE);
    m_stat |= GetMode(mode);
  }

  inline bool ModeTransition(Modes new_mode) const
  {
    return m_current_mode != new_mode;
  }

  inline void Transition()
  {
    m_current_mode = m_next_mode;

    switch (m_current_mode)
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

    SetStatMode(m_current_mode);
  }

  inline void Mode1()
  {
    if (!m_delay_frame)
    {
      if(m_frame_callback)
        m_frame_callback(m_frame, m_frame_time);
    }
    else
    {
      m_delay_frame = false;
    } 

    CheckStatInterrupt(StatBitMask::MODE0_INT_SELECT);
    m_vblank_interrupt_queued = true;

    m_wly = 0;
    m_next_mode = Modes::MODE_2_OAM;
    m_mode_transition_cycles = 4560;
  }

  inline void Mode2()
  {
    CheckStatInterrupt(StatBitMask::MODE2_INT_SELECT);

    m_next_mode = Modes::MODE_3_DRAW;
    m_mode_transition_cycles = 80;
  }

  inline void Mode3()
  {
    m_next_mode = Modes::MODE_0_HBLANK;
    m_mode_transition_cycles = 172; // TODO: Variable
  }

  inline void Mode0()
  {
    RenderScanline();
    CheckStatInterrupt(StatBitMask::MODE0_INT_SELECT);

    m_next_mode = m_ly == 143 ? Modes::MODE_1_VBLANK : Modes::MODE_2_OAM;
    m_mode_transition_cycles = 204; // TODO: Variable
  }

  inline void CheckLyc()
  {
    if (m_ly == m_lyc)
    {
      CheckStatInterrupt(StatBitMask::LYC_INT_SELECT);
      SetLyc();
    }
    else
      ResetLyc();
  }

  inline void SetLyc()
  {
    m_stat |= GetStatBitMask(StatBitMask::LYC_EQ_LY);
  }

  inline void ResetLyc()
  {
    m_stat &= ~GetStatBitMask(StatBitMask::LYC_EQ_LY);
  }

  bool UpdateDisabled()
  {
    m_disabled_cycle_count += 4;

    if (m_disabled_cycle_count == 4560)
    {
      m_frame.fill(m_bg_palette[0]);

      if(m_frame_callback)
        m_frame_callback(m_frame, m_frame_time);

      return true;
    }

    return false;
  }
   
  inline bool UpdateState()
  {
    m_cycle_count += 4;
    m_mode_transition_cycles -= 4;

    if (m_cycle_count == FRAME_CYCLES)
      m_cycle_count = 0;

    // Line 153 sets m_ly to 0 after 4 cycles
    if (m_cycle_count == (FRAME_CYCLES - CYCLES_PER_ROW + 4))
    {
      m_ly = 0;
      CheckLyc();
    }
    else if ((m_cycle_count % CYCLES_PER_ROW) == 0)
    {
      m_ly = m_cycle_count / CYCLES_PER_ROW;
      CheckLyc();
    }

    if (m_vblank_interrupt_queued)
    {
      TriggerInterrupt(InterruptBitMask::VBLANK);
      m_vblank_interrupt_queued = false;
    }

    if (m_mode_transition_cycles == 0)
    {
      Transition();

      return m_current_mode == Modes::MODE_1_VBLANK;
    }

    return false;
  }

  void RenderBackground();
  void RenderObjects();
  void RenderWindow();

  FrameCallback m_frame_callback = nullptr;

  Modes m_current_mode = Modes::MODE_0_HBLANK;
  Modes m_next_mode    = Modes::MODE_3_DRAW;
  uint32_t m_mode_transition_cycles = 76;

  bool m_vblank_interrupt_queued = false;

  std::array<uint8_t, SCREEN_WIDTH> m_line{};
  std::array<Pixel, SCREEN_SIZE> m_frame{};
  std::array<uint8_t, OAM_SIZE> m_oam{};
  std::array<uint8_t, VRAM_SIZE> m_vram{};
  std::array<Object, OBJECT_LIMIT> m_objects{};

  uint8_t m_lcdc{};
  uint8_t m_stat{};
  uint8_t m_scy{};
  uint8_t m_scx{};
  uint8_t m_ly{};
  uint8_t m_lyc{};
  uint8_t m_dma{};
  uint8_t m_bgp{};
  uint8_t m_obp0{};
  uint8_t m_obp1{};
  uint8_t m_wy{};
  uint8_t m_wx{};
  
  bool     m_lcd_enabled{};
  double   m_frame_time = 0;
  uint32_t m_cycle_count = 4;
  uint32_t m_disabled_cycle_count = 0;
  bool     m_delay_frame = true;
  bool     m_frame_ready = false;
  uint8_t  m_wly = 0;

  uint16_t m_dma_start_address = 0;
  uint8_t m_dma_byte_offset = 0;
  bool m_dma_requested = false;
  bool m_dma_flag = false;

  std::array<Pixel, 4> m_bg_palette = DEFAULT_COLOR_PALETTE;
  std::array<Pixel, 4> m_obj0_palette = DEFAULT_COLOR_PALETTE;
  std::array<Pixel, 4> m_obj1_palette = DEFAULT_COLOR_PALETTE;
};