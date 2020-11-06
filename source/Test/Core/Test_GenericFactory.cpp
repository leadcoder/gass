#include "Core/Math/GASSVector.h"
#include "Core/Utils/GASSGenericFactory.h"
#include "catch.hpp"

class FactoryProduct
{
public:
	virtual ~FactoryProduct(){}
};
typedef GASS_SHARED_PTR<FactoryProduct> FactoryProductPtr;

class Product1 : public FactoryProduct
{
public:
};
typedef GASS_SHARED_PTR<Product1> Product1Ptr;

class Product2 : public FactoryProduct
{
public:
};
typedef GASS_SHARED_PTR<Product2> Product2Ptr;

TEST_CASE("Test Generic Factory")
{
	GASS::GenericFactory<std::string, FactoryProductPtr> factory;
	factory.Register<Product1>("Product1");
	factory.Register<Product2>("Product2");
	FactoryProductPtr p1 = factory.Create("Product1");
	REQUIRE(p1 != nullptr);
	REQUIRE(GASS_DYNAMIC_PTR_CAST<Product1>(p1) != nullptr);

	FactoryProductPtr p2 = factory.Create("Product2");
	REQUIRE(p2 != nullptr);
	REQUIRE(GASS_DYNAMIC_PTR_CAST<Product2>(p2) != nullptr);

	bool status = factory.Deregister("Product2");
	REQUIRE(status == true);

	status = factory.IsCreatable("Product2");
	REQUIRE(status == false);

	std::vector<std::string> keys = factory.GetAllKeys();

	REQUIRE(keys.size() == 1);

	REQUIRE(keys[0] == "Product1");
}