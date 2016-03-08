#ifndef _ARRAY_H_
#define _ARRAY_H_

template <class TYPE>
class Array {
public:
	Array(){
		count = capacity = 0;
		list = NULL;
		freeElementStack = NULL;
		freeElementStackCapacity = freeElementStackCount = 0;
	}

	Array(const unsigned int iCapasity){
		count = 0;
		capacity = iCapasity;
		list = (TYPE *) malloc(capacity * sizeof(TYPE));

		freeElementStack = NULL;
		freeElementStackCapacity = freeElementStackCount = 0;
	}
	
	~Array(){
		free(list);

		if (freeElementStack)
			free(freeElementStack);
	}

	TYPE *data() const { return list; }
	TYPE *abandonArray(){
		TYPE *rList = list;
		list = NULL;

		if (freeElementStack)
			free(freeElementStack);

		freeElementStack = NULL;

		return rList;
	}

	TYPE &operator [] (const unsigned int index) const { return list[index]; }
	unsigned int size() const { return count; }

	void setCount(const unsigned int newCount)
	{
		capacity = count = newCount;
		list = (TYPE *) realloc(list, capacity * sizeof(TYPE));
	}

	void push_back(const TYPE object){

		if (count >= capacity){
			if (capacity) capacity += capacity; else capacity = 8;
			list = (TYPE *) realloc(list, capacity * sizeof(TYPE));
		}
		list[count] = object;
		count++;
	}

	unsigned int add(const TYPE object)
	{
		unsigned int index = 0;

		if (freeElementStackCount > 0)
		{
			index = freeElementStack[freeElementStackCount - 1];
			list[index] = object;
			//count++;

			freeElementStackCount--;			
		}
		else
		{
			push_back(object);
			index = count - 1;
		}

		return index;
	}

	void Remove(const unsigned int index)
	{
		if (index < count)
		{
			//count--;
			memset((void*)&list[index], 0, sizeof(TYPE));

			if (freeElementStackCount >= freeElementStackCapacity)
			{
				if (freeElementStackCapacity) freeElementStackCapacity += freeElementStackCapacity; else freeElementStackCapacity = 8;
				freeElementStack = (unsigned int*)realloc(freeElementStack, freeElementStackCapacity * sizeof(unsigned int));
			}

			freeElementStack[freeElementStackCount] = index;
			freeElementStackCount++;
		}
	}

	void fastRemove(const unsigned int index){
		if (index < count){
			count--;
			list[index] = list[count];
		}
	}

	void orderedRemove(const unsigned int index){
		if (index < count){
			count--;
			memmove(list + index, list + index + 1, (count - index) * sizeof(TYPE));
		}
	}

	void clear(){
		count = 0;
	}

	void reset(){
		free(list);
		list = NULL;
		count = capacity = 0;
	}

private:
	int partition(int (*compare)(const TYPE &elem0, const TYPE &elem1), int p, int r){
		TYPE tmp, pivot = list[p];
		int left = p;

		for (int i = p + 1; i <= r; i++){
			if (compare(list[i], pivot) < 0){
				left++;
				tmp = list[i];
				list[i] = list[left];
				list[left] = tmp;
			}
		}
		tmp = list[p];
		list[p] = list[left];
		list[left] = tmp;
		return left;
	}

	void quickSort(int (*compare)(const TYPE &elem0, const TYPE &elem1), int p, int r){
		if (p < r){
			int q = partition(compare, p, r);
			quickSort(compare, p, q - 1);
			quickSort(compare, q + 1, r);
		}
	}
public:
	void sort(int (*compare)(const TYPE &elem0, const TYPE &elem1)){
		quickSort(compare, 0, count - 1);
	}
protected:
	unsigned int capacity;
	unsigned int count;
	TYPE *list;

	unsigned int freeElementStackCount;
	unsigned int freeElementStackCapacity;
	unsigned int *freeElementStack;
};

#endif // _ARRAY_H_