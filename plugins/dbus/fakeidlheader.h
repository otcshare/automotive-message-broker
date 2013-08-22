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
/** @attributeName value
 *  @type object
 *  @access readonly
 *
 *  @attributeName timeStamp
 *  @type Date
 *  @access readonly
**/

/** @interface Vehicle **/
/** @method sequence<DOMString> getSupported();
 *  @method object get(DOMString property);
 *  @method void set(DOMString property, object value);
 *  @method sequence<VehiclePropertyTypes> getHistory(DOMString type, Date startTime, Date endTime);
 **/


#endif
