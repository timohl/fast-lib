#ifndef SERIALIZABLE_HPP
#define SERIALIZABLE_HPP

#include <yaml-cpp/yaml.h>

#include <string>
#include <type_traits>


namespace fast
{
	class Serializable
	{
//		friend struct convert<Serializable>;
	public:
		virtual YAML::Node emit() const = 0;
		virtual void load(const YAML::Node &node) = 0;

		virtual std::string to_string() const;
		virtual void from_string(const std::string &str);
	};
};

namespace YAML
{
	template<> struct convert<fast::Serializable>
	{
		static Node encode(const fast::Serializable &rhs)
		{
			return rhs.emit();
		}
		static bool decode(const Node &node, fast::Serializable &rhs)
		{
			try {
				rhs.load(node);
			} catch(...) {
				return false;
			}
			return true;
		}
	};

	template<class T> struct convert<std::shared_ptr<T>>
	{
		static Node encode(const std::shared_ptr<T> &rhs)
		{
			return convert<T>::encode(*rhs);
		}
		static bool decode(const Node &node, std::shared_ptr<T> &rhs)
		{
			rhs = std::make_shared<T>();
			return convert<T>::decode(node, *rhs);
		}
	};
};

/// \todo Add template parameter to convert struct in yaml-cpp to be able to use std::enable_if.
#define YAML_CONVERT_IMPL(CLASS) \
	namespace YAML\
	{\
		template<> struct convert< CLASS >\
		{\
			static_assert(std::is_base_of<fast::Serializable, CLASS >::value, "CLASS is not derived from fast::Serializable");\
			static Node encode(const CLASS &rhs)\
			{\
				return convert<fast::Serializable>::encode(rhs);\
			}\
			static bool decode(const Node &node, CLASS &rhs)\
			{\
				return convert<fast::Serializable>::decode(node, rhs);\
			}\
		};\
	};
#endif