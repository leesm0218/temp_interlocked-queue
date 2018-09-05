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
			bufferSize = 1LL << logTwoSize;
			buffer = new node[bufferSize];

			for (counter = 0; counter < bufferSize; counter++) {
				buffer[counter].data = NULL;
				buffer[counter].inOrder = 0;
				buffer[counter].outOrder = 0;
			}

			localShift = logTwoSize;
			localMax = memberMaxOrder >> localShift;
			localMask = ~(memberMaxOrder << logTwoSize);
			int a = 0;
		}
		else {

		}
	}

	void push(ULONG64* data) {
		ULONG64 localOrder = InterlockedIncrement(&memberInOrder);
		if (localOrder > memberMaxOrder) {
			InterlockedAnd(&memberInOrder, memberMaxOrder);
			localOrder = localOrder & memberMaxOrder;
			if (localOrder == 0) {
				localOrder = memberMaxOrder;
			}
			else {
				localOrder = localOrder - 1;
			}
		}
		else {
			localOrder = localOrder - 1;
		}

		ULONG64 inLocation = localOrder & localMask;
		ULONG64 inOrder = localOrder >> localShift;

		while (buffer[inLocation].inOrder != inOrder) {}
		while (buffer[inLocation].data != NULL) {}

		buffer[inLocation].data = data;
		InterlockedIncrement(&buffer[inLocation].inOrder);

		if (buffer[inLocation].inOrder > localMax) {
			InterlockedAnd(&(buffer[inLocation].inOrder), localMax);
		}
		if (localOrder % 10000 == 0) {
			std::cout << localOrder << "push\n";
		}

	}

	ULONG64* pop() {
		ULONG64 localOrder = InterlockedIncrement(&memberOutOrder);
		if (localOrder > memberMaxOrder) {
			InterlockedAnd(&memberOutOrder, memberMaxOrder);
			localOrder = localOrder & memberMaxOrder;
			if (localOrder == 0) {
				localOrder = memberMaxOrder;
			}
			else {
				localOrder = localOrder - 1;
			}
		}
		else {
			localOrder = localOrder - 1;
		}

		ULONG64 outLocation = localOrder & localMask;
		ULONG64 outOrder = localOrder >> localShift;

		while (buffer[outLocation].outOrder != outOrder) {}
		while (buffer[outLocation].data == NULL) {}

		ULONG64* data = buffer[outLocation].data;
		buffer[outLocation].data = NULL;
		InterlockedIncrement(&buffer[outLocation].outOrder);

		if (buffer[outLocation].outOrder > localMax) {
			InterlockedAnd(&(buffer[outLocation].outOrder), localMax);
		}

		if (localOrder % 10000 == 0) {
			std::cout << localOrder << "pop\n";
		}
		return data;

	}

private:
	struct node {
		__declspec(align(64))ULONG64 inOrder = 0;
		__declspec(align(64))ULONG64 outOrder = 0;
		__declspec(align(64))ULONG64* data = NULL;
	};

	__declspec(align(64))volatile node* buffer;
	__declspec(align(64))ULONG64 memberInOrder = 0;
	__declspec(align(64))ULONG64 memberOutOrder = 0;
	__declspec(align(64))ULONG64 memberMaxOrder = (MAXULONG64) >> 1;

	__declspec(align(64))ULONG64 localShift;
	__declspec(align(64))ULONG64 localMax;
	__declspec(align(64))ULONG64 localMask;


	__declspec(align(64))ULONG64 bufferSize;
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
