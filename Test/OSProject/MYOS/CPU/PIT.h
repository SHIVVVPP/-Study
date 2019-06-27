#ifndef PIT_H
#define PIT_H
#include "stdint.h"

/*
Ÿ�̸� ����̽��� PIC ��Ʈ�ѷ��� IRQ 0�� ����Ǿ� ������,
�� ���̳� ������ �ֱ�� ���ͷ�Ʈ�� �߻���Ų��.
Ÿ�̸Ӹ� PIT(Programmable Interval Timer) ��� �θ��µ�,
��Ʈ�ѷ��� �������Ϳ� �ֱ⸦ ���������ν� �߻� ������ ���� �� �� �ִ�.
Ÿ�̸� ����̽��� �ַ� �ð� �����̳� �ú��� ��Ƽ�½�ŷ�� ó���� �� ����Ѵ�.
*/


//-----------------------------------------------
//	Operational Command control bits ��Ʈ�� �������� �ʵ� ������ �ǹ�
//-----------------------------------------------

// PIT OCW(Operation Command Word) ���� Ŀ�ǵ� ��Ÿ���� �� ��Ʈ
#define		I86_PIT_OCW_MASK_BINCOUNT		1		//00000001 BCD��Ʈ(0��° ��Ʈ)
#define		I86_PIT_OCW_MASK_MODE			0xE		//00001110 Mode��Ʈ (1~3��° ��Ʈ)
#define		I86_PIT_OCW_MASK_RL				0x30	//00110000 RW��Ʈ (4~5��° ��Ʈ)
#define		I86_PIT_OCW_MASK_COUNTER		0xC0	//11000000 SC(SelectCount)��Ʈ (6~7��° ��Ʈ)

/*
0��° ��Ʈ (0000 000^)
BCD
ī������ ���� ���̳ʸ� �Ǵ� 4��Ʈ BCD �������� �����Ѵ�.
0���� ������ ��� ���̳ʸ� ������ ������� �ǹ��ϸ�, 1�� ������ ��� BCD������ ������� �ǹ��Ѵ�.
PIT ��Ʈ�ѷ� ������ ī���ʹ� 2����Ʈ �̹Ƿ�,BCD ������ 0~9999���� �����ϸ�,
���̳ʸ� ������ 0x00 ~ 0xFFFF���� �����ϴ�.
*/
#define		I86_PIT_OCW_BINCOUNT_BINARY		0		//0
#define		I86_PIT_OCW_BINCOUNT_BCD		1		//1

/*
1~3��° ��Ʈ (0000 ^^^0)
Mode
PIT ��Ʈ�ѷ��� ī��Ʈ ��带 �����Ѵ�.
*/
#define		I86_PIT_OCW_MODE_TERMINALCOUNT	0		//0000 Interrupt during counting
#define		I86_PIT_OCW_MODE_ONESHOT		0x2		//0010 Programmable monoflop
#define		I86_PIT_OCW_MODE_RATEGEN		0x4		//0100 Clock rate generator
#define		I86_PIT_OCW_MODE_SQUAREWAVEGEN	0x6		//0110 Square wave generator
#define		I86_PIT_OCW_MODE_SOFTWARETRIG	0x8		//1000 Software-triggered impulse
#define		I86_PIT_OCW_MODE_HARDWARETRIG	0xA		//1010 Hardware-triggered impulse

/*
4~5��° ��Ʈ (00^^ 0000)
RW
-Read/Write�� �����̸�, ī���Ϳ� �б� �Ǵ� ���⸦ ������ �� ��� �뵵�� ��������� �����Ѵ�.
*/
#define		I86_PIT_OCW_RL_LATCH			0			//000000 ī������ ���� ���� ����(Latch Instruction), 2����Ʈ ����
#define		I86_PIT_OCW_RL_LSBONLY			0x10		//010000 ī������ ���� ����Ʈ�� �аų� ��, 1����Ʈ ����
#define		I86_PIT_OCW_RL_MSBONLY			0x20		//100000 ī������ ���� ����Ʈ�� �аų� ��, 1����Ʈ ����
#define		I86_PIT_OCW_RL_DATA				0x30		//110000 ī������ ���� ����Ʈ���� ���� ����Ʈ ������ �����ؼ� I/O ��Ʈ�� �аų� ����. 2����Ʈ ����
// ��Ʈ 00 ���� ������ ��� , ��Ʈ 3~0�� ��� 0���� �����ϰ� ���� ����Ʈ���� ���� ����Ʈ ������ �����ؼ� I/O ��Ʈ�� �о�� �Ѵ�.

/*
6~7��° ��Ʈ (^^00 0000)
SC
-Select Counter�� �����̸�, Ŀ�ǵ��� ����� �Ǵ� ī���͸� �����Ѵ�.
*/
#define		I86_PIT_OCW_COUNTER_0			0		//00000000 ī���� 0
#define		I86_PIT_OCW_COUNTER_1			0x40	//01000000 ī���� 1
#define		I86_PIT_OCW_COUNTER_2			0x80	//10000000 ī���� 2

/*
PIT ��Ʈ�ѷ��� ���� Ŭ���� 1.193181Mhz���� �����ϸ�, ��ȸ���� �� ī������ ���� 1�� ���ҽ��� 0�� �Ǿ��� �� ��ȣ�� �߻���Ų��.
��ȣ�� �߻���Ų ���Ŀ� PIT ��Ʈ�ѷ��� ������ ��忡 ���� �ٸ��� �����Ѵ�.
*/
#define TimerFrequency 1193180


extern void SendPITCommand(uint8_t cmd);
extern void SendPITData(uint16_t data, uint8_t counter);

extern uint32_t SetPITTickCount(uint32_t i);
extern uint32_t GetPITTickCount();

void InitializePIT();


unsigned int GetTickCount();
void _cdecl msleep(int ms);

extern void StartPITCounter(uint32_t freq, uint8_t counter, uint8_t mode);
#endif