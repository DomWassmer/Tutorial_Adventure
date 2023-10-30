// TODO: Rename this file if I find something better

// Use these definitions for the ressources as "wrappers" of types in case I want to change the infrastructure in the future

#include <memory>

namespace TA {
	
	template<typename T> using SharedRes = std::shared_ptr<T>;

}
