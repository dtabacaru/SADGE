#include "Cpu.h"
#include "HighResTimerWrapper.h"

#include "raylib.h"

static Cpu gameboyCpu;

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
#pragma region ACTION
	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT))
	{
		gameboyCpu.GetJoypadController().PressActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_MIDDLE_RIGHT))
	{
		gameboyCpu.GetJoypadController().ReleaseActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT))
	{
		gameboyCpu.GetJoypadController().PressActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_MIDDLE_LEFT))
	{
		gameboyCpu.GetJoypadController().ReleaseActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
	{
		gameboyCpu.GetJoypadController().PressActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
	{
		gameboyCpu.GetJoypadController().ReleaseActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
	{
		gameboyCpu.GetJoypadController().PressActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
	{
		gameboyCpu.GetJoypadController().ReleaseActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}

	if (IsKeyDown(KEY_ENTER))
	{
		gameboyCpu.GetJoypadController().PressActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}
	else if (IsKeyReleased(KEY_ENTER))
	{
		gameboyCpu.GetJoypadController().ReleaseActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}

	if (IsKeyDown(KEY_RIGHT_SHIFT))
	{
		gameboyCpu.GetJoypadController().PressActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}
	else if (IsKeyReleased(KEY_RIGHT_SHIFT))
	{
		gameboyCpu.GetJoypadController().ReleaseActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}

	if (IsKeyDown(KEY_K))
	{
		gameboyCpu.GetJoypadController().PressActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}
	else if (IsKeyReleased(KEY_K))
	{
		gameboyCpu.GetJoypadController().ReleaseActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}

	if (IsKeyDown(KEY_J))
	{
		gameboyCpu.GetJoypadController().PressActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}
	else if (IsKeyReleased(KEY_J))
	{
		gameboyCpu.GetJoypadController().ReleaseActionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}
#pragma endregion

#pragma region DIRECTION

	if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > JOYSTICK_THRESH) && !joystick_down)
	{
		joystick_down = true;
		gameboyCpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}
	else if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) < JOYSTICK_THRESH) && joystick_down)
	{
		joystick_down = false;
		gameboyCpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}

	if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) < -JOYSTICK_THRESH) && !joystick_up)
	{
		joystick_up = true;
		gameboyCpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}
	else if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > -JOYSTICK_THRESH) && joystick_up)
	{
		joystick_up = false;
		gameboyCpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}

	if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) > JOYSTICK_THRESH) && !joystick_right)
	{
		joystick_right = true;
		gameboyCpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}
	else if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) < JOYSTICK_THRESH) && joystick_right)
	{
		joystick_right = false;
		gameboyCpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}

	if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) < -JOYSTICK_THRESH) && !joystick_left)
	{
		joystick_left = true;
		gameboyCpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}
	else if ((GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) > -JOYSTICK_THRESH) && joystick_left)
	{
		joystick_left = false;
		gameboyCpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
	{
		gameboyCpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
	{
		gameboyCpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
	{
		gameboyCpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
	{
		gameboyCpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
	{
		gameboyCpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
	{
		gameboyCpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}

	if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
	{
		gameboyCpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}
	else if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
	{
		gameboyCpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}

	if (IsKeyDown(KEY_S))
	{
		gameboyCpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}
	else if (IsKeyReleased(KEY_S))
	{
		gameboyCpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_START_DOWN);
	}

	if (IsKeyDown(KEY_W))
	{
		gameboyCpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}
	else if (IsKeyReleased(KEY_W))
	{
		gameboyCpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_SELECT_UP);
	}

	if (IsKeyDown(KEY_D))
	{
		gameboyCpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}
	else if (IsKeyReleased(KEY_D))
	{
		gameboyCpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_A_RIGHT);
	}

	if (IsKeyDown(KEY_A))
	{
		gameboyCpu.GetJoypadController().PressDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}
	else if (IsKeyReleased(KEY_A))
	{
		gameboyCpu.GetJoypadController().ReleaseDirectionButton(JoypadController::JoypadButtonBitMask::BUTTONS_B_LEFT);
	}

#pragma endregion
}

#pragma endregion

#pragma region AUDIO
constexpr static uint32_t AUDIO_STREAM_BUF_SIZE = NUM_SUB_SAMPLE / 6;

std::vector<short> audioBuf;
std::mutex audioBufLock;
std::atomic<bool> streamPlaying = false;
AudioStream stream;

void AudioInputCallback(void* buffer, unsigned int frames)
{
	std::lock_guard<std::mutex> lock(audioBufLock);

	short* short_buffer = reinterpret_cast<short*>(buffer);

	auto num_frames = (audioBuf.size() / 2) > frames ? frames : (audioBuf.size() / 2);
	for (uint64_t frame_num = 0; frame_num < num_frames * 2; frame_num++)
		short_buffer[frame_num] = audioBuf[frame_num];

	if (num_frames > 0)
		audioBuf.erase(audioBuf.begin(), audioBuf.begin() + num_frames * 2);
}

void PlayAudio(std::array<short, NUM_SUB_SAMPLE>& subsample_buffer)
{
	std::lock_guard<std::mutex> lock(audioBufLock);

	audioBuf.insert(audioBuf.end(), subsample_buffer.begin(), subsample_buffer.end());

	if (!streamPlaying)
	{
		PlayAudioStream(stream);
		streamPlaying = true;
	}
}
#pragma endregion

#pragma region DISPLAY

constexpr auto WINDOW_TITLE = "Sadge.Driver";

constexpr auto RENDER_SCALE = 5;

constexpr auto FPS_BUFFER_SIZE = 32;
constexpr auto FPS_FONT_SIZE = 20;
constexpr auto FPS_FONT_COLOR = LIME;
constexpr auto FPS_WINDOW_OFFSET = 10;
constexpr auto FPS_F = "%.4f FPS";

Texture2D          texture_buffer;
Vector2            window_pos{0.0, 0.0};

std::deque<double> fpsBuff;

void DrawDoubleFPS(double frame_time, int pos_x, int pos_y)
{
	if (fpsBuff.size() < FPS_BUFFER_SIZE)
	{
		fpsBuff.push_back(1 / frame_time);
		return;
	}

	fpsBuff.pop_front();
	fpsBuff.push_back(1 / frame_time);

	double fps = std::accumulate(fpsBuff.begin(), fpsBuff.end(), 0.0) / FPS_BUFFER_SIZE;

	DrawText(TextFormat(FPS_F, fps), pos_x, pos_y, FPS_FONT_SIZE, FPS_FONT_COLOR);
}

void DrawFrame(const std::array<Pixel, 160*144>& frame, double frame_time)
{
	if (WindowShouldClose())
	{
		gameboyCpu.Stop();
		UnloadTexture(texture_buffer);
		CloseWindow();
		return;
	}

	CheckButtons();

	UpdateTexture(texture_buffer, frame.data());
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

	HighResTimerWrapper highResTimer;

	try
	{
		rom_path = args[1];

		if (!std::filesystem::exists(rom_path))
		{
			std::cout << "Could not locate ROM" << std::endl;
			return ERROR;
		}

		Status status = gameboyCpu.InsertRom(rom_path);

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

		Image texInit = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
		texture_buffer = LoadTextureFromImage(texInit);
		UnloadImage(texInit);

		InitAudioDevice();

		SetAudioStreamBufferSizeDefault(AUDIO_STREAM_BUF_SIZE);
		stream = LoadAudioStream(A_RATE, AUDIO_BITS, AUDIO_CHANNELS);
		SetAudioStreamCallback(stream, AudioInputCallback);

		gameboyCpu.GetLcdController().SetFrameCallback(DrawFrame);
		gameboyCpu.GetAudioController().SetAudioCallback(PlayAudio);

		gameboyCpu.Run();

		StopAudioStream(stream);
		CloseAudioDevice();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
		return ERROR;
	}

	return SUCCESS;
}
