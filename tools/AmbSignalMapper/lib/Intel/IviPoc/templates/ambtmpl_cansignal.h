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

#ifndef AMBTMPL_CANSIGNAL_H_
#define AMBTMPL_CANSIGNAL_H_

#include <map>
#include <limits>
#include <glib.h>
#include <abstractpropertytype.h>
#include <vehicleproperty.h>
#include <byteswap.h>
#include <boost/any.hpp>

#include <canbus.h>
#include <canobserver.h>

enum Endian{
    Motorola = 0x0,     // BigEndian
    Intel = 0x1         // LittleEndian
};

enum Signedness{
    Unsigned = 0x0,     // Unsigned
    Signed = 0x1        // Signed
};

const int CANFrameDataSize = 8*sizeof(can_frame::data);

template<typename T>
T convert( double temp )
{
    return static_cast<T>(temp);
}

template<typename T>
T convert( const boost::any& temp )
{
    return boost::any_cast<T>(temp);
}

template<typename T>
GVariant* toGVariant(const T& value, const GVariantClass& c)
{
    if(c == G_VARIANT_CLASS_BOOLEAN)
        return g_variant_new_boolean(static_cast<gboolean>(value));

    else if(c == G_VARIANT_CLASS_BYTE)
        return g_variant_new_byte(static_cast<guchar>(value));

    else if(c == G_VARIANT_CLASS_INT16)
        return g_variant_new_int16(static_cast<gshort>(value));

    else if(c == G_VARIANT_CLASS_UINT16)
        return g_variant_new_uint16(static_cast<gushort>(value));

    else if(c == G_VARIANT_CLASS_INT32)
        return g_variant_new_int32(static_cast<gint>(value));

    else if(c ==  G_VARIANT_CLASS_UINT32)
        return g_variant_new_uint32(static_cast<guint>(value));

    else if(c == G_VARIANT_CLASS_INT64)
        return g_variant_new_int64(static_cast<gint64>(value));

    else if(c == G_VARIANT_CLASS_UINT64)
        return g_variant_new_int64(static_cast<guint64>(value));

    else if(c == G_VARIANT_CLASS_DOUBLE)
        return g_variant_new_double(static_cast<gdouble>(value));

    else
        return nullptr;
}

class CANSignal {

public:
    explicit CANSignal(std::function<AbstractPropertyType* ()> factoryFunction);
    virtual ~CANSignal();

    virtual GVariant *processSignal( const can_frame& frame ) = 0;

    virtual void onMessage( const can_frame& frame, std::function<void (AbstractPropertyType*)> changeCallback );
    virtual void onTimeout( const can_frame& frame, std::function<void (AbstractPropertyType*)> changeCallback );

    virtual bool updateFrame( can_frame* frame );

    template <typename T>
    GVariant* processFrameBits( const can_frame& frame )
    {
        int64_t bits = getSignalBits( frame );
        double temp = static_cast<double>(bits)*signalInfo.m_factor + signalInfo.m_offset;
        T value(convert<T>(conversionFunctionFrom(temp, bits)));
        return toGVariant<T>(value, signature);
    }

    virtual boost::any conversionFunctionFrom(double value, int64_t bits) = 0;

    virtual uint64_t conversionFunctionTo(double value, uint64_t bits) = 0;

    void setAmbProperty(std::shared_ptr<AbstractPropertyType> ambProperty);

    inline std::function<AbstractPropertyType* ()> factory()
    {
        return factoryFunction;
    }

protected:
    int64_t getSignalBits( const can_frame& frame );
    uint64_t toSignalBits( int64_t bits );

protected:
    struct SignalInfo {
        uint8_t m_startbit;
        uint8_t m_length;
        Endian m_byteOrdering;
        Signedness m_signedness;
        double m_factor;
        double m_offset;
    };

    SignalInfo signalInfo;
    std::shared_ptr<AbstractPropertyType> ambProperty;
    GVariantClass signature;
    std::function<AbstractPropertyType* ()> factoryFunction;
};

class CANMessage
{
public:
    CANMessage() = delete;

    CANMessage(canid_t canId, __u8 canDlc, double CycleTime) :
        canId(canId),
        canDlc(canDlc),
        CycleTime(CycleTime)
    {
    }

    void addSignal(const VehicleProperty::Property& name, std::shared_ptr<CANSignal> signal)
    {
        if(signal)
            canSignals[name] = signal;
    }

    void onMessage(const can_frame& frame, std::function<void (AbstractPropertyType*)> changeCallback)
    {
        // sanity check
        if(frame.can_dlc != canDlc || frame.can_id != canId)
            return;

        for ( auto it = canSignals.begin(); it != canSignals.end(); ++it ) {
            std::shared_ptr<CANSignal> signal(it->second);

            if ( signal ) {
                signal->onMessage(frame, changeCallback);
            }
        }
    }

    void onTimeout(const can_frame& frame, std::function<void (AbstractPropertyType*)> changeCallback)
    {
        // sanity check
        if(frame.can_id != canId)
            return;

        for ( auto it = canSignals.begin(); it != canSignals.end(); ++it ) {
            std::shared_ptr<CANSignal> signal(it->second);

            if ( signal ) {
                signal->onTimeout(frame, changeCallback);
            }
        }
    }

    void setupFrame(can_frame* frame)
    {
        if(!frame)
            return;

        memset(frame, 0, sizeof(can_frame));
        frame->can_id = canId;
        frame->can_dlc = canDlc;

        for ( auto it = canSignals.begin(); it != canSignals.end(); ++it ) {
            it->second->updateFrame(frame);
        }
    }

    bool registerOnCANBus(CANBus& canBus)
    {
        return canBus.registerCyclicMessageForReceive(canId, 0, CycleTime);
    }

private:
    canid_t canId;
    __u8 canDlc;
    double CycleTime;
    std::map< VehicleProperty::Property , std::shared_ptr<CANSignal> > canSignals;
};

#define CANSIGNAL(property, valueType, startbit, length, byteOrdering, signedness, factor, offset, minValue, maxValue, convertFrom, convertTo) \
    class property ## Type : public CANSignal { \
    public: property ## Type() : \
            CANSignal([](){ return new BasicPropertyType<valueType>(property); } ) \
            , convertFromFunction(convertFrom) \
            , convertToFunction(convertTo) \
    {\
        signalInfo.m_startbit = startbit; \
        signalInfo.m_length = length; \
        signalInfo.m_byteOrdering = byteOrdering; \
        signalInfo.m_signedness = signedness; \
        signalInfo.m_factor = factor; \
        signalInfo.m_offset = offset; \
        m_minValue = minValue; \
        m_maxValue = maxValue; \
    } \
    GVariant *processSignal( const can_frame& frame ) { \
        return processFrameBits<valueType>( frame ); \
    } \
    boost::any conversionFunctionFrom(double value, int64_t bits) { \
        valueType targetValue(convert<valueType>(value)); \
        if(convertFromFunction) \
            return convertFromFunction(targetValue, bits); \
        else return targetValue; \
    } \
    uint64_t conversionFunctionTo(double value, uint64_t bits) { \
        if(convertToFunction) \
            convertToFunction(convert<valueType>(value), bits); \
        else return bits; \
    } \
    valueType m_minValue; \
    valueType m_maxValue; \
    std::function<valueType(valueType, int64_t)> convertFromFunction; \
    std::function<uint64_t(valueType, uint64_t)> convertToFunction; \
    typedef valueType value_type; \
    };

#endif /* AMBTMPL_CANSIGNAL_H_ */
