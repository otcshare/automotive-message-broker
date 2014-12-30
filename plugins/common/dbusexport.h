#include <string>
#include <unordered_map>
#include <unordered_set>
#include <listplusplus.h>
#include <abstractsink.h>
#include <abstractroutingengine.h>

#include "varianttype.h"

namespace amb
{

class Exporter
{
public:

	std::shared_ptr<GDBusConnection> connection;

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
	void exportProperty(std::string interfaceName, std::unordered_map<std::string, std::string> properties, AbstractRoutingEngine *re)
	{
		T* t = new T(interfaceName, re, connection.get());

		std::string prop = t->objectName();

		for(auto props : properties)
		{
			t->wantPropertyVariant(props.first, props.second, VariantType::ReadWrite);
		}

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
				T* t = new T(prop, re, connection.get());

				for(auto props : properties)
				{
					t->wantPropertyVariant(props.first, props.second, VariantType::ReadWrite);
				}

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
	void exportProperty(AbstractRoutingEngine *re)
	{
		exportProperty<T>("", std::unordered_map<std::string, std::string>(), re);
	}

	template <typename T>
	void exportProperty(VehicleProperty::Property prop, AbstractRoutingEngine *re)
	{
		exportProperty<T>(prop, std::unordered_map<std::string, std::string>(), re);
	}

	static Exporter * instance()
	{
		if(mInstance)
			return mInstance;
		return mInstance = new Exporter();
	}

protected:
	Exporter();

private:
	static Exporter* mInstance;

};

}
