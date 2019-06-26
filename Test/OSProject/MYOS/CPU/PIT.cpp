#include "PIT.h"
#include "HAL.h"
#include "SkyConsole.h"

/*
PIT - Programmable Interval Timer
�ϻ����� �ð� ���� ������ �ذ��ϱ� ���� ����� ī����/Ÿ�̸� ����̽��̴�.
3���� ī���Ϳ� 1���� �������͸� ������ �ִ�.
040h Counter 0 �б�/����
041h Counter 1 �б�/����
042h Counter 2 �б�/����
043h Counter 3 ����
*/
#define I86_PIT_REG_COUNTER0 0x40
#define I86_PIT_REG_COUNTER1 0x41
#define I86_PIT_REG_COUNTER2 0x42
#define I86_PIT_REG_COMMAND 0x43

extern void SendEOI();

volatile uint32_t _pitTicks = 0;
DWORD _lastTickCount = 0;

__declspec(naked) void InterruptPITHandler()
{
	_asm
	{
		PUSHAD
		PUSHFD
		CLI
	}
	
	if (_pitTicks - _lastTickCount >= 100)
	{
		_lastTickCount = _pitTicks;
		SkyConsole::Print("Timer Count : %d\n", _pitTicks);
	}

	_pitTicks++;

	SendEOI();

	// �������͸� �����ϰ� ���� �����ϴ� ������ ���ư���.
	_asm
	{
		POPFD
		POPAD
		IRETD
	}
}

//  PIT �ʱ�ȭ
void InitializePIT()
{
	setvect(32, InterruptPITHandler);
}

//  Ÿ�̸Ӹ� ����
/*
PIT�� 1�ʸ��� 1193181���� ���ڸ� ī�����ϰ� Ÿ�̸� ���ͷ�Ʈ�� �߻���Ų��.
�� ���ͷ�Ʈ�� ���ؼ� �츮�� ���μ����� �������� �����ٸ��� �� �� �����Ƿ� PIT�� ������ �ſ� �߿��ϴ�.

ù���� ���ڴ� ������, �ι�° ���ڴ� ����� ī���� ��������, 
����° ���ڴ� ���� ���������� 1-3 ��Ʈ�� �����ϴ� �κ����� ī���� ����� �����ϴµ� ���δ�.
*/
void StartPITCounter(uint32_t freq, uint8_t counter, uint8_t mode)
{
	if (freq == 0)
		return;

	uint16_t divisor = uint16_t(1193181 / (uint16_t)freq);

	// Ŀ�ǵ� ����
	uint8_t ocw = 0;
	ocw = (ocw & ~I86_PIT_OCW_MASK_MODE) | mode;
	ocw = (ocw & ~I86_PIT_OCW_MASK_RL) | I86_PIT_OCW_RL_DATA;
	ocw = (ocw & ~I86_PIT_OCW_MASK_COUNTER) | counter;
	SendPITCommand(ocw);

	//�������� ���� ����
	SendPITData(divisor & 0xff, 0);
	SendPITData((divisor >> 8) & 0xff, 0);

	//Ÿ�̸� ƽ ī��Ʈ ����
	_pitTicks = 0;
}

void _cdecl msleep(int ms)
{

	unsigned int ticks = ms + GetTickCount();
	while (ticks > GetTickCount())
		;
}


void SendPITCommand(uint8_t cmd) {

	OutPortByte(I86_PIT_REG_COMMAND, cmd);
}

void SendPITData(uint16_t data, uint8_t counter) {

	uint8_t	port = (counter == I86_PIT_OCW_COUNTER_0) ? I86_PIT_REG_COUNTER0 :
		((counter == I86_PIT_OCW_COUNTER_1) ? I86_PIT_REG_COUNTER1 : I86_PIT_REG_COUNTER2);

	OutPortByte(port, (uint8_t)data);
}

uint32_t SetPITTickCount(uint32_t i)
{
	uint32_t ret = _pitTicks;
	_pitTicks = i;
	return ret;
}

//! returns current tick count
uint32_t GetPITTickCount() {

	return _pitTicks;
}

unsigned int GetTickCount()
{
	return _pitTicks;
}
