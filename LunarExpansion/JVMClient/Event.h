#pragma once

#include <functional>

template<class... T>
class EventCallback
{
private:
	using Event_ = std::function<void(T...)>;

	std::vector<Event_> m_callbacks;
public:
	constexpr size_t getFunctionCount() const { return m_callbacks.size(); };

	Event_ operator[](int Index)
	{
		return m_callbacks.at(Index);
	}

	void operator()(T... params)
	{
		for (Event_& eventCur : m_callbacks) {
			if (eventCur)
				eventCur(params...); // Correct expansion
		}
	}

	void operator+=(Event_ event_)
	{
		m_callbacks.push_back(event_);
	}


	void operator-=(Event_ event_)
	{
		for (Event_ it = m_callbacks.begin(); it != m_callbacks.end();)
		{
			Event_ Value_ = *it;

			if (event_ == Value_)
			{
				it = m_callbacks.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
};