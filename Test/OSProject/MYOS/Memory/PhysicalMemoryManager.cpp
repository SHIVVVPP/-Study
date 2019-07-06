#include "CYNOS.h"
#include "Exception.h"

namespace PhysicalMemoryManager
{
//==========================================================
// LIKE PRIVATE MEMBER VARIABLE
//==========================================================
	uint32_t m_memorySize = 0;
	uint32_t m_usedBlocks = 0;

	// �̿��� �� �ִ� �ִ� ��� ����
	uint32_t m_maxBlocks = 0;

	// ��Ʈ�� �迭, �� ��Ʈ�� �޸� ���� ǥ���Ѵ�.
	uint32_t*	m_pMemoryMap = 0; // �޸� ���� ���� �ּ�
	uint32_t	m_memoryMapSize = 0;

	//memorySize : ��ü �޸��� ũ�� (����Ʈ ������)
	//bitmapAddr : Ŀ�� ������ ��ġ�Ǵ� ��Ʈ�� �迭
	uint32_t g_totalMemorySize = 0;

//==========================================================
// Getter & Setter
//==========================================================
#pragma region GETTER & SETTER

	uint32_t GetKernelEnd()			{ return (uint32_t)m_pMemoryMap;				}	
	uint32_t GetMemoryMapSize()		{ return m_memoryMapSize;						}
	size_t	 GetMemorySize()		{ return m_memorySize;							}

	uint32_t GetTotalBlockCount()	{ return m_maxBlocks;							}
	uint32_t GetUsedBlockCount()	{ return m_usedBlocks;							}
	uint32_t GetFreeBlockCount()	{ return m_maxBlocks - m_usedBlocks;			}

	uint32_t GetFreeMemory()		{ return GetFreeBlockCount() * PMM_MEMORY_PER_BLOCKS; }
	uint32_t GetBlockSize()			{ return PMM_MEMORY_PER_BLOCKS;						}
#pragma endregion



//==========================================================
// Variable Initialize from BootInfo
//==========================================================
#pragma region Variable Initialize from BootInfo

	uint32_t GetTotalMemory(multiboot_info* bootinfo)
	{
		uint64_t endAddress = 0;

		//	bootinfo->mmap_length
		//	mmap_���۴� multiboot_memory_map_t ����ü�� �̷���� �ִ�.
		//	mmapEntryNum�� multiboot_memory_map ����ü �迭�� ���� �����̴�.
		uint32_t mmapEntryNum = bootinfo->mmap_length / sizeof(multiboot_memory_map_t);
		// bootinfo->mmap_addr�� �ּ�
		multiboot_mmap_entry* mmapAddr = (multiboot_mmap_entry*)bootinfo->mmap_addr;

#ifdef _CYN_DEBUG
		CYNConsole::Print("Memory Map Entry Num : %d\n", mmapEntryNum);
#endif

		for (uint32_t i = 0; i < mmapEntryNum; i++)
		{
			uint64_t areaStart = mmapAddr[i].addr;
			uint64_t areaEnd = areaStart + mmapAddr[i].len;

			//CYNConsole::Print("0x%q 0x%q\n", areaStart, areaEnd);

			if (mmapAddr[i].type != 1)
			{
				continue;
			}

			if (areaEnd > endAddress)
				endAddress = areaEnd;
		}

		if (endAddress > 0xFFFFFFFF) {
			endAddress = 0xFFFFFFFF;
		}

		return (uint32_t)endAddress;
	}

