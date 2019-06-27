#pragma once
#include <stdint.h>

/*
GDT(Global Descriptor Table)�̶�
���ӵ� ��ũ������ ����.
��, OS���� ����ϴ� �ڵ� ���׸�Ʈ ��ũ���Ϳ� ������ ���׸�Ʈ ��ũ���͸� ���ӵ� ������� �ڵ�� ��Ÿ�� ��
�ٸ�, NULL ��ũ���͸� ���� �պκп� �߰��ؾ� �Ѵ�.
*/



// �ִ� ��ũ���� �� 10
#define MAX_DESCRIPTORS 10

/*** GDT ��ũ���� ���� bit flags		***/


//0. ACCESS ������ ��Ʈ��, �����尡 �� ���׸�Ʈ�� �������� ��, 1�� �����ȴ�. �ѹ� ������ ���Ŀ��� Ŭ���� ���� �ʴ´�.
#define I86_GDT_DESC_ACCESS	0x0001  // 00000001
//1.READWIRTE �����Ǿ��� ��� ��ũ���ʹ� �а� ���Ⱑ �����ϴ�.
#define I86_GDT_DESC_READWRITE 0x0002 // 00000010
//2.EXPANSION Ȯ���Ʈ�� ����
#define I86_GDT_DESC_EXPANSION 0x0004 // 00000100
//3.EXEC_CODE �⺻�� ������ ���׸�Ʈ�̸�, �������� ��� �ڵ弼�׸�Ʈ�� �ǹ��ϰ� �ȴ�.
#define I86_GDT_DESC_EXE_CODE 0x0008 // 00001000
//4.CODEDATA �ý��ۿ� ���� ���ǵ� ���׸�Ʈ�� ��� 0, �ڵ� �Ǵ� ������ ���׸�Ʈ�� ��� 1
#define I86_GDT_DESC_CODEDATA 0x0010 // 00010000
//5.DPL 2bit �÷��׷�, DPL Ư�� ������ ��Ÿ����.
#define I86_GDT_DESC_DPL	0x0060	// 01100000
//6.MEMORY �ش� ���׸�Ʈ�� ���� �޸𸮿� �ö�� �־� ���� �������� �ǹ��Ѵ�. 0�� ��� �ش� ���׸�Ʈ ��ũ���Ͱ� ����Ű�� �޸𸮴� ������ �� ����.
#define I86_GDT_DESC_MEMORY	0x0080	// 10000000

/***	GDT Descriptor Granularity(����) bit Flag	***/

// 1. LIMITHI_MASK
//	�� 4bit ���� Segment Limit �� 16bit�� ���ļ� 20bit�� ��Ÿ����.
//	G ��Ʈ�� �����Ǿ� ������ 20bit�� 4GB �ּҰ��� ǥ���� �����ϴ�. 
#define I86_GDT_GRAND_LIMITHI_MASK	0x0f // 00001111
// 2. OS
//	���׸�Ʈ�� 64bit ����Ƽ�� �ڵ带 �����ϰ� �ִ����� �ǹ��ϸ�,
//	�� ��Ʈ�� �����Ǹ� D/B �ʵ�� 0���� �����Ǿ�� �Ѵ�.
#define I86_GDT_GRAND_OS	0x10	// 00010000
//3. 32BIT
//	32bit ����� �� �����ȴ�.
#define I86_GDT_GRAND_32BIT	0x40	// 01000000
//4. 4K
//	�� �÷��װ� 1�̸� ���׸�Ʈ ������ 4k(2^2 * 2^10 = 2^12)(12bit)�� �ȴ�.
#define I86_GDT_GRAND_4K	0x80	// 10000000

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

