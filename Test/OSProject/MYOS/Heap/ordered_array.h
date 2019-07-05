// ordered_array.h --	interface for creating, inserting and deleting
//						from ordered arrays.
//						

#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#include "windef.h"

// ordered_array�� ���Խ� �����Ѵ�.( �׻� ���ĵ� ���¸� �����Ѵ�. )
// � Ÿ���̵� ��� ���� ������ �� �ֵ��� type_t �� void*�� �Ѵ�.
typedef void* type_t;

// ù ��° �Ķ���Ͱ� �� ��° �Ķ���ͺ��� ���� ���, predicate�� 0�� �ƴ� ������ ��ȯ�ؾ� �Ѵ�. �׷��� ������ 0�� ��ȯ�Ѵ�.
typedef s8int(*lessthan_predicate_t)(type_t, type_t);

typedef struct
{
	type_t* array;
	u32int size;
	u32int max_size;
	lessthan_predicate_t less_than;
}ordered_array_t;

// �� �Լ�
s8int standard_lessthan_predicate(type_t a, type_t b);

// ordered_array �����Լ�
ordered_array_t create_ordered_array(u32int max_size, lessthan_predicate_t less_than);
ordered_array_t place_ordered_array(void* addr, u32int max_size, lessthan_predicate_t less_than);

void destroy_ordered_array(ordered_array_t* array);

void insert_ordered_array(type_t item, ordered_array_t* array);


type_t lookup_ordered_array(u32int i, ordered_array_t* array);

void remove_ordered_array(u32int i, ordered_array_t* array);


#endif