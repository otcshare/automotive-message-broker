#ifndef _FAKEIDLHEADER_H_
#define _FAKEIDLHEADER_H_

/** @module Vehicle **/

/** @moduleComment
*
* Details.
*
* \def-api-feature http://tizen.org/api/vehicle
* \brief Allows access to the vehicle API
*
**/

/** @interface VehicleObject */
/**
* @attributeName  vehicle
* @type Vehicle
* @access readonly
**/

/** @raw Tizen implements VehicleObject; **/

/** @interface VehiclePropertyType **/
/** @attributeName timeStamp
 *  @type Date
 *  @access readonly
**/

/** @raw callback VehiclePropertyCallback = void (VehiclePropertyType value); **/

/** @raw callback VehiclePropertyErrorCallback = void (VehiclePropertyError error); **/

/** @raw callback VehiclePropertyListCallback = void (sequence<VehiclePropertyType> values); **/

/** @interface VehiclePropertyError 
*
*    @enum const unsigned short PERMISSION_DENIED = 1;
*    @enum const unsigned short PROPERTY_UNAVAILABLE = 2;
*    @enum const unsigned short TIMEOUT = 3;
*
*    @attributeName code
*    @type unsigned short
*    @access readonly
*    @attributeComment MUST return error code.
*
*    @attributeName message
*    @type DOMString
*    @access readonly
*    @attributeComment MUST return error message
**/

/** @interface Vehicle **/
/** @method sequence<DOMString> getSupported();
 *  @method get(DOMString property, VehiclePropertyCallback successCallback, optional VehiclePropertyErrorCallback errorCallback);
 *  @method set(DOMString property, VehiclePropertyType value, optional SuccessCallback successCallback, optional VehiclePropertyErrorCallback errorCallback);
 *  @method getHistory(DOMString property, Date startTime, Date endTime, VehicleHistoryListCallback successCallback, VehiclePropertyErrorCallback errorCallback);
 **/


#endif