	uint32_t GetKernelEnd(multiboot_info* bootinfo)
	{
		// Ŀ�� ������ ������ ���ĺ��� �޸� ��Ʈ�� ������ �����Ѵ�.
		// GRUB�� PE Ŀ���� �ε��� ���� Ŀ�� ũ�⸦ Ȯ���� ����� ����.
		// ���� �������� �ε�� ����� ������ �ּҷ� Ȱ���Ѵ�.
		// ��, Ŀ���� ũ���
		//		�������� �ε�� ����� ������ �ּ� - 0x100000�� �ȴ�.
		// ���� Ŀ�� ��ü�� ũ��� ���ʷ� �ε�Ǵ� ����� ���۵Ǵ� �ּ� - 0x100000�� �� ���̴�.
		// Ŀ���� ũ�⸦ ���ϱ� ���ؼ��� ����� �������� ���ϹǷ� �ּ� �ϳ� �̻��� �ε� �Ǿ�� �Ѵ�.
		uint64_t endAddress = 0;
		uint32_t mods_count = bootinfo->mods_count;   /* Get the amount of modules available */
		uint32_t mods_addr = (uint32_t)bootinfo->Modules;     /* And the starting address of the modules */
		for (uint32_t i = 0; i < mods_count; i++) {
			Module* module = (Module*)(mods_addr + (i * sizeof(Module)));     /* Loop through all modules */

			uint32_t moduleStart = PAGE_ALIGN_DOWN((uint32_t)module->ModuleStart); // HAL.h �� PAGE_ALIGNE_DOWN, PAGE_ALIGN_UP �߰�
			uint32_t moduleEnd = PAGE_ALIGN_UP((uint32_t)module->ModuleEnd);

			if (endAddress < moduleEnd)
			{
				endAddress = moduleEnd;
			}

			CYNConsole::Print("%x %x\n", moduleStart, moduleEnd);
		}

		return (uint32_t)endAddress;
	}

#pragma endregion


//==========================================================
// like PUBLIC Method
//==========================================================
	void Initialize(multiboot_info* bootinfo)
	{
		CYNConsole::Print("Physical Memory Manager Init..\n");
		// �ʱ� ��� ���� ����
		g_totalMemorySize = GetTotalMemory(bootinfo);
		// ���� ��� �� (400KB �޸𸮰� ������̶��, ���� ����� ���� 100�� �̴�.)
		m_usedBlocks = 0;
		// ���� �޸� ũ�� 128MB(�⺻ �������� ����)
		m_memorySize = g_totalMemorySize;
		// ����� �� �ִ� �ִ� ��� �� : 128MB/ 4KB(qemu���� 128MB�� ����) (��� �ϳ��� 4KB�� ��Ÿ���Ƿ�)
		m_maxBlocks = m_memorySize / PMM_MEMORY_PER_BLOCKS;

		// �������� ��.
		// �ϳ��� �������� 4KB
		// maxBlocks(����� �� ��) / PMM_BLOCKS_PER_BYTE(��� �ϳ��� ����Ʈ) / ������ �ϳ��� ũ��
		// ��ϵ��� �����ϴ� ������ �� = �޸� ���� �Ҵ��ϱ� ���� �ʿ��� ������ ��
		int pageCount = m_maxBlocks / PMM_BLOCKS_PER_BYTE / PAGE_SIZE;
		if (pageCount == 0)
			pageCount = 1;

		m_pMemoryMap = (uint32_t*)GetKernelEnd(bootinfo);

		// 1MB = 1048576 byte
		CYNConsole::Print("Total Memory (%dMB)\n", g_totalMemorySize / 1048576);
		CYNConsole::Print("BitMap Start Address(0x%x)\n", m_pMemoryMap);
		CYNConsole::Print("Page Count (%d)\n", pageCount);
		CYNConsole::Print("BitMap Size(0x%x)\n", pageCount * PAGE_SIZE);

		//������ �ִ� ���� 8�� ����� ���߰� �������� ������
		//m_maxBlocks = m_maxBlocks - (m_maxBlocks % PMM_BLOCKS_PER_BYTE);

		// �޸𸮸��� ����Ʈũ��
		// m_memoryMapSize = ��Ʈ�� �迭�� ũ��, �� ũ�Ⱑ 4KB��� 128MB�� �޸𸮸� ������ �� �ִ�.
		m_memoryMapSize = m_maxBlocks / PMM_BLOCKS_PER_BYTE;
		m_usedBlocks = GetTotalBlockCount();

		// �޸� ���� ũ��
		int tempMemoryMapSize = (GetMemoryMapSize() / 4096) * 4096;

		// �޸� ���� ũ��� ��� ������ �������� �Ѵ�.
		// PMM_MEMORY_PER_BLOCK = 4096�̱� ������ 4096 ������ ũ�Ⱑ �������� �Ѵ�.
		if (GetMemoryMapSize() % 4096> 0) 
			tempMemoryMapSize += 4096;

		m_memoryMapSize = tempMemoryMapSize;

		//��� �޸� ������ ����߿� �ִٰ� �����Ѵ�.	
		unsigned char flag = 0xff;
		memset((char*)m_pMemoryMap, flag, m_memoryMapSize);
		//// �̿� ������ �޸� ����� �����Ѵ�.
		SetAvailableMemory((uint32_t)m_pMemoryMap, m_memorySize);
	}

