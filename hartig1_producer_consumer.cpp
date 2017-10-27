#include <iostream>
#include <pthread.h>
#include <mutex>
#include <queue>
#include <unistd.h>
#define SIZE 10 //size of buffer
#define LOOP -1 //how many times each thread loops
		 //negative is infinite
using namespace std;
class mut {
public:
	void get(int id){
		if(!(this->lock)){ //if nobody has lock
			lock=true; //take lock
			return;
		} else { //somebody has lock
			q.push(id); //add process to queue of processes waiting for lock
			while(true){ //wait for lock
				if(!(this->lock)){
					int first = q.front();
					if(first == id){ //give lock to first in queue
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
	queue<int> q = queue<int>(); //queue of process ids
};
class cq{ //circular queue
	//does not do any checking
	//producer and consumer must check
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
	void p(){//print
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
	Producer(cq* buffer, mut* mu, int pid){
		this->buf = buffer;
		this->m = mu;
		this->id = pid;
	}
	void produce(){
		int num = rand() % 50;
		if(buf->full()){ //go to sleep if buf is full
			cout << "Producer sleeping" << endl;
			while(buf->full()){
				sleep(1);
			}
		}
		//otherwise wait for lock
		cout << "Producing: " << num << endl;
		m->get(this->id);
		buf->add(num);
		m->ret();
		struct timespec t1;
		t1.tv_sec=0;
		//to make processes alternate increase
		//time const below
		//this increases runtime since each
		//process waits longer
		t1.tv_nsec=250000000; //time const .25 seconds
		struct timespec t2;
		t2.tv_sec=0;
		t2.tv_nsec=0;
		//process needs to sleep to allow others to grab lock
		//VM only runs a single thread so it allows one process to continually run
		//other process will wake up and grab lock when this sleeps
		nanosleep(&t1,&t2);
	}
private:
	cq* buf;
	mut* m;
public:
	int id;
};
class Consumer {
public:
	Consumer(cq* buffer, mut* mu, int pid){
		this->buf = buffer;
		this->m = mu;
		this->id = pid;
	}
	void consume(){
		if(buf->empty()){
			cout << "Consumer sleeping" << endl;
			while(buf->empty()){
				sleep(1);
			}
		}
		m->get(this->id);
		int x = buf->rm();
		cout << "Consuming: " << x << endl;
		m->ret();
		struct timespec t1;
		t1.tv_sec=0;
		t1.tv_nsec=250000000;
		struct timespec t2;
		t2.tv_sec=0;
		t2.tv_nsec=0;
		nanosleep(&t1,&t2);
	}
private:
	cq* buf;
	mut* m;
public:
	int id;
};
void* runP(void* v){
	int x=0;
	Producer* p = (Producer*)v;
	while(x!=LOOP){
		p->produce();
		x++;
	}
}
void* runC(void* v){
	int x=0;
	Consumer* c = (Consumer*)v;
	while(x!=LOOP){
		c->consume();
		x++;
	}
}
int main(int argc, char* argv[]){
	cq test = cq();
	mut m = mut();
	Producer p = Producer(&test, &m, 0);
	Consumer c = Consumer(&test, &m, 1);
	pthread_t t1 = 1;
	pthread_t t2 = 2;
	int p1, p2;
	p1 = pthread_create(&t1, NULL, runP, (void*)(&p));
	p2 = pthread_create(&t2, NULL, runC, (void*)(&p));
	if(p1 ){
		cout << "Thread creation failed" << endl;
		return 0;
	}
	if(p2){
		cout << "Thread cxreation failed" << endl;
		return 0;
	}
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	test.p();
	cout<<endl;
	return 0;
}
