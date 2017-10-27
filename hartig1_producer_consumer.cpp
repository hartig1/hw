#include <iostream>
#include <pthread.h>
#include <mutex>
#include <queue>
#include <unistd.h>
#define SIZE 10
using namespace std;
class mut {
public:
	void get(void* caller){
		//cout << lock << endl;
		if(!(this->lock)){
			lock=true;
			//sleep(1);
			return;
		} else {
			cout << (int)caller << " in queue" << endl;
			q.push(caller);
			while(true){
				if(!(this->lock)){
					void* first = q.front();
					if(first == caller){
						return ;
					}
				}
			}
		}
	}
	void ret(){
		lock = false;
	}
private:
	bool lock = false;
	queue<void*> q = queue<void*>();
};
class cq{
public:
	cq(){
		for(int x=0; x< SIZE; x++){
			this->buf[x] = -1;
		}
	}
	void add(int x){
		buf[in] = x;
		in++;
		space--;
		//cout << space <<endl;
		in %= SIZE;
	}
	int rm(){
		int x = buf[out];
		buf[out] = -1;
		space++;
		out++;
		out %= SIZE;
		return x;
	}
	bool full(){
		if(space == 0){
			return true;
		}
		return false;
	}
	bool empty(){
		if(space == SIZE){
			return true;
		}
		return false;
	}
	void p(){
		for(int x=0; x<SIZE; x++){
			cout << buf[x] << ",";
		}
	}
private:
	int in = 0;
	int out = 0;
	int buf[SIZE];
	int space = SIZE;
};
class Producer {
public:
	Producer(cq* buffer, mut* m){
		this->buf = buffer;
		this->m = m;
	}
	void produce(){
		int num = rand() % 50;
		if(buf->full()){
			cout << "Producer sleeping" << endl;
			while(buf->full()){
				sleep(1);
			}
		}
		cout << "Producing: " << num << endl;
		//sleep(1);
		m->get((void*)this);
		cout << "Producer get" << endl;
		buf->add(num);
		m->ret();
		//sleep(1);
	}
private:
	cq* buf;
	mut* m;
};
class Consumer {
public:
	Consumer(cq* buffer, mut* m){
		this->buf = buffer;
		this->m = m;
	}
	void consume(){
		if(buf->empty()){
			cout << "Consumer sleeping" << endl;
			while(buf->empty()){
				sleep(1);
			}
		}
		//sleep(1);
		m->get((void*)this);
		cout << "consumer get" << endl;
		int x = buf->rm();
		cout << "Consuming: " << x << endl;
		m->ret();
		//sleep(1);
	}
private:
	cq* buf;
	mut* m;
};
void* runP(void* v){
	cout << "Here" << endl;
	Producer* p = (Producer*)v;
	while(true){
		p->produce();
	}
}
void* runC(void* v){
	Consumer* c = (Consumer*)v;
	while(true){
		c->consume();
	}
}
int main(int argc, char* argv[]){
	cq test = cq();
	mut m = mut();
	Producer p = Producer(&test, &m);
	Consumer c = Consumer(&test, &m);
	/*for(int x=0;x<SIZE+1;x++){
	p.produce();
	}*/
	int loop =1000;
	pthread_t t1 = 1;
	pthread_t t2 = 2;
	int p1, p2;
	p1 = pthread_create(&t1, NULL, runP, (void*)(&p));
	p2 = pthread_create(&t2, NULL, runC, (void*)(&p));
	if(p1){
		cout << "Fail" << endl;
	};
	pthread_join(t1, NULL);
	test.p();
	cout<<endl;
	return 0;
}
