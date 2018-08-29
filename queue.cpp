#include <windows.h>

class lockfreeQueue {
	lockfreeQueue() {
		in = new node;
		out = in;
		in->next = NULL;
	}

	void inqueue(void* savedata) {
		node* next = new node;
		node* save = NULL;
		next->data = savedata;
		save = (node*)InterlockedExchangePointer((PVOID*)&in, next);
		(node*)InterlockedExchangePointer((PVOID*)&(save->next), next);
	}

	void* dequeue() {
		while (true) {
			temp = (node*)InterlockedExchangePointer((PVOID*)&(out->next), temp);
			if (temp != NULL) {
				(node*)InterlockedExchangePointer((PVOID*)&out, temp);
				data = temp->data;
				delete(temp);
				return data;
			}
		}
	}
  
private:
	struct node {
		__declspec(align(64))node* next;
		void* data;
	};

	__declspec(align(64))node* in;
	__declspec(align(64))node* out;

	__declspec(align(64))LONG counter;
};
