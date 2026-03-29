#include "Cpu.h"

void Cpu::TickEdge(EdgeType& edge, LevelType& level)
{
  edge = edge == EdgeType::FALLING ? EdgeType::RISING : EdgeType::FALLING;
  level = edge == EdgeType::FALLING ? LevelType::LOW : LevelType::HIGH;
}

void Cpu::TEvent()
{
  mTEdgeCount += 1;
}

void Cpu::FallingTEvent()
{
  mTLowCount += 1;
}

void Cpu::RisingTEvent()
{
  mTHighCount += 1;
}

void Cpu::MEvent()
{
  mMEdgeCount += 1;

  mAudioCtrl.TickCh3();
  mAudioCtrl.Tick();
}

void Cpu::FallingMEvent()
{
  mMLowCount += 1;
}

void Cpu::RisingMEvent()
{
  mMHighCount += 1;
  mMFrameCycles += 1;

  mTimerCtrl.Tick();
  mAudioCtrl.UpdateClk(mTimerCtrl.GetClk());

  TickExecution();
  
  if (mLcdCtrl.DmaRequested())
  {
    mAddressBus = mLcdCtrl.GetCurrentDmaAddress();
    Read();
    mLcdCtrl.ServiceDma(mDataBus);
  }

  mAudioCtrl.TickCh1();
  mAudioCtrl.TickCh2();
  mAudioCtrl.TickCh4();

  bool frame_ready = mLcdCtrl.Update() || mMFrameCycles == DEFAULT_FRAME_M_CYCLES;

  if (frame_ready)
    WaitFrame();
}

void Cpu::Main()
{
  TEvent();

  TickEdge(mTEdge, mTLevel);
  mTEdge == EdgeType::RISING ? RisingTEvent() : FallingTEvent();

  if (mTEdgeCount % M_DIV == 0)
  {
    MEvent();

    TickEdge(mMEdge, mMLevel);
    mMEdge == EdgeType::RISING ? RisingMEvent() : FallingMEvent();
  }
}

void Cpu::InterruptCompleteEvent()
{
  mExecutionMode = ExecutionMode::INSTRUCTION;
  InstructionCompleteEvent();
}

void Cpu::InstructionCompleteEvent()
{
  Fetch();

  mInterruptCtrl.Update(mImeRequest);
  mImeRequest = false;

  if (mInterruptCtrl.InterruptRequested())
    mExecutionMode = ExecutionMode::INTERRUPT;
}

void Cpu::ExtInstructionCompleteEvent()
{
  mExecutionMode = ExecutionMode::INSTRUCTION;
  InstructionCompleteEvent();
}

void Cpu::HaltCompleteEvent()
{
  mExecutionMode = ExecutionMode::INSTRUCTION;
  mInterruptCtrl.Update(mImeRequest);
  mImeRequest = false;

  if (mInterruptCtrl.InterruptRequested())
    mExecutionMode = ExecutionMode::INTERRUPT;
}

void Cpu::WaitFrame()
{
  double expectedFrameTime = (mMFrameCycles / M_RATE) - mCompensationTime;
  double waitTime = expectedFrameTime - mExeStopwatch.Elapsed();

  if(waitTime > MIN_SLEEP_TIME)
    std::this_thread::sleep_for(std::chrono::duration<double>(waitTime - MIN_SLEEP_TIME));

  // Busy wait loop the last MIN_SLEEP_TIME seconds
  while (mExeStopwatch.Elapsed() < expectedFrameTime) {}

  mFrameTime = mExeStopwatch.Elapsed();
  mExeStopwatch.Restart();

  if (waitTime < -DRAG_WINDOW_DETECT_TIME)
  {
    mCompensationTime = 0;
    mAudioCtrl.ClearSamples();
  }

  mCompensationTime = mFrameTime - expectedFrameTime;
  mLcdCtrl.UpdateFrameTime(mFrameTime);
  
  mMFrameCycles = 0;
}

void Cpu::ReadNextUint8()
{
  mAddressBus = mPC.HL;
  Read();
  mPC.HL += 1;
}

