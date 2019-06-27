#ifndef _IDT_H
#define _IDT_H
#include <stdint.h>

//Interrupt Descriptor Table : IDT�� ���ͷ�Ʈ�� �ٷ�� �������̽��� ������ å���� �ִ�.
//�������̽� ��ġ, ��û, �׸��� ���ͷ�Ʈ�� ó���ϴ� �ݹ� ��ƾ

//���ͷ�Ʈ �ڵ鷯�� �ִ� ���� : 256
#define I86_MAX_INTERRUPTS		256

// idt_descriptor ����ü�� flags ���� define {P(1) DPL(2) 0(1) Type(4)} 8bit 
#define I86_IDT_DESC_BIT16		0x06	//00000110 Type(^000)�� D�� ����Ʈ�� ����� ���� 1-32bit 0 16bit
#define I86_IDT_DESC_BIT32		0x0E	//00001110	   (0^^^)�� ����Ʈ ���� Ʈ������Ʈ(111) ���ͷ�Ʈ����Ʈ(110)
#define I86_IDT_DESC_RING1		0x40	//01000000 RING Ư�Ƿ��� 
#define I86_IDT_DESC_RING2		0x20	//00100000
#define I86_IDT_DESC_RING3		0x60	//01100000
#define I86_IDT_DESC_PRESENT	0x80	//10000000

//���ͷ�Ʈ �ڵ鷯 ���� �Լ�
//���ͷ�Ʈ �ڵ鷯�� ���μ����� ȣ���Ѵ�. �̶� ���� ������ ���ϴµ�
//���ͷ�Ʈ�� ó���ϰ� �����Ҷ� ������ ���ͷ�Ʈ �ڵ鷯 ȣ�������� ������ �Ȱ����� �����ؾ� �Ѵ�.
typedef void (_cdecl *I86_IRQ_HANDLER)(void);

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

//IDTR ����ü
/*
IDT�� �����ϰ� ���� IDT�� ��ġ�� CPU�� �˷��ֱ� ���� IDTR ����ü�� ������ �Ѵ�.
CPU�� IDTR �������͸� ���ؼ� IDT�� �����Ѵ�.
*/
struct idtr {

	//IDT�� ũ��
	uint16_t limit;

	//IDT�� ���̽� �ּ�
	uint32_t base;
};

//���ͷ�Ʈ ��ũ����
/*
���ͷ�Ʈ ��ũ������ ũ��� 8����Ʈ�̴�.
�� 8����Ʈ �߿��� 2����Ʈ ũ�⸦ �����ϴ� ���׸�Ʈ �����͸� ���ؼ�
GDT ���� �۷ι� ��ũ���͸� ã�� �� �ְ� �� �۷ι� ��ũ���͸� ���ؼ� ���׸�Ʈ�� ���̽� �ּҸ� ���� �� �ִ�.
���׸�Ʈ ���̽� �ּҿ��� ������ ���� ���ϸ� ISR(���ͷ�Ʈ ���� ��ƾ)�� �ּҸ� ���� �� �ִ�.
offsetLow, offsetHigh �ʵ�� ISR�� 4����Ʈ ������ ���� ���� �� �ִ�.
selector �ʵ�� GDT���� ��ũ���͸� ã�� ���׸�Ʈ�� ���̽� �ּҸ� ���� �� �ִ�.
*/
struct idt_descriptor {

	// base = �ڵ鷯 ������
	// ���ͷ�Ʈ �Ǵ� ���� �ڵ鷯�� ��Ʈ�� ����Ʈ

	//���ͷ�Ʈ �ڵ鷯 �ּ��� 0-16 ��Ʈ[0:15]
	uint16_t		baseLo;

	//GDT�� �ڵ� ������[16:31]
	//���ͷ�Ʈ �Ǵ� ���� �ڵ鷯 ���� �� ����� �ڵ� ���׸�Ʈ ��ũ����
	uint16_t		sel;

	//����� �� 0�̾�� �Ѵ�.{00000 IST[2:0]}
	// IST ���ͷ�Ʈ�� ���� �߻� ��, ����� ���� ��巹��(Top)
	// 0�� �ƴ� ������ �����ϸ� ���ͷ�Ʈ �߻� �� ������ ������ ��ȯ�Ѵ�.
	uint8_t			reserved;

	//8��Ʈ ��Ʈ �÷��� {P DPL[14:13] 0 Type[11:8]}
	// Type (IDT ����Ʈ�� ����)
	// ���ͷ�Ʈ ����Ʈ(0110) Ʈ�� ����Ʈ(0111)
	// ���ͷ�Ʈ ����Ʈ�� �����ϸ� �ڵ鷯�� �����ϴ� ���� ���ͷ�Ʈ�� �߻����� ���ϸ�,
	// Ʈ�� ����Ʈ�� ������ �ٸ� ���ͷ�Ʈ �߻�����
	// DPL(Descriptor Privilege Level) �ش� ��ũ���͸� ����ϴ� �� �ʿ��� ���� (0~3�� ��)
	// P(Present) ���� ��ũ���Ͱ� ��ȿ�� ��ũ�������� ǥ�� ��ȿ(1) ��ȿX(0)
	uint8_t			flags;

	//���ͷ�Ʈ �ڵ鷯 �ּ��� 16-32 ��Ʈ
	uint16_t		baseHi;
};

#ifdef _MSC_VER
#pragma pack (pop)
#endif

//i��° ���ͷ�Ʈ ��ũ��Ʈ�� ���´�.
idt_descriptor* GetInterruptDescriptor(uint32_t i);

//���ͷ�Ʈ �ڵ鷯 ��ġ.
bool InstallInterrputHandler(uint32_t i, uint16_t flags, uint16_t sel, I86_IRQ_HANDLER);
bool IDTInitialize(uint16_t codeSel);
void InterrputDefaultHandler();

#endif
