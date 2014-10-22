//
//  TestPondElement.h
//  SpringMeshGpu
//
//  Created by Ryan Bartley on 10/20/14.
//
//

#pragma once

#include "PondElement.h"

namespace heartbeat {
	
using PrimitivePondElementRef = std::shared_ptr<class PrimitivePondElement>;
	
class PrimitivePondElement : public PondElement {
public:
	
	static PrimitivePondElementRef create();
	
	void update() override;
	
private:
	PrimitivePondElement();
	
	void initialize( const ci::JsonTree &root );
	
};
	
}
