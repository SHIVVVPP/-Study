#pragma once
#include "windef.h"
#include "stdint.h"
#include "MultiBoot.h"
#include "HAL.h"

// �޸� ���� 4����Ʈ�� 32KB ������ �ٷ� �� �ִ�.
// �޸� �� ����� 4KB�� �����Ͽ����Ƿ� ���� �޸𸮸� ���� �ּ� �޸� �Ҵ� ũ��� 4KB(4 * 1024 Byte)�̴�.


//PMM - Physical Memory Manager
// 1����Ʈ�� ��� �� (1��Ʈ�� �ϳ��� ����� ��Ÿ����.)
#define PMM_BLOCKS_PER_BYTE	8	
// ��� �ϳ��� �޸� �Ҵ� ũ�� 4KB(4 * 1024 Byte)
#define PMM_MEMORY_PER_BLOCKS		4096
#define PMM_BLOCK_ALIGN		BLOCK_SIZE
// �ε��� �� ��ҿ� �ִ� bit ��
#define PMM_BITS_PER_INDEX	32

namespace PhysicalMemoryManager
{
	//==========================================================
	// Getter & Setter
	//==========================================================

	uint32_t GetMemoryMapSize();	// �޸� ���� ũ�⸦ ����.
	uint32_t GetKernelEnd();		// �ε�� Ŀ�� �ý����� ������ �ּҸ� ����.
	size_t	GetMemorySize(); 		// �޸� ����� ��´�.

	uint32_t GetTotalBlockCount();	// ����� ��ü ���� �����Ѵ�.
	uint32_t GetUsedBlockCount();	// ���� ��ϼ��� �����Ѵ�.
	uint32_t GetFreeBlockCount();	// ������ ���� ��ϼ��� �����Ѵ�.

	uint32_t GetFreeMemory();
	uint32_t GetBlockSize();		// ����� ����� �����Ѵ�. 4KB

//==========================================================
// Functions
//==========================================================

// INITIALIZE
	void Initialize(multiboot_info* bootinfo);
	// ���� �޸��� ��� ���ο� ���� �ʱ⿡ �����ӵ��� Set�ϰų� Unset�Ѵ�.
	void SetAvailableMemory(uint32_t base, size_t size);
	void SetDeAvailableMemory(uint32_t base, size_t size);

	// MEMORY BITMAP
		// ������, ���, �������� ��� ���� �ǹ��̴�.
		// �������� �ַ� ���� �ּҿ��� ���̰�, �������� �����ּҿ��� ���δ�.
		// �츮�� ����� �ü�� ������ ������(4K) ������(4K)��,
		// �ϳ��� ������, �������� ������ �޸� ũ�⸦ ��Ÿ����.
		// �����ӵ��� Set�ϰų� Unset�Ѵ�.
	void SetBit(int bit);
	void UnsetBit(int bit);

	bool TestMemoryMap(int bit);

	// MEMORY ALLOCATION
		// ����� �� �ִ� ������ ��ȣ�� ��´�.
	unsigned int GetFreeFrame();
	unsigned int GetFreeFrames(size_t size);

	// ����� �Ҵ��Ѵ�.
	void* AllocBlock();
	void* AllocBlocks(size_t size);

	// ����� �����Ѵ�. 
	void	FreeBlock(void* p);
	void	FreeBlocks(void* p, size_t size);


	//==========================================================
	// Control Register
	//==========================================================
	void EnablePaging(bool state);
	bool IsPaging();

	void		LoadPDBR(uint32_t physicalAddr);
	uint32_t	GetPDBR();



	//DEBUG
	void Dump();
}