void Cpu::InstructionHandler()
{
  switch (mOpcode)
  {
    case 0x00:
      Op0x00(); break;
    case 0x01:
      Op0x01(); break;
    case 0x02:
      Op0x02(); break;
    case 0x03:
      Op0x03(); break;
    case 0x04:
      Op0x04(); break;
    case 0x05:
      Op0x05(); break;
    case 0x06:
      Op0x06(); break;
    case 0x07:
      Op0x07(); break;
    case 0x08:
      Op0x08(); break;
    case 0x09:
      Op0x09(); break;
    case 0x0A:
      Op0x0A(); break;
    case 0x0B:
      Op0x0B(); break;
    case 0x0C:
      Op0x0C(); break;
    case 0x0D:
      Op0x0D(); break;
    case 0x0E:
      Op0x0E(); break;
    case 0x0F:
      Op0x0F(); break;
    case 0x10:
      Op0x10(); break;
    case 0x11:
      Op0x11(); break;
    case 0x12:
      Op0x12(); break;
    case 0x13:
      Op0x13(); break;
    case 0x14:
      Op0x14(); break;
    case 0x15:
      Op0x15(); break;
    case 0x16:
      Op0x16(); break;
    case 0x17:
      Op0x17(); break;
    case 0x18:
      Op0x18(); break;
    case 0x19:
      Op0x19(); break;
    case 0x1A:
      Op0x1A(); break;
    case 0x1B:
      Op0x1B(); break;
    case 0x1C:
      Op0x1C(); break;
    case 0x1D:
      Op0x1D(); break;
    case 0x1E:
      Op0x1E(); break;
    case 0x1F:
      Op0x1F(); break;
    case 0x20:
      Op0x20(); break;
    case 0x21:
      Op0x21(); break;
    case 0x22:
      Op0x22(); break;
    case 0x23:
      Op0x23(); break;
    case 0x24:
      Op0x24(); break;
    case 0x25:
      Op0x25(); break;
    case 0x26:
      Op0x26(); break;
    case 0x27:
      Op0x27(); break;
    case 0x28:
      Op0x28(); break;
    case 0x29:
      Op0x29(); break;
    case 0x2A:
      Op0x2A(); break;
    case 0x2B:
      Op0x2B(); break;
    case 0x2C:
      Op0x2C(); break;
    case 0x2D:
      Op0x2D(); break;
    case 0x2E:
      Op0x2E(); break;
    case 0x2F:
      Op0x2F(); break;
    case 0x30:
      Op0x30(); break;
    case 0x31:
      Op0x31(); break;
    case 0x32:
      Op0x32(); break;
    case 0x33:
      Op0x33(); break;
    case 0x34:
      Op0x34(); break;
    case 0x35:
      Op0x35(); break;
    case 0x36:
      Op0x36(); break;
    case 0x37:
      Op0x37(); break;
    case 0x38:
      Op0x38(); break;
    case 0x39:
      Op0x39(); break;
    case 0x3A:
      Op0x3A(); break;
    case 0x3B:
      Op0x3B(); break;
    case 0x3C:
      Op0x3C(); break;
    case 0x3D:
      Op0x3D(); break;
    case 0x3E:
      Op0x3E(); break;
    case 0x3F:
      Op0x3F(); break;
    case 0x40:
      Op0x40(); break;
    case 0x41:
      Op0x41(); break;
    case 0x42:
      Op0x42(); break;
    case 0x43:
      Op0x43(); break;
    case 0x44:
      Op0x44(); break;
    case 0x45:
      Op0x45(); break;
    case 0x46:
      Op0x46(); break;
    case 0x47:
      Op0x47(); break;
    case 0x48:
      Op0x48(); break;
    case 0x49:
      Op0x49(); break;
    case 0x4A:
      Op0x4A(); break;
    case 0x4B:
      Op0x4B(); break;
    case 0x4C:
      Op0x4C(); break;
    case 0x4D:
      Op0x4D(); break;
    case 0x4E:
      Op0x4E(); break;
    case 0x4F:
      Op0x4F(); break;
    case 0x50:
      Op0x50(); break;
    case 0x51:
      Op0x51(); break;
    case 0x52:
      Op0x52(); break;
    case 0x53:
      Op0x53(); break;
    case 0x54:
      Op0x54(); break;
    case 0x55:
      Op0x55(); break;
    case 0x56:
      Op0x56(); break;
    case 0x57:
      Op0x57(); break;
    case 0x58:
      Op0x58(); break;
    case 0x59:
      Op0x59(); break;
    case 0x5A:
      Op0x5A(); break;
    case 0x5B:
      Op0x5B(); break;
    case 0x5C:
      Op0x5C(); break;
    case 0x5D:
      Op0x5D(); break;
    case 0x5E:
      Op0x5E(); break;
    case 0x5F:
      Op0x5F(); break;
    case 0x60:
      Op0x60(); break;
    case 0x61:
      Op0x61(); break;
    case 0x62:
      Op0x62(); break;
    case 0x63:
      Op0x63(); break;
    case 0x64:
      Op0x64(); break;
    case 0x65:
      Op0x65(); break;
    case 0x66:
      Op0x66(); break;
    case 0x67:
      Op0x67(); break;
    case 0x68:
      Op0x68(); break;
    case 0x69:
      Op0x69(); break;
    case 0x6A:
      Op0x6A(); break;
    case 0x6B:
      Op0x6B(); break;
    case 0x6C:
      Op0x6C(); break;
    case 0x6D:
      Op0x6D(); break;
    case 0x6E:
      Op0x6E(); break;
    case 0x6F:
      Op0x6F(); break;
    case 0x70:
      Op0x70(); break;
    case 0x71:
      Op0x71(); break;
    case 0x72:
      Op0x72(); break;
    case 0x73:
      Op0x73(); break;
    case 0x74:
      Op0x74(); break;
    case 0x75:
      Op0x75(); break;
    case 0x76:
      Op0x76(); break;
    case 0x77:
      Op0x77(); break;
    case 0x78:
      Op0x78(); break;
    case 0x79:
      Op0x79(); break;
    case 0x7A:
      Op0x7A(); break;
    case 0x7B:
      Op0x7B(); break;
    case 0x7C:
      Op0x7C(); break;
    case 0x7D:
      Op0x7D(); break;
    case 0x7E:
      Op0x7E(); break;
    case 0x7F:
      Op0x7F(); break;
    case 0x80:
      Op0x80(); break;
    case 0x81:
      Op0x81(); break;
    case 0x82:
      Op0x82(); break;
    case 0x83:
      Op0x83(); break;
    case 0x84:
      Op0x84(); break;
    case 0x85:
      Op0x85(); break;
    case 0x86:
      Op0x86(); break;
    case 0x87:
      Op0x87(); break;
    case 0x88:
      Op0x88(); break;
    case 0x89:
      Op0x89(); break;
    case 0x8A:
      Op0x8A(); break;
    case 0x8B:
      Op0x8B(); break;
    case 0x8C:
      Op0x8C(); break;
    case 0x8D:
      Op0x8D(); break;
    case 0x8E:
      Op0x8E(); break;
    case 0x8F:
      Op0x8F(); break;
    case 0x90:
      Op0x90(); break;
    case 0x91:
      Op0x91(); break;
    case 0x92:
      Op0x92(); break;
    case 0x93:
      Op0x93(); break;
    case 0x94:
      Op0x94(); break;
    case 0x95:
      Op0x95(); break;
    case 0x96:
      Op0x96(); break;
    case 0x97:
      Op0x97(); break;
    case 0x98:
      Op0x98(); break;
    case 0x99:
      Op0x99(); break;
    case 0x9A:
      Op0x9A(); break;
    case 0x9B:
      Op0x9B(); break;
    case 0x9C:
      Op0x9C(); break;
    case 0x9D:
      Op0x9D(); break;
    case 0x9E:
      Op0x9E(); break;
    case 0x9F:
      Op0x9F(); break;
    case 0xA0:
      Op0xA0(); break;
    case 0xA1:
      Op0xA1(); break;
    case 0xA2:
      Op0xA2(); break;
    case 0xA3:
      Op0xA3(); break;
    case 0xA4:
      Op0xA4(); break;
    case 0xA5:
      Op0xA5(); break;
    case 0xA6:
      Op0xA6(); break;
    case 0xA7:
      Op0xA7(); break;
    case 0xA8:
      Op0xA8(); break;
    case 0xA9:
      Op0xA9(); break;
    case 0xAA:
      Op0xAA(); break;
    case 0xAB:
      Op0xAB(); break;
    case 0xAC:
      Op0xAC(); break;
    case 0xAD:
      Op0xAD(); break;
    case 0xAE:
      Op0xAE(); break;
    case 0xAF:
      Op0xAF(); break;
    case 0xB0:
      Op0xB0(); break;
    case 0xB1:
      Op0xB1(); break;
    case 0xB2:
      Op0xB2(); break;
    case 0xB3:
      Op0xB3(); break;
    case 0xB4:
      Op0xB4(); break;
    case 0xB5:
      Op0xB5(); break;
    case 0xB6:
      Op0xB6(); break;
    case 0xB7:
      Op0xB7(); break;
    case 0xB8:
      Op0xB8(); break;
    case 0xB9:
      Op0xB9(); break;
    case 0xBA:
      Op0xBA(); break;
    case 0xBB:
      Op0xBB(); break;
    case 0xBC:
      Op0xBC(); break;
    case 0xBD:
      Op0xBD(); break;
    case 0xBE:
      Op0xBE(); break;
    case 0xBF:
      Op0xBF(); break;
    case 0xC0:
      Op0xC0(); break;
    case 0xC1:
      Op0xC1(); break;
    case 0xC2:
      Op0xC2(); break;
    case 0xC3:
      Op0xC3(); break;
    case 0xC4:
      Op0xC4(); break;
    case 0xC5:
      Op0xC5(); break;
    case 0xC6:
      Op0xC6(); break;
    case 0xC7:
      Op0xC7(); break;
    case 0xC8:
      Op0xC8(); break;
    case 0xC9:
      Op0xC9(); break;
    case 0xCA:
      Op0xCA(); break;
    case 0xCB:
      Op0xCB(); break;
    case 0xCC:
      Op0xCC(); break;
    case 0xCD:
      Op0xCD(); break;
    case 0xCE:
      Op0xCE(); break;
    case 0xCF:
      Op0xCF(); break;
    case 0xD0:
      Op0xD0(); break;
    case 0xD1:
      Op0xD1(); break;
    case 0xD2:
      Op0xD2(); break;
    case 0xD3:
      Op0xD3(); break;
    case 0xD4:
      Op0xD4(); break;
    case 0xD5:
      Op0xD5(); break;
    case 0xD6:
      Op0xD6(); break;
    case 0xD7:
      Op0xD7(); break;
    case 0xD8:
      Op0xD8(); break;
    case 0xD9:
      Op0xD9(); break;
    case 0xDA:
      Op0xDA(); break;
    case 0xDB:
      Op0xDB(); break;
    case 0xDC:
      Op0xDC(); break;
    case 0xDD:
      Op0xDD(); break;
    case 0xDE:
      Op0xDE(); break;
    case 0xDF:
      Op0xDF(); break;
    case 0xE0:
      Op0xE0(); break;
    case 0xE1:
      Op0xE1(); break;
    case 0xE2:
      Op0xE2(); break;
    case 0xE3:
      Op0xE3(); break;
    case 0xE4:
      Op0xE4(); break;
    case 0xE5:
      Op0xE5(); break;
    case 0xE6:
      Op0xE6(); break;
    case 0xE7:
      Op0xE7(); break;
    case 0xE8:
      Op0xE8(); break;
    case 0xE9:
      Op0xE9(); break;
    case 0xEA:
      Op0xEA(); break;
    case 0xEB:
      Op0xEB(); break;
    case 0xEC:
      Op0xEC(); break;
    case 0xED:
      Op0xED(); break;
    case 0xEE:
      Op0xEE(); break;
    case 0xEF:
      Op0xEF(); break;
    case 0xF0:
      Op0xF0(); break;
    case 0xF1:
      Op0xF1(); break;
    case 0xF2:
      Op0xF2(); break;
    case 0xF3:
      Op0xF3(); break;
    case 0xF4:
      Op0xF4(); break;
    case 0xF5:
      Op0xF5(); break;
    case 0xF6:
      Op0xF6(); break;
    case 0xF7:
      Op0xF7(); break;
    case 0xF8:
      Op0xF8(); break;
    case 0xF9:
      Op0xF9(); break;
    case 0xFA:
      Op0xFA(); break;
    case 0xFB:
      Op0xFB(); break;
    case 0xFC:
      Op0xFC(); break;
    case 0xFD:
      Op0xFD(); break;
    case 0xFE:
      Op0xFE(); break;
    case 0xFF:
      Op0xFF(); break;
    default:
      break;
  }

  if (mExeCycle == EXE_COMPLETE)
    InstructionCompleteEvent();
}

