#include "abstractpropertytype.h"

const Zone::Type Zone::FrontRight = Zone::Type(Zone::Front | Zone::Right);
const Zone::Type Zone::FrontLeft = Zone::Type(Zone::Front | Zone::Left);
const Zone::Type Zone::MiddleRight = Zone::Type(Zone::Middle | Zone::Right);
const Zone::Type Zone::MiddleLeft = Zone::Type(Zone::Middle | Zone::Left);
const Zone::Type Zone::RearRight = Zone::Type(Zone::Rear | Zone::Right);
const Zone::Type Zone::RearLeft = Zone::Type(Zone::Rear | Zone::Left);
