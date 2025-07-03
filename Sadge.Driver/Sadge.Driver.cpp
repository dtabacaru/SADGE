#include "Cpu.h"

#include "raylib.h"

constexpr static float JOYSTICK_THRESH = 0.55f;

// Grayscale
constexpr static std::array<Color, 4> COLOR_PALETTE{
  Color{255, 255, 255, 255}, // 00 - White
  Color{170, 170, 170, 255}, // 01 - Light Gray
  Color{ 85,  85,  85, 255}, // 10 - Dark Gray
  Color{  0,   0,   0, 255}  // 11 - Black
};

// Cool colors
//constexpr static std::array<Color, 4> COLOR_PALETTE{
//  Color{235, 255, 255, 255},
//  Color{170, 170, 190, 255},
//  Color{ 85, 105,  85, 255},
//  Color{ 20,   0,   0, 255}
//};

// DMG
//constexpr static std::array<Color, 4> COLOR_PALETTE{
//	Color{232, 252, 204, 255}, // 00 - White
//	Color{172, 212, 144, 255}, // 01 - Light Gray
//	Color{ 84, 140, 112, 255}, // 10 - Dark Gray
//	Color{ 20,  44,  56, 255}  // 11 - Black
//};

Cpu game_boy_cpu;

bool joystick_down = false;
bool joystick_up = false;
bool joystick_left = false;
bool joystick_right = false;

static Image image_buffer;
static Texture2D texture_buffer;
static std::vector<Color> frame_buffer = std::vector<Color>(SCREEN_WIDTH * SCREEN_HEIGHT, COLOR_PALETTE[0]);
std::deque<double> m_fps_buffer;

std::vector<short> audio_buffer;
std::mutex audio_buffer_lock;
std::atomic<bool> stream_playing = false;
AudioStream audio_stream;

auto render_scale = 5;
Vector2 window_pos{};

void CheckButtons()
{
	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT))
	{
		game_boy_cpu.GetJoypadController().PressActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_MIDDLE_RIGHT))
	{
		game_boy_cpu.GetJoypadController().ReleaseActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT))
	{
		game_boy_cpu.GetJoypadController().PressActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_MIDDLE_LEFT))
	{
		game_boy_cpu.GetJoypadController().ReleaseActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
	{
		game_boy_cpu.GetJoypadController().PressActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
	{
		game_boy_cpu.GetJoypadController().ReleaseActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
	{
		game_boy_cpu.GetJoypadController().PressActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
	{
		game_boy_cpu.GetJoypadController().ReleaseActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}

	if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > JOYSTICK_THRESH) && !joystick_down)
	{
		joystick_down = true;
		game_boy_cpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}
	else if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) < JOYSTICK_THRESH) && joystick_down)
	{
		joystick_down = false;
		game_boy_cpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}

	if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) < -JOYSTICK_THRESH) && !joystick_up)
	{
		joystick_up = true;
		game_boy_cpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}
	else if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > -JOYSTICK_THRESH) && joystick_up)
	{
		joystick_up = false;
		game_boy_cpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}

	if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) > JOYSTICK_THRESH) && !joystick_right)
	{
		joystick_right = true;
		game_boy_cpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}
	else if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) < JOYSTICK_THRESH) && joystick_right)
	{
		joystick_right = false;
		game_boy_cpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}

	if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) < -JOYSTICK_THRESH) && !joystick_left)
	{
		joystick_left = true;
		game_boy_cpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}
	else if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) > -JOYSTICK_THRESH) && joystick_left)
	{
		joystick_left = false;
		game_boy_cpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
	{
		game_boy_cpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
	{
		game_boy_cpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
	{
		game_boy_cpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
	{
		game_boy_cpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
	{
		game_boy_cpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
	{
		game_boy_cpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
	{
		game_boy_cpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
	{
		game_boy_cpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}
}

void ApplyPallete(const std::vector<uint8_t>& frame)
{
	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i += 1)
	{
		frame_buffer[i] = COLOR_PALETTE[frame[i]];
	}
}

void DrawDoubleFPS(double frame_time, int pos_x, int pos_y)
{
	if (m_fps_buffer.size() < 30)
		m_fps_buffer.push_back(1 / frame_time);
	else
	{
		m_fps_buffer.pop_front();
		m_fps_buffer.push_back(1 / frame_time);

		double fps = std::accumulate(m_fps_buffer.begin(), m_fps_buffer.end(), 0.0) / 30.0;

		DrawText(TextFormat("%.4f FPS", fps), pos_x, pos_y, 20, LIME);
	}
}

void DrawFrame(const std::vector<uint8_t>& frame, double frame_time)
{
	if (WindowShouldClose())
	{
		UnloadTexture(texture_buffer);
		game_boy_cpu.Stop();
		CloseWindow();
		return;
	}

	CheckButtons();
	ApplyPallete(frame);

	UpdateTexture(texture_buffer, frame_buffer.data());
	BeginDrawing();
	ClearBackground(BLACK);
	DrawTextureEx(texture_buffer, window_pos, 0.0f, static_cast<float>(render_scale), WHITE);
	DrawDoubleFPS(frame_time, static_cast<int>(10 + window_pos.x), static_cast<int>(10 + window_pos.y));
	EndDrawing();
}

void AudioInputCallback(void* buffer, unsigned int frames)
{
	std::lock_guard<std::mutex> lock(audio_buffer_lock);

	short* short_buffer = (short*)buffer;

	auto num_frames = audio_buffer.size() > frames ? frames : audio_buffer.size();
	for (unsigned int i = 0; i < num_frames; i++)
		short_buffer[i] = audio_buffer[i];

	if(num_frames > 0)
		audio_buffer.erase(audio_buffer.begin(), audio_buffer.begin() + num_frames);
}

void PlayAudio(std::vector<short>& subsample_buffer)
{
	std::lock_guard<std::mutex> lock(audio_buffer_lock);

	audio_buffer.insert(audio_buffer.end(), subsample_buffer.begin(), subsample_buffer.end());

	if (!stream_playing)
	{
		PlayAudioStream(audio_stream);
		stream_playing = true;
	}
}

int main()
{
  SetTraceLogLevel(LOG_NONE);

	window_pos.x = 0;
	window_pos.y = 0;

	int window_height = SCREEN_HEIGHT * render_scale;
	int window_width = SCREEN_WIDTH * render_scale;

	InitWindow(window_width, window_height, "GameBoy");

	image_buffer = {
		.data = frame_buffer.data(),
		.width = SCREEN_WIDTH,
		.height = SCREEN_HEIGHT,
		.mipmaps = 1,
		.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
	};

	texture_buffer = LoadTextureFromImage(image_buffer);

	InitAudioDevice();
	SetAudioStreamBufferSizeDefault(375);
	audio_stream = LoadAudioStream(AUDIO_FREQUENCY, AUDIO_BITS, AUDIO_CHANNELS);
	SetAudioStreamCallback(audio_stream, AudioInputCallback);

  game_boy_cpu.InsertRom("D:\\Games\\Emulation\\Roms\\No-Intro 20190501\\Nintendo - Game Boy\\Tetris Attack (USA) (SGB Enhanced).gb");
  game_boy_cpu.GetLcdController().SetFrameCallback(DrawFrame);
	game_boy_cpu.GetAudioController().SetAudioCallback(PlayAudio);

  game_boy_cpu.Run();

	StopAudioStream(audio_stream);
	CloseAudioDevice();
}
