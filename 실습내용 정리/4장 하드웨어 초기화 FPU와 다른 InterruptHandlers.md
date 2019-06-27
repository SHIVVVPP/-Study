# 4장 하드웨어 초기화 FPU와 다른 Interrupt Handlers



## 1. FPU 초기화와 FLOAT 출력 테스트

### 1. FPU.h

```c++
#pragma once

bool InitFPU();
bool EnableFPU();
```

### 2. FPU.cpp

```c++
#include "FPU.h"

extern "C" void __writecr4(unsigned __int64 Data);
extern "C" unsigned long __readcr4(void);

// FPU가 존재하는지 여부를 판단한다.
bool InitFPU()
{
	int result = 0;
	unsigned short temp;

	/*
	CR(Control Register)
	운영 모드를 변경, 현재 운영 중인 모드의 특정 기능을 제어하는 레지스터
	x86 프로세서에는 CR0~CR4 5개의 컨트롤 레지스터가 있으며
	x86-64 프로세서에는 CR8 추가

	CR0 - 운영모드를 제어하는 레지스터, 리얼모드에서 보호모드로 전환하는 역할, 캐시, 페이징 기능 등을 활성화
	CR1 - 프로세서에 의해 예약됨
	CR2 - 페이징 폴트 발생 시 페이지 폴트가 발생한 가상 주소가 저장되는 레지스터(페이징 활성화 후, 페이지 폴트시에만 유효)
	CR3 - 페이지 디렉터리의 물리 주소와 페이지 캐시에 관련된 기능을 설정
	CR4 - 프로세서에서 지원하는 확장 기능을 제어
		  페이지 크기 화장, 메모리 영역 확장 등의 기능을 활성화
	CR8 - 테스크 우선 순위 레지스터의 값을 제어,
		  프로세스 외부에서 발생하는 인터럽트 필터링, IA-32e 모드에서는 64비트로 확장
	*/
	__asm
	{
		pushad;			// 모든 레지스터를 스택에 저장한다.
		mov eax, cr0;	// eax = CR0
		and al, ~6;		// EM과 MP 플래그를 클리어한다. ~0110 => 1001
		mov cr0, eax;	// eax에 저장된 값을 cr0 레지스터에 저장한다.
		fninit; FPU;	// 상태를 초기화 한다.
		mov temp, 0x5A5A;	// FPU의 상태를 저장할 임시변수값을 0이 아닌 값으로 설정한다.
		fnstsw temp;		// FPU의 상태를 얻어온다.
		cmp temp, 0;		// 상태값이 0이면 FPU가 존재하지 않는다.
		jne noFPU;			// FPU가 존재하지 않으니 noFPU 레이블로 점프한다.
		fnstcw temp;		// FPU 제어값을 임시변수에 얻어오고
		mov ax, temp;		// 얻어온 값을 ax 레지스터에 저장한다.
		and ax, 0x103F;		// ax와 0x103F AND 연산을 수행한 뒤 ax에 저장한다.
		cmp ax, 0x003F;		// ax에 저장된 값과 0x003F 비교
		jne noFPU;			// 값이 틀리다면 FPU가 존재하지 않으므로 noFPU 레이블로 점프한다.
		mov result, 1;		// 이 구문이 실행되면 FPU가 존재하는 것이다.
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
```

### TEST : double float형 출력

#### 3. SkyConsole::Print() 에 %f 처리구문 추가 

```c++
case 'f':
{
    double double_temp;
    double_temp = va_arg(args, double);
    char buffer[512];
    ftoa_fixed(buffer, double_temp);
    Write(buffer);
    i++;
    break;
}
```

#### 4. kMain에 TestFPU() 추가와 FPU 초기화

```c++
/*  kMain.cpp */
#include "kMain.h"
#include "FPU.h" // FPU 헤더 추가
...
   
void TestFPU()
{
	float sampleFloat = 0.3f;
	sampleFloat *= 5.482f;
	SkyConsole::Print("Sample Float Value %f\n", sampleFloat);
}

// 위 multiboot_entry에서 ebx eax에 푸쉬한 멀티 부트 구조체 포인터(->addr), 매직넘버(->magic)로 호출
void kmain(unsigned long magic, unsigned long addr)
{
	SkyConsole::Initialize();

	SkyConsole::Print("*** MY OS Console System Init ***\n");

	// 하드웨어 초기화 과정중 인터럽트가 발생하지 않아야 하므로
	kEnterCriticalSection(&g_criticalSection);

	HardwareInitialize();
	SkyConsole::Print("Hardware Init Complete\n");
	
	kLeaveCriticalSection(&g_criticalSection);
	
    // FPU 초기화 추가
	if (false == InitFPU())
	{
		SkyConsole::Print("[Warning] Floating Pointer Unit(FPU) Detection Fail!\n");
	}
	else
	{
		EnableFPU();
		SkyConsole::Print("FPU Init...\n");
		TestFPU();
	}

	//타이머를 시작한다.
	//StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);
}
```

### FPU TEST 실행화면

![image](https://user-images.githubusercontent.com/34773827/60235610-50140c00-98e3-11e9-8ab6-5404d48f333f.png)



## 2. 다른 Interrupt Handler 추가하기

