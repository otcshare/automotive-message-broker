#include "abstractpropertytype.h"


void ListPropertyType::append(AbstractPropertyType *property)
{
	mList.push_back(property->copy());
}


ListPropertyType::ListPropertyType()
{

}

ListPropertyType::ListPropertyType(int)
{

}

ListPropertyType::ListPropertyType(ListPropertyType &other)
{
	std::list<AbstractPropertyType*> l = other.list();
	for(auto itr = l.begin(); itr != l.end(); itr++)
	{
		append(*itr);
	}
}


ListPropertyType::~ListPropertyType()
{

}


uint ListPropertyType::count()
{
	return mList.size();
}


AbstractPropertyType *ListPropertyType::copy()
{
	return new ListPropertyType(*this);
}


std::string ListPropertyType::toString() const
{
	std::string str = "[";

	for(auto itr = mList.begin(); itr != mList.end(); itr++)
	{
		if(str != "[")
			str += ",";

		AbstractPropertyType* t = *itr;

		str += t->toString();
	}

	str += "]";

	return str;
}


void ListPropertyType::fromString(std::string)
{
	/// TODO: try to use VehicleProperty::getPropertyType... here
}


GVariant *ListPropertyType::toVariant()
{
	GVariantBuilder params;
	g_variant_builder_init(&params,G_VARIANT_TYPE_ARRAY);

	for(auto itr = mList.begin(); itr != mList.end(); itr++)
	{
		AbstractPropertyType* t;
		g_variant_builder_add_value(&params, t->toVariant());
	}

	return g_variant_builder_end(&params);
}


void ListPropertyType::fromVariant(GVariant *v)
{

}
