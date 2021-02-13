#pragma once

#include <Aka/Aka.h>

#include <mutex>

namespace aka {

template <typename T>
class ResourceManager
{
	using map = std::map<std::string, T>;
public:
	static T& create(const std::string& str, T&& data);
	static bool has(const std::string& str);
	static T& get(const std::string& str);
	static T& getDefault();
	static void destroy(const std::string& str);
	static size_t count() { return m_data.size(); }

	struct Iterator
	{
		using map = std::map<std::string, T>;
		typename map::iterator begin() { return ResourceManager<T>::m_data.begin(); }
		typename map::iterator end() { return ResourceManager<T>::m_data.end(); }
		typename map::const_iterator begin()const { return ResourceManager<T>::m_data.begin(); }
		typename map::const_iterator end() const { return ResourceManager<T>::m_data.end(); }
	};
	static Iterator iterator;
private:
	static std::mutex m_lock;
	static map m_data;
};

template <typename T>
std::mutex ResourceManager<T>::m_lock;
template <typename T>
std::map<std::string, T> ResourceManager<T>::m_data;
template <typename T>
typename ResourceManager<T>::Iterator ResourceManager<T>::iterator;

using FontManager = ResourceManager<Font>;
using SpriteManager = ResourceManager<Sprite>;
using AudioManager = ResourceManager<AudioStream::Ptr>;

template <typename T>
T& ResourceManager<T>::create(const std::string& str, T&& data)
{
	std::lock_guard<std::mutex> m(m_lock);
	auto it = m_data.insert(std::make_pair(str, std::move(data)));
	if (it.second)
		return it.first->second;
	return it.first->second;
}

template<typename T>
inline bool ResourceManager<T>::has(const std::string& str)
{
	std::lock_guard<std::mutex> m(m_lock);
	auto it = m_data.find(str);
	return it != m_data.end();
}

template <typename T>
T& ResourceManager<T>::get(const std::string& str)
{
	std::lock_guard<std::mutex> m(m_lock);
	auto it = m_data.find(str);
	return it->second;
}

template<typename T>
T& ResourceManager<T>::getDefault()
{
	std::lock_guard<std::mutex> m(m_lock);
	return m_data.begin()->second;
}

template <typename T>
void ResourceManager<T>::destroy(const std::string& str)
{
	std::lock_guard<std::mutex> m(m_lock);
	auto it = m_data.find(str);
	if (it == m_data.end())
		return;
	m_data.erase(it);
}

};