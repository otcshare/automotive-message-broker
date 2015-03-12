/*****************************************************************
Copyright (C) 2014  Intel Corporation

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 *****************************************************************/

#include "ambtmpl_cansignal.h"

template<>
bool convert<bool>( double temp )
{
    return abs(temp) > std::numeric_limits<double>::epsilon();
}

double fromGVariant(GVariant *value)
{
    GVariantClass c = g_variant_classify(value);
    if(c == G_VARIANT_CLASS_BOOLEAN)
        return g_variant_get_boolean(value) ? 1.0 : 0.0;

    else if(c == G_VARIANT_CLASS_BYTE)
        return static_cast<double>(g_variant_get_byte(value));

    else if(c == G_VARIANT_CLASS_INT16)
        return static_cast<double>(g_variant_get_int16(value));

    else if(c == G_VARIANT_CLASS_UINT16)
        return static_cast<double>(g_variant_get_uint16(value));

    else if(c == G_VARIANT_CLASS_INT32)
        return static_cast<double>(g_variant_get_int32(value));

    else if(c ==  G_VARIANT_CLASS_UINT32)
        return static_cast<double>(g_variant_get_uint32(value));

    else if(c == G_VARIANT_CLASS_INT64)
        return static_cast<double>(g_variant_get_int64(value));

    else if(c == G_VARIANT_CLASS_UINT64)
        return static_cast<double>(g_variant_get_uint64(value));

    else if(c == G_VARIANT_CLASS_DOUBLE)
        return static_cast<double>(g_variant_get_double(value));

    else
        return 0.0;

}

CANSignal::CANSignal(std::function<AbstractPropertyType* ()> factoryFunction) :
    ambProperty(nullptr),
    signature(static_cast<GVariantClass>(0)),
    factoryFunction(factoryFunction)
{
}

CANSignal::~CANSignal()
{
}

void CANSignal::onMessage(const can_frame& frame, std::function<void (AbstractPropertyType*)> changeCallback)
{
    std::unique_ptr<GVariant, decltype(&g_variant_unref)> variant(processSignal(frame), &g_variant_unref);
    if(variant){
        std::unique_ptr<GVariant, decltype(&g_variant_unref)> oldValue(ambProperty->toVariant(), &g_variant_unref);
        if(!g_variant_equal(variant.get(), oldValue.get())) {
            ambProperty->fromVariant( variant.get() );
            if(changeCallback)
                changeCallback(ambProperty.get());
        }
    }
}

void CANSignal::onTimeout(const can_frame& frame, std::function<void (AbstractPropertyType*)> changeCallback)
{
    //TODO: implement <no-value> handling
/*    if (ambProperty->toString() != "none") {
        ambProperty->setValue("none");
        if(changeCallback)
            changeCallback(ambProperty.get());
    }
*/
}


void CANSignal::setAmbProperty(std::shared_ptr<AbstractPropertyType> ambProperty)
{
    this->ambProperty = ambProperty;
    std::unique_ptr<GVariant, decltype(&g_variant_unref)> variant(ambProperty->toVariant(), &g_variant_unref);
    if(variant)
        signature = g_variant_classify(variant.get());
}

template <>
GVariant* CANSignal::processFrameBits<bool>( const can_frame& frame )
{
    bool value = getSignalBits( frame ) != 0ull;
    return toGVariant<bool>(value, signature);
}

bool CANSignal::updateFrame(can_frame* frame)
{
    if(/*!value ||*/ !frame)
        return false;

    std::unique_ptr<GVariant, decltype(&g_variant_unref)> variant(ambProperty->toVariant(), &g_variant_unref);
    //ambProperty->fromVariant(variant.get());

    double val(fromGVariant(variant.get()));
    double temp = (val - signalInfo.m_offset)/signalInfo.m_factor;

    int64_t bits = conversionFunctionTo(val, static_cast<int64_t>(temp));

    *(reinterpret_cast<uint64_t*>(&frame->data[0])) |= toSignalBits(bits);
}

int64_t CANSignal::getSignalBits( const can_frame& frame )
{
    int64_t bits = *reinterpret_cast<const int64_t* >(frame.data);
    int startbit = signalInfo.m_startbit;

    if (signalInfo.m_byteOrdering == Motorola ) {
        // Motorola
        bits = __bswap_64(bits);
        int rounded = signalInfo.m_startbit & (~0x07); //(signalInfo.m_startbit/8)*8;
        int remainder = signalInfo.m_startbit & 0x07; //signalInfo.m_startbit % 8;
        startbit = (CANFrameDataSize - ( (8 - remainder) + rounded) - (signalInfo.m_length - 1) );
    }
    else{
        // Intel - do nothing
    }

    bits = bits >> startbit;
    uint64_t mask = ~(0ull);
    if(signalInfo.m_length < 64){
        mask = (1ull << static_cast<uint64_t>(signalInfo.m_length)) - 1ull;
    }

    bits &= mask;
    if(signalInfo.m_signedness){
        if(signalInfo.m_length <= 8 ){
            bits = static_cast<int8_t>(bits);
        }
        else if(signalInfo.m_length <= 16 ){
            bits = static_cast<int16_t>(bits);
        }
        else if(signalInfo.m_length <= 32 ){
            bits = static_cast<int32_t>(bits);
        }
    }

    return bits;
}

uint64_t CANSignal::toSignalBits( int64_t bits )
{
    uint64_t signBit(0ull);
    if(signalInfo.m_signedness && bits < 0ull){
        signBit = (1ull << static_cast<uint64_t>(signalInfo.m_length-1));
    }

    uint64_t mask = ~(0ull);
    if(signalInfo.m_length < 64){
        mask = (1ull << static_cast<uint64_t>(signalInfo.m_length)) - 1ull;
    }
    bits &= mask;
    bits |= signBit;

    int startbit = signalInfo.m_startbit;
    if (signalInfo.m_byteOrdering == Motorola ) {
        // Motorola
        int rounded = signalInfo.m_startbit & (~0x07); //(signalInfo.m_startbit/8)*8;
        int remainder = signalInfo.m_startbit & 0x07; //signalInfo.m_startbit % 8;
        startbit = (CANFrameDataSize - ( (8 - remainder) + rounded) - (signalInfo.m_length - 1) );
    }
    else{
        // Intel - do nothing
    }

    bits = bits << startbit;

    if (signalInfo.m_byteOrdering == Motorola ) {
        bits = __bswap_64(bits);
    }

    return bits;
}
