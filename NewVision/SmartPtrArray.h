#if !defined( SMARTPTRARRAY_H )
#define SMARTPTRARRAY_H

#include <afxtempl.h>
#include <afx.h>

template <class TYPE> class SmartPtrArray : public CArray<TYPE*> {
public:
	// --------------------------------------------------------------------------
	INT_PTR Add(TYPE* newElement) {
		return CArray<TYPE*>::Add(newElement);		
	}
	// --------------------------------------------------------------------------
	INT_PTR AddCopy(TYPE* newElement) {
		TYPE* copy = new TYPE(*newElement);
		return CArray<TYPE*>::Add(copy);		
	}
	// --------------------------------------------------------------------------
	void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1) {
		for (int i=0;i<nCount;i++)
			delete GetAt( nIndex+i );
		CArray<TYPE*>::RemoveAt(nIndex, nCount);
	}
	// --------------------------------------------------------------------------
	void RemoveAll() {
		for(int i=0; i < GetCount(); i++ )
			delete GetAt( i );
		CArray<TYPE*>::RemoveAll();
	}
	// --------------------------------------------------------------------------
	void RemoveAllPointers() {
		CArray<TYPE*>::RemoveAll();
	}
	// --------------------------------------------------------------------------
	void Copy(const SmartPtrArray& src) {
		int mi = min(GetCount(), src.GetCount());
		int ma = max(GetCount(), src.GetCount());
		for(int i = 0; i < mi; i++ )
			SetAt(i, src[i]);
		
		if (GetCount() < src.GetCount()) {
			for(int i = mi; i < ma; i++ )
				AddCopy(src[i]);
		}
		else {
			RemoveAt(mi, ma-mi);
		}
	}
	// --------------------------------------------------------------------------
	void SetAt(INT_PTR nIndex, TYPE* newElement) {
		delete GetAt( nIndex );
		CArray<TYPE*>::SetAt(nIndex, new TYPE(*newElement));
	}
	// --------------------------------------------------------------------------
	bool In(TYPE* n) {
		for (int i=0;i < GetCount();i++)
			if ((*GetAt(i)) == n)
				return true;
		return false;
	}
	// ------------------------------------------------------------------------
	int Find(TYPE* n) {
		for (int i=0;i < GetCount();i++)
			if ((*GetAt(i)) == n)
				return i;
		return -1;
	}
	// --------------------------------------------------------------------------
	void Sort() {
		for (int i=0;i<GetCount();i++)
			for (int j=i+1;j<GetCount();j++) {
				if (GetAt(j-1)->Compare(GetAt(j)) > 0) {
					TYPE* temp = GetAt(j-1);
					GetAt(j-1) = GetAt(j);
					GetAt(j) = temp;
				}
			}
	}
	// --------------------------------------------------------------------------
};

#endif // !defined( SMARTPTRARRAY_H )