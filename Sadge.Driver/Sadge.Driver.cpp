#include "Cpu.h"

#include "raylib.h"

Cpu game_boy_cpu;

constexpr auto SUCCESS = 0;
constexpr auto ERROR = 1;

#pragma region JOYPAD
constexpr static float JOYSTICK_THRESH = 0.5f;

bool joystick_down = false;
bool joystick_up = false;
bool joystick_left = false;
bool joystick_right = false;

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


#pragma endregion

#pragma region AUDIO
std::vector<short> audio_buffer;
std::mutex audio_buffer_lock;
std::atomic<bool> stream_playing = false;
AudioStream audio_stream;

void AudioInputCallback(void* buffer, unsigned int frames)
{
	std::lock_guard<std::mutex> lock(audio_buffer_lock);

	short* short_buffer = (short*)buffer;

	auto num_frames = audio_buffer.size() > frames ? frames : audio_buffer.size();
	for (unsigned int i = 0; i < num_frames; i++)
		short_buffer[i] = audio_buffer[i];

	if (num_frames > 0)
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
#pragma endregion

#pragma region DISPLAY

constexpr auto WINDOW_TITLE = "Sadge.Driver";

constexpr auto RENDER_SCALE = 5;

constexpr auto  FPS_BUFFER_SIZE = 32;
constexpr auto  FPS_FONT_SIZE = 20;
constexpr Color FPS_FONT_COLOR = LIME;
constexpr auto  FPS_WINDOW_OFFSET = 10;
constexpr auto  FPS_F = "%.4f FPS";

Texture2D          texture_buffer;
std::vector<Color> frame_buffer = std::vector<Color>(SCREEN_WIDTH * SCREEN_HEIGHT, Color{232, 252, 204, 255});
Vector2            window_pos{0.0, 0.0};

std::deque<double> fps_buffer;

void ToColorFrame(const std::vector<Pixel>& frame)
{
	std::memcpy(frame_buffer.data(), frame.data(), sizeof(Color) * frame_buffer.size());
}

void DrawDoubleFPS(double frame_time, int pos_x, int pos_y)
{
	if (fps_buffer.size() < FPS_BUFFER_SIZE)
	{
		fps_buffer.push_back(1 / frame_time);
		return;
	}

	fps_buffer.pop_front();
	fps_buffer.push_back(1 / frame_time);

	double fps = std::accumulate(fps_buffer.begin(), fps_buffer.end(), 0.0) / FPS_BUFFER_SIZE;

	DrawText(TextFormat(FPS_F, fps), pos_x, pos_y, FPS_FONT_SIZE, FPS_FONT_COLOR);
}

void DrawFrame(const std::vector<Pixel>& frame, double frame_time)
{
	if (WindowShouldClose())
	{
		game_boy_cpu.Stop();
		UnloadTexture(texture_buffer);
		CloseWindow();
		return;
	}

	CheckButtons();
	ToColorFrame(frame);

	UpdateTexture(texture_buffer, frame_buffer.data());
	BeginDrawing();
	ClearBackground(BLACK);
	DrawTextureEx(texture_buffer, window_pos, 0.0f, static_cast<float>(RENDER_SCALE), WHITE);
	DrawDoubleFPS(frame_time, static_cast<int>(FPS_WINDOW_OFFSET + window_pos.x), static_cast<int>(FPS_WINDOW_OFFSET + window_pos.y));
	EndDrawing();
}
#pragma endregion

int main(int num_args, char* args[])
{
	if (num_args != 2)
	{
		std::cout << "Expected ROM as input." << std::endl;
		return ERROR;
	}

	std::filesystem::path rom_path;

	try
	{
		rom_path = args[1];

		if (!std::filesystem::exists(rom_path))
		{
			std::cout << "Could not locate ROM" << std::endl;
			return ERROR;
		}

		Status status = game_boy_cpu.InsertRom(rom_path);

		if (!status.Valid())
		{
			std::cout << status.Msg() << std::endl;
			return ERROR;
		}

		SetTraceLogLevel(LOG_NONE);

		window_pos.x = 0;
		window_pos.y = 0;

		int window_height = SCREEN_HEIGHT * RENDER_SCALE;
		int window_width  = SCREEN_WIDTH  * RENDER_SCALE;

		InitWindow(window_width, window_height, WINDOW_TITLE);

		Image icon = LoadImage("Sadge.png");
		SetWindowIcon(icon);
		UnloadImage(icon);

		Image image_buffer;

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

		game_boy_cpu.GetLcdController().SetFrameCallback(DrawFrame);
		game_boy_cpu.GetAudioController().SetAudioCallback(PlayAudio);

		game_boy_cpu.Run();

		StopAudioStream(audio_stream);
		CloseAudioDevice();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
		return ERROR;
	}

	return SUCCESS;
}
