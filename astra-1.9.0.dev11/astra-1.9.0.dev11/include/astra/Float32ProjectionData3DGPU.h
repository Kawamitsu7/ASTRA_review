/*
-----------------------------------------------------------------------
Copyright: 2010-2018, imec Vision Lab, University of Antwerp
           2014-2018, CWI, Amsterdam

Contact: astra@astra-toolbox.com
Website: http://www.astra-toolbox.com/

This file is part of the ASTRA Toolbox.


The ASTRA Toolbox is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

The ASTRA Toolbox is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with the ASTRA Toolbox. If not, see <http://www.gnu.org/licenses/>.

-----------------------------------------------------------------------
*/

#ifndef _INC_ASTRA_FLOAT32PROJECTIONDATA3DGPU
#define _INC_ASTRA_FLOAT32PROJECTIONDATA3DGPU

#include "Float32Data3DGPU.h"
#include "ProjectionGeometry3D.h"
#include "Float32ProjectionData3D.h"

#ifdef ASTRA_CUDA

namespace astra {

/**
 * This class represents three-dimensional Projection Data where the entire data block is stored in GPU memory.
 */
class _AstraExport CFloat32ProjectionData3DGPU : public CFloat32Data3DGPU, public CFloat32ProjectionData3D
{
public:

	/** Default constructor. Sets all numeric member variables to 0 and all pointer member variables to NULL.
	 *
	 * If an object is constructed using this default constructor, it must always be followed by a call 
	 * to one of the init() methods before the object can be used. Any use before calling init() is not allowed,
	 * except calling the member function isInitialized().
	 *
	 */
	CFloat32ProjectionData3DGPU();
	
	/** Construction.
	 *
	 * @param _pGeometry 3D volume geometry
	 * @param _hnd the CUDA memory handle
	 */

	CFloat32ProjectionData3DGPU(CProjectionGeometry3D* _pGeometry, astraCUDA3d::MemHandle3D _hnd);

	virtual ~CFloat32ProjectionData3DGPU();

	/** Initialization.
	 *
	 * @param _pGeometry 3D volume geometry
	 * @param _hnd the CUDA memory handle
	 */

	bool initialize(CProjectionGeometry3D* _pGeometry, astraCUDA3d::MemHandle3D _hnd);

	/** Which type is this class?
	 *
	 * @return DataType: PROJECTION
	 */
	virtual CFloat32Data3D::EDataType getType() const { return PROJECTION; }

	/** Get the volume geometry.
	 *
	 * @return pointer to volume geometry.
	 */
	CProjectionGeometry3D* getGeometry() const { ASTRA_ASSERT(m_bInitialized); return m_pGeometry; }

};

} // end namesProjection astra

#endif

#endif // _INC_ASTRA_FLOAT32PROJECTIONDATA3DGPU
