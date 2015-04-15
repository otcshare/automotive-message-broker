/*
	Copyright (C) 2015  Intel Corporation

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
*/

namespace amb {
namespace Quality {

/*!
 * \brief The ValueQuality enum describes the quality of an AbstractPropertyType
 * These codes are cherry picked from the OPC UA Quality Code specification
 */
enum ValueQuality {
	Good = 0x00000000,
	GoodNoData = 0x00A50000,
	Uncertain = 0x40000000,
	UncertainSensorNotAccurate = 0x40930000,
	UncertainInitialValue = 0x40920000,
	Bad = 0x80000000,
	BadUnexpectedError = 0x80010000,
	BadInternalError = 0x80020000,
	BadTimeout = 0x800A0000,
	BadServiceUnsupported = 0x800B0000,
	BadSecurityChecksFailed = 0x80130000,
	BadUserAccessDenied = 0x801F0000
};
}
}
