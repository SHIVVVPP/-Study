#pragma once
#include "windef.h"

/*
HAL (Hardware Abstraction Layer)
�ϵ���� �߻�ȭ ���̾�
��ǻ���� �������� �ϵ����� ��ǻ�Ϳ��� ����Ǵ� ����Ʈ���� ������ �߻�ȭ �����̴�.
��ǻ�Ϳ��� ���α׷��� ���������� �ϵ��� �� ���� ���� �ٷ� �� �ֵ��� OS���� ������ִ� ���� ������ �Ѵ�.
HAL�� ���� �ʿ��� ����� OS�� ��û�ϰ� �������� OS�� �˾Ƽ� ó�����ֵ��� �Ѵ�.
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