	// �޸� ���� ��� ���θ� �����Ѵ�.
	void SetAvailableMemory(uint32_t base, size_t size)
	{
		// ���⼭ �޸� ���� �޸𸮸� ����ϰ� �ֱ� ������
		// �޸� �ʿ��� ���Ǵ� ��� �� ��ŭ�� �޸𸮴�  ���ǰ� �ִٰ� ó���Ѵ�.
		int usedBlock = GetMemoryMapSize() / PMM_MEMORY_PER_BLOCKS;
		int blocks = GetTotalBlockCount();

		// (KernelEndAddress ���� ���Ǳ� ������ �ʱ� i�� usedBlock���� ����)
		for (int i = usedBlock; i < blocks; i++) 
		{
			UnsetBit(i);
			m_usedBlocks--;
		}
	}

	void SetDeAvailableMemory(uint32_t base, size_t size)
	{
		int align = base / PMM_MEMORY_PER_BLOCKS;
		int blocks = size / PMM_MEMORY_PER_BLOCKS;

		for (; blocks > 0; blocks--) 
		{
			SetBit(align++);
			m_usedBlocks++;
		}
	}

	/*
	8��° �޸� ���� ��������� ǥ���ϱ� ���� 1�� �����Ϸ���
	�迭 ù��° ��� (4����Ʈ) ����Ʈ�� 8��° ��Ʈ�� �����ؾ� �Ѵ�.
	�޸� ��Ʈ���� 4����Ʈ�� 32���� ����� ǥ���� �� �����Ƿ�,
	1301 �޸� ����� ���� ���
	1301 / 32 = 40, 1301 % 32 = 1
	��, �ε��� 1301 ����� ���ǰ� ������ ��Ÿ���� ����
	m_pMemoryMap[40]�� ������ ��,
	4����Ʈ �� 32��Ʈ �߿��� �� ��° ��Ʈ�� 1�� �����ϸ� 1301 ����� ��������� ��Ÿ�� �� �ִ�.
	1 << (1301 % 32 = 21) => 00100000 00000000 00000000 00000000
	��������� ������ ��Ÿ�� ����
	~(1 << (1301 % 32 = 21)) => 11011111 11111111 11111111 11111111
	*/
	void SetBit(int bit)
	{
		m_pMemoryMap[bit / 32] |= (1 << (bit % 32));
	}

	void UnsetBit(int bit)
	{
		m_pMemoryMap[bit / 32] &= ~(1 << (bit % 32));
	}

	// �ش� ��Ʈ�� ��Ʈ�Ǿ� �ִ��� ���θ� üũ�Ѵ�.
	// ��Ʈ�� ��ȿ���� ����� 0�� �����Ѵ�.
	bool TestMemoryMap(int bit)
	{
		return (m_pMemoryMap[bit / 32] & (1 << (bit % 32))) > 0;
	}

	

	

	//��Ʈ�� 0�� ������ �ε����� ����.
	unsigned int GetFreeFrame()
	{
		for (uint32_t i = 0; i < GetTotalBlockCount() / 32; i++)
		{
			if (m_pMemoryMap[i] != 0xffffffff)		// i ��° �޸� ���� ��� ��������� üũ
				for (unsigned int j = 0; j < PMM_BITS_PER_INDEX; j++)
				{
					unsigned int bit = 1 << j;	// �ڸ��� ��Ʈ
					if ((m_pMemoryMap[i] & bit) == 0)	// �ش� �ڸ��� ������� �ƴ϶��
						return i * PMM_BITS_PER_INDEX + j;	// ���° ������� ����� �����Ѵ�. i * m_pMemoryMap �� ��Ҵ� ��� �� + j
				}
		}

		return 0xffffffff;
	}
	   
