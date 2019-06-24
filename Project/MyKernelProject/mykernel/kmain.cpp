#include "kmain.h"

//Ŀ�� ��Ʈ�� ã��

/*
GRUB�� Ŀ���� ȣ���� �غ� �Ǿ�����,
Ŀ�� ��Ʈ�� ����Ʈ, �� Ŀ�� �Լ� ���� ������ ȣ���ϱ� ���� Ư�� ��(Ʋ)�� �ؼ��ؾ� �Ѵ�.
GRUB�� Ŀ���� Ư�� ���� ��Ű���� ���θ� Ŀ�� ���� 80KB(���ۺ��� ~ +0x14000) �κ��� �˻��� Ư�� �ñ״�ó�� ã�� �ľ��Ѵ�.
�� �ñ״�ó�� ��Ƽ ��Ʈ ��� ����ü (struct MULTIBOOT_HEADER) ��� �Ѵ�.
*/
//GRUB�� �䱸�ϴ� �ñ״�ó( ��Ƽ ��Ʈ ��� ����ü ) MULTIBOOT_HEADER���·� ä������.
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

// �� multiboot_entry���� ebx eax�� Ǫ���� ��Ƽ ��Ʈ ����ü ������(->addr), �����ѹ�(->magic)�� ȣ��
void kmain(unsigned long magic, unsigned long addr)
{
	InitializeConstructor(); //�۷ι� ��ü �ʱ�ȭ

	SkyConsole::Initialize(); //ȭ�鿡 ���ڿ��� ��� ���� �ʱ�ȭ�Ѵ�.

	SkyConsole::Print("Hello World!!\n");

	for (;;); //�����Լ��� ������ ����, ����
}