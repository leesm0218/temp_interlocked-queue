#pragma once

#include <iostream>
#include <string>
#include <thread>
#include <windows.h>

class lockfreeQueue {
public:
	lockfreeQueue(int logTwoSize) {
		int counter = 0;
		if (logTwoSize > 4 && logTwoSize < 10) {
			buffer_size = 1LL << logTwoSize;
			buffer = new __declspec(align(64))node[buffer_size];
			for (counter = 0; counter < buffer_size; counter++) {
				buffer[counter].data = NULL;
				buffer[counter].local_inOrder = 0;
				buffer[counter].local_outOrder = 0;
			}
			local_shift = logTwoSize;
			local_max = global_max >> local_shift;
			local_location_mask = ~(global_max << logTwoSize);
			int a = 0;
		}
		else {

		}
	}

	void push(ULONG64* data) {
		ULONG64 global_order = InterlockedIncrement(&global_inOrder);
		if (global_order > global_max) {
			InterlockedAnd(&global_inOrder, global_max);
			global_order = global_order & global_max;
			if (global_order == 0) {
				global_order = global_max;
			}
			else {
				global_order = global_order - 1;
			}
		}
		else {
			global_order = global_order - 1;
		}

		ULONG64 local_location = global_order & local_location_mask;
		ULONG64 local_order = global_order >> local_shift;

		while (buffer[local_location].local_inOrder != local_order) {		}
		while (buffer[local_location].data != NULL) {		}

		buffer[local_location].data = data;
		InterlockedIncrement(&buffer[local_location].local_inOrder);

		if (buffer[local_location].local_inOrder > local_max) {
			InterlockedAnd(&(buffer[local_location].local_inOrder), local_max);
		}

		if (global_order % 10000 == 0) {
			std::cout << global_order / 10000 << "0000push\n";
		}

	}

	ULONG64* pop() {
		ULONG64 global_order = InterlockedIncrement(&global_outOrder);
		if (global_order > global_max) {
			InterlockedAnd(&global_outOrder, global_max);
			global_order = global_order & global_max;
			if (global_order == 0) {
				global_order = global_max;
			}
			else {
				global_order = global_order - 1;
			}
		}
		else {
			global_order = global_order - 1;
		}

		ULONG64 local_location = global_order & local_location_mask;
		ULONG64 local_order = global_order >> local_shift;

		while (buffer[local_location].local_outOrder != local_order) {		}

		while (buffer[local_location].data == NULL) {		}

		ULONG64* data = buffer[local_location].data;
		buffer[local_location].data = NULL;
		InterlockedIncrement(&buffer[local_location].local_outOrder);

		if (buffer[local_location].local_outOrder > local_max) {
			InterlockedAnd(&(buffer[local_location].local_outOrder), local_max);
		}

		if (global_order % 10000 == 0) {
			std::cout << global_order / 10000 << "0000pop\n";
		}
		return data;

	}

private:
	struct node {
		__declspec(align(64))ULONG64 local_inOrder = 0;
		__declspec(align(64))ULONG64 local_outOrder = 0;
		__declspec(align(64))ULONG64* data = NULL;
	};

	__declspec(align(64))volatile node* buffer;
	__declspec(align(64))ULONG64 global_inOrder = 0;
	__declspec(align(64))ULONG64 global_outOrder = 0;

	__declspec(align(64))ULONG64 global_max = (MAXULONG64) >> 1;

	__declspec(align(64))ULONG64 local_shift;
	__declspec(align(64))ULONG64 local_max;
	__declspec(align(64))ULONG64 local_location_mask;


	__declspec(align(64))ULONG64 buffer_size;
};


const int test_size = 20;
int in_count = 100;

void push_proc(lockfreeQueue* LFQ) {
	while (true) {
		LFQ->push(new ULONG64(50));
	}
}

void only_pop_proc(lockfreeQueue* LFQ) {
	ULONG64* temp = NULL;
	while (true) {
		temp = LFQ->pop();
		if (temp != NULL) {
			if (*temp == 50) {
				delete(temp);
				temp = NULL;
			}
			else {
				std::cout << "error!50\n";
				return;
			}
		}
		else {
			std::cout << "error null pop\n";
			return;
		}
	}
}

void pop_proc(lockfreeQueue* LFQ) {

	int count = 0;
	const int size = 100;
	ULONG64* buffer[size];
	
	for (count = 0; count < size; count++) {
		buffer[count] = NULL;
	}

	count = 0;
	while (true) {
		buffer[count] = LFQ->pop();
		if (buffer[count] != NULL) {
			InterlockedIncrement(buffer[count]);
			count = count + 1;
			count = count % size;
			if (buffer[count] != NULL) {
				if (*buffer[count] == 51) {
					delete(buffer[count]);
					buffer[count] = NULL;
				}
				else {
					std::cout << "error!50\n";
					return;
				}
			}
		}
		else {
			std::cout << "error null pop\n";
			return;
		}
	}
}

int main() {
	std::string stop;
	lockfreeQueue lfq(5);
	//std::thread thread00(only_pop_proc, &lfq);

	std::thread thread01(push_proc, &lfq);
	std::thread thread02(push_proc, &lfq);
	std::thread thread03(push_proc, &lfq);
	std::thread thread04(push_proc, &lfq);
	std::thread thread05(push_proc, &lfq);
	std::thread thread06(push_proc, &lfq);
	std::thread thread07(push_proc, &lfq);
	std::thread thread08(push_proc, &lfq);

	std::thread thread1(pop_proc, &lfq);
	std::thread thread2(pop_proc, &lfq);
	std::thread thread3(pop_proc, &lfq);
	std::thread thread4(pop_proc, &lfq);
	std::thread thread5(pop_proc, &lfq);
	std::thread thread6(pop_proc, &lfq);
	std::thread thread7(pop_proc, &lfq);
	std::thread thread8(pop_proc, &lfq);

	thread1.join();
	thread2.join();
	thread3.join();
	thread4.join();
	thread5.join();
	thread6.join();
	thread7.join();
	thread8.join();

	while (true) {}

	return 0;
}