	// ���ӵ� �� ������(��)���� ����.
	unsigned int GetFreeFrames(size_t size)
	{
		if (size == 0)
			return 0xffffffff;

		if (size == 1)
			return GetFreeFrame();



		for (uint32_t i = 0; i < GetTotalBlockCount() / 32; i++)
		{
			if (m_pMemoryMap[i] != 0xffffffff)
			{
				for (unsigned int j = 0; j < 32; j++)
				{
					unsigned int bit = 1 << j;
					if ((m_pMemoryMap[i] & bit) == 0)
					{

						unsigned int startingBit = i * PMM_BITS_PER_INDEX + j;

						// ���ӵ� �� �������� ������ ������Ų��.

						uint32_t free = 0;
						for (uint32_t count = 0; count < size; count++)
						{
							//�޸𸮸��� ����� ��Ȳ
							if (startingBit + count >= m_maxBlocks)
								return 0xffffffff;

							if (TestMemoryMap(startingBit + count) == false)
								free++;
							else
								break;

							//���ӵ� �� �����ӵ��� �����Ѵ�. ���� ��Ʈ �ε����� startingBit
							if (free == size)
								return startingBit;
						}
					}
				}
			}
		}

		return 0xffffffff;
	}

	void* AllocBlock()
	{
		// �̿��� �� �ִ� ����� ���ٸ� �Ҵ� ����
		if (GetFreeBlockCount() <= 0)
			return NULL;
		// ���ǰ� ���� ���� ����� �ε����� ��´�.
		unsigned int frame = GetFreeFrame();

		// GetFreeFrame���� ��ȿ�� �������� ���ٸ� -1�� �����ϹǷ� ����ó��
		if (frame == -1)
			return NULL;

		// �޸� ��Ʈ�ʿ� �ش� ����� ���ǰ� ������ �����Ѵ�.
		SetBit(frame);

		// �Ҵ�� ���� �޸� �ּҸ� �����Ѵ�.
		uint32_t addr = frame * PMM_MEMORY_PER_BLOCKS + (uint32_t)m_pMemoryMap;
		m_usedBlocks++;

		return (void*)addr;
	}

	void FreeBlock(void* p)
	{
		uint32_t addr = (uint32_t)p;
		int frame = addr / PMM_MEMORY_PER_BLOCKS;

		UnsetBit(frame);

		m_usedBlocks--;
	}


	void* AllocBlocks(size_t size)
	{
		
		if (GetFreeBlockCount() <= size)
		{
			return NULL;
		}


		int frame = GetFreeFrames(size);

		if (frame == -1)
		{
			return NULL;	//���ӵ� �� ������ �������� �ʴ´�.
		}

		for (uint32_t i = 0; i < size; i++)
			SetBit(frame + i);

		uint32_t addr = frame * PMM_MEMORY_PER_BLOCKS + (uint32_t)m_pMemoryMap;
		m_usedBlocks += size;

		return (void*)addr;
	}


	void FreeBlocks(void* p, size_t size)
	{
		uint32_t addr = (uint32_t)p - (uint32_t)m_pMemoryMap;
		int frame = addr / PMM_MEMORY_PER_BLOCKS;

		for (uint32_t i = 0; i < size; i++)
			UnsetBit(frame + i);

		m_usedBlocks -= size;
	}
	 
//==========================================================
// Control Register (PAGING & PDBR)
//==========================================================
	void EnablePaging(bool state)
	{
#ifdef _MSC_VER
		_asm
		{
			mov	eax, cr0
			cmp[state], 1
			je	enable
			jmp disable
			enable :
			or eax, 0x80000000		//set bit 31
			mov	cr0, eax
			jmp done
			disable :
			and eax, 0x7FFFFFFF		//clear bit 31
				mov	cr0, eax
				done :
		}
#endif
	}

	bool IsPaging()
	{
		uint32_t res = 0;

#ifdef _MSC_VER
		_asm {
			mov	eax, cr0
			mov[res], eax
		}
#endif
		// 8000 0000 = 10000000 00000000 00000000 00000000
		return (res & 0x80000000) ? false : true;
	}

	void LoadPDBR(uint32_t physicalAddr)
	{
#ifdef _MSC_VER
		_asm
		{
			mov eax, [physicalAddr]
			mov cr3, eax // PDBR is cr3 register in i86
		}
#endif
	}

	uint32_t GetPDBR()
	{
#ifdef _MSC_VER
		_asm
		{
			mov eax, cr3
			ret
		}
#endif
	}

	void Dump()
	{
		CYNConsole::Print("Memory Size : 0x%x\n", m_memorySize);
		CYNConsole::Print("Memory Map Address : 0x%x\n", m_pMemoryMap);
		CYNConsole::Print("Memory Map Size : %d bytes\n", m_memoryMapSize);
		CYNConsole::Print("Max Block Count : %d\n", m_maxBlocks);

		CYNConsole::Print("Used Block Count : %d\n", m_usedBlocks);
	}

}