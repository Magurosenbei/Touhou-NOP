#ifndef _ARRAY_H
#define _ARRAY_H
#ifdef _MSC_VER
#pragma once
#endif

#include <assert.h>

/* Notes
This is a list or rather vector or dynamic storage
It is a linked list type of thing, if STL is giving problems use this instead
*/

template <typename T> class CArray
{
protected:
	T		*m_pData;
	size_t	 m_iSize;
	size_t	 m_iCapacity;
public:
	inline CArray()
		: m_pData		(NULL)
		, m_iSize		(0)
		, m_iCapacity	(0)
	{
	}
	inline CArray(T *pData, size_t iLength)
		: m_pData		(new T [iLength])
		, m_iSize		(iLength)
		, m_iCapacity	(iLength)
	{
		for (size_t i = 0; i < iLength; i++)
		{
			new (&m_pData[i]) T (pData[i]);
		}
	}
	inline CArray(const CArray &a)
		: m_pData		(new T [a.m_iSize])
		, m_iSize		(a.m_iSize)
		, m_iCapacity	(a.m_iSize)
	{
		for (size_t i = 0; i < m_iSize; i++)
		{
			new (&m_pData[i]) T (a.m_pData[i]);
		}
	}
	inline ~CArray()
	{
		clear();
	}
	inline CArray &operator =(const CArray &a)
	{
		m_pData = new T [a.m_iSize];
		m_iSize = a.m_iSize;
		m_iCapacity = a.m_iCapacity;
		for (size_t i = 0; i < m_iSize; i++)
		{
			new (&m_pData[i]) T (a.m_pData[i]);
		}
		return *this;
	}
	inline size_t size() const
	{
		return m_iSize;
	}
	inline size_t capacity() const
	{
		return m_iCapacity;
	}
	inline void remove(size_t iIndex)
	{
		m_iSize--;
		for (size_t i = iIndex; i < m_iSize; i++)
		{
			new (&m_pData[i]) T (m_pData[i + 1]);
		}
		new (&m_pData[m_iSize]) T ();
	}
	inline void resize(size_t iSize)
	{
		if (m_iCapacity != iSize)
		{
			if (iSize)
			{
				size_t i;
				T *pTemp = m_pData;
				m_pData = new T [iSize];
				m_iCapacity = iSize;
				if (pTemp)
				{
					if (iSize < m_iSize)
					{
						for (i = 0; i < iSize; i++)
						{
							new (&m_pData[i]) T (pTemp[i]);
						}
						m_iSize = iSize;
					}
					else
					{
						for (i = 0; i < m_iSize; i++)
						{
							new (&m_pData[i]) T (pTemp[i]);
						}
						for (i = m_iSize; i < iSize; i++)
						{
							new (&m_pData[i]) T ();
						}
					}
					delete []pTemp;
				}
			} else
			if (m_pData)
			{
				delete []m_pData;
				m_pData = NULL;
				m_iSize = 0;
				m_iCapacity = 0;
			}
		}
	}
	inline const T &insert(const T &v)
	{
		if (m_iCapacity)
		{
			if (m_iSize == m_iCapacity)
			{
				resize(m_iCapacity * 2);
			}
		}
		else
		{
			resize(8);
		}
		return (m_pData[m_iSize++] = v);
	}
	inline void clear()
	{
		delete []m_pData;
		m_pData = NULL;
		m_iSize = 0;
		m_iCapacity = 0;
	}
	inline const T &operator [](size_t iIndex) const
	{
		assert(iIndex < m_iSize);
		return m_pData[iIndex];
	}
	inline const T &at(size_t iIndex) const
	{
		assert(iIndex < m_iSize);
		return m_pData[iIndex];
	}
};

