#ifndef THREAD_BUFFER_H_
#define THREAD_BUFFER_H_

#include "emp-swap/constant.h"
namespace emp{

template<class T>
class ThreadBuffer{

public:
	void push(const T &x){
		std::unique_lock<std::mutex> lck(mtx);
		if(q.size()>=THREAD_BUFFER_SIZE*2)
			c_full.wait(lck);
		q.push(x);
		c_empt.notify_all();
	}
	void push(const T *data,int len){

		if(len>THREAD_BUFFER_SIZE){
			for(int i=0;i<len;i++)
				push(data[i]);
			return ;
		}

		std::unique_lock<std::mutex> lck(mtx);
		while(q.size()+len>=THREAD_BUFFER_SIZE*2)
			c_full.wait(lck);
		for(int i=0;i<len;i++)
			q.push(data[i]);
		c_empt.notify_all();
	}

	T get(){
		std::unique_lock<std::mutex> lck(mtx);
		if(!q.size())
			c_empt.wait(lck);
		T res=q.front();
		q.pop();
		c_full.notify_all();
		return res;
	}
	void get(T *data,int len){
		std::unique_lock<std::mutex> lck(mtx);
		while((int)q.size()<len)
			c_empt.wait(lck);
		for(int i=0;i<len;i++){
			data[i]=q.front();
			q.pop();
		}
		c_full.notify_all();
	}


    int size(){
        std::unique_lock<std::mutex> lck(mtx);
        return q.size();
    }

    void greedy_ensure_size(int num,long long all){
		int last=size(); 
		if(last>=THREAD_BUFFER_SIZE)
			return ;
		while(1){
			usleep(1500);
			int news=size();
			if(news>=THREAD_BUFFER_SIZE || (last==news&&last>=num))
				break;
			last=news;
		}
    }


private:
	std::condition_variable	c_full;
	std::condition_variable	c_empt;
	std::queue<T>q;
	std::mutex mtx;
};


template<class T>
class ThreadQueue{

public:
	void push(const T &x){
		std::unique_lock<std::mutex> lck(mtx);

		q.push(x);
		c_empt.notify_all();
	}

	T get(){
		std::unique_lock<std::mutex> lck(mtx);
		if(!q.size())
			c_empt.wait(lck);
		T res=q.front();
		q.pop();

		return res;
	}

    int size(){
        std::unique_lock<std::mutex> lck(mtx);
        return q.size();
    }



private:
	std::condition_variable	c_empt;
	std::queue<T>q;
	std::mutex mtx;
};

}
#endif