void Cpu::ExtInstructionHandler()
{
  switch (mOpcode)
  {
    case 0x00:
      OpCb0x00(); break;
    case 0x01:
      OpCb0x01(); break;
    case 0x02:
      OpCb0x02(); break;
    case 0x03:
      OpCb0x03(); break;
    case 0x04:
      OpCb0x04(); break;
    case 0x05:
      OpCb0x05(); break;
    case 0x06:
      OpCb0x06(); break;
    case 0x07:
      OpCb0x07(); break;
    case 0x08:
      OpCb0x08(); break;
    case 0x09:
      OpCb0x09(); break;
    case 0x0A:
      OpCb0x0A(); break;
    case 0x0B:
      OpCb0x0B(); break;
    case 0x0C:
      OpCb0x0C(); break;
    case 0x0D:
      OpCb0x0D(); break;
    case 0x0E:
      OpCb0x0E(); break;
    case 0x0F:
      OpCb0x0F(); break;
    case 0x10:
      OpCb0x10(); break;
    case 0x11:
      OpCb0x11(); break;
    case 0x12:
      OpCb0x12(); break;
    case 0x13:
      OpCb0x13(); break;
    case 0x14:
      OpCb0x14(); break;
    case 0x15:
      OpCb0x15(); break;
    case 0x16:
      OpCb0x16(); break;
    case 0x17:
      OpCb0x17(); break;
    case 0x18:
      OpCb0x18(); break;
    case 0x19:
      OpCb0x19(); break;
    case 0x1A:
      OpCb0x1A(); break;
    case 0x1B:
      OpCb0x1B(); break;
    case 0x1C:
      OpCb0x1C(); break;
    case 0x1D:
      OpCb0x1D(); break;
    case 0x1E:
      OpCb0x1E(); break;
    case 0x1F:
      OpCb0x1F(); break;
    case 0x20:
      OpCb0x20(); break;
    case 0x21:
      OpCb0x21(); break;
    case 0x22:
      OpCb0x22(); break;
    case 0x23:
      OpCb0x23(); break;
    case 0x24:
      OpCb0x24(); break;
    case 0x25:
      OpCb0x25(); break;
    case 0x26:
      OpCb0x26(); break;
    case 0x27:
      OpCb0x27(); break;
    case 0x28:
      OpCb0x28(); break;
    case 0x29:
      OpCb0x29(); break;
    case 0x2A:
      OpCb0x2A(); break;
    case 0x2B:
      OpCb0x2B(); break;
    case 0x2C:
      OpCb0x2C(); break;
    case 0x2D:
      OpCb0x2D(); break;
    case 0x2E:
      OpCb0x2E(); break;
    case 0x2F:
      OpCb0x2F(); break;
    case 0x30:
      OpCb0x30(); break;
    case 0x31:
      OpCb0x31(); break;
    case 0x32:
      OpCb0x32(); break;
    case 0x33:
      OpCb0x33(); break;
    case 0x34:
      OpCb0x34(); break;
    case 0x35:
      OpCb0x35(); break;
    case 0x36:
      OpCb0x36(); break;
    case 0x37:
      OpCb0x37(); break;
    case 0x38:
      OpCb0x38(); break;
    case 0x39:
      OpCb0x39(); break;
    case 0x3A:
      OpCb0x3A(); break;
    case 0x3B:
      OpCb0x3B(); break;
    case 0x3C:
      OpCb0x3C(); break;
    case 0x3D:
      OpCb0x3D(); break;
    case 0x3E:
      OpCb0x3E(); break;
    case 0x3F:
      OpCb0x3F(); break;
    case 0x40:
      OpCb0x40(); break;
    case 0x41:
      OpCb0x41(); break;
    case 0x42:
      OpCb0x42(); break;
    case 0x43:
      OpCb0x43(); break;
    case 0x44:
      OpCb0x44(); break;
    case 0x45:
      OpCb0x45(); break;
    case 0x46:
      OpCb0x46(); break;
    case 0x47:
      OpCb0x47(); break;
    case 0x48:
      OpCb0x48(); break;
    case 0x49:
      OpCb0x49(); break;
    case 0x4A:
      OpCb0x4A(); break;
    case 0x4B:
      OpCb0x4B(); break;
    case 0x4C:
      OpCb0x4C(); break;
    case 0x4D:
      OpCb0x4D(); break;
    case 0x4E:
      OpCb0x4E(); break;
    case 0x4F:
      OpCb0x4F(); break;
    case 0x50:
      OpCb0x50(); break;
    case 0x51:
      OpCb0x51(); break;
    case 0x52:
      OpCb0x52(); break;
    case 0x53:
      OpCb0x53(); break;
    case 0x54:
      OpCb0x54(); break;
    case 0x55:
      OpCb0x55(); break;
    case 0x56:
      OpCb0x56(); break;
    case 0x57:
      OpCb0x57(); break;
    case 0x58:
      OpCb0x58(); break;
    case 0x59:
      OpCb0x59(); break;
    case 0x5A:
      OpCb0x5A(); break;
    case 0x5B:
      OpCb0x5B(); break;
    case 0x5C:
      OpCb0x5C(); break;
    case 0x5D:
      OpCb0x5D(); break;
    case 0x5E:
      OpCb0x5E(); break;
    case 0x5F:
      OpCb0x5F(); break;
    case 0x60:
      OpCb0x60(); break;
    case 0x61:
      OpCb0x61(); break;
    case 0x62:
      OpCb0x62(); break;
    case 0x63:
      OpCb0x63(); break;
    case 0x64:
      OpCb0x64(); break;
    case 0x65:
      OpCb0x65(); break;
    case 0x66:
      OpCb0x66(); break;
    case 0x67:
      OpCb0x67(); break;
    case 0x68:
      OpCb0x68(); break;
    case 0x69:
      OpCb0x69(); break;
    case 0x6A:
      OpCb0x6A(); break;
    case 0x6B:
      OpCb0x6B(); break;
    case 0x6C:
      OpCb0x6C(); break;
    case 0x6D:
      OpCb0x6D(); break;
    case 0x6E:
      OpCb0x6E(); break;
    case 0x6F:
      OpCb0x6F(); break;
    case 0x70:
      OpCb0x70(); break;
    case 0x71:
      OpCb0x71(); break;
    case 0x72:
      OpCb0x72(); break;
    case 0x73:
      OpCb0x73(); break;
    case 0x74:
      OpCb0x74(); break;
    case 0x75:
      OpCb0x75(); break;
    case 0x76:
      OpCb0x76(); break;
    case 0x77:
      OpCb0x77(); break;
    case 0x78:
      OpCb0x78(); break;
    case 0x79:
      OpCb0x79(); break;
    case 0x7A:
      OpCb0x7A(); break;
    case 0x7B:
      OpCb0x7B(); break;
    case 0x7C:
      OpCb0x7C(); break;
    case 0x7D:
      OpCb0x7D(); break;
    case 0x7E:
      OpCb0x7E(); break;
    case 0x7F:
      OpCb0x7F(); break;
    case 0x80:
      OpCb0x80(); break;
    case 0x81:
      OpCb0x81(); break;
    case 0x82:
      OpCb0x82(); break;
    case 0x83:
      OpCb0x83(); break;
    case 0x84:
      OpCb0x84(); break;
    case 0x85:
      OpCb0x85(); break;
    case 0x86:
      OpCb0x86(); break;
    case 0x87:
      OpCb0x87(); break;
    case 0x88:
      OpCb0x88(); break;
    case 0x89:
      OpCb0x89(); break;
    case 0x8A:
      OpCb0x8A(); break;
    case 0x8B:
      OpCb0x8B(); break;
    case 0x8C:
      OpCb0x8C(); break;
    case 0x8D:
      OpCb0x8D(); break;
    case 0x8E:
      OpCb0x8E(); break;
    case 0x8F:
      OpCb0x8F(); break;
    case 0x90:
      OpCb0x90(); break;
    case 0x91:
      OpCb0x91(); break;
    case 0x92:
      OpCb0x92(); break;
    case 0x93:
      OpCb0x93(); break;
    case 0x94:
      OpCb0x94(); break;
    case 0x95:
      OpCb0x95(); break;
    case 0x96:
      OpCb0x96(); break;
    case 0x97:
      OpCb0x97(); break;
    case 0x98:
      OpCb0x98(); break;
    case 0x99:
      OpCb0x99(); break;
    case 0x9A:
      OpCb0x9A(); break;
    case 0x9B:
      OpCb0x9B(); break;
    case 0x9C:
      OpCb0x9C(); break;
    case 0x9D:
      OpCb0x9D(); break;
    case 0x9E:
      OpCb0x9E(); break;
    case 0x9F:
      OpCb0x9F(); break;
    case 0xA0:
      OpCb0xA0(); break;
    case 0xA1:
      OpCb0xA1(); break;
    case 0xA2:
      OpCb0xA2(); break;
    case 0xA3:
      OpCb0xA3(); break;
    case 0xA4:
      OpCb0xA4(); break;
    case 0xA5:
      OpCb0xA5(); break;
    case 0xA6:
      OpCb0xA6(); break;
    case 0xA7:
      OpCb0xA7(); break;
    case 0xA8:
      OpCb0xA8(); break;
    case 0xA9:
      OpCb0xA9(); break;
    case 0xAA:
      OpCb0xAA(); break;
    case 0xAB:
      OpCb0xAB(); break;
    case 0xAC:
      OpCb0xAC(); break;
    case 0xAD:
      OpCb0xAD(); break;
    case 0xAE:
      OpCb0xAE(); break;
    case 0xAF:
      OpCb0xAF(); break;
    case 0xB0:
      OpCb0xB0(); break;
    case 0xB1:
      OpCb0xB1(); break;
    case 0xB2:
      OpCb0xB2(); break;
    case 0xB3:
      OpCb0xB3(); break;
    case 0xB4:
      OpCb0xB4(); break;
    case 0xB5:
      OpCb0xB5(); break;
    case 0xB6:
      OpCb0xB6(); break;
    case 0xB7:
      OpCb0xB7(); break;
    case 0xB8:
      OpCb0xB8(); break;
    case 0xB9:
      OpCb0xB9(); break;
    case 0xBA:
      OpCb0xBA(); break;
    case 0xBB:
      OpCb0xBB(); break;
    case 0xBC:
      OpCb0xBC(); break;
    case 0xBD:
      OpCb0xBD(); break;
    case 0xBE:
      OpCb0xBE(); break;
    case 0xBF:
      OpCb0xBF(); break;
    case 0xC0:
      OpCb0xC0(); break;
    case 0xC1:
      OpCb0xC1(); break;
    case 0xC2:
      OpCb0xC2(); break;
    case 0xC3:
      OpCb0xC3(); break;
    case 0xC4:
      OpCb0xC4(); break;
    case 0xC5:
      OpCb0xC5(); break;
    case 0xC6:
      OpCb0xC6(); break;
    case 0xC7:
      OpCb0xC7(); break;
    case 0xC8:
      OpCb0xC8(); break;
    case 0xC9:
      OpCb0xC9(); break;
    case 0xCA:
      OpCb0xCA(); break;
    case 0xCB:
      OpCb0xCB(); break;
    case 0xCC:
      OpCb0xCC(); break;
    case 0xCD:
      OpCb0xCD(); break;
    case 0xCE:
      OpCb0xCE(); break;
    case 0xCF:
      OpCb0xCF(); break;
    case 0xD0:
      OpCb0xD0(); break;
    case 0xD1:
      OpCb0xD1(); break;
    case 0xD2:
      OpCb0xD2(); break;
    case 0xD3:
      OpCb0xD3(); break;
    case 0xD4:
      OpCb0xD4(); break;
    case 0xD5:
      OpCb0xD5(); break;
    case 0xD6:
      OpCb0xD6(); break;
    case 0xD7:
      OpCb0xD7(); break;
    case 0xD8:
      OpCb0xD8(); break;
    case 0xD9:
      OpCb0xD9(); break;
    case 0xDA:
      OpCb0xDA(); break;
    case 0xDB:
      OpCb0xDB(); break;
    case 0xDC:
      OpCb0xDC(); break;
    case 0xDD:
      OpCb0xDD(); break;
    case 0xDE:
      OpCb0xDE(); break;
    case 0xDF:
      OpCb0xDF(); break;
    case 0xE0:
      OpCb0xE0(); break;
    case 0xE1:
      OpCb0xE1(); break;
    case 0xE2:
      OpCb0xE2(); break;
    case 0xE3:
      OpCb0xE3(); break;
    case 0xE4:
      OpCb0xE4(); break;
    case 0xE5:
      OpCb0xE5(); break;
    case 0xE6:
      OpCb0xE6(); break;
    case 0xE7:
      OpCb0xE7(); break;
    case 0xE8:
      OpCb0xE8(); break;
    case 0xE9:
      OpCb0xE9(); break;
    case 0xEA:
      OpCb0xEA(); break;
    case 0xEB:
      OpCb0xEB(); break;
    case 0xEC:
      OpCb0xEC(); break;
    case 0xED:
      OpCb0xED(); break;
    case 0xEE:
      OpCb0xEE(); break;
    case 0xEF:
      OpCb0xEF(); break;
    case 0xF0:
      OpCb0xF0(); break;
    case 0xF1:
      OpCb0xF1(); break;
    case 0xF2:
      OpCb0xF2(); break;
    case 0xF3:
      OpCb0xF3(); break;
    case 0xF4:
      OpCb0xF4(); break;
    case 0xF5:
      OpCb0xF5(); break;
    case 0xF6:
      OpCb0xF6(); break;
    case 0xF7:
      OpCb0xF7(); break;
    case 0xF8:
      OpCb0xF8(); break;
    case 0xF9:
      OpCb0xF9(); break;
    case 0xFA:
      OpCb0xFA(); break;
    case 0xFB:
      OpCb0xFB(); break;
    case 0xFC:
      OpCb0xFC(); break;
    case 0xFD:
      OpCb0xFD(); break;
    case 0xFE:
      OpCb0xFE(); break;
    case 0xFF:
      OpCb0xFF(); break;
    default:
      break;
  }

  if (mExeCycle == EXE_COMPLETE)
    ExtInstructionCompleteEvent();
}