template <typename T> class CPtrArray
{
protected:
	T		**m_pData;
	size_t	  m_iSize;
	size_t	  m_iCapacity;
public:
	inline CPtrArray()
		: m_pData		(NULL)
		, m_iSize		(0)
		, m_iCapacity	(0)
	{
	}
	inline CPtrArray(T **pData, size_t iLength)
		: m_pData		(new T * [iLength])
		, m_iSize		(iLength)
		, m_iCapacity	(iLength)
	{
		for (size_t i = 0; i < iLength; i++)
		{
			m_pData[i] = pData[i];
		}
	}
	inline CPtrArray(const CPtrArray &a)
		: m_pData		(new T * [a.m_iSize])
		, m_iSize		(a.m_iSize)
		, m_iCapacity	(a.m_iSize)
	{
		for (size_t i = 0; i < m_iSize; i++)
		{
			m_pData[i] = a.m_pData[i];
		}
	}
	inline ~CPtrArray()
	{
		clear();
	}
	inline CPtrArray &operator =(const CPtrArray &a)
	{
		m_pData = new T * [a.m_iSize];
		m_iSize = a.m_iSize;
		m_iCapacity = a.m_iCapacity;
		for (size_t i = 0; i < m_iSize; i++)
		{
			m_pData[i] = a.m_pData[i];
		}
		return *this;
	}
	inline size_t size() const
	{
		return m_iSize;
	}
	inline size_t capacity() const
	{
		return m_iCapacity;
	}
	inline bool remove(const T *p)
	{
		for (size_t i = 0; i < m_iSize; i++)
		{
			if (m_pData[i] == p)
			{
				delete p;
				for (size_t j = i; j < m_iSize; j++)
				{
					m_pData[j] = m_pData[j + 1];
				}
				m_iSize--;
				return true;
			}
		}
		return false;
	}
	inline void remove(size_t iIndex)
	{
		delete m_pData[iIndex];
		for (size_t i = iIndex; i < m_iSize; i++)
		{
			m_pData[i] = m_pData[i + 1];
		}
		m_iSize--;
	}
	inline void resize(size_t iSize)
	{
		if (m_iCapacity != iSize)
		{
			if (iSize)
			{
				size_t i;
				T **pTemp = m_pData;
				m_pData = new T * [iSize];
				m_iCapacity = iSize;
				if (pTemp)
				{
					if (iSize < m_iSize)
					{
						for (i = 0; i < iSize; i++)
						{
							m_pData[i] = pTemp[i];
						}
						for (i = iSize; i < m_iSize; i++)
						{
							delete pTemp[i];
						}
						m_iSize = iSize;
					}
					else
					{
						for (i = 0; i < m_iSize; i++)
						{
							m_pData[i] = pTemp[i];
						}
						for (i = m_iSize; i < iSize; i++)
						{
							m_pData[i] = NULL;
						}
					}
					delete []pTemp;
				}
			} else
			if (m_pData)
			{
				delete []m_pData;
				m_pData = NULL;
				m_iSize = 0;
				m_iCapacity = 0;
			}
		}
	}
	inline T &insert(T *v)
	{
		if (m_iCapacity)
		{
			if (m_iSize == m_iCapacity)
			{
				resize(m_iCapacity * 2);
			}
		}
		else
		{
			resize(8);
		}
		m_pData[m_iSize] = v;
		return *m_pData[m_iSize++];
	}
	inline const T &insert(const T *v)
	{
		if (m_iCapacity)
		{
			if (m_iSize == m_iCapacity)
			{
				resize(m_iCapacity * 2);
			}
		}
		else
		{
			resize(8);
		}
		m_pData[m_iSize] = const_cast<T *>(v);
		return *m_pData[m_iSize++];
	}
	inline void clear()
	{
		for (size_t i = 0; i < m_iSize; i++)
		{
			if(m_pData[i] != NULL)
			{
				delete m_pData[i];
			}
		}
		delete []m_pData;
		m_pData = NULL;
		m_iSize = 0;
		m_iCapacity = 0;
	}
	inline const T &operator [](size_t iIndex) const
	{
		assert(iIndex < m_iSize);
		return *m_pData[iIndex];
	}
	inline T &operator [](size_t iIndex)
	{
		assert(iIndex < m_iSize);
		return *m_pData[iIndex];
	}
	inline T *at(size_t iIndex)
	{
		assert(iIndex < m_iSize);
		return m_pData[iIndex];
	}
};

#endif
