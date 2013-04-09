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

/** @interface Navigator
 *  @interfaceType partial **/
 /**
  * @attributeName vehicle
  * @type Vehicle
  * @access readonly
} **/

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
 *  @methodComment \brief returns supported properties
 *  @method get(DOMString property, VehiclePropertyCallback successCallback, optional VehiclePropertyErrorCallback errorCallback);
 *  @methodComment \brief fetch the current value for 'property'.
 *  @methodComment \arg DOMString property is the property
 *  @methodComment \arg VehiclePropertyCallback successCallback function to be called when method has completed successfully
 *  @methodComment \arg VehiclePropertyErrorCallback this function is called when an error has occured.
 *  @method set(DOMString property, VehiclePropertyType value, optional SuccessCallback successCallback, optional VehiclePropertyErrorCallback errorCallback);
 *  @methodComment \brief set the given property to value
 *  @methodComment \arg DOMString property property to set
 *  @methodComment \arg VehiclePropertyType value value to set
 *  @methodComment \arg SuccessCallback successCallback callback if operation is successfull
 *  @methodComment \arg VehiclePropertyErrorCallback errorCallback callback if error has been called.
 *  @method getHistory(DOMString property, Date startTime, Date endTime, VehiclePropertyListCallback successCallback, optional VehiclePropertyErrorCallback errorCallback);
 *  @methodComment \brief get values for a given property within a certain past time period between 'startTime' and 'endTime'
 *  @methodComment \arg DOMString property property to request
 *  @methodComment \arg Date startTime, starting period of time.
 *  @methodComment \arg Date endTime, ending period of time.
 *  @methodComment \arg VehiclePropertyListCallback successCallback. Callback with the result of the method call
 *  @methodComment \arg VehiclePropertyErrorCallback errorCallback. Callback if an error has occurred.
 **/


#endif
