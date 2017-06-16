#pragma once

template <class T> 
class Singleton
{

public:

	static T* Instance();

private:

	Singleton() {}     
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);
};

template <class T> T* Singleton<T>::Instance()
{
	static T* singletonObject = new T();
	return singletonObject;
}