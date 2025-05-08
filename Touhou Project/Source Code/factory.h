#pragma once

/*
Consist of a List and functions that recycle / create elements
*/

template <class A, class B>	class Factory
{	
public:
	list <A *> List;

	virtual A * Create(B & Def)
	{
		for(list <A *>::iterator index = List.begin(); index != List.end(); index++)
			if((*index)->CanDelete())
			{
				(*index)->ReInit(Def);
				return (*index);
			}
		A * Insertion = new A(Def);
		List.push_back(Insertion);
		return Insertion;
	}

	virtual A * Create(A * ToBeInsert)
	{
		for(list <A *>::iterator index = List.begin(); index != List.end(); index++)
			if((*index)->CanDelete())
			{
				*(*index) = *ToBeInsert;
				return (*index);
			}
		List.push_back(ToBeInsert);
		return ToBeInsert;
	}
};