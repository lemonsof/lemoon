/*!
 * \file nocopyable.hpp
 * \date 2014/04/29 9:48
 *
 * \author yayan
 * Contact: user@company.com
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note
*/
#ifndef LEMON_NOCOPYABLE_HPP
#define LEMON_NOCOPYABLE_HPP


namespace lemoon{
    class nocopyable
    {
    public:
        nocopyable(){}
        ~nocopyable(){}

        nocopyable(const nocopyable&) = delete;
        nocopyable(const nocopyable&&) = delete;
        nocopyable & operator = (const nocopyable&) = delete;
    };
}

#endif // LEMON_NOCOPYABLE_HPP