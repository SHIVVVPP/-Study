#pragma once
#include "windef.h"

/*
HAL (Hardware Abstraction Layer)
하드웨어 추상화 레이어
컴퓨터의 물리적인 하드웨어와 컴퓨터에서 실행되는 소프트웨어 사이의 추상화 계층이다.
컴퓨터에서 프로그램이 수만가지의 하드웨어를 별 차이 없이 다룰 수 있도록 OS에서 만들어주는 가교 역할을 한다.
HAL을 통해 필요한 기능을 OS에 요청하고 나머지는 OS가 알아서 처리해주도록 한다.
*/

/*
I/O 포트로 데이터 전송 및 받는 함수들을 따로 정리

키보드의 경우
	PC 내부 버스(BUS)와 포트 I/O 방식으로 연결되어 있으며, 포트 어드레스는 0x60와 0x64를 사용한다.
	실제로 할당된 포트는 두 개지만 포트에서 데이터를 읽을 때와 쓸 때 접근하는 레지스터가 다르므로,
	실제로는 네 개의 레지스터와 연결된 것과 같다.

PIT 컨트롤러의 경우
	PIT 컨트롤러는 1개의 컨트롤 레지스터와 3개의 카운터로 구성되며, I/O 포트 0x43, 0x40, 0x41, 0x42에 연결되어 있다.
	이중에서 컨트롤 레지스터는 쓰기만 가능하고, 컨트롤 레지스터를 제외한 카운터는 읽기와 쓰기가 모두 가능하다.

	... 
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


//PIT 만들면서 추가
void setvect(int intno, void(&vect)(), int flags = 0);