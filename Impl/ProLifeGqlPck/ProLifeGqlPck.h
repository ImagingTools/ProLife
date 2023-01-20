#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <prolifegql/CProductControllerComp.h>
#include <prolifegql/CProductCollectionControllerComp.h>
#include <prolifegql/COrderControllerComp.h>
#include <prolifegql/COrderCollectionControllerComp.h>




/**
	ProLifeLicenseGqlPck package
*/
namespace ProLifeGqlPck
{

typedef prolifegql::CProductControllerComp ProductController;
typedef prolifegql::CProductCollectionControllerComp ProductCollectionController;
typedef prolifegql::COrderControllerComp OrderController;
typedef prolifegql::COrderCollectionControllerComp OrderCollectionController;


} // namespace ImtLicenseGqlPck


