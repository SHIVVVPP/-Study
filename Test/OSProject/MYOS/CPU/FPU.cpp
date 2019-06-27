#include "FPU.h"

extern "C" void __writecr4(unsigned __int64 Data);
extern "C" unsigned long __readcr4(void);

// FPU�� �����ϴ��� ���θ� �Ǵ��Ѵ�.
bool InitFPU()
{
	int result = 0;
	unsigned short temp;

	/*
	CR(Control Register)
	� ��带 ����, ���� � ���� ����� Ư�� ����� �����ϴ� ��������
	x86 ���μ������� CR0~CR4 5���� ��Ʈ�� �������Ͱ� ������
	x86-64 ���μ������� CR8 �߰�

	CR0 - ���带 �����ϴ� ��������, �����忡�� ��ȣ���� ��ȯ�ϴ� ����, ĳ��, ����¡ ��� ���� Ȱ��ȭ
	CR1 - ���μ����� ���� �����
	CR2 - ����¡ ��Ʈ �߻� �� ������ ��Ʈ�� �߻��� ���� �ּҰ� ����Ǵ� ��������(����¡ Ȱ��ȭ ��, ������ ��Ʈ�ÿ��� ��ȿ)
	CR3 - ������ ���͸��� ���� �ּҿ� ������ ĳ�ÿ� ���õ� ����� ����
	CR4 - ���μ������� �����ϴ� Ȯ�� ����� ����
		  ������ ũ�� ȭ��, �޸� ���� Ȯ�� ���� ����� Ȱ��ȭ
	CR8 - �׽�ũ �켱 ���� ���������� ���� ����,
		  ���μ��� �ܺο��� �߻��ϴ� ���ͷ�Ʈ ���͸�, IA-32e ��忡���� 64��Ʈ�� Ȯ��
	*/
	__asm
	{
		pushad;			// ��� �������͸� ���ÿ� �����Ѵ�.
		mov eax, cr0;	// eax = CR0
		and al, ~6;		// EM�� MP �÷��׸� Ŭ�����Ѵ�. ~0110 => 1001
		mov cr0, eax;	// eax�� ����� ���� cr0 �������Ϳ� �����Ѵ�.
		fninit; FPU;	// ���¸� �ʱ�ȭ �Ѵ�.
		mov temp, 0x5A5A;	// FPU�� ���¸� ������ �ӽú������� 0�� �ƴ� ������ �����Ѵ�.
		fnstsw temp;		// FPU�� ���¸� ���´�.
		cmp temp, 0;		// ���°��� 0�̸� FPU�� �������� �ʴ´�.
		jne noFPU;			// FPU�� �������� ������ noFPU ���̺�� �����Ѵ�.
		fnstcw temp;		// FPU ����� �ӽú����� ������
		mov ax, temp;		// ���� ���� ax �������Ϳ� �����Ѵ�.
		and ax, 0x103F;		// ax�� 0x103F AND ������ ������ �� ax�� �����Ѵ�.
		cmp ax, 0x003F;		// ax�� ����� ���� 0x003F ��
		jne noFPU;			// ���� Ʋ���ٸ� FPU�� �������� �����Ƿ� noFPU ���̺�� �����Ѵ�.
		mov result, 1;		// �� ������ ����Ǹ� FPU�� �����ϴ� ���̴�.
	noFPU:
		popad;
	}

	return result == 1;
}


bool EnableFPU()
{
#ifdef _WIN32
	unsigned long regCR4 = __readcr4();
	__asm or regCR4, 0x200;
	__writecr4(regCR4);
#else
	/*__asm
	{
	 mov eax, cr4;
	 or eax, 0x200;
	 mov cr4, eax;
	}*/
#endif
}