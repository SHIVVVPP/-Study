#include "IDT.h"
#include "string.h"
#include "memory.h"

//���ͷ�Ʈ ��ũ���� ���̺�
static struct idt_descriptor	_idt [I86_MAX_INTERRUPTS];

//CPU�� IDTR �������͸� �ʱ�ȭ�ϴµ� ������ �ִ� IDTR ����ü
static struct idtr				_idtr;

//IDTR �������Ϳ� IDT�� �ּҰ��� �ִ´�.
static void IDTInstall() {
#ifdef _MSC_VER
	_asm lidt [_idtr]
#endif
}

/*
EOI - End Of Interrupt
EOI(End Of Interrupt) �� PIC(Programmable Interrupt Controller)�� �������� ��ȣ�̸�,
�־��� ���ͷ�Ʈ�� ���� ���ͷ�Ʈ ó�� �ϷḦ ��Ÿ����.
*/
#define DMA_PICU1       0x0020 // IO base Address for master PIC
#define DMA_PICU2       0x00A0 // IO base address for slave PIC
__declspec(naked) void SendEOI() //PIT �����ϸ鼭 �߰� E
{
	_asm
	{
		PUSH EBP
		MOV  EBP, ESP
		PUSH EAX

		; [EBP] < -EBP
		; [EBP + 4] < -RET Addr
		; [EBP + 8] < -IRQ ��ȣ

		MOV AL, 20H; EOI ��ȣ�� ������.
		OUT DMA_PICU1, AL

		CMP BYTE PTR[EBP + 8], 7
		JBE END_OF_EOI
		OUT DMA_PICU2, AL; Send to 2 also

		END_OF_EOI :
		POP EAX
			POP EBP
			RET
	}
}


//�ٷ�� �ִ� �ڵ鷯�� �������� ������ ȣ��Ǵ� �⺻ �ڵ鷯
__declspec(naked) void InterrputDefaultHandler () {

	//�������͸� �����ϰ� ���ͷ�Ʈ�� ����. 
	_asm {
		cli // cliear interrupt ���ͷ�Ʈ �÷��׸� 0(interrupt disable)���� ����
		pushad
	}

	// �������͸� �����ϰ� ���� �����ϴ� ������ ���ư���.
	_asm {
		mov al, 0x20
		out 0x20, al
		popad
		sti // set interrupt ���ͷ�Ʈ �÷��׸� 1(interrupt enable)�� ����
		iretd
	}
}

//i��° ���ͷ�Ʈ ��ũ��Ʈ�� ���´�.
idt_descriptor* GetInterruptDescriptor(uint32_t i) {

	if (i>I86_MAX_INTERRUPTS)
		return 0;

	return &_idt[i];
}

//���ͷ�Ʈ �ڵ鷯 ��ġ	//I86_IRQ_HANDLER - ���ͷ�Ʈ �ڵ鷯 �Լ� 
bool InstallInterrputHandler(uint32_t i, uint16_t flags, uint16_t sel, I86_IRQ_HANDLER irq) {

	if (i>I86_MAX_INTERRUPTS)
		return false;

	if (!irq)
		return false;

	//���ͷ�Ʈ(�Լ�)�� ���̽� �ּҸ� ���´�.
	uint64_t		uiBase = (uint64_t)&(*irq);
	
	if ((flags & 0x0500) == 0x0500) {
		_idt[i].sel = sel;
		_idt[i].flags = uint8_t(flags);
	}
	else
	{
		//���˿� �°� ���ͷ�Ʈ �ڵ鷯�� �÷��� ���� ��ũ���Ϳ� �����Ѵ�.
		_idt[i].baseLo = uint16_t(uiBase & 0xffff);
		_idt[i].baseHi = uint16_t((uiBase >> 16) & 0xffff);
		_idt[i].reserved = 0;
		_idt[i].flags = uint8_t(flags);
		_idt[i].sel = sel;
	}

	return	true;
}

//IDT�� �ʱ�ȭ�ϰ� ����Ʈ �ڵ鷯�� ����Ѵ�
bool IDTInitialize(uint16_t codeSel) {

	//IDTR �������Ϳ� �ε�� ����ü �ʱ�ȭ
	_idtr.limit = sizeof(struct idt_descriptor) * I86_MAX_INTERRUPTS - 1;
	_idtr.base = (uint32_t)&_idt[0];

	//NULL ��ũ����
	memset((void*)&_idt[0], 0, sizeof(idt_descriptor) * I86_MAX_INTERRUPTS - 1);

	//����Ʈ �ڵ鷯 ���
	for (int i = 0; i<I86_MAX_INTERRUPTS; i++)
		InstallInterrputHandler(i, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
			codeSel, (I86_IRQ_HANDLER)InterrputDefaultHandler);

	//IDTR �������͸� �¾��Ѵ�
	IDTInstall();

	return true;
}


