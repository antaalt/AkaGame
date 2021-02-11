#pragma once

#include <Aka/Aka.h>

namespace aka {

template <typename T>
class ResourceManager
{
	using map = std::map<std::string, T>;
public:
	T& create(const std::string& str, T&& data);
	T* get(const std::string& str);
	T* getDefault();
	void destroy(const std::string& str);
	size_t count() const { return m_data.size(); }

	typename map::iterator begin() { return m_data.begin(); }
	typename map::iterator end() { return m_data.end(); }
	typename map::const_iterator begin()const { return m_data.begin(); }
	typename map::const_iterator end() const { return m_data.end(); }
private:
	map m_data;
};

using FontManager = ResourceManager<Font>;
using SpriteManager = ResourceManager<Sprite>;

struct Resources
{
	static FontManager font;
	static SpriteManager sprite;
};

template <typename T>
T& ResourceManager<T>::create(const std::string& str, T&& data)
{
	auto it = m_data.insert(std::make_pair(str, std::move(data)));
	if (it.second)
		return it.first->second;
	return it.first->second;
}

template <typename T>
T* ResourceManager<T>::get(const std::string& str)
{
	auto it = m_data.find(str);
	if (it == m_data.end())
		return nullptr;
	return &it->second;
}

template<typename T>
T* ResourceManager<T>::getDefault()
{
	return &m_data.begin()->second;
}

template <typename T>
void ResourceManager<T>::destroy(const std::string& str)
{
	auto it = m_data.find(str);
	if (it == m_data.end())
		return;
	m_data.erase(it);
}

};