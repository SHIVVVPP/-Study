#include "kmain.h"

//Ŀ�� ��Ʈ�� ã��

//GRUB�� �䱸�ϴ� �ñ״�ó( ��Ƽ ��Ʈ ��� ����ü ) 
_declspec(naked) void multiboot_entry(void)
{
	__asm {
		align 4

		multiboot_header :
		// ��Ƽ ��Ʈ ��� ������ : 0x20
		dd(MULTIBOOT_HEADER_MAGIC); magic number
		dd(MULTIBOOT_HEADER_FLAGS); flags
		dd(CHECKSUM);				checksum
		dd(HEADER_ADRESS);			// ��� �ּ� KERNEL_LOAD_ADDRESS+ALIGN(0x100064)
		dd(KERNEL_LOAD_ADDRESS);	// Ŀ���� �ε�� �����ּ� ����
		dd(00);						// ������ ����
		dd(00);						// ������ ����
		dd(HEADER_ADRESS + 0x20);	// Ŀ�� ���� �ּ� : ��Ƽ��Ʈ ��� �ּ� + 0x20, kernel_entry

	kernel_entry :
		mov		esp, KERNEL_STACK;	// ���� ����

		push 0;						// �÷��� �������� �ʱ�ȭ
		popf

		//GRUB�� ���� ��� �ִ� �������� ���ÿ� Ǫ��
		push	ebx;				// ��Ƽ ��Ʈ ����ü ������
		push	eax;				// �����ѹ�

		//���� �� �Ķ���Ϳ� �Բ� kmain �Լ��� ȣ���Ѵ�.
		call	kmain;				// C++ ���� �Լ� ȣ��

		// ������ ����. kmain�� ���ϵ��� ������ �Ʒ� �ڵ�� ������� �ʴ´�.
		halt:
		jmp halt;

	}
}

void InitializeConstructor()
{
	//���� ������ ���߿� �߰��Ѵ�.
}

void kmain(unsigned long magic, unsigned long addr)
{
	InitializeConstructor(); //�۷ι� ��ü �ʱ�ȭ

	SkyConsole::Initialize(); //ȭ�鿡 ���ڿ��� ��� ���� �ʱ�ȭ�Ѵ�.

	SkyConsole::Print("Hello World!!\n");

	for (;;); //�����Լ��� ������ ����, ����
}