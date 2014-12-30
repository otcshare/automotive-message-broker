#include <string>
#include <unordered_map>
#include <unordered_set>
#include <listplusplus.h>
#include <abstractsink.h>
#include <abstractroutingengine.h>

namespace amb
{

std::unordered_map<std::string, std::unordered_set<Zone::Type>> getUniqueSourcesList(AbstractRoutingEngine *re, PropertyList implementedProperties)
{
	std::unordered_map<std::string, std::unordered_set<Zone::Type>> uniqueSourcesList;

	for(auto property : implementedProperties)
	{
		std::vector<std::string> sources = re->sourcesForProperty(property);

		for(auto source : sources)
		{
			PropertyInfo info = re->getPropertyInfo(property, source);

			std::unordered_set<Zone::Type> uniqueZoneList;

			if(uniqueSourcesList.count(source))
			{
				uniqueZoneList = uniqueSourcesList[source];
			}

			Zone::ZoneList zoneList = info.zones();

			if(!zoneList.size())
			{
				uniqueZoneList.emplace(Zone::None);
			}

			for(auto zoneItr : zoneList)
			{
				uniqueZoneList.emplace(zoneItr);
			}

			uniqueSourcesList[source] = uniqueZoneList;
		}
	}

	return uniqueSourcesList;
}

template <typename T>
void exportProperty(VehicleProperty::Property prop, AbstractRoutingEngine *re, GDBusConnection *connection)
{
	T* t = new T(prop, re, connection);

	prop = t->objectName();

	/// check if we need more than one instance:

	PropertyList implementedProperties = t->wantsProperties();

	std::unordered_map<std::string, std::unordered_set<Zone::Type> > uniqueSourcesList = getUniqueSourcesList(re, implementedProperties);

	delete t;

	PropertyList supported = re->supported();

	for(auto itr : uniqueSourcesList)
	{
		std::unordered_set<Zone::Type> zones = itr.second;

		std::string source = itr.first;

		std::string objectPath = "/" + source;

		boost::algorithm::erase_all(objectPath, "-");

		for(auto zone : zones)
		{
			T* t = new T(prop, re, connection);
			std::stringstream fullobjectPath;
			fullobjectPath<< objectPath << "/" << zone << "/" <<t->objectName();
			t->setObjectPath(fullobjectPath.str());
			t->setSourceFilter(source);
			t->setZoneFilter(zone);
			t->supportedChanged(supported);
		}

	}
}

template <typename T>
void exportProperty(AbstractRoutingEngine *re, GDBusConnection *connection)
{
	exportProperty<T>("", re, connection);
}

}
