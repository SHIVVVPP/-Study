#include "GDT.h"
#include "string.h"
#include "memory.h"
#include "windef.h"
#include "defines.h"

//! global descriptor table is an array of descriptors
static struct gdt_descriptor	_gdt [MAX_DESCRIPTORS];
//! gdtr data
static struct gdtr				_gdtr;

// install gdtr
// gdtr�� �ҷ��´�. To load the GDTR, the instruction LGDT is used: 
static void InstallGDT () {
#ifdef _MSC_VER
	_asm lgdt [_gdtr]
#endif
}

//! Setup a descriptor in the Global Descriptor Table
void gdt_set_descriptor(uint32_t i, uint64_t base, uint64_t limit, uint8_t access, uint8_t grand)
{
	if (i > MAX_DESCRIPTORS)
		return;

	//! null out the descriptor
	memset ((void*)&_gdt[i], 0, sizeof (gdt_descriptor));

	//! set limit and base addresses
	_gdt[i].baseLo	= uint16_t(base & 0xffff);
	_gdt[i].baseMid	= uint8_t((base >> 16) & 0xff);
	_gdt[i].baseHi	= uint8_t((base >> 24) & 0xff);
	_gdt[i].limit	= uint16_t(limit & 0xffff);

	//! set flags and grandularity bytes
	_gdt[i].flags = access;
	_gdt[i].grand = uint8_t((limit >> 16) & 0x0f);
	_gdt[i].grand |= grand & 0xf0;
}


//! returns descriptor in gdt
gdt_descriptor* i86_gdt_get_descriptor (int i) {

	if (i > MAX_DESCRIPTORS)
		return 0;

	return &_gdt[i];
}

//GDT �ʱ�ȭ �� GDTR �������Ϳ� GDT �ε�
int GDTInitialize()
{
	//GDTR �������Ϳ� �ε�� _gdtr ����ü�� �� �ʱ�ȭ
	//_gdtr ����ü�� �ּҴ� ����¡ ���ܰ��̸� ���� �����ּҿ� �ش� ������ �Ҵ�Ǿ� �ִ�.
	//��ũ������ ���� ��Ÿ���� MAX_DESCRIPTORS�� ���� 5�̴�.
	//NULL ��ũ����, Ŀ�� �ڵ� ��ũ����, Ŀ�� ������ ��ũ����, ���� �ڵ� ��ũ����
	//���� ������ ��ũ���� �̷��� �� 5���̴�.
	//��ũ���ʹ� 8����Ʈ�̹Ƿ� GDT�� ũ��� 40����Ʈ��.
	_gdtr.m_limit = (sizeof(struct gdt_descriptor) * MAX_DESCRIPTORS) - 1;
	_gdtr.m_base = (uint32_t)&_gdt[0];

	//NULL ��ũ������ ����
	gdt_set_descriptor(0, 0, 0, 0, 0);

	//Ŀ�� �ڵ� ��ũ������ ����
	gdt_set_descriptor(1, 0, 0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_EXEC_CODE | I86_GDT_DESC_CODEDATA |
		I86_GDT_DESC_MEMORY, I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT |
		I86_GDT_GRAND_LIMITHI_MASK);

	//Ŀ�� ������ ��ũ������ ����
	gdt_set_descriptor(2, 0, 0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_MEMORY,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

	//������� ��ũ������ ����
	gdt_set_descriptor(3, 0, 0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_EXEC_CODE | I86_GDT_DESC_CODEDATA |
		I86_GDT_DESC_MEMORY | I86_GDT_DESC_DPL, I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT |
		I86_GDT_GRAND_LIMITHI_MASK);

	//������� ������ ��ũ������ ����
	gdt_set_descriptor(4, 0, 0xffffffff, I86_GDT_DESC_READWRITE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_MEMORY | I86_GDT_DESC_DPL,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);


	/*
	��ũ���ʹ� ��� �ڵ峪 ���׸�Ʈ�̸� I86_GDT_DESC_CODEDATA
	�޸� �� ���׸�Ʈ�� �����ϰ� I86_GDT,DESC_MEMORY
	���׸�Ʈ ũ��� 20bit������ 4GB �ּ� ������ �����ϸ� I86_GDT_GRAND_4K,G�÷��� Ȱ��ȭ(I86_GDT_GRAND_LIMITHI_MASK)
	���׸�Ʈ�� 32bit �ڵ带 ��� �ִ�. I86_GDT_GRAND_32BIT
	�ڵ� ���׸�Ʈ�� ��쿡�� �ڵ� ������ �����ϵ��� I86_GDT_DESC_EXEC_CODE �÷��׸� �����Ѵ�.

	��ũ���� ���� �����ϴ� �κ�(2, 3��° �Ű�����)�� ����
	��� ��ũ������ ���׸�Ʈ ���̽� �ּҴ� 0�̸�, ���׸�Ʈ ũ���  4GB(0xffffffff)�� �����ߴ�.
	*/

	//GDTR �������Ϳ� GDT �ε�
	InstallGDT();

	return 0;
}
