#pragma once


// ACF includes
#include <iser/IObject.h>


namespace prolifedata
{


/**
	Interface for describing an Product.
	\ingroup Authentification
*/
class IProductInfo: virtual public iser::IObject
{
public:

	enum MetaInfoTypes
	{

		/**
			Product name given as QString.
		*/
		MIT_PRODUCT_NAME,

		/**
			Product description given as QString.
		*/
		MIT_PRODUCT_DESCRIPTION,

		/**
			Product manufacturer given as QString.
		*/
		MIT_PRODUCT_MANUFACTURER
	};


	/**
		Get name of the Product.
	*/
	virtual QString GetProductName() const = 0;

	/**
		Set name of the Product.
	*/
	virtual void SetProductName(QString ProductName) = 0;

	/**
		Get description of the Product.
	*/
	virtual QString GetProductDescription() const = 0;

	/**
		Set description of the Product.
	*/
	virtual void SetProductDescription(QString ProductDescription) = 0;

	/**
		Get manufacturer of the Product.
	*/
	virtual QString GetProductManufacturer() const = 0;

	/**
		Set manufacturer of the Product.
	*/
	virtual void SetProductManufacturer(QString ProductManufacturer) = 0;
};


} // namespace prolifedata