void Cpu::Fetch()
{
  ReadNextUint8();
  mOpcode = mDataBus;
}

void Cpu::HaltHandler()
{
  if (mInterruptCtrl.InterruptExists())
    HaltCompleteEvent();
}

void Cpu::InterruptHandler()
{
  switch (mExeCycle)
  {
    case 0:
      mAddressBus = mPC.HL;
      mPC.HL -= 1;
      mExeCycle += 1;
      break;
    case 1:
      mAddressBus = mSP.HL;
      mSP.HL -= 1;
      mExeCycle += 1;
      break;
    case 2:
      mAddressBus = mSP.HL;
      mDataBus = mPC.H;
      Write();
      mSP.HL -= 1;
      mExeCycle += 1;
      break;
    case 3:
      mAddressBus = mSP.HL;
      mDataBus = mPC.L;
      Write();
      mExeCycle += 1;
      break;
    case 4:
      mPC.HL = mInterruptCtrl.HandleInterrupt();
      mExeCycle = EXE_COMPLETE;
      InterruptCompleteEvent();
      break;
    default:
      break;
  }
}

void Cpu::TickExecution()
{
  switch (this->mExecutionMode)
  {
    case ExecutionMode::INSTRUCTION:
      InstructionHandler();
      break;
    case ExecutionMode::EXT_INSTRUCTION:
      ExtInstructionHandler();
      break;
    case ExecutionMode::INTERRUPT:
      InterruptHandler();
      break;
    case ExecutionMode::HALT:
      HaltHandler();
      break;
    case ExecutionMode::STOP:
      throw std::exception("STOP not implemented.");
      break;
    default:
      break;
  }
}
