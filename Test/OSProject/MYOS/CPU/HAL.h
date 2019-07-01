#pragma once
#include "windef.h"


/*
HAL (Hardware Abstraction Layer)
�ϵ���� �߻�ȭ ���̾�
��ǻ���� �������� �ϵ����� ��ǻ�Ϳ��� ����Ǵ� ����Ʈ���� ������ �߻�ȭ �����̴�.
��ǻ�Ϳ��� ���α׷��� ���������� �ϵ��� �� ���� ���� �ٷ� �� �ֵ��� OS���� ������ִ� ���� ������ �Ѵ�.
HAL�� ���� �ʿ��� ����� OS�� ��û�ϰ� �������� OS�� �˾Ƽ� ó�����ֵ��� �Ѵ�.
*/

/*
I/O ��Ʈ�� ������ ���� �� �޴� �Լ����� ���� ����

Ű������ ���
	PC ���� ����(BUS)�� ��Ʈ I/O ������� ����Ǿ� ������, ��Ʈ ��巹���� 0x60�� 0x64�� ����Ѵ�.
	������ �Ҵ�� ��Ʈ�� �� ������ ��Ʈ���� �����͸� ���� ���� �� �� �����ϴ� �������Ͱ� �ٸ��Ƿ�,
	�����δ� �� ���� �������Ϳ� ����� �Ͱ� ����.

PIT ��Ʈ�ѷ��� ���
	PIT ��Ʈ�ѷ��� 1���� ��Ʈ�� �������Ϳ� 3���� ī���ͷ� �����Ǹ�, I/O ��Ʈ 0x43, 0x40, 0x41, 0x42�� ����Ǿ� �ִ�.
	���߿��� ��Ʈ�� �������ʹ� ���⸸ �����ϰ�, ��Ʈ�� �������͸� ������ ī���ʹ� �б�� ���Ⱑ ��� �����ϴ�.

	... 
*/


extern "C" int _outp(unsigned short, int);
extern "C" unsigned long _outpd(unsigned int, int);
extern "C" unsigned short _outpw(unsigned short, unsigned short);
extern "C" int _inp(unsigned short);
extern "C" unsigned short _inpw(unsigned short);
extern "C" unsigned long _inpd(unsigned int shor);

void OutPortByte(ushort port, uchar value);
void OutPortWord(ushort port, ushort value);
void OutPortDWord(ushort port, unsigned int value);
uchar InPortByte(ushort port);
ushort InPortWord(ushort port);
long InPortDWord(unsigned int port);


//PIT ����鼭 �߰�
void setvect(int intno, void(&vect)(), int flags = 0);

//Exception ����鼭 �߰�

#ifdef _MSC_VER
#define interrupt __declspec(naked)
#else
#define interrupt
#endif

#pragma pack (push, 1)
typedef struct registers
{
	u32int ds, es, fs, gs;                  // Data segment selector
	u32int edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pushad.
	//u32int int_no, err_code;    // Interrupt number and error code (if applicable)
	u32int eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} registers_t;
#pragma pack (pop)


// PhysicalMemoryManager ������ �߰�

#define PAGE_SIZE 4096  // �������� ũ��� 4096

// PAGE_SIZE �� 4096 �̹Ƿ� PAGE_SIZE -1 = 1111 1111 1111
// ���� 32bit ���, ~(PAGE_SIZE - 1) = 11111111 11111111 11110000 00000000
// PAGE_TABLE_ENTRY ������
//	������ ���� �ּ�[31:12](20bit) Available[11:9](3bit) opation[8:0](9bit) �̹Ƿ�
// �ּ� ���� ���� �� �ִ�.
#define PAGE_ALIGN_DOWN(value)	((value) & ~(PAGE_SIZE - 1))

// PAGE - 1 = 1111 1111 1111
// ���ڸ��� 0000 0000 0000 �̶��
// value �״��
// �ƴ϶�� PAGE_ALIGN_DOWN + PAGE_SIZE �� �ּҰ�
#define PAGE_ALIGN_UP(value)	((value) & (PAGE_SIZE -1)) ? \
									(PAGE_ALIGN_DOWN((value)) + PAGE_SIZE) : ((value))
