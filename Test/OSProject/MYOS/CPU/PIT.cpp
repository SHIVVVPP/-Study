#include "PIT.h"
#include "HAL.h"
#include "SkyConsole.h"

/*
PIT ��Ʈ�ѷ��� 1���� ��Ʈ�� �������Ϳ� 3���� ī���ͷ� �����Ǹ�,
I/O ��Ʈ 0x43(��Ʈ�� ��������), 0x40, 0x41, 0x42�� ����Ǿ� �ִ�.
���� ��Ʈ�� �������ʹ� ���⸸ �����ϰ�, ��Ʈ�� �������͸� ������ ī���ʹ� �б� ���Ⱑ ��� �����ϴ�.

-------------------------------------------------------------------------------------------
��Ʈ��ȣ	|		�б�/����		|	�������� �̸�		| ����
-------------------------------------------------------------------------------------------
0x43	|		����			|	��Ʈ�� ��������	| PIT ��Ʈ�ѷ��� �����ϴ� ��������
-------------------------------------------------------------------------------------------
0x40	|		�б�/����		|	ī����0			| ī���Ϳ� ����� ���� �����ϰų�
0x41	|		�б�/����		|	ī����1			| ���� ī������ ���� ������ ����ϴ� ��������
0x42	|		�б�/����		|	ī����2			|
-------------------------------------------------------------------------------------------
PIT ��Ʈ�ѷ��� ��Ʈ�� ���������� ũ��� 1����Ʈ�̰�, ī���� ���������� ũ��� 2����Ʈ�̴�.
I/O ��Ʈ�� PIT ��Ʈ�ѷ��� 1����Ʈ ������ �����ϹǷ�,
��Ʈ�� �������Ϳ� �����ϴ� Ŀ�ǵ忡 ���� ī���� I/O ��Ʈ�� �а� ���� ����Ʈ ���� �����ȴ�.
��Ʈ�� �������ʹ� 4���� �ʵ�� �����Ǿ� �ִ�.
*/
#define I86_PIT_REG_COUNTER0 0x40
#define I86_PIT_REG_COUNTER1 0x41
#define I86_PIT_REG_COUNTER2 0x42
#define I86_PIT_REG_COMMAND 0x43

extern void SendEOI();

volatile uint32_t _pitTicks = 0;
DWORD _lastTickCount = 0;

/*
PUSHAD
���� ���������� ���� ���ÿ� ���� �ִ� ��ɾ��̴�. 
����Ǵ� ������ EAX,ECX,EDX,EBX,ESP,EBP,ESI,EDI
PUSHFD
�÷��� ���� �����Ѵ�. EFL
*/
__declspec(naked) void InterruptPITHandler()
{
	_asm
	{
		PUSHAD
		PUSHFD
		CLI
	}
	
	/*
	_pitTicks ������ Ÿ�̸� ���ͷ�Ʈ �ڵ鷯�� ȣ��� ������ ī��Ʈ�� �����ȴ�.
	_lastTickCount �������� ���� 100�̸� Ÿ�̸� ���ڿ��� ����Ѵ�.
	StartPITCounter �Լ� ȣ�� �� ������ ���� 100���� �����߱� ������ ����
	���ν������� �� ���� �������� ���� 100�϶�, �� 1�ʸ��� ���ڿ��� ��Ȯ�� ��µȴ�.*/
	if (_pitTicks - _lastTickCount >= 100)
	{
		_lastTickCount = _pitTicks;
		SkyConsole::Print("Timer Count : %d\n", _pitTicks);
	}

	_pitTicks++;

	
	/*
	EOI - End Of Interrupt
	EOI(End Of Interrupt) �� PIC(Programmable Interrupt Controller)�� �������� ��ȣ�̸�,
	�־��� ���ͷ�Ʈ�� ���� ���ͷ�Ʈ ó�� �ϷḦ ��Ÿ����.
	*/
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


!!! StartPITCounter�� Ÿ�̸Ӹ� �����ϸ� 1�ʸ��� 1193181���� ���ڸ� ī���� �ϰ� Ÿ�̸� ���ͷ�Ʈ�� �߻���Ų��
StartPITCounter(100,I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN)���� �Լ��� ȣ���ϴµ�,

ù���� ���ڴ� ������, �ι�° ���ڴ� ����� ī���� ��������,
����° ���ڴ� ���� ���������� 1-3 ��Ʈ�� �����ϴ� �κ����� ī���� ����� �����ϴµ� ���δ�.
���� ���������� ������ Select Register(2bit) Read Write(2bit) MODE(3bit) BCD(1bit) �̴�.
I86_PIT_OCW_MODE_SQUAREWAVEGEN�� ���� 0x011(3bit) �̸� MODE�� 3bit�� �����ȴ�.

�ʴ� �������� 100�̶�� ���� 1�ʴ� 100���� Ÿ�̸� ���ͷ�Ʈ�� �߻��Ѵٴ� �ǹ��̸�,
�� ���� ��ȭ��Ŵ�� ���� �ʴ� ���ͷ�Ʈ�� ���� ������ ������ ������ �� �ִ�.
���� �������� 1�� �Ѵٸ� ���ͷ�Ʈ�� �߻��� �� PIT�� ���� ���� ī���� ���� 1193181�� �� ���̴�.
�������� 100�� �ȴٸ� ���ͷ�Ʈ�� �߻����� ���� PIT ���� ���� ī������ 11931�� �� ���̴�. �̰��� Divisor���Ѵ�.
*/
void StartPITCounter(uint32_t freq, uint8_t counter, uint8_t mode)
{
	if (freq == 0)
		return;

	//Divisor ���ͷ�Ʈ�� �߻����� ���� PIT ���� ���� ī���� ��
	uint16_t divisor = uint16_t(1193181 / (uint16_t)freq);

	// Ŀ�ǵ� ����
	// OCW - Operation Command Word 
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

	/*
	���� 1����Ʈ ���� �������Ϳ� �츮�� ����� ī���� ��������(00) �б�/����, ��� �� ���� �����ؼ� ����� ������,
	0��° ī���� �������Ϳ� Divisor �����͸� �������ν� PIT�� �ʱ�ȭ �ȴ�.
	�׷��� PIT�� CPU�� �츮�� ������ �������� �°� �Ź� ���ͷ�Ʈ�� ������.
	*/
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