/*
GDT �ٽ� ����
	GDT(Global Descriptor Table)�� CPU�� ��ȣ ��� ����� ����ϱ� ����
	�ü�� �����ڰ� �ۼ��ؾ� �ϴ� ���̺��̴�.

	// �ϴ� �ü���� ��ȣ��忡 �����ϸ� ���� �޸� �ּҿ� ���� �����͸� �аų� ���� ������ �Ұ����ϴ�.
	// ����, ���� �ڵ嵵 �޸� �ּҸ� ���� �����ؼ� �������� ���� �ƴ϶� GDT�� ���ļ� ���ǿ� ������
	// ������ �����ϸ�, �׷��� ������ ���ܸ� ����Ų��.

*/


/*
GDT Descriptor ����ü
	GDT ��ũ���� ����ü ũ��� 8byte�̴�.

	�����ּ�[31:24](baseHi 8bit) {G D/B L AVL ���׸�Ʈũ��[19:16]}(grand 8bit) {P DPL S Ÿ��[11:8]}(flag 8bit) �����ּ�(23:16)[7:0](baseMid 8bit)
	�����ּ�(15:00)[31:16](baseLo 16bit) ���׸�Ʈ ũ��[15:0](limit 16bit)



	baseLow, baseMiddle, baseHigh �ʵ带 ���ؼ� ���̽� �ּҸ� ����
	���׸�Ʈ�� ũ��� segmentLimit 2Byte�� �ʵ�� grand 4bit�� ���ؼ� �� �� �ִ�.

	// ���⼭ ���׸�Ʈ�� ũ��� 20bit�� �Ǵµ�, ���̽� �ּҰ� 0�̶�� �ϸ� 20bit�δ�
	// 4GB�� �޸� �ּҸ� ��Ÿ���⿡ �����ϴ�.
	// ������ ���׸�Ʈ ũ�� ������ 4k(12bit) �̹Ƿ�
	// 20bit�� ũ�⸦ 12bit �������� ����Ʈ �Ͽ� 32bit�� ���׸�Ʈ ũ�⸦ ��Ÿ�� �� �־� 4GB ǥ���� �����ϴ�.
*/
struct gdt_descriptor {


	// ���׸�Ʈ ũ�� [15:0](16bit)
	uint16_t		limit; 

	// �����ּ� [15:00](baseLo 16bit) [23:16](baseMid 8bit)
	uint16_t		baseLo;
	uint8_t			baseMid;

	//Access Flag {P DPL S Type[11:8]}(Type 8bit)
	uint8_t			flags;

	// Grand {G D/B L AVL ���׸�Ʈũ��[19:16]}(grand 8bit)
	uint8_t			grand;

	// �����ּ� [31:24](baseHi 16bit)
	uint8_t			baseHi;
};


/*
GDTR �������� ����ü
	���׸�Ʈ ��ũ���ʹ� �޸𸮻� ��ġ�ϴ� �ڷᱸ���� �������� GDT�� ���ִµ�
	GDT�� ��������� GDT�� ��� ��ġ�ϴ��� CPU�� �˷��־�� �Ѵ�.
	CPU�� GDTR �������͸� �����ؼ� GDT�� �����ϹǷ� GDTR�������Ϳ� ������ ���� �����ؾ� �Ѵ�.
	����� ����� lgdt ��ɾ GDTR �������Ϳ� ���� �����Ѵ�.
*/
struct gdtr {

	//! Global Descriptor Table�� ũ��
	uint16_t		m_limit;

	//! Global Descriptor Table�� ���� �ּ�
	uint32_t		m_base;
};

#ifdef _MSC_VER
#pragma pack (pop)
#endif

// GDT(Global Descriptor Table) ���� �Լ�
extern void gdt_set_descriptor(uint32_t i, uint64_t base, uint64_t limit, uint8_t access, uint8_t grand);

//! returns descritor
extern gdt_descriptor* i86_gdt_get_descriptor (int i);
// GDT�� �����ϰ� GDTR �������Ϳ� �ʱ�ȭ �ϴ� �Լ�
extern	int GDTInitialize();