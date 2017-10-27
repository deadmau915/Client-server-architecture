#include <iostream>
#include <vector>

using std::vector;

template <typename T> class sparseMat
{
private:
	T dimension;
	mutable vector<T> val;
	mutable vector<T> col_ind;
	mutable vector<T> row_ptr;

public:
	sparseMat() {}
	sparseMat(T dimension, vector<T> val, vector<T> col_ind, vector<T> row_ptr): dimension(dimension), col_ind(col_ind), row_ptr(row_ptr) {}

	void setDimension(T element) {
		dimension = element;
	}

	void setValue(T element) {
		val.push_back(element);
	}

	void setCol_ind(T element) {
		col_ind.push_back(element);
	}
	
	void setRow_ptr(T element) {
		row_ptr.push_back(element);
	}

	T getDimension() {
		return dimension;
	}

	T getValue(int pos) {
		return val[pos];
	}

	T getCol_ind(int pos) {
		return col_ind[pos];
	}
	
	T getRow_ptr(int pos) {
		return row_ptr[pos];
	}

	~sparseMat() {}

};