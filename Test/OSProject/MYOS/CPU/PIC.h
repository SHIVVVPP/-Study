// 8259 Programmable Interrupt Contoroller
#pragma once
#include <stdint.h>

/*
�ϵ���� ��ġ�� �����͸� �ְ���� �� �ְ� �ϴ� Ʋ�� �����Ѵ�.
*/

/*
PIC - Programmable Interrupt Controller
Ű���峪 ���콺 ���� �̹�Ʈ ���� CPU�� �����ϴ� �����
ex) ������ Ű���带 ������ PIC�� �� ��ȣ�� �����ϰ� ���ͷ�Ʈ�� �߻����� �ü���� ��ϵ� ���� �ڵ鷯�� �����Ų��.
�̷������� Ű����, ���콺, �÷��ǵ�ũ, �ϵ��ũ �� �پ��� ��ü�� ���� ���ͷ�Ʈ�� �߻��ǰ� OS�� �̰��� �����Ѵ�.
 - OS�� ����̽� �ĺ��� �����ؾ� �Ѵ�.

OS�� IRQ(���ͷ�Ʈ ������Ʈ) ��ȣ�� ���� �ش� ���ͷ�Ʈ�� ���� ��ȣ�� �� �� �ְ�, �� ��ȣ�� ���� �ش� ���ͷ�Ʈ��
��ó�� ������� �� �� �ִ�.
*/

//���ͷ�Ʈ�� �߻���Ű�� ���� PIC1�� ����ϴ� ����̽� ����Ʈ // Master
#define		I86_PIC_IRQ_TIMER			0
#define		I86_PIC_IRQ_KEYBOARD		1
#define		I86_PIC_IRQ_SERIAL2			3
#define		I86_PIC_IRQ_SERIAL1			4
#define		I86_PIC_IRQ_PARALLEL2		5
#define		I86_PIC_IRQ_DISKETTE		6
#define		I86_PIC_IRQ_PARALLEL1		7

//���ͷ�Ʈ�� �߻���Ű�� ���� PIC2�� ����ϴ� ����̽� ����Ʈ // Slave
#define		I86_PIC_IRQ_CMOSTIMER		0
#define		I86_PIC_IRQ_CGARETRACE		1
#define		I86_PIC_IRQ_AUXILIARY		4
#define		I86_PIC_IRQ_FPU				5
#define		I86_PIC_IRQ_HDC				6

/*
PIC�� IRQ���� �۵�
	������
	1. ������ PIC���� ���ͷ�Ʈ�� �߻��Ѵ�.
	2. ������ PIC�� �ڽ��� INT�� ��ȣ�� �ư� CPU�� INT�� �����Ѵ�.
	3. CPU�� ���ͷ�Ʈ�� ������ EFLAG�� IE ��Ʈ�� 1�� �����ϰ� INTA�� ���� �޾Ҵٴ� ��ȣ�� PIC�� �����Ѵ�.
	4. PIC�� �ڽ��� INTA�� ���� �� ��ȣ�� �ް� � IRQ�� ����� ��ġ���� ���ͷ�Ʈ�� �߻��ߴ���
	   ������ ������ ���� CPU�� �����Ѵ�.
	5. CPU�� ���� �����尡 ��ȣ ����� IDT ��ũ���͸� ã�Ƽ� ���ͷ�Ʈ �ڵ鷯�� �����Ѵ�.

	�����̺�
	1. �����̺� PIC���� ���ͷ�Ʈ�� �߻��Ѵ�.
	2. �����̺� PIC�� �ڽ��� INT�ɿ� ��ȣ�� �ư� ������ PIC IRQ 2���� ���ͷ�Ʈ ��ȣ�� ������.
	3. �����ʹ� ������ ������ 5������ ������ �����Ѵ�. 
	   ��, �̰������� CPU�� �� ��° IRQ���� ���ͷ�Ʈ�� �߻��ߴ��� �˷��� ������ 8~15���� �ȴ�.
*/


//--------------------------------------------
//		����̽��� �����ϱ� ���� Ŀ�ǵ�
//--------------------------------------------

// common word 2 bit masks. Ŀ�ǵ带 ���� �� ����Ѵ�.
#define		I86_PIC_OCW2_MASK_L1		1		//00000001
#define		I86_PIC_OCW2_MASK_L2		2		//00000010
#define		I86_PIC_OCW2_MASK_L3		4		//00000100
#define		I86_PIC_OCW2_MASK_EOI		0x20	//00100000
#define		I86_PIC_OCW2_MASK_SL		0x40	//01000000
#define		I86_PIC_OCW2_MASK_ROTATE	0x80	//10000000

//! Command Word 3 bit masks. Ŀ�ǵ带 ������ ���
#define		I86_PIC_OCW3_MASK_RIS		1		//00000001
#define		I86_PIC_OCW3_MASK_RIR		2		//00000010
#define		I86_PIC_OCW3_MASK_MODE		4		//00000100
#define		I86_PIC_OCW3_MASK_SMM		0x20	//00100000
#define		I86_PIC_OCW3_MASK_ESMM		0x40	//01000000
#define		I86_PIC_OCW3_MASK_D7		0x80	//10000000


void PICInitialize(uint8_t base0, uint8_t base1);

// PIC�κ��� 1����Ʈ�� �д´�.
uint8_t ReadDataFromPIC(uint8_t picNum);

// PIC�� �����͸� ������.
void SendDataToPIC(uint8_t data, uint8_t picNum);

// PIC�� ��ɾ �����Ѵ�.
void SendCommandToPIC(uint8_t cmd, uint8_t picNum);

