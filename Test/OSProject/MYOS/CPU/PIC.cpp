#ifndef ARCH_X86
#error "[PIC.cpp for i86] requires i86 architecture. Define ARCH_X86"
#endif

#include "PIC.h"
#include "HAL.h"
//-----------------------------------------------
//	64��Ʈ ��Ƽ �ھ� OS 415page
//-----------------------------------------------

//-----------------------------------------------
//	Controller Registers
//-----------------------------------------------

/*
PIC ��Ʈ�ѷ��� Ű���� ��Ʈ�ѷ��� ���������� I/O ��Ʈ ������� ����Ǿ� �ִ�.
�츮�� ����ϴ� PC�� PIC ��Ʈ�ѷ��� ���� �� ���� I/O ��Ʈ�� �Ҵ��ϸ�,
������ PIC ��Ʈ�ѷ��� 0x20�� 0x21�� ����ϰ�
�����̺� PIC ��Ʈ�ѷ��� 0xA0�� 0xA1�� ����Ѵ�.
PIC ��Ʈ�ѷ��� �Ҵ�� I/O ��Ʈ�� �б�� ���Ⱑ ��� �����ϴ�.

I/O��Ʈ�� PIC ��Ʈ�ѷ����� ����

				��Ʈ ��ȣ							�б� ����			�������
������ PIC ��Ʈ�ѷ�		�����̺� PIC ��Ʈ�ѷ�
---------------------------------------------------------------------------
0x20				|	0xA0				|	IRR ��������	|	ICW1 Ŀ�ǵ�
					|						|	ISR ��������	|	OCW2 Ŀ�ǵ�
					|						|				|	OCW3 Ŀ�ǵ�
---------------------------------------------------------------------------
0x21				|	0xA1				|	IMR ��������	|	ICW2 Ŀ�ǵ�
					|						|				|	ICW3 Ŀ�ǵ�
					|						|				|	ICW4 Ŀ�ǵ�
					|						|				|	OCW1 Ŀ�ǵ�
---------------------------------------------------------------------------
*/

//! PIC 1 �������� ��Ʈ �ּ�
#define I86_PIC1_REG_COMMAND	0x20
#define I86_PIC1_REG_STATUS		0x20
#define I86_PIC1_REG_DATA		0x21
#define I86_PIC1_REG_IMR		0x21

//! PIC 2 �������� ��Ʈ �ּ�
#define I86_PIC2_REG_COMMAND	0xA0
#define I86_PIC2_REG_STATUS		0xA0
#define I86_PIC2_REG_DATA		0xA1
#define I86_PIC2_REG_IMR		0xA1


//-----------------------------------------------
//	Initialization Command 1 ���� ��Ʈ
//-----------------------------------------------

#define I86_PIC_ICW1_IC4_EXPECT				1			//1
#define I86_PIC_ICW1_IC4_NO					0			//0

#define I86_PIC_ICW1_SNGL_YES				2			//10
#define I86_PIC_ICW1_SNGL_NO				0			//00

#define I86_PIC_ICW1_ADI_CALLINTERVAL4		4			//100
#define I86_PIC_ICW1_ADI_CALLINTERVAL8		0			//000

#define I86_PIC_ICW1_LTIM_LEVELTRIGGERED	8			//1000
#define I86_PIC_ICW1_LTIM_EDGETRIGGERED		0			//0000

#define I86_PIC_ICW1_INIT_YES				0x10		//10000
#define I86_PIC_ICW1_INIT_NO				0			//00000

//-----------------------------------------------
//	Initialization Command 4 ���� ��Ʈ
//-----------------------------------------------

#define I86_PIC_ICW4_UPM_86MODE			1			//1
#define I86_PIC_ICW4_UPM_MCSMODE		0			//0

#define I86_PIC_ICW4_AEOI_AUTOEOI		2			//10
#define I86_PIC_ICW4_AEOI_NOAUTOEOI		0			//0

#define I86_PIC_ICW4_MS_BUFFERMASTER	4			//100
#define I86_PIC_ICW4_MS_BUFFERSLAVE		0			//0

#define I86_PIC_ICW4_BUF_MODEYES		8			//1000
#define I86_PIC_ICW4_BUF_MODENO			0			//0

#define I86_PIC_ICW4_SFNM_NESTEDMODE	0x10		//10000
#define I86_PIC_ICW4_SFNM_NOTNESTED		0			//a binary 2 (futurama joke hehe ;)




