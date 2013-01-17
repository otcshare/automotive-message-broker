#include "uuidhelper.h"

std::string amb::createUuid()
{
	uuid_t id;
	uuid_generate_random(id);

	char out[37];
	uuid_unparse(id, out);

	return out;
}