/*
PIC ��Ʈ�ѷ��� �ʱ�ȭ �۾��� ������ �� �����̺� PIC ��Ʈ�ѷ��� ���������� �����ؾ� �Ѵ�.
�ʱ�ȭ�� ����ϴ� ���� ������ PIC ��Ʈ�ѷ��� �����̺� PIC ��Ʈ�ѷ��� ���� ����.
*/
// PICInitialize(0x20, 0x28); 0x20���� �����ϸ� PIC ��Ʈ�ѷ� 0~7�� ���� ���ͷ�Ʈ�� ���μ����� ���� 0x20~0x27�� ���޵ȴ�.
// ���� 0x20, 0x28 �μ����� ���� ������ pic�� ���� 0x20~0x27, �����̺� PIC�� ���� 0x28~0x2F
void PICInitialize(uint8_t base0, uint8_t base1)
{
	/*
	IRQ �ϵ���� ���ͷ�Ʈ�� �߻��� �� ������ �۵��ϵ��� �ϱ� ���� PIC�� ���� IRQ�� �ʱ�ȭ ���־�� �Ѵ�.
	�̸� ���� ������ PIC�� ��� �������ͷ� ����� �����ؾ� �ϴµ� �̶� ICW(Initialization Control Word)�� ���ȴ�.
	�� ICW�� 4������ �ʱ�ȭ ��ɾ�� �����ȴ�.
	*/

	/*
	PIC ��Ʈ�ѷ��� �ʱ�ȭ �ϴ� �۾��� ICW1 Ŀ�ǵ带 I/O ��Ʈ 0x20 �Ǵ� 0xA0�� ���� ������ �����Ѵ�.
	0x20�� 0xA0 ��Ʈ�� ICW1�� ������ 0x21�� 0xA1 ��Ʈ�� ���� �����ʹ� ICW2,ICW3,ICW4 ������ �ؼ��Ǹ�,
	������ �Ϸ�Ǹ� PIC ��Ʈ�ѷ��� ���ŵ� �����͸� �������� �ڽ��� �ʱ�ȭ �Ѵ�.
	*/

	// 0 ������ PIC ��Ʈ, 1 �����̺� PIC ��Ʈ

	uint8_t	icw = 0;

	// PIC �ʱ�ȭ ICW1 ����� ������.
	// (icw & ~bit mask) | option 
	// ���� icw�� ��Ʈ ���� �����ϸ鼭 ��Ʈ ����ũ �κи� �ٲ۴�.
	icw = (icw & ~I86_PIC_ICW1_MASK_INIT) | I86_PIC_ICW1_INIT_YES;
	icw = (icw & ~I86_PIC_ICW1_MASK_IC4) | I86_PIC_ICW1_IC4_EXPECT;

	//SendCommandToPIC(icw, 0); // 0x11 (0001 0001) LTIM ��Ʈ = 0, SNGL ��Ʈ = 0, IC4 ��Ʈ = 1
	//SendCommandToPIC(icw, 1);
	SendCommandToPIC(0x11, 0);
	SendCommandToPIC(0x11, 1);

	//! PIC�� ICW2 ����� ������. base0�� base1�� IRQ�� ���̽� �ּҸ� �ǹ��Ѵ�.
	SendDataToPIC(base0, 0); // ������ PIC�� ���ͷ�Ʈ ���͸� 0x20(32) ���� ���ʷ� �Ҵ�
	SendDataToPIC(base1, 1); // �����̺� PIC�� ���ͷ�Ʈ ���͸� 0x28(40) ���� ���ʷ� �Ҵ�

	//PIC�� ICW3 ����� ������. �����Ϳ� �����̺� PIC���� ���踦 �����Ѵ�.
	SendDataToPIC(0x04, 0);
	SendDataToPIC(0x02, 1);

	//ICW4 ����� ������. i86 ��带 Ȱ��ȭ �Ѵ�.
	icw = (icw & ~I86_PIC_ICW4_MASK_UPM) | I86_PIC_ICW4_UPM_86MODE;

	SendDataToPIC(icw, 0);
	SendDataToPIC(icw, 1);
	//PIC �ʱ�ȭ �Ϸ�

	/*
	PIC�� �ʱ�ȭ �Ǹ�,
	EX) ������ Ű���带 ������ PIC�� �� ��ȣ�� �����ϰ� ���ͷ�Ʈ�� �߻�����
	�ü���� *��ϵ�* ���� �ڵ鷯�� �����Ų��! 
	*/
}

// 0�̸� 0x20��Ʈ(������ PIC ��Ʈ) 1�̸� 0xA0��Ʈ(�����̺� PIC ��Ʈ)
//PIC�� ����� ������
inline void SendCommandToPIC(uint8_t cmd, uint8_t picNum) {

	if (picNum > 1)
		return;

	uint8_t	reg = (picNum == 1) ? I86_PIC2_REG_COMMAND : I86_PIC1_REG_COMMAND;
	OutPortByte(reg, cmd);
}


//PIC�� �����͸� ������.
inline void SendDataToPIC(uint8_t data, uint8_t picNum) {

	if (picNum > 1)
		return;

	uint8_t	reg = (picNum == 1) ? I86_PIC2_REG_DATA : I86_PIC1_REG_DATA;
	OutPortByte(reg, data);
}


//PIC�κ��� 1����Ʈ�� �д´�
inline uint8_t ReadDataFromPIC(uint8_t picNum) {

	if (picNum > 1)
		return 0;

	uint8_t	reg = (picNum == 1) ? I86_PIC2_REG_DATA : I86_PIC1_REG_DATA;
	return InPortByte(reg);
}
